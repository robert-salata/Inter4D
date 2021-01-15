#include "half_space3D.h"

HalfSpace3D::HalfSpace3D(const HalfSpace3D& halfspace) : Plane3D(halfspace)
{
	this->less = halfspace.less;
	this->closed = halfspace.closed;
}

HalfSpace3D::HalfSpace3D(Plane3D* plane, Point3D* point, bool closed) : Plane3D(*plane)
{
	this->closed = closed;
	this->less = true;
	if (!contains(point))
		this->less = false;
}

bool HalfSpace3D::operator == (const HalfSpace3D& halfspace)
{
	return *(Plane3D*)this == (Plane3D)halfspace && closed == halfspace.closed && less == halfspace.less;
}

Plane3D* HalfSpace3D::get_boundary_plane()
{
	return (Plane3D*)this;
}

bool HalfSpace3D::contains(Point3D* point)
{
	RealNumber ineq_left;
	RealNumber ineq_right;
	switch (mode)
	{
	case NORMAL:
		ineq_left = point->y;
		ineq_right = a*point->x + b*point->z + c;
		break;
	case VERTICAL:
		ineq_left = point->z;
		ineq_right = a*point->x + b;
		break;
	case Z_ALIGNED:
		ineq_left = point->x;
		ineq_right = a;
	}
	return less && (closed && ineq_left <= ineq_right || !closed && ineq_left < ineq_right) ||
		!less && (closed && ineq_left >= ineq_right || !closed && ineq_left > ineq_right);
}

Point3D HalfSpace3D::representant()
{
	Vector3D normal = this->calculate_normal();
	Point3D origin = this->calculate_origin();
	return contains(&(origin + normal)) ? origin + normal : origin - normal;
}