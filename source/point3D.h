#ifndef Point_3d_class
#define Point_3d_class

#include "real_number.h"

class Point3D
{
public:
	Point3D();
	Point3D(const Point3D& point);
	Point3D(RealNumber x, RealNumber y, RealNumber z);
	Point3D operator + (Point3D point) const;
	Point3D operator += (Point3D point);
	Point3D operator - (Point3D point) const;
	Point3D operator -= (Point3D point);
	Point3D operator * (RealNumber factor) const;
	Point3D operator *= (RealNumber factor);
	Point3D operator / (RealNumber factor) const;
	Point3D operator /= (RealNumber factor);
	bool operator == (Point3D point) const;
	RealNumber distance(Point3D) const;

	RealNumber x, y, z;
	
};

#endif