#include <GLFW/glfw3.h>

#include "spxe.h"

#include <tea.h>

static void window_setSize(tea_State* T)
{
    GLFWwindow* window = spxeGetWindow();
    int width = tea_check_integer(T, 0);
    int height = tea_check_integer(T, 1);
    glfwSetWindowSize(window, width, height);
}

static void window_setTitle(tea_State* T)
{
    GLFWwindow* window = spxeGetWindow();
    const char* title = tea_check_string(T, 0);
    glfwSetWindowTitle(window, title);
}

static void window_getTitle(tea_State* T)
{
    GLFWwindow* window = spxeGetWindow();
    const char* title = glfwGetWindowTitle(window);
    tea_push_string(T, title);
}

static void window_setFullscreen(tea_State* T)
{
    bool b = tea_check_bool(T, 0);
    spxeSetFullscreen(b);
}

static const tea_Reg reg[] = {
    { "setSize", window_setSize, 2, 0 },
    { "setTitle", window_setTitle, 1, 0 },
    { "getTitle", window_getTitle, 0, 0 },
    { "setFullscreen", window_setFullscreen, 1, 0 },
    { NULL }
};

void micro_open_window(tea_State* T)
{
    tea_create_submodule(T, "window", reg);
}