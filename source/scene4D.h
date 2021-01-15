#ifndef Scene_4d_class
#define Scene_4d_class

#include "shape4D.h"
#include <string>

class Scene4D
{
public:
	class Object
	{
	public:
		Object() { };
		Object(string name, Shape4D shape) : name(name), shape(shape) { }
		Shape4D shape;
		string name;
		bool corrupted = false;
	};

	Scene4D();
	Scene4D(Object object);

	static Scene4D load_scene(const wchar_t* file);
	static void save_scene(Scene4D* scene, const wchar_t* file);
	static Scene4D generate_hypertetrahedron();
	static Scene4D generate_hypercylinder();
	static Scene4D generate_hypercone();
	static Scene4D generate_hypersphere();
	static Scene4D generate_8_cell();
	static Scene4D generate_16_cell();
	static Scene4D generate_120_cell();
	static Scene4D generate_600_cell();
	static Scene4D generate_cross();
	static vector<int> extrude(Shape4D& shape, vector<int> vertices);

	vector<Object> objects;
	string error_info = "";
};

#endif