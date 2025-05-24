#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "teax.h"

#include "def.h"
#include "vec.h"
#include "m_source.h"

#define CLASS_NAME SOURCE_CLASS_NAME

#define FX_BITS (12)
#define FX_UNIT (1 << FX_BITS)
#define FX_MASK (FX_UNIT - 1)
#define FX_LERP(a, b, p) ((a) + ((((b) - (a)) * (p)) >> (FX_BITS)))

static int samplerate = 44100;
static Source* master;
static int masterRef = TEA_NOREF;
static vec_t(Source*) sources;
static tea_State* teaState;

/* Sources can have assigned different streams, each stream has its own onEvent
 * function to handle the initing, deiniting, processing and rewinding of a
 * stream. A source works with a stream through the use of a stereo ring buffer
 * (Source.rawBufLeft, Source.rawBufRight).  When the source requires a sample
 * which the stream has not created yet it emits the STREAM_EVENT_PROCESS with
 * an offset and length into the ring buffer, the stream fills this buffer with
 * stereo 16bit audio for the given length and offset. The raw PCM is then
 * linearly interpolated and written to the Source's main output buffer
 * (Source.buf) at the Source's given playback rate (Source.rate). The Lua
 * callback is run (if it exists), gains are applied, then the Source writes
 * its output to its destination.
 *
 * In a STREAM_EVENT_PROCESS event, when a stream reaches the end of its data
 * before it has filled the requested amount of the raw PCM buffer, it loops
 * back around to the beginning of the file and continue filling the buffer.
 * This allows the Source to continue playing beyond the length of the stream's
 * data (Source.length) if it wants to loop, in the case that it doesn't it
 * will simply stop once it reaches the end (Source.end).
 *
 * `Source` structures are not directly stored as a lua udata, but rather a
 * pointer to a Source is stored as a udata so that a Source can continue to
 * exist after being GCed until it can be safely destroyed in the audio thread.
 *
 *  Raw PCM ring buffer
 *  -------------------
 *
 *                                  +-- Current playhead position
 *                                  |              (Source.position)
 *                          +-------|----------+
 *                          | ......>ooooooooo |
 *                          | . +----------+ o |
 *         Past samples ----- . |          | o --- Future samples
 *                          | . +----------+ o |
 *                          | ............oooo |
 *                          +------------|-----+
 *                               |       |
 *        Raw PCM ring buffers --+       +-- End of future samples
 *                (Source.rawBufLeft,                 (Source.bufEnd)
 *                 Source.rawBufRight)
 *
 *
 *  Process chain
 *  -------------
 *
 *    +------------------------+  +----------+  +-------+  +-------------+
 *    | Write interpolated raw |->| Do lua   |->| Apply |->| Write to    |
 *    | PCM to Source.buf      |  | callback |  | gains |  | destination |
 *    +------------------------+  +----------+  +-------+  +-------------+
 *          |            ^
 *          v            |
 *     +----------------------+
 *     | Process samples from |
 *     | stream to raw buffer |
 *     +----------------------+
 *
 *
 *  Commands
 *  --------
 *
 *  All commands issued by lua are pushed to the command vector (`commands`).
 *
 *  In the audio thread's callback each command is processed via
 *  `source_processCommand()` which also clears the vector. Each Source's audio
 *  is then processed via `source_ProcessAllSources()` (see [Process Chain]
 *  above)
 *
 */

typedef struct
{
    int type;
    Source *source;
    int i;
    double f;
    void *p;
} Command;

enum
{
    COMMAND_NULL,
    COMMAND_ADD,
    COMMAND_DESTROY,
    COMMAND_PLAY,
    COMMAND_PAUSE,
    COMMAND_STOP,
    COMMAND_SET_DESTINATION,
    COMMAND_SET_GAIN,
    COMMAND_SET_PAN,
    COMMAND_SET_RATE,
    COMMAND_SET_LOOP
};

static vec_t(Command) commands;

static Command command(int type, Source* source)
{
    Command c;
    memset(&c, 0, sizeof(c));
    c.type = type;
    c.source = source;
    return c;
}

static void pushCommand(Command* c)
{
    vec_push(&commands, *c);
}

static SourceEvent event(int type)
{
    SourceEvent e;
    memset(&e, 0, sizeof(e));
    e.type = type;
    return e;
}

static void emitEvent(Source* s, SourceEvent* e)
{
    if(s->onEvent)
    {
        s->onEvent(s, e);
    }
}

