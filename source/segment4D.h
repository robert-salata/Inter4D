#ifndef Segment_4d_class
#define Segment_4d_class

#include "point4D.h"

class Segment4D
{
public:
	Segment4D(Point4D* point1, Point4D* point2);
	void get_ends(Point4D& point1, Point4D& point2);

private:
	Point4D point1;
	Point4D point2;
};

#endif