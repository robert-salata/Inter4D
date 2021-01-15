#ifndef Material_class
#define Material_class

class Color
{
public:
	Color() : r(0), g(0), b(0), a(0) { }
	Color(double r, double g, double b, double a = 1) : r(r), g(g), b(b), a(a) { }
	double r, g, b, a;
};

enum MaterialType { NORMAL, SELF_COLOR };

class Material
{
public:
	Material(MaterialType type = NORMAL, Color color = Color(1, 1, 1)) : type(type), color(color) { }
	MaterialType type;
	Color color;
};

#endif