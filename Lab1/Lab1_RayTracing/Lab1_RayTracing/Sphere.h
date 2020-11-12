#pragma once
#include "Shape.h"

class Sphere : public Shape
{
private:
	Vector3D center;
	double radius;
public:
	Sphere(const Vector3D& colour, const Vector3D& center, double radius);

	// Inherited via Shape
	virtual bool Intersection(const Ray& ray, double& t) override;
};