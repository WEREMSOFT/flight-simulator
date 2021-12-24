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
    float zPosition = 1.f;
    auto square = createCubeShape(zPosition);

    Sprite checker({320, 240}, 5, {0x77, 0, 0x77}, {0, 0x77, 0x77});
    Sprite map("assets/color.png");
    float rotationZ = 0;
    float rotationX = 0.0;
    float rotationY = 0.0;
    float translationX = 0, translationY = 0;
    while (glfwGetKey(graphics->window, GLFW_KEY_ESCAPE) != GLFW_PRESS)
    {
        deltaTime = getDeltaTime();
        graphics->imageData.clear();

        if (glfwGetKey(graphics->window, GLFW_KEY_UP))
        {
            zPosition += 10.f * deltaTime;
        }

        if (glfwGetKey(graphics->window, GLFW_KEY_DOWN))
        {
            zPosition -= 10.f * deltaTime;
        }

        if (glfwGetKey(graphics->window, GLFW_KEY_LEFT))
        {
            translationX += 50.f * deltaTime;
        }

        if (glfwGetKey(graphics->window, GLFW_KEY_RIGHT))
        {
            translationX -= 50.f * deltaTime;
        }

        if (glfwGetKey(graphics->window, GLFW_KEY_A))
        {
            rotationY -= .5f * deltaTime;
        }

        if (glfwGetKey(graphics->window, GLFW_KEY_D))
        {
            rotationY += .5f * deltaTime;
        }

        if (glfwGetKey(graphics->window, GLFW_KEY_W))
        {
            rotationX -= .5f * deltaTime;
        }

        if (glfwGetKey(graphics->window, GLFW_KEY_S))
        {
            rotationX += .5f * deltaTime;
        }

        square.translate({translationX, 0.f, zPosition});

        checker.draw(graphics->imageData);
        graphics->imageData.printString((PointI){160, 120}, "X");

        // rotationY += 0.5 * deltaTime;
        // rotationX += 0.5 * deltaTime;
        // rotationZ += 0.5 * deltaTime;
        square.rotate(rotationX, rotationY, rotationZ);
        square.draw(graphics->imageData);
        printFPS();

        graphics->render();
        glfwPollEvents();
    }
}

Shape Program::createCubeShape(float zPosition)
{
    Shape shape(8);

    const int cubeSize = 50;

    shape.vertices.emplace_back((PointF3){-cubeSize, -cubeSize, cubeSize, 1.f});
    shape.vertices.emplace_back((PointF3){cubeSize, -cubeSize, cubeSize, 1.f});
    shape.vertices.emplace_back((PointF3){cubeSize, cubeSize, cubeSize, 1.f});
    shape.vertices.emplace_back((PointF3){-cubeSize, cubeSize, cubeSize, 1.f});

    shape.vertices.emplace_back((PointF3){-cubeSize, -cubeSize, -cubeSize, 1.f});
    shape.vertices.emplace_back((PointF3){cubeSize, -cubeSize, -cubeSize, 1.f});
    shape.vertices.emplace_back((PointF3){cubeSize, cubeSize, -cubeSize, 1.f});
    shape.vertices.emplace_back((PointF3){-cubeSize, cubeSize, -cubeSize, 1.f});

    shape.vertexIndex.emplace_back(std::array<uint32_t, 3>({0, 1, 2}));
    shape.vertexIndex.emplace_back(std::array<uint32_t, 3>({0, 2, 3}));

    shape.vertexIndex.emplace_back(std::array<uint32_t, 3>({4, 5, 6}));
    shape.vertexIndex.emplace_back(std::array<uint32_t, 3>({4, 7, 6}));

    shape.vertexIndex.emplace_back(std::array<uint32_t, 3>({4, 0, 3}));
    shape.vertexIndex.emplace_back(std::array<uint32_t, 3>({4, 7, 3}));

    shape.vertexIndex.emplace_back(std::array<uint32_t, 3>({4, 0, 1}));
    shape.vertexIndex.emplace_back(std::array<uint32_t, 3>({4, 5, 1}));

    shape.vertexIndex.emplace_back(std::array<uint32_t, 3>({2, 5, 6}));
    shape.vertexIndex.emplace_back(std::array<uint32_t, 3>({2, 5, 1}));

    shape.vertexIndex.emplace_back(std::array<uint32_t, 3>({3, 2, 6}));
    shape.vertexIndex.emplace_back(std::array<uint32_t, 3>({3, 7, 6}));

    shape.transformedVertices.resize(shape.vertices.size());
    shape.projectedVertices.resize(shape.vertices.size());

    shape.translate({0.f, 0.f, zPosition});

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