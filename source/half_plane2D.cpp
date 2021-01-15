#include "half_plane2D.h"

HalfPlane2D::HalfPlane2D(const HalfPlane2D& halfplane)
{
	this->edge = new Line2D(*halfplane.edge);
	this->less = halfplane.less;
	this->closed = halfplane.closed;
}

HalfPlane2D::HalfPlane2D(Line2D* line, Point2D* point, bool closed)
{
	this->edge = new Line2D(*line);
	this->closed = closed;
	this->less = true;
	if (!contains(point))
		this->less = false;
}

HalfPlane2D::~HalfPlane2D()
{
	delete edge;
}

bool HalfPlane2D::operator == (HalfPlane2D halfplane) const
{
	return *edge == *halfplane.edge && closed == halfplane.closed && less == halfplane.less;
}

Line2D HalfPlane2D::get_boundary_line() const
{
	return *this->edge;
}

bool HalfPlane2D::contains(Point2D* point) const
{
	RealNumber A, B, C;
	edge->get_parameters(A, B, C);
	RealNumber ineq_left;
	RealNumber ineq_right;
	if (B == 0)
	{
		ineq_left = point->x;
		ineq_right = -C / A;
	}
	else
	{
		ineq_left = point->y;
		ineq_right = -A/B*point->x - C/B;
	}

	return less && (closed && ineq_left <= ineq_right || !closed && ineq_left < ineq_right) ||
		  !less && (closed && ineq_left >= ineq_right || !closed && ineq_left > ineq_right);
}

Point2D HalfPlane2D::representant() const
{
	Vector2D normal = edge->calculate_normal();
	Point2D origin = edge->calculate_origin();
	return contains(&(origin + normal)) ? origin + normal : origin - normal;
}