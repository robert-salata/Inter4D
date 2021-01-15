#ifndef Plane_3d_class
#define Plane_3d_class

#include "point3D.h"
#include "vector3D.h"
#include "line3D.h"

class Plane3D
{
public:
	Plane3D(const Plane3D& plane);
	Plane3D(RealNumber A, RealNumber B, RealNumber C, RealNumber D); 	//Ax + By + Cz + D = 0
	Plane3D(const Point3D* point1, const Point3D* point2, const Point3D* point3); //plane defined by 3 points
	bool operator == (Plane3D line);
	bool operator != (Plane3D line);
	void get_parameters(RealNumber& A, RealNumber& B, RealNumber& C, RealNumber& D) const; //Ax + By + Cz + D = 0
	bool is_parallel(const Plane3D* plane);
	Point3D calculate_origin();
	Vector3D calculate_normal() const;
	Line3D intersect(const Plane3D* plane);
	bool contains(const Point3D* point);

protected:
	RealNumber a;
	RealNumber b;
	RealNumber c;

	//NORMAL:		y = ax + bz + c
	//VERTICAL:		z = ax + b
	//Z_ALIGNED:	x = a
	enum Mode { NORMAL, VERTICAL, Z_ALIGNED };
	Mode mode;
};

#endif