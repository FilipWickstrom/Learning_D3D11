#include "OrientedBoundingBox.h"
#include <cmath>    //std::abs

OrientedBoundBox::OrientedBoundBox(const Vector3D& colour, const Vector3D& center, const Vector3D& v1, const Vector3D& v2, const Vector3D& v3, double xlen, double ylen, double zlen)
    :Shape(colour), centerpoint(center)
{
    this->vec[0] = v1;
    this->vec[1] = v2;
    this->vec[2] = v3;
    for (unsigned int i = 0; i < 3; i++)
        this->vec[i].Normalize();
    this->lengths[0] = xlen;
    this->lengths[1] = ylen;
    this->lengths[2] = zlen;
}

void OrientedBoundBox::rotate(double rotX, double rotY, double rotZ)
{
    double xval, yval, zval;
    //Checking if zero to avoid rotation
    if (rotX != 0)
    {
        Vector3D xrotmatrix[3] = { {1,0,0}, {0,cos(rotX),-sin(rotX)}, {0,sin(rotX),cos(rotX)} };
        for (unsigned int i = 0; i < 3; i++)
        {
            xval = this->vec[i].DotProduct(xrotmatrix[0]);
            yval = this->vec[i].DotProduct(xrotmatrix[1]);
            zval = this->vec[i].DotProduct(xrotmatrix[2]);
            this->vec[i].setCoords(xval, yval, zval);
        }
    }
    if (rotY != 0)
    {
        Vector3D yrotmatrix[3] = { {cos(rotY),0,sin(rotY)}, {0,1,0}, {-sin(rotY),0,cos(rotY)} };
        for (unsigned int i = 0; i < 3; i++)
        {
            xval = this->vec[i].DotProduct(yrotmatrix[0]);
            yval = this->vec[i].DotProduct(yrotmatrix[1]);
            zval = this->vec[i].DotProduct(yrotmatrix[2]);
            this->vec[i].setCoords(xval, yval, zval);
        }
    }
    if (rotZ != 0)
    {
        Vector3D zrotmatrix[3] = { {cos(rotZ),-sin(rotZ),0}, {sin(rotZ),cos(rotZ),0}, {0,0,1} };
        for (unsigned int i = 0; i < 3; i++)
        {
            xval = this->vec[i].DotProduct(zrotmatrix[0]);
            yval = this->vec[i].DotProduct(zrotmatrix[1]);
            zval = this->vec[i].DotProduct(zrotmatrix[2]);
            this->vec[i].setCoords(xval, yval, zval);
        }
    }
}

bool OrientedBoundBox::Intersection(const Ray& ray, double& t)
{
    Vector3D origin = ray.origin;
    Vector3D direction = ray.direction;
    
    double tmin = -INFINITY;
    double tmax = INFINITY;
    Vector3D originToCenter = this->centerpoint - origin;    //Creates a vector between the two points

    //Goes through all the planes
    for (unsigned int i = 0; i < 3; i++)
    {
        double vecDotOriginToCenter = this->vec[i].DotProduct(originToCenter);  //The length of originToCenter in direction of the normvector 
        double vecDotDirection = this->vec[i].DotProduct(direction);            //The length of the rays direction in the normvectors direction

        //Check that the plane and the rays direction is not perpendicular. Actually 2 planes but they have the same normal
        if (std::abs(vecDotDirection) > 0.0000001f) 
        {
            /*Calculates t-values for the two planes. (+-) length is so that they are not in the center of the box
              Plane EQ: t = (-dp - np*o)/np*d  =>  t1 = (length[i] + vecToOriginToCenter) / vecDotDirection*/
            double t1 = (vecDotOriginToCenter + this->lengths[i]) * (1 / vecDotDirection);
            double t2 = (vecDotOriginToCenter - this->lengths[i]) * (1 / vecDotDirection);  
            //Swap for right order
            if (t1 > t2)
            {
                double temp = t1;
                t1 = t2;
                t2 = temp;
            }
            //Checking previous values
            if (t1 > tmin)
                tmin = t1;
            if (t2 < tmax)
                tmax = t2;
            //The ray missed the box or box is behind the ray origin
            if (tmin > tmax || tmax < 0)
                return false;
        }
        //Check with vecDotOriginToCenter to see if we are outside the box
        else if (-vecDotOriginToCenter - this->lengths[i] > 0 || -vecDotOriginToCenter + this->lengths[i] < 0)
            return false;
        //Else, went through only one of the two planes. Keep looping through the other planes
    }
    //Final check to see which t to send
    if (tmin > 0)
        t = tmin;
    else
        t = tmax;
    return true;
}
