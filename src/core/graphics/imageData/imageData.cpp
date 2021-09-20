#include "imagedata.hpp"
#include <cmath>
#include <cstring>

extern char fonts[][5];

ImageData::ImageData()
{
}

ImageData::~ImageData()
{
}

void ImageData::putPixel(PointI point, Color color)
{
    int position = (point.x + point.y * this->size.x);
    this->data[position] = color;
}

Color ImageData::getPixel(PointU point)
{
    int position = point.x + point.y * this->size.x;
    return this->data[position];
}

void ImageData::drawCircle(PointI center, double radious, Color color)
{
    for (int i = center.x - radious; i <= center.x + radious; i++)
    {
        for (int j = center.y - radious; j <= center.y + radious; j++)
        {
            if (floor(sqrt(pow(center.x - i, 2) + pow(center.y - j, 2))) == radious)
                this->putPixel((PointI){i, j}, color);
        }
    }
}

void ImageData::drawSquare(PointI topLeftCorner, PointI size, Color color)
{
    for (int i = topLeftCorner.x; i <= topLeftCorner.x + size.x; i++)
    {
        for (int j = topLeftCorner.y; j <= topLeftCorner.y + size.y; j++)
        {
            if (j == topLeftCorner.y || j == topLeftCorner.y + size.y || i == topLeftCorner.x || i == topLeftCorner.x + size.x)
                this->putPixel((PointI){i, j}, color);
        }
    }
}

void ImageData::drawCircleFill(PointI center, double radious, Color color)
{
    for (int i = center.x - radious; i <= center.x + radious; i++)
    {
        for (int j = center.y - radious; j <= center.y + radious; j++)
        {
            if (floor(sqrt(pow(center.x - i, 2) + pow(center.y - j, 2))) == radious)
                this->putPixel((PointI){i, j}, color);
        }
    }
}

void ImageData::clear(void)
{
    memset(this->data, 0, this->bufferSize);
}

void ImageData::clearTransparent(void)
{
    const int limit = this->size.x * this->size.y;
    for (int i = 0; i < limit; i++)
    {
        this->data[i] = (Color){0xFF, 0, 0xFF};
    }
}

void ImageData::clearColor(Color color)
{
    const int limit = this->size.x * this->size.y;
    for (int i = 0; i < limit; i++)
    {
        this->data[i] = color;
    }
}

void ImageData::drawCharacter(PointI topLeftCorner, unsigned int letter, Color color)
{
    for (int i = 0; i < 5; i++)
    {
        for (int j = 0; j <= 8; j++)
        {
            if (fonts[letter][i] & (0b1000000 >> j))
                this->putPixel((PointI){topLeftCorner.x + j, topLeftCorner.y + i}, color);
        }
    }
}

void ImageData::printFontTest(void)
{
    static Color color = {0xff, 0xff, 0xff};
    for (int i = 0; i < 255; i++)
    {
        this->drawCharacter((PointI){i * 6, 100}, i, color);
    }
}

void ImageData::printString(PointI topLeftCorner, const std::string string, Color color)
{
    size_t stringLen = string.length();
    for (int32_t i = 0; i < stringLen; i++)
    {
        int charOffset = string[i];
        this->drawCharacter((PointI){topLeftCorner.x + i * 6, topLeftCorner.y}, charOffset, color);
    }
}

void ImageData::drawLine(PointI pointA, PointI pointB, Color color)
{

    int dx = abs(pointB.x - pointA.x), sx = pointA.x < pointB.x ? 1 : -1;
    int dy = abs(pointB.y - pointA.y), sy = pointA.y < pointB.y ? 1 : -1;
    int err = (dx > dy ? dx : -dy) / 2, e2;

    for (;;)
    {
        this->putPixel(pointA, color);
        if (pointA.x == pointB.x && pointA.y == pointB.y)
            break;
        e2 = err;
        if (e2 > -dx)
        {
            err -= dy;
            pointA.x += sx;
        }
        if (e2 < dy)
        {
            err += dx;
            pointA.y += sy;
        }
    }
}

void ImageData::drawSquareFill(PointI topLeftCorner, PointI size, Color color)
{
    for (int i = topLeftCorner.x; i <= topLeftCorner.x + size.x; i++)
    {
        for (int j = topLeftCorner.y; j <= topLeftCorner.y + size.y; j++)
        {
            this->putPixel((PointI){i, j}, color);
        }
    }
}

inline PointI pointFToPointI(PointF source)
{
    return (PointI){(int)source.x, (int)source.y};
}

inline PointU pointIToPointU(PointI source)
{
    return (PointU){(unsigned int)abs(source.x), (unsigned int)abs(source.y)};
}

inline PointU pointFToPointU(PointF source)
{
    return (PointU){(unsigned int)abs((int)round(source.x)), (unsigned int)abs((int)round(source.y))};
}
