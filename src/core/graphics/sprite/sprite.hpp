#pragma once
#include "animation/animation.hpp"
#include "../imageData/imagedata.hpp"
#include <string>
#include <memory>

class Sprite
{
    Animation animation;

public:
    bool animated;
    PointF position = {0};
    std::unique_ptr<ImageData> imageData;
    Sprite(PointI size);
    Sprite(std::string fileName);
    // Creates a checker board for placeholding
    static std::unique_ptr<Sprite> createChecker(PointI size, int checkerWidth, Color color1, Color color2);
    // Creates a half and half board for placeholding
    static std::unique_ptr<Sprite> createSplit(PointI size, int topHeight, Color color1, Color color2);

    ~Sprite();

    void draw(ImageData &imageData);
    void drawClipped(ImageData &imageData);
    void drawTransparent(ImageData &ImageData);
    void drawTransparentClipped(ImageData &imageData);
    void drawTransparentAnimatedClipped(ImageData &imageData, double deltaTime);
};