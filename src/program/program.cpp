#include "program.hpp"
#include <cmath>

Program::Program()
{
    float aspectRatio = 4.0 / 3.0;
    int wideSize = 800;
    graphics = std::make_unique<Graphics>(wideSize, wideSize / aspectRatio);
    glfwSwapInterval(0);
}
Program::~Program()
{
    std::cout << "destroing program" << std::endl;
}

void Program::update(void)
{
    int32_t x = 0;
    Graphics *graphics = this->graphics.get();
    bool firstLoop = true;
    while (glfwGetKey(graphics->window, GLFW_KEY_ESCAPE) != GLFW_PRESS)
    {
        deltaTime = getDeltaTime();
        if (firstLoop)
        {
            graphics->imageData.clearColor((Color){255, 255, 0});
            firstLoop = false;
        }
        else
        {
            graphics->imageData.clear();
        }
        // graphics->imageData.putPixel((PointI){.x = 0, .y = 0}, (Color){.r = 255, .g = 0, .b = 0});
        // graphics->imageData.printFontTest();
        // graphics->imageData.drawCircle((PointI){100, 100}, 50, (Color){255, 255, 0});
        printFPS();
        graphics->render();

        glfwPollEvents();
    }
}

void Program::printFPS()
{
#define FPS_HISTORY 10
    static double fpsHistory[FPS_HISTORY] = {0};
    static int counter = 1;
    counter++;
    counter %= FPS_HISTORY;
    fpsHistory[counter] = (1 / this->deltaTime);

    int sum = 0;
    for (int i = 0; i < FPS_HISTORY; i++)
    {
        sum += fpsHistory[i];
    }

    float avg = sum / FPS_HISTORY;

    {
        char text[1000] = {0};
        snprintf(text, 1000, "fps: %d", (int)floor(avg));
        this->graphics->imageData.printString((PointI){100, 0}, text, (Color){0, 0xff, 0xff});
    }
}

double Program::getDeltaTime()
{
    static double lastUpdate = 0;

    double deltaTime = glfwGetTime() - lastUpdate;
    lastUpdate = glfwGetTime();

    return deltaTime;
}