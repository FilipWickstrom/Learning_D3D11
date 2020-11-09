#pragma once
#include "Shape.h"

class OrientedBoundBox : public Shape
{
private:
	Vector3D centerpoint;
	Vector3D vec[3];
	double lengths[3];

public:
	OrientedBoundBox(const Vector3D& colour, const Vector3D& center, const Vector3D& vec1, const Vector3D& vec2, const Vector3D& vec3, double xlen, double ylen, double zlen);

	void rotate(double rotX, double rotY, double rotZ);

	// Inherited via Shape
	virtual bool Intersection(const Ray& ray, double& t) override;
};