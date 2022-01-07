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
#include "../core/gameObject/gameObject.hpp"

#define CREATE_CHECKER 0

void showGUI(Shape &shape, bool &demoMode)
{
    static bool showDebugWindow = false;
    if (ImGui::BeginMainMenuBar())
    {
        if (ImGui::BeginMenu("Debug"))
        {
            ImGui::MenuItem("Show Debug Window", "", &showDebugWindow);
            ImGui::EndMenu();
        }
        ImGui::EndMainMenuBar();
        if (showDebugWindow)
        {
            ImGui::Begin("Debug");
            ImGui::Checkbox("Show Wireframe", &shape.wireFrame);
            ImGui::Checkbox("Disable BackFaceCuling", &shape.backFaceCulingDisabled);
            ImGui::Checkbox("Show Vertex Number", &shape.showVertexNumber);
            ImGui::Checkbox("Draw Normals", &shape.drawNormals);
            ImGui::Checkbox("Demo Mode", &demoMode);
            ImGui::End();
        }
    }
}

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
    GameObject parent;
    GameObject child;
    parent.children.emplace_back(child);

    parent.update(10.0);

    Graphics *graphics = this->graphics.get();
    bool demoMode = false;

    float zPosition = 200.f;
    Shape shape(1);
    Shape::appendPiramid(shape, 60, 100, {0, -40, 0});
    Shape::appendCube(shape, 45, {0, 0, 0});

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

        if (demoMode)
        {

            rotationX += 1.0f * deltaTime;
            rotationY += 1.0f * deltaTime;
            rotationZ += 1.0f * deltaTime;
        }

        shape.translate({translationX, 0.f, zPosition});

        checker->draw(graphics->imageData);

        shape.rotate(rotationX, rotationY, rotationZ);
        shape.draw(graphics->imageData);
        printFPS();
        showGUI(shape, demoMode);

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
        this->graphics->imageData.printString({100, 10}, text, {0, 0xff, 0xff});
    }
}

double Program::getDeltaTime()
{
    static double lastUpdate = 0;

    double deltaTime = glfwGetTime() - lastUpdate;
    lastUpdate = glfwGetTime();

    return deltaTime;
}