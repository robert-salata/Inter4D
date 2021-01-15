#ifndef Shape_3d_class
#define Shape_3d_class

#include "point3D.h"
#include "shape2D.h"
#include <string>
#include <set>
#include <vector>
#include <map>
using namespace std;

class Shape3D
{
public:
	class Edge
	{
	public:
		Edge(int index1, int index2);
		bool operator == (Edge edge2);
		bool operator < (const Edge edge2) const; //optimalization purposes only

		int first;
		int second;
	};

	class Face
	{
	public:
		Face(const vector<int> vertices);
		vector<int> get_indices_cycle() const;
		void revert_cycle();

		friend class Shape3D;
	private:
		vector<int> vertices_cycle;

		//calculated
		bool calculated_view_vertices = false;
		vector<Point3D*> view_vertices;
	};

	Shape3D();
	Shape3D(const vector<Point3D> vertices, const vector<Face> faces);
	Shape3D(Shape2D* flat_shape);

	static Shape3D empty();
	vector<Point3D*> get_vertices();
	vector<Face*> get_faces();
	vector<Edge*> get_edges();
	vector<Point3D*> vertices_cycle_of_face(Face* face);
	vector<Edge*> edges_cycle_of_face(Face* face);
	Point3D calculate_center();
	void recalculate_normals();
	map<Face*, set<Face*>> get_adjacent_faces();
	set<Face*> faces_with_edge(Edge* edge);
	Shape2D flat_face(Face* face);
	bool is_closed();
	bool has_valid_faces();
	bool is_convex();
	bool has_concave_faces();

private:
	vector<Point3D> vertices;
	vector<Face> faces;

	//calculated
	void calculate_faces_of_edge();
	void calculate_edges();

	bool calculated_adjacent_faces = false;
	bool calculated_faces_of_edge = false;
	bool calculated_edges = false;
	bool calculated_convex = false;

	map<Face*, set<Face*>> adjacent_faces;
	map<Edge*, set<Face*>> faces_of_edge;
	vector<Edge> edges;
	bool is_convex_value;
};

#endif
