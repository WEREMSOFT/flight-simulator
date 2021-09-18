#pragma once

#include <cstdint>

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
    uint32_t x;
    uint32_t y;
} PointU;

typedef struct
{
    double x;
    double y;
} PointF;

class ImageData
{
public:
    ImageData();
    ~ImageData();

    void putPixel(PointI point, Color color);
    void drawCircle(PointI center, double radious, Color color);
    void drawCircleFill(PointI center, double radious, Color color);
    void drawSquare(PointI topLeftCorner, PointI size, Color color);
    void clear(void);
    void clearColor(Color color);
    void clearTransparent(void);
    void drawCharacter(PointI topLeftCorner, unsigned int letter, Color color);
    void printFontTest(void);
    void printString(PointI topLeftCorner, char *string, Color color);
    void drawLine(PointI pointA, PointI pointB, Color color);
    void drawSquareFill(PointI topLeftCorner, PointI size, Color color);

    Color getPixel(PointU point);
    PointI size;

    int bufferSize;
    int elementCount;
    Color *data;

private:
};