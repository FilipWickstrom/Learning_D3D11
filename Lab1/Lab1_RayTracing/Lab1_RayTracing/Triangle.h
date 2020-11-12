#pragma once
#include "Shape.h"

class Triangle : public Shape
{
private:
	Vector3D vertices[3];
public:
	Triangle(const Vector3D& colour, const Vector3D& vert1, const Vector3D& vert2, const Vector3D& vert3);

	// Inherited via Shape
	virtual bool Intersection(const Ray& ray, double& t) override;
};
