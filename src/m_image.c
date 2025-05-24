#include <stdlib.h>

#include <tea.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "sera.h"
#include "fs.h"
#include "m_image.h"

#define CLASS_NAME  IMAGE_CLASS_NAME

sr_Pixel get_color(tea_State* T, int idx)
{
    int r = tea_opt_integer(T, idx, 255);
    int g = tea_opt_integer(T, idx + 1, 255);
    int b = tea_opt_integer(T, idx + 2, 255);
    int a = tea_opt_integer(T, idx + 3, 255);
    return sr_pixel(r, g, b, a);
}

sr_Rect get_rect(tea_State* T, int idx)
{
    tea_check_map(T, idx);
    tea_push_literal(T, "x");
    tea_get_index(T, idx);
    int x = tea_check_integer(T, -1);
    tea_push_literal(T, "y");
    tea_get_index(T, idx);
    int y = tea_check_integer(T, -1);
    tea_push_literal(T, "w");
    tea_get_index(T, idx);
    int w = tea_check_integer(T, -1);
    tea_push_literal(T, "h");
    tea_get_index(T, idx);
    int h = tea_check_integer(T, -1);
    tea_pop(T, 4);
    return sr_rect(x, y, w, h);
}

void check_subrect(tea_State* T, int idx, sr_Buffer* b, sr_Rect* r)
{
    if(r->x < 0 || r->y < 0 || r->x + r->w > b->w || r->y + r->h > b->h)
    {
        tea_arg_error(T, idx, "sub rectangle out of bounds");
    }
}

static void image_free(void* ud)
{
    Image* self = (Image*)ud;
    if(self->buf)
    {
        sr_destroyBuffer(self->buf);
    }
}

Image* micro_image_new(tea_State* T)
{
    Image* self = (Image*)tea_new_udata(T, sizeof(*self), CLASS_NAME);
    tea_set_finalizer(T, image_free);
    memset(self, 0, sizeof(*self));
    return self;
}

static int image_load(Image* self, const void* data, int len)
{
    int w, h;
    void* pixels = stbi_load_from_memory(data, len, &w, &h, NULL, STBI_rgb_alpha);
    if(!pixels)
    {
        return -1;
    }
    self->buf = sr_newBuffer(w, h);
    if(!self->buf)
    {
        free(pixels);
        return -1;
    }
    sr_loadPixels(self->buf, pixels, SR_FMT_RGBA);
    free(pixels);
    return 0;
}

static void image_fromFile(tea_State* T)
{
    const char* filename = tea_check_string(T, 0);
    Image* self = micro_image_new(T);
    size_t len;
    void* data = fs_read(filename, &len);
    if(!data)
    {
        tea_error(T, "could not open file '%s'", filename);
    }
    int err = image_load(self, data, len);
    free(data);
    if(err)
    {
        tea_error(T, "could not load buffer");
    }
}

static void image_fromString(tea_State* T)
{
    size_t len;
    const char* str = tea_check_lstring(T, 0, &len);
    Image* self = micro_image_new(T);
    int err = image_load(self, str, len);
    if(err)
    {
        tea_error(T, "could not load buffer");
    }
}

static void image_fromBlank(tea_State* T)
{
    int w = tea_check_integer(T, 0);
    int h = tea_check_integer(T, 1);
    if(w <= 0)
        tea_arg_error(T, 1, "expected width greater than 0");
    if(h <= 0)
        tea_arg_error(T, 2, "expected height greater than 0");
    Image* self = micro_image_new(T);
    self->buf = sr_newBuffer(w, h);
    sr_clear(self->buf, sr_pixel(0, 0, 0, 0));
    if(!self->buf)
    {
        tea_error(T, "could not create buffer");
    }
}

static void image_clone(tea_State* T)
{
    Image* self = (Image*)tea_check_udata(T, 0, CLASS_NAME);
    Image* clone = micro_image_new(T);
    clone->buf = sr_cloneBuffer(self->buf);
}

static void image_reset(tea_State* T)
{
    Image* self = (Image*)tea_check_udata(T, 0, CLASS_NAME);
    sr_reset(self->buf);
}

