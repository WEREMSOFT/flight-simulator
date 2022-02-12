#pragma once
#include <vector>
#include <array>
#include "../../core/math/mathUtils.hpp"
#include "../../core/graphics/imageData/imagedata.hpp"
#include "../camera/camera.hpp"
#include "../object3D/object3d.hpp"
#include <functional>

class Shape : public Object3D
{
    bool isBackFace(PointF normal, PointF vector);
    static bool sortTriangleZ(PointF a, PointF b);
    static bool sortTriangleX(PointF a, PointF b);
    static bool sortTriangleXRight(PointF a, PointF b);
    void transform();

public:
    Color difuseColor;
    std::vector<PointF> vertices;
    std::vector<PointF> normals;
    std::vector<std::array<uint32_t, 3>> vertexIndex;
    std::vector<uint32_t> normalIndex;
    std::vector<PointF> transformedVertices;
    std::vector<PointF> transformedNormals;
    std::vector<PointF> projectedVertices;

    Shape(int vertexNum);
    ~Shape();

    void draw(ImageData &pImageData, Camera camera);
    void update(void);

    void project(float distance);
    void project(TrianglesI &pProjectedVertices, TrianglesF &triangles, float distance);

    void rasterizeTriangle(Triangle<int32_t> triangle, ImageData &pImageData);
    void clipTriangleGeneric(TrianglesF &triangles,
                             TriangleF triangle,
                             float boundary,
                             std::vector<uint32_t> &localNormalIndex,
                             int32_t normalIndex,
                             std::function<bool(PointF, float)> compareFunc,
                             std::function<bool(PointF, PointF)> sortFunc,
                             PointF planeNormal,
                             PointF planePoint);

    static Shape createCube(float cubeSize, float zPosition);
    static Shape createPyramid(float baseSize = 50, float height = 50, float zPosition = 140);
    static void appendPiramid(Shape &shape, float baseSize, float height, PointF zPosition);
    static void appendCube(Shape &shape, float cubeSize, PointF zPosition);
    static void appendQuad(Shape &shape, float cubeSize, PointF zPosition);
    static void appendWall(Shape &shape, float size, PointF position);
    static void appendCircle(Shape &shape, float radius, int sides, PointF position);
};