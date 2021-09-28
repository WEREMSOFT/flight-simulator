#pragma once

class Animation
{
public:
    int frameWidth;
    int t;
    int currentFrame;
    int frameCount;
    float frameIncrement;
    float inverseFramerrate;
};