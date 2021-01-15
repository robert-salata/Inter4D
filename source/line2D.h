#ifndef Line_2d_class
#define Line_2d_class

#include "point2D.h"
#include "vector2D.h"
#include "geometry_base.h"

class Line2D
{
public:
	Line2D(const Line2D& line);
	Line2D(RealNumber A, RealNumber B, RealNumber C); 	//Ax + By + C = 0
	Line2D(Point2D* point1, Point2D* point2); //line defined by 2 points
	bool operator == (Line2D line);
	bool operator != (Line2D line);
	void get_parameters(RealNumber& A, RealNumber& B, RealNumber& C); //Ax + By + C = 0
	bool is_parallel(Line2D* plane);
	Point2D calculate_origin();
	Vector2D calculate_normal();
	OperationResult2D intersect(Line2D* line);
	bool contains(Point2D* point);

protected:
	RealNumber a;
	RealNumber b;

	//NORMAL:		y = ax + b
	//VERTICAL:		x = a
	enum Mode { NORMAL, VERTICAL };
	Mode mode;
};

#endif