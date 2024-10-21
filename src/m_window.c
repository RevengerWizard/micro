#include <GLFW/glfw3.h>

#include <tea.h>

static bool fullscreen = false;

static void window_setTitle(tea_State* T)
{
    const char* title = tea_check_string(T, 0);
    glfwSetWindowTitle(NULL, title);
}

static void window_getTitle(tea_State* T)
{
    const char* title = glfwGetWindowTitle(NULL);
    tea_push_string(T, title);
}

static const tea_Reg reg[] = {
    //{ "setSize", window_setSize },
    { "setTitle", window_setTitle, 1, 0 },
    { "getTitle", window_getTitle, 0, 0 },
    //{ "setFullscreen", window_setFullscreen },
    { NULL, NULL }
};

void micro_open_window(tea_State* T)
{
    tea_create_submodule(T, "window", reg);
}