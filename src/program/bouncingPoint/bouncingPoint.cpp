#include "bouncingPoint.hpp"
#include <iostream>
#include <algorithm>

BouncingPointF::BouncingPointF(double pX, double pY, PointF pSpeed, const PointI &pBounds)
{
    x = pX;
    y = pY;
    speed = pSpeed;
    bounds.x = pBounds.x;
    bounds.y = pBounds.y;
}

BouncingPointF::~BouncingPointF()
{
    std::cout << "destroying bouncing point " << std::endl;
}

void BouncingPointF::update(float deltaTime)
{
    x += speed.x * deltaTime;
    y += speed.y * deltaTime;

    if (x > bounds.x || x < 0)
        speed.x *= -1;
    if (y > bounds.y || y < 0)
        speed.y *= -1;

    x = std::min<double>(std::max<double>(0, x), bounds.x);
    y = std::min<double>(std::max<double>(0, y), bounds.y);
}