#include "shape2D.h"
#include "line2D.h"

Shape2D::Shape2D()
{

}

Shape2D::Shape2D(const vector<Point2D> vertices)
{
	for (auto it = vertices.begin(); it != vertices.end(); it++)
		this->vertices_cycle.push_back(Point2D(*it));
}

Shape2D Shape2D::empty()
{
	return Shape2D();
}

const vector<Point2D> Shape2D::get_vertices_cycle()
{
	return this->vertices_cycle;
}

Point2D Shape2D::calculate_center()
{
	Point2D result = Point2D(RealNumber(0), RealNumber(0));
	for (auto it = vertices_cycle.begin(); it != vertices_cycle.end(); it++)
		result += *it;
	result /= vertices_cycle.size();
	return result;
}

bool Shape2D::contains(Point2D* point)
{
	int number_of_intersections = 0;

	vector<Point2D> vertices = vertices_cycle;
	for (int i = 0; i < vertices.size(); i++)
	{
		vertices[i].x -= point->x;
		if (vertices[i].x == 0)
			vertices[i].x = 0.00001;
	}
	Line2D line = Line2D(-1, 0, 0);

	for (int i = 0; i < vertices.size(); i++)
	{
		auto v1 = vertices[i];
		auto v2 = vertices[(i + 1) % vertices.size()];
		if (v1.x*v2.x < 0)
		{
			OperationResult2D cross = line.intersect(&Line2D(&v1, &v2));
			if (cross.point->y < point->y)
				number_of_intersections++;
			delete cross.point;
		}
	}

	return number_of_intersections % 2 == 1;
}

bool Shape2D::is_clockwise_oriented()
{
	RealNumber sum;
	for (int i = 0; i < vertices_cycle.size(); i++)
	{
		Point2D v1 = vertices_cycle[i];
		Point2D v2 = vertices_cycle[(i + 1) % vertices_cycle.size()];
		sum += (v2.x - v1.x)*(v2.y + v1.y);
	}
	return sum > 0;
}

void Shape2D::change_orientation()
{
	vector<Point2D> vertices_original = vertices_cycle;
	for (int i = 0; i < vertices_original.size(); i++)
		vertices_cycle[i] = vertices_original[vertices_original.size() - 1 - i];
}

bool Shape2D::is_convex()
{
	if (is_clockwise_oriented())
		change_orientation();

	int n = vertices_cycle.size();
	for (int i = 0; i < vertices_cycle.size(); i++)
	{
		Point2D neighbor_1 = vertices_cycle[(i - 1 + n) % n];
		Point2D neighbor_2 = vertices_cycle[(i + 1) % n];
		Vector2D v1 = vertices_cycle[i] - neighbor_1;
		Vector2D v2 = neighbor_2 - vertices_cycle[i];
		if (v1.x*v2.y - v1.y*v2.x <= 0)
			return false;
	}
	return true;
}