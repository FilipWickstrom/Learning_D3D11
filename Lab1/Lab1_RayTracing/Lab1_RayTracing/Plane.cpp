#include "Plane.h"
#include <cmath> //For abs

Plane::Plane(const Vector3D& colour, const Vector3D& normal, const Vector3D& point)
	:Shape(colour), normal(normal), pointzero(point)
{
	this->normal.Normalize();
}

bool Plane::Intersection(const Ray& ray, double& t)
{
	Vector3D origin = ray.origin;
	Vector3D direction = ray.direction;

	//Check if the ray is perpendicual to the planes normal. Perpendicul if the dotproduct is 0
	double directionDotNormal = direction * normal;
	if (std::abs(directionDotNormal) < 0.0000001f)
	{
		return false;
	}
	
	//Plane intersection test
	//ax + by + cz + d = 0 where a = normal.x, b = normal.y, c = normal.z
	//d = -ax - by - cz
	double d = normal * this->pointzero * -1;

	//Changed version of normal * (origin + t*direction) + d = 0
	t = (normal*-1 * origin - d) / (normal*direction);

	if (t < 0.0f)
	{
		return false;
	}
	else
	{
		return true;
	}
}
