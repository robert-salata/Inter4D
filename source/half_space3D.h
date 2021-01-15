#ifndef Half_space_3d_class
#define Half_space_3d_class

#include "plane3D.h"

class HalfSpace3D : public Plane3D
{
public:
	HalfSpace3D(const HalfSpace3D& halfspace);
	HalfSpace3D(Plane3D* plane, Point3D* point, bool closed = true); //halfspace defined by plane, containing point
	bool operator == (const HalfSpace3D& halfspace);
	Plane3D* get_boundary_plane();
	bool contains(Point3D* point);
	Point3D representant();

private:
	bool less; //less: y < ax + bz + c (and other plane-equations similarly)
	bool closed; //closed: <=   !closed: <
};

#endif