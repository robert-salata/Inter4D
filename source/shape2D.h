#ifndef Shape_2d_class
#define Shape_2d_class

#include "point2D.h"
#include <string>
#include <set>
#include <vector>
#include <fstream>
#include <sstream>
using namespace std;

class Shape2D
{
public:
	Shape2D();
	Shape2D(const vector<Point2D> vertices);

	static Shape2D empty();
	const vector<Point2D> get_vertices_cycle();
	void change_orientation();
	Point2D calculate_center();
	bool contains(Point2D* point);
	bool is_clockwise_oriented();
	bool is_convex();

private:
	vector<Point2D> vertices_cycle;
};

#endif
