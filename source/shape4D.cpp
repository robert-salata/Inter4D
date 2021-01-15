#include "shape4D.h"
#include "space4D.h"
#include "geometry_master.h"

Shape4D::Edge::Edge(int index1, int index2)
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

bool Shape4D::Edge::operator == (Edge edge2)
{
	return first == edge2.first && second == edge2.second;
}

bool Shape4D::Edge::operator != (Edge edge2)
{
	return !(*this == edge2);
}

bool Shape4D::Edge::operator < (const Edge edge2) const
{
	return first < edge2.first || (first == edge2.first && second < edge2.second);
}


Shape4D::Face::Face(const vector<int> vertices)
{
	for (int i = 0; i < vertices.size(); i++)
		this->vertices_cycle.push_back(vertices[i]);
}

Shape4D::Face::Face(const vector<Shape4D::Edge> edges)
{
	if (edges.size() == 0)
		return;
	vertices_cycle.push_back(edges[0].first);
	vertices_cycle.push_back(edges[0].second);

	int current_vert = edges[0].second;
	Edge currend_edge = edges[0];
	while (current_vert != vertices_cycle[0])
		for (int i = 0; i < edges.size(); i++)
			if (currend_edge != edges[i] && (edges[i].first == current_vert || edges[i].second == current_vert))
			{
				current_vert = current_vert == edges[i].first ? edges[i].second : edges[i].first;
				if (current_vert != vertices_cycle[0])
					vertices_cycle.push_back(current_vert);
				currend_edge = edges[i];
				break;
			}
}

bool Shape4D::Face::operator == (Face face)
{
	if (vertices_cycle.size() != face.vertices_cycle.size())
		return false;
	int n = vertices_cycle.size();
	int first_in_second = -1;
	for (int i = 0; i < n && first_in_second == -1; i++)
		if (face.vertices_cycle[i] == vertices_cycle[0])
			first_in_second = i;
	if (first_in_second == -1)
		return false;

	bool forward = face.vertices_cycle[(first_in_second + 1) % n] == vertices_cycle[1];
	if (forward)
	{
		for (int i = 2; i < n; i++)
			if (vertices_cycle[i] != face.vertices_cycle[(first_in_second + i) % n])
				return false;
		return true;
	}
	else
	{
		for (int i = 1; i < n; i++)
			if (vertices_cycle[i] != face.vertices_cycle[(first_in_second - i + n) % n])
				return false;
		return true;
	}
}

vector<int> Shape4D::Face::get_indices_cycle()
{
	return this->vertices_cycle;
}

Shape4D::Cell::Cell(const set<int> faces)
{
	for (auto it = faces.begin(); it != faces.end(); it++)
		this->faces.insert(*it);
}

set<int> Shape4D::Cell::get_faces()
{
	return this->faces;
}

Shape4D::Shape4D()
{

}

Shape4D::Shape4D(const vector<Point4D> vertices, const vector<Face> faces, const vector<Cell> cells)
{
	for (auto it = vertices.begin(); it != vertices.end(); it++)
		this->vertices.push_back(Point4D(*it));

	for (auto it = faces.begin(); it != faces.end(); it++)
		this->faces.push_back(Face(*it));

	for (auto it = cells.begin(); it != cells.end(); it++)
		this->cells.push_back(Cell(*it));
}

Shape4D::Shape4D(Shape3D* shape_3d)
{
	vector<Point3D*> points_3d = shape_3d->get_vertices();
	for (int i = 0; i != points_3d.size(); i++)
		this->vertices.push_back(Point4D(points_3d[i]->x, points_3d[i]->y, points_3d[i]->z, 0));

	vector<Shape3D::Face*> faces_3d = shape_3d->get_faces();
	for (int i = 0; i < faces_3d.size(); i++)
		this->faces.push_back(Face(faces_3d[i]->get_indices_cycle()));

	set<int> cell_faces;
	for (int i = 0; i < faces_3d.size(); i++)
		cell_faces.insert(i);

	this->cells.push_back(Cell(cell_faces));
}

Shape4D Shape4D::empty()
{
	return Shape4D();
}

vector<Point4D*> Shape4D::get_vertices()
{
	vector<Point4D*> result;
	for (int i = 0; i < vertices.size(); i++)
		result.push_back(&vertices[i]);
	return result;
}

vector<Shape4D::Edge*> Shape4D::get_edges()
{
	if (!calculated_edges)
		calculate_edges();

	vector<Edge*> result;
	for (auto it = edges.begin(); it != edges.end(); it++)
		result.push_back(&*it);
	return result;
}

vector<Shape4D::Face*> Shape4D::get_faces()
{
	vector<Face*> result;
	for (int i = 0; i < faces.size(); i++)
		result.push_back(&faces[i]);
	return result;
}

