#ifndef Halfplane_3d_class
#define Halfplane_3d_class

#include "half_space3D.h"
#include "plane3D.h"
#include "line3D.h"

class HalfPlane3D
{
public:
	HalfPlane3D(HalfSpace3D* halfspace, Plane3D* plane);
	~HalfPlane3D();
	Line3D calculate_edge();

private:
	HalfSpace3D* halfspace;
	Plane3D* plane;
};

#endif
