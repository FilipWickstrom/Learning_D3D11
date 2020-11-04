#pragma once
#include <math.h>

class Vector3D
{
private:
	double x;
	double y;
	double z;

public: 
	Vector3D();
	Vector3D(double x, double y, double z);
	Vector3D(const Vector3D& vector);	//Copy constructor
	
	double GetX() const { return this->x; }
	double GetY() const { return this->y; }
	double GetZ() const { return this->z; }

	Vector3D operator+(const Vector3D& vec);
	Vector3D operator-(const Vector3D& vec);
	Vector3D operator*(double val);
	Vector3D operator/(double val);

	//double operator*(const Vector3D& vec);

	double dotProduct(const Vector3D& vec);
	Vector3D crossproduct(const Vector3D& vec);

	double Length() const;
	void Normalize();
};