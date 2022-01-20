#pragma once
#include "../math/mathUtils.hpp"
class Object3D
{
protected:
    PointF3 transformMatrix[4] = {0};
    PointF3 translationMatrix[4] = {0};
    PointF3 rotationMatrix[4] = {0};
    PointF3 scaleMatrix[4] = {0};
    void recalculateTransformMatrix();

public:
    Object3D();
    void translate(PointF3);
    void scale(PointF3);
    void rotate(float x, float y, float z);
};