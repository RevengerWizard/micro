#include <stdio.h>
#include <stdlib.h>

#include "micro.h"
#include "m_init.h"
#include "m_image.h"

static sr_Pixel get_color(tea_State* T, int idx)
{
    int r = tea_opt_integer(T, idx, 255);
    int g = tea_opt_integer(T, idx + 1, 255);
    int b = tea_opt_integer(T, idx + 2, 255);
    int a = tea_opt_integer(T, idx + 3, 255);
    return sr_pixel(r, g, b, a);
}

static sr_Rect get_rect(tea_State* T, int idx)
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

static void check_subrect(tea_State* T, int idx, sr_Buffer* b, sr_Rect* r)
{
    if(r->x < 0 || r->y < 0 || r->x + r->w > b->w || r->y + r->h > b->h)
    {
        tea_arg_error(T, idx, "sub rectangle out of bounds");
    }
}

static bool inited = false;

static void gfx_init(tea_State* T)
{
    int width = tea_check_integer(T, 0);
    int height = tea_check_integer(T, 1);
    if(inited)
    {
        tea_error(T, "gfx is already inited");
    }

    pixbuf = spxeStart("micrö", 800, 800, width, height);
    if(!pixbuf)
    {
        fprintf(stderr, "could not init spxe\n");
        exit(1);
    }
    screen = sr_newBufferShared(pixbuf, width, height);
    Image* img = micro_image_new(T);
    img->buf = screen;
    tea_push_value(T, -1);
    tea_set_key(T, TEA_REGISTRY_INDEX, "micro.screen");
    inited = true;
}

static void gfx_setAlpha(tea_State* T)
{
    sr_setAlpha(screen, tea_opt_integer(T, 1, 255));
}

static void gfx_setBlend(tea_State* T)
{
    const char* const modes[] = { "alpha", "color", "add", "subtract", "multiply", "lighten", "darken", "screen", "difference", NULL };
    int mode = tea_check_option(T, 0, "alpha", modes);
    sr_setBlend(screen, mode);
}

static void gfx_setColor(tea_State* T)
{
    sr_setColor(screen, get_color(T, 0));
}

static void gfx_clear(tea_State* T)
{
    sr_clear(screen, get_color(T, 0));
}

static void gfx_pixel(tea_State* T)
{
    int x = tea_check_integer(T, 0);
    int y = tea_check_integer(T, 1);
    sr_drawPixel(screen, get_color(T, 2), x, y);
}

static void gfx_line(tea_State* T)
{
    int x1 = tea_check_integer(T, 0);
    int y1 = tea_check_integer(T, 1);
    int x2 = tea_check_integer(T, 2);
    int y2 = tea_check_integer(T, 3);
    sr_drawLine(screen, get_color(T, 4), x1, y1, x2, y2);
}

static const char* styles[] = { "fill", "line", NULL };

static void gfx_rect(tea_State* T)
{
    int id = tea_check_option(T, 0, NULL, styles);
    int x = tea_check_integer(T, 1);
    int y = tea_check_integer(T, 2);
    int w = tea_check_integer(T, 3);
    int h = tea_check_integer(T, 4);
    sr_Pixel px = get_color(T, 5);
    if(id == 0)
        sr_drawFilledRect(screen, px, x, y, w, h);
    else
        sr_drawRect(screen, px, x, y, w, h);
}

static void gfx_circle(tea_State* T)
{
    int id = tea_check_option(T, 0, NULL, styles);
    int x = tea_check_integer(T, 1);
    int y = tea_check_integer(T, 2);
    int radius = tea_check_integer(T, 3);
    sr_Pixel px = get_color(T, 4);
    if(id == 0)
        sr_drawFilledCircle(screen, px, x, y, radius);
    else
        sr_drawCircle(screen, px, x, y, radius);
}

static void gfx_draw(tea_State* T)
{
    int hasrect = 0;
    sr_Rect sub;
    sr_Transform t;
    Image* src = (Image*)tea_check_udata(T, 0, IMAGE_CLASS_NAME);
    int x = tea_check_integer(T, 1);
    int y = tea_check_integer(T, 2);
    if(!tea_is_nonenil(T, 3))
    {
        hasrect = 1;
        sub = get_rect(T, 3);
        check_subrect(T, 3, src->buf, &sub);
    }
    t.r = tea_opt_integer(T, 4, 0);
    t.sx = tea_opt_integer(T, 5, 1);
    t.sy = tea_opt_integer(T, 6, t.sx);
    t.ox = tea_opt_integer(T, 7, 0);
    t.oy = tea_opt_integer(T, 8, 0);
    sr_drawBuffer(screen, src->buf, x, y, hasrect ? &sub : NULL, &t);
}

static const tea_Reg reg[] = {
    { "init", gfx_init, 2, 0 },
    { "setAlpha", gfx_setAlpha, 0, 1 },
    { "setBlend", gfx_setBlend, 1, 0 },
    { "setColor", gfx_setColor, 0, 4 },
    { "clear", gfx_clear, 0, 4 },
    { "pixel", gfx_pixel, 2, 4 },
    { "line", gfx_line, 4, 4 },
    { "rect", gfx_rect, 5, 4 },
    { "circle", gfx_circle, 4, 4 },
    { "draw", gfx_draw, 3, 6 },
    { NULL, NULL }
};

void micro_open_gfx(tea_State* T)
{
    tea_create_submodule(T, "gfx", reg);
}