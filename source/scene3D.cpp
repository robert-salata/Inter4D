#include "scene3D.h"
#include "vector3D.h"
#include "matrix3D.h"
#include "geometry_master.h"

Scene3D::Scene3D()
{

}

Scene3D::Scene3D(Scene3D::Object object)
{
	this->objects.push_back(object);
}

Scene3D Scene3D::load_scene(const wchar_t* file)
{
	Scene3D result;

	int last_shapes_vertices = 0;
	bool is_some_shape = false;

	vector<Point3D> vertices;
	vector<Shape3D::Face> faces;
	Object current_object;
	current_object.name = "Object";

	ifstream input;
	input.open(file);
	if (!input)
	{
		Scene3D result;
		wstring filename = wstring(file);
		result.error_info = "File " + string(filename.begin(), filename.end()) + " not found";
		return result;
	}

	char character;
	string line;
	while (!input.eof())
	{
		character = ' ';
		input >> character;
		if (character == '#' || character == 's' || character == 'm' || character == 'u')
			getline(input, line);
		if (character == 'o')
		{
			if (is_some_shape)
			{
				if (!current_object.corrupted)
				{
					current_object.shape = Shape3D(vertices, faces);
					result.objects.push_back(current_object);
				}
				current_object = Object();
				last_shapes_vertices += vertices.size();
				vertices.clear();
				faces.clear();
			}
			is_some_shape = true;

			input.get(character);
			getline(input, line);
			current_object.name = line == "" ? "Object" : line;
		}
		if (character == 'v')
		{
			input.get(character);
			getline(input, line);

			if (character != 'n')
			{
				stringstream stream_line = stringstream(line);
				double vx, vy, vz;
				stream_line >> vx >> vy >> vz;
				if (!stream_line)
				{
					current_object.corrupted = true;
					result.error_info += "Vertex " + line + " of " + current_object.name + " is malformed (should contain 3 coordinates; e.g.: v 1.23 -0.32 4).\n";
				}
				vertices.push_back(Point3D(vx, vy, vz));
			}
		}
		if (character == 'f')
		{
			vector<int> vertices_temp;

			getline(input, line);
			stringstream stream_line = stringstream(line);

			while (stream_line && !stream_line.eof())
			{
				int current_vertex;
				stream_line >> current_vertex;
				vertices_temp.push_back(current_vertex - 1 - last_shapes_vertices);
				if (!stream_line)
				{
					current_object.corrupted = true;
					result.error_info += "Face " + line + " of " + current_object.name + " is malformed (should contain indices of previously defined vertices, numbered from 1; e.g.: f 1 2 3 4).\n";
				}	
				stream_line.get(character);
				if (character == '/') //avoid normals
				{
					stream_line.get(character);
					stream_line >> current_vertex;
				}
				if (!stream_line.eof())
				{
					do
					{
						stream_line.get(character);
					} while ((character == ' ' || character == '\t') && !stream_line.eof());
					stream_line.unget();
				}
			}

			faces.push_back(Shape3D::Face(vertices_temp));
		}
	}

	if (!current_object.corrupted)
	{
		current_object.shape = Shape3D(vertices, faces);
		result.objects.push_back(current_object);
	}

	input.close();

	//faces validation
	for (int i = 0; i < result.objects.size(); i++)
	{
		vector<Shape3D::Face*> object_faces = result.objects[i].shape.get_faces();
		for (int j = 0; j < object_faces.size() && !result.objects[i].corrupted; j++)
			for (int k = 0; k < object_faces[j]->get_indices_cycle().size() && !result.objects[i].corrupted; k++)
			{
				if (object_faces[j]->get_indices_cycle()[k] < 0)
				{
					result.objects[i].corrupted = true;
					result.error_info += "Face of " + result.objects[i].name + " is malformed (should contain indices of previously defined vertices, numbered from 1; e.g.: f 1 2 3 4).\n";
				}
				else if (object_faces[j]->get_indices_cycle()[k] >= result.objects[i].shape.get_vertices().size())
				{
					result.objects[i].corrupted = true;
					result.error_info += "Face of " + result.objects[i].name + " indicates vertex with index higher than vertices number.\n";
				}
			}
	}
	for (int i = 0; i < result.objects.size(); i++)
		if (result.objects[i].corrupted)
			result.objects.erase(result.objects.begin() + i--);

	//normalization
	if (result.objects.size() > 0)
	{
		Point3D* first = NULL;
		for (int i = 0; i < result.objects.size() && first == NULL; i++)
		{
			auto object_vertices = result.objects[i].shape.get_vertices();
			if (object_vertices.size() > 0)
				first = object_vertices[0];
		}
		if (first == NULL)
			return result;

		RealNumber min_x = first->x, max_x = first->x, min_y = first->y, max_y = first->y, min_z = first->z, max_z = first->z;
		for (int i = 0; i < result.objects.size(); i++)
		{
			auto verts = result.objects[i].shape.get_vertices();
			for (int j = 0; j < verts.size(); j++)
			{
				if (verts[j]->x < min_x)
					min_x = verts[j]->x;
				if (verts[j]->x > max_x)
					max_x = verts[j]->x;
				if (verts[j]->y < min_y)
					min_y = verts[j]->y;
				if (verts[j]->y > max_y)
					max_y = verts[j]->y;
				if (verts[j]->z < min_z)
					min_z = verts[j]->z;
				if (verts[j]->z > max_z)
					max_z = verts[j]->z;
			}
		}

		RealNumber max_d = max(max_x - min_x, max(max_y - min_y, max_z - min_z));
		Matrix3D transformation = Matrix3D::scale(2 / max_d)*Matrix3D::translation(-(min_x + max_x) / 2, -(min_y + max_y) / 2, -(min_z + max_z) / 2);
		for (int i = 0; i < result.objects.size(); i++)
			result.objects[i].shape = GeometryMaster::transform(&result.objects[i].shape, &transformation);
	}

	return result;
}

