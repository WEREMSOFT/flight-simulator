#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include "sprite.hpp"
#include <iostream>
#include <memory>
#include <algorithm>

Sprite::Sprite(std::string fileName)
{
    int nrChannels;
    PointI p = { 0, 0, 0, 0 };
    imageData = std::make_unique<ImageData>(p);

    auto im = imageData.get();
    auto vectorPointer = (Color *)stbi_load(fileName.c_str(), &im->size.x, &im->size.y, &nrChannels, 0);

    im->elementCount = im->size.x * im->size.y;
    im->bufferSize = im->elementCount * sizeof(char) * nrChannels;

    im->data.reserve(im->elementCount);

    std::copy(vectorPointer, vectorPointer + im->elementCount, std::back_inserter(im->data));

    if (vectorPointer == NULL)
    {
        std::cerr << "Error loading file " << fileName << ". Aborting." << std::endl;
        exit(-1);
    }
}

Sprite::Sprite(PointI size)
{
    PointI p = { 0 };
    imageData = std::make_unique<ImageData>(p);
    auto im = imageData.get();

    im->size = size;
    im->data = std::vector<Color>(im->size.x * im->size.y, {0xFF, 0, 0xFF});
}

std::unique_ptr<Sprite> Sprite::createChecker(PointI size, int checkerWidth, Color color1, Color color2)
{
    auto returnValue = std::make_unique<Sprite>(size);
    auto ret = returnValue.get();

    for (int y = 0; y < ret->imageData->size.y; y++)
    {
        Color currentColor = color1;
        for (int x = 0; x < ret->imageData->size.x; x++)
        {
            if ((y / checkerWidth + x / checkerWidth) % 2)
            {
                currentColor = color1;
            }
            else
            {
                currentColor = color2;
            }
            ret->imageData->data[x + y * size.x] = currentColor;
        }
    }
    return returnValue;
}

std::unique_ptr<Sprite> Sprite::createSplit(PointI size, int topHeight, Color color1, Color color2)
{
    auto returnValue = std::make_unique<Sprite>(size);
    auto ret = returnValue.get();

    for (int y = 0; y < ret->imageData->size.y; y++)
    {
        Color currentColor = y < topHeight ? color1 : color2;
        for (int x = 0; x < size.x; x++)
        {
            ret->imageData->data[x + y * size.x] = currentColor;
        }
    }
    return returnValue;
}

Sprite::~Sprite()
{
    std::cout << "destroying sprite" << std::endl;
}

void Sprite::draw(ImageData &pImageData)
{
    auto im = *imageData.get();
    for (int32_t i = 0; i < im.size.x; i++)
    {
        for (int32_t j = 0; j < im.size.y; j++)
        {
            Color color = im.data[j * im.size.x + i];
            pImageData.putPixel({static_cast<int32_t>(position.x) + i, static_cast<int32_t>(position.y) + j}, color);
        }
    }
}

void Sprite::drawClipped(ImageData &pImageData)
{
    auto im = *this->imageData.get();
    int clippedWidth = fmin(im.size.x, fmax(0, im.size.x - (im.size.x + position.x - pImageData.size.x)));
    int clippedHeight = fmin(im.size.y, fmax(0, im.size.y - (im.size.y + position.y - pImageData.size.y)));
    int clippedX = position.x < 0 ? -position.x : 0;
    int clippedY = position.y < 0 ? -position.y : 0;
    for (int i = clippedX; i < clippedWidth; i++)
    {
        for (int j = clippedY; j < clippedHeight; j++)
        {
            auto color = im.data[j * im.size.x + i];
            pImageData.putPixel({static_cast<int32_t>(position.x) + i, static_cast<int32_t>(position.y) + j}, color);
        }
    }
}

void Sprite::drawTransparent(ImageData &pImageData)
{
    auto im = *this->imageData.get();
    for (int i = 0; i < im.size.x; i++)
    {
        for (int j = 0; j < im.size.y; j++)
        {
            Color color = im.data[j * im.size.x + i];
            if (!(color.r == 0xFF && color.b == 0xFF && color.g == 0))
                pImageData.putPixel({static_cast<int32_t>(position.x) + i, static_cast<int32_t>(position.y) + j}, color);
        }
    }
}