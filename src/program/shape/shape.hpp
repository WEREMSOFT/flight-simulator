#pragma once
#include <vector>
#include <array>
#include "../math/mathUtils.hpp"
#include "../../core/graphics/imageData/imagedata.hpp"
#include "../camera/camera.hpp"
#include "../object3D/object3d.hpp"

class Shape : public Object3D
{
    static bool isBackFace(PointF3 normal);
    static bool sortTriangleZ(PointF3 a, PointF3 b);
    void transform();

public:
    bool wireFrame = false;
    bool backFaceCulingDisabled = false;
    bool showVertexNumber = false;
    bool drawNormals = false;
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
    void clipTriangle(TrianglesF &triangles, TriangleF triangle, float z, std::vector<uint32_t> &localNormalIndex, int32_t normalIndex);

    static Shape createCube(float cubeSize, float zPosition);
    static Shape createPyramid(float baseSize = 50, float height = 50, float zPosition = 140);
    static void appendPiramid(Shape &shape, float baseSize, float height, PointF3 zPosition);
    static void appendCube(Shape &shape, float cubeSize, PointF3 zPosition);
    static void appendQuad(Shape &shape, float cubeSize, PointF3 zPosition);
    static void appendWall(Shape &shape, float size, PointF3 position);
};