#include "math_engine.h"
#include "geometry_master.h"
#include "timer.h"
#include <algorithm>

vector<Shape3D> MathEngine::intersect_concave(Shape3D* shape, RealNumber rX, RealNumber rY, RealNumber d, vector<Shape2D>& intersections)
{
	Plane3D z0_plane = Plane3D(0, 0, 1, 0);
	Matrix3D transformation = rotation_transformation(rX, rY, d);

	Shape3D shape_modified = GeometryMaster::transform(shape, &transformation.inverse());
	auto shape_vertices = shape_modified.get_vertices();
	for (auto it = shape_vertices.begin(); it != shape_vertices.end(); it++)
		if ((*it)->z == 0)
			(*it)->z = 0.00001; //epsilon = 0.000001

	auto shape_edges = shape->get_edges();

	struct StructPoint
	{
		StructPoint() { };
		StructPoint(Point3D p, Shape3D::Edge* edge) : point(p), edge(edge) { };
		Point3D point;
		Shape3D::Edge* edge;
		bool visited = false;
	};
	map<const Shape3D::Edge*, StructPoint> point_of_edge;

	for (int i = 0; i < shape_edges.size(); i++)
	{
		if (shape_vertices[shape_edges[i]->first]->z*shape_vertices[shape_edges[i]->second]->z < 0)
		{
			Line3D edge_line = Line3D(shape_vertices[shape_edges[i]->first], shape_vertices[shape_edges[i]->second]);
			OperationResult3D cross = GeometryMaster::intersect(&edge_line, &z0_plane);
			if (cross.result == OperationResult3D::POINT)
			{
				point_of_edge.insert(make_pair(shape_edges[i], StructPoint(*cross.point, shape_edges[i])));
				delete cross.point;
			}
			else if (cross.result == OperationResult3D::LINE)
				delete cross.line;
		}
	}

	if (point_of_edge.size() < 3)
		return vector<Shape3D>();

	vector<Shape3D> result_shapes;
	vector<Point3D> result_points;

	StructPoint* current_point = &point_of_edge.begin()->second;
	while (!current_point->visited)
	{
		result_points.push_back(current_point->point);
		current_point->visited = true;

		auto adjacent_faces = shape->faces_with_edge(current_point->edge);
		for (auto it = adjacent_faces.begin(); it != adjacent_faces.end() && current_point->visited; it++)
		{
			auto possible_edges = shape->edges_cycle_of_face(*it);
			for (auto it2 = possible_edges.begin(); it2 != possible_edges.end() && current_point->visited; it2++)
				if (point_of_edge.find(*it2) != point_of_edge.end() && !point_of_edge[*it2].visited)// && 
					//!are_concave(&shape_modified, current_point->edge, *it2))
					current_point = &point_of_edge[*it2];
		}

		if (current_point->visited)
		{
			if (result_points.size() >= 3)
			{
				vector<int> face;
				for (int i = 0; i < result_points.size(); i++)
					face.push_back(i);
				result_shapes.push_back(Shape3D(result_points, vector<Shape3D::Face>{face}));
			}
			result_points.clear();

			for (auto it = point_of_edge.begin(); it != point_of_edge.end() && current_point->visited; it++)
				if (!it->second.visited)
					current_point = &it->second;
		}
	}

	intersections.clear();
	for (int i = 0; i < result_shapes.size(); i++)
	{
		intersections.push_back(flatten_z(&result_shapes[i]));
		result_shapes[i] = GeometryMaster::transform(&result_shapes[i], &transformation);
	}

	return result_shapes;
}

