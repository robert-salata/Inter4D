#include "point4D.h"

Point4D::Point4D()
{
	this->x = 0;
	this->y = 0;
	this->z = 0;
	this->w = 0;
}

Point4D::Point4D(const Point4D& point)
{
	this->x = point.x;
	this->y = point.y;
	this->z = point.z;
	this->w = point.w;
}

Point4D::Point4D(RealNumber x, RealNumber y, RealNumber z, RealNumber w)
{
	this->x = x;
	this->y = y;
	this->z = z;
	this->w = w;
}

RealNumber Point4D::distance(Point4D point)
{
	return sqrt((this->x - point.x)*(this->x - point.x) + (this->y - point.y)*(this->y - point.y) + 
		(this->z - point.z)*(this->z - point.z) + (this->w - point.w)*(this->w - point.w));
}

Point4D Point4D::operator + (Point4D point)
{
	return Point4D(this->x + point.x, this->y + point.y, this->z + point.z, this->w + point.w);
}

Point4D Point4D::operator += (Point4D point)
{
	return Point4D(this->x += point.x, this->y += point.y, this->z += point.z, this->w += point.w);
}

Point4D Point4D::operator - (Point4D point)
{
	return Point4D(this->x - point.x, this->y - point.y, this->z - point.z, this->w - point.w);
}

Point4D Point4D::operator -= (Point4D point)
{
	return Point4D(this->x -= point.x, this->y -= point.y, this->z -= point.z, this->w -= point.w);
}

Point4D Point4D::operator * (RealNumber factor)
{
	return Point4D(this->x * factor, this->y * factor, this->z * factor, this->w * factor);
}

Point4D Point4D::operator *= (RealNumber factor)
{
	return Point4D(this->x *= factor, this->y *= factor, this->z *= factor, this->w *= factor);
}

Point4D Point4D::operator / (RealNumber factor)
{
	return Point4D(this->x / factor, this->y / factor, this->z / factor, this->w / factor);
}

Point4D Point4D::operator /= (RealNumber factor)
{
	return Point4D(this->x /= factor, this->y /= factor, this->z /= factor, this->w /= factor);
}

bool Point4D::operator == (Point4D point)
{
	return this->x == point.x && this->y == point.y && this->z == point.z && this->w == point.w;
}

Point3D Point4D::xyz() const { return Point3D(x, y, z); }
Point3D Point4D::xyw() const { return Point3D(x, y, w); }
Point3D Point4D::xzw() const { return Point3D(x, z, w); }
Point3D Point4D::yzw() const { return Point3D(y, z, w); }

Point2D Point4D::xy() const { return Point2D(x, y); }
Point2D Point4D::xz() const { return Point2D(x, z); }
Point2D Point4D::xw() const { return Point2D(x, w); }
Point2D Point4D::yz() const { return Point2D(y, z); }
Point2D Point4D::yw() const { return Point2D(y, w); }
Point2D Point4D::zw() const { return Point2D(z, w); }