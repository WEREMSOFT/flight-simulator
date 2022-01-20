#include "camera.hpp"
#include "../math/mathUtils.hpp"

Camera::Camera(float pZNear)
{
    zNear = pZNear;

    MathUtils::setMatrixAsIdentity(transformMatrix);
    MathUtils::setMatrixAsIdentity(rotationMatrix);
    MathUtils::setMatrixAsIdentity(translationMatrix);
}

void Camera::recalculateTransformMatrix()
{
    MathUtils::setMatrixAsIdentity(transformMatrix);
    MathUtils::multiplyMatrix(transformMatrix, translationMatrix);
    MathUtils::multiplyMatrix(transformMatrix, rotationMatrix);
    MathUtils::multiplyMatrix(transformMatrix, scaleMatrix);
}