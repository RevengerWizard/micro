#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "gleq.h"

#ifndef GLEQ_CAPACITY
    #define GLEQ_CAPACITY 1024
#endif

static struct
{
    GLEQevent events[GLEQ_CAPACITY];
    size_t head;
    size_t tail;
} gleq_queue = { {}, 0, 0 };

static char* gleq_strdup(const char* string)
{
    const size_t size = strlen(string) + 1;
    char* result = (char*) malloc(size);
    memcpy(result, string, size);
    return result;
}

static GLEQevent* gleq_new_event(void)
{
    GLEQevent* event = gleq_queue.events + gleq_queue.head;
    gleq_queue.head = (gleq_queue.head + 1) % GLEQ_CAPACITY;
    assert(gleq_queue.head != gleq_queue.tail);
    memset(event, 0, sizeof(GLEQevent));
    return event;
}

static void gleq_window_pos_callback(GLFWwindow* window, int x, int y)
{
    GLEQevent* event = gleq_new_event();
    event->type = GLEQ_WINDOW_MOVED;
    event->window = window;
    event->pos.x = x;
    event->pos.y = y;
}

static void gleq_window_size_callback(GLFWwindow* window, int width, int height)
{
    GLEQevent* event = gleq_new_event();
    event->type = GLEQ_WINDOW_RESIZED;
    event->window = window;
    event->size.width = width;
    event->size.height = height;
}

static void gleq_window_close_callback(GLFWwindow* window)
{
    GLEQevent* event = gleq_new_event();
    event->type = GLEQ_WINDOW_CLOSED;
    event->window = window;
}

static void gleq_window_refresh_callback(GLFWwindow* window)
{
    GLEQevent* event = gleq_new_event();
    event->type = GLEQ_WINDOW_REFRESH;
    event->window = window;
}

static void gleq_window_focus_callback(GLFWwindow* window, int focused)
{
    GLEQevent* event = gleq_new_event();
    event->window = window;

    if (focused)
        event->type = GLEQ_WINDOW_FOCUSED;
    else
        event->type = GLEQ_WINDOW_DEFOCUSED;
}

static void gleq_window_iconify_callback(GLFWwindow* window, int iconified)
{
    GLEQevent* event = gleq_new_event();
    event->window = window;

    if (iconified)
        event->type = GLEQ_WINDOW_ICONIFIED;
    else
        event->type = GLEQ_WINDOW_UNICONIFIED;
}

static void gleq_framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    GLEQevent* event = gleq_new_event();
    event->type = GLEQ_FRAMEBUFFER_RESIZED;
    event->window = window;
    event->size.width = width;
    event->size.height = height;
}

static void gleq_mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
    GLEQevent* event = gleq_new_event();
    event->window = window;
    event->mouse.button = button;
    event->mouse.mods = mods;

    if (action == GLFW_PRESS)
        event->type = GLEQ_BUTTON_PRESSED;
    else if (action == GLFW_RELEASE)
        event->type = GLEQ_BUTTON_RELEASED;
}

static void gleq_cursor_pos_callback(GLFWwindow* window, double x, double y)
{
    GLEQevent* event = gleq_new_event();
    event->type = GLEQ_CURSOR_MOVED;
    event->window = window;
    event->pos.x = (int) x;
    event->pos.y = (int) y;
}

static void gleq_cursor_enter_callback(GLFWwindow* window, int entered)
{
    GLEQevent* event = gleq_new_event();
    event->window = window;

    if (entered)
        event->type = GLEQ_CURSOR_ENTERED;
    else
        event->type = GLEQ_CURSOR_LEFT;
}

static void gleq_scroll_callback(GLFWwindow* window, double x, double y)
{
    GLEQevent* event = gleq_new_event();
    event->type = GLEQ_SCROLLED;
    event->window = window;
    event->scroll.x = x;
    event->scroll.y = y;
}

static void gleq_key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    GLEQevent* event = gleq_new_event();
    event->window = window;
    event->keyboard.key = key;
    event->keyboard.scancode = scancode;
    event->keyboard.mods = mods;

    if (action == GLFW_PRESS)
        event->type = GLEQ_KEY_PRESSED;
    else if (action == GLFW_RELEASE)
        event->type = GLEQ_KEY_RELEASED;
    else if (action == GLFW_REPEAT)
        event->type = GLEQ_KEY_REPEATED;
}

static void gleq_char_callback(GLFWwindow* window, unsigned int codepoint)
{
    GLEQevent* event = gleq_new_event();
    event->type = GLEQ_CODEPOINT_INPUT;
    event->window = window;
    event->codepoint = codepoint;
}

