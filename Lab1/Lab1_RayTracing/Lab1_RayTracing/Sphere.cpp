#include "Sphere.h"

//Note: förstå vad det är som händer vid beräkning av b och c på rad: 15 och 16

Sphere::Sphere(const Vector3D& colour, const Vector3D& center, double radius)
    :Shape(colour), center(center), radius(radius)
{
}

bool Sphere::Intersection(const Ray& ray, double& t)
{
    Vector3D origin = ray.origin;
    Vector3D direction = ray.direction;

    double b = direction * (origin - this->center);
    double c = (origin - this->center) * (origin - this->center) - (this->radius * this->radius);
    //Check if it's imaginary numbers 
    if (b * b - c < 0)
    {
        return false;
    }
    else
    {
        double t1 = -b + sqrt(b*b - c);
        double t2 = -b - sqrt(b*b - c);
        //See which one is closest to the "camera"

        //Both are behind the "camera"
        if (t1 < 0.0f && t2 < 0.0f)
        {
            return false;
        }
        //t1 is behind the "camera"
        else if (t1 < 0.0f && t2 >= 0.0f)
        {
            t = t2;
        }
        //t2 is behind the "camera"
        else if (t2 < 0.0f && t1 >= 0.0f)
        {
            t = t1;
        }
        //Check which one is closest to the camera
        else
        {
            Vector3D intersectionPoint1 = origin + (direction * t1);
            Vector3D intersectionPoint2 = origin + (direction * t2);
            if (intersectionPoint1.Length() <= intersectionPoint2.Length())
                t = t1;
            else
                t = t2;
        }
        return true;
    }
}
