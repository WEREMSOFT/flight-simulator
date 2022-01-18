#include "shape.hpp"
#include <iostream>
#include <cmath>
#include <array>
#include <algorithm>
#include <vector>
#include <string>
#include <list>

#define ANGLE_RATIO 3.1416 * 255

namespace MathUtils
{
    PointF3 scaleVector(PointF3 v, float scalar)
    {
        return {v.x * scalar, v.y * scalar, v.z * scalar, v.w * scalar};
    }
    PointF3 addVertex(PointF3 u, PointF3 v)
    {
        return {u.x + v.x, u.y + v.y, u.z + v.z, u.w + v.w};
    }

    PointF3 substractVertex(PointF3 u, PointF3 v)
    {
        return {u.x - v.x, u.y - v.y, u.z - v.z, u.w - v.w};
    }

    PointF3 crossProduct(PointF3 u, PointF3 v)
    {
        return {u.y * v.z - u.z * v.y,
                u.z * v.x - u.x * v.z,
                u.x * v.y - u.y * v.x, 0};
    }

    double dotProduct(PointF3 a, PointF3 b)
    {
        return a.x * b.x + a.y * b.y + a.z * b.z;
    }

    float length(PointF3 v)
    {
        return sqrt(pow(v.x, 2) + pow(v.y, 2) + pow(v.z, 2));
    }

    PointF3 normalize(PointF3 v)
    {
        auto length = MathUtils::length(v);
        return {v.x / length, v.y / length, v.z / length};
    }

    PointF3 intersectionPoint(PointF3 lineVector, PointF3 linePoint, PointF3 planeNormal, PointF3 planePoint)
    {
        PointF3 diff = substractVertex(linePoint, planePoint);

        return addVertex(addVertex(diff, planePoint), scaleVector(lineVector, -dotProduct(diff, planeNormal) / dotProduct(lineVector, planeNormal)));
    }

    PointF3 linePlaneIntersection(PointF3 ray, PointF3 rayOrigin, PointF3 normal, PointF3 coord)
    {

        // calculate plane
        float d = dotProduct(normal, coord);

        if (dotProduct(normal, ray))
        {
            return {0}; // avoid divide by zero
        }

        // Compute the t value for the directed line ray intersecting the plane
        float t = (d - dotProduct(normal, rayOrigin)) / dotProduct(normal, ray);

        // scale the ray by t
        PointF3 newRay = scaleVector(ray, t);

        // calc contact point
        return addVertex(rayOrigin, newRay);
    }

