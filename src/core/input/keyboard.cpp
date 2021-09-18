#include "keyboard.hpp"
#include <GLFW/glfw3.h>

static uint32_t keys[255] = {0};

bool input::isKeyJustPressed(GLFWwindow *window, int key)
{
    int actualKeyState = glfwGetKey(window, key);

    if (keys[key] == GLFW_PRESS && actualKeyState == GLFW_PRESS)
        return false;

    keys[key] = actualKeyState;

    return actualKeyState == GLFW_PRESS;
}