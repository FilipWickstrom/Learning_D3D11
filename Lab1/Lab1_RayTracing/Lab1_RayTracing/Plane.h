#pragma once
#include "Shape.h"

class Plane : public Shape
{
private:
	Vector3D normal;
	Vector3D pointzero;

public:
	Plane(const Vector3D& colour, const Vector3D& normal, const Vector3D& point);

	// Inherited via Shape
	virtual bool Intersection(const Ray& ray, double& t) override;
};