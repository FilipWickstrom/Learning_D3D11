#pragma once
#include "Vector3D.h"

struct Ray
{
	Vector3D origin;
	Vector3D direction;

	Ray(const Vector3D& origin, const Vector3D& direction);
};