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
    projectedVertices.reserve(vertexNum);

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
        project(150);
        isTransformDirty = false;
    }

    auto size = vertexIndex.size();
    for (int i; i < size; i++)
    {
        auto index = vertexIndex[i];
        PointI p1 = {static_cast<int>(projectedVertices[index[0]].x), static_cast<int>(projectedVertices[index[0]].y)};
        PointI p2 = {static_cast<int>(projectedVertices[index[1]].x), static_cast<int>(projectedVertices[index[1]].y)};
        PointI p3 = {static_cast<int>(projectedVertices[index[2]].x), static_cast<int>(projectedVertices[index[2]].y)};

        pImageData.drawLine(p1, p2, {255, 0, 0});
        pImageData.drawLine(p2, p3, {0, 255, 0});
        pImageData.drawLine(p3, p1, {0, 0, 255});

        pImageData.drawCharacter(p1, index[0] + '0');
        pImageData.drawCharacter(p2, index[1] + '0');
        pImageData.drawCharacter(p3, index[2] + '0');
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
    translationMatrix[0] = {1, 0, 0, 0};
    translationMatrix[1] = {0, 1, 0, 0};
    translationMatrix[2] = {0, 0, 1, 0};
    translationMatrix[3] = {translation.x, translation.y, translation.z, 1};

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

void Shape::rotate(float x, float y, float z)
{
    PointF3 rotationMatrixX[4] = {0};

    rotationMatrixX[0] = {1, 0, 0, 0};
    rotationMatrixX[1] = {0, cosf(x), sinf(x), 0};
    rotationMatrixX[2] = {0, -sinf(x), cosf(x), 0};
    rotationMatrixX[3] = {0, 0, 0, 1};

    PointF3 rotationMatrixY[4] = {0};

    rotationMatrixY[0] = {cosf(y), 0, -sinf(y), 0};
    rotationMatrixY[1] = {0, 1, 0, 0};
    rotationMatrixY[2] = {sinf(y), 0, cosf(y), 0};
    rotationMatrixY[3] = {0, 0, 0, 1};

    PointF3 rotationMatrixZ[4] = {0};

    rotationMatrixZ[0] = {cosf(z), sinf(z), 0, 0};
    rotationMatrixZ[1] = {-sinf(z), cosf(z), 0, 0};
    rotationMatrixZ[2] = {0, 0, 1, 0};
    rotationMatrixZ[3] = {0, 0, 0, 1};

    setMatrixAsIdentity(rotationMatrix);
    multiplyMatrix(rotationMatrix, rotationMatrixZ);
    multiplyMatrix(rotationMatrix, rotationMatrixX);
    multiplyMatrix(rotationMatrix, rotationMatrixY);

    isTransformDirty = true;
}

void Shape::project(float distance)
{
    auto size = transformedVertices.size();
    for (int i = 0; i < size; i++)
    {
        projectedVertices[i].x = transformedVertices[i].x + 160;
        projectedVertices[i].y = transformedVertices[i].y + 100;
        // projectedVertices[i].x = distance * transformedVertices[i].x / transformedVertices[i].z + 160;
        // projectedVertices[i].y = distance * transformedVertices[i].y / transformedVertices[i].z + 100;
    }
}