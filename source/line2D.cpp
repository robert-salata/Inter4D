#include "line2D.h"
#include "algebra_master.h"

Line2D::Line2D(const Line2D& line)
{
	this->a = line.a;
	this->b = line.b;
	this->mode = line.mode;
}

Line2D::Line2D(RealNumber A, RealNumber B, RealNumber C)
{
	if (B == 0)
	{
		mode = VERTICAL;
		a = -C / A;
	}
	else
	{
		mode = NORMAL;
		a = -A / B;
		b = -C / B;
	}
}

Line2D::Line2D(Point2D* point1, Point2D* point2)
{
	if (point1->x == point2->x)
	{
		mode = VERTICAL;
		a = point1->x;
		return;
	}

	RealNumber xs[2] = { point1->x, point2->x, };
	RealNumber ones[2] = { 1, 1 };
	RealNumber* columns[2] = { xs, ones };
	RealNumber vector[2] = { point1->y, point2->y, };
	RealNumber* result = AlgebraMaster::solveEquationSystem(2, columns, vector);
	if (result != NULL)
	{
		mode = NORMAL;
		a = result[0];
		b = result[1];
		delete result;
	}
}

bool Line2D::operator == (Line2D line)
{
	return mode == line.mode && a == line.a && b == line.b;
}

bool Line2D::operator != (Line2D line)
{
	return !(*this == line);
}

bool Line2D::is_parallel(Line2D* plane)
{
	if (mode == NORMAL && plane->mode == NORMAL)
		return a == plane->a;
	if (mode == VERTICAL && plane->mode == VERTICAL)
		return true;
	return false;
}

Point2D Line2D::calculate_origin()
{
	if (mode == NORMAL)
		return Point2D(RealNumber(0), b);
	if (mode == VERTICAL)
		return Point2D(a, RealNumber(0));
}

Vector2D Line2D::calculate_normal()
{
	switch (mode)
	{
	case NORMAL:
		return Vector2D(-a, 1).normalized();
	case VERTICAL:
		return Vector2D(1, 0);
	}
}

OperationResult2D Line2D::intersect(Line2D* plane)
{
	Line2D* line1 = this;
	Line2D* line2 = plane;
	if (line1->mode == NORMAL && line2->mode == NORMAL)
	{
		RealNumber x_column[2] = { line1->a, line2->a };
		RealNumber y_column[2] = { -1, -1 };
		RealNumber* columns[2] = { x_column, y_column };
		RealNumber c_column[2] = { -line1->b, -line2->b };
		RealNumber* result = AlgebraMaster::solveEquationSystem(2, columns, c_column); //-b = a*x - y
		if (result != NULL)
		{
			OperationResult2D res = OperationResult2D(OperationResult2D::POINT, new Point2D(result[0], result[1]));
			delete result;
			return res;
		}
		else
			return OperationResult2D(OperationResult2D::EMPTYSET);
	}
	if (line1->mode == VERTICAL && line2->mode == NORMAL)
		common::swap<Line2D*>(line1, line2);
	if (line1->mode == NORMAL && line2->mode == VERTICAL)
	{
		RealNumber x = line2->a;
		RealNumber y = line1->a*x + line1->b;
		return OperationResult2D(OperationResult2D::POINT, new Point2D(x, y));
	}
	if (line1->mode == VERTICAL && line2->mode == VERTICAL)
		return line1->a == line2->a ? OperationResult2D(OperationResult2D::LINE, line1) : OperationResult2D(OperationResult2D::EMPTYSET);
}

void Line2D::get_parameters(RealNumber& A, RealNumber& B, RealNumber& C)
{
	switch (mode)
	{
	case NORMAL:
		A = a;
		B = -1;
		C = b;
		return;
	case VERTICAL:
		A = -1;
		B = 0;
		C = a;
		return;
	}
}

bool Line2D::contains(Point2D* point)
{
	switch (mode)
	{
	case NORMAL:
		return point->y == a*point->x + b;
	case VERTICAL:
		return point->x == a;
	}
}