#ifndef _MICRO_H
#define _MICRO_H

#ifndef MICRO_NO_AUDIO
#include "miniaudio.h"
#endif

#include "sera.h"
#include "spxe.h"

#include <tea.h>

extern sr_Buffer* screen;
extern Px* pixbuf;
extern double maxFps;

#ifndef MICRO_NO_AUDIO
extern ma_device device;
#endif

void micro_open_Source(tea_State* T);
void micro_open_Data(tea_State* T);
void micro_open_Gif(tea_State* T);
void micro_open_Font(tea_State* T);
void micro_open_Image(tea_State* T);

void micro_open_audio(tea_State* T);
void micro_open_data(tea_State* T);
void micro_open_event(tea_State* T);
void micro_open_system(tea_State* T);
void micro_open_keyboard(tea_State* T);
void micro_open_mouse(tea_State* T);
void micro_open_timer(tea_State* T);
void micro_open_window(tea_State* T);
void micro_open_filesystem(tea_State* T);
void micro_open_gfx(tea_State* T);
void micro_open_fx(tea_State* T);

tea_State* micro_open(int argc, char** argv);
void micro_run(tea_State* T);
bool micro_close(tea_State* T);

#endif
