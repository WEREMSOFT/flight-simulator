#pragma once
#include "../../core/graphics/imageData/imagedata.hpp"
class BouncingPointF : public PointF
{
    PointF speed;
    PointF bounds;

public:
    BouncingPointF(double pX, double pY, PointF pSpeed, const PointI &pBounds);
    ~BouncingPointF();
    void update(float deltaTime);
};