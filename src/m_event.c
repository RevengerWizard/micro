#include "def.h"

#include <GLFW/glfw3.h>
#include "gleq.h"

#include "spxe.h"

#include <tea.h>

#define NONE "unknown"

/* Start from 32 */
static const char* const key_names1[] = {
    "space",
    NONE, NONE, NONE, NONE, NONE, NONE,    /* Empty */
    "'",
    NONE, NONE, NONE, NONE,    /* Empty */
    ",",
    "-",
    ".",
    "/",
    "0",
    "1",
    "2",
    "3",
    "4",
    "5",
    "6",
    "7",
    "8",
    "9",
    NONE,   /* Empty */
    ";",
    NONE,   /* Empty */
    "=",
    NONE, NONE, NONE,   /* Empty */
    "a",
    "b",
    "c",
    "d",
    "e",
    "f",
    "g",
    "h",
    "i",
    "j",
    "k",
    "l",
    "m",
    "n",
    "o",
    "p",
    "q",
    "r",
    "s",
    "t",
    "u",
    "v",
    "w",
    "x",
    "y",
    "z",
    "[",
    "\\",
    "]",
    NONE, NONE, /* Empty */
    "`",
};

/* Start from 256 */
static const char* const key_names2[] = {
    "escape",
    "enter",
    "tab",
    "backspace",
    "insert",
    "delete",
    "right",
    "left",
    "down",
    "up",
    "pageup",
    "pagedown",
    "home",
    "end",
    NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE, /* Empty */
    "capslock",
    "scroll",
    "numlock",
    "print",
    "pause",
    NONE, NONE, NONE, NONE, NONE,   /* Empty */
    "f1",
    "f2",
    "f3",
    "f4",
    "f5",
    "f6",
    "f7",
    "f8",
    "f9",
    "f10",
    "f11",
    "f12",
    "f13",
    "f14",
    "f15",
    "f16",
    "f17",
    "f18",
    "f19",
    "f20",
    "f21",
    "f22",
    "f23",
    "f24",
    "f25",
    NONE, NONE, NONE, NONE, NONE,   /* Empty */
    /* KP */
    "0",
    "1",
    "2",
    "3",
    "4",
    "5",
    "6",
    "7",
    "8",
    "9",
    ".",
    "/",
    "*",
    "-",
    "+",
    "enter",
    "=",
    NONE, NONE, NONE,   /* Empty */
    "lshift",
    "lctrl",
    "lalt",
    "lgui",
    "rshift",
    "rctrl",
    "ralt",
    "rgui",
    "menu"
};

static const char* const mouse_names[] = {
    "left",
    "right",
    "middle",
    "button4",
    "button5",
    "button6",
    "button7",
    "button8",
};

static const char* micro_key_filter(int key)
{
    if(key >= GLFW_KEY_SPACE && key <= GLFW_KEY_GRAVE_ACCENT)
        return key_names1[key - GLFW_KEY_SPACE];
    else if(key >= GLFW_KEY_ESCAPE && key <= GLFW_KEY_LAST)
        return key_names2[key - GLFW_KEY_ESCAPE];
    else
        return NONE;
}

static size_t utf32_to_utf8(uint32_t utf32, char* utf8)
{
    if(utf32 < 0x80)
    {
        utf8[0] = (char)utf32;
        return 1;
    }
    else if(utf32 < 0x800)
    {
        utf8[0] = (char)(0xC0 | (utf32 >> 6));
        utf8[1] = (char)(0x80 | (utf32 & 0x3F));
        return 2;
    }
    else if(utf32 < 0x10000)
    {
        utf8[0] = (char)(0xE0 | (utf32 >> 12));
        utf8[1] = (char)(0x80 | ((utf32 >> 6) & 0x3F));
        utf8[2] = (char)(0x80 | (utf32 & 0x3F));
        return 3;
    }
    else if(utf32 < 0x110000)
    {
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
                size_t len = utf32_to_utf8(e.codepoint, utf8);
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
    UNUSED(T);
    glfwPollEvents();
}

static void event_quit(tea_State* T)
{
    UNUSED(T);
    GLFWwindow* window = spxeGetWindow();
    glfwSetWindowShouldClose(window, GLFW_TRUE);
}

static const tea_Reg reg[] = {
    {"poll", event_poll, 0, 0},
    {"pump", event_pump, 0, 0},
    {"quit", event_quit, 0, 0},
    {NULL}
};

void micro_open_event(tea_State* T)
{
    tea_create_submodule(T, "event", reg);
}