void Scene3D::save_scene(Scene3D* scene, const wchar_t* file)
{
	ofstream result;
	result.open(file);
	result.precision(numeric_limits<double>::digits10 + 2);

	int last_shapes_vertices = 0;

	for (int i = 0; i < scene->objects.size(); i++)
	{
		result << "o " + scene->objects[i].name << endl;
		vector<Point3D*> vertices = scene->objects[i].shape.get_vertices();
		for (int j = 0; j < vertices.size(); j++)
			result << "v " << vertices[j]->x.evaluate() << " " << vertices[j]->y.evaluate() << " " << vertices[j]->z.evaluate() << endl;

		vector<Shape3D::Face*> faces = scene->objects[i].shape.get_faces();
		for (int j = 0; j < faces.size(); j++)
		{
			result << "f ";
			for (int k = 0; k < faces[j]->get_indices_cycle().size(); k++)
				result << faces[j]->get_indices_cycle()[k] + 1 + last_shapes_vertices << " ";
			result << endl;
		}

		last_shapes_vertices += vertices.size();
	}

	result.close();
}

Scene3D Scene3D::generate_cube()
{
	double a = 2;
	vector<Point3D> vertices;
	vertices.push_back(Point3D(0, 0, 0));
	vertices.push_back(Point3D(a, 0, 0));
	for (int i = 0; i < 2; i++)
		vertices.push_back(vertices[i] + Vector3D(0, a, 0));
	for (int i = 0; i < 4; i++)
		vertices.push_back(vertices[i] + Vector3D(0, 0, a));

	vector<Shape3D::Face> faces;
	faces.push_back(vector<int>{1, 0, 2, 3});
	faces.push_back(vector<int>{5, 4, 6, 7});
	for (int i = 0; i < 4; i++)
		faces.push_back(vector<int>{faces[0].get_indices_cycle()[i], faces[1].get_indices_cycle()[i],
			faces[1].get_indices_cycle()[(i + 1) % 4], faces[0].get_indices_cycle()[(i + 1) % 4] });

	Shape3D cube = Shape3D(vertices, faces);
	cube = GeometryMaster::transform(&cube, &Matrix3D::translation(-Vector3D(cube.calculate_center())));
	cube.recalculate_normals();

	return Scene3D(Object("Cube", cube));
}

Scene3D Scene3D::generate_tetrahedron()
{
	double a = 2;
	vector<Point3D> vertices;
	vertices.push_back(Point3D(0, 0, 0));
	vertices.push_back(Point3D(a, 0, 0));
	
	Point3D center = Shape3D(vertices, vector<Shape3D::Face>({ Shape3D::Face(vector<int>{0, 1}) })).calculate_center();
	vertices.push_back(center + Vector3D(0, 0, sqrt(a*a - center.distance(vertices[0])*center.distance(vertices[0]))));

	center = Shape3D(vertices, vector<Shape3D::Face>({ Shape3D::Face(vector<int>{0, 1, 2}) })).calculate_center();
	vertices.push_back(center + Vector3D(0, sqrt(a*a - center.distance(vertices[0])*center.distance(vertices[0])), 0));

	vector<Shape3D::Face> faces;
	faces.push_back(vector<int>{0, 1, 2});
	for (int i = 0; i < 3; i++)
		faces.push_back(vector<int>{faces[0].get_indices_cycle()[i], 3, faces[0].get_indices_cycle()[(i + 1) % 3]});

	Shape3D tetrahedron = Shape3D(vertices, faces);
	tetrahedron = GeometryMaster::transform(&tetrahedron, &Matrix3D::translation(-Vector3D(tetrahedron.calculate_center())));
	tetrahedron.recalculate_normals();

	return Scene3D(Object("Tetrahedron", tetrahedron));
}

