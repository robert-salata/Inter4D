#ifndef Half_plane_2d_class
#define Half_plane_2d_class

#include "line2D.h"
#include "point2D.h"

class HalfPlane2D
{
public:
	HalfPlane2D(const HalfPlane2D& halfspace);
	HalfPlane2D(Line2D* line, Point2D* point, bool closed = true); //halfspace defined by plane, containing point
	~HalfPlane2D();
	bool operator == (HalfPlane2D halfplane) const;
	Line2D get_boundary_line() const;
	bool contains(Point2D* point) const;
	Point2D representant() const;

private:
	Line2D* edge;
	bool less; //less: y < ax + b (and other plane-equations similarly)
	bool closed; //closed: <=   !closed: <
};

#endif