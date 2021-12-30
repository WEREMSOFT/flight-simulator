#include "shape.hpp"
#include <iostream>
#include <cmath>
#include <array>
#include <algorithm>
#include <vector>

#define WIREFRAME 0
#define BACKFACE_CULlING 1
#define SHOW_VERTEX_NUMBER 0
#define DRAW_NORMALS 0
#define USE_HACKY_SHADING 0
#define ANGLE_RATIO 3.1416 * 255

namespace MathUtils
{
    PointF3 addVertex(PointF3 u, PointF3 v)
    {
        return {u.x + v.x, u.y + v.y, u.z + v.z, u.w + v.w};
    }
    PointF3 dotProduct(PointF3 u, PointF3 v)
    {
        return {u.y * v.z - u.z * v.y,
                u.z * v.x - u.x * v.z,
                u.x * v.y - u.y * v.x, 0};
    }

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
}

Shape::Shape(int vertexNum)
{
    vertices.reserve(vertexNum);
    transformedVertices.reserve(vertexNum);
    projectedVertices.reserve(vertexNum);

    MathUtils::setMatrixAsIdentity(transformMatrix);
    MathUtils::setMatrixAsIdentity(rotationMatrix);
    MathUtils::setMatrixAsIdentity(scaleMatrix);
    MathUtils::setMatrixAsIdentity(translationMatrix);
}

Shape::~Shape()
{
}

float angleBetweenVectors(PointF3 a, PointF3 b)
{
    auto dot = a.x * b.x + a.y * b.y + a.z * b.z;
    auto lenSq1 = a.x * a.x + a.y * a.y + a.z * a.z;
    auto lenSq2 = b.x * b.x + b.y * b.y + b.z * b.z;
    return acos(dot / sqrt(lenSq1 * lenSq2));
}

bool isBackFace(std::array<PointF3, 3> triangle)
{
    auto z = (triangle[1].x - triangle[0].x) * (triangle[2].y - triangle[0].y) - (triangle[1].y - triangle[0].y) * (triangle[2].x - triangle[0].x);
    return z < 0;
}

bool isInTheShadow(std::array<PointF3, 3> triangle)
{
    auto z = (triangle[1].y - triangle[0].y) * (triangle[2].z - triangle[0].z) - (triangle[1].z - triangle[0].z) * (triangle[2].y - triangle[0].y);
    return z > 0;
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
    Color color = {0, 0, 255};
    for (int i = 0; i < size; i++)
    {
        auto index = vertexIndex[i];
        PointI p1 = {static_cast<int32_t>(projectedVertices[index[0]].x), static_cast<int32_t>(projectedVertices[index[0]].y), static_cast<int32_t>(projectedVertices[index[0]].z)};
        PointI p2 = {static_cast<int32_t>(projectedVertices[index[1]].x), static_cast<int32_t>(projectedVertices[index[1]].y), static_cast<int32_t>(projectedVertices[index[1]].z)};
        PointI p3 = {static_cast<int32_t>(projectedVertices[index[2]].x), static_cast<int32_t>(projectedVertices[index[2]].y), static_cast<int32_t>(projectedVertices[index[2]].z)};

#if BACKFACE_CULlING
        if (isBackFace({projectedVertices[index[0]], projectedVertices[index[1]], projectedVertices[index[2]]}))
            continue;
#endif

#if USE_HACKY_SHADING
        if (isInTheShadow({projectedVertices[index[0]], projectedVertices[index[1]], projectedVertices[index[2]]}))
            color = {0, 0, 0xFF};
        else
            color = {0, 0, 0x99};
#else
        auto component = angleBetweenVectors(transformedNormals[normalIndex[i]], {1, 1, 1}) / ANGLE_RATIO;
        color = {static_cast<unsigned char>(component),
                 static_cast<unsigned char>(component),
                 static_cast<unsigned char>(component)};
#endif
#if WIREFRAME
        pImageData.drawLine(p1, p2, {255, 0, 0});
        pImageData.drawLine(p2, p3, {0, 255, 0});
        pImageData.drawLine(p3, p1, {0, 0, 255});
#else
        rasterizeTriangle({p1, p2, p3}, pImageData, color);
#endif

#if DRAW_NORMALS

        pImageData.drawLine({p2.x, p2.y}, {p2.x + static_cast<int>(transformedNormals[normalIndex[i]].x * 10), p2.y + static_cast<int>(transformedNormals[normalIndex[i]].y * 10)});
        pImageData.drawLine({p2.x, p2.y}, {p2.x + static_cast<int>(transformedNormals[normalIndex[i]].x * 10), p2.y + static_cast<int>(transformedNormals[normalIndex[i]].y * 10)});
        pImageData.drawLine({p3.x, p3.y}, {p3.x + static_cast<int>(transformedNormals[normalIndex[i]].x * 10), p3.y + static_cast<int>(transformedNormals[normalIndex[i]].y * 10)});

#endif

#if SHOW_VERTEX_NUMBER
        pImageData.drawCharacter(p1, index[0] + '0');
        pImageData.drawCharacter(p2, index[1] + '0');
        pImageData.drawCharacter(p3, index[2] + '0');
#endif
    }
}

