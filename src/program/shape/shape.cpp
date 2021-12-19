#include "shape.hpp"
#include <iostream>
#include <cmath>
#include <array>

void multiplyVertexByMatrix(PointF3 &vertDestination, PointF3 &vertSource, PointF3 mat[3])
{
    vertDestination.x = vertSource.x * mat[0].x +
                        vertSource.y * mat[1].x +
                        vertSource.z * mat[2].x;

    vertDestination.y = vertSource.x * mat[0].y +
                        vertSource.y * mat[1].y +
                        vertSource.z * mat[2].y;

    vertDestination.z = vertSource.x * mat[0].z +
                        vertSource.y * mat[1].z +
                        vertSource.z * mat[2].z;
}

void setMatrixAsIdentity(PointF3 matrix[3])
{
    for (int i = 0; i < 3; i++)
    {
        matrix[i] = {0};
    }
    matrix[0].x = 1.0;
    matrix[1].y = 1.0;
    matrix[2].z = 1.0;
}

void copyMatrix(PointF3 destination[3], PointF3 source[3])
{
    for (int i = 0; i < 3; i++)
    {
        destination[i] = source[i];
    }
}

void multiplyMatrix(PointF3 mat1[3], PointF3 mat2[3])
{
    PointF3 returnValue[3] = {0};
    setMatrixAsIdentity(returnValue);

    for (int i = 0; i < 3; i++)
    {
        multiplyVertexByMatrix(returnValue[i], mat1[i], mat2);
    }

    copyMatrix(mat1, returnValue);
}

Shape::Shape(int vertexNum)
{
    vertices.reserve(vertexNum);
    transformedVertices.reserve(vertexNum);

    transformMatrix[0].x = 1.0;
    transformMatrix[1].y = 1.0;
    transformMatrix[2].z = 1.0;

    rotationMatrix[0].x = 1.0;
    rotationMatrix[1].y = 1.0;
    rotationMatrix[2].z = 1.0;

    translationMatrix[0].x = 1.0;
    translationMatrix[1].y = 1.0;
    translationMatrix[2].z = 1.0;

    scaleMatrix[0].x = 1.0;
    scaleMatrix[1].y = 1.0;
    scaleMatrix[2].z = 1.0;
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

    auto size = vertices.size();
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
    translationMatrix[2].x = translation.x;
    translationMatrix[2].y = translation.y;
    translationMatrix[2].z = translation.z;
    isTransformDirty = true;
}

void Shape::scale(PointF3 scale)
{
    scaleMatrix[0].x = scale.x;
    scaleMatrix[1].y = scale.y;
    scaleMatrix[2].z = scale.z;
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
    for (int i = 0; i < vertices.size(); i++)
    {
        auto ver = vertices[i];
    }
}