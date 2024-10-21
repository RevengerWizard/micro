#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define JO_GIF_HEADER_FILE_ONLY
#include "jo_gif.c"

#include <tea.h>
#include "def.h"
#include "m_image.h"

#define CLASS_NAME "micro.Gif"

typedef struct
{
    int state;
    jo_gif_t gif;
    int w, h;
    unsigned char* buf;
} Gif;

enum
{
    STATE_INIT,
    STATE_ACTIVE,
    STATE_CLOSED,
};

static void gif_free(void* ud)
{
    Gif* self = (Gif*)ud;
    free(self->buf);
    /* Not closed? close now */
    if(self->state == STATE_ACTIVE)
    {
        jo_gif_end(&self->gif);
    }
}

static void gif_new(tea_State* T)
{
    const char* filename = tea_check_string(T, 0);
    int w = tea_check_integer(T, 1);
    int h = tea_check_integer(T, 2);
    int ncolors = tea_opt_integer(T, 3, 63);
    Gif* self = tea_new_udata(T, sizeof(*self), CLASS_NAME);
    tea_set_finalizer(T, gif_free);
    memset(self, 0, sizeof(*self));
    self->state = STATE_INIT;
    self->w = w;
    self->h = h;
    self->buf = malloc(w * h * 4);
    memset(self->buf, 0, w * h * 4);
    /* Activate gif */
    self->gif = jo_gif_start(filename, self->w, self->h, 0, ncolors);
    self->state = STATE_ACTIVE;
}

static void gif_update(tea_State* T)
{
    Gif* self = tea_check_udata(T, 0, CLASS_NAME);
    Image* img = tea_check_udata(T, 1, IMAGE_CLASS_NAME);
    int delay = tea_check_integer(T, 2);
    /* Already closed? Error */
    if(self->state == STATE_CLOSED)
    {
        tea_error(T, "can't update closed gif");
    }
    /* Buffer dimensions are okay? */
    if(img->buf->w != self->w || img->buf->h != self->h)
    {
        tea_error(T, "bad buffer dimensions for gif object, expected %dx%d",
                   self->w, self->h);
    }
    /* Copy pixels to buffer -- jo_gif expects a specific channel byte-order
     * which may differ from what sera is using -- alpha channel isn't copied
     * since jo_gif doesn't use this */
    int i, n;
    int len = self->w * self->h;
    sr_Pixel* p = img->buf->pixels;
    for(i = 0; i < len; i++)
    {
        n = i * 4;
        self->buf[n] = p[i].rgba.r;
        self->buf[n + 1] = p[i].rgba.g;
        self->buf[n + 2] = p[i].rgba.b;
    }
    /* Update */
    jo_gif_frame(&self->gif, self->buf, delay, 0);
}

static void gif_close(tea_State* T)
{
    Gif* self = tea_check_udata(T, 0, CLASS_NAME);
    if(self->state == STATE_CLOSED)
    {
        tea_error(T, "state already closed");
    }
    self->state = STATE_CLOSED;
    jo_gif_end(&self->gif);
}

static const tea_Methods reg[] = {
    { "new", "static", gif_new, 3, 1 },
    { "update", "method", gif_update, 3, 0 },
    { "close", "method", gif_close, 0, 0 },
    { NULL, NULL }
};

void micro_open_gif(tea_State* T)
{
    tea_create_class(T, "Gif", reg);
    tea_push_value(T, -1);
    tea_set_key(T, TEA_REGISTRY_INDEX, CLASS_NAME);
}