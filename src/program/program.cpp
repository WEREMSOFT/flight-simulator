#include "program.hpp"
#include "../core/graphics/sprite/sprite.hpp"
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
    std::vector<BouncingPointF> points;

    points.reserve(3);

    points.emplace_back(10.0, 10.0, (PointF){100.f, 200.f}, graphics->imageData.size);
    points.emplace_back(100.0, 100.0, (PointF){-100.f, -200.f}, graphics->imageData.size);
    points.emplace_back(110.0, 200.0, (PointF){-10.f, -20.f}, graphics->imageData.size);

    Sprite checker({320, 240}, 5, {0x77, 0, 0x77}, {0, 0x77, 0x77});
    Sprite map("assets/color.png");

    while (glfwGetKey(graphics->window, GLFW_KEY_ESCAPE) != GLFW_PRESS)
    {
        deltaTime = getDeltaTime();
        graphics->imageData.clear();
        checker.draw(graphics->imageData);
        map.drawClipped(graphics->imageData);

        graphics->imageData.printString((PointI){0, 100}, "Hello World!!");
        graphics->imageData.drawCircle((PointI){100, 100}, 50, (Color){255, 255, 0});

        for (auto &i : points)
        {
            i.update(deltaTime);
        }

        for (auto i = 0; i < points.size(); i += 3)
        {
            const auto &point1 = points[i];
            const auto &point2 = points[i + 1];
            const auto &point3 = points[i + 2];

            graphics->imageData.drawLine({(int)point1.x, (int)point1.y}, {(int)point2.x, (int)point2.y});
            graphics->imageData.drawLine({(int)point2.x, (int)point2.y}, {(int)point3.x, (int)point3.y});
            graphics->imageData.drawLine({(int)point3.x, (int)point3.y}, {(int)point1.x, (int)point1.y});
        }

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