static void image_clear(tea_State* T)
{
    Image* self = (Image*)tea_check_udata(T, 0, CLASS_NAME);
    sr_clear(self->buf, get_color(T, 1));
}

static void image_setPixel(tea_State* T)
{
    Image* self = (Image*)tea_check_udata(T, 0, CLASS_NAME);
    int x = tea_check_integer(T, 1);
    int y = tea_check_integer(T, 2);
    sr_drawPixel(self->buf, get_color(T, 3), x, y);
}

static void image_getPixel(tea_State* T)
{
    Image* self = (Image*)tea_check_udata(T, 0, CLASS_NAME);
    int x = tea_check_integer(T, 1);
    int y = tea_check_integer(T, 2);
    sr_Pixel px = sr_getPixel(self->buf, x, y);
    tea_new_list(T, 4);
    tea_push_integer(T, px.rgba.r); tea_add_item(T, -2);
    tea_push_integer(T, px.rgba.g); tea_add_item(T, -2);
    tea_push_integer(T, px.rgba.b); tea_add_item(T, -2);
    tea_push_integer(T, px.rgba.a); tea_add_item(T, -2);
}

static void image_copyPixels(tea_State* T)
{
    sr_Rect sub;
    Image* self = (Image*)tea_check_udata(T, 0, CLASS_NAME);
    Image* src = (Image*)tea_check_udata(T, 1, CLASS_NAME);
    int x = tea_opt_integer(T, 2, 0);
    int y = tea_opt_integer(T, 3, 0);
    int hassub = 0;
    if(!tea_is_nonenil(T, 4))
    {
        hassub = 1;
        sub = get_rect(T, 4);
        check_subrect(T, 4, src->buf, &sub);
    }
    float sx = tea_opt_number(T, 5, 1);
    float sy = tea_opt_number(T, 6, sx);
    sr_copyPixels(self->buf, src->buf, x, y,
                    hassub ? &sub : NULL, sx, sy);
}

static void image_noise(tea_State* T)
{
    Image* self = tea_check_udata(T, 0, CLASS_NAME);
    int seed = tea_opt_number(T, 1, rand());
    int low = tea_opt_number(T, 2, 0);
    int high = tea_opt_number(T, 3, 0);
    int grey = tea_opt_bool(T, 4, false);
    sr_noise(self->buf, seed, low, high, grey);
}

static void image_floodFill(tea_State* T)
{
    Image* self = tea_check_udata(T, 0, CLASS_NAME);
    int x = tea_check_integer(T, 1);
    int y = tea_check_integer(T, 2);
    sr_Pixel px = get_color(T, 3);
    sr_floodFill(self->buf, px, x, y);
}

static void image_getWidth(tea_State* T)
{
    Image* self = (Image*)tea_check_udata(T, 0, CLASS_NAME);
    tea_push_integer(T, self->buf->w);
}

static void image_getHeight(tea_State* T)
{
    Image* self = (Image*)tea_check_udata(T, 0, CLASS_NAME);
    tea_push_integer(T, self->buf->h);
}

static void image_tostring(tea_State* T)
{
    tea_push_string(T, "<Image>");
}

static const tea_Methods reg[] = {
    { "fromFile", "static", image_fromFile, 1, 0 },
    { "fromString", "static", image_fromString, 1, 0 },
    { "fromBlank", "static", image_fromBlank, 2, 0 },
    { "clone", "method", image_clone, 1, 0 },
    { "reset", "method", image_reset, 1, 0 },
    { "clear", "method", image_clear, 1, 4 },
    { "setPixel", "method", image_setPixel, 3, 4 },
    { "getPixel", "method", image_getPixel, 3, 0 },
    { "copyPixels", "method", image_copyPixels, 2, 5 },
    { "noise", "method", image_noise, 1, 4 },
    { "floodFill", "method", image_floodFill, 3, 4 },
    { "getWidth", "method", image_getWidth, 1, 0 },
    { "getHeight", "method", image_getHeight, 1, 0 },
    { "tostring", "method", image_tostring, 1, 0 },
    { NULL, NULL, NULL }
};

void micro_open_image(tea_State* T)
{
    tea_create_class(T, "Image", reg);
    tea_push_value(T, -1);
    tea_set_key(T, TEA_REGISTRY_INDEX, CLASS_NAME);
}