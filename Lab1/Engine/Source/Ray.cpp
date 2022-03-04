#include "Ray.h"

Ray::Ray(const Vector3D& origin, const Vector3D& direction)
	:origin(origin), direction(direction)
{
	this->direction.Normalize();
}
