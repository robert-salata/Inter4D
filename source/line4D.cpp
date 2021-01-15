#include "line4D.h"

Line4D::Line4D(Point4D* origin, Vector4D* direction) : origin(*origin), direction(*direction) { }

Line4D::Line4D(Point4D* point1, Point4D* point2) : origin(*point1), direction(*point2 - *point1) { }

bool Line4D::operator == (Line4D line)
{
	return is_parallel(&line) && this->direction.is_parallel(line.origin - this->origin);
}

bool Line4D::is_parallel(Line4D* line)
{
	return this->direction.is_parallel(line->direction);
}

Point4D Line4D::get_origin()
{
	return this->origin;
}

Vector4D Line4D::get_direction()
{
	return this->direction;
}