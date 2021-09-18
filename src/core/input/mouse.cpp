#include "mouse.hpp"

static uint32_t mouseButtons[3] = {0};

bool input::isMouseButtonJustPressed(GLFWwindow *window, int mouseButton)
{
    int currentMouseState = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_1);

    if (mouseButtons[mouseButton] && currentMouseState)
        return false;

    mouseButtons[mouseButton] = currentMouseState;

    return currentMouseState;
}