#include "Triangle.h"
#include <cmath>

Triangle::Triangle(const Vector3D& colour, const Vector3D& vert1, const Vector3D& vert2, const Vector3D& vert3)
    :Shape(colour)
{
    this->vertices[0] = vert1;
    this->vertices[1] = vert2;
    this->vertices[2] = vert3;
}

bool Triangle::Intersection(const Ray& ray, double& t)
{
    Vector3D origin = ray.origin;
    Vector3D direction = ray.direction;

    /* --- Barycentric coordinates with Cramer's rule ---
       0<=u<=1, 0<=v<=1 and 0<=w<=1 where u+v+w = 1  =>  w = 1-u-v
       
       origin + t*direction = (1-u-v)p0 + (u)p1 + (v)p2  =>  (-d p1-p0 p2-p0)*(t, u, v) = origin - p0  =>
       (t, u, v) = (-d p1-p0 p2-p0)^(-1) * (origin - p0)

       (t, u, v) = ( 1 / (d x v2)*v1 ) * ( (s x v1) * v2 )  =  (1 / a) * ( r * v2 )  =   f * ( r * v2 )
                                         ( (d x v2) * s  )               ( q * s  )          ( q * s  )
                                         ( (s x v1) * d  )               ( r * d  )          ( r * d  )
    */

    //Creates two vectors from the vertices
    Vector3D v1 = this->vertices[1] - this->vertices[0];
    Vector3D v2 = this->vertices[2] - this->vertices[0];
    Vector3D dCrossv2 = direction.CrossProduct(v2);     //q

    //Determinant of matrix m. (d x v2)*v1
    double v1DotdCrossv2 = v1.DotProduct(dCrossv2);     //a

    //Factor is around zero. Ray does not intersect with the triangle as it is perpendicular to dCrossv2
    if (std::abs(v1DotdCrossv2) < 0.0000001f)
        return false;

    //First part of the equation. Saved in variable to avoid multiple calculations
    double devider = 1 / v1DotdCrossv2;   //f

    Vector3D v0ToOrigin = origin - this->vertices[0];  //s

    //The value of u is compared to an edge of the triangle
    double u = (v0ToOrigin.DotProduct(dCrossv2)) * devider;

    //u is negative or larger then 1. Outside of the triangle
    if (u < 0.0  || u > 1.0)
        return false;

    Vector3D r = v0ToOrigin.CrossProduct(v1);
    double v = (direction.DotProduct(r)) * devider;

    //Outside the range it can be within
    if (v < 0.0 || u + v > 1.0)
        return false;

    //Intersect with the triangle. Calculates the t
    t = (v2.DotProduct(r)) * devider;
    return true;
}
