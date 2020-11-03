#include "OrientedBoundingBox.h"

OrientedBoundBox::OrientedBoundBox(const Vector3D& colour, const Vector3D& center, const Vector3D& vec1, const Vector3D& vec2, const Vector3D& vec3)
    :Shape(colour), centerpoint(center), vec1(vec1), vec2(vec2), vec3(vec3), vec1norm(vec1), vec2norm(vec2), vec3norm(vec3)
{
    this->vec1norm.Normalize();
    this->vec2norm.Normalize();
    this->vec3norm.Normalize();
}

bool OrientedBoundBox::Intersection(const Ray& ray, double& t)
{
    Vector3D origin = ray.origin;
    Vector3D direction = ray.direction;
    
    //tmin = -INFINITY
    //tmax = INFINITY


    return false;
}
