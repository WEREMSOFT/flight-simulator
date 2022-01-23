#include "object3d.hpp"

Object3D::Object3D()
{
    MathUtils::setMatrixAsIdentity(transformMatrix);
    MathUtils::setMatrixAsIdentity(rotationMatrix);
    MathUtils::setMatrixAsIdentity(scaleMatrix);
    MathUtils::setMatrixAsIdentity(translationMatrix);
}

void Object3D::translate(PointF3 translation)
{
    this->position = translation;
    translationMatrix[0] = {1, 0, 0, 0};
    translationMatrix[1] = {0, 1, 0, 0};
    translationMatrix[2] = {0, 0, 1, 0};
    translationMatrix[3] = {translation.x, translation.y, translation.z, 1};
}

void Object3D::scale(PointF3 scale)
{
    MathUtils::setMatrixAsIdentity(scaleMatrix);
    scaleMatrix[0].x = scale.x;
    scaleMatrix[1].y = scale.y;
    scaleMatrix[2].z = scale.z;
    scaleMatrix[3].w = 1.f;
}

void Object3D::recalculateTransformMatrix()
{
    MathUtils::setMatrixAsIdentity(transformMatrix);
    MathUtils::multiplyMatrix(transformMatrix, rotationMatrix);
    MathUtils::multiplyMatrix(transformMatrix, scaleMatrix);
    MathUtils::multiplyMatrix(transformMatrix, translationMatrix);
}

void Object3D::rotate(PointF3 pRotation)
{
    rotation = pRotation;
    PointF3 rotationMatrixX[4] = {0};

    rotationMatrixX[0] = {1, 0, 0, 0};
    rotationMatrixX[1] = {0, cosf(rotation.x), sinf(rotation.x), 0};
    rotationMatrixX[2] = {0, -sinf(rotation.x), cosf(rotation.x), 0};
    rotationMatrixX[3] = {0, 0, 0, 1};

    PointF3 rotationMatrixY[4] = {0};

    rotationMatrixY[0] = {cosf(rotation.y), 0, -sinf(rotation.y), 0};
    rotationMatrixY[1] = {0, 1, 0, 0};
    rotationMatrixY[2] = {sinf(rotation.y), 0, cosf(rotation.y), 0};
    rotationMatrixY[3] = {0, 0, 0, 1};

    PointF3 rotationMatrixZ[4] = {0};

    rotationMatrixZ[0] = {cosf(rotation.z), sinf(rotation.z), 0, 0};
    rotationMatrixZ[1] = {-sinf(rotation.z), cosf(rotation.z), 0, 0};
    rotationMatrixZ[2] = {0, 0, 1, 0};
    rotationMatrixZ[3] = {0, 0, 0, 1};

    MathUtils::setMatrixAsIdentity(rotationMatrix);
    MathUtils::multiplyMatrix(rotationMatrix, rotationMatrixZ);
    MathUtils::multiplyMatrix(rotationMatrix, rotationMatrixX);
    MathUtils::multiplyMatrix(rotationMatrix, rotationMatrixY);
}

void Object3D::update(void)
{
    recalculateTransformMatrix();
}