#include "plane3D.h"
#include "algebra_master.h"

Plane3D::Plane3D(const Plane3D& plane)
{
	this->a = plane.a;
	this->b = plane.b;
	this->c = plane.c;
	this->mode = plane.mode;
}

Plane3D::Plane3D(RealNumber A, RealNumber B, RealNumber C, RealNumber D)
{
	if (B == 0 && C == 0)
	{
		mode = Z_ALIGNED;
		a = -D / A;
	}
	else if (B == 0)
	{
		mode = VERTICAL;
		a = -A / C;
		b = -D / C;
	}
	else
	{
		mode = NORMAL;
		a = -A / B;
		b = -C / B;
		c = -D / B;
	}
}

Plane3D::Plane3D(const Point3D* point1, const Point3D* point2, const Point3D* point3)
{
	if (point1->x == point2->x && point2->x == point3->x)
	{
		mode = Z_ALIGNED;
		a = point1->x;
		return;
	}

	{
		RealNumber xs[3] = { point1->x, point2->x, point3->x };
		RealNumber zs[3] = { point1->z, point2->z, point3->z };
		RealNumber ones[3] = { 1, 1, 1 };
		RealNumber* columns[3] = { xs, zs, ones };
		RealNumber vector[3] = { point1->y, point2->y, point3->y };
		RealNumber* result = AlgebraMaster::solveEquationSystem(3, columns, vector);
		if (result != NULL)
		{
			mode = NORMAL;
			a = result[0];
			b = result[1];
			c = result[2];
			delete result;
			return;
		}
	}

	{
		const Point3D* first = point1;
		const Point3D* second = point2->x == point1->x ? point3 : point2;
		RealNumber xs[2] = { first->x, second->x };
		RealNumber ones[2] = { 1, 1 };
		RealNumber* columns[2] = { xs, ones };
		RealNumber vector[2] = { first->z, second->z };
		RealNumber* result = AlgebraMaster::solveEquationSystem(2, columns, vector);
		if (result != NULL)
		{
			mode = VERTICAL;
			a = result[0];
			b = result[1];
			delete result;
			return;
		}
	}
}

bool Plane3D::operator == (Plane3D plane)
{
	return mode == plane.mode && a == plane.a && b == plane.b && c == plane.c;
}

bool Plane3D::operator != (Plane3D plane)
{
	return !(*this == plane);
}

bool Plane3D::is_parallel(const Plane3D* plane)
{
	if (mode == NORMAL && plane->mode == NORMAL)
		return a == plane->a && b == plane->b;
	if (mode == VERTICAL && plane->mode == VERTICAL)
		return b == plane->b;
	if (mode == Z_ALIGNED && plane->mode == Z_ALIGNED)
		return true;
	return false;
}

Point3D Plane3D::calculate_origin()
{
	if (mode == NORMAL)
		return Point3D(0, c, 0);
	if (mode == VERTICAL)
		return Point3D(0, 0, b);
	if (mode == Z_ALIGNED)
		return Point3D(a, 0, 0);
}

Vector3D Plane3D::calculate_normal() const
{
	RealNumber A, B, C, D;
	get_parameters(A, B, C, D);
	return Vector3D(A, B, C);
	/*switch (mode)
	{
	case NORMAL:
		return Vector3D(1, a, 0).product(Vector3D(0, b, 1)).normalized();
	case VERTICAL:
		return Vector3D(1, 0, a).product(Vector3D(0, 1, 0)).normalized();
	case Z_ALIGNED:
		return Vector3D(0, 0, 1).product(Vector3D(0, 1, 0)).normalized();
	}*/
}

Line3D Plane3D::intersect(const Plane3D* plane)
{
	const Plane3D* plane1 = this;
	const Plane3D* plane2 = plane;
	Vector3D normal1 = plane1->calculate_normal();
	Vector3D normal2 = plane2->calculate_normal();
	Vector3D direction = normal1.product(normal2);
	Point3D origin = Point3D(0, 0, 0);
	if (plane1->mode == NORMAL && plane2->mode == NORMAL)
	{
		RealNumber x_column[2] = { plane1->a, plane2->a };
		RealNumber y_column[2] = { -1, -1 };
		RealNumber* columns[2] = { x_column, y_column };
		RealNumber c_column[2] = { -plane1->c, -plane2->c };
		RealNumber* result = AlgebraMaster::solveEquationSystem(2, columns, c_column); //-c = a*x - y + b*0
		if (result != NULL)
		{
			origin = Point3D(result[0], result[1], 0);
			delete result;
		}
		else
		{
			RealNumber y_column[2] = { -1, -1 };
			RealNumber z_column[2] = { plane1->b, plane2->b };
			RealNumber* columns[2] = { y_column, z_column };
			RealNumber c_column[2] = { -plane1->c, -plane2->c };
			RealNumber* result = AlgebraMaster::solveEquationSystem(2, columns, c_column); //-c = a*0 - y + b*z
			origin = Point3D(0, result[0], result[1]);
			delete result;
		}
	}
	if (plane1->mode == VERTICAL && plane2->mode == NORMAL)
		common::swap<const Plane3D*>(plane1, plane2);
	if (plane1->mode == NORMAL && plane2->mode == VERTICAL)
	{
		RealNumber x = 0;
		RealNumber z = plane2->a*x + plane2->b;
		RealNumber y = plane1->a*x + plane1->b*z + plane1->c;
		origin = Point3D(x, y, z);
	}
	if (plane1->mode == VERTICAL && plane2->mode == VERTICAL)
	{
		RealNumber x_column[2] = { plane1->a, plane2->a };
		RealNumber z_column[2] = { -1, -1 };
		RealNumber* columns[2] = { x_column, z_column };
		RealNumber b_column[2] = { -plane1->b, -plane2->b };
		RealNumber* result = AlgebraMaster::solveEquationSystem(2, columns, b_column); //-b = a*x - z
		origin = Point3D(result[0], 0, result[1]);
		delete result;
	}
	if (plane1->mode == Z_ALIGNED)
		common::swap<const Plane3D*>(plane1, plane2);
	if (plane1->mode == NORMAL && plane2->mode == Z_ALIGNED)
	{
		RealNumber x = plane2->a;
		RealNumber z = 0;
		RealNumber y = plane1->a*x + plane1->b*z + plane1->c;
		origin = Point3D(x, y, z);
	}
	if (plane1->mode == VERTICAL && plane2->mode == Z_ALIGNED)
	{
		RealNumber x = plane2->a;
		RealNumber z = plane1->a*x + plane1->b;
		RealNumber y = 0;
		origin = Point3D(x, y, z);
	}
	return Line3D(&origin, &direction);
}

void Plane3D::get_parameters(RealNumber& A, RealNumber& B, RealNumber& C, RealNumber& D) const
{
	switch (mode)
	{
	case NORMAL:
		A = a;
		B = -1;
		C = b;
		D = c;
		return;
	case VERTICAL:
		A = a;
		B = 0;
		C = -1;
		D = b;
		return;
	case Z_ALIGNED:
		A = -1;
		B = 0;
		C = 0;
		D = a;
	}
}

bool Plane3D::contains(const Point3D* point)
{
	switch (mode)
	{
	case NORMAL:
		return point->y == a*point->x + b*point->z + c;
	case VERTICAL:
		return point->z == a*point->x + b;
	case Z_ALIGNED:
		return point->x == a;
	}
}