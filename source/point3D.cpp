#include "point3D.h"

Point3D::Point3D()
{
	this->x = 0;
	this->y = 0;
	this->z = 0;
}

Point3D::Point3D(const Point3D& point)
{
	this->x = point.x;
	this->y = point.y;
	this->z = point.z;
}

Point3D::Point3D(RealNumber x, RealNumber y, RealNumber z)
{
	this->x = x;
	this->y = y;
	this->z = z;
}

RealNumber Point3D::distance(Point3D point) const
{
	return sqrt((this->x - point.x)*(this->x - point.x) + (this->y - point.y)*(this->y - point.y) + 
		(this->z - point.z)*(this->z - point.z));
}

Point3D Point3D::operator + (Point3D point) const
{
	return Point3D(this->x + point.x, this->y + point.y, this->z + point.z);
}

Point3D Point3D::operator += (Point3D point)
{
	return Point3D(this->x += point.x, this->y += point.y, this->z += point.z);
}

Point3D Point3D::operator - (Point3D point) const
{
	return Point3D(this->x - point.x, this->y - point.y, this->z - point.z);
}

Point3D Point3D::operator -= (Point3D point)
{
	return Point3D(this->x -= point.x, this->y -= point.y, this->z -= point.z);
}

Point3D Point3D::operator * (RealNumber factor) const
{
	return Point3D(this->x * factor, this->y * factor, this->z * factor);
}

Point3D Point3D::operator *= (RealNumber factor)
{
	return Point3D(this->x *= factor, this->y *= factor, this->z *= factor);
}

Point3D Point3D::operator / (RealNumber factor) const
{
	return Point3D(this->x / factor, this->y / factor, this->z / factor);
}

Point3D Point3D::operator /= (RealNumber factor)
{
	return Point3D(this->x /= factor, this->y /= factor, this->z /= factor);
}

bool Point3D::operator == (Point3D point) const
{
	return this->x == point.x && this->y == point.y && this->z == point.z;
}