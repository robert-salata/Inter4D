#ifndef Segment_3d_class
#define Segment_3d_class

#include "point3D.h"

class Segment3D
{
public:
	Segment3D(Point3D* point1, Point3D* point2);
	void get_ends(Point3D& point1, Point3D& point2);

private:
	Point3D point1;
	Point3D point2;
};

#endif