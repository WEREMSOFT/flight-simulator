#pragma once
#include "imageData/imagedata.hpp"
#include "../shader/shader.hpp"
#include <memory>
#include <cstdint>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

class Graphics
{
public:
    ImageData imageData;

    Graphics(uint32_t width, uint32_t height);
    ~Graphics();
    void render();
    void updateMouseCoordinates();

    GLFWwindow *window;

private:
    uint32_t textureId;
    uint32_t VAO;
    std::unique_ptr<Shader> shaderProgram;
    PointI mousePosition;
    bool mouseRightDown;

    void createTexture(void);
};