#pragma once
#include "../object3D/object3d.hpp"
#include "../../core/math/vector/vector3.hpp"
class Camera : public Object3D
{
protected:
    void recalculateTransformMatrix();

public:
    float zNear;
    Camera(float zNear = 50);
    void moveForward(float distance);
};
