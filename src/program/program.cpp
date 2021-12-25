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

    float zPosition = 125.f;
    auto square = Shape::createCube(zPosition);

    Sprite checker({320, 240}, 20, {0x77, 0x77, 0x77}, {0, 0, 0});
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
            zPosition += 70.f * deltaTime;
        }

        if (glfwGetKey(graphics->window, GLFW_KEY_DOWN))
        {
            zPosition -= 70.f * deltaTime;
        }

        if (glfwGetKey(graphics->window, GLFW_KEY_LEFT))
        {
            translationX += 70.f * deltaTime;
        }

        if (glfwGetKey(graphics->window, GLFW_KEY_RIGHT))
        {
            translationX -= 70.f * deltaTime;
        }

        if (glfwGetKey(graphics->window, GLFW_KEY_A))
        {
            rotationY -= 1.5f * deltaTime;
        }

        if (glfwGetKey(graphics->window, GLFW_KEY_D))
        {
            rotationY += 1.5f * deltaTime;
        }

        if (glfwGetKey(graphics->window, GLFW_KEY_W))
        {
            rotationX -= 1.5f * deltaTime;
        }

        if (glfwGetKey(graphics->window, GLFW_KEY_S))
        {
            rotationX += 1.5f * deltaTime;
        }

        // rotationX += 1.5f * deltaTime;
        // rotationY += 1.5f * deltaTime;
        // rotationZ += 1.5f * deltaTime;

        square.translate({translationX, 0.f, zPosition});

        checker.draw(graphics->imageData);

        square.rotate(rotationX, rotationY, rotationZ);
        square.draw(graphics->imageData);
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
        this->graphics->imageData.printString((PointI){100, 10}, text, (Color){0, 0xff, 0xff});
    }
}

double Program::getDeltaTime()
{
    static double lastUpdate = 0;

    double deltaTime = glfwGetTime() - lastUpdate;
    lastUpdate = glfwGetTime();

    return deltaTime;
}