Shape4D MathEngine::intersect_concave(Shape4D* shape, RealNumber rXZ, RealNumber rYZ, RealNumber rXY, RealNumber d, Shape3D& intersection)
{
	Space4D w0_space = Space4D(0, 0, 0, 1, 0);
	Matrix4D transformation = rotation_transformation(rXZ, rYZ, rXY, d);

	Shape4D shape_modified = GeometryMaster::transform(shape, &transformation.inverse());
	auto shape_vertices = shape_modified.get_vertices();
	for (auto it = shape_vertices.begin(); it != shape_vertices.end(); it++)
		if ((*it)->w == 0)
			(*it)->w = 0.00001; //epsilon = 0.000001

	auto shape_edges = shape->get_edges();

	struct StructPoint
	{
		StructPoint() { };
		StructPoint(Point4D p, Shape4D::Edge* edge) : point(p), edge(edge) { };
		Point4D point;
		Shape4D::Edge* edge;
		int index;
		bool visited = false;
	};
	map<Shape4D::Edge*, StructPoint> point_of_edge;

	for (int i = 0; i < shape_edges.size(); i++)
	{
		if (shape_vertices[shape_edges[i]->first]->w*shape_vertices[shape_edges[i]->second]->w < 0)
		{
			Line4D edge_line = Line4D(shape_vertices[shape_edges[i]->first], shape_vertices[shape_edges[i]->second]);
			OperationResult4D cross = GeometryMaster::intersect(&edge_line, &w0_space);
			if (cross.result == OperationResult4D::POINT)
			{
				point_of_edge.insert(make_pair(shape_edges[i], StructPoint(*cross.point, shape_edges[i])));
				delete cross.point;
			}
			else if (cross.result == OperationResult4D::LINE)
				delete cross.line;
		}
	}

	map<Shape4D::Cell*, set<Shape4D::Edge*>> edges_of_cell;
	for (auto it = point_of_edge.begin(); it != point_of_edge.end(); it++)
	{
		auto corresponding_cells = shape->cells_with_edge(it->first);
		for (auto it2 = corresponding_cells.begin(); it2 != corresponding_cells.end(); it2++)
		{
			if (edges_of_cell.find(*it2) == edges_of_cell.end())
				edges_of_cell.insert(make_pair(*it2, set<Shape4D::Edge*>()));
			edges_of_cell[*it2].insert(it->first);
		}
	}

	vector<Point4D> result_points;
	int index = 0;
	for (auto it = point_of_edge.begin(); it != point_of_edge.end(); it++)
	{
		result_points.push_back(it->second.point);
		it->second.index = index++;
	}

	vector<Shape4D::Face> result_faces;
	for (auto it = edges_of_cell.begin(); it != edges_of_cell.end(); it++)
		if (it->second.size() >= 3)
		{
			for (auto it2 = point_of_edge.begin(); it2 != point_of_edge.end(); it2++)
				it2->second.visited = false;

			vector<int> face;
			auto faces_of_cell = shape->faces_of_cell(it->first);

			StructPoint* current_point = &point_of_edge[*(it->second.begin())];
			while (!current_point->visited)
			{
				face.push_back(current_point->index);
				current_point->visited = true;

				auto adjacent_faces = shape->faces_with_edge(current_point->edge);
				for (auto it = adjacent_faces.begin(); it != adjacent_faces.end() && current_point->visited; it++)
					if (faces_of_cell.find(*it) != faces_of_cell.end())
					{
						auto possible_edges = shape->edges_cycle_of_face(*it);
						for (auto it2 = possible_edges.begin(); it2 != possible_edges.end() && current_point->visited; it2++)
							if (point_of_edge.find(*it2) != point_of_edge.end() && !point_of_edge[*it2].visited)
								current_point = &point_of_edge[*it2];
					}

				if (current_point->visited)
				{
					if (face.size() >= 3)
						result_faces.push_back(Shape4D::Face{face});
				}
			}
		}

	set<int> cell;
	for (int i = 0; i < result_faces.size(); i++)
		cell.insert(i);

	Shape4D result = Shape4D(result_points, result_faces, vector<Shape4D::Cell>{cell});

	intersection = flatten_w(&result);
	intersection.recalculate_normals();
	result = GeometryMaster::transform(&result, &transformation);

	return result;
}

/*bool MathEngine::are_concave(Shape3D* shape, Shape3D::Edge* edge1, Shape3D::Edge* edge2)
{
	auto shape_vertices = shape->get_vertices();
	Vector3D vector1 = *shape_vertices[edge1->second] - *shape_vertices[edge1->first];
	Vector3D vector2 = *shape_vertices[edge2->second] - *shape_vertices[edge2->first];
	return vector1.dot_product
}*/

