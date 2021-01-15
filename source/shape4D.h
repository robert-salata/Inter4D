#ifndef Shape_4d_class
#define Shape_4d_class

#include "point4D.h"
#include "shape3D.h"
#include <string>
#include <set>
#include <vector>
#include <map>
using namespace std;

class Shape4D
{
public:
	class Edge
	{
	public:
		Edge(int index1, int index2);
		bool operator == (Edge edge2);
		bool operator != (Edge edge2);
		bool operator < (const Edge edge2) const; //optimalization purposes only

		int first;
		int second;
	};


	class Face
	{
	public:
		Face(const vector<int> vertices);
		Face(const vector<Edge> edges);
		bool operator == (Face face);

		vector<int> get_indices_cycle(); //returns all unique vertices in face

	private:
		vector<int> vertices_cycle;
	};

	class Cell
	{
	public:
		Cell(const set<int> faces);
		set<int> get_faces(); //returns all unique faces in cell

	private:
		set<int> faces;
	};

	Shape4D();
	Shape4D(const vector<Point4D> vertices, const vector<Face> faces, const vector<Cell> cells);
	Shape4D(Shape3D* shape_3d);

	static Shape4D empty();
	vector<Point4D*> get_vertices();
	vector<Edge*> get_edges();
	vector<Face*> get_faces();
	vector<Cell*> get_cells();
	vector<Point4D*> vertices_cycle_of_face(Face* face);
	set<Point4D*> vertices_of_cell(Cell* cell);
	set<Edge*> edges_with_vertex(Point4D* vertex);
	vector<Edge*> edges_cycle_of_face(Face* face);
	set<Face*> faces_of_cell(Cell* cell);
	set<Face*> faces_with_edge(Edge* edge);
	set<Cell*> cells_with_face(Face* face);
	set<Cell*> cells_with_edge(Edge* edge);
	map<Cell*, set<Cell*>> get_adjacent_cells();
	Point4D calculate_center();
	bool is_closed();
	bool has_valid_cells();
	bool has_valid_faces();

private:
	vector<Point4D> vertices;
	vector<Face> faces;
	vector<Cell> cells;

	//calculated
	void calculate_edges();
	void calculate_cells_of_face();
	void calculate_cells_of_edge();
	void calculate_edges_of_vertex();
	void calculate_faces_of_edge();

	bool calculated_edges = false;
	bool calculated_adjacent_cells = false;
	bool calculated_cells_of_face = false;
	bool calculated_cells_of_edge = false;
	bool calculated_edges_of_vertex = false;
	bool calculated_faces_of_edge = false;

	vector<Edge> edges;
	map<Cell*, set<Cell*>> adjacent_cells;
	map<Face*, set<Cell*>>  cells_of_face;
	map<Edge*, set<Cell*>> cells_of_edge;
	map<Point4D*, set<Edge*>> edges_of_vertex;
	map<Edge*, set<Face*>> faces_of_edge;
};

#endif
