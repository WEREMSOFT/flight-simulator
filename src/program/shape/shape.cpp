#include "shape.hpp"
#include <iostream>
#include <cmath>
#include <array>
#include <algorithm>
#include <vector>
#include <string>
#include <list>
#include "../../core/math/vector/vector3.hpp"

#define ANGLE_RATIO 3.1416 * 255

Shape::Shape(int vertexNum)
{
    difuseColor = {0xFF, 0xFF, 0xFF};
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

bool Shape::isBackFace(PointF normal, PointF vector)
{
    auto angle = acosf(normal.normalize().dot(vector.normalize()));
    return angle < 1.4f;
}

void Shape::clipTriangleGeneric(TrianglesF &triangles,
                                TriangleF triangle,
                                float boundary,
                                std::vector<uint32_t> &localNormalIndex,
                                int32_t normalIndex,
                                std::function<bool(PointF, float)> compareFunc,
                                std::function<bool(PointF, PointF)> sortFunc,
                                PointF planeNormal,
                                PointF planePoint)
{
    int vertexInside = 0;
    for (auto &vertex : triangle)
    {
        if (compareFunc(vertex, boundary))
            vertexInside++;
    }
    // discard triangle
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
        std::sort(triangle.begin(), triangle.end(), sortFunc);
        // the normal of the zNear plane is hardcoded. Also the vector that locates the plane.
        triangle[1] = MathUtils::intersectionPoint((triangle[0] - triangle[1]), triangle[1], planeNormal, planePoint);
        triangle[2] = MathUtils::intersectionPoint((triangle[0] - triangle[2]), triangle[2], planeNormal, planePoint);
        triangles.emplace_back(triangle);
        localNormalIndex.emplace_back(normalIndex);
        return;
    }

    if (vertexInside == 2)
    {
        std::sort(triangle.begin(), triangle.end(), sortFunc);
        TriangleF newTriangle = {0};
        newTriangle[0] = triangle[1];
        newTriangle[1] = MathUtils::intersectionPoint((triangle[0] - triangle[2]), triangle[2], planeNormal, planePoint);
        newTriangle[2] = MathUtils::intersectionPoint((triangle[1] - triangle[2]), triangle[1], planeNormal, planePoint);

        triangle[2] = MathUtils::intersectionPoint((triangle[0] - triangle[2]), triangle[2], planeNormal, planePoint);
        triangles.emplace_back(triangle);
        localNormalIndex.emplace_back(normalIndex);

        triangles.emplace_back(newTriangle);
        localNormalIndex.emplace_back(normalIndex);
    }
}

void Shape::update(void)
{
    Object3D::recalculateTransformMatrix();
    transform();
}

