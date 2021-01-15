#ifndef Vector_3d_class
#define Vector_3d_class

#include "point3D.h"

class Vector3D : public Point3D
{
public:
	Vector3D(RealNumber x, RealNumber y, RealNumber z);
	Vector3D(Point3D point);
	Vector3D operator -();

	Vector3D product(Vector3D vector);
	RealNumber dot_product(Vector3D vector);
	RealNumber length();
	Vector3D normalized();
	bool is_parallel(Vector3D vector);
};

#endif
