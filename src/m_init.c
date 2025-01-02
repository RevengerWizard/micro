#include <stdio.h>
#include <stdlib.h>

#include "m_init.h"

void micro_open_keyboard(tea_State* T)
{
    tea_create_submodule(T, "keyboard", NULL);
}

static const tea_Reg mods[] = {
    /* Objects */
    { "Source", micro_open_source },
    { "Data", micro_open_data },
    { "Gif", micro_open_gif },
    { "Font", micro_open_font },
    { "Image", micro_open_image },
    /* Modules */
    { "audio", micro_open_audio },
    { "data", micro_open_data_ },
    { "keyboard", micro_open_keyboard },
    { "mouse", micro_open_mouse },
    { "event", micro_open_event },
    { "system", micro_open_system },
    { "timer", micro_open_timer },
    { "window", micro_open_window },
    { "filesystem", micro_open_filesystem },
    { "gfx", micro_open_gfx },
    { "fx", micro_open_fx },
    { NULL, NULL }
};

bool micro_open(tea_State* T)
{
    tea_new_module(T, "micro");
    for(int i = 0; mods[i].name; i++)
    {
        mods[i].fn(T);
        tea_set_attr(T, -2, mods[i].name);
    }
    tea_set_global(T, "micro");

    /* 
    * Init embedded scripts 
    * -- these should be ordered in the array in the order we want them loaded;
    * init.tea should always be last since it depends on all the other modules 
    */
#include "graphics_tea.h"
#include "keyboard_tea.h"
#include "mouse_tea.h"
#include "timer_tea.h"
#include "init_tea.h"
    struct
    {
        const char* name;
        const char* data;
        int size;
    } items[] = {
        { "graphics.tea", graphics_tea, sizeof(graphics_tea) },
        { "keyboard.tea", keyboard_tea, sizeof(keyboard_tea) },
        { "mouse.tea", mouse_tea, sizeof(mouse_tea) },
        { "timer.tea", timer_tea, sizeof(timer_tea) },
        { "init.tea", init_tea, sizeof(init_tea) },
        { NULL, NULL, 0 }
    };

    int i;
    for(i = 0; items[i].name; i++)
    {
        int status = tea_load_buffer(T, items[i].data, items[i].size, items[i].name);
        if(status || tea_pcall(T, 0) != TEA_OK)
        {
            const char* str = tea_to_string(T, -1);
            fputs(str, stderr);
            fputc('\n', stderr);
            return false;
        }
    }
    return true;
}