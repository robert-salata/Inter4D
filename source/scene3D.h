#ifndef Scene_3d_class
#define Scene_3d_class

#include "shape3D.h"
#include "common.h"

class Scene3D
{
public:
	class Object
	{
	public:
		Object() { };
		Object(string name, Shape3D shape) : name(name), shape(shape) { }
		Shape3D shape;
		string name;
		bool corrupted = false;
	};

	Scene3D();
	Scene3D(Object object);

	static Scene3D load_scene(const wchar_t* file);
	static void save_scene(Scene3D* scene, const wchar_t* file);
	static Scene3D generate_cube();
	static Scene3D generate_tetrahedron();
	static Scene3D generate_cylinder();
	static Scene3D generate_cone();
	static Scene3D generate_sphere(Point3D center, double radius, int horizontal_segments, int vertical_segments);


	vector<Object> objects;
	string error_info = "";
};

#endif