static void gleq_monitor_callback(GLFWmonitor* monitor, int action)
{
    GLEQevent* event = gleq_new_event();
    event->monitor = monitor;

    if (action == GLFW_CONNECTED)
        event->type = GLEQ_MONITOR_CONNECTED;
    else if (action == GLFW_DISCONNECTED)
        event->type = GLEQ_MONITOR_DISCONNECTED;
}

#if GLFW_VERSION_MINOR >= 1
static void gleq_file_drop_callback(GLFWwindow* window, int count, const char** paths)
{
    GLEQevent* event = gleq_new_event();
    event->type = GLEQ_FILE_DROPPED;
    event->window = window;
    event->file.paths = (char**) malloc(count * sizeof(char*));
    event->file.count = count;

    while (count--)
        event->file.paths[count] = gleq_strdup(paths[count]);
}
#endif

#if GLFW_VERSION_MINOR >= 2
static void gleq_joystick_callback(int jid, int action)
{
    GLEQevent* event = gleq_new_event();
    event->joystick = jid;

    if (action == GLFW_CONNECTED)
        event->type = GLEQ_JOYSTICK_CONNECTED;
    else if (action == GLFW_DISCONNECTED)
        event->type = GLEQ_JOYSTICK_DISCONNECTED;
}
#endif

#if GLFW_VERSION_MINOR >= 3
static void gleq_window_maximize_callback(GLFWwindow* window, int maximized)
{
    GLEQevent* event = gleq_new_event();
    event->window = window;

    if (maximized)
        event->type = GLEQ_WINDOW_MAXIMIZED;
    else
        event->type = GLEQ_WINDOW_UNMAXIMIZED;
}

static void gleq_window_content_scale_callback(GLFWwindow* window, float xscale, float yscale)
{
    GLEQevent* event = gleq_new_event();
    event->window = window;
    event->type = GLEQ_WINDOW_SCALE_CHANGED;
    event->scale.x = xscale;
    event->scale.y = yscale;
}
#endif

GLEQDEF void gleqInit(void)
{
    glfwSetMonitorCallback(gleq_monitor_callback);
#if GLFW_VERSION_MINOR >= 2
    glfwSetJoystickCallback(gleq_joystick_callback);
#endif
}

GLEQDEF void gleqTrackWindow(GLFWwindow* window)
{
    glfwSetWindowPosCallback(window, gleq_window_pos_callback);
    //glfwSetWindowSizeCallback(window, gleq_window_size_callback);
    glfwSetWindowCloseCallback(window, gleq_window_close_callback);
    glfwSetWindowRefreshCallback(window, gleq_window_refresh_callback);
    glfwSetWindowFocusCallback(window, gleq_window_focus_callback);
    glfwSetWindowIconifyCallback(window, gleq_window_iconify_callback);
    glfwSetFramebufferSizeCallback(window, gleq_framebuffer_size_callback);
    glfwSetMouseButtonCallback(window, gleq_mouse_button_callback);
    glfwSetCursorPosCallback(window, gleq_cursor_pos_callback);
    glfwSetCursorEnterCallback(window, gleq_cursor_enter_callback);
    glfwSetScrollCallback(window, gleq_scroll_callback);
    glfwSetKeyCallback(window, gleq_key_callback);
    glfwSetCharCallback(window, gleq_char_callback);
#if GLFW_VERSION_MINOR >= 1
    glfwSetDropCallback(window, gleq_file_drop_callback);
#endif
#if GLFW_VERSION_MINOR >= 3
    glfwSetWindowMaximizeCallback(window, gleq_window_maximize_callback);
    glfwSetWindowContentScaleCallback(window, gleq_window_content_scale_callback);
#endif
}

GLEQDEF int gleqNextEvent(GLEQevent* event)
{
    memset(event, 0, sizeof(GLEQevent));

    if (gleq_queue.head != gleq_queue.tail)
    {
        *event = gleq_queue.events[gleq_queue.tail];
        gleq_queue.tail = (gleq_queue.tail + 1) % GLEQ_CAPACITY;
    }

    return event->type != GLEQ_NONE;
}

GLEQDEF void gleqFreeEvent(GLEQevent* event)
{
#if GLFW_VERSION_MINOR >= 1
    if (event->type == GLEQ_FILE_DROPPED)
    {
        while (event->file.count--)
            free(event->file.paths[event->file.count]);

        free(event->file.paths);
    }
#endif

    memset(event, 0, sizeof(GLEQevent));
}