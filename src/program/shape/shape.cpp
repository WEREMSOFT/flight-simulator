#include "shape.hpp"
#include <iostream>
#include <cmath>
#include <array>
#include <algorithm>
#include <vector>
#include <string>
#include <list>
#include "../math/mathUtils.hpp"

#define ANGLE_RATIO 3.1416 * 255

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

bool Shape::isBackFace(PointF3 normal)
{
    auto angle = MathUtils::dotProduct(normal, {0, 0, 1});
    return angle > 0;
}

bool Shape::sortTriangleZ(PointF3 a, PointF3 b)
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
        std::sort(triangle.begin(), triangle.end(), Shape::sortTriangleZ);
        triangle[1] = MathUtils::intersectionPoint(MathUtils::substractVertex(triangle[0], triangle[1]), triangle[1], {0, 0, 1}, {0, 0, z});
        triangle[2] = MathUtils::intersectionPoint(MathUtils::substractVertex(triangle[0], triangle[2]), triangle[2], {0, 0, 1}, {0, 0, z});
        triangles.emplace_back(triangle);
        localNormalIndex.emplace_back(normalIndex);
        return;
    }

    if (vertexInside == 2)
    {
        std::sort(triangle.begin(), triangle.end(), Shape::sortTriangleZ);
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

void Shape::draw(ImageData &pImageData, Camera camera)
{
    TrianglesF clippedTriangles;
    TrianglesI projectedVerticesLocal;
    std::vector<uint32_t> localNormalIndex;

    recalculateTransformMatrix();
    transform();

    int i = 0;
    for (auto &index : vertexIndex)
    {
        PointF3 p1 = {transformedVertices[index[0]].x, transformedVertices[index[0]].y, transformedVertices[index[0]].z};
        PointF3 p2 = {transformedVertices[index[1]].x, transformedVertices[index[1]].y, transformedVertices[index[1]].z};
        PointF3 p3 = {transformedVertices[index[2]].x, transformedVertices[index[2]].y, transformedVertices[index[2]].z};
        clipTriangle(clippedTriangles, {p1, p2, p3}, camera.zNear, localNormalIndex, normalIndex[i++]);
    }

    project(projectedVerticesLocal, clippedTriangles, 100);

    for (int i = 0; i < projectedVerticesLocal.size(); i++)
    {
        auto triangle = projectedVerticesLocal[i];
        auto transformedNormal = transformedNormals[localNormalIndex[i]];
        if (!backFaceCulingDisabled)
            if (isBackFace(transformedNormal))
            {
                continue;
            }

        if (drawNormals)
        {
            auto transformedNormalX = static_cast<int>(transformedNormal.x * 10);
            auto transformedNormalY = static_cast<int>(transformedNormal.y * 10);

            pImageData.drawLine({triangle[0].x, triangle[0].y}, {triangle[0].x + transformedNormalX, triangle[0].y + transformedNormalY});
            pImageData.drawLine({triangle[1].x, triangle[1].y}, {triangle[1].x + transformedNormalX, triangle[1].y + transformedNormalY});
            pImageData.drawLine({triangle[2].x, triangle[2].y}, {triangle[2].x + transformedNormalX, triangle[2].y + transformedNormalY});
        }

        if (wireFrame)
        {
            pImageData.drawLine(triangle[0], triangle[1], {255, 0, 0});
            pImageData.drawLine(triangle[1], triangle[2], {0, 255, 0});
            pImageData.drawLine(triangle[2], triangle[0], {0, 0, 255});
            continue;
        }
        Color color = {0, 0, 255};
        auto component = MathUtils::angleBetweenVectors(transformedNormal, {1, 1, 1}) / ANGLE_RATIO;
        color = {static_cast<unsigned char>(component),
                 static_cast<unsigned char>(0),
                 static_cast<unsigned char>(255 - component)};
        rasterizeTriangle(triangle, pImageData, color);
        if (showVertexNumber)
        {
            char numberString[50] = {0};
            snprintf(numberString, 50, "%d", i);
            pImageData.printString(triangle[0], numberString);
            snprintf(numberString, 50, "%d", i);
            pImageData.printString(triangle[1], numberString);
            snprintf(numberString, 50, "%d", i);
            pImageData.printString(triangle[2], numberString);
        }
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
}

void Shape::scale(PointF3 scale)
{
    MathUtils::setMatrixAsIdentity(scaleMatrix);
    scaleMatrix[0].x = scale.x;
    scaleMatrix[1].y = scale.y;
    scaleMatrix[2].z = scale.z;
    scaleMatrix[3].w = 1.f;
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
