#ifndef Half_hyperspace_3d_class
#define Half_hyperspace_3d_class

#include "space4D.h"

class HalfHyperspace4D
{
public:
	HalfHyperspace4D(const HalfHyperspace4D& halfspace);
	HalfHyperspace4D(Space4D* space, Point4D* point, bool closed = true); //halfspace defined by space, containing point
	Space4D* get_boundary_space();
	bool contains(Point4D* point);
	Point4D representant();

private:
	Space4D boundary;
	bool less; //less: y < ax + bz + cw + d (and other plane-equations similarly)
	bool closed; //closed: <=   !closed: <
};

#endif