static void recalcGains(Source* self)
{
    double left, right;
    double pan = CLAMP(self->pan, -1.0, 1.0);
    double gain = MAX(self->gain, 0.);
    /* Get linear gains */
    left = ((pan < 0.0) ? 1.0 : (1.0 - pan)) * gain;
    right = ((pan > 0.0) ? 1.0 : (1.0 + pan)) * gain;
    /* Apply curve */
    left = left * left;
    right = right * right;
    /* Set fixedpoint gains */
    self->lgain = left * FX_UNIT;
    self->rgain = right * FX_UNIT;
}

static Source* checkSource(tea_State* T, int idx)
{
    Source** p = tea_check_udata(T, idx, CLASS_NAME);
    return *p;
}

static void source_free(void* ud)
{
    Source* self = (Source*)ud;
    Command c = command(COMMAND_DESTROY, self);
    pushCommand(&c);
}

static Source* newSource(tea_State* T)
{
    Source* self = malloc(sizeof(*self));
    memset(self, 0, sizeof(*self));
    self->dataRef = TEA_NOREF;
    self->destRef = TEA_NOREF;
    self->gain = 1.0;
    self->pan = 0.0;
    recalcGains(self);
    /* Init tea pointer to the actual Source struct */
    Source** p = tea_new_udata(T, sizeof(Source*), CLASS_NAME);
    tea_set_finalizer(T, source_free);
    *p = self;
    return self;
}

static void destroySource(Source* self)
{
    /* Note: All the lua references of the source should be unreferenced before
     * the source is destroyed. The source should also be removed from the
     * `sources` vector -- this is done in `source_processCommands()`, the only
     * place this function should ever be called. */
    SourceEvent e = event(SOURCE_EVENT_DEINIT);
    emitEvent(self, &e);
    free(self);
}

static double getBaseRate(Source* self)
{
    return (double)self->samplerate / (double)samplerate;
}

static void rewindStream(Source* self, long long position)
{
    /* Rewind stream */
    SourceEvent e = event(SOURCE_EVENT_REWIND);
    emitEvent(self, &e);
    /* Process first chunk and reset */
    e = event(SOURCE_EVENT_PROCESS);
    e.offset = 0;
    e.len = SOURCE_BUFFER_MAX / 2;
    emitEvent(self, &e);
    self->end = self->length;
    self->bufEnd = e.len;
    self->position = position;
}

static void onEventWav(Source* s, SourceEvent* e)
{
    switch (e->type)
    {
    case SOURCE_EVENT_INIT:
    {
        int err = wav_read(&s->wav, s->data->data, s->data->len);
        if(err != WAV_ESUCCESS)
        {
            tea_error(e->teaState,
                        "could not init wav stream: %s", wav_strerror(err));
        }
        if(s->wav.bitdepth != 16)
        {
            tea_error(e->teaState,
                       "could not init wav stream, expected 16bit wave");
        }
        if(s->wav.channels != 1 && s->wav.channels != 2)
        {
            tea_error(e->teaState,
                       "could not init wav stream, expected mono/stereo wave");
        }
        s->length = s->wav.length;
        s->samplerate = s->wav.samplerate;
        break;
    }
    case SOURCE_EVENT_REWIND:
        s->wavIdx = 0;
        break;
    case SOURCE_EVENT_PROCESS:
    {
        int i, x;
        for(i = 0; i < e->len; i++)
        {
            /* Hit the end? Rewind and continue */
            if(s->wavIdx >= s->length)
            {
                s->wavIdx = 0;
            }
            /* Process */
            int idx = (e->offset + i) & SOURCE_BUFFER_MASK;
            if(s->wav.channels == 2)
            {
                /* Process stereo */
                x = s->wavIdx << 1;
                s->rawBufLeft[idx] = ((short *)s->wav.data)[x];
                s->rawBufRight[idx] = ((short *)s->wav.data)[x + 1];
            }
            else
            {
                /* Process mono */
                s->rawBufLeft[idx] =
                    s->rawBufRight[idx] = ((short *)s->wav.data)[s->wavIdx];
            }
            s->wavIdx++;
        }
        break;
    }
    }
}

