//#include "math_engine.h"
//#include "geometry_master.h"
//#include <algorithm>
//
//Shape3D MathEngine::intersect(Shape3D* shape, RealNumber rX, RealNumber rY, RealNumber d, Shape2D& intersection)
//{
//	Matrix3D transformation = rotation_transformation(rX, rY, d);
//
//	vector<Shape3D::Face*> faces = shape->get_faces();
//	Point3D shape_center = shape->calculate_center();
//	set<HalfPlane2D*> halfplanes;
//	map<Shape3D::Face*, HalfPlane2D*> faces_to_halfplanes;
//	map<HalfPlane2D*, Shape3D::Face*> halfplanes_to_faces;
//
//	for (auto it = faces.begin(); it != faces.end(); it++)
//	{
//		vector<Point3D*> face_vertices = shape->vertices_cycle_of_face((*it));
//		Plane3D face_plane = Plane3D(face_vertices[0], face_vertices[1], face_vertices[2]);
//		HalfSpace3D face_halfspace = HalfSpace3D(&face_plane, &shape_center);
//		HalfSpace3D view_halfspace = GeometryMaster::in_base(&face_halfspace, &transformation);
//
//		OperationResult2D res = flatten_z(&view_halfspace);
//		switch (res.result)
//		{
//		case OperationResult2D::EMPTYSET: //intersections of halfspaces and plane is empty
//			intersection = Shape2D::empty();
//			return Shape3D::empty();
//		case OperationResult2D::PLANE: //halfspace and plane simplify to plane
//			break;
//		case OperationResult2D::HALFPLANE: //important contribution to result shape
//			halfplanes.insert(res.halfplane);
//			faces_to_halfplanes.insert(make_pair(*it, res.halfplane));
//			halfplanes_to_faces.insert(make_pair(res.halfplane, *it));
//		}
//	}
//
//	intersection = intersect(halfplanes, shape->get_adjacent_faces(), faces_to_halfplanes, halfplanes_to_faces);
//
//	for (auto it = halfplanes.begin(); it != halfplanes.end(); it++)
//		delete *it;
//	Shape3D result = Shape3D(&intersection);
//	result = GeometryMaster::transform(&result, &transformation);
//
//	return result;
//}
//
//Shape4D MathEngine::intersect(Shape4D* shape, RealNumber rXZ, RealNumber rYZ, RealNumber rXY, RealNumber d, Shape3D& intersection)
//{
//	Matrix4D transformation = rotation_transformation(rXZ, rYZ, rXY, d);
//
//	vector<Shape4D::Cell*> cells = shape->get_cells();
//	Point4D shape_center = shape->calculate_center();
//	set<HalfSpace3D*> halfspaces;
//	map<Shape4D::Cell*, HalfSpace3D*> cells_to_halfspaces;
//	map<HalfSpace3D*, Shape4D::Cell*> halfspaces_to_cells;
//
//	for (auto it = cells.begin(); it != cells.end(); it++)
//	{
//		set<Shape4D::Face*> cell_faces = shape->faces_of_cell(*it);
//		vector<Point4D*> vertices1 = shape->vertices_cycle_of_face(*cell_faces.begin());
//		vector<Point4D*> vertices2 = shape->vertices_cycle_of_face(*std::next(cell_faces.begin(), 1));
//		if (vertices1.size() < 3)
//			return Shape4D::empty();
//		const Point4D* vertex1 = vertices1[0];
//		const Point4D* vertex2 = vertices1[1];
//		const Point4D* vertex3 = vertices1[2];
//		int last_pos = 0;
//		for (; std::find(vertices1.begin(), vertices1.end(), vertices2[last_pos]) != vertices1.end(); last_pos++)
//			if (last_pos == vertices2.size() - 1)
//			{
//				intersection = Shape3D::empty();
//				return Shape4D::empty();
//			}
//		const Point4D* vertex4 = vertices2[last_pos];
//
//		Space4D cell_space = Space4D(vertex1, vertex2, vertex3, vertex4);
//		HalfHyperspace4D cell_halfspace = HalfHyperspace4D(&cell_space, &shape_center);
//		HalfHyperspace4D view_halfspace = GeometryMaster::in_base(&cell_halfspace, &transformation);
//
//		OperationResult3D res = flatten_w(&view_halfspace);
//		switch (res.result)
//		{
//		case OperationResult3D::EMPTYSET:
//			intersection = Shape3D::empty();
//			return Shape4D::empty();
//		case OperationResult3D::SPACE:
//			break;
//		case OperationResult3D::HALFSPACE:
//			halfspaces.insert(res.halfspace);
//			cells_to_halfspaces.insert(make_pair(*it, res.halfspace));
//			halfspaces_to_cells.insert(make_pair(res.halfspace, *it));
//		}
//	}
//
//	intersection = intersect(halfspaces, shape->get_adjacent_cells(), cells_to_halfspaces, halfspaces_to_cells);
//	intersection.recalculate_normals();
//	for (auto it = halfspaces.begin(); it != halfspaces.end(); it++)
//		delete *it;
//
//	Shape4D result = Shape4D(&intersection);
//	result = GeometryMaster::transform(&result, &transformation);
//
//	return result;
//}
//
//Shape2D MathEngine::intersect(set<HalfPlane2D*> halfplanes, map<Shape3D::Face*, set<Shape3D::Face*>> adjacent_faces,
//	map<Shape3D::Face*, HalfPlane2D*> faces_to_halfplanes, map<HalfPlane2D*, Shape3D::Face*> halfplanes_to_faces)
//{
//	struct PointWithLines {
//		Point2D point;
//		set<const HalfPlane2D*> lines;
//		bool visited = false;
//	};
//
//	set<PointWithLines*> points;
//	map<const HalfPlane2D*, set<PointWithLines*>> points_of_halfplane;
//
//	for (auto it = halfplanes.begin(); it != halfplanes.end(); it++)
//	{
//		set<Shape3D::Face*> adjacent = adjacent_faces[halfplanes_to_faces[*it]];
//		for (auto it2 = adjacent.begin(); it2 != adjacent.end(); it2++)
//		{
//			HalfPlane2D* it2_as_halfplane = faces_to_halfplanes[*it2];
//			if (it2_as_halfplane != NULL)
//			{
//				OperationResult2D res = (*it)->get_boundary_line().intersect(&it2_as_halfplane->get_boundary_line());
//				if (res.result == OperationResult2D::POINT)
//				{
//					HalfPlane2D* it3_as_halfplane;
//					bool satisfies_all = true;
//					for (auto it3 = adjacent.begin(); it3 != adjacent.end() && satisfies_all; it3++)
//						if ((*it3) != (*it2) && (it3_as_halfplane = faces_to_halfplanes[*it3]) &&
//							it3_as_halfplane != (*it) && !it3_as_halfplane->contains(res.point))
//						{
//							satisfies_all = false;
//							delete res.point;
//						}
//
//					if (satisfies_all)
//					{
//						PointWithLines* point = NULL;
//						for (auto it3 = points.begin(); it3 != points.end() && point == NULL; it3++)
//							//if ((*it3)->point == *res.point)
//							if ((*it3)->point.distance(*res.point) < 0.001)
//								point = (*it3);
//
//						if (point == NULL)
//						{
//							point = new PointWithLines();
//							point->point = *res.point;
//							points.insert(point);
//						}
//						point->lines.insert(*it);
//						point->lines.insert(it2_as_halfplane);
//						if (points_of_halfplane.find(*it) == points_of_halfplane.end())
//							points_of_halfplane.insert(make_pair(*it, set<PointWithLines*>()));
//						points_of_halfplane[*it].insert(point);
//						if (points_of_halfplane.find(it2_as_halfplane) == points_of_halfplane.end())
//							points_of_halfplane.insert(make_pair(it2_as_halfplane, set<PointWithLines*>()));
//						points_of_halfplane[it2_as_halfplane].insert(point);
//					}
//				}
//			}
//		}
//	}
//
//	vector<Point2D> result_points;
//	if (points.size() > 0)
//	{
//		PointWithLines* starting_point = *points.begin();
//		PointWithLines* current_point = starting_point;
//		do
//		{
//			result_points.push_back(current_point->point);
//			current_point->visited = true;
//
//			/*for (auto it = current_point->lines.begin(); current_point->visited && it != current_point->lines.end(); it++)
//			for (auto it2 = points_of_halfplane[*it].begin(); current_point->visited && it2 != points_of_halfplane[*it].end(); it2++)
//			if (!(*it2)->visited)
//			current_point = *it2;*/
//
//			vector<PointWithLines*> neighbors;
//			for (auto it = current_point->lines.begin(); it != current_point->lines.end(); it++)
//				for (auto it2 = points_of_halfplane[*it].begin(); it2 != points_of_halfplane[*it].end(); it2++)
//					if (!(*it2)->visited)
//						neighbors.push_back(*it2);
//
//			if (neighbors.size() > 0)
//				current_point = *std::min_element(neighbors.begin(), neighbors.end(), [current_point](PointWithLines* a, PointWithLines* b) {
//				return a->point.distance(current_point->point) < b->point.distance(current_point->point);
//			});
//
//		} while (!current_point->visited);
//	}
//
//	for (auto it = points.begin(); it != points.end(); it++)
//		delete *it;
//
//	return Shape2D(result_points);
//}
//
//Shape3D MathEngine::intersect(set<HalfSpace3D*> halfspaces, map<Shape4D::Cell*, set<Shape4D::Cell*>> adjacent_cells,
//	map<Shape4D::Cell*, HalfSpace3D*> cells_to_halfspaces, map<HalfSpace3D*, Shape4D::Cell*> halfspaces_to_cells)
//{
//	struct PointWithPlanes {
//		Point3D point;
//		int index;
//		set<HalfSpace3D*> halfspaces;
//		bool visited;
//	};
//	vector<PointWithPlanes*> points;
//	map<const HalfSpace3D*, set<PointWithPlanes*>> points_of_halfspace;
//
//	for (auto it = halfspaces.begin(); it != halfspaces.end(); it++)
//	{
//		set<Shape4D::Cell*> adjacent = adjacent_cells[halfspaces_to_cells[*it]];
//		for (auto it2 = adjacent.begin(); it2 != adjacent.end(); it2++)
//		{
//			HalfSpace3D* it2_as_halfspace = cells_to_halfspaces[*it2];
//			if (it2_as_halfspace != NULL)
//				for (auto it3 = std::next(it2, 1); it3 != adjacent.end(); it3++)
//				{
//					HalfSpace3D* it3_as_halfspace = cells_to_halfspaces[*it3];
//					if (it3_as_halfspace != NULL)
//					{
//						OperationResult3D res = GeometryMaster::intersect((*it)->get_boundary_plane(), it2_as_halfspace->get_boundary_plane(), it3_as_halfspace->get_boundary_plane());
//						if (res.result == OperationResult2D::POINT)
//						{
//							HalfSpace3D* it4_as_halfspace;
//							bool satisfies_all = true;
//							for (auto it4 = adjacent.begin(); it4 != adjacent.end() && satisfies_all; it4++)
//								if ((*it4) != (*it3) && (*it4) != (*it2) && (it4_as_halfspace = cells_to_halfspaces[*it4]) &&
//									it4_as_halfspace != (*it) && !(it4_as_halfspace->contains(res.point)))
//								{
//									satisfies_all = false;
//									delete res.point;
//								}
//
//							if (satisfies_all)
//							{
//								PointWithPlanes* point = NULL;
//								for (auto it4 = points.begin(); it4 != points.end() && point == NULL; it4++)
//									//if (*(*it4)->point == *res.point)
//									if ((*it4)->point.distance(*res.point) < 0.001)
//										point = (*it4);
//
//								if (point == NULL)
//								{
//									point = new PointWithPlanes();
//									point->point = *res.point;
//									point->index = points.size();
//									point->visited = false;
//									points.push_back(point);
//								}
//								point->halfspaces.insert(*it);
//								point->halfspaces.insert(it2_as_halfspace);
//								point->halfspaces.insert(it3_as_halfspace);
//								if (points_of_halfspace.find(*it) == points_of_halfspace.end())
//									points_of_halfspace.insert(make_pair(*it, set<PointWithPlanes*>()));
//								points_of_halfspace[*it].insert(point);
//								if (points_of_halfspace.find(it2_as_halfspace) == points_of_halfspace.end())
//									points_of_halfspace.insert(make_pair(it2_as_halfspace, set<PointWithPlanes*>()));
//								points_of_halfspace[it2_as_halfspace].insert(point);
//								if (points_of_halfspace.find(it3_as_halfspace) == points_of_halfspace.end())
//									points_of_halfspace.insert(make_pair(it3_as_halfspace, set<PointWithPlanes*>()));
//								points_of_halfspace[it3_as_halfspace].insert(point);
//							}
//						}
//					}
//				}
//		}
//	}
//
//	vector<Point3D> result_points;
//	for (int i = 0; i < points.size(); i++)
//		result_points.push_back(points[i]->point);
//
//	vector<Shape3D::Face> result_faces;
//	for (auto it = halfspaces.begin(); it != halfspaces.end(); it++)
//	{
//		vector<int> face_points;
//
//		for (auto it2 = points.begin(); it2 != points.end(); it2++)
//			(*it2)->visited = true;
//		auto points_of_it = points_of_halfspace[*it];
//		for (auto it2 = points_of_it.begin(); it2 != points_of_it.end(); it2++)
//			(*it2)->visited = false;
//
//		if (points_of_it.size() > 2)
//		{
//			PointWithPlanes* starting_point = *points_of_it.begin();
//			PointWithPlanes* current_point = starting_point;
//			do
//			{
//				face_points.push_back(current_point->index);
//				current_point->visited = true;
//
//				set<PointWithPlanes*> neighbors;
//				for (auto it2 = current_point->halfspaces.begin(); it2 != current_point->halfspaces.end(); it2++)
//					if (*it2 != *it)
//						for (auto it3 = points_of_halfspace[*it2].begin(); it3 != points_of_halfspace[*it2].end(); it3++)
//							if (!(*it3)->visited)
//								neighbors.insert((*it3));
//
//				if (neighbors.size() > 0)
//					current_point = *std::min_element(neighbors.begin(), neighbors.end(), [current_point](PointWithPlanes* a, PointWithPlanes* b) {
//					return a->point.distance(current_point->point) < b->point.distance(current_point->point);
//				});
//
//			} while (!current_point->visited);
//
//			if (face_points.size() > 2)
//				result_faces.push_back(Shape3D::Face(face_points));
//		}
//	}
//
//	for (auto it = points.begin(); it != points.end(); it++)
//		delete *it;
//
//	return Shape3D(result_points, result_faces);
//}
//
////Shape3D MathEngine::intersect(set<HalfSpace3D*> halfspaces)
////{
////	struct PointWithPlanes;
////	struct PlaneWithPoints {
////		Plane3D* plane;
////		set<PointWithPlanes*> points;
////	};
////
////	struct PointWithPlanes {
////		Point3D* point;
////		int index;
////		set<PlaneWithPoints*> planes;
////		bool visited;
////	};
////
////	for (auto it = halfspaces.begin(); it != halfspaces.end(); it++)
////		for (auto it2 = std::next(it, 1); it2 != halfspaces.end();)
////			if (*(*it2)->get_boundary_plane() == *(*it)->get_boundary_plane())
////			{
////				if (**it2 == **it)
////					it2 = halfspaces.erase(it2);
////				else
////					return Shape3D::empty();
////			}
////			else
////				it2++;
////
////	set<PlaneWithPoints*> planes;
////	for (auto it = halfspaces.begin(); it != halfspaces.end(); it++)
////	{
////		PlaneWithPoints* plane = new PlaneWithPoints();
////		plane->plane = new Plane3D(*(*it)->get_boundary_plane());
////		planes.insert(plane);
////	}
////	vector<PointWithPlanes*> points;
////	int points_number = 0;
////
////	for (auto it = planes.begin(); it != planes.end(); it++)
////		for (auto it2 = std::next(it, 1); it2 != planes.end(); it2++)
////			for (auto it3 = std::next(it2, 1); it3 != planes.end(); it3++)
////			{
////				OperationResult3D res = intersect((*it)->plane, (*it2)->plane, (*it3)->plane);
////				if (res.result == OperationResult2D::POINT)
////				{
////					bool satisfies_all = true;
////					for (auto it4 = halfspaces.begin(); it4 != halfspaces.end(); it4++)
////						if (*(*it4)->get_boundary_plane() != *(*it3)->plane &&
////							*(*it4)->get_boundary_plane() != *(*it2)->plane && 
////							*(*it4)->get_boundary_plane() != *(*it)->plane &&
////							!(*it4)->contains(res.point))
////						{
////							satisfies_all = false;
////							delete res.point;
////							break;
////						}
////					if (satisfies_all)
////					{
////						PointWithPlanes* point;
////						bool exists = false;
////						for (auto it4 = points.begin(); it4 != points.end() && !exists; it4++)
////							if (*(*it4)->point == *res.point)
////							{
////								point = (*it4);
////								exists = true;
////							}
////						if (!exists)
////						{
////							point = new PointWithPlanes();
////							point->point = res.point;
////							point->index = points_number;
////							point->visited = false;
////							points_number++;
////							points.push_back(point);
////						}
////						point->planes.insert(*it);
////						point->planes.insert(*it2);
////						point->planes.insert(*it3);
////						(*it)->points.insert(point);
////						(*it2)->points.insert(point);
////						(*it3)->points.insert(point);
////					}
////				}
////			}
////
////	vector<Point3D> result_points;
////	for (int i = 0; i < points.size(); i++)
////		result_points.push_back(*points[i]->point);
////
////	vector<Shape3D::Face> result_faces;
////	for (auto it = planes.begin(); it != planes.end(); it++)
////	{
////		vector<int> face_points;
////
////		for (auto it2 = points.begin(); it2 != points.end(); it2++)
////			(*it2)->visited = true;
////		for (auto it2 = (*it)->points.begin(); it2 != (*it)->points.end(); it2++)
////			(*it2)->visited = false;
////
////		if ((*it)->points.size() > 2)
////		{
////			PointWithPlanes* starting_point = *(*it)->points.begin();
////			PointWithPlanes* current_point = starting_point;
////			do
////			{
////				face_points.push_back(current_point->index);
////				current_point->visited = true;
////
////				set<PointWithPlanes*> neighbors;
////				for (auto it2 = current_point->planes.begin(); it2 != current_point->planes.end(); it2++)
////					if (*it2 != *it)
////						for (auto it3 = (*it2)->points.begin(); it3 != (*it2)->points.end(); it3++)
////							if (!(*it3)->visited)
////								neighbors.insert((*it3));
////
////				if (neighbors.size() > 0)
////					current_point = *neighbors.begin();
////
////				//if (current_point->visited && neighbors.find(starting_point) == neighbors.end())
////				//	face_points.clear();
////			} while (!current_point->visited);
////
////			if (face_points.size() > 2)
////				result_faces.push_back(Shape3D::Face(face_points));
////		}
////	}
////
////	for (auto it = planes.begin(); it != planes.end(); it++)
////	{
////		delete (*it)->plane;
////		delete *it;
////	}
////	for (auto it = points.begin(); it != points.end(); it++)
////	{
////		delete (*it)->point;
////		delete *it;
////	}
////
////	return Shape3D(result_points, result_faces);
////}