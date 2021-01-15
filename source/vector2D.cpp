#include "vector2D.h"

Vector2D::Vector2D(RealNumber x, RealNumber y) : Point2D(x, y)
{

}

Vector2D::Vector2D(Point2D point) : Point2D(point)
{

}

Vector2D Vector2D::operator - ()
{
	return Vector2D(-x, -y);
}

RealNumber Vector2D::dot_product(Vector2D vector)
{
	return this->x*vector.x + this->y*vector.y;
}

RealNumber Vector2D::length()
{
	return sqrt(x*x + y*y);
}

Vector2D Vector2D::normalized()
{
	RealNumber length = this->length();
	return Vector2D(*this) / length;
}

bool Vector2D::is_parallel(Vector2D vector)
{
	Vector2D first = this->normalized();
	Vector2D second = vector.normalized();
	return first == second || first == -second;
}