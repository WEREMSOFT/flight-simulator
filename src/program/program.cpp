#include "program.hpp"

Program::Program()
{
    float aspectRatio = 4.0 / 3.0;
    int wideSize = 800;
    this->graphics = new Graphics(wideSize, wideSize / aspectRatio);
    glfwSwapInterval(0);
}
Program::~Program()
{
    delete this->graphics;
    std::cout << "destroing program" << std::endl;
}

void Program::update(void)
{
    int32_t x = 0;
    while (glfwGetKey(graphics->window, GLFW_KEY_ESCAPE) != GLFW_PRESS)
    {
        // this->graphics->imageData.clearColor((Color){.r = 255, .g = 255, .b = 0});
        this->graphics->imageData.clear();
        this->graphics->imageData.putPixel((PointI){.x = 100, .y = 100}, (Color){.r = 255, .g = 0, .b = 0});
        this->graphics->imageData.printFontTest();
        this->graphics->imageData.drawCircle((PointI){100, 100}, 50, (Color){255, 255, 0});
        this->graphics->render();

        glfwPollEvents();
    }
}