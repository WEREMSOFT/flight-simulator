#include "shape.hpp"
#include <iostream>
#include <cmath>
#include <array>

void multiplyVertexByMatrix(PointF3 &vertDestination, PointF3 &vertSource, PointF3 mat[3])
{
    vertDestination.x = vertSource.x * mat[0].x +
                        vertSource.y * mat[1].x +
                        vertSource.z * mat[2].x +
                        vertSource.w * mat[3].x;

    vertDestination.y = vertSource.x * mat[0].y +
                        vertSource.y * mat[1].y +
                        vertSource.z * mat[2].y +
                        vertSource.w * mat[3].y;

    vertDestination.z = vertSource.x * mat[0].z +
                        vertSource.y * mat[1].z +
                        vertSource.z * mat[2].z +
                        vertSource.w * mat[3].z;

    vertDestination.w = vertSource.x * mat[0].w +
                        vertSource.y * mat[1].w +
                        vertSource.z * mat[2].w +
                        vertSource.w * mat[3].w;
}

void setMatrixAsIdentity(PointF3 matrix[4])
{
    for (int i = 0; i < 4; i++)
    {
        matrix[i] = {0};
    }
    matrix[0].x = 1.0;
    matrix[1].y = 1.0;
    matrix[2].z = 1.0;
    matrix[3].w = 1.0;
}

void copyMatrix(PointF3 destination[4], PointF3 source[4])
{
    for (int i = 0; i < 4; i++)
    {
        destination[i] = source[i];
    }
}

void multiplyMatrix(PointF3 mat1[4], PointF3 mat2[4])
{
    PointF3 returnValue[4] = {0};
    setMatrixAsIdentity(returnValue);

    for (int i = 0; i < 4; i++)
    {
        multiplyVertexByMatrix(returnValue[i], mat1[i], mat2);
    }

    copyMatrix(mat1, returnValue);
}

Shape::Shape(int vertexNum)
{
    vertices.reserve(vertexNum);
    transformedVertices.reserve(vertexNum);

    setMatrixAsIdentity(transformMatrix);
    setMatrixAsIdentity(rotationMatrix);
    setMatrixAsIdentity(scaleMatrix);
    setMatrixAsIdentity(translationMatrix);
}

Shape::~Shape()
{
}

void Shape::draw(ImageData &pImageData)
{
    if (isTransformDirty)
    {
        recalculateTransformMatrix();
        transform();
        isTransformDirty = false;
    }

    project(150);

    auto size = transformedVertices.size();
    for (int i = 0; i < size; i++)
    {
        auto nextIndex = (i + 1) % size;
        PointI p1 = {static_cast<int>(transformedVertices[i].x), static_cast<int>(transformedVertices[i].y)};
        PointI p2 = {static_cast<int>(transformedVertices[nextIndex].x), static_cast<int>(transformedVertices[nextIndex].y)};

        pImageData.drawLine(p1, p2);
    }
}

void Shape::transform()
{
    auto vertexSize = vertices.size();
    for (auto i = 0; i < vertexSize; i++)
    {
        multiplyVertexByMatrix(transformedVertices[i], vertices[i], transformMatrix);
    }
}

void Shape::translate(PointF3 translation)
{
    setMatrixAsIdentity(translationMatrix);
    translationMatrix[2].x = translation.x;
    translationMatrix[2].y = translation.y;
    translationMatrix[2].z = translation.z;
    translationMatrix[2].w = 1.f;
    isTransformDirty = true;
}

void Shape::scale(PointF3 scale)
{
    setMatrixAsIdentity(scaleMatrix);
    scaleMatrix[0].x = scale.x;
    scaleMatrix[1].y = scale.y;
    scaleMatrix[2].z = scale.z;
    scaleMatrix[3].w = 1.f;
    isTransformDirty = true;
}

void Shape::recalculateTransformMatrix()
{
    setMatrixAsIdentity(transformMatrix);
    multiplyMatrix(transformMatrix, rotationMatrix);
    multiplyMatrix(transformMatrix, scaleMatrix);
    multiplyMatrix(transformMatrix, translationMatrix);
}

void Shape::rotateZ(float angle)
{
    rotationMatrix[0].x = cos(angle);
    rotationMatrix[0].y = -sin(angle);

    rotationMatrix[1].x = sin(angle);
    rotationMatrix[1].y = cos(angle);
    isTransformDirty = true;
}

void Shape::project(float distance)
{
    auto size = transformedVertices.size();
    for (int i = 0; i < size; i++)
    {
        transformedVertices[i].x = distance * transformedVertices[i].x / transformedVertices[i].z + 160;
        transformedVertices[i].y = distance * transformedVertices[i].y / transformedVertices[i].z + 100;
    }
}