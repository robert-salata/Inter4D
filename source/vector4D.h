#ifndef Vector_4d_class
#define Vector_4d_class

#include "point4D.h"

class Vector4D : public Point4D
{
public:
	Vector4D(RealNumber x, RealNumber y, RealNumber z, RealNumber w);
	Vector4D(Point4D point);
	Vector4D operator -();

	RealNumber dot_product(Vector4D vector);
	RealNumber length();
	Vector4D normalized();
	bool is_parallel(Vector4D vector);
};

#endif
