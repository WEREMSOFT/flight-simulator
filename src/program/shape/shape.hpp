#pragma once
#include <vector>
#include <array>
#include "../../core/graphics/imageData/imagedata.hpp"

class Shape
{
    PointF3 transformMatrix[4] = {0};
    PointF3 translationMatrix[4] = {0};
    PointF3 rotationMatrix[4] = {0};
    PointF3 scaleMatrix[4] = {0};
    void transform();
    void recalculateTransformMatrix();
    bool isTransformDirty = true;

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
    void draw(ImageData &pImageData);
    void translate(PointF3);
    void scale(PointF3);
    void rotateZ(float angle);
    void rotate(float x, float y, float z);
    void project(float distance);
    void rasterizeTriangle(std::array<PointI, 3> triangle, ImageData &pImageData, Color color);

    static Shape createCube(float cubeSize, float zPosition);
    static Shape createPyramid(float baseSize = 50, float height = 50, float zPosition = 140);
    static void appendPiramid(Shape &shape, float baseSize, float height, PointF3 zPosition);
    static void appendCube(Shape &shape, float cubeSize, PointF3 zPosition);
};