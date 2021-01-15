#include "line3D.h"

Line3D::Line3D(Point3D* origin, Vector3D* direction) : origin(*origin), direction(*direction) { }

Line3D::Line3D(Point3D* point1, Point3D* point2) : origin(*point1), direction(*point2 - *point1) { }

bool Line3D::operator == (Line3D line)
{
	return is_parallel(&line) && this->direction.is_parallel(line.origin - this->origin);
}

bool Line3D::is_parallel(Line3D* line)
{
	return this->direction.is_parallel(line->direction);
}

Point3D Line3D::get_origin()
{
	return this->origin;
}

Vector3D Line3D::get_direction()
{
	return this->direction;
}