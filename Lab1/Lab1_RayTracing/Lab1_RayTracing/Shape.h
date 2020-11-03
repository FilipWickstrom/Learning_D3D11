#pragma once

#include "Vector3D.h"
#include "Ray.h"

class Shape
{
private:

protected:
	Vector3D colour;

public:
	Shape(const Vector3D& colour);

	const Vector3D& GetColour();

	virtual bool Intersection(const Ray& ray, double& t) = 0;

};