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
    PointF position;
    std::unique_ptr<ImageData> imageData;
    Sprite(std::string fileName);
    // Creates a checker boarrd for placeholding
    Sprite(PointI size, int checkerWidth, Color color1, Color color2);
    ~Sprite();

    void draw(ImageData &imageData);
    void drawClipped(ImageData &imageData);
    void drawTransparent(ImageData &ImageData);
    void drawTransparentClipped(ImageData &imageData);
    void drawTransparentAnimatedClipped(ImageData &imageData, double deltaTime);
};