OperationResult2D MathEngine::flatten_z(HalfSpace3D* halfspace)
{
	RealNumber A, B, C, D;
	halfspace->get_boundary_plane()->get_parameters(A, B, C, D);
	if (A == 0 && B == 0)
		return halfspace->contains(&Point3D(0, 0, 0)) ?
		OperationResult2D(OperationResult2D::PLANE) : OperationResult2D(OperationResult2D::EMPTYSET);
	Line2D edge = Line2D(A, B, D);
	Point2D o = edge.calculate_origin();
	Vector2D n = edge.calculate_normal();
	Point2D representant = o + n;
	if (!halfspace->contains(&Point3D(representant.x, representant.y, 0)))
		representant = o - n;
	return OperationResult2D(OperationResult2D::HALFPLANE, new HalfPlane2D(&edge, &representant));
}

OperationResult3D MathEngine::flatten_w(HalfHyperspace4D* halfspace)
{
	RealNumber A, B, C, D, E;
	halfspace->get_boundary_space()->get_parameters(A, B, C, D, E);
	if (A == 0 && B == 0 && C == 0)
		return halfspace->contains(&Point4D(0, 0, 0, 0)) ?
		OperationResult3D(OperationResult3D::SPACE) : OperationResult3D(OperationResult3D::EMPTYSET);
	Plane3D boundary = Plane3D(A, B, C, E);
	Point3D o = boundary.calculate_origin();
	Vector3D n = boundary.calculate_normal();
	Point3D representant = o + n;
	if (!halfspace->contains(&Point4D(representant.x, representant.y, representant.z, 0)))
		representant = o - n;
	return OperationResult3D(OperationResult3D::HALFSPACE, new HalfSpace3D(&boundary, &representant));
}

Shape2D MathEngine::flatten_z(Shape3D* shape)
{
	vector<Point2D> vertices;
	auto shape_vertices = shape->get_vertices();
	for (int i = 0; i < shape_vertices.size(); i++)
		vertices.push_back(Point2D(shape_vertices[i]->x, shape_vertices[i]->y));
	return Shape2D(vertices);
}

Shape3D MathEngine::flatten_w(Shape4D* shape)
{
	vector<Point3D> vertices;
	auto shape_vertices = shape->get_vertices();
	for (int i = 0; i < shape_vertices.size(); i++)
		vertices.push_back(Point3D(shape_vertices[i]->x, shape_vertices[i]->y, shape_vertices[i]->z));

	vector<Shape3D::Face> faces;
	auto shape_faces = shape->get_faces();
	for (int i = 0; i < shape_faces.size(); i++)
		faces.push_back(Shape3D::Face(shape_faces[i]->get_indices_cycle()));

	return Shape3D(vertices, faces);
}

Matrix3D MathEngine::rotation_transformation(RealNumber rX, RealNumber rY, RealNumber d)
{
	return Matrix3D::rotation_y(rY)*Matrix3D::rotation_x(-rX)*Matrix3D::translation(0, 0, d);
}

Matrix4D MathEngine::rotation_transformation(RealNumber rXZ, RealNumber rYZ, RealNumber rXY, RealNumber d)
{
	return Matrix4D::rotation_xy(-rXY)*Matrix4D::rotation_yz(-rYZ)*Matrix4D::rotation_xz(-rXZ)*Matrix4D::translation(0, 0, 0, d);
}

void MathEngine::get_rotation_params(Plane3D* plane, RealNumber& rX, RealNumber& rY, RealNumber& d)
{
	RealNumber A, B, C, D;
	plane->get_parameters(A, B, C, D);
	if (A == 0 && C == 0)
	{
		rX = GeometryMaster::PI/2;
		rY = 0;
		d = -(-D / B);
	}
	else
	{
		rY = C == 0 ? GeometryMaster::PI / 2 : -atan(A / C);
		if (rY < 0)
			rY += 2 * GeometryMaster::PI;
		rX = B == 0 ? 0 : atan(A == 0 ? cos(rY) / (-C / B) : -sin(rY) / (-A / B));
		if (rX == 0)
		{
			if (-D / C < 0)
				rY += GeometryMaster::PI;
		}
		if (rX < 0)
		{
			rX = -rX;
			rY += GeometryMaster::PI;
		}
		d = B != 0 ? -D * sin(rX) : A != 0 ? D / A * sin(rY)*cos(rX) : -D / C * cos(rY)*cos(rX);
	}
}

