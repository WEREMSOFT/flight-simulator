#pragma once

#include <iostream>

class Shader
{
    uint32_t vertexShader;
    uint32_t fragmentShader;
    uint32_t shaderProgram;

public:
    Shader(std::string vertexShaderPath, std::string fragmentShaderPath);
    ~Shader();
    void setUniformValueMatrix4F(uint32_t shaderProgram, std::string uniformName, const float *value);
    void setUniformValueMatrix3F(uint32_t shaderProgram, std::string uniformName, const float *value);
    void setUniformValue3F(uint32_t shaderProgram, std::string uniformName, const float *value);
    void setUniformValueF(uint32_t shaderProgram, std::string uniformName, const float value);
    void setUniformValueI(uint32_t shaderProgram, std::string uniformName, const int value);
    uint32_t createFromFile(std::string fileName, uint32_t *vertexShader, int32_t shaderType);
    void use(void);
};