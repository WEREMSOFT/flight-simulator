#include "shader.hpp"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <fstream>
#include <streambuf>
#include <cstring>

Shader::Shader(std::string vertexShaderPath, std::string fragmentShaderPath)
{
    this->createFromFile(vertexShaderPath, &this->vertexShader, GL_VERTEX_SHADER);
    this->createFromFile(fragmentShaderPath, &this->fragmentShader, GL_FRAGMENT_SHADER);

    this->shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, this->vertexShader);
    glAttachShader(shaderProgram, this->fragmentShader);
    if (this->vertexShader == 0 || this->fragmentShader == 0)
    {
        fprintf(stderr, "error loading shadfer files\n");
        exit(-1);
    }
    glLinkProgram(this->shaderProgram);

    int success;

    glGetProgramiv(this->shaderProgram, GL_LINK_STATUS, &success);
    if (!success)
    {
        char infoLog[512];
        glGetProgramInfoLog(this->shaderProgram, 512, NULL, infoLog);
        printf("%s::%s - Error linking shader program: %s\n", __FILE__, __func__, infoLog);
        exit(-1);
    }

    glDeleteShader(this->fragmentShader);
    glDeleteShader(this->vertexShader);
}

Shader::~Shader()
{
}

void Shader::setUniformValueMatrix4F(uint32_t shaderProgram, std::string uniformName, const float *value)
{
}

void Shader::setUniformValueMatrix3F(uint32_t shaderProgram, std::string uniformName, const float *value)
{
}

void Shader::setUniformValue3F(uint32_t shaderProgram, std::string uniformName, const float *value)
{
}

void Shader::setUniformValueF(uint32_t shaderProgram, std::string uniformName, const float value)
{
}

void Shader::setUniformValueI(uint32_t shaderProgram, std::string uniformName, const int value)
{
}

uint32_t Shader::createFromFile(std::string fileName, uint32_t *vertexShader, int32_t shaderType)
{
    int returnValue = 0;
    size_t fileSize = 0;
    std::string shaderCode;
    *vertexShader = glCreateShader(shaderType);

    std::fstream shaderFile;

    shaderFile.open(fileName);

    if (shaderFile.fail())
    {
        std::cerr << "Error opening shader file \"" << fileName << "\": " << strerror(errno) << std::endl;
        std::exit(-1);
    }

    shaderFile.seekg(0, std::ios::end);

    fileSize = shaderFile.tellg();

    shaderCode.reserve(fileSize + 1);

    shaderFile.seekg(0, std::ios::beg);

    shaderCode.assign((std::istreambuf_iterator<char>(shaderFile)), std::istreambuf_iterator<char>());

    glShaderSource(*vertexShader, 1, (const char **)&shaderCode, NULL);

    glCompileShader(*vertexShader);

    int shaderCompilationSuccess;
    char infoLog[512];

    glGetShaderiv(*vertexShader, GL_COMPILE_STATUS, &shaderCompilationSuccess);

    if (!shaderCompilationSuccess)
    {
        glGetShaderInfoLog(*vertexShader, 512, NULL, infoLog);
        printf("%s::%s : vertex shader compilation failed. Error %s\n", __FILE__, __func__, infoLog);
        returnValue = -1;
        goto error_handler;
    }

error_handler:
    shaderFile.close();
    return returnValue;
}

void Shader::use(void)
{
    glUseProgram(this->shaderProgram);
}