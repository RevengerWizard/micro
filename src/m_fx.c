#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "def.h"
#include "sera/sera.h"
#include "m_image.h"

#define LERP(bits, a, b, p) ((a) + ((((b) - (a)) * (p)) >> (bits)))

#define FX_BITS 10
#define FX_UNIT (1 << FX_BITS)
#define FX_MASK (FX_UNIT - 1)

static int tablesInited = 0;
static int tableSin[FX_UNIT];

static void initTables(void)
{
    if(tablesInited)
        return;
    /* Make sin table */
    int i;
    for(i = 0; i < FX_UNIT; i++)
    {
        tableSin[i] = sin((i / (float)FX_UNIT) * 6.283185) * FX_UNIT;
    }
    /* Done */
    tablesInited = 1;
}

static int fxsin(int n)
{
    return tableSin[n & FX_MASK];
}

static sr_Pixel getColorFromTable(tea_State* T, int idx)
{
    if(tea_is_nonenil(T, idx) || tea_get_type(T, idx) != TEA_TYPE_LIST)
    {
        tea_error(T, "expected list");
    }
    sr_Pixel px;
    int i;
    float v;
    for(i = 0; i < 4; i++)
    {
        tea_get_item(T, idx, i + 1);
        v = tea_to_number(T, -1);
        v = CLAMP(v, 0, 1) * 255;
        switch(i)
        {
            case 0:
                px.rgba.r = v;
                break;
            case 1:
                px.rgba.g = v;
                break;
            case 2:
                px.rgba.b = v;
                break;
            case 3:
                px.rgba.a = v;
                break;
        }
        tea_pop(T, 1);
    }
    return px;
}

static void checkBufferSizesMatch(tea_State* T, Image* a, Image* b)
{
    if (a->buf->w != b->buf->w || a->buf->h != b->buf->h)
    {
        tea_error(T, "expected buffer sizes to match");
    }
}

static void fx_desaturate(tea_State* T)
{
    Image* self = tea_check_udata(T, 0, IMAGE_CLASS_NAME);
    int amount = tea_opt_integer(T, 1, 1) * 0xff;
    amount = CLAMP(amount, 0, 0xff);
    int i = self->buf->w * self->buf->h;
    sr_Pixel* p = self->buf->pixels;
    if(amount >= 0xfe)
    {
        /* Full amount? Don't bother with lerping, just write pixel avg */
        while(i--)
        {
            p->rgba.r = p->rgba.g = p->rgba.b =
                ((p->rgba.r + p->rgba.g + p->rgba.b) * 341) >> 10;
            p++;
        }
    }
    else
    {
        while(i--)
        {
            int avg = ((p->rgba.r + p->rgba.g + p->rgba.b) * 341) >> 10;
            p->rgba.r = LERP(8, p->rgba.r, avg, amount);
            p->rgba.g = LERP(8, p->rgba.g, avg, amount);
            p->rgba.b = LERP(8, p->rgba.b, avg, amount);
            p++;
        }
    }
    tea_push_nil(T);
}

static void fx_mask(tea_State* T)
{
    Image* self = tea_check_udata(T, 0, IMAGE_CLASS_NAME);
    Image* mask = tea_check_udata(T, 1, IMAGE_CLASS_NAME);
    const char* channel = tea_opt_string(T, 2, "a");
    checkBufferSizesMatch(T, self, mask);
    if(!strchr("rgba", *channel))
    {
        tea_error(T, "expected channel to be 'r', 'g', 'b' or 'a'");
    }
    int i = self->buf->w * self->buf->h;
    sr_Pixel* d = self->buf->pixels;
    sr_Pixel* s = mask->buf->pixels;
    while(i--)
    {
        switch(*channel)
        {
            case 'r':
                d->rgba.a = (d->rgba.a * s->rgba.r) >> 8;
                break;
            case 'g':
                d->rgba.a = (d->rgba.a * s->rgba.g) >> 8;
                break;
            case 'b':
                d->rgba.a = (d->rgba.a * s->rgba.b) >> 8;
                break;
            case 'a':
                d->rgba.a = (d->rgba.a * s->rgba.a) >> 8;
                break;
        }
        d++;
        s++;
    }
    tea_push_nil(T);
}