vector<Shape4D::Cell*> Shape4D::get_cells()
{
	vector<Cell*> result;
	for (int i = 0; i < cells.size(); i++)
		result.push_back(&cells[i]);
	return result;
}

Point4D Shape4D::calculate_center()
{
	Point4D result = Point4D(0, 0, 0, 0);
	for (auto it = vertices.begin(); it != vertices.end(); it++)
		result += *it;
	result /= vertices.size();
	return result;
}

vector<Point4D*> Shape4D::vertices_cycle_of_face(Shape4D::Face* face)
{
	vector<Point4D*> result;
	vector<int> indices = face->get_indices_cycle();
	for (int i = 0; i < indices.size(); i++)
		result.push_back(&vertices[indices[i]]);
	return result;
}

set<Shape4D::Face*> Shape4D::faces_of_cell(Cell* cell)
{
	set<Face*> result;
	set<int> indices = cell->get_faces();
	for (auto it = indices.begin(); it != indices.end(); it++)
		result.insert(&faces[*it]);
	return result;
}

set<Shape4D::Face*> Shape4D::faces_with_edge(Shape4D::Edge* edge)
{
	if (!calculated_faces_of_edge)
		calculate_faces_of_edge();
	return faces_of_edge[edge];
}

set<Point4D*> Shape4D::vertices_of_cell(Cell* cell)
{
	set<Point4D*> result;
	auto faces = faces_of_cell(cell);
	for (auto it = faces.begin(); it != faces.end(); it++)
	{
		auto verts = vertices_cycle_of_face(*it);
		for (int i = 0; i < verts.size(); i++)
			result.insert(verts[i]);
	}
	return result;
}

map<Shape4D::Cell*, set<Shape4D::Cell*>> Shape4D::get_adjacent_cells()
{
	if (calculated_adjacent_cells)
		return adjacent_cells;

	if (!calculated_cells_of_face)
		calculate_cells_of_face();

	for (auto it = cells.begin(); it != cells.end(); it++)
		adjacent_cells.insert(make_pair(&*it, set<Cell*>()));

	for (auto it = cells_of_face.begin(); it != cells_of_face.end(); it++)
		for (auto it2 = it->second.begin(); it2 != it->second.end(); it2++)
			for (auto it3 = std::next(it2, 1); it3 != it->second.end(); it3++)
			{
				adjacent_cells[*it2].insert(*it3);
				adjacent_cells[*it3].insert(*it2);
			}

	calculated_adjacent_cells = true;
	return adjacent_cells;
}

set<Shape4D::Cell*> Shape4D::cells_with_face(Shape4D::Face* face)
{
	if (!calculated_cells_of_face)
		calculate_cells_of_face();
	return cells_of_face[face];
}

set<Shape4D::Cell*> Shape4D::cells_with_edge(Shape4D::Edge* edge)
{
	if (!calculated_cells_of_edge)
		calculate_cells_of_edge();
	return cells_of_edge[edge];
}

void Shape4D::calculate_cells_of_face()
{
	for (auto it = cells.begin(); it != cells.end(); it++)
	{
		auto faces = faces_of_cell(&*it);
		for (auto it2 = faces.begin(); it2 != faces.end(); it2++)
		{
			if (cells_of_face.find(*it2) == cells_of_face.end())
				cells_of_face.insert(make_pair(*it2, set<Cell*>()));
			cells_of_face[*it2].insert(&*it);
		}
	}

	calculated_cells_of_face = true;
}

void Shape4D::calculate_cells_of_edge()
{
	if (!calculated_cells_of_face)
		calculate_cells_of_face();

	edges.clear();
	for (auto it = faces.begin(); it != faces.end(); it++)
	{
		vector<int> indices = (*it).get_indices_cycle();
		for (int i = 0; i < indices.size() - 1; i++)
		{
			Edge edge = Edge(indices[i], indices[i + 1]);
			auto found_edge = std::find(edges.begin(), edges.end(), edge);
			if (found_edge == edges.end())
			{
				edges.push_back(edge);
				auto cells_of_it = cells_of_face[&*it];
				cells_of_edge.insert(make_pair(&edge, cells_of_it));
			}
			else
			{
				auto cells_of_it = cells_of_face[&*it];
				cells_of_edge[&*found_edge].insert(cells_of_it.begin(), cells_of_it.end());
			}
		}
		Edge edge = Edge(indices[0], indices[indices.size() - 1]);
		auto found_edge = std::find(edges.begin(), edges.end(), edge);
		if (found_edge == edges.end())
		{
			edges.push_back(edge);
			auto cells_of_it = cells_of_face[&*it];
			cells_of_edge.insert(make_pair(&edge, cells_of_it));
		}
		else
		{
			auto cells_of_it = cells_of_face[&*it];
			cells_of_edge[&*found_edge].insert(cells_of_it.begin(), cells_of_it.end());
		}
	}

	calculated_edges = true;
	calculated_cells_of_edge = true;
}

