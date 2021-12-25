#include "shape.hpp"
#include <iostream>
#include <cmath>
#include <array>
#include <algorithm>
#include <vector>

// #define WIREFRAME

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
        project(100);
        isTransformDirty = false;
    }

    auto size = vertexIndex.size();
    for (int i; i < size; i++)
    {
        auto index = vertexIndex[i];
        PointI p1 = {static_cast<int>(projectedVertices[index[0]].x), static_cast<int>(projectedVertices[index[0]].y)};
        PointI p2 = {static_cast<int>(projectedVertices[index[1]].x), static_cast<int>(projectedVertices[index[1]].y)};
        PointI p3 = {static_cast<int>(projectedVertices[index[2]].x), static_cast<int>(projectedVertices[index[2]].y)};

#ifndef WIREFRAME
        rasterizeTriangleTop({p1, p2, p3}, pImageData);
#else
        pImageData.drawLine(p1, p2, {255, 0, 0});
        pImageData.drawLine(p2, p3, {0, 255, 0});
        pImageData.drawLine(p3, p1, {0, 0, 255});
#endif

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
        projectedVertices[i].x = distance * transformedVertices[i].x / transformedVertices[i].z + 160;
        projectedVertices[i].y = distance * transformedVertices[i].y / transformedVertices[i].z + 120;
    }
}

Shape Shape::createCube(float zPosition)
{
    Shape shape(8);

    const int cubeSize = 50;

    shape.vertices.emplace_back((PointF3){-cubeSize, -cubeSize, cubeSize, 1.f});
    shape.vertices.emplace_back((PointF3){cubeSize, -cubeSize, cubeSize, 1.f});
    shape.vertices.emplace_back((PointF3){cubeSize, cubeSize, cubeSize, 1.f});
    shape.vertices.emplace_back((PointF3){-cubeSize, cubeSize, cubeSize, 1.f});

    shape.vertices.emplace_back((PointF3){-cubeSize, -cubeSize, -cubeSize, 1.f});
    shape.vertices.emplace_back((PointF3){cubeSize, -cubeSize, -cubeSize, 1.f});
    shape.vertices.emplace_back((PointF3){cubeSize, cubeSize, -cubeSize, 1.f});
    shape.vertices.emplace_back((PointF3){-cubeSize, cubeSize, -cubeSize, 1.f});

    shape.vertexIndex.emplace_back(std::array<uint32_t, 3>({0, 1, 2}));
    shape.vertexIndex.emplace_back(std::array<uint32_t, 3>({0, 2, 3}));

    shape.vertexIndex.emplace_back(std::array<uint32_t, 3>({4, 5, 6}));
    shape.vertexIndex.emplace_back(std::array<uint32_t, 3>({4, 7, 6}));

    shape.vertexIndex.emplace_back(std::array<uint32_t, 3>({4, 0, 3}));
    shape.vertexIndex.emplace_back(std::array<uint32_t, 3>({4, 7, 3}));

    shape.vertexIndex.emplace_back(std::array<uint32_t, 3>({4, 0, 1}));
    shape.vertexIndex.emplace_back(std::array<uint32_t, 3>({4, 5, 1}));

    shape.vertexIndex.emplace_back(std::array<uint32_t, 3>({2, 5, 6}));
    shape.vertexIndex.emplace_back(std::array<uint32_t, 3>({2, 5, 1}));

    shape.vertexIndex.emplace_back(std::array<uint32_t, 3>({3, 2, 6}));
    shape.vertexIndex.emplace_back(std::array<uint32_t, 3>({3, 7, 6}));

    shape.transformedVertices.resize(shape.vertices.size());
    shape.projectedVertices.resize(shape.vertices.size());

    shape.translate({0.f, 0.f, zPosition});

    return shape;
}

bool sortTriangleY(PointI a, PointI b)
{
    if (a.y == b.y)
        return a.x < b.x;
    return a.y < b.y;
}

bool sortTriangleX(PointI a, PointI b)
{
    return a.x < b.x;
}

void Shape::rasterizeTriangleTop(std::array<PointI, 3> triangle, ImageData &pImageData)
{
    std::sort(triangle.begin(), triangle.end(), sortTriangleY);

    int height = triangle[1].y - triangle[0].y;

    float dy = (float)(triangle[1].y - triangle[0].y);
    float dx = (float)(triangle[2].y - triangle[0].y);

    float incrementY = dy ? (float)(triangle[1].x - triangle[0].x) / dy : 0;
    float incrementXLimit = dx ? (float)(triangle[2].x - triangle[0].x) / dx : 0;
    float start = 0;
    float end = 0;

    for (int i = 0; i < height; i++)
    {
        int localStart = floor(start);
        int localEnd = floor(end);
        pImageData.drawLine({triangle[0].x + localStart, triangle[0].y + i}, {triangle[0].x + localEnd, triangle[0].y + i}, {0xFF, 0, 0});
        start += incrementXLimit;
        end += incrementY;
    }

    if (height != 0)
    {
        dy = (float)(triangle[2].y - triangle[1].y);
        incrementY = dy ? (float)(triangle[2].x - triangle[1].x) / dy : 0;
    }
    else
    {
        incrementY = incrementXLimit;
        dy = (float)(triangle[2].y - triangle[1].y);
        incrementXLimit = dy ? (float)(triangle[2].x - triangle[1].x) / dy : 0;
        start = triangle[1].x - triangle[0].x;
    }
    height = triangle[2].y - triangle[1].y;

    for (int i = 0; i < height; i++)
    {
        int localStart = floor(start);
        int localEnd = floor(end);
        pImageData.drawLine({triangle[0].x + localStart, triangle[1].y + i}, {triangle[0].x + localEnd, triangle[1].y + i}, {0xFF, 0, 0});
        start += incrementXLimit;
        end += incrementY;
    }

    return;
}

void Shape::rasterizeTriangleBottom(std::array<PointI, 3> triangle, ImageData &pImageData)
{
    std::sort(triangle.begin(), triangle.end(), sortTriangleY);

    int height = triangle[2].y - triangle[1].y;

    float dy = (float)(triangle[1].y - triangle[0].y);
    float dx = (float)(triangle[2].y - triangle[0].y);

    float incrementY = dx ? (float)(triangle[2].x - triangle[0].x) / dx : 0;
    float incrementXLimit = dy ? (float)(triangle[0].x - triangle[2].x) / dy : 0;
    float start = 0;
    float end = 0;

    for (int i = 0; i < height; i++)
    {
        int localStart = floor(start);
        int localEnd = floor(end);
        pImageData.drawLine({triangle[1].x + localStart, triangle[1].y + i}, {triangle[1].x + localEnd, triangle[1].y + i}, {0xFF, 0, 0});
        start += incrementXLimit;
        end += incrementY;
    }

    return;
}