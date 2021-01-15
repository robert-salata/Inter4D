#ifndef Matrix_3d_class
#define Matrix_3d_class

#include "vector3D.h"

class Matrix3D
{
public:
	Matrix3D();
	Matrix3D(Vector3D x, Vector3D y, Vector3D z);	
	~Matrix3D();
	Matrix3D operator * (Matrix3D m);
	Point3D operator * (Point3D v);
	static Matrix3D translation(RealNumber x, RealNumber y, RealNumber z);
	static Matrix3D translation(Vector3D vector);
	static Matrix3D rotation_x(RealNumber rotation); //+z going to +y
	static Matrix3D rotation_y(RealNumber rotation); //+x going to +z
	static Matrix3D rotation_z(RealNumber rotation); //+x going to +y
	static Matrix3D scale(RealNumber scale);
	static Matrix3D scale(RealNumber scale_x, RealNumber scale_y, RealNumber scale_z);
	Matrix3D inverse();
	RealNumber m[4][4];

private:
	bool calculated_inverse = false;
	Matrix3D* matrix_inverse;
};

#endif