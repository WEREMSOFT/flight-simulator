#include "mathUtils.hpp"

using TriangleI = std::array<PointI, 3>;
using TriangleF = std::array<PointF3, 3>;
using TrianglesI = std::vector<TriangleI>;
using TrianglesF = std::vector<TriangleF>;

namespace MathUtils
{

    PointF3 intersectionPoint(PointF3 lineVector, PointF3 linePoint, PointF3 planeNormal, PointF3 planePoint)
    {
        PointF3 diff = linePoint - planePoint;

        return linePoint + lineVector.scale(-diff.dot(planeNormal) / lineVector.dot(planeNormal));
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
}
