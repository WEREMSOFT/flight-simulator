#pragma once
#include <vector>
#include "../../core/graphics/imageData/imagedata.hpp"

class Shape
{
    PointF3 transformMatrix[4] = {0};
    PointF3 translationMatrix[4] = {0};
    PointF3 rotationMatrix[4] = {0};
    PointF3 scaleMatrix[4] = {0};
    std::vector<PointF3> transformedVertices;
    void transform();
    void recalculateTransformMatrix();
    bool isTransformDirty = true;

public:
    std::vector<PointF3> vertices;
    Shape(int vertexNum);
    ~Shape();
    void draw(ImageData &pImageData);
    void translate(PointF3);
    void scale(PointF3);
    void rotateZ(float angle);
    void project(float distance);
};