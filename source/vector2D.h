#ifndef Vector_2d_class
#define Vector_2d_class

#include "point2D.h"

class Vector2D : public Point2D
{
public:
	Vector2D(RealNumber x, RealNumber y);
	Vector2D(Point2D point);
	Vector2D operator -();

	RealNumber dot_product(Vector2D vector);
	RealNumber length();
	Vector2D normalized();
	bool is_parallel(Vector2D vector);
};

#endif
