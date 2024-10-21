#include <GLFW/glfw3.h>

#include "spxe.h"

#include <tea.h>

static void mouse_isVisible(tea_State* T)
{
    GLFWwindow* window = spxeGetWindow();
    bool visible = glfwGetInputMode(window, GLFW_CURSOR);
    tea_push_bool(T, visible);
}

static void mouse_setVisible(tea_State* T)
{
    bool visible = tea_check_bool(T, 0);
    GLFWwindow* window = spxeGetWindow();
    glfwSetInputMode(window, GLFW_CURSOR, !visible ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL);
    tea_push_nil(T);
}

static void mouse_setPosition(tea_State* T)
{
    double x = tea_check_number(T, 0);
    double y = tea_check_number(T, 1);
    GLFWwindow* window = spxeGetWindow();
    glfwSetCursorPos(window, x, y);
}

static const tea_Reg reg[] = {
    { "isVisible", mouse_isVisible, 0, 0 },
    { "setVisible", mouse_setVisible, 1, 0 },
    { "setPosition", mouse_setPosition, 2, 0 },
    { NULL, NULL }
};

void micro_open_mouse(tea_State* T)
{
    tea_create_submodule(T, "mouse", reg);
}