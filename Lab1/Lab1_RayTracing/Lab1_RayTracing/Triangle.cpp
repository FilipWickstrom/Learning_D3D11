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

    //Creates two vectors from the vertices
    Vector3D v1 = this->vertices[1] - this->vertices[0];
    Vector3D v2 = this->vertices[2] - this->vertices[0];
    Vector3D d_X_v2 = direction.CrossProduct(v2);

    //(t, u, v) = (r*v2, q*s, r*d)*f 

    //Delare till första delen****
    double a = v1.DotProduct(d_X_v2);

    //Factor is around zero. Ray does not intersect with the triangle 
    if (a > -0.0000001f && a < 0.0000001f)
        return false;

    //First part of the equation. Saved in variable to avoid multiple calculations
    double f = 1 / a;
    
    Vector3D s = origin - this->vertices[0];
    //
    double u = (s.DotProduct(d_X_v2)) * f;

    //u is negative. Outside of the triangle
    if (u < 0.0)
        return false;

    Vector3D r = s.CrossProduct(v1);
    double v = (direction.DotProduct(r)) * f;

    //Outside of the triangle
    if (v < 0.0 || u + v > 1.0)
        return false;

    //Intersect with the triangle. Calculates the t
    t = (v2.DotProduct(r)) * f;
    return true;
}