void MathEngine::get_rotation_params(Space4D* space, RealNumber& rXZ, RealNumber& rYZ, RealNumber &rXY, RealNumber& d)
{
	RealNumber A, B, C, D, E;
	space->get_parameters(A, B, C, D, E);
	if (A == 0 && C == 0 && D == 0)
	{
		rXY = 0; //any
		rYZ = 0; //any
		rXZ = GeometryMaster::PI / 2;
	}
	else if (C == 0 && D == 0)
	{
		rXY = 0; //any
		rYZ = GeometryMaster::PI / 2;
		rXZ = B == 0 ? 0 : -atan(1 / (A / B));
	}
	else if (D == 0)
	{
		rXY = GeometryMaster::PI / 2;
		rYZ = atan(1 * A / C);
		rXZ = B == 0 ? 0 : atan(A != 0 ? -sin(rYZ) / (A / B) : -cos(rYZ)/ (C / B));
	}
	else
	{
		rXY = atan(-C / D);
		rYZ = atan(C != 0 ? sin(rXY) * (A / C) : -cos(rXY) * (A / D));
		rXZ = B == 0 ? 0 : atan(cos(rYZ)*cos(rXY) / (D / B));
	}

	d = B != 0 ? -E * sin(rXZ) : A != 0 ? -E / A * sin(rYZ)*cos(rXZ) : C != 0 ? -E / C * cos(rYZ)*sin(rXY)*cos(rXZ) : E / D * cos(rYZ)*cos(rXY)*cos(rXZ);
}

Shape3D MathEngine::project(Shape4D* shape, RealNumber rXZ, RealNumber rYZ, RealNumber rXY, RealNumber d)
{
	Shape4D rotated_shape = GeometryMaster::transform(shape, &rotation_transformation(rXZ, rYZ, rXY, 0).inverse());
	Space4D clipping_space = Space4D(0, 0, 0, -1, d - 0.1);

	vector<Point4D> clipped_vertices;
	vector<Point4D*> shape_vertices = rotated_shape.get_vertices();
	vector<bool> is_wrong;
	for (int i = 0; i < shape_vertices.size(); i++)
	{
		is_wrong.push_back(d - shape_vertices[i]->w <= 0.1);
		clipped_vertices.push_back(*shape_vertices[i]);
	}

	vector<Shape4D::Face> clipped_faces;
	vector<Shape4D::Face*> shape_faces = shape->get_faces();
	for (auto it = shape_faces.begin(); it != shape_faces.end(); it++)
	{
		vector<Shape4D::Edge> face_edges;
		auto face_vertices = (*it)->get_indices_cycle();
		for (int i = 0; i < face_vertices.size() - 1; i++)
			face_edges.push_back(Shape4D::Edge(face_vertices[i], face_vertices[i + 1]));
		face_edges.push_back(Shape4D::Edge(face_vertices[face_vertices.size() - 1], face_vertices[0]));

		vector<Shape4D::Edge> clipped_face_edges;
		vector<Point4D> artificial;

		for (int i = 0; i < face_edges.size(); i++)
		{
			if (!is_wrong[face_edges[i].first] && !is_wrong[face_edges[i].second])
				clipped_face_edges.push_back(face_edges[i]);
			else if (is_wrong[face_edges[i].first] && is_wrong[face_edges[i].second])
				;
			else 
			{
				Line4D edge_line = Line4D(shape_vertices[face_edges[i].first], shape_vertices[face_edges[i].second]);
				OperationResult4D cross = GeometryMaster::intersect(&edge_line, &clipping_space);
				artificial.push_back(*cross.point);

				clipped_face_edges.push_back(Shape4D::Edge(is_wrong[face_edges[i].first] ? face_edges[i].second : face_edges[i].first,
					clipped_vertices.size() + artificial.size() - 1));
				delete cross.point;
			}
		}

		if (artificial.size() == 2)
		{
			clipped_face_edges.push_back(Shape4D::Edge(clipped_vertices.size(), clipped_vertices.size() + 1));
			clipped_vertices.push_back(artificial[0]);
			clipped_vertices.push_back(artificial[1]);
		}
		clipped_faces.push_back(Shape4D::Face(clipped_face_edges));
	}

	/*for (int i = 0; i < is_wrong.size(); i++)
		if (is_wrong[i])
		{
			clipped_vertices.erase(clipped_vertices.begin() + i);
			is_wrong.erase(is_wrong.begin() + i);
			for (int f = 0; f < clipped_faces.size(); f++)
			{
				auto face_indices = clipped_faces[f].get_indices_cycle();
				for (int j = 0; j < face_indices.size(); j++)
					if (face_indices[j] >= i)
						face_indices[j]--;
				clipped_faces[f] = Shape4D::Face(face_indices);
			}
			i--;
		}*/

	map<int, int> new_position;
	int position = 0;
	for (int i = 0; i < clipped_vertices.size(); i++)
		if (i < is_wrong.size() && is_wrong[i])
		{
			clipped_vertices.erase(clipped_vertices.begin() + i);
			is_wrong.erase(is_wrong.begin() + i);
			position++;
			i--;
		}
		else
			new_position.insert(make_pair(position++, new_position.size()));

	for (int f = 0; f < clipped_faces.size(); f++)
	{
		auto face_indices = clipped_faces[f].get_indices_cycle();
		for (int j = 0; j < face_indices.size(); j++)
			face_indices[j] = new_position[face_indices[j]];
		clipped_faces[f] = Shape4D::Face(face_indices);
	}

	vector<Point3D> projected_vertices;
	for (int i = 0; i < clipped_vertices.size(); i++)
	{
		Vector3D distance_to_viewpoint = Point3D(clipped_vertices[i].x, clipped_vertices[i].y, clipped_vertices[i].z) - Point3D(0, 0, 0);
		RealNumber w_distance = d - clipped_vertices[i].w;
		Point3D projected_point = distance_to_viewpoint / w_distance;
		projected_vertices.push_back(projected_point);
	}

	vector<Shape3D::Face> new_faces;
	for (auto it = clipped_faces.begin(); it != clipped_faces.end(); it++)
		new_faces.push_back(Shape3D::Face((*it).get_indices_cycle()));

	Shape3D result = Shape3D(projected_vertices, new_faces);
	return result;
}