void Shape4D::calculate_edges()
{
	for (auto it = faces.begin(); it != faces.end(); it++)
	{
		vector<int> indices = (*it).get_indices_cycle();
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

	calculated_edges = true;
}

void Shape4D::calculate_faces_of_edge()
{
	if (!calculated_edges)
		calculate_edges();

	for (auto it = faces.begin(); it != faces.end(); it++)
	{
		auto edges = edges_cycle_of_face(&*it);

		for (auto it2 = edges.begin(); it2 != edges.end(); it2++)
		{
			if (faces_of_edge.find(*it2) == faces_of_edge.end())
				faces_of_edge.insert(make_pair(*it2, set<Shape4D::Face*>()));
			faces_of_edge[*it2].insert(&*it);
		}
	}

	calculated_faces_of_edge = true;
}

set<Shape4D::Edge*> Shape4D::edges_with_vertex(Point4D* vertex)
{
	if (!calculated_edges_of_vertex)
		calculate_edges_of_vertex();

	return edges_of_vertex[vertex];
}

vector<Shape4D::Edge*> Shape4D::edges_cycle_of_face(Shape4D::Face* face)
{
	vector<Edge*> result;
	vector<int> indices = face->get_indices_cycle();
	for (int i = 0; i < indices.size() - 1; i++)
		result.push_back(&*std::find(edges.begin(), edges.end(), Edge(indices[i], indices[i + 1])));
	result.push_back(&*std::find(edges.begin(), edges.end(), Edge(indices[0], indices[indices.size() - 1])));
	return result;
}

void Shape4D::calculate_edges_of_vertex()
{
	if (!calculated_edges)
		calculate_edges();

	for (int i = 0; i < edges.size(); i++)
	{
		if (edges_of_vertex.find(&vertices[edges[i].first]) == edges_of_vertex.end())
			edges_of_vertex.insert(make_pair(&vertices[edges[i].first], set<Edge*>()));
		edges_of_vertex[&vertices[edges[i].first]].insert(&edges[i]);

		if (edges_of_vertex.find(&vertices[edges[i].second]) == edges_of_vertex.end())
			edges_of_vertex.insert(make_pair(&vertices[edges[i].second], set<Edge*>()));
		edges_of_vertex[&vertices[edges[i].second]].insert(&edges[i]);
	}

	calculated_edges_of_vertex = true;
}

bool Shape4D::is_closed()
{
	if (!calculated_adjacent_cells)
		calculate_cells_of_face();

	for (auto it = cells_of_face.begin(); it != cells_of_face.end(); it++)
		if (it->second.size() != 2)
			return false;
	return true;
}

bool Shape4D::has_valid_cells()
{
	for (auto it = cells.begin(); it != cells.end(); it++)
		if (faces.size() > 2)
		{
			set<Shape4D::Face*> cell_faces = faces_of_cell(&(*it));
			vector<Point4D*> vertices1 = vertices_cycle_of_face(*cell_faces.begin());
			vector<Point4D*> vertices2 = vertices_cycle_of_face(*std::next(cell_faces.begin(), 1));
			if (vertices1.size() < 3)
				return false;
			const Point4D* vertex1 = vertices1[0];
			const Point4D* vertex2 = vertices1[1];
			const Point4D* vertex3 = vertices1[2];
			int last_pos = 0;
			for (; std::find(vertices1.begin(), vertices1.end(), vertices2[last_pos]) != vertices1.end(); last_pos++)
				if (last_pos == vertices2.size() - 1)
					return false;
			const Point4D* vertex4 = vertices2[last_pos];

			Space4D cell_space = Space4D(vertex1, vertex2, vertex3, vertex4);
			for (auto it2 = cell_faces.begin(); it2 != cell_faces.end(); it2++)
				for (int i = 0; i < (*it2)->get_indices_cycle().size(); i++)
					if (GeometryMaster::distance(&cell_space, &vertices[(*it2)->get_indices_cycle()[i]]) > 0.1)
						return false;
		}

	return true;
}

/*bool Shape4D::has_valid_faces()
{
	for (auto it = faces.begin(); it != faces.end(); it++)
		if (faces.size() > 3)
		{
			vector<Point4D*> face_vertices = vertices_cycle_of_face(&*it);
			Plane4D face_plane = Plane4D(face_vertices[0], face_vertices[1], face_vertices[2]);
			for (int i = 3; i < face_vertices.size(); i++)
				if (MathEngine::distance(&face_plane, face_vertices[i]) > 0.1)
					//if (!face_plane.contains(face_vertices[i]))
					return false;
		}
	return true;
}*/