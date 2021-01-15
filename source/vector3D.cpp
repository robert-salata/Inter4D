#include "vector3D.h"

Vector3D::Vector3D(RealNumber x, RealNumber y, RealNumber z) : Point3D(x, y, z)
{

}

Vector3D::Vector3D(Point3D point) : Point3D(point)
{

}

Vector3D Vector3D::operator - ()
{
	return Vector3D(-x, -y, -z);
}

Vector3D Vector3D::product(Vector3D vector)
{
	return Vector3D(
		this->y*vector.z - this->z*vector.y,
		this->z*vector.x - this->x*vector.z,
		this->x*vector.y - this->y*vector.x);
}

RealNumber Vector3D::dot_product(Vector3D vector)
{
	return this->x*vector.x + this->y*vector.y + this->z*vector.z;
}

RealNumber Vector3D::length()
{
	return sqrt(x*x + y*y + z*z);
}

Vector3D Vector3D::normalized()
{
	RealNumber length = this->length();
	return Vector3D(*this) / length;
}

bool Vector3D::is_parallel(Vector3D vector)
{
	Vector3D first = this->normalized();
	Vector3D second = vector.normalized();
	return first == second || first == -second;
}