static void fx_palette(tea_State* T)
{
    Image* self = tea_check_udata(T, 0, IMAGE_CLASS_NAME);
    if(tea_is_nonenil(T, 1) || tea_get_type(T, 1) != TEA_TYPE_LIST)
    {
        tea_arg_error(T, 2, "expected list");
    }
    /* Load palette from table */
    sr_Pixel pal[256];
    int ncolors = tea_len(T, 1);
    if(ncolors == 0)
    {
        tea_arg_error(T, 2, "expected non-empty list");
    }
    int i;
    for(i = 0; i < 256; i++)
    {
        tea_get_item(T, 1, ((i * ncolors) >> 8) + 1);
        pal[i] = getColorFromTable(T, -1);
        tea_pop(T, 1);
    }
    /* Convert each pixel to palette color based on its brightest channel */
    i = self->buf->w * self->buf->h;
    sr_Pixel *p = self->buf->pixels;
    while(i--)
    {
        int idx = MAX(MAX(p->rgba.r, p->rgba.b), p->rgba.g);
        p->rgba.r = pal[idx].rgba.r;
        p->rgba.g = pal[idx].rgba.g;
        p->rgba.b = pal[idx].rgba.b;
        p++;
    }
    tea_push_nil(T);
}

static unsigned long long xorshift64star(unsigned long long *x)
{
    *x ^= *x >> 12;
    *x ^= *x << 25;
    *x ^= *x >> 27;
    return *x * 2685821657736338717ULL;
}

static void fx_dissolve(tea_State* T)
{
    Image* self = tea_check_udata(T, 0, IMAGE_CLASS_NAME);
    unsigned long long s = 1ULL << 32;
    unsigned int amount;
    amount = tea_check_integer(T, 1) * 256;
    s |= (unsigned)(tea_opt_integer(T, 2, 0));
    amount = CLAMP(amount, 0, 0xff);
    int i = self->buf->w * self->buf->h;
    sr_Pixel* p = self->buf->pixels;
    while(i--)
    {
        if((xorshift64star(&s) & 0xff) < amount)
        {
            p->rgba.a = 0;
        }
        p++;
    }
    tea_push_nil(T);
}

static void fx_wave(tea_State* T)
{
    Image* self = tea_check_udata(T, 0, IMAGE_CLASS_NAME);
    Image* src = tea_check_udata(T, 1, IMAGE_CLASS_NAME);
    checkBufferSizesMatch(T, self, src);
    int amountX = tea_check_number(T, 2);
    int amountY = tea_check_number(T, 3);
    int scaleX = tea_check_number(T, 4) * FX_UNIT;
    int scaleY = tea_check_number(T, 5) * FX_UNIT;
    int offsetX = tea_opt_integer(T, 6, 0) * FX_UNIT;
    int offsetY = tea_opt_integer(T, 7, 0) * FX_UNIT;
    int x, y;
    for(y = 0; y < self->buf->h; y++)
    {
        sr_Pixel *d = self->buf->pixels + y * self->buf->w;
        int ox = (fxsin(offsetX + ((y * scaleX) >> FX_BITS)) * amountX) >> FX_BITS;
        for(x = 0; x < self->buf->w; x++)
        {
            int oy = (fxsin(offsetY + ((x * scaleY) >> FX_BITS)) * amountY) >> FX_BITS;
            *d = sr_getPixel(src->buf, x + ox, y + oy);
            d++;
        }
    }
    tea_push_nil(T);
}

static int getChannel(sr_Pixel px, char channel)
{
    switch(channel)
    {
        case 'r':
            return px.rgba.r;
        case 'g':
            return px.rgba.g;
        case 'b':
            return px.rgba.b;
        case 'a':
            return px.rgba.a;
    }
    return 0;
}

