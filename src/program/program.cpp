#include "program.hpp"
#include <algorithm>
#include <cmath>
#include "bouncingPoint/bouncingPoint.hpp"
#include <vector>

Program::Program()
{
    float aspectRatio = 4.0 / 3.0;
    int wideSize = 320;
    deltaTime = 0;
    graphics = std::make_unique<Graphics>(wideSize, wideSize / aspectRatio);
    glfwSwapInterval(0);
}

Program::~Program()
{
    std::cout << "destroing program" << std::endl;
}

void Program::update(void)
{
    Graphics *graphics = this->graphics.get();
    BouncingPointF lineStart(10.0, 10.0, {100.f, 200.f}, graphics->imageData.size);
    BouncingPointF lineEnd(100.0, 100.0, {-100.f, -200.f}, graphics->imageData.size);

    std::vector<std::reference_wrapper<BouncingPointF>> points;

    points.reserve(2);

    points.emplace_back(std::ref(lineStart));
    points.emplace_back(std::ref(lineEnd));

    while (glfwGetKey(graphics->window, GLFW_KEY_ESCAPE) != GLFW_PRESS)
    {
        deltaTime = getDeltaTime();

        for (auto p : points)
        {
            p.get().update(deltaTime);
        }

        graphics->imageData.clear();
        graphics->imageData.printString((PointI){0, 100}, "Hello World!!");
        graphics->imageData.drawCircle((PointI){100, 100}, 50, (Color){255, 255, 0});

        graphics->imageData.drawLine({(int)lineStart.x, (int)lineStart.y}, {(int)lineEnd.x, (int)lineEnd.y});
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
    fpsHistory[counter] = (1 / deltaTime);

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