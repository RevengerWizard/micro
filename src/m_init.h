#ifndef _M_INIT_H
#define _M_INIT_H

#include <tea.h>

void micro_open_source(tea_State* T);
void micro_open_data(tea_State* T);
void micro_open_gif(tea_State* T);
void micro_open_font(tea_State* T);
void micro_open_image(tea_State* T);

void micro_open_audio(tea_State* T);
void micro_open_event(tea_State* T);
void micro_open_system(tea_State* T);
void micro_open_keyboard(tea_State* T);
void micro_open_mouse(tea_State* T);
void micro_open_timer(tea_State* T);
void micro_open_window(tea_State* T);
void micro_open_filesystem(tea_State* T);
void micro_open_gfx(tea_State* T);
void micro_open_fx(tea_State* T);
void micro_open_debug(tea_State* T);

bool micro_open(tea_State* T);

#endif