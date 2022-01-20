#pragma once
#include "../math/mathUtils.hpp"
class Camera
{
public:
    PointF3 transformMatrix[4] = {0};
    PointF3 translationMatrix[4] = {0};
    PointF3 rotationMatrix[4] = {0};
    float zNear;
    Camera(float zNear = 200);
};
