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

template <typename T>
struct Point2
{
    T x, y;
};

template <typename T>
struct Point3
{
    T x, y, z, w;
};

typedef Point3<float> PointF3;
typedef Point2<int32_t> PointI;
typedef Point2<uint32_t> PointU;
typedef Point2<float> PointF;

class ImageData
{
public:
    ImageData(PointI size);
    ~ImageData();

    void init(void);
    void updateTexture(void);
    bool putPixel(PointI point, Color color = (Color){0xFF, 0xFF, 0xFF});
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

    std::vector<Color> data;

private:
    uint32_t textureId;
    void createTexture(void);
};