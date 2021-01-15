#include "space4D.h"
#include "algebra_master.h"
#include "vector3D.h"

Space4D::Space4D(const Space4D& space)
{
	this->a = space.a;
	this->b = space.b;
	this->c = space.c;
	this->d = space.d;
	this->mode = space.mode;
}

Space4D::Space4D(RealNumber A, RealNumber B, RealNumber C, RealNumber D, RealNumber E)
{
	if (A == 0 && B == 0 && C == 0)
	{
		mode = Y0Z0X0;
		a = -E / D;
	}
	else if (B == 0 && C == 0)
	{
		mode = Y0Z0;
		a = -D / A;
		b = -E / A;
	}
	else if (B == 0)
	{
		mode = Y0;
		a = -A / C;
		b = -D / C;
		c = -E / C;
	}
	else
	{
		mode = NORMAL;
		a = -A / B;
		b = -C / B;
		c = -D / B;
		d = -E / B;
	}
}

Space4D::Space4D(const Point4D* point1, const Point4D* point2, const Point4D* point3, const Point4D* point4)
{
	{
		RealNumber xs[4] = { point1->x, point2->x, point3->x, point4->x };
		RealNumber zs[4] = { point1->z, point2->z, point3->z, point4->z };
		RealNumber ws[4] = { point1->w, point2->w, point3->w, point4->w };
		RealNumber ones[4] = { 1, 1, 1, 1 };
		RealNumber* columns[4] = { xs, zs, ws, ones };
		RealNumber vector[4] = { point1->y, point2->y, point3->y, point4->y };
		RealNumber* result = AlgebraMaster::solveEquationSystem(4, columns, vector);
		if (result != NULL)
		{
			mode = NORMAL;
			a = result[0];
			b = result[1];
			c = result[2];
			d = result[3];
			delete result;
			return;
		}
	}

	{
		const Point4D* first = point1, *second, *third;
		if (point2->x == point1->x && point2->w == point1->w)
		{
			second = point3;
			third = point4;
		}
		else
		{
			second = point2;
			if (point3->x == point1->x && point3->w == point1->w || point3->x == point2->x && point3->w == point2->w)
				third = point4;
			else
			{
				if (Vector3D(point3->xzw() - point2->xzw()).is_parallel(point3->xzw() - point1->xzw()))
					third = point4;
				else
					third = point3;
			}
			//third = (point3->x == point1->x && point3->w == point1->w || point3->x == point2->x && point3->w == point2->w) ? point4 : point3;
		}
		RealNumber xs[3] = { first->x, second->x, third->x };
		RealNumber ws[3] = { first->w, second->w, third->w };
		RealNumber ones[3] = { 1, 1, 1 };
		RealNumber* columns[3] = { xs, ws, ones };
		RealNumber vector[3] = { first->z, second->z, third->z };
		RealNumber* result = AlgebraMaster::solveEquationSystem(3, columns, vector);
		if (result != NULL)
		{
			mode = Y0;
			a = result[0];
			b = result[1];
			c = result[2];
			delete result;
			return;
		}
	}

	{
		const Point4D* first = point1;
		const Point4D* second = point2->w == point1->w ? point3->w == point1->w ? point4 : point3 : point2;
		RealNumber ws[2] = { first->w, second->w };
		RealNumber ones[2] = { 1, 1 };
		RealNumber* columns[2] = { ws, ones };
		RealNumber vector[2] = { first->x, second->x };
		RealNumber* result = AlgebraMaster::solveEquationSystem(2, columns, vector);
		if (result != NULL)
		{
			mode = Y0Z0;
			a = result[0];
			b = result[1];
			delete result;
			return;
		}
	}

	{
		mode = Y0Z0X0;
		a = point1->w;
	}
}

bool Space4D::is_parallel(const Space4D* space)
{
	if (mode == NORMAL && space->mode == NORMAL)
		return a == space->a && b == space->b && c == space->c;
	if (mode == Y0 && space->mode == Y0)
		return a == space->a && b == space->b;
	if (mode == Y0Z0 && space->mode == Y0Z0)
		return a == space->a;
	if (mode == Y0Z0X0 && space->mode == Y0Z0X0)
		return true;
	return false;
}

Point4D Space4D::calculate_origin()
{
	if (mode == NORMAL)
		return Point4D(0, d, 0, 0);
	if (mode == Y0)
		return Point4D(0, 0, c, 0);
	if (mode == Y0Z0)
		return Point4D(b, 0, 0, 0);
	if (mode == Y0Z0X0)
		return Point4D(0, 0, 0, a);
}

Vector4D Space4D::calculate_normal() const
{
	RealNumber A, B, C, D, E;
	get_parameters(A, B, C, D, E);
	return Vector4D(A, B, C, D).normalized();
}

void Space4D::get_parameters(RealNumber& A, RealNumber& B, RealNumber& C, RealNumber& D, RealNumber &E) const
{
	switch (mode)
	{
	case NORMAL:
		A = a;
		B = -1;
		C = b;
		D = c;
		E = d;
		return;
	case Y0:
		A = a;
		B = 0;
		C = -1;
		D = b;
		E = c;
		return;
	case Y0Z0:
		A = -1;
		B = 0;
		C = 0;
		D = a;
		E = b;
		return;
	case Y0Z0X0:
		A = 0;
		B = 0;
		C = 0;
		D = -1;
		E = a;
	}
}

bool Space4D::contains(const Point4D* point)
{
	switch (mode)
	{
	case NORMAL:
		return point->y == a*point->x + b*point->z + c*point->w + d;
	case Y0:
		return point->z == a*point->x + b*point->w + c;
	case Y0Z0:
		return point->x == a*point->w + b;
	case Y0Z0X0:
		return point->w == a;
	}
}