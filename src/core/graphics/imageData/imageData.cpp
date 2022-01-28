#include "imagedata.hpp"
#include <cmath>
#include <cstring>
#include <array>
#include <iostream>
#include <limits>

extern char fonts[][5];

ImageData::ImageData(PointI pSize)
{
    size = pSize;
}

void ImageData::init()
{
    createTexture();
}

bool ImageData::putPixel(PointI point, Color color)
{
    if (!(point.x > 0 &&
          point.y > 0 &&
          point.x < size.x &&
          point.y < size.y))
        return false;
    int position = (point.x + point.y * this->size.x);
    this->data[position] = color;
    return true;
}

bool ImageData::putPixelZbuffer(PointI point, int32_t color)
{
    if (!(point.x > 0 &&
          point.y > 0 &&
          point.x < size.x &&
          point.y < size.y))
        return false;
    int position = (point.x + point.y * this->size.x);
    this->zBuffer[position] = color;
    return true;
}

Color ImageData::getPixel(PointU point)
{
    int position = point.x + point.y * this->size.x;
    return this->data[position];
}

uint64_t ImageData::getPixelZBuffer(PointI point)
{
    if (!(point.x > 0 &&
          point.y > 0 &&
          point.x < size.x &&
          point.y < size.y))
        return 0;
    int position = point.x + point.y * this->size.x;
    return this->zBuffer[position];
}

void ImageData::drawCircle(PointI center, double radious, Color color)
{
    for (int i = center.x - radious; i <= center.x + radious; i++)
    {
        for (int j = center.y - radious; j <= center.y + radious; j++)
        {
            if (floor(sqrt(pow(center.x - i, 2) + pow(center.y - j, 2))) == radious)
                this->putPixel({i, j}, color);
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
                this->putPixel({i, j}, color);
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
                this->putPixel({i, j}, color);
        }
    }
}

void ImageData::clear(void)
{
    memset(this->data.data(), 0, this->bufferSize);
}

void ImageData::clearZBuffer(void)
{
    std::fill(zBuffer.begin(), zBuffer.end(), INT32_MAX);
}

void ImageData::clearTransparent(void)
{
    const int limit = this->size.x * this->size.y;
    for (int i = 0; i < limit; i++)
    {
        this->data[i] = {0xFF, 0, 0xFF};
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
                this->putPixel({topLeftCorner.x + j, topLeftCorner.y + i}, color);
        }
    }
}

void ImageData::printFontTest(void)
{
    static Color color = {0xff, 0xff, 0xff};
    for (int i = 0; i < 255; i++)
    {
        this->drawCharacter({i * 6, 100}, i, color);
    }
}

void ImageData::printString(PointI topLeftCorner, const std::string &string, const Color color)
{
    size_t stringLen = string.length();
    for (int32_t i = 0; i < stringLen; i++)
    {
        int charOffset = string[i];
        this->drawCharacter({topLeftCorner.x + i * 6, topLeftCorner.y}, charOffset, color);
    }
}

// z = (z3(x-x1)(y-y2) + z1(x-x2)(y-y3) + z2(x-x3)(y-y1) - z2(x-x1)(y-y3) - z3(x-x2)(y-y1) - z1(x-x3)(y-y2)) / (  (x-x1)(y-y2) +   (x-x2)(y-y3) +   (x-x3)(y-y1) -   (x-x1)(y-y3) -   (x-x2)(y-y1) -   (x-x3)(y-y2))

int32_t getZForTriangle(PointI position, std::array<PointI, 3> triangle)
{
    auto x1 = triangle[0].x;
    auto x2 = triangle[1].x;
    auto x3 = triangle[2].x;

    auto y1 = triangle[0].y;
    auto y2 = triangle[1].y;
    auto y3 = triangle[2].y;

    auto z1 = triangle[0].z;
    auto z2 = triangle[1].z;
    auto z3 = triangle[2].z;

    auto x = position.x;
    auto y = position.y;

    auto divisor = ((x - x1) * (y - y2) + (x - x2) * (y - y3) + (x - x3) * (y - y1) - (x - x1) * (y - y3) - (x - x2) * (y - y1) - (x - x3) * (y - y2));

    int32_t z = (z3 * (x - x1) * (y - y2) + z1 * (x - x2) * (y - y3) + z2 * (x - x3) * (y - y1) - z2 * (x - x1) * (y - y3) - z3 * (x - x2) * (y - y1) - z1 * (x - x3) * (y - y2)) / (divisor != 0 ? divisor : 1);
    // auto z = (triangle[0].z + triangle[1].z + triangle[2].z) / 3;
    return z;
}

void ImageData::drawLineZ(PointI pointA, PointI pointB, std::array<PointI, 3> triangle, Color color)
{

    int dx = abs(pointB.x - pointA.x), sx = pointA.x < pointB.x ? 1 : -1;
    int dy = abs(pointB.y - pointA.y), sy = pointA.y < pointB.y ? 1 : -1;
    int err = (dx > dy ? dx : -dy) / 2, e2;

    while (true)
    {
        auto z = getZForTriangle(pointA, triangle);

        auto zBufferValue = getPixelZBuffer(pointA);
        if (z < zBufferValue)
        {
            putPixelZbuffer(pointA, z);
            if (!this->putPixel(pointA, color))
                return;
        }

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

void ImageData::drawLine(PointI pointA, PointI pointB, Color color)
{

    int dx = abs(pointB.x - pointA.x), sx = pointA.x < pointB.x ? 1 : -1;
    int dy = abs(pointB.y - pointA.y), sy = pointA.y < pointB.y ? 1 : -1;
    int err = (dx > dy ? dx : -dy) / 2, e2;

    while (true)
    {
        if (!this->putPixel(pointA, color))
            return;

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
            this->putPixel({i, j}, color);
        }
    }
}

void ImageData::drawZBuffer(PointI position)
{
    auto ratio = 255.f / 1000.f;
    for (int i = 0; i < size.x; i++)
    {
        for (int j = 0; j < size.y; j++)
        {

            auto pixel = static_cast<unsigned char>(getPixelZBuffer({i, j}) * ratio);
            this->putPixel({i, j}, {pixel, pixel, pixel});
        }
    }
}

void ImageData::createTexture(void)
{
    glGenTextures(1, &textureId);
    glBindTexture(GL_TEXTURE_2D, textureId);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 3);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    bufferSize = size.x * size.y * sizeof(Color);
    Color initialColor = {0};

    data = std::vector<Color>(size.x * size.y, initialColor);
    zBuffer = std::vector<uint64_t>(size.x * size.y, 0);

    glTexImage2D(GL_TEXTURE_2D,
                 0,
                 GL_RGB,
                 size.x,
                 size.y,
                 0,
                 GL_RGB,
                 GL_UNSIGNED_BYTE,
                 data.data());
}

void ImageData::updateTexture(void)
{
    glBindTexture(GL_TEXTURE_2D, textureId);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, size.x, size.y, GL_RGB, GL_UNSIGNED_BYTE, data.data());
}

inline PointI pointFToPointI(PointF source)
{
    return {(int)source.x, (int)source.y};
}

inline PointU pointIToPointU(PointI source)
{
    return {(unsigned int)abs(source.x), (unsigned int)abs(source.y)};
}

inline PointU pointFToPointU(PointF source)
{
    return {(unsigned int)abs((int)round(source.x)), (unsigned int)abs((int)round(source.y))};
}