static void fx_displace(tea_State* T)
{
    Image* self = tea_check_udata(T, 0, IMAGE_CLASS_NAME);
    Image* src = tea_check_udata(T, 1, IMAGE_CLASS_NAME);
    Image* map = tea_check_udata(T, 2, IMAGE_CLASS_NAME);
    const char* channelX = tea_check_string(T, 3);
    const char* channelY = tea_check_string(T, 4);
    int scaleX = tea_check_integer(T, 5) * (1 << 7);
    int scaleY = tea_check_integer(T, 6) * (1 << 7);
    int x, y;
    checkBufferSizesMatch(T, self, src);
    checkBufferSizesMatch(T, self, map);
    if(!strchr("rgba", *channelX))
        tea_arg_error(T, 4, "bad channel");
    if(!strchr("rgba", *channelY))
        tea_arg_error(T, 5, "bad channel");
    for(y = 0; y < self->buf->h; y++)
    {
        sr_Pixel *d = self->buf->pixels + y * self->buf->w;
        sr_Pixel *m = map->buf->pixels + y * self->buf->w;
        for(x = 0; x < self->buf->w; x++)
        {
            int cx = ((getChannel(*m, *channelX) - (1 << 7)) * scaleX) >> 14;
            int cy = ((getChannel(*m, *channelY) - (1 << 7)) * scaleY) >> 14;
            *d = sr_getPixel(src->buf, x + cx, y + cy);
            d++;
            m++;
        }
    }
    tea_push_nil(T);
}

static void fx_blur(tea_State* T)
{
    Image* self = tea_check_udata(T, 0, IMAGE_CLASS_NAME);
    Image* src = tea_check_udata(T, 1, IMAGE_CLASS_NAME);
    int radiusx = tea_check_integer(T, 2);
    int radiusy = tea_check_integer(T, 3);
    int y, x, ky, kx;
    int r, g, b, r2, g2, b2;
    sr_Pixel p2;
    int w = src->buf->w;
    int h = src->buf->h;
    int dx = 256 / (radiusx * 2 + 1);
    int dy = 256 / (radiusy * 2 + 1);
    sr_Rect bounds = sr_rect(radiusx, radiusy, w - radiusx, h - radiusy);
    sr_Pixel* p = self->buf->pixels;
    checkBufferSizesMatch(T, self, src);
    /* do blur */
    for(y = 0; y < h; y++)
    {
        int inBoundsY = y >= bounds.y && y < bounds.h;
        for(x = 0; x < w; x++)
        {
            /* are the pixels that will be used in bounds? */
            int inBounds = inBoundsY && x >= bounds.x && x < bounds.w;
/* blur pixel */
#define GET_PIXEL_FAST(b, x, y) ((b)->pixels[(x) + (y) * w])
#define BLUR_PIXEL(GET_PIXEL) \
    r = 0, g = 0, b = 0; \
    for(ky = -radiusy; ky <= radiusy; ky++) \
    { \
        r2 = 0, g2 = 0, b2 = 0; \
        for(kx = -radiusx; kx <= radiusx; kx++) \
        { \
            p2 = GET_PIXEL(src->buf, x + kx, y + ky); \
            r2 += p2.rgba.r; \
            g2 += p2.rgba.g; \
            b2 += p2.rgba.b; \
        } \
        r += (r2 * dx) >> 8; \
        g += (g2 * dx) >> 8; \
        b += (b2 * dx) >> 8; \
    }
            if(inBounds)
            {
                BLUR_PIXEL(GET_PIXEL_FAST)
            }
            else
            {
                BLUR_PIXEL(sr_getPixel)
            }
            /* set pixel */
            p->rgba.r = (r * dy) >> 8;
            p->rgba.g = (g * dy) >> 8;
            p->rgba.b = (b * dy) >> 8;
            p->rgba.a = 0xff;
            p++;
        }
    }
    tea_push_nil(T);
}

static const tea_Reg reg[] = {
    { "desaturate", fx_desaturate, 1, 1 },
    { "palette", fx_palette, 2, 0 },
    { "dissolve", fx_dissolve, 2, 1 },
    { "mask", fx_mask, 2, 1 },
    { "wave", fx_wave, 6, 2 },
    { "displace", fx_displace, 7, 0 },
    { "blur", fx_blur, 4, 0 },
    { NULL, NULL }
};

void micro_open_fx(tea_State* T)
{
    tea_create_submodule(T, "fx", reg);
    initTables();
}