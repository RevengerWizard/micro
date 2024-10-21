/*
 * GLEQ - A basic event queue for GLFW 3
 * Copyright © Camilla Löwy <elmindreda@glfw.org>
 *
 * This software is provided 'as-is', without any express or implied
 * warranty. In no event will the authors be held liable for any damages
 * arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software
 *    in a product, an acknowledgment in the product documentation would
 *    be appreciated but is not required.
 *
 * 2. Altered source versions must be plainly marked as such, and must not
 *    be misrepresented as being the original software.
 *
 * 3. This notice may not be removed or altered from any source
 *    distribution.
 */

#ifndef GLEQ_HEADER_FILE
#define GLEQ_HEADER_FILE

#include <GLFW/glfw3.h>

#ifdef GLEQ_STATIC
    #define GLEQDEF static
#else
    #define GLEQDEF extern
#endif

typedef enum
{
    GLEQ_NONE,
    GLEQ_WINDOW_MOVED,
    GLEQ_WINDOW_RESIZED,
    GLEQ_WINDOW_CLOSED,
    GLEQ_WINDOW_REFRESH,
    GLEQ_WINDOW_FOCUSED,
    GLEQ_WINDOW_DEFOCUSED,
    GLEQ_WINDOW_ICONIFIED,
    GLEQ_WINDOW_UNICONIFIED,
    GLEQ_FRAMEBUFFER_RESIZED,
    GLEQ_BUTTON_PRESSED,
    GLEQ_BUTTON_RELEASED,
    GLEQ_CURSOR_MOVED,
    GLEQ_CURSOR_ENTERED,
    GLEQ_CURSOR_LEFT,
    GLEQ_SCROLLED,
    GLEQ_KEY_PRESSED,
    GLEQ_KEY_REPEATED,
    GLEQ_KEY_RELEASED,
    GLEQ_CODEPOINT_INPUT,
    GLEQ_MONITOR_CONNECTED,
    GLEQ_MONITOR_DISCONNECTED,
#if GLFW_VERSION_MINOR >= 1
    GLEQ_FILE_DROPPED,
#endif
#if GLFW_VERSION_MINOR >= 2
    GLEQ_JOYSTICK_CONNECTED,
    GLEQ_JOYSTICK_DISCONNECTED,
#endif
#if GLFW_VERSION_MINOR >= 3
    GLEQ_WINDOW_MAXIMIZED,
    GLEQ_WINDOW_UNMAXIMIZED,
    GLEQ_WINDOW_SCALE_CHANGED,
#endif
} GLEQtype;

typedef struct GLEQevent
{
    GLEQtype type;
    union {
        GLFWwindow* window;
        GLFWmonitor* monitor;
        int joystick;
    };
    union {
        struct {
            int x;
            int y;
        } pos;
        struct {
            int width;
            int height;
        } size;
        struct {
            double x;
            double y;
        } scroll;
        struct {
            int key;
            int scancode;
            int mods;
        } keyboard;
        struct {
            int button;
            int mods;
        } mouse;
        unsigned int codepoint;
#if GLFW_VERSION_MINOR >= 1
        struct {
            char** paths;
            int count;
        } file;
#endif
#if GLFW_VERSION_MINOR >= 3
        struct {
            float x;
            float y;
        } scale;
#endif
    };
} GLEQevent;

GLEQDEF void gleqInit(void);
GLEQDEF void gleqTrackWindow(GLFWwindow* window);

GLEQDEF int gleqNextEvent(GLEQevent* event);
GLEQDEF void gleqFreeEvent(GLEQevent* event);

#endif