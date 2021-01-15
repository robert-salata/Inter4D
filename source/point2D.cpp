#include "point2D.h"

Point2D::Point2D()
{
	this->x = 0;
	this->y = 0;
}

Point2D::Point2D(const Point2D& point)
{
	this->x = point.x;
	this->y = point.y;
}

Point2D::Point2D(RealNumber x, RealNumber y)
{
	this->x = x;
	this->y = y;
}

RealNumber Point2D::distance(Point2D point)
{
	return sqrt((this->x - point.x)*(this->x - point.x) + (this->y - point.y)*(this->y - point.y));
}

Point2D Point2D::operator + (Point2D point)
{
	return Point2D(this->x + point.x, this->y + point.y);
}

Point2D Point2D::operator += (Point2D point)
{
	return Point2D(this->x += point.x, this->y += point.y);
}

Point2D Point2D::operator - (Point2D point)
{
	return Point2D(this->x - point.x, this->y - point.y);
}

Point2D Point2D::operator -= (Point2D point)
{
	return Point2D(this->x -= point.x, this->y -= point.y);
}

Point2D Point2D::operator * (RealNumber factor)
{
	return Point2D(this->x * factor, this->y * factor);
}

Point2D Point2D::operator *= (RealNumber factor)
{
	return Point2D(this->x *= factor, this->y *= factor);
}

Point2D Point2D::operator / (RealNumber factor)
{
	return Point2D(this->x / factor, this->y / factor);
}

Point2D Point2D::operator /= (RealNumber factor)
{
	return Point2D(this->x /= factor, this->y /= factor);
}

bool Point2D::operator == (Point2D point)
{
	return this->x == point.x && this->y == point.y;
}