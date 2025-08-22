#include <stdint.h>

#include "micro.h"

#define MINIAUDIO_IMPLEMENTATION
#include <miniaudio.h>

#include <tea.h>

#include "def.h"
#include "m_source.h"

static bool inited = false;
static double samplerate = 0;

static void audio_callback(ma_device* pDevice, void* pOutput, const void* pInput, ma_uint32 frameCount)
{
    tea_State* T = (tea_State*)pDevice->pUserData;
    int16_t* buffer = (int16_t*)pOutput;
    int len = frameCount * pDevice->playback.channels;
    int i;
    UNUSED(T);

    /* Process source commands */
    source_processCommands();
    /* Process sources audio */
    source_processAllSources(len);
    /* Copy master to buffer */
    Source* master = source_getMaster(NULL);
    for(i = 0; i < len; i++)
    {
        int x = master->buf[i];
        buffer[i] = CLAMP(x, -32768, 32767);
    }

    UNUSED(pInput);
}

static void audio_init(tea_State* T)
{
    int rate = tea_opt_integer(T, 0, 44100);
    int bufferSize = tea_opt_integer(T, 1, 44100);
    UNUSED(bufferSize);
    if(inited)
    {
        tea_error(T, "audio is already inited");
    }
    
    ma_device_config config;
    config = ma_device_config_init(ma_device_type_playback);
    config.playback.format   = ma_format_s16;
    config.playback.channels = 2;
    config.sampleRate        = rate;
    config.dataCallback      = audio_callback;
    config.pUserData         = T;

    if(ma_device_init(NULL, &config, &device) != MA_SUCCESS)
    {
        tea_error(T, "could not init audio");
    }

    samplerate = rate;
    inited = true;
    source_setSamplerate(samplerate);

    if(ma_device_start(&device) != MA_SUCCESS)
    {
        tea_error(T, "could not start audio");
    }
}

static const tea_Reg reg[] = {
    { "init", audio_init, 0, 2 },
    { NULL }
};

void micro_open_audio(tea_State* T)
{
    tea_create_submodule(T, "audio", reg);
}