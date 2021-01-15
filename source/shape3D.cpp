#include "shape3D.h"
#include "vector3D.h"
#include "half_space3D.h"
#include "geometry_master.h"
#include "math_engine.h"

Shape3D::Edge::Edge(int index1, int index2)
{
	if (index1 < index2)
	{
		first = index1;
		second = index2;
	}
	else
	{
		first = index2;
		second = index1;
	}
}

bool Shape3D::Edge::operator == (Edge edge2)
{
	return first == edge2.first && second == edge2.second;
}

bool Shape3D::Edge::operator < (const Edge edge2) const
{
	return first < edge2.first || (first == edge2.first && second < edge2.second);
}

Shape3D::Face::Face(const vector<int> vertices)
{
	for (int i = 0; i < vertices.size(); i++)
		this->vertices_cycle.push_back(vertices[i]);
}

vector<int> Shape3D::Face::get_indices_cycle() const
{
	return this->vertices_cycle;
}

void Shape3D::Face::revert_cycle()
{
	vector<int> new_cycle;
	for (int i = vertices_cycle.size() - 1; i >= 0; i--)
		new_cycle.push_back(vertices_cycle[i]);
	vertices_cycle = new_cycle;
}

Shape3D::Shape3D()
{

}

Shape3D::Shape3D(const vector<Point3D> vertices, const vector<Face> faces)
{
	for (auto it = vertices.begin(); it != vertices.end(); it++)
		this->vertices.push_back(Point3D(*it));

	for (auto it = faces.begin(); it != faces.end(); it++)
		this->faces.push_back(Face(*it));
}

Shape3D::Shape3D(Shape2D* flat_shape)
{
	vector<Point2D> flat_points = flat_shape->get_vertices_cycle();
	vector<int> face_points;

	for (int i = 0; i != flat_points.size(); i++)
	{
		this->vertices.push_back(Point3D(flat_points[i].x, flat_points[i].y, 0));
		face_points.push_back(i);
	}

	this->faces.push_back(Face(face_points));
}

Shape3D Shape3D::empty()
{
	return Shape3D();
}

vector<Point3D*> Shape3D::get_vertices()
{
	vector<Point3D*> result;
	for (int i = 0; i < vertices.size(); i++)
		result.push_back(&vertices[i]);
	return result;
}

vector<Shape3D::Face*> Shape3D::get_faces()
{
	vector<Face*> result;
	for (int i = 0; i < faces.size(); i++)
		result.push_back(&faces[i]);
	return result;
}

vector<Shape3D::Edge*> Shape3D::get_edges()
{
	if (!calculated_faces_of_edge)
		calculate_faces_of_edge();

	vector<Edge*> result;
	for (auto it = edges.begin(); it != edges.end(); it++)
		result.push_back(&*it);
	return result;
}

Point3D Shape3D::calculate_center()
{
	vector<Point3D*> vertices = get_vertices();
	Point3D result = Point3D(0, 0, 0);
	for (auto it = vertices.begin(); it != vertices.end(); it++)
		result += **it;
	result /= vertices.size();
	return result;
}

vector<Point3D*> Shape3D::vertices_cycle_of_face(Shape3D::Face* face)
{
	if (face->calculated_view_vertices)
		return face->view_vertices;

	vector<int> indices = face->get_indices_cycle();
	for (int i = 0; i < indices.size(); i++)
		face->view_vertices.push_back(&vertices[indices[i]]);

	face->calculated_view_vertices = true;
	return face->view_vertices;
}

vector<Shape3D::Edge*> Shape3D::edges_cycle_of_face(Shape3D::Face* face)
{
	vector<Edge*> result;
	vector<int> indices = face->get_indices_cycle();
	if (indices.size() >= 2)
	{
		for (int i = 0; i < (int)indices.size() - 1; i++)
			result.push_back(&*std::find(edges.begin(), edges.end(), Edge(indices[i], indices[i + 1])));
		result.push_back(&*std::find(edges.begin(), edges.end(), Edge(indices[0], indices[indices.size() - 1])));
	}
	return result;
}

void Shape3D::recalculate_normals()
{
	for (auto it = faces.begin(); it != faces.end(); it++)
		if ((*it).get_indices_cycle().size() >= 3)
		{
			Point3D p1 = vertices[(*it).get_indices_cycle()[0]];
			Point3D p2 = vertices[(*it).get_indices_cycle()[1]];
			Point3D p3 = vertices[(*it).get_indices_cycle()[2]];
			Vector3D v1 = p1 - p2;
			Vector3D v2 = p3 - p2;
			Vector3D normal = v1.product(v2);
			if (!HalfSpace3D(&Plane3D(&p1, &p2, &p3), &(p1 + normal)).contains(&calculate_center()))
				(*it).revert_cycle();
		}
}