Scene3D Scene3D::generate_cylinder()
{
	double a = 2;
	int n = 128;
	vector<Point3D> vertices;
	for (int i = 0; i < n; i++)
		vertices.push_back(Point3D(a/2 * sin(2 * GeometryMaster::PI * i / n), 0, a/2 * cos(2 * GeometryMaster::PI * i / n)));
	for (int i = 0; i < n; i++)
		vertices.push_back(vertices[i] + Vector3D(0, a, 0));

	vector<Shape3D::Face> faces;
	vector<int> face1;
	for (int i = 0; i < n; i++)
		face1.push_back(i);
	faces.push_back(face1);
	vector<int> face2;
	for (int i = 0; i < n; i++)
		face2.push_back(n + i);
	faces.push_back(face2);	
	for (int i = 0; i < n; i++)
		faces.push_back(vector<int>{faces[0].get_indices_cycle()[i], faces[1].get_indices_cycle()[i],
			faces[1].get_indices_cycle()[(i + 1) % n], faces[0].get_indices_cycle()[(i + 1) % n] });

	Shape3D cylinder = Shape3D(vertices, faces);
	cylinder = GeometryMaster::transform(&cylinder, &Matrix3D::translation(-Vector3D(cylinder.calculate_center())));
	cylinder.recalculate_normals();

	return Scene3D(Object("Cylinder", cylinder));
}

Scene3D Scene3D::generate_cone()
{
	double a = 2;
	int n = 128;
	vector<Point3D> vertices;
	for (int i = 0; i < n; i++)
		vertices.push_back(Point3D(a / 2 * sin(2 * GeometryMaster::PI * i / n), 0, a / 2 * cos(2 * GeometryMaster::PI * i / n)));
	vertices.push_back(Vector3D(0, a, 0));

	vector<Shape3D::Face> faces;
	vector<int> face1;
	for (int i = 0; i < n; i++)
		face1.push_back(i);
	faces.push_back(face1);
	for (int i = 0; i < n; i++)
		faces.push_back(vector<int>{faces[0].get_indices_cycle()[i], n, faces[0].get_indices_cycle()[(i + 1) % n] });

	Shape3D cone = Shape3D(vertices, faces);
	cone = GeometryMaster::transform(&cone, &Matrix3D::translation(Vector3D(0, -a/2, 0)));
	cone.recalculate_normals();

	return Scene3D(Object("Cone", cone));
}

Scene3D Scene3D::generate_sphere(Point3D center, double radius, int horizontal_segments, int vertical_segments)
{
	int n = vertical_segments;
	int m = horizontal_segments;
	vector<Point3D> vertices;
	vertices.push_back(center + Point3D(0, -radius, 0));
	for (int j = 0; j < m - 1; j++)
	{
		double angle = (GeometryMaster::PI*(j + 1) / m - GeometryMaster::PI / 2).evaluate();
		double ring_radius = radius * cos(angle);
		for (int i = 0; i < n; i++)
			vertices.push_back(center + Point3D(ring_radius*sin(2 * GeometryMaster::PI * i / n), radius * sin(angle), ring_radius * cos(2 * GeometryMaster::PI * i / n)));
	}
	vertices.push_back(center + Point3D(0, radius, 0));

	vector<Shape3D::Face> faces;
	for (int i = 0; i < n; i++)
		faces.push_back(vector<int>{0, 1 + i, 1 + (i + 1) % n});
	for (int j = 0; j < m - 2; j++)
		for (int i = 0; i < n; i++)
			faces.push_back(vector<int>{1 + j*n + i, 1 + j*n + (i+1)%n, 1 + (j+1)*n + (i + 1) % n, 1 + (j+1)*n + i});
	for (int i = 0; i < n; i++)
		faces.push_back(vector<int>{1+(m-1)*n, 1 + (m-2)*n + i, 1 + (m-2)*n + (i + 1) % n});

	Shape3D sphere = Shape3D(vertices, faces);
	sphere.recalculate_normals();

	return Scene3D(Object("Sphere", sphere));
}