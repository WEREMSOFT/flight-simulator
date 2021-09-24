#pragma once
#include <cstdint>
#include <string>
#include <vector>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

typedef struct
{
    unsigned char r, g, b;
} Color;

typedef struct
{
    int32_t x;
    int32_t y;
} PointI;

typedef struct
{
    unsigned int x;
    unsigned int y;
} PointU;

typedef struct
{
    double x;
    double y;
} PointF;

class ImageData
{
public:
    ImageData(PointI size);
    ~ImageData();

    void init(void);
    void updateTexture(void);
    void putPixel(PointI point, Color color = (Color){0xFF, 0xFF, 0xFF});
    void drawCircle(PointI center, double radious, Color color = (Color){0xFF, 0xFF, 0xFF});
    void drawCircleFill(PointI center, double radious, Color color = (Color){0xFF, 0xFF, 0xFF});
    void drawSquare(PointI topLeftCorner, PointI size, Color color = (Color){0xFF, 0xFF, 0xFF});
    void clear(void);
    void clearColor(Color color);
    void clearTransparent(void);
    void drawCharacter(PointI topLeftCorner, unsigned int letter, const Color color = (Color){0xFF, 0xFF, 0xFF});
    void printFontTest(void);
    void printString(PointI topLeftCorner, const std::string &string, const Color color = (Color){0xFF, 0xFF, 0xFF});
    void drawLine(PointI pointA, PointI pointB, Color color = (Color){0xFF, 0xFF, 0xFF});
    void drawSquareFill(PointI topLeftCorner, PointI size, Color color = (Color){0xFF, 0xFF, 0xFF});

    Color getPixel(PointU point);
    PointI size;

    int bufferSize;
    int elementCount;

private:
    std::vector<Color> data;
    uint32_t textureId;
    void createTexture(void);
};