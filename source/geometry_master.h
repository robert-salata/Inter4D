#ifndef Geometry_master
#define Geometry_master

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
#include "segment4D.h"
#include "segment3D.h"

class GeometryMaster
{
public:
	static OperationResult3D intersect(HalfSpace3D* halfspace, Plane3D* plane);
	static OperationResult3D intersect(Line3D* line, Plane3D* plane);
	static OperationResult3D intersect(Plane3D* plane1, Plane3D* plane2, Plane3D* plane3);
	static OperationResult4D intersect(Line4D* line, Space4D* space);
	static OperationResult3D intersect(Line3D* line1, Line3D* line2);
	static HalfSpace3D in_base(HalfSpace3D* halfspace, Matrix3D* base);
	static HalfHyperspace4D in_base(HalfHyperspace4D* halfspace, Matrix4D* base);
	static Shape3D transform(Shape3D* shape, Matrix3D* transformation);
	static Shape4D transform(Shape4D* shape, Matrix4D* transformation);
	static Plane3D transform(Plane3D* plane, Matrix3D* transformation);
	static Space4D transform(Space4D* space, Matrix4D* transformation);
	static Segment3D transform(Segment3D* segment, Matrix3D* transformation);
	static Segment4D transform(Segment4D* segment, Matrix4D* transformation);
	static RealNumber distance(Plane3D* plane, Point3D* point);
	static RealNumber distance(Space4D* space, Point4D* point);

	static const RealNumber PI;
};

#endif