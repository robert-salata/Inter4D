#include "half_hyperspace4D.h"

HalfHyperspace4D::HalfHyperspace4D(const HalfHyperspace4D& halfspace) : boundary(halfspace.boundary)
{
	this->less = halfspace.less;
	this->closed = halfspace.closed;
}

HalfHyperspace4D::HalfHyperspace4D(Space4D* space, Point4D* point, bool closed) : boundary(*space)
{
	this->closed = closed;
	this->less = true;
	if (!contains(point))
		this->less = false;
}

Space4D* HalfHyperspace4D::get_boundary_space()
{
	return &boundary;
}

bool HalfHyperspace4D::contains(Point4D* point)
{
	RealNumber A, B, C, D, E;
	boundary.get_parameters(A, B, C, D, E);
	RealNumber ineq_left;
	RealNumber ineq_right;
	if (A == 0 && B == 0 && C == 0)
	{
		ineq_left = point->w;
		ineq_right = -E / D;
	}
	else if (B == 0 && C == 0)
	{
		ineq_left = point->x;
		ineq_right = -D / A*point->w - E / A;
	}
	else if (B == 0)
	{
		ineq_left = point->z;
		ineq_right = -A / C*point->x - D / C*point->w - E / C;
	}
	else
	{
		ineq_left = point->y;
		ineq_right = -A / B*point->x - C / B*point->z - D / B*point->w - E / B;
	}

	return less && (closed && ineq_left <= ineq_right || !closed && ineq_left < ineq_right) ||
		!less && (closed && ineq_left >= ineq_right || !closed && ineq_left > ineq_right);
}

Point4D HalfHyperspace4D::representant()
{
	Vector4D normal = boundary.calculate_normal();
	Point4D origin = boundary.calculate_origin();
	return contains(&(origin + normal)) ? origin + normal : origin - normal;
}