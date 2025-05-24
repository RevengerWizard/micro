#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <tea.h>
#include "sera.h"
#include "fs.h"
#include "ttf.h"
#include "m_image.h"

#define CLASS_NAME  "micro.Font"
#define DEFAULT_FONTSIZE 8

typedef struct
{
    ttf_Font* font;
} Font;

static void font_free(void* ud)
{
    Font* self = (Font*)ud;
    if(self->font)
    {
        ttf_destroy(self->font);
    }
}

static Font* font_new(tea_State* T)
{
    Font* self = (Font*)tea_new_udata(T, sizeof(*self), CLASS_NAME);
    tea_set_finalizer(T, font_free);
    memset(self, 0, sizeof(*self));
    return self;
}

static const char* load_font(Font* self, const void* data, int len, int ptsize)
{
    self->font = ttf_new(data, len);
    if(!self->font)
    {
        return "could not load font";
    }
    ttf_ptsize(self->font, ptsize);
    return NULL;
}

static void font_fromFile(tea_State* T)
{
    const char* filename = tea_check_string(T, 0);
    int fontsize = tea_opt_integer(T, 1, DEFAULT_FONTSIZE);
    Font* self = font_new(T);
    size_t len;
    void* data = fs_read(filename, &len);
    /* Load new font */
    if(!data)
    {
        tea_error(T, "could not open file '%s'", filename);
    }
    const char* err = load_font(self, data, len, fontsize);
    free(data);
    if(err)
        tea_error(T, "%s", err);
}

static void font_fromEmbedded(tea_State* T)
{
#include "font_ttf.h"

    int fontsize = tea_opt_integer(T, 0, DEFAULT_FONTSIZE);
    Font* self = font_new(T);
    const char* err = load_font(self, font_ttf, sizeof(font_ttf), fontsize);
    if(err)
        tea_error(T, "%s", err);
}

static void font_render(tea_State* T)
{
    int w, h;
    Font* self = (Font*)tea_check_udata(T, 0, CLASS_NAME);
    const char* str = tea_check_string(T, 1);
    if(!str || *str == '\0')
        str = " ";
    Image* img = micro_image_new(T);
    void* data = ttf_render(self->font, str, &w, &h);
    if(!data)
    {
        tea_error(T, "could not render text");
    }
    /* Load bitmap and free intermediate 8bit bitmap */
    img->buf = sr_newBuffer(w, h);
    if(!img->buf)
    {
        free(data);
        tea_error(T, "could not create buffer");
    }
    sr_loadPixels8(img->buf, data, NULL);
    free(data);
}

static void font_getWidth(tea_State* T)
{
    Font* self = (Font*)tea_check_udata(T, 0, CLASS_NAME);
    const char* str = tea_check_string(T, 1);
    tea_push_integer(T, ttf_width(self->font, str));
}

static void font_getHeight(tea_State* T)
{
    Font* self = (Font*)tea_check_udata(T, 0, CLASS_NAME);
    tea_push_integer(T, ttf_height(self->font));
}

static void font_tostring(tea_State* T)
{
    tea_push_string(T, "<Font>");
}

static const tea_Methods reg[] = {
    { "fromFile", "static", font_fromFile, 1, 1 },
    { "fromEmbedded", "static", font_fromEmbedded, 0, 1 },
    { "render", "method", font_render, 2, 0 },
    { "getWidth", "method", font_getWidth, 2, 0 },
    { "getHeight", "method", font_getHeight, 1, 0 },
    { "tostring", "method", font_tostring, 1, 0 },
    { NULL, NULL, NULL }
};

void micro_open_font(tea_State* T)
{
    tea_create_class(T, "Font", reg);
    tea_push_value(T, -1);
    tea_set_key(T, TEA_REGISTRY_INDEX, CLASS_NAME);
}