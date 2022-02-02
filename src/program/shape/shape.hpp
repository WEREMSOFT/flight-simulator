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
    bool isBackFace(PointF3 normal, PointF3 vector);
    static bool sortTriangleZ(PointF3 a, PointF3 b);
    static bool sortTriangleX(PointF3 a, PointF3 b);
    static bool sortTriangleXRight(PointF3 a, PointF3 b);
    void transform();

public:
    Color difuseColor;
    std::vector<PointF3> vertices;
    std::vector<PointF3> normals;
    std::vector<std::array<uint32_t, 3>> vertexIndex;
    std::vector<uint32_t> normalIndex;
    std::vector<PointF3> transformedVertices;
    std::vector<PointF3> transformedNormals;
    std::vector<PointF3> projectedVertices;

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
                             std::function<bool(PointF3, float)> compareFunc,
                             std::function<bool(PointF3, PointF3)> sortFunc,
                             PointF3 planeNormal,
                             PointF3 planePoint);

    static Shape createCube(float cubeSize, float zPosition);
    static Shape createPyramid(float baseSize = 50, float height = 50, float zPosition = 140);
    static void appendPiramid(Shape &shape, float baseSize, float height, PointF3 zPosition);
    static void appendCube(Shape &shape, float cubeSize, PointF3 zPosition);
    static void appendQuad(Shape &shape, float cubeSize, PointF3 zPosition);
    static void appendWall(Shape &shape, float size, PointF3 position);
    static void appendCircle(Shape &shape, float radius, int sides, PointF3 position);
};