void Shape::draw(ImageData &pImageData, Camera camera)
{
    TrianglesF clippedTriangles;
    TrianglesF clippedTriangles2;

    TrianglesI projectedVerticesLocal;
    std::vector<uint32_t> localNormalIndex;
    std::vector<uint32_t> localNormalIndex2;
    std::vector<PointF> transformedVerticesLocal;

    transformedVerticesLocal.reserve(transformedVertices.size());

    for (auto i = 0; i < transformedVertices.size(); i++)
    {
        MathUtils::multiplyVertexByMatrix(transformedVerticesLocal[i], transformedVertices[i], camera.transformMatrix);
    }

    PointF normal = {1, 0, 0};
    normal = normal.normalize();
    for (int i = 0; i < vertexIndex.size(); i++)
    {
        auto index = vertexIndex[i];
        PointF p1 = {transformedVerticesLocal[index[0]].x, transformedVerticesLocal[index[0]].y, transformedVerticesLocal[index[0]].z};
        PointF p2 = {transformedVerticesLocal[index[1]].x, transformedVerticesLocal[index[1]].y, transformedVerticesLocal[index[1]].z};
        PointF p3 = {transformedVerticesLocal[index[2]].x, transformedVerticesLocal[index[2]].y, transformedVerticesLocal[index[2]].z};
        clipTriangleGeneric(
            clippedTriangles, {p1, p2, p3}, camera.frustrum.x, localNormalIndex, normalIndex[i], [](PointF a, float b) -> bool
            { return a.x > b; },
            [](PointF a, PointF b) -> bool
            { return a.x > b.x; },
            normal,
            {camera.frustrum.x, 0, 0});
    }

    normal = {-1, 0, 0};
    normal = normal.normalize();
    for (int i = 0; i < clippedTriangles.size(); i++)
    {
        clipTriangleGeneric(
            clippedTriangles2, clippedTriangles[i], camera.frustrum.w, localNormalIndex2, localNormalIndex[i], [](PointF a, float b) -> bool
            { return a.x < b; },
            [](PointF a, PointF b) -> bool
            { return a.x < b.x; },
            normal,
            {camera.frustrum.w, 0, 0});
    }

    clippedTriangles.clear();
    localNormalIndex.clear();

    for (int i = 0; i < clippedTriangles2.size(); i++)
    {
        clipTriangleGeneric(
            clippedTriangles, clippedTriangles2[i], camera.frustrum.z, localNormalIndex, localNormalIndex2[i], [](PointF a, float b) -> bool
            { return a.z > b; },
            [](PointF a, PointF b) -> bool
            { return a.z > b.z; },
            {0, 0, 1},
            {0, 0, camera.frustrum.z});
    }

    project(projectedVerticesLocal, clippedTriangles, 100);

    for (int i = 0; i < projectedVerticesLocal.size(); i++)
    {
        auto triangle = projectedVerticesLocal[i];
        auto transformedNormal = transformedNormals[localNormalIndex[i]];
        if (camera.backFaceCulling)
            if (isBackFace(transformedNormal, camera.getForwardVector()))
            {
                continue;
            }

        if (camera.drawNormals)
        {
            auto transformedNormalX = static_cast<int>(transformedNormal.x * 10);
            auto transformedNormalY = static_cast<int>(transformedNormal.y * 10);

            pImageData.drawLine({triangle[0].x, triangle[0].y}, {triangle[0].x + transformedNormalX, triangle[0].y + transformedNormalY});
            pImageData.drawLine({triangle[1].x, triangle[1].y}, {triangle[1].x + transformedNormalX, triangle[1].y + transformedNormalY});
            pImageData.drawLine({triangle[2].x, triangle[2].y}, {triangle[2].x + transformedNormalX, triangle[2].y + transformedNormalY});
        }

        if (camera.wireframe)
        {
            pImageData.drawLine(triangle[0], triangle[1], {255, 0, 0});
            pImageData.drawLine(triangle[1], triangle[2], {0, 255, 0});
            pImageData.drawLine(triangle[2], triangle[0], {0, 0, 255});
            continue;
        }
        Color color = {0, 0, 255};
        auto component = transformedNormal.angleWith({1, 1, 1}) / M_PI;
        color = {static_cast<unsigned char>(difuseColor.r * component),
                 static_cast<unsigned char>(difuseColor.g * component),
                 static_cast<unsigned char>(difuseColor.b * component)};

        Triangle<int32_t> triangleI = {triangle, color};

        rasterizeTriangle(triangleI, pImageData);
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

void Shape::appendPiramid(Shape &shape, float baseSize, float height, PointF position)
{
    uint32_t vertexOffset = shape.vertices.size();
    shape.vertices.emplace_back(position + (PointF){-baseSize, 0, -baseSize, 1.f});
    shape.vertices.emplace_back(position + (PointF){-baseSize, 0, baseSize, 1.f});
    shape.vertices.emplace_back(position + (PointF){baseSize, 0, baseSize, 1.f});
    shape.vertices.emplace_back(position + (PointF){baseSize, 0, -baseSize, 1.f});
    shape.vertices.emplace_back(position + (PointF){0, -height, 0, 1.f});

    shape.vertexIndex.emplace_back(std::array<uint32_t, 3>({vertexOffset + 0, vertexOffset + 2, vertexOffset + 1}));
    shape.normals.emplace_back((shape.vertices[vertexOffset + 0] - shape.vertices[vertexOffset + 1]).cross((shape.vertices[vertexOffset + 0] - shape.vertices[vertexOffset + 2])).normalize());
    shape.normalIndex.emplace_back(shape.normals.size() - 1);

    shape.vertexIndex.emplace_back(std::array<uint32_t, 3>({vertexOffset + 0, vertexOffset + 3, vertexOffset + 2}));
    shape.normals.emplace_back((shape.vertices[vertexOffset + 0] - shape.vertices[vertexOffset + 2]).cross((shape.vertices[vertexOffset + 0] - shape.vertices[vertexOffset + 3])).normalize());
    shape.normalIndex.emplace_back(shape.normals.size() - 1);

    shape.vertexIndex.emplace_back(std::array<uint32_t, 3>({vertexOffset + 4, vertexOffset + 1, vertexOffset + 2}));
    shape.normals.emplace_back((shape.vertices[vertexOffset + 4] - shape.vertices[vertexOffset + 2]).cross((shape.vertices[vertexOffset + 4] - shape.vertices[vertexOffset + 1])).normalize());
    shape.normalIndex.emplace_back(shape.normals.size() - 1);

    shape.vertexIndex.emplace_back(std::array<uint32_t, 3>({vertexOffset + 4, vertexOffset + 0, vertexOffset + 1}));
    shape.normals.emplace_back((shape.vertices[vertexOffset + 4] - shape.vertices[vertexOffset + 1]).cross((shape.vertices[vertexOffset + 4] - shape.vertices[vertexOffset + 0])).normalize());
    shape.normalIndex.emplace_back(shape.normals.size() - 1);

    shape.vertexIndex.emplace_back(std::array<uint32_t, 3>({vertexOffset + 4, vertexOffset + 3, vertexOffset + 0}));
    shape.normals.emplace_back((shape.vertices[vertexOffset + 4] - shape.vertices[vertexOffset + 0]).cross((shape.vertices[vertexOffset + 4] - shape.vertices[vertexOffset + 3])).normalize());
    shape.normalIndex.emplace_back(shape.normals.size() - 1);

    shape.vertexIndex.emplace_back(std::array<uint32_t, 3>({vertexOffset + 4, vertexOffset + 2, vertexOffset + 3}));
    shape.normals.emplace_back((shape.vertices[vertexOffset + 4] - shape.vertices[vertexOffset + 3]).cross((shape.vertices[vertexOffset + 4] - shape.vertices[vertexOffset + 2])).normalize());
    shape.normalIndex.emplace_back(shape.normals.size() - 1);

    shape.transformedNormals.resize(shape.normals.size());

    shape.transformedVertices.resize(shape.vertices.size());
    shape.projectedVertices.resize(shape.vertices.size());

    shape.translate({0.f, 0.f, position.z});
}

void Shape::appendQuad(Shape &shape, float cubeSize, PointF position)
{
    uint32_t vertexOffset = shape.vertices.size();

    shape.vertices.emplace_back(position + (PointF){-cubeSize, -cubeSize, 0, 1.f});
    shape.vertices.emplace_back(position + (PointF){cubeSize, -cubeSize, 0, 1.f});
    shape.vertices.emplace_back(position + (PointF){cubeSize, cubeSize, 0, 1.f});
    shape.vertices.emplace_back(position + (PointF){-cubeSize, cubeSize, 0, 1.f});

    shape.vertexIndex.emplace_back(std::array<uint32_t, 3>({vertexOffset + 0, vertexOffset + 2, vertexOffset + 1}));
    shape.normals.emplace_back(
        (shape.vertices[vertexOffset + 0] - shape.vertices[vertexOffset + 2]).cross((shape.vertices[vertexOffset + 0] - shape.vertices[vertexOffset + 1])).normalize());
    shape.normalIndex.emplace_back(shape.normals.size() - 1);

    shape.vertexIndex.emplace_back(std::array<uint32_t, 3>({vertexOffset + 0, vertexOffset + 3, vertexOffset + 2}));
    shape.normals.emplace_back(
        (shape.vertices[vertexOffset + 0] - shape.vertices[vertexOffset + 3]).cross((shape.vertices[vertexOffset + 0] - shape.vertices[vertexOffset + 2])).normalize());
    shape.normalIndex.emplace_back(shape.normals.size() - 1);

    shape.transformedNormals.resize(shape.normals.size());
    shape.transformedVertices.resize(shape.vertices.size());
    shape.projectedVertices.resize(shape.vertices.size());

    shape.translate({0.f, 0.f, position.z});
}
void Shape::appendWall(Shape &shape, float size, PointF position)
{
    uint32_t vertexOffset = shape.vertices.size();

    shape.vertices.emplace_back(position + (PointF){-size, -size, 0, 1.f});
    shape.vertices.emplace_back(position + (PointF){size, -size, 0, 1.f});
    shape.vertices.emplace_back(position + (PointF){size, size, 0, 1.f});
    shape.vertices.emplace_back(position + (PointF){-size, size, 0, 1.f});

    shape.vertexIndex.emplace_back(std::array<uint32_t, 3>({vertexOffset + 0, vertexOffset + 2, vertexOffset + 1}));
    shape.normals.emplace_back((shape.vertices[vertexOffset + 0] - shape.vertices[vertexOffset + 1]).cross((shape.vertices[vertexOffset + 0] - shape.vertices[vertexOffset + 2])).normalize());
    shape.normalIndex.emplace_back(shape.normals.size() - 1);

    shape.vertexIndex.emplace_back(std::array<uint32_t, 3>({vertexOffset + 0, vertexOffset + 3, vertexOffset + 2}));
    shape.normalIndex.emplace_back(shape.normals.size() - 1);

    shape.transformedNormals.resize(shape.normals.size());

    shape.transformedVertices.resize(shape.vertices.size());
    shape.projectedVertices.resize(shape.vertices.size());

    shape.translate({0.f, 0.f, position.z});
}
void Shape::appendCube(Shape &shape, float cubeSize, PointF position)
{
    uint32_t vertexOffset = shape.vertices.size();

    shape.vertices.emplace_back(position + (PointF){-cubeSize, -cubeSize, cubeSize, 1.f});
    shape.vertices.emplace_back(position + (PointF){cubeSize, -cubeSize, cubeSize, 1.f});
    shape.vertices.emplace_back(position + (PointF){cubeSize, cubeSize, cubeSize, 1.f});
    shape.vertices.emplace_back(position + (PointF){-cubeSize, cubeSize, cubeSize, 1.f});

    shape.vertices.emplace_back(position + (PointF){-cubeSize, -cubeSize, -cubeSize, 1.f});
    shape.vertices.emplace_back(position + (PointF){cubeSize, -cubeSize, -cubeSize, 1.f});
    shape.vertices.emplace_back(position + (PointF){cubeSize, cubeSize, -cubeSize, 1.f});
    shape.vertices.emplace_back(position + (PointF){-cubeSize, cubeSize, -cubeSize, 1.f});

    shape.vertexIndex.emplace_back(std::array<uint32_t, 3>({vertexOffset + 0, vertexOffset + 2, vertexOffset + 1}));
    shape.normals.emplace_back(

        (shape.vertices[vertexOffset + 0] - shape.vertices[vertexOffset + 1]).cross((shape.vertices[vertexOffset + 0] - shape.vertices[vertexOffset + 2])).normalize());
    shape.normalIndex.emplace_back(shape.normals.size() - 1);

    shape.vertexIndex.emplace_back(std::array<uint32_t, 3>({vertexOffset + 0, vertexOffset + 3, vertexOffset + 2}));
    shape.normals.emplace_back(
        (shape.vertices[vertexOffset + 0] - shape.vertices[vertexOffset + 2]).cross((shape.vertices[vertexOffset + 0] - shape.vertices[vertexOffset + 3])).normalize());
    shape.normalIndex.emplace_back(shape.normals.size() - 1);

    shape.vertexIndex.emplace_back(std::array<uint32_t, 3>({vertexOffset + 4, vertexOffset + 5, vertexOffset + 6}));
    shape.normals.emplace_back(

        (shape.vertices[vertexOffset + 4] - shape.vertices[vertexOffset + 6]).cross((shape.vertices[vertexOffset + 4] - shape.vertices[vertexOffset + 5])).normalize());
    shape.normalIndex.emplace_back(shape.normals.size() - 1);

    shape.vertexIndex.emplace_back(std::array<uint32_t, 3>({vertexOffset + 4, vertexOffset + 6, vertexOffset + 7}));
    shape.normals.emplace_back(
        (shape.vertices[vertexOffset + 4] - shape.vertices[vertexOffset + 7]).cross((shape.vertices[vertexOffset + 4] - shape.vertices[vertexOffset + 6])).normalize());
    shape.normalIndex.emplace_back(shape.normals.size() - 1);

    shape.vertexIndex.emplace_back(std::array<uint32_t, 3>({vertexOffset + 0, vertexOffset + 4, vertexOffset + 3}));
    shape.normals.emplace_back(
        (shape.vertices[vertexOffset + 0] - shape.vertices[vertexOffset + 3]).cross((shape.vertices[vertexOffset + 0] - shape.vertices[vertexOffset + 4])).normalize());
    shape.normalIndex.emplace_back(shape.normals.size() - 1);

    shape.vertexIndex.emplace_back(std::array<uint32_t, 3>({vertexOffset + 4, vertexOffset + 7, vertexOffset + 3}));
    shape.normals.emplace_back((shape.vertices[vertexOffset + 4] - shape.vertices[vertexOffset + 3]).cross((shape.vertices[vertexOffset + 4] - shape.vertices[vertexOffset + 7])).normalize());
    shape.normalIndex.emplace_back(shape.normals.size() - 1);

    shape.vertexIndex.emplace_back(std::array<uint32_t, 3>({vertexOffset + 4, vertexOffset + 0, vertexOffset + 1}));
    shape.normals.emplace_back((shape.vertices[vertexOffset + 4] - shape.vertices[vertexOffset + 1]).cross((shape.vertices[vertexOffset + 4] - shape.vertices[vertexOffset + 0])).normalize());
    shape.normalIndex.emplace_back(shape.normals.size() - 1);

    shape.vertexIndex.emplace_back(std::array<uint32_t, 3>({vertexOffset + 4, vertexOffset + 1, vertexOffset + 5}));
    shape.normals.emplace_back((shape.vertices[vertexOffset + 4] - shape.vertices[vertexOffset + 5]).cross((shape.vertices[vertexOffset + 4] - shape.vertices[vertexOffset + 1])).normalize());
    shape.normalIndex.emplace_back(shape.normals.size() - 1);

    shape.vertexIndex.emplace_back(std::array<uint32_t, 3>({vertexOffset + 2, vertexOffset + 6, vertexOffset + 5}));
    shape.normals.emplace_back(
        (shape.vertices[vertexOffset + 2] - shape.vertices[vertexOffset + 5]).cross((shape.vertices[vertexOffset + 2] - shape.vertices[vertexOffset + 6])).normalize());
    shape.normalIndex.emplace_back(shape.normals.size() - 1);

    shape.vertexIndex.emplace_back(std::array<uint32_t, 3>({vertexOffset + 2, vertexOffset + 5, vertexOffset + 1}));
    shape.normals.emplace_back((shape.vertices[vertexOffset + 2] - shape.vertices[vertexOffset + 1]).cross((shape.vertices[vertexOffset + 2] - shape.vertices[vertexOffset + 5])).normalize());
    shape.normalIndex.emplace_back(shape.normals.size() - 1);

    shape.vertexIndex.emplace_back(std::array<uint32_t, 3>({vertexOffset + 3, vertexOffset + 6, vertexOffset + 2}));
    shape.normals.emplace_back(
        (shape.vertices[vertexOffset + 3] - shape.vertices[vertexOffset + 2]).cross((shape.vertices[vertexOffset + 3] - shape.vertices[vertexOffset + 6])).normalize());
    shape.normalIndex.emplace_back(shape.normals.size() - 1);

    shape.vertexIndex.emplace_back(std::array<uint32_t, 3>({vertexOffset + 3, vertexOffset + 7, vertexOffset + 6}));
    shape.normals.emplace_back((shape.vertices[vertexOffset + 3] - shape.vertices[vertexOffset + 6]).cross((shape.vertices[vertexOffset + 3] - shape.vertices[vertexOffset + 7])).normalize());
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

void Shape::rasterizeTriangle(Triangle<int32_t> triangle, ImageData &pImageData)
{
    std::sort(triangle.vertices.begin(), triangle.vertices.end(), sortTriangleY);

    float dy = (float)(triangle.vertices[1].y - triangle.vertices[0].y);
    float dx = (float)(triangle.vertices[2].y - triangle.vertices[0].y);
    float incrementY = dy ? (float)(triangle.vertices[1].x - triangle.vertices[0].x) / dy : 0;
    float incrementXLimit = dx ? (float)(triangle.vertices[2].x - triangle.vertices[0].x) / dx : 0;
    float start = 0;
    float end = 0;
    int height = triangle.vertices[1].y - triangle.vertices[0].y;

    for (int i = 0; i < height; i++)
    {
        int localStart = floor(start);
        int localEnd = floor(end);

        pImageData.drawLineZ({triangle.vertices[0].x + localStart, triangle.vertices[0].y + i}, {triangle.vertices[0].x + localEnd, triangle.vertices[0].y + i}, triangle.vertices, triangle.color);

        start += incrementXLimit;
        end += incrementY;
    }

    if (height != 0)
    {
        dy = (float)(triangle.vertices[2].y - triangle.vertices[1].y);
        incrementY = dy ? (float)(triangle.vertices[2].x - triangle.vertices[1].x) / dy : 0;
    }
    else
    {
        incrementY = incrementXLimit;
        dy = (float)(triangle.vertices[2].y - triangle.vertices[1].y);
        incrementXLimit = dy ? (float)(triangle.vertices[2].x - triangle.vertices[1].x) / dy : 0;
        start = triangle.vertices[1].x - triangle.vertices[0].x;
    }
    height = triangle.vertices[2].y - triangle.vertices[1].y;

    for (int i = 0; i < height; i++)
    {
        int localStart = floor(start);
        int localEnd = floor(end);
        pImageData.drawLineZ({triangle.vertices[0].x + localStart, triangle.vertices[1].y + i}, {triangle.vertices[0].x + localEnd, triangle.vertices[1].y + i}, triangle.vertices, triangle.color);
        start += incrementXLimit;
        end += incrementY;
    }

    return;
}

void Shape::appendCircle(Shape &shape, float radius, int sides, PointF position)
{
    float phaseIncrement = 3.1416 / sides;
    float phase = 0;
    shape.vertices.emplace_back(position);
    for (int i = 0; i < sides; i++)
    {
        PointF vertex = {static_cast<float>(sin(phase) * radius), 0, static_cast<float>(cos(phase) * radius)};
        phase += phaseIncrement;
        shape.vertices.emplace_back(vertex);
    }
    shape.normals.push_back({0.f, 1.f, 0.f, 0.f});
    shape.normalIndex.emplace_back(shape.normals.size() - 1);
}