static void onEventOgg(Source* s, SourceEvent* e)
{
    switch(e->type)
    {
    case SOURCE_EVENT_INIT:
    {
        int err;
        s->oggStream = stb_vorbis_open_memory(s->data->data, s->data->len,
                                              &err, NULL);
        if (!s->oggStream)
        {
            tea_error(e->teaState, "could not init ogg stream; bad data?");
        }
        stb_vorbis_info info = stb_vorbis_get_info(s->oggStream);
        s->samplerate = info.sample_rate;
        s->length = stb_vorbis_stream_length_in_samples(s->oggStream);
        break;
    }
    case SOURCE_EVENT_DEINIT:
        if(s->oggStream)
        {
            stb_vorbis_close(s->oggStream);
        }
        break;
    case SOURCE_EVENT_REWIND:
        stb_vorbis_seek_start(s->oggStream);
        break;
    case SOURCE_EVENT_PROCESS:
    {
        int i, n;
        short buf[SOURCE_BUFFER_MAX];
        int len = e->len * 2;
        int z = e->offset;
    fill:
        n = stb_vorbis_get_samples_short_interleaved(s->oggStream, 2, buf, len);
        n *= 2;
        for(i = 0; i < n; i += 2)
        {
            int idx = z++ & SOURCE_BUFFER_MASK;
            s->rawBufLeft[idx] = buf[i];
            s->rawBufRight[idx] = buf[i + 1];
        }
        /* Reached end of stream before the end of the buffer? rewind and fill
         * remaining buffer */
        if(len != n)
        {
            stb_vorbis_seek_start(s->oggStream);
            len -= n;
            goto fill;
        }
        break;
    }
    }
}

Source* source_getMaster(int* ref)
{
    if(ref)
    {
        *ref = masterRef;
    }
    return master;
}

void source_setSamplerate(int sr)
{
    samplerate = sr;
}

void source_processCommands(void)
{
    int i;
    Command* c;
    vec_t(int) oldRefs;
    vec_init(&oldRefs);
    /* Handle commands */
    vec_foreach_ptr(&commands, c, i)
    {
        switch(c->type)
        {
        case COMMAND_ADD:
            vec_push(&sources, c->source);
            break;
        case COMMAND_DESTROY:
            vec_push(&oldRefs, c->source->dataRef);
            vec_push(&oldRefs, c->source->destRef);
            vec_remove(&sources, c->source);
            destroySource(c->source);
            break;
        case COMMAND_PLAY:
            if (c->i || c->source->state == SOURCE_STATE_STOPPED)
            {
                rewindStream(c->source, 0);
            }
            c->source->state = SOURCE_STATE_PLAYING;
            break;
        case COMMAND_PAUSE:
            if (c->source->state == SOURCE_STATE_PLAYING)
            {
                c->source->state = SOURCE_STATE_PAUSED;
            }
            break;
        case COMMAND_STOP:
            c->source->state = SOURCE_STATE_STOPPED;
            break;
        case COMMAND_SET_DESTINATION:
            vec_push(&oldRefs, c->source->destRef);
            c->source->destRef = c->i;
            c->source->dest = c->p;
            break;
        case COMMAND_SET_GAIN:
            c->source->gain = c->f;
            recalcGains(c->source);
            break;
        case COMMAND_SET_PAN:
            c->source->pan = c->f;
            recalcGains(c->source);
            break;
        case COMMAND_SET_RATE:
            c->source->rate = getBaseRate(c->source) * c->f * FX_UNIT;
            break;
        case COMMAND_SET_LOOP:
            if (c->i)
            {
                c->source->flags |= SOURCE_FLOOP;
            }
            else
            {
                c->source->flags &= ~SOURCE_FLOOP;
            }
            break;
        }
    }
    /* Clear command vector */
    vec_clear(&commands);
    /* Remove old Lua references */
    if(oldRefs.length > 0)
    {
        int i, ref;
        vec_foreach(&oldRefs, ref, i)
        {
            teax_unref(teaState, TEA_REGISTRY_INDEX, ref);
        }
    }
    vec_deinit(&oldRefs);
}

