#include <stdlib.h>
#include <stdio.h>

#include <tea.h>

#include <GLFW/glfw3.h>

#include "spxe.h"

#include "micro.h"

#ifdef  _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif

sr_Buffer* screen = NULL;
Px* pixbuf = NULL;

#ifndef MICRO_NO_AUDIO
ma_device device;
#endif

static void micro_sleep(double wait)
{
#ifdef _WIN32
    Sleep(wait * 1000);
#endif
}

void micro_open_keyboard(tea_State* T)
{
    tea_create_submodule(T, "keyboard", NULL);
}

static const struct { char* name; void (*fn)(tea_State*); } mods[] = {
    /* Objects */
    {"Source", micro_open_Source},
    {"Data", micro_open_Data},
    {"Gif", micro_open_Gif},
    {"Font", micro_open_Font},
    {"Image", micro_open_Image},
    /* Modules */
    {"audio", micro_open_audio },
    {"data", micro_open_data},
    {"keyboard", micro_open_keyboard},
    {"mouse", micro_open_mouse},
    {"event", micro_open_event},
    {"system", micro_open_system},
    {"timer", micro_open_timer},
    {"window", micro_open_window},
    {"fs", micro_open_filesystem},
    {"gfx", micro_open_gfx},
    {"imagefx", micro_open_fx},
    {NULL, NULL}
};

tea_State* micro_open(int argc, char** argv)
{
    tea_State* T = tea_open();
    if(!T) return NULL;
    tea_set_argv(T, argc, argv, 0);

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
        {"=graphics.tea", graphics_tea, sizeof(graphics_tea)},
        {"=keyboard.tea", keyboard_tea, sizeof(keyboard_tea)},
        {"=mouse.tea", mouse_tea, sizeof(mouse_tea)},
        {"=timer.tea", timer_tea, sizeof(timer_tea)},
        {"=init.tea", init_tea, sizeof(init_tea)},
        {NULL, NULL, 0}
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
            return NULL;
        }
    }
    return T;
}

void micro_run(tea_State* T)
{
    if(tea_get_global(T, "micro"))
    {
        tea_get_attr(T, -1, "run");
        if(tea_pcall(T, 0) != TEA_OK)
        {
            const char* str = tea_to_string(T, -1);
            fputs(str, stderr);
            fputc('\n', stderr);
            return;
        }
        if(tea_is_number(T, -1) && tea_get_number(T, -1) == 1)
            return;
        tea_pop(T, 1);
    }
    spxeRender(pixbuf);
}

bool micro_close(tea_State* T)
{
#ifndef MICRO_NO_AUDIO
    ma_device_uninit(&device);
#endif

    tea_close(T);
    bool res = spxeEnd(pixbuf);
    screen = NULL;
    pixbuf = NULL;
    return res;
}

int main(int argc, char** argv)
{
#ifdef _WIN32
    /* On Windows, you never know... */
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
#endif

    tea_State* T = micro_open(argc, argv);
    if(!T) return EXIT_FAILURE;

    /* Do main loop */
    double last = 0;
    while(spxeStep())
    {
        micro_run(T);
        /* Wait for next frame */
        double step = 1.0 / maxFps;
        double now = glfwGetTime();
        double wait = step - (now - last);
        last += step;
        if(wait > 0)
        {
            micro_sleep(wait);
        }
        else
        {
            last = now;
        }
    }

    return micro_close(T);
}