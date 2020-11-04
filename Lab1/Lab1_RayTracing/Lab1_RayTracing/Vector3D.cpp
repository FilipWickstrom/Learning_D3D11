#include "Vector3D.h"
#include <iostream> //*****

Vector3D::Vector3D()
	:x(0), y(0), z(0)
{
}

Vector3D::Vector3D(double x, double y, double z)
	:x(x), y(y), z(z)
{
}

Vector3D::Vector3D(const Vector3D& vector)
	:x(vector.x), y(vector.y), z(vector.z)
{
}

Vector3D Vector3D::operator+(const Vector3D& vec)
{
	return Vector3D(this->x + vec.x, this->y+vec.y, this->z+vec.z);
}

Vector3D Vector3D::operator-(const Vector3D& vec)
{
	return Vector3D(this->x - vec.x, this->y - vec.y, this->z - vec.z);
}

Vector3D Vector3D::operator*(double val)
{
	return Vector3D(this->x*val, this->y*val, this->z*val);
}

Vector3D Vector3D::operator/(double val)
{
	return Vector3D(this->x/val, this->y/val, this->z/val);
}

double Vector3D::dotProduct(const Vector3D& vec)
{
	return this->x * vec.x + this->y * vec.y + this->z * vec.z;
}

Vector3D Vector3D::crossproduct(const Vector3D& vec)
{
	return Vector3D(this->y * vec.z - this->z * vec.y, this->z*vec.x - this->x*vec.z, this->x*vec.y - this->y*vec.x);
}

double Vector3D::Length() const
{
	return sqrt(this->x * this->x + this->y * this->y + this->z * this->z);
}

void Vector3D::Normalize()
{
	//std::cout << "Before: " << this->x << ", " << this->y << ", " << this->z << std::endl;
	if (this->x != 0 && this->y != 0 && this->z != 0)	//(0,0,0) is not allowed
	{
		double length = this->Length();
		this->x = this->x / length;
		this->y = this->y / length;
		this->z = this->z / length;
	}
	//std::cout << "After: " << this->x << ", " << this->y << ", " << this->z << std::endl;
}