void source_process(Source* self, int len)
{
    int i;
    /* Replace flag still set? Zeroset the buffer */
    if(self->flags & SOURCE_FREPLACE)
    {
        memset(self->buf, 0, sizeof(*self->buf) * len);
    }
    /* Process audio stream and add to our buffer */
    if(self->state == SOURCE_STATE_PLAYING && self->onEvent)
    {
        for(i = 0; i < len; i += 2)
        {
            int idx = (self->position >> FX_BITS);
            /* Process the stream and fill the raw buffer if the next index requires
             * samples we don't yet have */
            if(idx + 1 >= self->bufEnd)
            {
                SourceEvent e = event(SOURCE_EVENT_PROCESS);
                e.offset = (self->bufEnd) & SOURCE_BUFFER_MASK;
                e.len = SOURCE_BUFFER_MAX / 2;
                emitEvent(self, &e);
                self->bufEnd += e.len;
            }
            /* Have we reached the end? */
            if(idx >= self->end)
            {
                /* Not set to loop? Stop and stop processing */
                if(~self->flags & SOURCE_FLOOP)
                {
                    self->state = SOURCE_STATE_STOPPED;
                    break;
                }
                /* Set to loop: Streams always fill the raw buffer in a loop, so we
                 * just increment the end index by the stream's length so that it
                 * continues for another iteration of the sound file */
                self->end = idx + self->length;
            }
            /* Write interpolated frame to buffer */
            int p = self->position & FX_MASK;
            /* Left */
            int la = self->rawBufLeft[idx & SOURCE_BUFFER_MASK];
            int lb = self->rawBufLeft[(idx + 1) & SOURCE_BUFFER_MASK];
            self->buf[i] += FX_LERP(la, lb, p);
            /* Right */
            int ra = self->rawBufRight[idx & SOURCE_BUFFER_MASK];
            int rb = self->rawBufRight[(idx + 1) & SOURCE_BUFFER_MASK];
            self->buf[i + 1] += FX_LERP(ra, rb, p);
            /* Increment position */
            self->position += self->rate;
        }
    }
    /* Apply gains */
    for(i = 0; i < len; i += 2)
    {
        self->buf[i] = (self->buf[i] * self->lgain) >> FX_BITS;
        self->buf[i + 1] = (self->buf[i + 1] * self->rgain) >> FX_BITS;
    }
    /* Write to destination */
    if(self->dest)
    {
        if(self->dest->flags & SOURCE_FREPLACE)
        {
            memcpy(self->dest->buf, self->buf, sizeof(*self->buf) * len);
            self->dest->flags &= ~SOURCE_FREPLACE;
        }
        else
        {
            for(i = 0; i < len; i++)
            {
                self->dest->buf[i] += self->buf[i];
            }
        }
    }
    /* Reset our flag as to replace the buffer's content */
    self->flags |= SOURCE_FREPLACE;
}

void source_processAllSources(int len)
{
    int i;
    Source *s;
    /* Sources are processed in reverse (newer Sources are processed first) --
     * this assures the master is processed last */
    vec_foreach_rev(&sources, s, i)
    {
        source_process(s, len);
    }
}

static void source_fromData(tea_State* T)
{
    Data* data = tea_check_udata(T, 0, DATA_CLASS_NAME);
    Source* self = newSource(T);
    /* Init data reference */
    self->data = data;
    tea_push_value(T, 0);
    self->dataRef = teax_ref(T, TEA_REGISTRY_INDEX);
    /* Detect format and set appropriate event handler */
    /* Is .wav? */
    if(data->len > 12 && !memcmp(((char*)data->data) + 8, "WAVE", 4))
    {
        self->onEvent = onEventWav;
        goto init;
    }
    /* Is .ogg? */
    if(data->len > 4 && !memcmp(data->data, "OggS", 4))
    {
        self->onEvent = onEventOgg;
        goto init;
    }
    /* Made it here? Error out because we couldn't detect the format */
    tea_error(T, "could not init Source; bad Data format?");
    /* Init stream */
init:;
    SourceEvent e = event(SOURCE_EVENT_INIT);
    e.teaState = T;
    emitEvent(self, &e);
    /* Init */
    self->rate = getBaseRate(self) * FX_UNIT;
    self->dest = master;
    /* Issue "add" command to push to `sources` vector */
    Command c = command(COMMAND_ADD, self);
    pushCommand(&c);
}

static void source_fromBlank(tea_State* T)
{
    Source* self = newSource(T);
    /* Init */
    self->dest = master;
    /* Issue "add" command to push to `sources` vector */
    Command c = command(COMMAND_ADD, self);
    pushCommand(&c);
}

static void source_getLength(tea_State* T)
{
    Source* self = checkSource(T, 0);
    tea_push_number(T, getBaseRate(self) * self->length / self->samplerate);
}

