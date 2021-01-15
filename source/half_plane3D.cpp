#include "half_plane3D.h"
#include "math_engine.h"

HalfPlane3D::HalfPlane3D(HalfSpace3D* halfspace, Plane3D* plane)
{
	this->halfspace = new HalfSpace3D(*halfspace);
	this->plane = new Plane3D(*plane);
}

HalfPlane3D::~HalfPlane3D()
{
	delete halfspace;
	delete plane;
}

Line3D HalfPlane3D::calculate_edge()
{
	return plane->intersect(halfspace->get_boundary_plane());
}
