#ifndef Math_engine
#define Math_engine

#include "shape2D.h"
#include "shape3D.h"
#include "shape4D.h"
#include "plane3D.h"
#include "space4D.h"
#include "half_plane2D.h"
#include "half_plane3D.h"
#include "half_space3D.h"
#include "half_hyperspace4D.h"
#include "matrix3D.h"
#include "matrix4D.h"
#include "line4D.h"
#include "segment3D.h"
#include "segment4D.h"

class MathEngine
{
public:
	static Shape3D intersect(Shape3D* shape, RealNumber rX, RealNumber rY, RealNumber d, Shape2D& intersection);
	static Shape4D intersect(Shape4D* shape, RealNumber rXZ, RealNumber rYZ, RealNumber rXY, RealNumber d, Shape3D& intersection);
	
	static vector<Shape3D> intersect_concave(Shape3D* shape, RealNumber rX, RealNumber rY, RealNumber d, vector<Shape2D>& intersections);
	static Shape4D intersect_concave(Shape4D* shape, RealNumber rXZ, RealNumber rYZ, RealNumber rXY, RealNumber d, Shape3D& intersections);

	static void get_rotation_params(Plane3D* plane, RealNumber& rX, RealNumber& rY, RealNumber& d);
	static void get_rotation_params(Space4D* space, RealNumber& rXZ, RealNumber& rYZ, RealNumber &rXY, RealNumber& d);
	
	static Matrix3D rotation_transformation(RealNumber rX, RealNumber rY, RealNumber d);
	static Matrix4D rotation_transformation(RealNumber rX, RealNumber rY, RealNumber rZ, RealNumber d);

	static Shape3D project(Shape4D* shape, RealNumber rXZ, RealNumber rYZ, RealNumber rXY, RealNumber d);
	static Segment3D project(Segment4D* shape, RealNumber rXZ, RealNumber rYZ, RealNumber rXY, RealNumber d);

	static vector<Shape2D> triangulate(Shape2D* polygon);

private:
	static Shape2D intersect(set<HalfPlane2D*> halfplanes, map<Shape3D::Face*, set<Shape3D::Face*>> adjacent_halfplanes,
		map<Shape3D::Face*, HalfPlane2D*> faces_to_halfplanes, map<HalfPlane2D*, Shape3D::Face*> halfplanes_to_faces);
	//static Shape2D intersect(set<HalfPlane2D*> halfplanes, map<const HalfPlane2D*, set<const HalfPlane2D*>> adjacent_halfplanes);
	static Shape3D intersect(set<HalfSpace3D*> halfspaces, map<Shape4D::Cell*, set<Shape4D::Cell*>> adjacent_cells,
		map<Shape4D::Cell*, HalfSpace3D*> cells_to_halfspaces, map<HalfSpace3D*, Shape4D::Cell*> halfspaces_to_cells);
	//static Shape3D intersect(set<HalfSpace3D*> halfspaces);

	static bool are_concave(Shape3D* shape, Shape3D::Edge* edge1, Shape3D::Edge* edge2);
	static OperationResult2D flatten_z(HalfSpace3D* halfspace);
	static OperationResult3D flatten_w(HalfHyperspace4D* halfspace);
	static Shape2D flatten_z(Shape3D* shape);
	static Shape3D flatten_w(Shape4D* shape);
};

#endif