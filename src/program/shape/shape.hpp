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
    std::vector<PointF3> vertices;
    std::vector<std::array<uint32_t, 3>> vertexIndex;
    std::vector<PointF3> transformedVertices;
    std::vector<PointF3> projectedVertices;
    Shape(int vertexNum);
    ~Shape();
    void draw(ImageData &pImageData);
    void translate(PointF3);
    void scale(PointF3);
    void rotateZ(float angle);
    void rotate(float x, float y, float z);
    void project(float distance);
};