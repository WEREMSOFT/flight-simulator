#include "camera.hpp"
#include "../../core/math/mathUtils.hpp"
#include <iostream>

Camera::Camera(float pZNear)
{
    zNear = pZNear;

    MathUtils::setMatrixAsIdentity(transformMatrix);
    MathUtils::setMatrixAsIdentity(rotationMatrix);
    MathUtils::setMatrixAsIdentity(translationMatrix);
}

void Camera::moveForward(float distance)
{
    PointF3 forwardVector = {-sinf(rotation.y), 0, cosf(rotation.y)};
    std::cout << rotation.y << " # " << forwardVector.x << " # " << forwardVector.y << " # " << forwardVector.z << std::endl;
    position = position - forwardVector.scale(distance);
    translate(position);
}

void Camera::recalculateTransformMatrix()
{
    MathUtils::setMatrixAsIdentity(transformMatrix);
    MathUtils::multiplyMatrix(transformMatrix, translationMatrix);
    MathUtils::multiplyMatrix(transformMatrix, rotationMatrix);
    MathUtils::multiplyMatrix(transformMatrix, scaleMatrix);
}