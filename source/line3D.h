#ifndef Line_3d_class
#define Line_3d_class

#include "vector3D.h"
#include "geometry_base.h"

class Line3D
{
public:
	Line3D(Point3D* origin, Vector3D* direction);
	Line3D(Point3D* point1, Point3D* point2);
	bool operator == (Line3D line);

	bool is_parallel(Line3D* line);
	OperationResult3D intersect(Line3D* line);
	Point3D get_origin();
	Vector3D get_direction();

private:
	Point3D origin;
	Vector3D direction;
};

#endif
