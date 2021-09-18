#include "graphics.hpp"
#include <iostream>
#include "../shader/shader.hpp"
#include "imageData/imagedata.hpp"

Graphics::Graphics(uint32_t width, uint32_t height)
{
    this->imageData.size.x = width;
    this->imageData.size.y = height;

    glfwInit();
    GLFWmonitor *monitor = glfwGetPrimaryMonitor();

    const GLFWvidmode *mode = glfwGetVideoMode(monitor);

    glfwWindowHint(GLFW_RED_BITS, mode->redBits);
    glfwWindowHint(GLFW_GREEN_BITS, mode->greenBits);
    glfwWindowHint(GLFW_BLUE_BITS, mode->blueBits);
    glfwWindowHint(GLFW_REFRESH_RATE, mode->refreshRate);
    glfwWindowHint(GLFW_DOUBLEBUFFER, GL_FALSE);

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

    this->createTexture();

    this->shaderProgram = new Shader("assets/shaders/default.vs", "assets/shaders/default.fs");
    this->shaderProgram->use();

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

    glfwSetInputMode(this->window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);

    glfwSetCursorPos(this->window, 0, 0);
}

void Graphics::createTexture(void)
{

    glGenTextures(1, &this->textureId);
    glBindTexture(GL_TEXTURE_2D, this->textureId);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 3);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    this->imageData.bufferSize = this->imageData.size.x * this->imageData.size.y * sizeof(Color);
    this->imageData.data = (Color *)malloc(this->imageData.bufferSize);
    if (this->imageData.data == NULL)
    {
        fprintf(stderr, "Error allocating memory\n");
        exit(-1);
    }

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, this->imageData.size.x, this->imageData.size.y, 0, GL_RGB, GL_UNSIGNED_BYTE, this->imageData.data);
}

Graphics::~Graphics()
{
    glfwSetWindowShouldClose(this->window, true);
    free(this->imageData.data);
    glfwDestroyWindow(this->window);
    glfwTerminate();
}

void Graphics::render(void)
{
    // Update texture
    glBindTexture(GL_TEXTURE_2D, this->textureId);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, this->imageData.size.x, this->imageData.size.y, GL_RGB, GL_UNSIGNED_BYTE, this->imageData.data);

    // render container
    glBindVertexArray(this->VAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    glFlush();
}