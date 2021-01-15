#include "segment4D.h"

Segment4D::Segment4D(Point4D* point1, Point4D* point2) : point1(*point1), point2(*point2)
{

}

void Segment4D::get_ends(Point4D& point1, Point4D& point2)
{
	point1 = this->point1;
	point2 = this->point2;
}