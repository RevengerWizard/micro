#include <stdlib.h>
#include <stdio.h>
#include <windows.h>

#include <tea.h>

#include <GLFW/glfw3.h>

#include "spxe.h"

#include "micro.h"
#include "m_init.h"

sr_Buffer* screen = NULL;
Px* pixbuf = NULL;
ma_device device;

int main(int argc, char** argv)
{
    tea_State* T = tea_open();
    tea_set_argv(T, argc, argv, 0);

    if(!micro_open(T))
        return EXIT_FAILURE;

    /* Do main loop */
    double last = 0;
    while(spxeStep())
    {
        if(tea_get_global(T, "micro"))
        {
            tea_get_attr(T, -1, "run");
            if(tea_pcall(T, 0) != TEA_OK)
            {
                const char* str = tea_to_string(T, -1);
                fputs(str, stderr);
                fputc('\n', stderr);
                break;
            }
            tea_pop(T, 1);
        }
        spxeRender(pixbuf);
        /* Wait for next frame */
        double step = 1 / 60;
        double now = glfwGetTime();
        double wait = step - (now - last);
        last += step;
        if(wait > 0)
        {
            Sleep(wait * 1000);
        }
        else
        {
            last = now;
        }
    }

    screen = NULL;
    pixbuf = NULL;
    ma_device_uninit(&device);

    tea_close(T);

    return spxeEnd(pixbuf);
}