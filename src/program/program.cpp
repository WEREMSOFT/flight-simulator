#include "program.hpp"
#include "../core/graphics/sprite/sprite.hpp"
#include "shape/shape.hpp"
#include "bouncingPoint/bouncingPoint.hpp"
#include <algorithm>
#include <cmath>
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
    std::cout << "destroying program" << std::endl;
}

void Program::update(void)
{
    Graphics *graphics = this->graphics.get();

    // auto square = createSquareShape(2.f);
    auto square = createCubeShape();

    Sprite checker({320, 240}, 5, {0x77, 0, 0x77}, {0, 0x77, 0x77});
    Sprite map("assets/color.png");
    float angle = 0;
    while (glfwGetKey(graphics->window, GLFW_KEY_ESCAPE) != GLFW_PRESS)
    {
        deltaTime = getDeltaTime();
        graphics->imageData.clear();

        checker.draw(graphics->imageData);
        // map.drawClipped(graphics->imageData);
        graphics->imageData.printString((PointI){160, 120}, "X");
        // updatePoints(points);

        angle += 0.5 * deltaTime;
        square.rotateZ(angle);
        square.draw(graphics->imageData);
        printFPS();

        graphics->render();
        glfwPollEvents();
    }
}

Shape Program::createSquareShape(float distance = 1.0)
{
    Shape shape(4);

    shape.vertices.emplace_back((PointF3){-50, -50, distance, 1.f});
    shape.vertices.emplace_back((PointF3){50.f, -50, distance, 1.f});
    shape.vertices.emplace_back((PointF3){50.f, 50.f, distance, 1.f});
    shape.vertices.emplace_back((PointF3){-50, 50.f, distance, 1.f});

    shape.transformedVertices.resize(shape.vertices.size());

    shape.translate({160, 120, 0});
    shape.scale({.5, .5, .5});

    return shape;
}

Shape Program::createCubeShape()
{
    Shape shape(8);

    shape.vertices.emplace_back((PointF3){-10, -10, 20.f, 1.f});
    shape.vertices.emplace_back((PointF3){10.f, -10, 20.f, 1.f});
    shape.vertices.emplace_back((PointF3){10.f, 10.f, 20.f, 1.f});
    shape.vertices.emplace_back((PointF3){-10, 10.f, 20.f, 1.f});

    shape.vertices.emplace_back((PointF3){-10, -10, -10.f, 1.f});
    shape.vertices.emplace_back((PointF3){10.f, -10, -10.f, 1.f});
    shape.vertices.emplace_back((PointF3){10.f, 10.f, -10.f, 1.f});
    shape.vertices.emplace_back((PointF3){-10, 10.f, -10.f, 1.f});

    shape.transformedVertices.resize(shape.vertices.size());

    shape.translate({0, 0, 5});
    shape.scale({1., 1., 1.});

    return shape;
}

void Program::drawLines(std::vector<BouncingPointF> points)
{
    for (auto i = 0; i < points.size(); i += 3)
    {
        const auto &point1 = points[i];
        const auto &point2 = points[i + 1];
        const auto &point3 = points[i + 2];

        graphics->imageData.drawLine({(int)point1.x, (int)point1.y}, {(int)point2.x, (int)point2.y});
        graphics->imageData.drawLine({(int)point2.x, (int)point2.y}, {(int)point3.x, (int)point3.y});
        graphics->imageData.drawLine({(int)point3.x, (int)point3.y}, {(int)point1.x, (int)point1.y});
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