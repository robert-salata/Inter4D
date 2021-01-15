#include "segment3D.h"

Segment3D::Segment3D(Point3D* point1, Point3D* point2) : point1(*point1), point2(*point2)
{

}

void Segment3D::get_ends(Point3D& point1, Point3D& point2)
{
	point1 = this->point1;
	point2 = this->point2;
}