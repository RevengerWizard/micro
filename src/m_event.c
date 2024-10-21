#include <GLFW/glfw3.h>
#include "gleq.h"

#include "spxe.h"

#include <tea.h>

#include "mapping.h"

// I hate you GLFW
static int utf32_to_utf8(uint32_t utf32, char* utf8)
{
    if (utf32 < 0x80) {
        utf8[0] = (char)utf32;
        return 1;
    } else if (utf32 < 0x800) {
        utf8[0] = (char)(0xC0 | (utf32 >> 6));
        utf8[1] = (char)(0x80 | (utf32 & 0x3F));
        return 2;
    } else if (utf32 < 0x10000) {
        utf8[0] = (char)(0xE0 | (utf32 >> 12));
        utf8[1] = (char)(0x80 | ((utf32 >> 6) & 0x3F));
        utf8[2] = (char)(0x80 | (utf32 & 0x3F));
        return 3;
    } else if (utf32 < 0x110000) {
        utf8[0] = (char)(0xF0 | (utf32 >> 18));
        utf8[1] = (char)(0x80 | ((utf32 >> 12) & 0x3F));
        utf8[2] = (char)(0x80 | ((utf32 >> 6) & 0x3F));
        utf8[3] = (char)(0x80 | (utf32 & 0x3F));
        return 4;
    }
    return 0; // Invalid Unicode code point
}

static void event_poll(tea_State* T)
{
    /* Create event list */
    tea_new_list(T, 0);

    /* Poll events */
    GLEQevent e;
    while(gleqNextEvent(&e))
    {
        tea_new_map(T);

        switch(e.type)
        {
            case GLEQ_WINDOW_CLOSED:
            {
                tea_push_literal(T, "quit");
                tea_set_key(T, -2, "type");
                break;
            }
            case GLEQ_CURSOR_MOVED:
            {
                int x, y;
                spxeMousePos(&x, &y);

                //int x = e.pos.x, y = e.pos.y;
                tea_push_literal(T, "mousemove");
                tea_set_key(T, -2, "type");
                tea_push_number(T, x);
                tea_set_key(T, -2, "x");
                tea_push_number(T, y);
                tea_set_key(T, -2, "y");
                break;
            }
            case GLEQ_BUTTON_PRESSED:
            case GLEQ_BUTTON_RELEASED:
            {
                bool down = e.type == GLEQ_BUTTON_PRESSED;
                if(down)
                    tea_push_literal(T, "mousedown");
                else
                    tea_push_literal(T, "mouseup");
                tea_set_key(T, -2, "type");
                tea_push_string(T, mouse_names[e.mouse.button]);
                tea_set_key(T, -2, "button");
                break;
            }
            case GLEQ_KEY_PRESSED:
            case GLEQ_KEY_RELEASED:
            {
                bool down = e.type == GLEQ_KEY_PRESSED;
                if(down)
                    tea_push_literal(T, "keydown");
                else
                    tea_push_literal(T, "keyup");
                tea_set_key(T, -2, "type");
                tea_push_string(T, micro_key_filter(e.keyboard.key));
                tea_set_key(T, -2, "key");
                break;
            }
            case GLEQ_CODEPOINT_INPUT:
            {
                char utf8[5] = {0};
                int len = utf32_to_utf8(e.codepoint, utf8);
                if(len > 0)
                {
                    tea_push_literal(T, "textinput");
                    tea_set_key(T, -2, "type");
                    tea_push_lstring(T, utf8, len);
                    tea_set_key(T, -2, "text");
                }
                break;
            }
            default:
                break;
        }
        gleqFreeEvent(&e);

        /* Add event to events list */
        tea_add_item(T, -2);
    }
}

static void event_pump(tea_State* T)
{
    glfwPollEvents();
}

static void event_quit(tea_State* T)
{
    GLFWwindow* window = spxeGetWindow();
    glfwSetWindowShouldClose(window, GLFW_TRUE);
}

static const tea_Reg reg[] = {
    { "poll", event_poll, 0, 0 },
    { "pump", event_pump, 0, 0 },
    { "quit", event_quit, 0, 0 },
    { NULL, NULL }
};

void micro_open_event(tea_State* T)
{
    tea_create_submodule(T, "event", reg);
}