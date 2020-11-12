#include "Plane.h"
#include <cmath> //std::abs

Plane::Plane(const Vector3D& colour, const Vector3D& normal, const Vector3D& point)
	:Shape(colour), normal(normal), planepoint(point)
{
	this->normal.Normalize();
}

bool Plane::Intersection(const Ray& ray, double& t)
{
	Vector3D origin = ray.origin;
	Vector3D direction = ray.direction;

	//Check if the ray is perpendicual to the planes normal. Perpendicul if the dotproduct close to 0
	double directionDotNormal = direction.DotProduct(this->normal);
	if (std::abs(directionDotNormal) < 0.0000001f)
	{
		return false;
	}
	
	//ax + by + cz + d = 0 where a = normal.x, b = normal.y, c = normal.z
	//d = -ax - by - cz = normal(dot)planepoint * -1
	double d = this->normal.DotProduct(this->planepoint) * (-1);

	//Changed version of normal * (origin + t*direction) + d = 0
	double tCheck = (-d - (this->normal.DotProduct(origin))) / (this->normal.DotProduct(direction));

	//tcheck should be positive to intersect with the plane
	if (tCheck < 0.0f)
	{
		return false;
	}
	else
	{
		t = tCheck;
		return true;
	}
}
