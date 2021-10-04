#include "shape.hpp"
#include <iostream>
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
        recalculateTranslationMatrix();
        transform();
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
        transformedVertices[i].x = vertices[i].x * transformMatrix[0].x +
                                   vertices[i].y * transformMatrix[1].x +
                                   vertices[i].z * transformMatrix[2].x;

        transformedVertices[i].y = vertices[i].x * transformMatrix[0].y +
                                   vertices[i].y * transformMatrix[1].y +
                                   vertices[i].z * transformMatrix[2].y;

        transformedVertices[i].z = vertices[i].x * transformMatrix[0].z +
                                   vertices[i].y * transformMatrix[1].z +
                                   vertices[i].z * transformMatrix[2].z;
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

void Shape::recalculateTranslationMatrix()
{
    PointF3 identityMatrix[4] = {0};
    identityMatrix[0].x = 1;
    identityMatrix[1].y = 1;
    identityMatrix[2].z = 1;

    for (int i = 0; i < 3; i++)
    {
        transformMatrix[i].x = scaleMatrix[i].x * translationMatrix[0].x +
                               scaleMatrix[i].y * translationMatrix[1].x +
                               scaleMatrix[i].z * translationMatrix[2].x;

        transformMatrix[i].y = scaleMatrix[i].x * translationMatrix[0].y +
                               scaleMatrix[i].y * translationMatrix[1].y +
                               scaleMatrix[i].z * translationMatrix[2].y;

        transformMatrix[i].z = scaleMatrix[i].x * translationMatrix[0].z +
                               scaleMatrix[i].y * translationMatrix[1].z +
                               scaleMatrix[i].z * translationMatrix[2].z;
    }
}