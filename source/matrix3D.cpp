#include "matrix3D.h"
#include "algebra_master.h"

Matrix3D::Matrix3D()
{
	for (int i = 0; i < 4; i++)
		for (int j = 0; j < 4; j++)
			m[i][j] = i == j ? 1 : 0;
}

Matrix3D::Matrix3D(Vector3D x, Vector3D y, Vector3D z)
{
	m[0][0] = x.x;
	m[0][1] = x.y;
	m[0][2] = x.z;
	m[0][3] = 0;
	m[1][0] = y.x;
	m[1][1] = y.y;
	m[1][2] = y.z;
	m[1][3] = 0;
	m[2][0] = z.x;
	m[2][1] = z.y;
	m[2][2] = z.z;
	m[2][3] = 0;
	m[3][0] = 0;
	m[3][1] = 0;
	m[3][2] = 0;
	m[3][3] = 1;
}

Matrix3D::~Matrix3D()
{
	if (calculated_inverse)
		delete matrix_inverse;
}

Matrix3D Matrix3D::operator * (Matrix3D matrix)
{
	Matrix3D result;
	for (int i = 0; i < 4; i++)
		for (int j = 0; j < 4; j++)
		{
			result.m[i][j] = 0;
			for (int k = 0; k < 4; k++)
				result.m[i][j] += this->m[k][j] * matrix.m[i][k];
		}
	return result;
}

Matrix3D Matrix3D::translation(RealNumber x, RealNumber y, RealNumber z)
{
	Matrix3D result;
	result.m[3][0] = x;
	result.m[3][1] = y;
	result.m[3][2] = z;
	return result;
}

Matrix3D Matrix3D::translation(Vector3D vector)
{
	return translation(vector.x, vector.y, vector.z);
}

Matrix3D Matrix3D::rotation_x(RealNumber rotation)
{
	Matrix3D result;
	result.m[1][1] = cos(rotation);
	result.m[1][2] = -sin(rotation);
	result.m[2][1] = sin(rotation);
	result.m[2][2] = cos(rotation);
	return result;
}

Matrix3D Matrix3D::rotation_y(RealNumber rotation)
{
	Matrix3D result;
	result.m[0][0] = cos(rotation);
	result.m[0][2] = sin(rotation);
	result.m[2][0] = -sin(rotation);
	result.m[2][2] = cos(rotation);
	return result;
}

Matrix3D Matrix3D::rotation_z(RealNumber rotation)
{
	Matrix3D result;
	result.m[0][0] = cos(rotation);
	result.m[0][1] = sin(rotation);
	result.m[1][0] = -sin(rotation);
	result.m[1][1] = cos(rotation);
	return result;
}

Matrix3D Matrix3D::scale(RealNumber scale)
{
	return Matrix3D::scale(scale, scale, scale);
}

Matrix3D Matrix3D::scale(RealNumber scale_x, RealNumber scale_y, RealNumber scale_z)
{
	Matrix3D result;
	result.m[0][0] = scale_x;
	result.m[1][1] = scale_y;
	result.m[2][2] = scale_z;
	return result;
}

Matrix3D Matrix3D::inverse()
{
	if (calculated_inverse)
		return *matrix_inverse;

	matrix_inverse = new Matrix3D();
	for (int i = 0; i < 4; i++)
	{
		RealNumber* input[4] = { m[0], m[1], m[2], m[3] };
		RealNumber outcome[4] = { 0, 0, 0, 0 };
		outcome[i] = 1;
		RealNumber* res = AlgebraMaster::solveEquationSystem(4, input, outcome);
		for (int j = 0; j < 4; j++)
			matrix_inverse->m[i][j] = res[j];
		delete res;
	}
	matrix_inverse->matrix_inverse = this;

	calculated_inverse = true;
	return *matrix_inverse;
}

Point3D Matrix3D::operator * (Point3D v)
{
	return Vector3D(
		v.x*m[0][0] + v.y*m[1][0] + v.z*m[2][0] + m[3][0], 
		v.x*m[0][1] + v.y*m[1][1] + v.z*m[2][1] + m[3][1],
		v.x*m[0][2] + v.y*m[1][2] + v.z*m[2][2] + m[3][2]
	);
}

