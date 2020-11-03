#include "Vector3D.h"

Vector3D::Vector3D(double x, double y, double z)
	:x(x), y(y), z(z)
{
}

Vector3D::Vector3D(const Vector3D& vector)
{
	this->x = vector.x;
	this->y = vector.y;
	this->z = vector.z;
}

Vector3D Vector3D::operator+(const Vector3D& vec2)
{
	return Vector3D(this->x + vec2.x, this->y+vec2.y, this->z+vec2.z);
}

Vector3D Vector3D::operator-(const Vector3D& vec2)
{
	return Vector3D(this->x - vec2.x, this->y - vec2.y, this->z - vec2.z);
}

double Vector3D::operator*(const Vector3D& vec2)
{
	return this->x * vec2.x + this->y * vec2.y + this->z * vec2.z;
}

Vector3D Vector3D::operator*(const double& val)
{
	return Vector3D(this->x*val, this->y*val, this->z*val);
}

Vector3D Vector3D::crossproduct(const Vector3D& vec2)
{
	return Vector3D(this->y * vec2.z - this->z * vec2.y, this->z*vec2.x - this->x*vec2.z, this->x*vec2.y - this->y*vec2.x);
}

double Vector3D::Length()
{
	return sqrt(this->x * this->x + this->y * this->y + this->z * this->z);
}

void Vector3D::Normalize()
{
	if (this->x != 0 && this->y != 0 && this->z != 0)	//(0,0,0) is not allowed
	{
		double normalvalue = 1 / sqrt(this->x * this->x + this->y * this->y + this->z * this->z);
		this->x = this->x * normalvalue;
		this->y = this->y * normalvalue;
		this->z = this->z * normalvalue;
	}
}
