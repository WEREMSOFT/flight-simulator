#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include "sprite.hpp"
#include <iostream>
#include <memory>
#include <algorithm>

Sprite::Sprite(std::string fileName)
{
    int nrChannels;
    imageData = std::make_unique<ImageData>((PointI){0});

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

Sprite::Sprite(PointI size, int checkerWidth, Color color1, Color color2)
{
    Color currentColor = color1;
    imageData = std::make_unique<ImageData>((PointI){0});
    auto im = imageData.get();

    im->size = size;
    im->data = std::vector<Color>(im->size.x * im->size.y, (Color){0});

    for (int y = 0; y < im->size.y; y++)
    {
        for (int x = 0; x < im->size.x; x++)
        {
            if ((y / checkerWidth + x / checkerWidth) % 2)
            {
                currentColor = color1;
            }
            else
            {
                currentColor = color2;
            }
            im->data[x + y * size.x] = currentColor;
        }
    }
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
            pImageData.putPixel((PointI){static_cast<int32_t>(position.x) + i, static_cast<int32_t>(position.y) + j}, color);
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