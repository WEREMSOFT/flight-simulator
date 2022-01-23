#pragma once
#include "../../core/math/mathUtils.hpp"
class Object3D
{
protected:
    PointF3 translationMatrix[4] = {0};
    PointF3 rotationMatrix[4] = {0};
    PointF3 scaleMatrix[4] = {0};
    virtual void recalculateTransformMatrix();

public:
    PointF3 position = {0};
    PointF3 rotation = {0};
    PointF3 transformMatrix[4] = {0};
    Object3D();
    void translate(PointF3);
    void scale(PointF3);
    void rotate(PointF3 rotation);
    virtual void update(void);
};