#include "vector4D.h"

Vector4D::Vector4D(RealNumber x, RealNumber y, RealNumber z, RealNumber w) : Point4D(x, y, z, w)
{

}

Vector4D::Vector4D(Point4D point) : Point4D(point)
{

}

Vector4D Vector4D::operator - ()
{
	return Vector4D(-x, -y, -z, -w);
}

RealNumber Vector4D::dot_product(Vector4D vector)
{
	return this->x*vector.x + this->y*vector.y + this->z*vector.z + this->w*vector.w;
}

RealNumber Vector4D::length()
{
	return sqrt(x*x + y*y + z*z + w*w);
}

Vector4D Vector4D::normalized()
{
	RealNumber length = this->length();
	return Vector4D(*this) / length;
}

bool Vector4D::is_parallel(Vector4D vector)
{
	Vector4D first = this->normalized();
	Vector4D second = vector.normalized();
	return first == second || first == -second;
}