    void multiplyVertexByMatrix(PointF3 &vertDestination, PointF3 &vertSource, PointF3 mat[4])
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

bool isBackFace(PointF3 normal)
{
    auto angle = MathUtils::dotProduct(normal, {0, 0, 1});
    return angle > 0;
}

bool sortTriangleZ(PointF3 a, PointF3 b)
{
    return a.z > b.z;
}

void Shape::clipTriangle(TrianglesF &triangles, TriangleF triangle, float z, std::vector<uint32_t> &localNormalIndex, int32_t normalIndex)
{
    int vertexInside = 0;
    for (auto &vertex : triangle)
    {
        if (vertex.z >= z)
            vertexInside++;
    }

    if (vertexInside == 0)
        return;

    if (vertexInside == 3)
    {
        triangles.emplace_back(triangle);
        localNormalIndex.emplace_back(normalIndex);
        return;
    }

    if (vertexInside == 1)
    {
        std::sort(triangle.begin(), triangle.end(), sortTriangleZ);
        triangle[1] = MathUtils::intersectionPoint(MathUtils::substractVertex(triangle[0], triangle[1]), triangle[1], {0, 0, 1}, {0, 0, z});
        triangle[2] = MathUtils::intersectionPoint(MathUtils::substractVertex(triangle[0], triangle[2]), triangle[2], {0, 0, 1}, {0, 0, z});
        triangles.emplace_back(triangle);
        localNormalIndex.emplace_back(normalIndex);
        return;
    }

    if (vertexInside == 2)
    {
        std::sort(triangle.begin(), triangle.end(), sortTriangleZ);
        TriangleF newTriangle = {0};
        newTriangle[0] = triangle[1];
        newTriangle[1] = MathUtils::intersectionPoint(MathUtils::substractVertex(triangle[0], triangle[2]), triangle[2], {0, 0, -1}, {0, 0, z});
        newTriangle[2] = MathUtils::intersectionPoint(MathUtils::substractVertex(triangle[1], triangle[2]), triangle[1], {0, 0, 1}, {0, 0, z});

        triangle[2] = MathUtils::intersectionPoint(MathUtils::substractVertex(triangle[0], triangle[2]), triangle[2], {0, 0, -1}, {0, 0, z});
        triangles.emplace_back(triangle);
        localNormalIndex.emplace_back(normalIndex);

        triangles.emplace_back(newTriangle);
        localNormalIndex.emplace_back(normalIndex);

        return;
    }
}

void Shape::draw(ImageData &pImageData, float zNear)
{
    TrianglesF clippedTriangles;
    TrianglesI projectedVerticesLocal;
    std::vector<uint32_t> localNormalIndex;

    if (isTransformDirty)
    {
        recalculateTransformMatrix();
        transform();

        int i = 0;
        for (auto &index : vertexIndex)
        {
            PointF3 p1 = {transformedVertices[index[0]].x, transformedVertices[index[0]].y, transformedVertices[index[0]].z};
            PointF3 p2 = {transformedVertices[index[1]].x, transformedVertices[index[1]].y, transformedVertices[index[1]].z};
            PointF3 p3 = {transformedVertices[index[2]].x, transformedVertices[index[2]].y, transformedVertices[index[2]].z};
            clipTriangle(clippedTriangles, {p1, p2, p3}, zNear, localNormalIndex, normalIndex[i++]);
        }

        project(projectedVerticesLocal, clippedTriangles, 100);
        isTransformDirty = false;
    }

    int i = 0;
    for (auto &triangle : projectedVerticesLocal)
    {
        if (!backFaceCulingDisabled)
            if (isBackFace(transformedNormals[localNormalIndex[i]]))
            {
                i++;
                continue;
            }

        if (wireFrame)
        {
            pImageData.drawLine(triangle[0], triangle[1], {255, 0, 0});
            pImageData.drawLine(triangle[1], triangle[2], {0, 255, 0});
            pImageData.drawLine(triangle[2], triangle[0], {0, 0, 255});
            continue;
        }
        Color color = {0, 0, 255};
        auto component = angleBetweenVectors(transformedNormals[localNormalIndex[i]], {1, 1, 1}) / ANGLE_RATIO;
        color = {static_cast<unsigned char>(component),
                 static_cast<unsigned char>(component),
                 static_cast<unsigned char>(component)};
        rasterizeTriangle(triangle, pImageData, color);
        if (showVertexNumber)
        {
            char numberString[50] = {0};
            snprintf(numberString, 50, "%d", vertexIndex[i]);
            pImageData.printString(triangle[0], numberString);
            snprintf(numberString, 50, "%d", vertexIndex[i]);
            pImageData.printString(triangle[1], numberString);
            snprintf(numberString, 50, "%d", vertexIndex[i]);
            pImageData.printString(triangle[2], numberString);
        }
        i++;
    }

    /*
    auto size = vertexIndex.size();
    Color color = {0, 0, 255};
    for (int i = 0; i < size; i++)
    {
        auto index = vertexIndex[i];
        PointI p1 = {static_cast<int32_t>(projectedVertices[index[0]].x), static_cast<int32_t>(projectedVertices[index[0]].y), static_cast<int32_t>(projectedVertices[index[0]].z)};
        PointI p2 = {static_cast<int32_t>(projectedVertices[index[1]].x), static_cast<int32_t>(projectedVertices[index[1]].y), static_cast<int32_t>(projectedVertices[index[1]].z)};
        PointI p3 = {static_cast<int32_t>(projectedVertices[index[2]].x), static_cast<int32_t>(projectedVertices[index[2]].y), static_cast<int32_t>(projectedVertices[index[2]].z)};

        if (!backFaceCulingDisabled)
            if (isBackFace({projectedVertices[index[0]], projectedVertices[index[1]], projectedVertices[index[2]]}))
                continue;

        auto component = angleBetweenVectors(transformedNormals[normalIndex[i]], {1, 1, 1}) / ANGLE_RATIO;
        color = {static_cast<unsigned char>(component),
                 static_cast<unsigned char>(component),
                 static_cast<unsigned char>(component)};

        if (wireFrame)
        {
            pImageData.drawLine(p1, p2, {255, 0, 0});
            pImageData.drawLine(p2, p3, {0, 255, 0});
            pImageData.drawLine(p3, p1, {0, 0, 255});
        }
        else
        {
            rasterizeTriangle({p1, p2, p3}, pImageData, color);
        }

        if (drawNormals)
        {
            pImageData.drawLine({p1.x, p1.y}, {p1.x + static_cast<int>(transformedNormals[normalIndex[i]].x * 10), p1.y + static_cast<int>(transformedNormals[normalIndex[i]].y * 10)});
            pImageData.drawLine({p2.x, p2.y}, {p2.x + static_cast<int>(transformedNormals[normalIndex[i]].x * 10), p2.y + static_cast<int>(transformedNormals[normalIndex[i]].y * 10)});
            pImageData.drawLine({p3.x, p3.y}, {p3.x + static_cast<int>(transformedNormals[normalIndex[i]].x * 10), p3.y + static_cast<int>(transformedNormals[normalIndex[i]].y * 10)});
        }

        if (showVertexNumber)
        {
            char numberString[50] = {0};
            snprintf(numberString, 50, "%d", index[0]);
            pImageData.printString(p1, numberString);
            snprintf(numberString, 50, "%d", index[1]);
            pImageData.printString(p2, numberString);
            snprintf(numberString, 50, "%d", index[2]);
            pImageData.printString(p3, numberString);
        }
    }*/
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

void Shape::project(TrianglesI &pProjectedVertices, TrianglesF &triangles, float distance)
{
    for (auto &triangle : triangles)
    {
        TriangleI triangleD = {0};
        for (int i = 0; i < triangleD.size(); ++i)
        {
            triangleD[i].x = static_cast<int32_t>(distance * triangle[i].x / triangle[i].z + 160);
            triangleD[i].y = static_cast<int32_t>(distance * triangle[i].y / triangle[i].z + 120);
            triangleD[i].z = static_cast<int32_t>(triangle[i].z);
        }
        pProjectedVertices.emplace_back(triangleD);
    }
}

Shape Shape::createPyramid(float baseSize, float height, float zPosition)
{
    Shape shape(8);

    Shape::appendPiramid(shape, baseSize, height, {0, 0, 0});

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
    shape.normals.emplace_back(
        MathUtils::normalize(
            MathUtils::crossProduct(
                MathUtils::substractVertex(shape.vertices[vertexOffset + 0], shape.vertices[vertexOffset + 1]),
                MathUtils::substractVertex(shape.vertices[vertexOffset + 0], shape.vertices[vertexOffset + 2]))));
    shape.normalIndex.emplace_back(shape.normals.size() - 1);

    shape.vertexIndex.emplace_back(std::array<uint32_t, 3>({vertexOffset + 0, vertexOffset + 3, vertexOffset + 2}));
    shape.normals.emplace_back(
        MathUtils::normalize(
            MathUtils::crossProduct(
                MathUtils::substractVertex(shape.vertices[vertexOffset + 0], shape.vertices[vertexOffset + 2]),
                MathUtils::substractVertex(shape.vertices[vertexOffset + 0], shape.vertices[vertexOffset + 3]))));
    shape.normalIndex.emplace_back(shape.normals.size() - 1);

    shape.vertexIndex.emplace_back(std::array<uint32_t, 3>({vertexOffset + 4, vertexOffset + 1, vertexOffset + 2}));
    shape.normals.emplace_back(
        MathUtils::normalize(
            MathUtils::crossProduct(
                MathUtils::substractVertex(shape.vertices[vertexOffset + 4], shape.vertices[vertexOffset + 2]),
                MathUtils::substractVertex(shape.vertices[vertexOffset + 4], shape.vertices[vertexOffset + 1]))));
    shape.normalIndex.emplace_back(shape.normals.size() - 1);

    shape.vertexIndex.emplace_back(std::array<uint32_t, 3>({vertexOffset + 4, vertexOffset + 0, vertexOffset + 1}));
    shape.normals.emplace_back(
        MathUtils::normalize(
            MathUtils::crossProduct(
                MathUtils::substractVertex(shape.vertices[vertexOffset + 4], shape.vertices[vertexOffset + 1]),
                MathUtils::substractVertex(shape.vertices[vertexOffset + 4], shape.vertices[vertexOffset + 0]))));
    shape.normalIndex.emplace_back(shape.normals.size() - 1);

    shape.vertexIndex.emplace_back(std::array<uint32_t, 3>({vertexOffset + 4, vertexOffset + 3, vertexOffset + 0}));
    shape.normals.emplace_back(
        MathUtils::normalize(
            MathUtils::crossProduct(
                MathUtils::substractVertex(shape.vertices[vertexOffset + 4], shape.vertices[vertexOffset + 0]),
                MathUtils::substractVertex(shape.vertices[vertexOffset + 4], shape.vertices[vertexOffset + 3]))));
    shape.normalIndex.emplace_back(shape.normals.size() - 1);

    shape.vertexIndex.emplace_back(std::array<uint32_t, 3>({vertexOffset + 4, vertexOffset + 2, vertexOffset + 3}));
    shape.normals.emplace_back(
        MathUtils::normalize(
            MathUtils::crossProduct(
                MathUtils::substractVertex(shape.vertices[vertexOffset + 4], shape.vertices[vertexOffset + 3]),
                MathUtils::substractVertex(shape.vertices[vertexOffset + 4], shape.vertices[vertexOffset + 2]))));
    shape.normalIndex.emplace_back(shape.normals.size() - 1);

    shape.transformedNormals.resize(shape.normals.size());

    shape.transformedVertices.resize(shape.vertices.size());
    shape.projectedVertices.resize(shape.vertices.size());

    shape.translate({0.f, 0.f, position.z});
}

void Shape::appendQuad(Shape &shape, float cubeSize, PointF3 position)
{
    uint32_t vertexOffset = shape.vertices.size();

    shape.vertices.emplace_back(MathUtils::addVertex({-cubeSize, -cubeSize, cubeSize, 1.f}, position));
    shape.vertices.emplace_back(MathUtils::addVertex({cubeSize, -cubeSize, cubeSize, 1.f}, position));
    shape.vertices.emplace_back(MathUtils::addVertex({cubeSize, cubeSize, cubeSize, 1.f}, position));
    shape.vertices.emplace_back(MathUtils::addVertex({-cubeSize, cubeSize, cubeSize, 1.f}, position));

    shape.vertexIndex.emplace_back(std::array<uint32_t, 3>({vertexOffset + 0, vertexOffset + 2, vertexOffset + 1}));
    shape.normals.emplace_back(
        MathUtils::normalize(
            MathUtils::crossProduct(
                MathUtils::substractVertex(shape.vertices[vertexOffset + 0], shape.vertices[vertexOffset + 2]),
                MathUtils::substractVertex(shape.vertices[vertexOffset + 0], shape.vertices[vertexOffset + 1]))));
    shape.normalIndex.emplace_back(shape.normals.size() - 1);

    shape.vertexIndex.emplace_back(std::array<uint32_t, 3>({vertexOffset + 0, vertexOffset + 3, vertexOffset + 2}));
    shape.normals.emplace_back(
        MathUtils::normalize(
            MathUtils::crossProduct(
                MathUtils::substractVertex(shape.vertices[vertexOffset + 0], shape.vertices[vertexOffset + 3]),
                MathUtils::substractVertex(shape.vertices[vertexOffset + 0], shape.vertices[vertexOffset + 2]))));
    shape.normalIndex.emplace_back(shape.normals.size() - 1);

    shape.transformedNormals.resize(shape.normals.size());

    shape.transformedVertices.resize(shape.vertices.size());
    shape.projectedVertices.resize(shape.vertices.size());

    shape.translate({0.f, 0.f, position.z});
}
void Shape::appendWall(Shape &shape, float size, PointF3 position)
{
    uint32_t vertexOffset = shape.vertices.size();

    shape.vertices.emplace_back(MathUtils::addVertex({-size, -size, 0, 1.f}, position));
    shape.vertices.emplace_back(MathUtils::addVertex({size, -size, 0, 1.f}, position));
    shape.vertices.emplace_back(MathUtils::addVertex({size, size, 0, 1.f}, position));
    shape.vertices.emplace_back(MathUtils::addVertex({-size, size, 0, 1.f}, position));

    shape.vertexIndex.emplace_back(std::array<uint32_t, 3>({vertexOffset + 0, vertexOffset + 2, vertexOffset + 1}));
    shape.normals.emplace_back(
        MathUtils::normalize(
            MathUtils::crossProduct(
                MathUtils::substractVertex(shape.vertices[vertexOffset + 0], shape.vertices[vertexOffset + 1]),
                MathUtils::substractVertex(shape.vertices[vertexOffset + 0], shape.vertices[vertexOffset + 2]))));
    shape.normalIndex.emplace_back(shape.normals.size() - 1);

    shape.vertexIndex.emplace_back(std::array<uint32_t, 3>({vertexOffset + 0, vertexOffset + 3, vertexOffset + 2}));
    shape.normalIndex.emplace_back(shape.normals.size() - 1);

    shape.transformedNormals.resize(shape.normals.size());

    shape.transformedVertices.resize(shape.vertices.size());
    shape.projectedVertices.resize(shape.vertices.size());

    shape.translate({0.f, 0.f, position.z});
}
void Shape::appendCube(Shape &shape, float cubeSize, PointF3 position)
{
    uint32_t vertexOffset = shape.vertices.size();

    shape.vertices.emplace_back(MathUtils::addVertex({-cubeSize, -cubeSize, cubeSize, 1.f}, position));
    shape.vertices.emplace_back(MathUtils::addVertex({cubeSize, -cubeSize, cubeSize, 1.f}, position));
    shape.vertices.emplace_back(MathUtils::addVertex({cubeSize, cubeSize, cubeSize, 1.f}, position));
    shape.vertices.emplace_back(MathUtils::addVertex({-cubeSize, cubeSize, cubeSize, 1.f}, position));

    shape.vertices.emplace_back(MathUtils::addVertex({-cubeSize, -cubeSize, -cubeSize, 1.f}, position));
    shape.vertices.emplace_back(MathUtils::addVertex({cubeSize, -cubeSize, -cubeSize, 1.f}, position));
    shape.vertices.emplace_back(MathUtils::addVertex({cubeSize, cubeSize, -cubeSize, 1.f}, position));
    shape.vertices.emplace_back(MathUtils::addVertex({-cubeSize, cubeSize, -cubeSize, 1.f}, position));

    shape.vertexIndex.emplace_back(std::array<uint32_t, 3>({vertexOffset + 0, vertexOffset + 2, vertexOffset + 1}));
    shape.normals.emplace_back(
        MathUtils::normalize(
            MathUtils::crossProduct(
                MathUtils::substractVertex(shape.vertices[vertexOffset + 0], shape.vertices[vertexOffset + 1]),
                MathUtils::substractVertex(shape.vertices[vertexOffset + 0], shape.vertices[vertexOffset + 2]))));
    shape.normalIndex.emplace_back(shape.normals.size() - 1);

    shape.vertexIndex.emplace_back(std::array<uint32_t, 3>({vertexOffset + 0, vertexOffset + 3, vertexOffset + 2}));
    shape.normals.emplace_back(
        MathUtils::normalize(
            MathUtils::crossProduct(
                MathUtils::substractVertex(shape.vertices[vertexOffset + 0], shape.vertices[vertexOffset + 2]),
                MathUtils::substractVertex(shape.vertices[vertexOffset + 0], shape.vertices[vertexOffset + 3]))));
    shape.normalIndex.emplace_back(shape.normals.size() - 1);

    shape.vertexIndex.emplace_back(std::array<uint32_t, 3>({vertexOffset + 4, vertexOffset + 5, vertexOffset + 6}));
    shape.normals.emplace_back(
        MathUtils::normalize(
            MathUtils::crossProduct(
                MathUtils::substractVertex(shape.vertices[vertexOffset + 4], shape.vertices[vertexOffset + 6]),
                MathUtils::substractVertex(shape.vertices[vertexOffset + 4], shape.vertices[vertexOffset + 5]))));
    shape.normalIndex.emplace_back(shape.normals.size() - 1);

    shape.vertexIndex.emplace_back(std::array<uint32_t, 3>({vertexOffset + 4, vertexOffset + 6, vertexOffset + 7}));
    shape.normals.emplace_back(
        MathUtils::normalize(
            MathUtils::crossProduct(
                MathUtils::substractVertex(shape.vertices[vertexOffset + 4], shape.vertices[vertexOffset + 7]),
                MathUtils::substractVertex(shape.vertices[vertexOffset + 4], shape.vertices[vertexOffset + 6]))));
    shape.normalIndex.emplace_back(shape.normals.size() - 1);

    shape.vertexIndex.emplace_back(std::array<uint32_t, 3>({vertexOffset + 0, vertexOffset + 4, vertexOffset + 3}));
    shape.normals.emplace_back(
        MathUtils::normalize(
            MathUtils::crossProduct(
                MathUtils::substractVertex(shape.vertices[vertexOffset + 0], shape.vertices[vertexOffset + 3]),
                MathUtils::substractVertex(shape.vertices[vertexOffset + 0], shape.vertices[vertexOffset + 4]))));
    shape.normalIndex.emplace_back(shape.normals.size() - 1);

    shape.vertexIndex.emplace_back(std::array<uint32_t, 3>({vertexOffset + 4, vertexOffset + 7, vertexOffset + 3}));
    shape.normals.emplace_back(
        MathUtils::normalize(
            MathUtils::crossProduct(
                MathUtils::substractVertex(shape.vertices[vertexOffset + 4], shape.vertices[vertexOffset + 3]),
                MathUtils::substractVertex(shape.vertices[vertexOffset + 4], shape.vertices[vertexOffset + 7]))));
    shape.normalIndex.emplace_back(shape.normals.size() - 1);

    shape.vertexIndex.emplace_back(std::array<uint32_t, 3>({vertexOffset + 4, vertexOffset + 0, vertexOffset + 1}));
    shape.normals.emplace_back(
        MathUtils::normalize(
            MathUtils::crossProduct(
                MathUtils::substractVertex(shape.vertices[vertexOffset + 4], shape.vertices[vertexOffset + 1]),
                MathUtils::substractVertex(shape.vertices[vertexOffset + 4], shape.vertices[vertexOffset + 0]))));
    shape.normalIndex.emplace_back(shape.normals.size() - 1);

    shape.vertexIndex.emplace_back(std::array<uint32_t, 3>({vertexOffset + 4, vertexOffset + 1, vertexOffset + 5}));
    shape.normals.emplace_back(
        MathUtils::normalize(
            MathUtils::crossProduct(
                MathUtils::substractVertex(shape.vertices[vertexOffset + 4], shape.vertices[vertexOffset + 5]),
                MathUtils::substractVertex(shape.vertices[vertexOffset + 4], shape.vertices[vertexOffset + 1]))));
    shape.normalIndex.emplace_back(shape.normals.size() - 1);

    shape.vertexIndex.emplace_back(std::array<uint32_t, 3>({vertexOffset + 2, vertexOffset + 6, vertexOffset + 5}));
    shape.normals.emplace_back(
        MathUtils::normalize(
            MathUtils::crossProduct(
                MathUtils::substractVertex(shape.vertices[vertexOffset + 2], shape.vertices[vertexOffset + 5]),
                MathUtils::substractVertex(shape.vertices[vertexOffset + 2], shape.vertices[vertexOffset + 6]))));
    shape.normalIndex.emplace_back(shape.normals.size() - 1);

    shape.vertexIndex.emplace_back(std::array<uint32_t, 3>({vertexOffset + 2, vertexOffset + 5, vertexOffset + 1}));
    shape.normals.emplace_back(
        MathUtils::normalize(
            MathUtils::crossProduct(
                MathUtils::substractVertex(shape.vertices[vertexOffset + 2], shape.vertices[vertexOffset + 1]),
                MathUtils::substractVertex(shape.vertices[vertexOffset + 2], shape.vertices[vertexOffset + 5]))));
    shape.normalIndex.emplace_back(shape.normals.size() - 1);

    shape.vertexIndex.emplace_back(std::array<uint32_t, 3>({vertexOffset + 3, vertexOffset + 6, vertexOffset + 2}));
    shape.normals.emplace_back(
        MathUtils::normalize(
            MathUtils::crossProduct(
                MathUtils::substractVertex(shape.vertices[vertexOffset + 3], shape.vertices[vertexOffset + 2]),
                MathUtils::substractVertex(shape.vertices[vertexOffset + 3], shape.vertices[vertexOffset + 6]))));
    shape.normalIndex.emplace_back(shape.normals.size() - 1);

    shape.vertexIndex.emplace_back(std::array<uint32_t, 3>({vertexOffset + 3, vertexOffset + 7, vertexOffset + 6}));
    shape.normals.emplace_back(
        MathUtils::normalize(
            MathUtils::crossProduct(
                MathUtils::substractVertex(shape.vertices[vertexOffset + 3], shape.vertices[vertexOffset + 6]),
                MathUtils::substractVertex(shape.vertices[vertexOffset + 3], shape.vertices[vertexOffset + 7]))));
    shape.normalIndex.emplace_back(shape.normals.size() - 1);

    shape.transformedNormals.resize(shape.normals.size());

    shape.transformedVertices.resize(shape.vertices.size());
    shape.projectedVertices.resize(shape.vertices.size());

    shape.translate({0.f, 0.f, position.z});
}

Shape Shape::createCube(float cubeSize = 50, float zPosition = 140)
{
    Shape shape(8);

    shape.vertices.push_back({-cubeSize, -cubeSize, cubeSize, 1.f});
    shape.vertices.push_back({cubeSize, -cubeSize, cubeSize, 1.f});
    shape.vertices.push_back({cubeSize, cubeSize, cubeSize, 1.f});
    shape.vertices.push_back({-cubeSize, cubeSize, cubeSize, 1.f});

    shape.vertices.push_back({-cubeSize, -cubeSize, -cubeSize, 1.f});
    shape.vertices.push_back({cubeSize, -cubeSize, -cubeSize, 1.f});
    shape.vertices.push_back({cubeSize, cubeSize, -cubeSize, 1.f});
    shape.vertices.push_back({-cubeSize, cubeSize, -cubeSize, 1.f});

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

    shape.normals.push_back({1, 0, 0, 0});
    shape.normals.push_back({0, 1, 0, 0});
    shape.normals.push_back({0, 0, 1, 0});

    shape.normals.push_back({-1, 0, 0, 0});
    shape.normals.push_back({0, -1, 0, 0});
    shape.normals.push_back({0, 0, -1, 0});

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
