#pragma once
#include <cstdint>
#include <string>
#include <vector>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <array>

#include "../../math/vector/vector3.hpp"

typedef struct
{
    unsigned char r, g, b;
} Color;

template <typename T>
struct Point2
{
    T x, y;
};

typedef Point3<float> PointF3;
typedef Point3<int32_t> PointI;
typedef Point2<uint32_t> PointU;
typedef Point2<float> PointF;

typedef union
{
    PointF3 p;
    float a[4];
} Mat4Elem;

class ImageData
{
public:
    ImageData(PointI size);
    ~ImageData();

    void init(void);
    void updateTexture(void);
    bool putPixel(PointI point, Color color = {0xFF, 0xFF, 0xFF});
    void drawCircle(PointI center, double radius, Color color = {0xFF, 0xFF, 0xFF});
    void drawCircleFill(PointI center, double radius, Color color = {0xFF, 0xFF, 0xFF});
    void drawSquare(PointI topLeftCorner, PointI size, Color color = {0xFF, 0xFF, 0xFF});
    void clear(void);
    void clearZBuffer(void);
    void clearColor(Color color);
    void clearTransparent(void);
    void drawCharacter(PointI topLeftCorner, unsigned int letter, const Color color = {0xFF, 0xFF, 0xFF});
    void printFontTest(void);
    void printString(PointI topLeftCorner, const std::string &string, const Color color = {0xFF, 0xFF, 0xFF});
    void drawLine(PointI pointA, PointI pointB, Color color = {0xFF, 0xFF, 0xFF});
    void drawLineZ(PointI pointA, PointI pointB, std::array<PointI, 3> triangle, Color color);
    void drawSquareFill(PointI topLeftCorner, PointI size, Color color = {0xFF, 0xFF, 0xFF});
    void drawZBuffer(PointI position);
    Color getPixel(PointU position);
    bool putPixelZbuffer(PointI point, int32_t color);
    int32_t getPixelZBuffer(PointI position);

    PointI size;
    int bufferSize;
    int elementCount;

    std::vector<Color> data;
    std::vector<int32_t> zBuffer;

private:
    uint32_t textureId;
    void createTexture(void);
};