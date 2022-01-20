#include "mathUtils.hpp"

using TriangleI = std::array<PointI, 3>;
using TriangleF = std::array<PointF3, 3>;
using TrianglesI = std::vector<TriangleI>;
using TrianglesF = std::vector<TriangleF>;

namespace MathUtils
{
    PointF3 scaleVector(PointF3 v, float scalar)
    {
        return {v.x * scalar, v.y * scalar, v.z * scalar, v.w * scalar};
    }
    PointF3 addVertex(PointF3 u, PointF3 v)
    {
        return {u.x + v.x, u.y + v.y, u.z + v.z, u.w + v.w};
    }

    PointF3 substractVertex(PointF3 u, PointF3 v)
    {
        return {u.x - v.x, u.y - v.y, u.z - v.z, u.w - v.w};
    }

    PointF3 crossProduct(PointF3 u, PointF3 v)
    {
        return {u.y * v.z - u.z * v.y,
                u.z * v.x - u.x * v.z,
                u.x * v.y - u.y * v.x, 0};
    }

    double dotProduct(PointF3 a, PointF3 b)
    {
        return a.x * b.x + a.y * b.y + a.z * b.z;
    }

    float length(PointF3 v)
    {
        return sqrt(pow(v.x, 2) + pow(v.y, 2) + pow(v.z, 2));
    }

    PointF3 normalize(PointF3 v)
    {
        auto length = MathUtils::length(v);
        return {v.x / length, v.y / length, v.z / length};
    }

    PointF3 intersectionPoint(PointF3 lineVector, PointF3 linePoint, PointF3 planeNormal, PointF3 planePoint)
    {
        PointF3 diff = substractVertex(linePoint, planePoint);

        return addVertex(addVertex(diff, planePoint), scaleVector(lineVector, -dotProduct(diff, planeNormal) / dotProduct(lineVector, planeNormal)));
    }

    void multiplyVertexByMatrix(PointF3 &vertDestination, PointF3 &vertSource, PointF3 mat[4])
    {
        vertDestination.x = vertSource.x * mat[0].x +
                            vertSource.y * mat[1].x +
                            vertSource.z * mat[2].x +
                            vertSource.w * mat[3].x;

        vertDestination.y = vertSource.x * mat[0].y +
                            vertSource.y * mat[1].y +
                            vertSource.z * mat[2].y +
                            vertSource.w * mat[3].y;

        vertDestination.z = vertSource.x * mat[0].z +
                            vertSource.y * mat[1].z +
                            vertSource.z * mat[2].z +
                            vertSource.w * mat[3].z;

        vertDestination.w = vertSource.x * mat[0].w +
                            vertSource.y * mat[1].w +
                            vertSource.z * mat[2].w +
                            vertSource.w * mat[3].w;
    }

    void setMatrixAsIdentity(PointF3 matrix[4])
    {
        for (int i = 0; i < 4; i++)
        {
            matrix[i] = {0};
        }
        matrix[0].x = 1.0;
        matrix[1].y = 1.0;
        matrix[2].z = 1.0;
        matrix[3].w = 1.0;
    }

    void copyMatrix(PointF3 destination[4], PointF3 source[4])
    {
        for (int i = 0; i < 4; i++)
        {
            destination[i] = source[i];
        }
    }

    void multiplyMatrix(PointF3 mat1[4], PointF3 mat2[4])
    {
        PointF3 returnValue[4] = {0};
        setMatrixAsIdentity(returnValue);

        for (int i = 0; i < 4; i++)
        {
            multiplyVertexByMatrix(returnValue[i], mat1[i], mat2);
        }

        copyMatrix(mat1, returnValue);
    }

    float angleBetweenVectors(PointF3 a, PointF3 b)
    {
        auto dot = a.x * b.x + a.y * b.y + a.z * b.z;
        auto lenSq1 = a.x * a.x + a.y * a.y + a.z * a.z;
        auto lenSq2 = b.x * b.x + b.y * b.y + b.z * b.z;
        return acos(dot / sqrt(lenSq1 * lenSq2));
    }
}

bool isBackFace(PointF3 normal)
{
    auto angle = MathUtils::dotProduct(normal, {0, 0, 1});
    return angle > 0;
}

bool sortTriangleZ(PointF3 a, PointF3 b)
{
    return a.z > b.z;
}
