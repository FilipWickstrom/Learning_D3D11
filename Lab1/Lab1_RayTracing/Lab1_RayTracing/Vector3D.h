#pragma once
#include <math.h>

class Vector3D
{
private:
	double x;
	double y;
	double z;

public: 
	Vector3D(double x, double y, double z);
	Vector3D(const Vector3D& vector);

	double GetX() const { return this->x; }
	double GetY() const { return this->y; }
	double GetZ() const { return this->z; }

	Vector3D operator+(const Vector3D& vec2);
	Vector3D operator-(const Vector3D& vec2);
	double operator*(const Vector3D& vec2);	 //Dotproduct
	Vector3D operator*(const double& val);
	Vector3D crossproduct(const Vector3D& vec2);

	double Length() const;
	void Normalize();
};