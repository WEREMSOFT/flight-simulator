#pragma once
#include "../object3D/object3d.hpp"
#include "../math/mathUtils.hpp"
class Camera : public Object3D
{
protected:
    void recalculateTransformMatrix();

public:
    float zNear;
    Camera(float zNear = 50);
};