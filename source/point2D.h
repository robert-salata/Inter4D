#ifndef Point_2d_class
#define Point_2d_class

#include "real_number.h"

class Point2D
{
public:
	Point2D();
	Point2D(const Point2D& point);
	Point2D(RealNumber x, RealNumber y);
	Point2D operator + (Point2D point);
	Point2D operator += (Point2D point);
	Point2D operator - (Point2D point);
	Point2D operator -= (Point2D point);
	Point2D operator * (RealNumber factor);
	Point2D operator *= (RealNumber factor);
	Point2D operator / (RealNumber factor);
	Point2D operator /= (RealNumber factor);
	bool operator == (Point2D point);
	RealNumber distance(Point2D);

	RealNumber x, y;

};

#endif
