#pragma once
#include <array>
#include <vector>
#include <cmath>

#include "../../core/graphics/imageData/imagedata.hpp"

using TriangleI = std::array<PointI, 3>;
using TriangleF = std::array<PointF3, 3>;
using TrianglesI = std::vector<TriangleI>;
using TrianglesF = std::vector<TriangleF>;

namespace MathUtils
{
    PointF3 scaleVector(PointF3 v, float scalar);
    PointF3 addVertex(PointF3 u, PointF3 v);
    PointF3 substractVertex(PointF3 u, PointF3 v);
    PointF3 crossProduct(PointF3 u, PointF3 v);
    double dotProduct(PointF3 a, PointF3 b);
    float length(PointF3 v);
    PointF3 normalize(PointF3 v);
    PointF3 intersectionPoint(PointF3 lineVector, PointF3 linePoint, PointF3 planeNormal, PointF3 planePoint);
    void multiplyVertexByMatrix(PointF3 &vertDestination, PointF3 &vertSource, PointF3 mat[4]);
    void setMatrixAsIdentity(PointF3 matrix[4]);
    void copyMatrix(PointF3 destination[4], PointF3 source[4]);
    void multiplyMatrix(PointF3 mat1[4], PointF3 mat2[4]);
    float angleBetweenVectors(PointF3 a, PointF3 b);
}
