#ifndef Space_4d_class
#define Space_4d_class

#include "point4D.h"
#include "vector4D.h"

class Space4D
{
public:
	Space4D(const Space4D& space);
	Space4D(RealNumber A, RealNumber B, RealNumber C, RealNumber D, RealNumber E); 	//Ax + By + Cz + Dw + E = 0
	Space4D(const Point4D* point1, const Point4D* point2, const Point4D* point3, const Point4D* point4); //space defined by 4 points
	void get_parameters(RealNumber& A, RealNumber& B, RealNumber& C, RealNumber& D, RealNumber& E) const; //Ax + By + Cz + D = 0
	bool is_parallel(const Space4D* space);
	Point4D calculate_origin();
	Vector4D calculate_normal() const;
	bool contains(const Point4D* point);

private:
	RealNumber a;
	RealNumber b;
	RealNumber c;
	RealNumber d;

	//NORMAL:		y = ax + bz + cw + d
	//Y0:			z = ax + bw + c
	//Y0Z0:			x = aw + b
	//Y0Z0X0:		w = a
	enum Mode { NORMAL, Y0, Y0Z0, Y0Z0X0 };
	Mode mode;
};

#endif