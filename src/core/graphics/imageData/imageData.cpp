#include "imagedata.hpp"
#include "../../math/mathUtils.hpp"
#include <cmath>
#include <cstring>
#include <array>
#include <iostream>
#include <limits>

extern const char fonts[][5];

ImageData::ImageData(PointI pSize) : size(pSize)
{
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

bool ImageData::putPixelZbuffer(PointI point, ZBufferT color)
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

ZBufferT ImageData::getPixelZBuffer(PointI point)
{
    if (!(point.x > 0 &&
          point.y > 0 &&
          point.x < size.x &&
          point.y < size.y))
        return 0;
    int position = point.x + point.y * this->size.x;
    return this->zBuffer[position];
}

void ImageData::drawCircle(PointI center, int radius, Color color)
{
    for (int i = center.x - radius; i <= center.x + radius; i++)
    {
        for (int j = center.y - radius; j <= center.y + radius; j++)
        {
            if (floor(sqrt(pow(center.x - i, 2) + pow(center.y - j, 2))) == radius)
                this->putPixel({i, j}, color);
        }
    }
}

void ImageData::drawSquare(PointI topLeftCorner, PointI pSize, Color color)
{
    for (int i = topLeftCorner.x; i <= topLeftCorner.x + pSize.x; i++)
    {
        for (int j = topLeftCorner.y; j <= topLeftCorner.y + pSize.y; j++)
        {
            if (j == topLeftCorner.y || j == topLeftCorner.y + pSize.y || i == topLeftCorner.x || i == topLeftCorner.x + pSize.x)
                this->putPixel({i, j}, color);
        }
    }
}

void ImageData::drawCircleFill(PointI center, int radius, Color color)
{
    for (int i = center.x - radius; i <= center.x + radius; i++)
    {
        for (int j = center.y - radius; j <= center.y + radius; j++)
        {
            if (floor(sqrt(pow(center.x - i, 2) + pow(center.y - j, 2))) == radius)
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
    std::fill(zBuffer.begin(), zBuffer.end(), ZBUFFER_MAX);
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

ZBufferT getZForTriangle(PointI position, std::array<PointI, 3> triangle)
{
    PointI planeNormalI = (triangle[0] - triangle[1]).cross(triangle[0] - triangle[2]);
    PointF planeNormalF = {(float)planeNormalI.x, (float)planeNormalI.y, (float)planeNormalI.z};
    PointF planePoint = {(float)triangle[0].x, (float)triangle[0].y, (float)triangle[0].z};
    auto point = MathUtils::intersectionPoint({0, 0, 1}, {(float)position.x, (float)position.y, 0}, planeNormalF, planePoint);
    return -10.f * 1.f / point.z;
}

void ImageData::drawLineZ(PointI pointA, PointI pointB, std::array<PointI, 3> triangle, Color color)
{

    int dx = abs(pointB.x - pointA.x);
    int sx = pointA.x < pointB.x ? 1 : -1;
    int sy = -1;
    int err = dx > 0 ? dx / 2 : 0;
    int e2 = 0;

    while (true)
    {
        auto z = getZForTriangle(pointA, triangle);

        auto zBufferValue = getPixelZBuffer(pointA);
        if (z < zBufferValue)
        {
            putPixelZbuffer(pointA, z);
            // if (zBufferValue != ZBUFFER_MAX)
            // {
            // this->putPixel(pointA, {0xFF, 0, 0});
            // }
            // else
            // {
            this->putPixel(pointA, color);
            // }
        }

        if (pointA.x == pointB.x)
            break;

        e2 = err;
        if (e2 > -dx)
        {
            pointA.x += sx;
        }
        if (e2 < 0)
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
            this->putPixel({i, j}, color);
        }
    }
}

void ImageData::drawZBuffer(PointI position)
{
    for (int i = 0; i < size.x; i++)
    {
        for (int j = 0; j < size.y; j++)
        {

            // ZBufferT pixel = getPixelZBuffer({i, j});
            // Color *c1 = (Color *)&pixel;
            auto z = getPixelZBuffer({i, j});
            // auto normalizedZ = z != ZBUFFER_MAX ? ZBUFFER_MAX - z / ZBUFFER_MAX * 230 : 0;
            auto normalizedZ = z * 30000.f;
            auto pixel = (unsigned char)(normalizedZ);
            // auto pixel = MathUtils::map<uint64_t>(getPixelZBuffer({i, j}), 0, 255, 0, UINT64_MAX);
            // auto pixel = static_cast<unsigned char>(MathUtils::map<ZBufferT>(getPixelZBuffer({i, j}), 255, ZBUFFER_MAX));

            this->putPixel({i, j}, (Color){pixel, pixel, pixel});
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
    zBuffer = std::vector<ZBufferT>(size.x * size.y, 0);

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
