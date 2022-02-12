#pragma once
#include "../../core/math/mathUtils.hpp"
class Object3D
{
protected:
    PointF translationMatrix[4] = {0};
    PointF rotationMatrix[4] = {0};
    PointF scaleMatrix[4] = {0};
    virtual void recalculateTransformMatrix();

public:
    PointF position = {0};
    PointF rotation = {0};
    PointF transformMatrix[4] = {0};
    Object3D();
    void translate(PointF);
    void scale(PointF);
    void rotate(PointF rotation);
    virtual void update(void);
};