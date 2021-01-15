#ifndef Point_4d_class
#define Point_4d_class

#include "real_number.h"
#include "point3D.h"
#include "point2D.h"

class Point4D
{
public:
	Point4D();
	Point4D(const Point4D& point);
	Point4D(RealNumber x, RealNumber y, RealNumber z, RealNumber w);
	Point4D operator + (Point4D point);
	Point4D operator += (Point4D point);
	Point4D operator - (Point4D point);
	Point4D operator -= (Point4D point);
	Point4D operator * (RealNumber factor);
	Point4D operator *= (RealNumber factor);
	Point4D operator / (RealNumber factor);
	Point4D operator /= (RealNumber factor);
	bool operator == (Point4D point);
	
	RealNumber distance(Point4D);
	
	Point3D xyz() const;
	Point3D xyw() const;
	Point3D xzw() const;
	Point3D yzw() const;

	Point2D xy() const;
	Point2D xz() const;
	Point2D xw() const;
	Point2D yz() const;
	Point2D yw() const;
	Point2D zw() const;

	RealNumber x, y, z, w;

};

#endif