#ifndef Line_4d_class
#define Line_4d_class

#include "vector4D.h"
#include "geometry_base.h"

class Line4D
{
public:
	Line4D(Point4D* origin, Vector4D* direction);
	Line4D(Point4D* point1, Point4D* point2);

	bool operator == (Line4D line);

	bool is_parallel(Line4D* line);
	Point4D get_origin();
	Vector4D get_direction();

private:
	Point4D origin;
	Vector4D direction;
};

#endif