Segment3D MathEngine::project(Segment4D* segment, RealNumber rXZ, RealNumber rYZ, RealNumber rXY, RealNumber d)
{
	Segment4D rotated_segment = GeometryMaster::transform(segment, &rotation_transformation(rXZ, rYZ, rXY, 0).inverse());
	Point4D p1, p2;
	rotated_segment.get_ends(p1, p2);

	vector<Point3D> projected_vertices;
	vector<Point4D> shape_vertices{ p1, p2 };
	for (int i = 0; i < shape_vertices.size(); i++)
	{
		Vector3D distance_to_viewpoint = Point3D(shape_vertices[i].x, shape_vertices[i].y, shape_vertices[i].z) - Point3D(0, 0, 0);
		RealNumber w_distance = d - shape_vertices[i].w;
		Point3D projected_point = distance_to_viewpoint / w_distance;
		projected_vertices.push_back(projected_point);
	}

	return Segment3D(&projected_vertices[0], &projected_vertices[1]);
}

vector<Shape2D> MathEngine::triangulate(Shape2D* polygon)
{
	vector<Shape2D> result;

	if (polygon->get_vertices_cycle().size() == 4)
		int a = 2;

	Shape2D copy = *polygon;
	if (copy.is_clockwise_oriented())
		copy.change_orientation();
	vector<Point2D> vertices = copy.get_vertices_cycle();

	int n = vertices.size();
	if (n < 3)
		return result;
	if (n == 3)
		return vector<Shape2D>{*polygon};
	

	for (int i = 0; i < vertices.size(); i++)
	{
		Point2D possible_ear = vertices[i];
		Point2D neighbor_1 = vertices[(i - 1 + n) % n];
		Point2D neighbor_2 = vertices[(i + 1) % n];
		Vector2D v1 = possible_ear - neighbor_1;
		Vector2D v2 = neighbor_2 - possible_ear;
		if (v1.x*v2.y - v1.y*v2.x <= 0) //possible ear is concave
			continue;

		Shape2D triangle = Shape2D(vector<Point2D>{neighbor_1, possible_ear, neighbor_2});
		
		bool is_ear = true;
		for (int j = 0; j < vertices.size() && is_ear; j++)
			if (j != (i - 1 + n) % n && j != i && j != (i + 1) % n && triangle.contains(&vertices[j]))
			{
				is_ear = false;
				triangle.contains(&vertices[j]);
			}

		if (is_ear)
		{
			vertices.erase(vertices.begin() + i);
			result = triangulate(&Shape2D(vertices));
			result.push_back(triangle);
			return result;
		}
	}
	return result;
}