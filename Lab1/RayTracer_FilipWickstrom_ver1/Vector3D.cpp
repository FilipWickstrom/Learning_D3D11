#include "Vector3D.h"

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

void Vector3D::setCoords(double x, double y, double z)
{
	this->x = x; 
	this->y = y; 
	this->z = z;
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

Vector3D& Vector3D::operator=(const Vector3D& vec)
{
	this->x = vec.x;
	this->y = vec.y;
	this->z = vec.z;
	return *this;
}

double Vector3D::DotProduct(const Vector3D& vec)
{
	return this->x * vec.x + this->y * vec.y + this->z * vec.z;
}

Vector3D Vector3D::CrossProduct(const Vector3D& vec)
{
	return Vector3D(this->y * vec.z - this->z * vec.y, this->z*vec.x - this->x*vec.z, this->x*vec.y - this->y*vec.x);
}

double Vector3D::Length() const
{
	return sqrt(this->x * this->x + this->y * this->y + this->z * this->z);
}

void Vector3D::Normalize()
{
	double length = this->Length();
	if (length != 0)
	{
		this->x = this->x / length;
		this->y = this->y / length;
		this->z = this->z / length;
	}
}
