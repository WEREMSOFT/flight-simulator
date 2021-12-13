#include "shape.hpp"
#include <iostream>
#include <cmath>

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

Shape::Shape(int vertexNum)
{
    vertices.reserve(vertexNum);
    transformedVertices.reserve(vertexNum);

    transformMatrix[0].x = 1.0;
    transformMatrix[1].y = 1.0;
    transformMatrix[2].z = 1.0;

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
    PointF3 identityMatrix[4] = {0};
    identityMatrix[0].x = 1;
    identityMatrix[1].y = 1;
    identityMatrix[2].z = 1;

    for (int i = 0; i < 3; i++)
    {
        multiplyVertexByMatrix(transformMatrix[i], scaleMatrix[i], translationMatrix);
    }
}

void Shape::rotateZ(float angle)
{
    translationMatrix[0].x = cos(angle);
    translationMatrix[0].y = -sin(angle);

    translationMatrix[1].x = sin(angle);
    translationMatrix[1].y = cos(angle);
    isTransformDirty = true;
}

void Shape::project(float distance)
{
    for (int i = 0; i < vertices.size(); i++)
    {
        auto ver = vertices[i];
    }
}