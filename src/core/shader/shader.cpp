#include "shader.hpp"
#include <glad/glad.h>
#include <GLFW/glfw3.h>

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
    char *shaderCode = NULL;
    *vertexShader = glCreateShader(shaderType);

    FILE *fp = fopen(fileName.c_str(), "r");

    if (fp == NULL)
    {
        fprintf(stderr, "Error opening file\n");
        returnValue = -1;
        goto error_handler;
    }

    if (fseek(fp, 0, SEEK_END) != 0)
    {
        perror("Error obtaining the file size ");
        returnValue = -1;
        goto error_handler;
    }

    fileSize = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    shaderCode = (char *)malloc(fileSize + 1 * sizeof(char));

    if (shaderCode == NULL)
    {
        returnValue = -1;
        goto error_handler;
    }

    if (fread(shaderCode, sizeof(char), fileSize, fp) != fileSize)
    {
        fprintf(stderr, "Error opening shader %s\n", fileName.c_str());
        exit(-1);
    }

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
    if (shaderCode != NULL)
        free(shaderCode);
    if (fp != NULL)
        fclose(fp);

    return returnValue;
}

void Shader::use(void)
{
    glUseProgram(this->shaderProgram);
}