void Shape::transform()
{
    auto normalsSize = normals.size();
    for (auto i = 0; i < normalsSize; i++)
    {
        MathUtils::multiplyVertexByMatrix(transformedNormals[i], normals[i], transformMatrix);
    }
    auto vertexSize = vertices.size();
    for (auto i = 0; i < vertexSize; i++)
    {
        MathUtils::multiplyVertexByMatrix(transformedVertices[i], vertices[i], transformMatrix);
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
    MathUtils::setMatrixAsIdentity(scaleMatrix);
    scaleMatrix[0].x = scale.x;
    scaleMatrix[1].y = scale.y;
    scaleMatrix[2].z = scale.z;
    scaleMatrix[3].w = 1.f;
    isTransformDirty = true;
}

void Shape::recalculateTransformMatrix()
{
    MathUtils::setMatrixAsIdentity(transformMatrix);
    MathUtils::multiplyMatrix(transformMatrix, rotationMatrix);
    MathUtils::multiplyMatrix(transformMatrix, scaleMatrix);
    MathUtils::multiplyMatrix(transformMatrix, translationMatrix);
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

    MathUtils::setMatrixAsIdentity(rotationMatrix);
    MathUtils::multiplyMatrix(rotationMatrix, rotationMatrixZ);
    MathUtils::multiplyMatrix(rotationMatrix, rotationMatrixX);
    MathUtils::multiplyMatrix(rotationMatrix, rotationMatrixY);

    isTransformDirty = true;
}

void Shape::project(float distance)
{
    auto size = transformedVertices.size();
    for (int i = 0; i < size; i++)
    {
        projectedVertices[i].x = distance * transformedVertices[i].x / transformedVertices[i].z + 160;
        projectedVertices[i].y = distance * transformedVertices[i].y / transformedVertices[i].z + 120;
        projectedVertices[i].z = transformedVertices[i].z;
    }
}

Shape Shape::createPyramid(float baseSize, float height, float zPosition)
{
    Shape shape(8);

    shape.vertices.emplace_back((PointF3){-baseSize, 0, -baseSize, 1.f});
    shape.vertices.emplace_back((PointF3){-baseSize, 0, baseSize, 1.f});
    shape.vertices.emplace_back((PointF3){baseSize, 0, baseSize, 1.f});
    shape.vertices.emplace_back((PointF3){baseSize, 0, -baseSize, 1.f});
    shape.vertices.emplace_back((PointF3){0, -height, 0, 1.f});

    shape.vertexIndex.emplace_back(std::array<uint32_t, 3>({0, 2, 1}));
    shape.normals.emplace_back(MathUtils::dotProduct(shape.vertices[0], shape.vertices[1]));
    shape.normalIndex.emplace_back(shape.normals.size() - 1);
    shape.vertexIndex.emplace_back(std::array<uint32_t, 3>({0, 3, 2}));
    shape.normalIndex.emplace_back(shape.normals.size() - 1);

    shape.vertexIndex.emplace_back(std::array<uint32_t, 3>({4, 1, 2}));
    shape.normals.emplace_back(MathUtils::dotProduct(shape.vertices[4], shape.vertices[2]));
    shape.normalIndex.emplace_back(shape.normals.size() - 1);

    shape.vertexIndex.emplace_back(std::array<uint32_t, 3>({4, 0, 1}));
    shape.normals.emplace_back(MathUtils::dotProduct(shape.vertices[4], shape.vertices[1]));
    shape.normalIndex.emplace_back(shape.normals.size() - 1);

    shape.vertexIndex.emplace_back(std::array<uint32_t, 3>({4, 3, 0}));
    shape.normals.emplace_back(MathUtils::dotProduct(shape.vertices[4], shape.vertices[0]));
    shape.normalIndex.emplace_back(shape.normals.size() - 1);

    shape.vertexIndex.emplace_back(std::array<uint32_t, 3>({4, 2, 3}));
    shape.normals.emplace_back(MathUtils::dotProduct(shape.vertices[4], shape.vertices[3]));
    shape.normalIndex.emplace_back(shape.normals.size() - 1);

    shape.transformedNormals.resize(shape.normals.size());

    shape.transformedVertices.resize(shape.vertices.size());
    shape.projectedVertices.resize(shape.vertices.size());

    shape.translate({0.f, 0.f, zPosition});

    return shape;
}

void Shape::appendPiramid(Shape &shape, float baseSize, float height, PointF3 position)
{
    uint32_t vertexOffset = shape.vertices.size();
    shape.vertices.emplace_back(MathUtils::addVertex({-baseSize, 0, -baseSize, 1.f}, position));
    shape.vertices.emplace_back(MathUtils::addVertex({-baseSize, 0, baseSize, 1.f}, position));
    shape.vertices.emplace_back(MathUtils::addVertex({baseSize, 0, baseSize, 1.f}, position));
    shape.vertices.emplace_back(MathUtils::addVertex({baseSize, 0, -baseSize, 1.f}, position));
    shape.vertices.emplace_back(MathUtils::addVertex({0, -height, 0, 1.f}, position));

    shape.vertexIndex.emplace_back(std::array<uint32_t, 3>({vertexOffset + 0, vertexOffset + 2, vertexOffset + 1}));
    shape.normals.emplace_back(MathUtils::dotProduct(shape.vertices[vertexOffset], shape.vertices[vertexOffset + 1]));
    shape.normalIndex.emplace_back(shape.normals.size() - 1);
    shape.vertexIndex.emplace_back(std::array<uint32_t, 3>({vertexOffset + 0, vertexOffset + 3, vertexOffset + 2}));
    shape.normalIndex.emplace_back(shape.normals.size() - 1);

    shape.vertexIndex.emplace_back(std::array<uint32_t, 3>({vertexOffset + 4, vertexOffset + 1, vertexOffset + 2}));
    shape.normals.emplace_back(MathUtils::dotProduct(shape.vertices[vertexOffset + 4], shape.vertices[vertexOffset + 2]));
    shape.normalIndex.emplace_back(shape.normals.size() - 1);

    shape.vertexIndex.emplace_back(std::array<uint32_t, 3>({vertexOffset + 4, vertexOffset + 0, vertexOffset + 1}));
    shape.normals.emplace_back(MathUtils::dotProduct(shape.vertices[vertexOffset + 4], shape.vertices[vertexOffset + 1]));
    shape.normalIndex.emplace_back(shape.normals.size() - 1);

    shape.vertexIndex.emplace_back(std::array<uint32_t, 3>({vertexOffset + 4, vertexOffset + 3, vertexOffset + 0}));
    shape.normals.emplace_back(MathUtils::dotProduct(shape.vertices[vertexOffset + 4], shape.vertices[vertexOffset + 0]));
    shape.normalIndex.emplace_back(shape.normals.size() - 1);

    shape.vertexIndex.emplace_back(std::array<uint32_t, 3>({vertexOffset + 4, vertexOffset + 2, vertexOffset + 3}));
    shape.normals.emplace_back(MathUtils::dotProduct(shape.vertices[vertexOffset + 4], shape.vertices[vertexOffset + 3]));
    shape.normalIndex.emplace_back(shape.normals.size() - 1);

    shape.transformedNormals.resize(shape.normals.size());

    shape.transformedVertices.resize(shape.vertices.size());
    shape.projectedVertices.resize(shape.vertices.size());

    shape.translate({0.f, 0.f, position.z});
}

Shape Shape::createCube(float cubeSize = 50, float zPosition = 140)
{
    Shape shape(8);

    shape.vertices.emplace_back((PointF3){-cubeSize, -cubeSize, cubeSize, 1.f});
    shape.vertices.emplace_back((PointF3){cubeSize, -cubeSize, cubeSize, 1.f});
    shape.vertices.emplace_back((PointF3){cubeSize, cubeSize, cubeSize, 1.f});
    shape.vertices.emplace_back((PointF3){-cubeSize, cubeSize, cubeSize, 1.f});

    shape.vertices.emplace_back((PointF3){-cubeSize, -cubeSize, -cubeSize, 1.f});
    shape.vertices.emplace_back((PointF3){cubeSize, -cubeSize, -cubeSize, 1.f});
    shape.vertices.emplace_back((PointF3){cubeSize, cubeSize, -cubeSize, 1.f});
    shape.vertices.emplace_back((PointF3){-cubeSize, cubeSize, -cubeSize, 1.f});

    shape.vertexIndex.emplace_back(std::array<uint32_t, 3>({0, 2, 1}));
    shape.normalIndex.emplace_back(2);
    shape.vertexIndex.emplace_back(std::array<uint32_t, 3>({0, 3, 2}));
    shape.normalIndex.emplace_back(2);

    shape.vertexIndex.emplace_back(std::array<uint32_t, 3>({4, 5, 6}));
    shape.normalIndex.emplace_back(5);
    shape.vertexIndex.emplace_back(std::array<uint32_t, 3>({4, 6, 7}));
    shape.normalIndex.emplace_back(5);

    shape.vertexIndex.emplace_back(std::array<uint32_t, 3>({0, 4, 3}));
    shape.normalIndex.emplace_back(3);
    shape.vertexIndex.emplace_back(std::array<uint32_t, 3>({4, 7, 3}));
    shape.normalIndex.emplace_back(3);

    shape.vertexIndex.emplace_back(std::array<uint32_t, 3>({4, 0, 1}));
    shape.normalIndex.emplace_back(4);
    shape.vertexIndex.emplace_back(std::array<uint32_t, 3>({4, 1, 5}));
    shape.normalIndex.emplace_back(4);

    shape.vertexIndex.emplace_back(std::array<uint32_t, 3>({2, 6, 5}));
    shape.normalIndex.emplace_back(0);
    shape.vertexIndex.emplace_back(std::array<uint32_t, 3>({2, 5, 1}));
    shape.normalIndex.emplace_back(0);

    shape.vertexIndex.emplace_back(std::array<uint32_t, 3>({3, 6, 2}));
    shape.normalIndex.emplace_back(1);
    shape.vertexIndex.emplace_back(std::array<uint32_t, 3>({3, 7, 6}));
    shape.normalIndex.emplace_back(1);

    shape.normals.emplace_back((PointF3){1, 0, 0, 0});
    shape.normals.emplace_back((PointF3){0, 1, 0, 0});
    shape.normals.emplace_back((PointF3){0, 0, 1, 0});

    shape.normals.emplace_back((PointF3){-1, 0, 0, 0});
    shape.normals.emplace_back((PointF3){0, -1, 0, 0});
    shape.normals.emplace_back((PointF3){0, 0, -1, 0});

    shape.transformedNormals.resize(shape.normals.size());

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

void Shape::rasterizeTriangle(std::array<PointI, 3> triangle, ImageData &pImageData, Color color)
{
    std::sort(triangle.begin(), triangle.end(), sortTriangleY);

    float dy = (float)(triangle[1].y - triangle[0].y);
    float dx = (float)(triangle[2].y - triangle[0].y);

    float incrementY = dy ? (float)(triangle[1].x - triangle[0].x) / dy : 0;
    float incrementXLimit = dx ? (float)(triangle[2].x - triangle[0].x) / dx : 0;
    float start = 0;
    float end = 0;

    int height = triangle[1].y - triangle[0].y;
    for (int i = 0; i < height; i++)
    {
        int localStart = floor(start);
        int localEnd = floor(end);

        pImageData.drawLineZ({triangle[0].x + localStart, triangle[0].y + i}, {triangle[0].x + localEnd, triangle[0].y + i}, triangle, color);

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
        pImageData.drawLineZ({triangle[0].x + localStart, triangle[1].y + i}, {triangle[0].x + localEnd, triangle[1].y + i}, triangle, color);
        start += incrementXLimit;
        end += incrementY;
    }

    return;
}
