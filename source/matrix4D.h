#ifndef Matrix_4d_class
#define Matrix_4d_class

#include "vector4D.h"

class Matrix4D
{
public:
	Matrix4D();
	Matrix4D(Vector4D x, Vector4D y, Vector4D z, Vector4D w);
	Matrix4D operator * (Matrix4D m);
	Point4D operator * (Point4D v);
	static Matrix4D translation(RealNumber x, RealNumber y, RealNumber z, RealNumber w);
	static Matrix4D translation(Vector4D vector);
	static Matrix4D rotation_zw(RealNumber rotation); //+x going to +y
	static Matrix4D rotation_yw(RealNumber rotation); //+x going to +z
	static Matrix4D rotation_xw(RealNumber rotation); //+z going to +y
	static Matrix4D rotation_xy(RealNumber rotation); //+w going to +z
	static Matrix4D rotation_xz(RealNumber rotation); //+w going to +y
	static Matrix4D rotation_yz(RealNumber rotation); //+w going to +x
	static Matrix4D scale(RealNumber scale);
	static Matrix4D scale(RealNumber scale_x, RealNumber scale_y, RealNumber scale_z, RealNumber scale_w);
	Matrix4D inverse();
	RealNumber m[5][5];
};

#endif