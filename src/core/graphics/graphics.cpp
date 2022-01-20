#include "graphics.hpp"
#include <iostream>
#include "../shader/shader.hpp"
#include "imageData/imagedata.hpp"
#include "../../imgui/imgui.h"
#include "../../imgui/imgui_impl_glfw.h"
#include "../../imgui/imgui_impl_opengl3.h"

void Graphics::initImGUI()
{
    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();

    // GL 3.0 + GLSL 130
    const char *glsl_version = "#version 130";

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);
}

Graphics::Graphics(int32_t width, int32_t height) : imageData({width, height})
{

    glfwInit();
    GLFWmonitor *monitor = glfwGetPrimaryMonitor();
    const GLFWvidmode *mode = glfwGetVideoMode(monitor);

    glfwWindowHint(GLFW_RED_BITS, mode->redBits);
    glfwWindowHint(GLFW_GREEN_BITS, mode->greenBits);
    glfwWindowHint(GLFW_BLUE_BITS, mode->blueBits);
    glfwWindowHint(GLFW_REFRESH_RATE, mode->refreshRate);
    // glfwWindowHint(GLFW_DOUBLEBUFFER, GL_FALSE);

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    this->window = glfwCreateWindow(mode->width, mode->height, "Frame Buffer", monitor, NULL);
    glfwMakeContextCurrent(this->window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        exit(-1);
    }
    imageData.init();

    float ratioX = ((float)this->imageData.size.x / (float)this->imageData.size.y) / ((float)mode->width / (float)mode->height);
    float ratioY = 1.0;

    // set up vertex data (and buffer(s)) and configure vertex attributes
    // ------------------------------------------------------------------
    float vertices[] = {
        // positions       // texture coords
        ratioX * 1.0f, ratioY * 1.0f, 0.0f, 1.0f, 0.0f,   // top right
        ratioX * 1.0f, ratioY * -1.0f, 0.0f, 1.0f, 1.0f,  // bottom right
        ratioX * -1.0f, ratioY * -1.0f, 0.0f, 0.0f, 1.0f, // bottom left
        ratioX * -1.0f, ratioY * 1.0f, 0.0f, 0.0f, 0.0f   // top left
    };

    unsigned int indices[] = {
        0, 1, 3, // first triangle
        1, 2, 3  // second triangle
    };

    this->shaderProgram = std::make_unique<Shader>("assets/shaders/default.vs", "assets/shaders/default.fs");
    this->shaderProgram.get()->use();

    unsigned int VBO, EBO;
    glGenVertexArrays(1, &this->VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(this->VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);
    // texture coord attribute
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glfwSetCursorPos(this->window, 0, 0);
    initImGUI();
}

Graphics::~Graphics()
{
    std::cout << "destroying graphics" << std::endl;
    glfwSetWindowShouldClose(this->window, true);
    glfwDestroyWindow(this->window);
    glfwTerminate();
}
void Graphics::newFrame(void)
{
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
    imageData.clearZBuffer();
}

void Graphics::endFrame(void)
{

    glClearColor(0, 0, 0, 1.f);
    glClear(GL_COLOR_BUFFER_BIT);

    imageData.updateTexture();
    // render container
    glBindVertexArray(this->VAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    glfwSwapBuffers(window);
}