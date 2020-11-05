#include "OrientedBoundingBox.h"
#include <cmath>

OrientedBoundBox::OrientedBoundBox(const Vector3D& colour, const Vector3D& center, const Vector3D& vec1, const Vector3D& vec2, const Vector3D& vec3)
    :Shape(colour), centerpoint(center)
{
    this->vec[0] = vec1;
    this->vec[1] = vec2;
    this->vec[2] = vec3;
    this->normvec[0] = vec1;
    this->normvec[1] = vec2;
    this->normvec[2] = vec3;
    this->normvec[0].Normalize();
    this->normvec[1].Normalize();
    this->normvec[2].Normalize();
}

bool OrientedBoundBox::Intersection(const Ray& ray, double& t)
{
    Vector3D origin = ray.origin;
    Vector3D direction = ray.direction;
    
    double tmin = -INFINITY;
    double tmax = INFINITY;
    Vector3D p = this->centerpoint - origin;    //Creates a vector between the two points

    for (unsigned int i = 0; i < 3; i++)
    {
        double e = this->normvec[i].DotProduct(p);              //***förklara
        double f = this->normvec[i].DotProduct(direction);      //***förklara
        double hi = this->vec[i].Length();

        if (std::abs(f) > 0.0000001f)   //Check if perpendicular
        {
            double t1 = (e + hi) * (1 / f); //Is told in the book to be faster
            double t2 = (e - hi) * (1 / f);
            if (t1 > t2)    //Swap
            {
                double temp = t1;
                t1 = t2;
                t2 = temp;
            }
            if (t1 > tmin)
                tmin = t1;
            if (t2 < tmax)
                tmax = t2;
            //Check to see if ray missed entirely
            if (tmin > tmax)
                return false;
            //Behind ray origin
            if (tmax < 0)
                return false;
        }
        else if (-e - hi > 0 || -e + hi < 0)
        {
            return false;
        }
    }
    if (tmin > 0)
        t = tmin;
    else
        t = tmax;
    return true;
}
