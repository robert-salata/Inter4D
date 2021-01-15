#include "geometry_master.h"
#include "algebra_master.h"

const RealNumber GeometryMaster::PI = 3.14159265358979323846264338327950288419716939937510582097494459230781640628620899862803482534211706798214808651328;

OperationResult3D GeometryMaster::intersect(HalfSpace3D* halfspace, Plane3D* plane)
{
	if (halfspace->get_boundary_plane()->is_parallel(plane))
	{
		if (halfspace->contains(&plane->calculate_origin()))
			return OperationResult3D(OperationResult3D::PLANE, new Plane3D(*plane));
		else
			return OperationResult3D(OperationResult3D::EMPTYSET);
	}
	else
		return OperationResult3D(OperationResult3D::HALFPLANE, new HalfPlane3D(halfspace, plane));
}

OperationResult3D GeometryMaster::intersect(Line3D* line, Plane3D* plane)
{
	if (plane->calculate_normal().dot_product(line->get_direction()) == 0)
	{
		if (plane->contains(&line->get_origin()))
			return OperationResult3D(OperationResult3D::LINE, new Line3D(*line));
		else
			return OperationResult3D(OperationResult3D::EMPTYSET);
	}
	else
	{
		RealNumber A, B, C, D;
		plane->get_parameters(A, B, C, D);
		Point3D o = line->get_origin();
		Point3D d = line->get_direction();
		RealNumber t = -(A*o.x + B*o.y + C*o.z + D) / (A*d.x + B*d.y + C*d.z);

		return OperationResult3D(OperationResult3D::POINT, new Point3D(o + d*t));
	}
}

OperationResult4D GeometryMaster::intersect(Line4D* line, Space4D* space)
{
	if (space->calculate_normal().dot_product(line->get_direction()) == 0)
	{
		if (space->contains(&line->get_origin()))
			return OperationResult4D(OperationResult4D::LINE, new Line4D(*line));
		else
			return OperationResult4D(OperationResult4D::EMPTYSET);
	}
	else
	{
		RealNumber A, B, C, D, E;
		space->get_parameters(A, B, C, D, E);
		Point4D o = line->get_origin();
		Point4D d = line->get_direction();
		RealNumber t = -(A*o.x + B*o.y + C*o.z + D*o.w + E) / (A*d.x + B*d.y + C*d.z + D*d.w);

		return OperationResult4D(OperationResult4D::POINT, new Point4D(o + d*t));
	}
}

OperationResult3D GeometryMaster::intersect(Line3D* line1, Line3D* line2)
{
	const Point3D* o1 = &line1->get_origin();
	const Vector3D* d1 = &line1->get_direction();
	const Point3D* o2 = &line2->get_origin();
	const Vector3D* d2 = &line2->get_direction();

	RealNumber first_parameter[2] = { d1->x, d1->y };
	RealNumber second_parameter[2] = { d2->x, d2->y };
	RealNumber* parameters[2] = { first_parameter, second_parameter };
	RealNumber values[2] = { o2->x - o1->x, o2->y - o1->y };
	RealNumber* result = AlgebraMaster::solveEquationSystem(2, parameters, values); //t1*d1 - t2*d2 = o2 - o1
	if (result != NULL)
	{
		RealNumber t1 = result[0];
		RealNumber t2 = result[1];
		delete result;
		//check third equation
		if (t1 * d1->z - t2 * d2->z != o2->z - o1->z)
			return OperationResult3D(OperationResult3D::EMPTYSET);
		else
			return OperationResult3D(OperationResult3D::POINT, &(*o1 + (*d1) * t1));
	}
	else
	{
		RealNumber first_parameter[2] = { d1->x, d1->z };
		RealNumber second_parameter[2] = { d2->x, d2->z };
		RealNumber* parameters[2] = { first_parameter, second_parameter };
		RealNumber values[2] = { o2->x - o1->x, o2->z - o1->z };
		RealNumber* result = AlgebraMaster::solveEquationSystem(2, parameters, values); //t1*d1 - t2*d2 = o2 - o1
		if (result != NULL)
		{
			RealNumber t1 = result[0];
			RealNumber t2 = result[1];
			delete result;
			//check third equation
			if (t1 * d1->y - t2 * d2->y != o2->y - o1->y)
				return OperationResult3D(OperationResult3D::EMPTYSET);
			else
				return OperationResult3D(OperationResult3D::POINT, &(*o1 + (*d1) * t1));
		}
		else
		{
			if (*line1 == *line2)
				return OperationResult3D(OperationResult3D::LINE, new Line3D(*line1));
			else
				return OperationResult3D(OperationResult3D::EMPTYSET);
		}
	}
}

OperationResult3D GeometryMaster::intersect(Plane3D* plane1, Plane3D* plane2, Plane3D* plane3)
{
	RealNumber A1, B1, C1, D1;
	plane1->get_parameters(A1, B1, C1, D1);
	RealNumber A2, B2, C2, D2;
	plane2->get_parameters(A2, B2, C2, D2);
	RealNumber A3, B3, C3, D3;
	plane3->get_parameters(A3, B3, C3, D3);

	RealNumber xs[3] = { A1, A2, A3 };
	RealNumber ys[3] = { B1, B2, B3 };
	RealNumber zs[3] = { C1, C2, C3 };
	RealNumber* columns[3] = { xs, ys, zs };
	RealNumber values[3] = { -D1, -D2, -D3 };
	RealNumber* result = AlgebraMaster::solveEquationSystem(3, columns, values); //A*x + B*y + C*z = -D
	if (result != NULL)
	{
		OperationResult3D res = OperationResult3D(OperationResult3D::POINT, new Point3D(result[0], result[1], result[2]));
		delete result;
		return res;
	}
	else
		return OperationResult3D(OperationResult3D::EMPTYSET);
}