static void source_getState(tea_State* T)
{
    Source* self = checkSource(T, 0);
    switch(self->state)
    {
    case SOURCE_STATE_PLAYING:
        tea_push_literal(T, "playing");
        break;
    case SOURCE_STATE_PAUSED:
        tea_push_literal(T, "paused");
        break;
    case SOURCE_STATE_STOPPED:
        tea_push_literal(T, "stopped");
        break;
    default:
        tea_push_literal(T, "?");
        break;
    }
}

static void source_setDestination(tea_State* T)
{
    Source* self = checkSource(T, 0);
    Source* dest = master;
    if(!tea_is_nonenil(T, 1))
    {
        dest = checkSource(T, 1);
    }
    /* Master? Disallow */
    if(self == master)
    {
        tea_arg_error(T, 1, "master cannot be rerouted");
    }
    /* Check for feedback loop */
    Source* s = dest;
    while(s)
    {
        if(s == self)
        {
            tea_error(T, "routing results in a feedback loop");
        }
        s = s->dest;
    }
    /* Do routing */
    Command c = command(COMMAND_SET_DESTINATION, self);
    if(!tea_is_nonenil(T, 1))
    {
        tea_push_value(T, 1);
    }
    else
    {
        tea_push_integer(T, masterRef);
        tea_get_field(T, TEA_REGISTRY_INDEX);
    }
    c.i = teax_ref(T, TEA_REGISTRY_INDEX);
    c.p = dest;
    pushCommand(&c);
}

static void source_setGain(tea_State* T)
{
    Source* self = checkSource(T, 0);
    double gain = tea_opt_number(T, 1, 1);
    Command c = command(COMMAND_SET_GAIN, self);
    c.f = gain;
    pushCommand(&c);
}

static void source_setPan(tea_State* T)
{
    Source* self = checkSource(T, 0);
    double pan = tea_opt_number(T, 1, 0);
    Command c = command(COMMAND_SET_PAN, self);
    c.f = pan;
    pushCommand(&c);
}

static void source_setRate(tea_State* T)
{
    Source* self = checkSource(T, 0);
    double rate = tea_opt_number(T, 1, 1);
    if(rate < 0)
    {
        tea_arg_error(T, 2, "expected value of zero or greater");
    }
    if(rate > 16)
    {
        tea_arg_error(T, 2, "value is too large");
    }
    Command c = command(COMMAND_SET_RATE, self);
    c.f = rate;
    pushCommand(&c);
}

static void source_setLoop(tea_State* T)
{
    Source* self = checkSource(T, 0);
    int loop = tea_opt_bool(T, 1, 0);
    Command c = command(COMMAND_SET_LOOP, self);
    c.i = loop;
    pushCommand(&c);
}

static void source_play(tea_State* T)
{
    Source* self = checkSource(T, 0);
    int reset = tea_opt_bool(T, 1, 0);
    Command c = command(COMMAND_PLAY, self);
    c.i = reset;
    pushCommand(&c);
}

static void source_pause(tea_State* T)
{
    Source* self = checkSource(T, 0);
    Command c = command(COMMAND_PAUSE, self);
    pushCommand(&c);
}

static void source_stop(tea_State* T)
{
    Source *self = checkSource(T, 0);
    Command c = command(COMMAND_STOP, self);
    pushCommand(&c);
}

static const tea_Methods reg[] = {
    { "fromData", "static",  source_fromData, 1, 0 },
    { "fromBlank", "static", source_fromBlank, 1, 0 },
    { "getLength", "method", source_getLength, 1, 0 },
    { "getState", "method", source_getState, 1, 0 },
    { "setDestination", "method", source_setDestination, 1, 1 },
    { "setGain", "method", source_setGain, 1, 1 },
    { "setPan", "method", source_setPan, 1, 1 },
    { "setRate", "method", source_setRate, 1, 1 },
    { "setLoop", "method", source_setLoop, 1, 1 },
    { "play", "method", source_play, 1, 1 },
    { "pause", "method", source_pause, 1, 0 },
    { "stop", "method", source_stop, 1, 0 },
    { NULL, NULL }
};

void micro_open_source(tea_State* T)
{
    tea_create_class(T, "Source", reg);
    tea_push_value(T, -1);
    tea_set_key(T, TEA_REGISTRY_INDEX, CLASS_NAME);
    /* Set tea state */
    teaState = T;
    /* Init master */
    master = newSource(T);
    masterRef = teax_ref(T, TEA_REGISTRY_INDEX);
    Command c = command(COMMAND_ADD, master);
    pushCommand(&c);
}