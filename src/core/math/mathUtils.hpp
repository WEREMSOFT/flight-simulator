#pragma once
#include <array>
#include <vector>
#include <cmath>

#include "../../core/graphics/imageData/imagedata.hpp"

using TriangleI = std::array<PointI, 3>;
using TriangleF = std::array<PointF3, 3>;
using TrianglesI = std::vector<TriangleI>;
using TrianglesF = std::vector<TriangleF>;

template <typename T>
class Triangle
{
public:
    std::array<Point3<T>, 3> vertices;
    Color color;
};

namespace MathUtils
{
    template <typename T>
    T map(T a, double end1, double end2)
    {
        long double ratio = end1 / end2;

        T returnValue = a * ratio;

        return returnValue;
    };
    PointF3 intersectionPoint(PointF3 lineVector, PointF3 linePoint, PointF3 planeNormal, PointF3 planePoint);
    void multiplyVertexByMatrix(PointF3 &vertDestination, PointF3 &vertSource, PointF3 mat[4]);
    void setMatrixAsIdentity(PointF3 matrix[4]);
    void copyMatrix(PointF3 destination[4], PointF3 source[4]);
    void multiplyMatrix(PointF3 mat1[4], PointF3 mat2[4]);
}