void Shape3D::calculate_edges()
{
	for (auto it = faces.begin(); it != faces.end(); it++)
	{
		vector<int> indices = (*it).get_indices_cycle();
		if (indices.size() >= 2)
		{
			for (int i = 0; i < indices.size() - 1; i++)
			{
				Edge edge = Edge(indices[i], indices[i + 1]);
				if (std::find(edges.begin(), edges.end(), edge) == edges.end())
					edges.push_back(edge);
			}
			Edge edge = Edge(indices[0], indices[indices.size() - 1]);
			if (std::find(edges.begin(), edges.end(), edge) == edges.end())
				edges.push_back(edge);
		}
	}

	calculated_edges = true;
}

map<Shape3D::Face*, set<Shape3D::Face*>> Shape3D::get_adjacent_faces()
{
	if (calculated_adjacent_faces)
		return adjacent_faces;

	if (!calculated_faces_of_edge)
		calculate_faces_of_edge();

	for (auto it = faces.begin(); it != faces.end(); it++)
		adjacent_faces.insert(make_pair(&*it, set<Face*>()));

	for (auto it = faces_of_edge.begin(); it != faces_of_edge.end(); it++)
		for (auto it2 = it->second.begin(); it2 != it->second.end(); it2++)
			for (auto it3 = std::next(it2, 1); it3 != it->second.end(); it3++)
			{
				adjacent_faces[*it2].insert(*it3);
				adjacent_faces[*it3].insert(*it2);
			}

	calculated_adjacent_faces = true;
	return adjacent_faces;
}

set<Shape3D::Face*> Shape3D::faces_with_edge(Shape3D::Edge* edge)
{
	if (!calculated_faces_of_edge)
		calculate_faces_of_edge();
	return faces_of_edge[edge];
}

void Shape3D::calculate_faces_of_edge()
{
	if (!calculated_edges)
		calculate_edges();

	for (auto it = faces.begin(); it != faces.end(); it++)
	{
		auto edges = edges_cycle_of_face(&*it);

		for (auto it2 = edges.begin(); it2 != edges.end(); it2++)
		{
			if (faces_of_edge.find(*it2) == faces_of_edge.end())
				faces_of_edge.insert(make_pair(*it2, set<Shape3D::Face*>()));
			faces_of_edge[*it2].insert(&*it);
		}
	}

	calculated_faces_of_edge = true;
}

Shape2D Shape3D::flat_face(Face* face)
{
	vector<Point3D*> face_vertices = vertices_cycle_of_face(face);
	if (face_vertices.size() < 3)
		return Shape2D::empty();
	Plane3D face_plane = Plane3D(face_vertices[0], face_vertices[1], face_vertices[2]);
	RealNumber rX, rY, d;
	MathEngine::get_rotation_params(&face_plane, rX, rY, d);
	Matrix3D rotation = MathEngine::rotation_transformation(rX, rY, d).inverse();

	vector<Point2D> result_vertices;
	for (int i = 0; i < face_vertices.size(); i++)
	{
		Point3D transformed = rotation**face_vertices[i];
		result_vertices.push_back(Point2D(transformed.x, transformed.y));
	}
	return Shape2D(result_vertices);
}

bool Shape3D::is_closed()
{
	if (!calculated_adjacent_faces)
		calculate_faces_of_edge();

	for (auto it = faces_of_edge.begin(); it != faces_of_edge.end(); it++)
		if (it->second.size() != 2)
			return false;
	return true;
}

bool Shape3D::has_valid_faces()
{
	for (auto it = faces.begin(); it != faces.end(); it++)
		if (faces.size() > 3)
		{
			vector<Point3D*> face_vertices = vertices_cycle_of_face(&*it);
			Plane3D face_plane = Plane3D(face_vertices[0], face_vertices[1], face_vertices[2]);
			for (int i = 3; i < face_vertices.size(); i++)
				if (GeometryMaster::distance(&face_plane, face_vertices[i]) > 0.1)
				//if (!face_plane.contains(face_vertices[i]))
					return false;
		}
	return true;
}

bool Shape3D::has_concave_faces()
{
	for (auto it = faces.begin(); it != faces.end(); it++)
	{
		Shape2D face_2d = flat_face(&*it);
		if (!face_2d.is_convex())
			return true;
	}
	return false;
}

bool Shape3D::is_convex()
{
	if (calculated_convex)
		return is_convex_value;
	calculated_convex = true;

	for (int i = 0; i < faces.size(); i++)
	{
		auto face_vertices = faces[i].get_indices_cycle();
		if (face_vertices.size() >= 3)
		{
			Vector3D edge1 = vertices[face_vertices[2]] - vertices[face_vertices[0]];
			Vector3D edge2 = vertices[face_vertices[1]] - vertices[face_vertices[0]];
			Vector3D normal = edge1.product(edge2);
			for (int j = 0; j < vertices.size(); j++)
				if (std::find(face_vertices.begin(), face_vertices.end(), j) == face_vertices.end())
					if (normal.dot_product(vertices[j] - vertices[face_vertices[0]]) < 0)
						return is_convex_value = false;
		}
	}

	return is_convex_value = true;
}