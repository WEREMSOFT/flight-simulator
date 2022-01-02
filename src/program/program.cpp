#include "program.hpp"
#include "../core/graphics/sprite/sprite.hpp"
#include "shape/shape.hpp"
#include "bouncingPoint/bouncingPoint.hpp"
#include <algorithm>
#include <cmath>
#include <vector>
#include "../imgui/imgui.h"
#include "../imgui/imgui_impl_glfw.h"
#include "../imgui/imgui_impl_opengl3.h"

#define DEMO_MODE 0
#define CREATE_CHECKER 0

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
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    std::cout << "destroying program" << std::endl;
}

void Program::update(void)
{
    Graphics *graphics = this->graphics.get();

    float zPosition = 200.f;
    Shape shape(1);
    Shape::appendPiramid(shape, 60, 100, {0, -40, 0});
    Shape::appendCube(shape, 45, {0, 0, 0});

    // auto square = Shape::createCube(50, zPosition);
    // auto shape = Shape::createPyramid(50, 100, 130);
    // Shape shape(1);
    // Shape::appendPiramid(shape, 50, 100, {0, 0, -100});
    // Shape::appendPiramid(shape, 50, 100, {0, 0, 0});
    // Shape::appendPiramid(shape, 50, 100, {0, 0, 100});

#if CREATE_CHECKER
    auto tempChecker = Sprite::createChecker({320, 240}, 20, {0x77, 0x55, 0x33}, {0x77, 0, 0});
#else
    auto tempChecker = Sprite::createSplit({320, 240}, 115, {0x77, 0x77, 0xAA}, {0, 0x77, 0});
#endif
    auto checker = tempChecker.get();
    float rotationZ = 0.0;
    float rotationX = 0.0;
    float rotationY = 0.91;
    float translationX = 0, translationY = 0;
    while (glfwGetKey(graphics->window, GLFW_KEY_ESCAPE) != GLFW_PRESS)
    {
        graphics->newFrame();

        deltaTime = getDeltaTime();

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
            translationX -= 70.f * deltaTime;
        }

        if (glfwGetKey(graphics->window, GLFW_KEY_RIGHT))
        {
            translationX += 70.f * deltaTime;
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

#if DEMO_MODE
        rotationX += 1.0f * deltaTime;
        rotationY += 1.0f * deltaTime;
        rotationZ += 1.0f * deltaTime;
#endif
        shape.translate({translationX, 0.f, zPosition});

        checker->draw(graphics->imageData);

        shape.rotate(rotationX, rotationY, rotationZ);
        shape.draw(graphics->imageData);
        printFPS();

        {
            static float f = 0.0f;
            static int counter = 0;

            ImGui::Begin("Hello, world!"); // Create a window called "Hello, world!" and append into it.

            ImGui::Text("This is some useful text."); // Display some text (you can use a format strings too)

            ImGui::SliderFloat("float", &f, 0.0f, 1.0f); // Edit 1 float using a slider from 0.0f to 1.0f

            if (ImGui::Button("Button")) // Buttons return true when clicked (most widgets return true when edited/activated)
                counter++;
            ImGui::SameLine();
            ImGui::Text("counter = %d", counter);

            ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
            ImGui::End();
        }

        graphics->endFrame();
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