RealNumber GeometryMaster::distance(Plane3D* plane, Point3D* point)
{
	Line3D line = Line3D(point, &plane->calculate_normal());
	return point->distance(*intersect(&line, plane).point);
}

RealNumber GeometryMaster::distance(Space4D* space, Point4D* point)
{
	Line4D line = Line4D(point, &space->calculate_normal());
	return point->distance(*intersect(&line, space).point);
}

HalfSpace3D GeometryMaster::in_base(HalfSpace3D* halfspace, Matrix3D* base)
{
	RealNumber A, B, C, D;
	halfspace->get_boundary_plane()->get_parameters(A, B, C, D);
	Plane3D halfspace_front = Plane3D(
		A*base->m[0][0] + B*base->m[0][1] + C*base->m[0][2],
		A*base->m[1][0] + B*base->m[1][1] + C*base->m[1][2],
		A*base->m[2][0] + B*base->m[2][1] + C*base->m[2][2],
		A*base->m[3][0] + B*base->m[3][1] + C*base->m[3][2] + D
	);
	return HalfSpace3D(&halfspace_front, &(base->inverse()*halfspace->representant()));
}

HalfHyperspace4D GeometryMaster::in_base(HalfHyperspace4D* halfspace, Matrix4D* base)
{
	RealNumber A, B, C, D, E;
	halfspace->get_boundary_space()->get_parameters(A, B, C, D, E);
	Space4D halfspace_front = Space4D(
		A*base->m[0][0] + B*base->m[0][1] + C*base->m[0][2] + D*base->m[0][3],
		A*base->m[1][0] + B*base->m[1][1] + C*base->m[1][2] + D*base->m[1][3],
		A*base->m[2][0] + B*base->m[2][1] + C*base->m[2][2] + D*base->m[2][3],
		A*base->m[3][0] + B*base->m[3][1] + C*base->m[3][2] + D*base->m[3][3],
		A*base->m[4][0] + B*base->m[4][1] + C*base->m[4][2] + D*base->m[4][3] + E
	);
	return HalfHyperspace4D(&halfspace_front, &(base->inverse()*halfspace->representant()));
}

Shape3D GeometryMaster::transform(Shape3D* shape, Matrix3D* transformation)
{
	vector<Point3D*> vertices = shape->get_vertices();
	vector<Point3D> new_vertices;
	for (auto it = vertices.begin(); it != vertices.end(); it++)
		new_vertices.push_back(Point3D((*transformation)*(**it)));

	vector<Shape3D::Face*> faces = shape->get_faces();
	vector<Shape3D::Face> new_faces;
	for (auto it = faces.begin(); it != faces.end(); it++)
		new_faces.push_back(Shape3D::Face(**it));

	return Shape3D(new_vertices, new_faces);
}

Shape4D GeometryMaster::transform(Shape4D* shape, Matrix4D* transformation)
{
	vector<Point4D*> vertices = shape->get_vertices();
	vector<Point4D> new_vertices;
	for (auto it = vertices.begin(); it != vertices.end(); it++)
		new_vertices.push_back(Point4D((*transformation)*(**it)));

	vector<Shape4D::Face*> faces = shape->get_faces();
	vector<Shape4D::Face> new_faces;
	for (auto it = faces.begin(); it != faces.end(); it++)
		new_faces.push_back(Shape4D::Face(**it));

	vector<Shape4D::Cell*> cells = shape->get_cells();
	vector<Shape4D::Cell> new_cells;
	for (auto it = cells.begin(); it != cells.end(); it++)
		new_cells.push_back(Shape4D::Cell(**it));

	return Shape4D(new_vertices, new_faces, new_cells);
}

Plane3D GeometryMaster::transform(Plane3D* plane, Matrix3D* transformation)
{
	RealNumber A, B, C, D;
	plane->get_parameters(A, B, C, D);
	Matrix3D inv = transformation->inverse();
	return Plane3D(
		A*inv.m[0][0] + B*inv.m[0][1] + C*inv.m[0][2],
		A*inv.m[1][0] + B*inv.m[1][1] + C*inv.m[1][2],
		A*inv.m[2][0] + B*inv.m[2][1] + C*inv.m[2][2],
		A*inv.m[3][0] + B*inv.m[3][1] + C*inv.m[3][2] + D
	);
}

Space4D GeometryMaster::transform(Space4D* space, Matrix4D* transformation)
{
	RealNumber A, B, C, D, E;
	space->get_parameters(A, B, C, D, E);
	Matrix4D inv = transformation->inverse();
	return Space4D(
		A*inv.m[0][0] + B*inv.m[0][1] + C*inv.m[0][2] + D*inv.m[0][3],
		A*inv.m[1][0] + B*inv.m[1][1] + C*inv.m[1][2] + D*inv.m[1][3],
		A*inv.m[2][0] + B*inv.m[2][1] + C*inv.m[2][2] + D*inv.m[2][3],
		A*inv.m[3][0] + B*inv.m[3][1] + C*inv.m[3][2] + D*inv.m[3][3],
		A*inv.m[4][0] + B*inv.m[4][1] + C*inv.m[4][2] + D*inv.m[4][3] + E
	);
}

Segment3D GeometryMaster::transform(Segment3D* segment, Matrix3D* transformation)
{
	Point3D p1, p2;
	segment->get_ends(p1, p2);
	return Segment3D(&((*transformation)*p1), &((*transformation)*p2));
}


Segment4D GeometryMaster::transform(Segment4D* segment, Matrix4D* transformation)
{
	Point4D p1, p2;
	segment->get_ends(p1, p2);
	return Segment4D(&((*transformation)*p1), &((*transformation)*p2));
}
