#pragma once
#ifndef Geometry_base
#define Geometry_base
#define NULL 0

class Point4D;
class Line4D;

class OperationResult4D
{
public:
	enum ResultType { EMPTYSET, POINT, LINE };

	OperationResult4D()
	{
		result = EMPTYSET;
		point = NULL;
		line = NULL;
	}

	OperationResult4D(ResultType res) : OperationResult4D()
	{
		result = res;
	}

	OperationResult4D(ResultType res, void* value) : OperationResult4D(res)
	{
		switch (res)
		{
		case POINT:
			point = (Point4D*)value; break;
		case LINE:
			line = (Line4D*)value; break;
		}
	}

	ResultType result;
	Point4D* point;
	Line4D* line;
};

class HalfPlane3D;
class Plane3D;
class Line3D;
class Point3D;
class HalfSpace3D;

class OperationResult3D
{
public:
	enum ResultType { EMPTYSET, HALFPLANE, PLANE, LINE, POINT, SPACE, HALFSPACE };

	OperationResult3D()
	{
		result = EMPTYSET;
		halfplane = NULL;
		plane = NULL;
		line = NULL;
		point = NULL;
		halfspace = NULL;
	}

	OperationResult3D(ResultType res) : OperationResult3D()
	{
		result = res;
	}

	OperationResult3D(ResultType res, void* value) : OperationResult3D(res)
	{
		switch (res)
		{
		case HALFPLANE:
			halfplane = (HalfPlane3D*)value; break;
		case PLANE:
			plane = (Plane3D*)value; break;
		case LINE:
			line = (Line3D*)value; break;
		case POINT:
			point = (Point3D*)value; break;
		case HALFSPACE:
			halfspace = (HalfSpace3D*)value; break;
		}
	}

	ResultType result;

	HalfPlane3D* halfplane;
	Plane3D* plane;
	Line3D* line;
	Point3D* point;
	HalfSpace3D* halfspace;
};

class HalfPlane2D;
class Line2D;
class Point2D;

class OperationResult2D
{
public:
	enum ResultType { EMPTYSET, HALFPLANE, PLANE, LINE, POINT };

	OperationResult2D()
	{
		result = EMPTYSET;
		halfplane = NULL;
		line = NULL;
		point = NULL;
	}

	OperationResult2D(ResultType res) : OperationResult2D()
	{
		result = res;
	}

	OperationResult2D(ResultType res, void* value) : OperationResult2D(res)
	{
		switch (res)
		{
		case HALFPLANE:
			halfplane = (HalfPlane2D*)value; break;
		case LINE:
			line = (Line2D*)value; break;
		case POINT:
			point = (Point2D*)value; break;
		}
	}

	ResultType result;

	HalfPlane2D* halfplane;
	Line2D* line;
	Point2D* point;
};

#endif
