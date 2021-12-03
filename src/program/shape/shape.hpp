#pragma once
#include <vector>
#include "../../core/graphics/imageData/imagedata.hpp"

class Shape
{
    PointF3 transformMatrix[3] = {0};
    PointF3 translationMatrix[3] = {0};
    PointF3 scaleMatrix[3] = {0};
    std::vector<PointF3> transformedVertices;
    void transform();
    void recalculateTranslationMatrix();
    bool isTransformDirty = true;

public:
    std::vector<PointF3> vertices;
    Shape(int vertexNum);
    ~Shape();
    void draw(ImageData &pImageData);
    void translate(PointF3);
    void scale(PointF3);
    void rotateZ(float angle);
};