#include "Sphere.h"

Sphere::Sphere(const Vector3D& colour, const Vector3D& center, double radius)
    :Shape(colour), center(center), radius(radius)
{
}

bool Sphere::Intersection(const Ray& ray, double& t)
{
    Vector3D origin = ray.origin;
    Vector3D direction = ray.direction;

    /*Comes from: |point - center| = radius  =>  |(origin + t*direction) - center| = radius  =>  ...
    calculation of t leads to  =>  t = -b (+-) sqrt(b^2 - c) */
    double b = direction.DotProduct(origin - this->center);     //The length between center to origin in direction of "direction"
    double c = (origin - this->center).DotProduct((origin - this->center)) - (this->radius * this->radius); //The length from center to origin - radius^2
    
    //Check to avoid imaginary numbers in the quadratic equation
    if (b * b - c < 0)
    {
        return false;
    }
    else
    {
        double t1 = -b + sqrt(b*b - c);
        double t2 = -b - sqrt(b*b - c);
 
        //Both are behind the origin
        if (t1 < 0.0f && t2 < 0.0f)
        {
            return false;
        }
        //Check which one is closest to the origin, lower is closer
        else
        {
            if (t1 <= t2)
                t = t1;
            else
                t = t2;
            return true;
        }
    }
}
