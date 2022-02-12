#include "camera.hpp"
#include "../../core/math/mathUtils.hpp"
#include <iostream>

Camera::Camera(float pZNear)
{
    frustrum.x = -800;
    frustrum.z = 100;
    frustrum.w = 800;

    MathUtils::setMatrixAsIdentity(transformMatrix);
    MathUtils::setMatrixAsIdentity(rotationMatrix);
    MathUtils::setMatrixAsIdentity(translationMatrix);
}

void Camera::moveForward(float deltaTime)
{
    PointF forwardVector = {-sinf(rotation.y), 0, cosf(rotation.y)};
    position = position - forwardVector.scale(deltaTime * speed);
    translate(position);
}

void Camera::strafe(float deltaTime)
{
    float localRotation = rotation.y + M_PI / 2;
    PointF strafeVector = {-sinf(localRotation), 0, cosf(localRotation)};
    position = position + strafeVector.scale(deltaTime * speed);
    translate(position);
}

void Camera::recalculateTransformMatrix()
{
    MathUtils::setMatrixAsIdentity(transformMatrix);
    MathUtils::multiplyMatrix(transformMatrix, translationMatrix);
    MathUtils::multiplyMatrix(transformMatrix, rotationMatrix);
    MathUtils::multiplyMatrix(transformMatrix, scaleMatrix);
}

PointF Camera::getForwardVector()
{
    return {-sinf(rotation.y), 0, cosf(rotation.y)};
}