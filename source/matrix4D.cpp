#include "matrix4D.h"
#include "algebra_master.h"

Matrix4D::Matrix4D()
{
	for (int i = 0; i < 5; i++)
		for (int j = 0; j < 5; j++)
			m[i][j] = i == j ? 1 : 0;
}

Matrix4D::Matrix4D(Vector4D x, Vector4D y, Vector4D z, Vector4D w)
{
	m[0][0] = x.x;
	m[0][1] = x.y;
	m[0][2] = x.z;
	m[0][3] = x.w;
	m[0][4] = 0;
	m[1][0] = y.x;
	m[1][1] = y.y;
	m[1][2] = y.z;
	m[1][3] = y.w;
	m[1][4] = 0;
	m[2][0] = z.x;
	m[2][1] = z.y;
	m[2][2] = z.z;
	m[2][3] = z.w;
	m[2][4] = 0;
	m[3][0] = w.x;
	m[3][1] = w.y;
	m[3][2] = w.z;
	m[3][3] = w.w;
	m[3][4] = 0;
	m[4][0] = 0;
	m[4][1] = 0;
	m[4][2] = 0;
	m[4][3] = 0;
	m[4][4] = 1;
}

Matrix4D Matrix4D::operator * (Matrix4D matrix)
{
	Matrix4D result;
	for (int i = 0; i < 5; i++)
		for (int j = 0; j < 5; j++)
		{
			result.m[i][j] = 0;
			for (int k = 0; k < 5; k++)
				result.m[i][j] += this->m[k][j] * matrix.m[i][k];
		}
	return result;
}

Matrix4D Matrix4D::translation(RealNumber x, RealNumber y, RealNumber z, RealNumber w)
{
	Matrix4D result;
	result.m[4][0] = x;
	result.m[4][1] = y;
	result.m[4][2] = z;
	result.m[4][3] = w;
	return result;
}

Matrix4D Matrix4D::translation(Vector4D vector)
{
	return translation(vector.x, vector.y, vector.z, vector.w);
}

Matrix4D Matrix4D::rotation_xy(RealNumber rotation)
{
	Matrix4D result;
	result.m[2][2] = cos(rotation);
	result.m[2][3] = -sin(rotation);
	result.m[3][2] = sin(rotation);
	result.m[3][3] = cos(rotation);
	return result;
}

Matrix4D Matrix4D::rotation_xz(RealNumber rotation)
{
	Matrix4D result;
	result.m[1][1] = cos(rotation);
	result.m[1][3] = -sin(rotation);
	result.m[3][1] = sin(rotation);
	result.m[3][3] = cos(rotation);
	return result;
}

Matrix4D Matrix4D::rotation_xw(RealNumber rotation)
{
	Matrix4D result;
	result.m[1][1] = cos(rotation);
	result.m[1][2] = -sin(rotation);
	result.m[2][1] = sin(rotation);
	result.m[2][2] = cos(rotation);
	return result;
}

Matrix4D Matrix4D::rotation_yz(RealNumber rotation)
{
	Matrix4D result;
	result.m[0][0] = cos(rotation);
	result.m[0][3] = -sin(rotation);
	result.m[3][0] = sin(rotation);
	result.m[3][3] = cos(rotation);
	return result;
}

Matrix4D Matrix4D::rotation_yw(RealNumber rotation)
{
	Matrix4D result;
	result.m[0][0] = cos(rotation);
	result.m[0][2] = sin(rotation);
	result.m[2][0] = -sin(rotation);
	result.m[2][2] = cos(rotation);
	return result;
}

Matrix4D Matrix4D::rotation_zw(RealNumber rotation)
{
	Matrix4D result;
	result.m[0][0] = cos(rotation);
	result.m[0][1] = sin(rotation);
	result.m[1][0] = -sin(rotation);
	result.m[1][1] = cos(rotation);
	return result;
}

Matrix4D Matrix4D::scale(RealNumber scale)
{
	return Matrix4D::scale(scale, scale, scale, scale);
}

Matrix4D Matrix4D::scale(RealNumber scale_x, RealNumber scale_y, RealNumber scale_z, RealNumber scale_w)
{
	Matrix4D result;
	result.m[0][0] = scale_x;
	result.m[1][1] = scale_y;
	result.m[2][2] = scale_z;
	result.m[3][3] = scale_w;
	return result;
}

Matrix4D Matrix4D::inverse()
{
	Matrix4D result;
	for (int i = 0; i < 5; i++)
	{
		RealNumber* input[5] = { m[0], m[1], m[2], m[3], m[4] };
		RealNumber outcome[5] = { 0, 0, 0, 0, 0 };
		outcome[i] = 1;
		RealNumber* res = AlgebraMaster::solveEquationSystem(5, input, outcome);
		for (int j = 0; j < 5; j++)
			result.m[i][j] = res[j];
		delete res;
	}

	return result;
}

Point4D Matrix4D::operator * (Point4D v)
{
	return Vector4D(
		v.x*m[0][0] + v.y*m[1][0] + v.z*m[2][0] + v.w*m[3][0] + m[4][0],
		v.x*m[0][1] + v.y*m[1][1] + v.z*m[2][1] + v.w*m[3][1] + m[4][1],
		v.x*m[0][2] + v.y*m[1][2] + v.z*m[2][2] + v.w*m[3][2] + m[4][2],
		v.x*m[0][3] + v.y*m[1][3] + v.z*m[2][3] + v.w*m[3][3] + m[4][3]
	);
}

