#include "program.hpp"
#include "../core/graphics/sprite/sprite.hpp"
#include "shape/shape.hpp"
#include <algorithm>
#include <cmath>
#include <vector>
#include "../imgui/imgui.h"
#include "../imgui/imgui_impl_glfw.h"
#include "../imgui/imgui_impl_opengl3.h"
#include "../core/gameObject/gameObject.hpp"

void showGUI(Camera &camera, bool &demoMode, bool &drawZBuffer)
{
    static bool showDebugWindow = true;
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
            ImGui::Checkbox("Show Wireframe", &camera.wireframe);
            ImGui::Checkbox("BackFaceCuling", &camera.backFaceCulling);
            ImGui::Checkbox("Draw Normals", &camera.drawNormals);
            ImGui::Checkbox("Draw ZBuffer", &drawZBuffer);
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

void createHouse(Shape &house)
{
    house.difuseColor = {0xFF, 0, 0};
    Shape::appendPiramid(house, 60, 60, {0, -85, 0});
    Shape::appendCube(house, 45, {0, -45, 0});
    house.translate({0, 50, 800.f});
}

void createCross(Shape &cross)
{
    cross.difuseColor = {0, 0xFF, 0};
    Shape::appendCube(cross, 10, {0, 0, 0});
    Shape::appendCube(cross, 10, {23, 0, 0});
    Shape::appendCube(cross, 10, {-23, 0, 0});
    Shape::appendCube(cross, 10, {0, 23, 0});
    Shape::appendCube(cross, 10, {0, -23, 0});
    cross.translate({800.f, 0, 0});
}

void createPyramid(Shape &pyramid)
{
    pyramid.difuseColor = {0xFF, 0, 0xFF};
    Shape::appendPiramid(pyramid, 500, 300, {0});
    pyramid.translate({0, 0, -800});
}

void Program::update(void)
{
    GameObject parent;
    GameObject child;

    Graphics *graphics = this->graphics.get();
    bool demoMode = false;
    bool drawZBuffer = false;

    Shape house(1);
    Shape cross(1);
    Shape pyramidPurple(1);

    createHouse(house);
    createCross(cross);
    createPyramid(pyramidPurple);

    Camera camera;

    // auto tempChecker = Sprite::createSplit({320, 240}, 115, {0x77, 0x77, 0xAA}, {0, 0x77, 0});
    auto tempChecker = Sprite::createSplit({320, 240}, 115, {0, 0, 0}, {0, 0, 0});
    auto checker = tempChecker.get();
    float rotationX = 0;
    float cameraRotationY = 0;

    while (glfwGetKey(graphics->window, GLFW_KEY_ESCAPE) != GLFW_PRESS)
    {
        graphics->newFrame();

        deltaTime = getDeltaTime();

        if (glfwGetKey(graphics->window, GLFW_KEY_UP))
        {
            camera.moveForward(deltaTime);
        }

        if (glfwGetKey(graphics->window, GLFW_KEY_DOWN))
        {
            camera.moveForward(-deltaTime);
        }

        if (glfwGetKey(graphics->window, GLFW_KEY_LEFT))
        {
            cameraRotationY += 1.5f * deltaTime;
        }

        if (glfwGetKey(graphics->window, GLFW_KEY_RIGHT))
        {
            cameraRotationY -= 1.5f * deltaTime;
        }

        if (glfwGetKey(graphics->window, GLFW_KEY_Q))
        {
            cameraRotationY -= 1.5f * deltaTime;
        }

        if (glfwGetKey(graphics->window, GLFW_KEY_E))
        {
            cameraRotationY += 1.5f * deltaTime;
        }

        if (glfwGetKey(graphics->window, GLFW_KEY_A))
        {
            camera.strafe(-deltaTime);
        }

        if (glfwGetKey(graphics->window, GLFW_KEY_D))
        {
            camera.strafe(deltaTime);
        }

        if (glfwGetKey(graphics->window, GLFW_KEY_W))
        {
            rotationX += camera.speed * deltaTime;
            camera.translate({0, rotationX, 0});
        }

        if (glfwGetKey(graphics->window, GLFW_KEY_S))
        {
            rotationX -= camera.speed * deltaTime;
            camera.translate({0, rotationX, 0});
        }

        if (glfwGetKey(graphics->window, GLFW_KEY_R))
        {
            camera.translate({0, 0, 0});
        }

        camera.rotate({0, cameraRotationY, 0});
        camera.update();

        checker->draw(graphics->imageData);

        house.update();
        cross.update();
        pyramidPurple.update();

        house.draw(graphics->imageData, camera);
        cross.draw(graphics->imageData, camera);
        pyramidPurple.draw(graphics->imageData, camera);
        printFPS();
        showGUI(camera, demoMode, drawZBuffer);

        if (drawZBuffer)
        {
            graphics->imageData.drawZBuffer({0});
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