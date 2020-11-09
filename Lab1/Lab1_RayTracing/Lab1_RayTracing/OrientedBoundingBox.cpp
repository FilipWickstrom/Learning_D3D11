#include "OrientedBoundingBox.h"
#include <cmath>

OrientedBoundBox::OrientedBoundBox(const Vector3D& colour, const Vector3D& center, const Vector3D& vec1, const Vector3D& vec2, const Vector3D& vec3, double xlen, double ylen, double zlen)
    :Shape(colour), centerpoint(center)
{
    this->vec[0] = vec1;
    this->vec[1] = vec2;
    this->vec[2] = vec3;
    for (unsigned int i = 0; i < 3; i++)
        this->vec[i].Normalize();
    this->lengths[0] = xlen;
    this->lengths[1] = ylen;
    this->lengths[2] = zlen;
}

void OrientedBoundBox::rotate(double rotX, double rotY, double rotZ)
{
    double xval, yval, zval;
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
    Vector3D orgToCenter = this->centerpoint - origin;    //Creates a vector between the two points

    //Goes through all the planes
    for (unsigned int i = 0; i < 3; i++)
    {
        double first = this->vec[i].DotProduct(orgToCenter);    //Check-value for the orgToCenter and the normvector 
        double second = this->vec[i].DotProduct(direction);     //Check-value for the direction and the normvector

        if (std::abs(second) > 0.0000001f)  //Check if perpendicular with second one
        {
            double t1 = (first + this->lengths[i]) * (1 / second);
            double t2 = (first - this->lengths[i]) * (1 / second);
            if (t1 > t2)    //Swap for right order
            {
                double temp = t1;
                t1 = t2;
                t2 = temp;
            }
            //Check from previous values
            if (t1 > tmin)
                tmin = t1;
            //Also check from previous values
            if (t2 < tmax)
                tmax = t2;
            //Check to see if ray missed entirely or check if behind rays origin
            if (tmin > tmax || tmax < 0)
                return false;
        }
        //Check with first
        else if (-first - this->lengths[i] > 0 || -first + this->lengths[i] < 0)
            return false;
        //Else did not go through the planes
    }
    //Final check to see which t to send
    if (tmin > 0)
        t = tmin;
    else
        t = tmax;
    return true;
}
