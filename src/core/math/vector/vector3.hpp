#pragma once

template <typename T>
class Point3
{
public:
    T x, y, z, w;
    Point3<T> operator-(Point3<T> v)
    {
        return {x - v.x, y - v.y, z - v.z, w - v.w};
    }

    Point3<T> operator+(Point3<T> v)
    {
        return {x + v.x, y + v.y, z + v.z, w + v.w};
    }
    T dot(Point3<T> v)
    {
        return x * v.x + y * v.y + z * v.z;
    }

    Point3<T> cross(Point3<T> v)
    {
        return {y * v.z - z * v.y,
                z * v.x - x * v.z,
                x * v.y - y * v.x, 0};
    }

    Point3<T> normalize()
    {
        return {x / length(), y / length(), z / length()};
    }

    T length()
    {
        return sqrt(pow(x, 2) + pow(y, 2) + pow(z, 2));
    }

    Point3<T> scale(float scalar)
    {
        return {x * scalar, y * scalar, z * scalar, w * scalar};
    }

    float angleWith(Point3<T> v)
    {
        auto dot = x * v.x + y * v.y + z * v.z;
        auto lenSq1 = x * x + y * y + z * z;
        auto lenSq2 = v.x * v.x + v.y * v.y + v.z * v.z;
        return acos(dot / sqrt(lenSq1 * lenSq2));
    }
};
