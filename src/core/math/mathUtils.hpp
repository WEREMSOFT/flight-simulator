#pragma once
#include <array>
#include <vector>
#include <cmath>

#include "../../core/graphics/imageData/imagedata.hpp"

using TriangleI = std::array<PointI, 3>;
using TriangleF = std::array<PointF, 3>;
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
    PointF intersectionPoint(PointF lineVector, PointF linePoint, PointF planeNormal, PointF planePoint);
    void multiplyVertexByMatrix(PointF &vertDestination, PointF &vertSource, PointF mat[4]);
    void setMatrixAsIdentity(PointF matrix[4]);
    void copyMatrix(PointF destination[4], PointF source[4]);
    void multiplyMatrix(PointF mat1[4], PointF mat2[4]);
}
