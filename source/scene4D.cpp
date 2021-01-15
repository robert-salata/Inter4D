#include "scene4D.h"
#include "vector4D.h"
#include "matrix4D.h"
#include "geometry_master.h"
#include "scene3D.h"
#include <algorithm>

Scene4D::Scene4D()
{

}

Scene4D::Scene4D(Scene4D::Object object)
{
	this->objects.push_back(object);
}

Scene4D Scene4D::load_scene(const wchar_t* file)
{
	Scene4D result;

	int last_shapes_vertices = 0;
	int last_shapes_faces = 0;
	bool is_some_shape = false;

	vector<Point4D> vertices;
	vector<Shape4D::Face> faces;
	vector<Shape4D::Cell> cells;
	Object current_object;
	current_object.name = "Object";

	ifstream input;
	input.open(file);
	if (!input)
	{
		Scene4D result;
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
		if (character == '#')
			getline(input, line);
		if (character == 'o')
		{
			if (is_some_shape)
			{
				if (!current_object.corrupted)
				{
					current_object.shape = Shape4D(vertices, faces, cells);
					result.objects.push_back(current_object);
				}
				current_object = Object();
				last_shapes_vertices += vertices.size();
				last_shapes_faces += faces.size();
				vertices.clear();
				faces.clear();
				cells.clear();
			}
			is_some_shape = true;

			input.get(character);
			getline(input, line);
			current_object.name = line == "" ? "Object" : line;
		}
		if (character == 'v')
		{
			getline(input, line);
			stringstream stream_line = stringstream(line);
			double vx, vy, vz, vw;
			stream_line >> vx >> vy >> vz >> vw;
			if (!stream_line)
			{
				current_object.corrupted = true;
				result.error_info += "Vertex " + line + " of " + current_object.name + " is malformed (should contain 4 coordinates; e.g.: v 1.23 -0.32 4 0).\n";
			}
			vertices.push_back(Point4D(vx, vy, vz, vw));
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
				if (!stream_line.eof())
				{
					do
					{
						stream_line.get(character);
					} while ((character == ' ' || character == '\t') && !stream_line.eof());
					stream_line.unget();
				}
			}

			faces.push_back(Shape4D::Face(vertices_temp));
		}
		if (character == 'h')
		{
			set<int> faces_temp;

			getline(input, line);
			stringstream stream_line = stringstream(line);

			while (stream_line && !stream_line.eof())
			{
				int current_face;
				stream_line >> current_face;
				faces_temp.insert(current_face - 1 - last_shapes_faces);
				if (!stream_line)
				{
					current_object.corrupted = true;
					result.error_info += "Cell " + line + " of " + current_object.name + " is malformed (should contain indices of previously defined faces, numbered from 1; e.g.: h 1 2 3 4).\n";
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

			cells.push_back(Shape4D::Cell(faces_temp));
		}
	}

	if (!current_object.corrupted)
	{
		current_object.shape = Shape4D(vertices, faces, cells);
		result.objects.push_back(current_object);
	}

	input.close();

	//faces validation
	for (int i = 0; i < result.objects.size(); i++)
	{
		vector<Shape4D::Face*> object_faces = result.objects[i].shape.get_faces();
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
	//cells validation
	for (int i = 0; i < result.objects.size(); i++)
	{
		vector<Shape4D::Cell*> object_cells = result.objects[i].shape.get_cells();
		for (int j = 0; j < object_cells.size() && !result.objects[i].corrupted; j++)
		{
			auto cell_faces = object_cells[j]->get_faces();
			for (auto it = cell_faces.begin(); it != cell_faces.end() && !result.objects[i].corrupted; it++)
			{
				if (*it < 0)
				{
					result.objects[i].corrupted = true;
					result.error_info += "Cell of " + result.objects[i].name + " is malformed (should contain indices of previously defined faces, numbered from 1; e.g.: h 1 2 3 4).\n";
				}
				else if (*it >= result.objects[i].shape.get_faces().size())
				{
					result.objects[i].corrupted = true;
					result.error_info += "Cell of " + result.objects[i].name + " indicates face with index higher than faces number.\n";
				}
			}
		}
	}
	for (int i = 0; i < result.objects.size(); i++)
		if (result.objects[i].corrupted)
			result.objects.erase(result.objects.begin() + i--);

	//normalization
	if (result.objects.size() > 0)
	{
		Point4D* first = NULL;
		for (int i = 0; i < result.objects.size() && first == NULL; i++)
		{
			auto object_vertices = result.objects[i].shape.get_vertices();
			if (object_vertices.size() > 0)
				first = object_vertices[0];
		}
		if (first == NULL)
			return result;

		RealNumber min_x = first->x, max_x = first->x, min_y = first->y, max_y = first->y, min_z = first->z, max_z = first->z, min_w = first->w, max_w = first->w;
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
				if (verts[j]->w < min_w)
					min_w = verts[j]->w;
				if (verts[j]->w > max_w)
					max_w = verts[j]->w;
			}
		}

		RealNumber max_d = max(max_x - min_x, max(max_y - min_y, max(max_z - min_z, max_w - min_w)));
		Matrix4D transformation = Matrix4D::scale(2 / max_d)*
			Matrix4D::translation(-(min_x + max_x) / 2, -(min_y + max_y) / 2, -(min_z + max_z) / 2, -(min_w + max_w) / 2);
		for (int i = 0; i < result.objects.size(); i++)
			result.objects[i].shape = GeometryMaster::transform(&result.objects[i].shape, &transformation);
	}

	return result;
}

void Scene4D::save_scene(Scene4D* scene, const wchar_t* file)
{
	ofstream result;
	result.open(file);
	result.precision(numeric_limits<double>::digits10 + 2);

	int last_shapes_vertices = 0;
	int last_shapes_faces = 0;

	for (int i = 0; i < scene->objects.size(); i++)
	{
		result << "o " + scene->objects[i].name << endl;
		vector<Point4D*> vertices = scene->objects[i].shape.get_vertices();
		for (int j = 0; j < vertices.size(); j++)
			result << "v " << vertices[j]->x.evaluate() << " " << vertices[j]->y.evaluate() << " " << vertices[j]->z.evaluate() << " " << vertices[j]->w.evaluate() << endl;
		
		vector<Shape4D::Face*> faces = scene->objects[i].shape.get_faces();
		for (int j = 0; j < faces.size(); j++)
		{
			result << "f ";
			for (int k = 0; k < faces[j]->get_indices_cycle().size(); k++)
				result << faces[j]->get_indices_cycle()[k] + 1 + last_shapes_vertices  << " ";
			result << endl;
		}

		vector<Shape4D::Cell*> cells = scene->objects[i].shape.get_cells();
		for (int j = 0; j < cells.size(); j++)
		{
			result << "h ";
			set<int> cell_faces = cells[j]->get_faces();
			for (auto it = cell_faces.begin(); it != cell_faces.end(); it++)
				result << *it + 1 + last_shapes_faces << " ";
			result << endl;
		}

		last_shapes_vertices += vertices.size();
		last_shapes_faces += faces.size();
	}

	result.close();
}

/*Scene4D Scene4D::generate_hypercube()
{
	double a = 2;
	vector<Point4D> vertices;
	vertices.push_back(Point4D(0, 0, 0, 0));
	vertices.push_back(Point4D(a, 0, 0, 0));
	for (int i = 0; i < 2; i++)
		vertices.push_back(vertices[i] + Vector4D(0, a, 0, 0));
	for (int i = 0; i < 4; i++)
		vertices.push_back(vertices[i] + Vector4D(0, 0, a, 0));
	for (int i = 0; i < 8; i++)
		vertices.push_back(vertices[i] + Vector4D(0, 0, 0, a));

	Shape3D cube = Scene3D::generate_cube().objects[0].shape;

	vector<Shape4D::Face> faces;
	for (int i = 0; i < cube.get_faces.size(); i++)
		faces.push_back(cube.get_faces()[i]->get_indices_cycle());
	for (int i = 0; i < 6; i++)
	{
		vector<int> backface = faces[i].get_indices_cycle();
		for (int j = 0; j < 4; j++)
			backface[j] += 8;
		faces.push_back(backface);
	}


	Shape4D hypercube = Shape4D(vertices, faces, cells);
	hypercube = MathEngine::transform(&hypercube, &Matrix4D::translation(-Vector4D(hypercube.calculate_center())));

	return Scene4D(Object("HyperCube", hypercube));
}*/

Scene4D Scene4D::generate_hypertetrahedron()
{
	double a = 2;
	vector<Point4D> vertices;
	vertices.push_back(Point4D(0, 0, 0, 0));
	vertices.push_back(Point4D(a, 0, 0, 0));

	Point4D center = Shape4D(vertices, vector<Shape4D::Face>({ Shape4D::Face(vector<int>{0, 1}) }), vector<Shape4D::Cell>({ Shape4D::Cell(set<int>{0})})).calculate_center();
	vertices.push_back(center + Vector4D(0, 0, sqrt(a*a - center.distance(vertices[0])*center.distance(vertices[0])), 0));

	center = Shape4D(vertices, vector<Shape4D::Face>({ Shape4D::Face(vector<int>{0, 1, 2}) }), vector<Shape4D::Cell>({ Shape4D::Cell(set<int>{0}) })).calculate_center();
	vertices.push_back(center + Vector4D(0, 0, 0, sqrt(a*a - center.distance(vertices[0])*center.distance(vertices[0]))));

	center = Shape4D(vertices, 
		vector<Shape4D::Face>({ Shape4D::Face(vector<int>{0, 1, 2}), Shape4D::Face(vector<int>{0, 1, 3}), Shape4D::Face(vector<int>{1, 2, 3}) }), 
		vector<Shape4D::Cell>({ Shape4D::Cell(set<int>{0}) })).calculate_center();
	vertices.push_back(center + Vector4D(0, sqrt(a*a - center.distance(vertices[0])*center.distance(vertices[0])), 0, 0));

	vector<Shape4D::Face> faces;
	faces.push_back(vector<int>{0, 1, 2}); //0
	faces.push_back(vector<int>{0, 1, 3}); //1
	faces.push_back(vector<int>{0, 1, 4}); //2
	faces.push_back(vector<int>{0, 2, 3}); //3
	faces.push_back(vector<int>{0, 2, 4}); //4
	faces.push_back(vector<int>{0, 3, 4}); //5
	faces.push_back(vector<int>{1, 2, 3}); //6
	faces.push_back(vector<int>{1, 2, 4}); //7
	faces.push_back(vector<int>{1, 3, 4}); //8
	faces.push_back(vector<int>{2, 3, 4}); //9

	vector<Shape4D::Cell> cells;
	cells.push_back(set<int>{6, 7, 8, 9});
	cells.push_back(set<int>{3, 4, 5, 9});
	cells.push_back(set<int>{1, 2, 5, 8});
	cells.push_back(set<int>{0, 2, 4, 7});
	cells.push_back(set<int>{0, 1, 3, 6});

	Shape4D tetrahedron = Shape4D(vertices, faces, cells);
	tetrahedron = GeometryMaster::transform(&tetrahedron, &Matrix4D::translation(-Vector4D(tetrahedron.calculate_center())));

	return Scene4D(Object("HyperTetrahedron", tetrahedron));
}

Scene4D Scene4D::generate_hypercylinder()
{
	double a = 2;

	Shape3D sphere = Scene3D::generate_sphere(Point3D(0, 0, 0), 1, 16, 32).objects[0].shape;
	auto sphere_vertices = sphere.get_vertices();
	auto sphere_edges = sphere.get_edges();
	auto sphere_faces = sphere.get_faces();

	vector<Point4D> vertices;
	for (int i = 0; i < sphere_vertices.size(); i++)
		vertices.push_back(Point4D(sphere_vertices[i]->x, -a / 2, sphere_vertices[i]->z, sphere_vertices[i]->y));
	for (int i = 0; i < sphere_vertices.size(); i++)
		vertices.push_back(Point4D(sphere_vertices[i]->x, a / 2, sphere_vertices[i]->z, sphere_vertices[i]->y));

	vector<Shape4D::Face> faces;
	for (int i = 0; i < sphere_faces.size(); i++)
		faces.push_back(sphere_faces[i]->get_indices_cycle());
	for (int i = 0; i < sphere_faces.size(); i++)
	{
		vector<int> backface = faces[i].get_indices_cycle();
		for (int j = 0; j < backface.size(); j++)
			backface[j] += sphere_vertices.size();
		faces.push_back(backface);
	}
	for (int i = 0; i < sphere_edges.size(); i++)
		faces.push_back(Shape4D::Face(vector<int>{sphere_edges[i]->first, sphere_edges[i]->second,
			sphere_edges[i]->second + (int)sphere_vertices.size(), sphere_edges[i]->first + (int)sphere_vertices.size()}));

	vector<Shape4D::Cell> cells;
	set<int> lower_cell;
	for (int i = 0; i < sphere_faces.size(); i++)
		lower_cell.insert(i);
	cells.push_back(Shape4D::Cell(lower_cell));
	set<int> upper_cell;
	for (int i = 0; i < sphere_faces.size(); i++)
		upper_cell.insert(i + sphere_faces.size());
	cells.push_back(Shape4D::Cell(upper_cell));

	for (int i = 0; i < sphere_faces.size(); i++)
	{
		set<int> cell_faces;
		cell_faces.insert(i);
		cell_faces.insert(i + sphere_faces.size());
		auto face_edges = sphere.edges_cycle_of_face(sphere_faces[i]);
		for (int j = 0; j < face_edges.size(); j++)
		{
			auto edge_face = Shape4D::Face(vector<int>{face_edges[j]->first, face_edges[j]->second,
				face_edges[j]->second + (int)sphere_vertices.size(), face_edges[j]->first + (int)sphere_vertices.size()});
			cell_faces.insert(std::find(faces.begin(), faces.end(), edge_face) - faces.begin());
		}
		cells.push_back(cell_faces);
	}

	Shape4D cylinder = Shape4D(vertices, faces, cells);
	return Scene4D(Object("HyperCylinder", cylinder));
}

Scene4D Scene4D::generate_hypercone()
{
	double a = 2;

	Shape3D sphere = Scene3D::generate_sphere(Point3D(0, 0, 0), 1, 8, 16).objects[0].shape;
	auto sphere_vertices = sphere.get_vertices();
	auto sphere_edges = sphere.get_edges();
	auto sphere_faces = sphere.get_faces();

	vector<Point4D> vertices;
	for (int i = 0; i < sphere_vertices.size(); i++)
		vertices.push_back(Point4D(sphere_vertices[i]->x, -a / 2, sphere_vertices[i]->z, sphere_vertices[i]->y));
	vertices.push_back(Point4D(0, a / 2, 0, 0));

	vector<Shape4D::Face> faces;
	for (int i = 0; i < sphere_faces.size(); i++)
		faces.push_back(sphere_faces[i]->get_indices_cycle());
	for (int i = 0; i < sphere_edges.size(); i++)
		faces.push_back(Shape4D::Face(vector<int>{sphere_edges[i]->first, sphere_edges[i]->second, (int)sphere_vertices.size()}));

	vector<Shape4D::Cell> cells;
	set<int> lower_cell;
	for (int i = 0; i < sphere_faces.size(); i++)
		lower_cell.insert(i);
	cells.push_back(Shape4D::Cell(lower_cell));

	for (int i = 0; i < sphere_faces.size(); i++)
	{
		set<int> cell_faces;
		cell_faces.insert(i);
		auto face_edges = sphere.edges_cycle_of_face(sphere_faces[i]);
		for (int j = 0; j < face_edges.size(); j++)
		{
			auto edge_face = Shape4D::Face(vector<int>{face_edges[j]->first, face_edges[j]->second, (int)sphere_vertices.size()});
			cell_faces.insert(std::find(faces.begin(), faces.end(), edge_face) - faces.begin());
		}
		cells.push_back(cell_faces);
	}

	Shape4D cone = Shape4D(vertices, faces, cells);
	return Scene4D(Object("HyperCone", cone));
}

Scene4D Scene4D::generate_hypersphere()
{
	double a = 2;
	int layers = 8;

	Shape3D sphere = Scene3D::generate_sphere(Point3D(0, 0, 0), 1, 8, 16).objects[0].shape;
	auto sphere_vertices = sphere.get_vertices();
	auto sphere_edges = sphere.get_edges();
	auto sphere_faces = sphere.get_faces();

	vector<Point4D> vertices;
	vertices.push_back(Point4D(0, 0, 0, -a/2));
	for (int l = 0; l < layers - 1; l++)
	{
		double angle = (GeometryMaster::PI*(l + 1) / layers - GeometryMaster::PI / 2).evaluate();
		double layer_radius = a/2 * cos(angle);
		Shape3D layer = Scene3D::generate_sphere(Point3D(0, 0, 0), layer_radius, 8, 16).objects[0].shape;
		auto layer_vertices = layer.get_vertices();
		for (int j = 0; j < layer_vertices.size(); j++)
			vertices.push_back(Point4D(layer_vertices[j]->x, layer_vertices[j]->y, layer_vertices[j]->z, a / 2 * sin(angle)));
	}
	vertices.push_back(Point4D(0, 0, 0, a/2));

	vector<Shape4D::Face> faces;
	for (int l = 0; l < layers - 1; l++)
		for (int i = 0; i < sphere_faces.size(); i++)
		{
			vector<int> layer_face = sphere_faces[i]->get_indices_cycle();
			for (int j = 0; j < layer_face.size(); j++)
				layer_face[j] += 1 + l*(int)sphere_vertices.size();
			faces.push_back(layer_face);
		}

	for (int i = 0; i < sphere_edges.size(); i++)
		faces.push_back(Shape4D::Face(vector<int>{1 + sphere_edges[i]->first, 1 + sphere_edges[i]->second, 0}));

	for (int l = 0; l < layers - 2; l++)
		for (int i = 0; i < sphere_edges.size(); i++)
			faces.push_back(Shape4D::Face(vector<int>{
				1 + sphere_edges[i]->first + l*(int)sphere_vertices.size(), 1 + sphere_edges[i]->second + l*(int)sphere_vertices.size(),
				1 + sphere_edges[i]->second + (l+1)*(int)sphere_vertices.size(), 1 + sphere_edges[i]->first + (l+1)*(int)sphere_vertices.size()}));

	for (int i = 0; i < sphere_edges.size(); i++)
		faces.push_back(Shape4D::Face(vector<int>{
			1 + sphere_edges[i]->first + (layers - 2)*(int)sphere_vertices.size(), 
			1 + sphere_edges[i]->second + (layers - 2)*(int)sphere_vertices.size(), 1 + (layers - 1)*(int)sphere_vertices.size()}));

	vector<Shape4D::Cell> cells;
	for (int i = 0; i < sphere_faces.size(); i++)
	{
		set<int> cell_faces;
		cell_faces.insert(i);
		auto face_edges = sphere.edges_cycle_of_face(sphere_faces[i]);
		for (int j = 0; j < face_edges.size(); j++)
		{
			auto edge_face = Shape4D::Face(vector<int>{1 + face_edges[j]->first, 1 + face_edges[j]->second, 0});
			cell_faces.insert(std::find(faces.begin(), faces.end(), edge_face) - faces.begin());
		}
		cells.push_back(cell_faces);
	}

	for (int l = 0; l < layers - 2; l++)
		for (int i = 0; i < sphere_faces.size(); i++)
		{
			set<int> cell_faces;
			cell_faces.insert(i + l*sphere_faces.size());
			cell_faces.insert(i + (l+1)*sphere_faces.size());
			auto face_edges = sphere.edges_cycle_of_face(sphere_faces[i]);
			for (int j = 0; j < face_edges.size(); j++)
			{
				auto edge_face = Shape4D::Face(vector<int>{
					1 + face_edges[j]->first + l*(int)sphere_vertices.size(), 1 + face_edges[j]->second + l*(int)sphere_vertices.size(),
					1 + face_edges[j]->second + (l+1)*(int)sphere_vertices.size(), 1 + face_edges[j]->first + (l+1)*(int)sphere_vertices.size()});
				cell_faces.insert(std::find(faces.begin(), faces.end(), edge_face) - faces.begin());
			}
			cells.push_back(cell_faces);
		}

	for (int i = 0; i < sphere_faces.size(); i++)
	{
		set<int> cell_faces;
		cell_faces.insert(i + (layers-2)*sphere_faces.size());
		auto face_edges = sphere.edges_cycle_of_face(sphere_faces[i]);
		for (int j = 0; j < face_edges.size(); j++)
		{
			auto edge_face = Shape4D::Face(vector<int>{
				1 + face_edges[j]->first + (layers - 2)*(int)sphere_vertices.size(), 
				1 + face_edges[j]->second + (layers - 2)*(int)sphere_vertices.size(), 1 + (layers - 1)*(int)sphere_vertices.size()});
			cell_faces.insert(std::find(faces.begin(), faces.end(), edge_face) - faces.begin());
		}
		cells.push_back(cell_faces);
	}

	Shape4D hypersphere = Shape4D(vertices, faces, cells);
	return Scene4D(Object("HyperSphere", hypersphere));
}

Scene4D Scene4D::generate_16_cell()
{
	Scene4D main = load_scene(L"data/4D/16.obj");
	auto main_faces = main.objects[0].shape.get_faces();
	for (int i = 0; i < 24; i++)
	{
		Scene3D cell = Scene3D::load_scene((L"data/4D/16_" + to_wstring(i + 1) + L".obj").c_str());
		vector<int> faces;
		auto cell_faces = cell.objects[0].shape.get_faces();
		for (int j = 0; j < cell_faces.size(); j++)
		{
			Shape4D::Face f = Shape4D::Face(cell_faces[j]->get_indices_cycle());
			for (int k = 0; k < main_faces.size(); k++)
				if (*main_faces[k] == f)
				{
					faces.push_back(k + 1);
					break;
				}
		}
		int a = 2;
		a += 2;
	}

	return Scene4D();
}

Scene4D Scene4D::generate_120_cell()
{
	Scene3D main = Scene3D::load_scene(L"data/4D/120BLEND.obj"); //120BLEND
	vector<Point3D*> points = main.objects[0].shape.get_vertices();
	vector<Shape3D::Edge*> edges = main.objects[0].shape.get_edges();
	vector<Shape3D::Face*> faces = main.objects[0].shape.get_faces();
	/*std::sort(points.begin(), points.end(), 
		[](Point3D* v1, Point3D* v2) { return v1->distance(Point3D(0, 0, 0)) < v2->distance(Point3D(0, 0, 0)); });

	vector<double> distancess;
	for (int i = 0; i < points.size(); i++)
		distancess.push_back(points[i]->distance(Point3D(0, 0, 0)).evaluate());*/

	//estabilishing vert layers
	vector<vector<int>> layers;
	layers.push_back(vector<int>());
	for (int i = 0; i < points.size(); i++)
		if (points[i]->distance(Point3D(0, 0, 0)) > 4) // > 4 without normalization
			layers[0].push_back(i);

	int layer_done = 0;
	while (layers[layer_done].size() > 0)
	{
		layers.push_back(vector<int>());
		for (int i = 0; i < layers[layer_done].size(); i++)
			for (int j = 0; j < edges.size(); j++)
			{
				int neighbor = -1;
				if (edges[j]->first == layers[layer_done][i])
					neighbor = edges[j]->second;
				if (edges[j]->second == layers[layer_done][i])
					neighbor = edges[j]->first;
				if (neighbor != -1)
				{
					if (std::find(layers[layer_done + 1].begin(), layers[layer_done + 1].end(), neighbor) == layers[layer_done + 1].end() &&
						std::find(layers[layer_done].begin(), layers[layer_done].end(), neighbor) == layers[layer_done].end() &&
						(layer_done == 0 || 
							std::find(layers[layer_done - 1].begin(), layers[layer_done - 1].end(), neighbor) == layers[layer_done - 1].end()))
						layers[layer_done + 1].push_back(neighbor);
				}
			}
		layer_done++;
	}

	vector<vector<int>> layers_reverse;
	layers_reverse.push_back(vector<int>());
	for (int i = 0; i < layers[layer_done - 1].size(); i++)
		layers_reverse[0].push_back(layers[layer_done - 1][i]);

	layer_done = 0;
	while (layers_reverse[layer_done].size() > 0)
	{
		layers_reverse.push_back(vector<int>());
		for (int i = 0; i < layers_reverse[layer_done].size(); i++)
			for (int j = 0; j < edges.size(); j++)
			{
				int neighbor = -1;
				if (edges[j]->first == layers_reverse[layer_done][i])
					neighbor = edges[j]->second;
				if (edges[j]->second == layers_reverse[layer_done][i])
					neighbor = edges[j]->first;
				if (neighbor != -1)
				{
					if (std::find(layers_reverse[layer_done + 1].begin(), layers_reverse[layer_done + 1].end(), neighbor) == layers_reverse[layer_done + 1].end() &&
						std::find(layers_reverse[layer_done].begin(), layers_reverse[layer_done].end(), neighbor) == layers_reverse[layer_done].end() &&
						(layer_done == 0 ||
							std::find(layers_reverse[layer_done - 1].begin(), layers_reverse[layer_done - 1].end(), neighbor) == layers_reverse[layer_done - 1].end()))
						layers_reverse[layer_done + 1].push_back(neighbor);
				}
			}
		layer_done++;
	}

	vector<vector<int>> layers_final = vector<vector<int>>(15);
	for (int i = 0; i < 6; i++)
		layers_final[i] = layers[i];
	for (int i = 0; i < 6; i++)
		layers_final[14 - i] = layers_reverse[i];

	vector<int> middle;
	for (int i = 0; i < layers[6].size(); i++)
		if (std::find(layers_reverse[6].begin(), layers_reverse[6].end(), layers[6][i]) != layers_reverse[6].end())
			middle.push_back(layers[6][i]);

	vector<int>premiddle;
	for (int i = 0; i < layers[6].size(); i++)
		if (std::find(middle.begin(), middle.end(), layers[6][i]) == middle.end())
			premiddle.push_back(layers[6][i]);

	vector<int>postmiddle;
	for (int i = 0; i < layers_reverse[6].size(); i++)
		if (std::find(middle.begin(), middle.end(), layers_reverse[6][i]) == middle.end())
			postmiddle.push_back(layers_reverse[6][i]);

	layers_final[6] = premiddle;
	layers_final[7] = middle;
	layers_final[8] = postmiddle;

	set<int> test;
	for (int i = 0; i < layers_final.size(); i++)
		for (int j = 0; j < layers_final[i].size(); j++)
			test.insert(layers_final[i][j]);

	bool ok = test.size() == 600;

	vector<vector<double>>distances;
	for (int i = 0; i < layers_final.size(); i++)
	{
		distances.push_back(vector<double>());
		for (int j = 0; j < layers_final[i].size(); j++)
			distances[i].push_back(points[layers_final[i][j]]->distance(Point3D(0, 0, 0)).evaluate());
	}
	
	//unprojecting
	vector<Point4D> new_points;
	map<int, int> vert_map;

	double w_array[15] = { 2.61803398875, 2.2360679775, 2, 1.61803398875, 1, 0.61803398875, 0.38196601125, 0,
		-0.38196601125, -0.61803398875, -1, -1.61803398875, -2, -2.2360679775, -2.61803398875 };
	for (int i = 0; i < layers_final.size(); i++)
	{
		for (int j = 0; j < layers_final[i].size(); j++)
		{
			Point3D scaled = *points[layers_final[i][j]];
			scaled = scaled/scaled.distance(Point3D(0, 0, 0)) * sqrt(8 - w_array[i] * w_array[i]);
			new_points.push_back(Point4D(scaled.x, scaled.y, scaled.z, w_array[i]));
			vert_map.insert(make_pair(new_points.size() - 1, layers_final[i][j]));
		}
	}

	//snapping to allowed values
	double fi = (sqrt(5) + 1) / 2;
	double possible_4[7][4] = {
		{2, 2, 0, 0},
		{sqrt(5), 1, 1, 1},
		{fi, fi, fi, 1/(fi*fi)},
		{fi*fi, 1/fi, 1/fi, 1/fi},
		
		{fi*fi, 1, 1/(fi*fi), 0},
		{sqrt(5), fi, 1/fi, 0},
		{2, fi, 1, 1/fi}
	};
	int fitted_in[7];
	for (int i = 0; i < 7; i++)
		fitted_in[i] = 0;
	for (int i = 0; i < new_points.size(); i++)
	{
		RealNumber coords[4] = { new_points[i].x, new_points[i].y, new_points[i].z, new_points[i].w };
		int pos[4] = { 0, 1, 2, 3 };

		for (int k = 0; k < 4; k++)
		{
			int max = k;
			for (int l = k + 1; l < 4; l++)
				if (abs(coords[l]) > abs(coords[max]))
					max = l;
			swap(coords[k], coords[max]);
			swap(pos[k], pos[max]);
		}

		int best_fit_pos = 0;
		RealNumber best_fit = 100;
		for (int j = 0; j < 7; j++)
		{
			RealNumber fit = 0;
			for (int k = 0; k < 4; k++)
				fit += (possible_4[j][k] - abs(coords[k]))*(possible_4[j][k] - abs(coords[k]));
			if (fit < best_fit)
			{
				best_fit = fit;
				best_fit_pos = j;
			}
		}

		double coords_precise[4];
		for (int j = 0; j < 4; j++)
			coords_precise[pos[j]] = possible_4[best_fit_pos][j] * (coords[j] < 0 ? -1 : 1);

		new_points[i] = Point4D(coords_precise[0], coords_precise[1], coords_precise[2], coords_precise[3]);

		fitted_in[best_fit_pos]++;
	}

	//fixing faulty vertices
	Shape3D blend1 = Scene3D::load_scene(L"data/4d/120snapshot.obj").objects[0].shape;
	vector<Point3D*> v1 = blend1.get_vertices();
	Shape3D blend2 = Scene3D::load_scene(L"data/4d/120fixed.obj").objects[0].shape;
	vector<Point3D*> v2 = blend2.get_vertices();

	for (int i = 0; i < new_points.size(); i++)
	{
		Point3D saved_as = Point3D(new_points[i].x, new_points[i].y, new_points[i].z) / (3 - new_points[i].w);
		Point3D res;
		for (int j = 0; j < v1.size(); j++)
			if (abs(v1[j]->x - saved_as.x) < 0.0001 && abs(v1[j]->y - saved_as.y) < 0.0001 && abs(v1[j]->z - saved_as.z) < 0.0001)
				res = *v2[j];

		res *= (3 - new_points[i].w);
		new_points[i] = Point4D(res.x, res.y, res.z, new_points[i].w);
	}

	//resnapping to allowed values
	for (int i = 0; i < 7; i++)
		fitted_in[i] = 0;
	for (int i = 0; i < new_points.size(); i++)
	{
		RealNumber coords[4] = { new_points[i].x, new_points[i].y, new_points[i].z, new_points[i].w };
		int pos[4] = { 0, 1, 2, 3 };

		for (int k = 0; k < 4; k++)
		{
			int max = k;
			for (int l = k + 1; l < 4; l++)
				if (abs(coords[l]) > abs(coords[max]))
					max = l;
			swap(coords[k], coords[max]);
			swap(pos[k], pos[max]);
		}

		int best_fit_pos = 0;
		RealNumber best_fit = 100;
		for (int j = 0; j < 7; j++)
		{
			RealNumber fit = 0;
			for (int k = 0; k < 4; k++)
				fit += (possible_4[j][k] - abs(coords[k]))*(possible_4[j][k] - abs(coords[k]));
			if (fit < best_fit)
			{
				best_fit = fit;
				best_fit_pos = j;
			}
		}

		double coords_precise[4];
		for (int j = 0; j < 4; j++)
			coords_precise[pos[j]] = possible_4[best_fit_pos][j] * (coords[j] < 0 ? -1 : 1);

		new_points[i] = Point4D(coords_precise[0], coords_precise[1], coords_precise[2], coords_precise[3]);

		fitted_in[best_fit_pos]++;
	}

	//restoring original order
	for (int i = 0; i < new_points.size(); i++)
		for (int j = i + 1; j < new_points.size(); j++)
			if (new_points[i] == new_points[j])
				int a = 3;

	vector<Point4D> points_final = vector<Point4D>(new_points.size());
	for (int i = 0; i < new_points.size(); i++)
		points_final[vert_map[i]] = new_points[i];

	//faces
	vector<Shape4D::Face> fac;
	for (int i = 0; i < faces.size(); i++)
			fac.push_back(faces[i]->get_indices_cycle());

	//cells
	vector<Shape4D::Cell> new_cells;
	for (int i = 0; i < 120; i++)
	{
		Scene3D cell = Scene3D::load_scene((L"data/4D/120_" + to_wstring(i + 1) + L".obj").c_str());
		set<int> cell_faces_indices;
		auto cell_faces = cell.objects[0].shape.get_faces();
		for (int j = 0; j < cell_faces.size(); j++)
		{
			Shape4D::Face f = Shape4D::Face(cell_faces[j]->get_indices_cycle());
			for (int k = 0; k < faces.size(); k++)
				if (Shape4D::Face(faces[k]->get_indices_cycle()) == f)
				{
					cell_faces_indices.insert(k);
					break;
				}
		}
		new_cells.push_back(Shape4D::Cell(cell_faces_indices));
	}

	int a = 22;
	int b = 3;

	return Scene4D(Scene4D::Object("mega", Shape4D(points_final, fac, new_cells)));
}

Scene4D Scene4D::generate_8_cell()
{
	Scene4D main = load_scene(L"data/4D/tesseract.obj");
	auto main_faces = main.objects[0].shape.get_faces();
	for (int i = 0; i < 8; i++)
	{
		Scene3D cell = Scene3D::load_scene((L"data/4D/tesseract_" + to_wstring(i + 1) + L".obj").c_str());
		vector<int> faces;
		auto cell_faces = cell.objects[0].shape.get_faces();
		for (int j = 0; j < cell_faces.size(); j++)
		{
			Shape4D::Face f = Shape4D::Face(cell_faces[j]->get_indices_cycle());
			for (int k = 0; k < main_faces.size(); k++)
				if (*main_faces[k] == f)
				{
					faces.push_back(k + 1);
					break;
				}
		}
		int a = 2;
		a += 2;
	}

	return Scene4D();
}

Scene4D Scene4D::generate_600_cell()
{
	Shape4D cell120 = Scene4D::load_scene(L"data/4D/120-cell.obj").objects[0].shape;

	vector<Point4D> vertices;
	vector<Shape4D::Cell*> cells_120 = cell120.get_cells();
	for (int i = 0; i < cells_120.size(); i++)
	{
		auto cell_vertices = cell120.vertices_of_cell(cells_120[i]);
		Point4D center;
		for (auto it = cell_vertices.begin(); it != cell_vertices.end(); it++)
			center += **it;
		center /= cell_vertices.size();
		vertices.push_back(center);
	}

	vector<Shape4D::Face> faces;
	vector<Shape4D::Edge*> edges_120 = cell120.get_edges();
	for (int i = 0; i < edges_120.size(); i++)
	{
		auto cells_of_edge = cell120.cells_with_edge(edges_120[i]);
		vector<int> indices;
		for (auto it = cells_of_edge.begin(); it != cells_of_edge.end(); it++)
			indices.push_back(std::find(cells_120.begin(), cells_120.end(), *it) - cells_120.begin());
		faces.push_back(Shape4D::Face(indices));
	}

	vector<Shape4D::Cell> cells;
	vector<Point4D*> vertices_120 = cell120.get_vertices();
	for (int i = 0; i < vertices_120.size(); i++)
	{
		auto edges_of_vert = cell120.edges_with_vertex(vertices_120[i]);
		set<int> indices;
		for (auto it = edges_of_vert.begin(); it != edges_of_vert.end(); it++)
			indices.insert(std::find(edges_120.begin(), edges_120.end(), *it) - edges_120.begin());
		cells.push_back(Shape4D::Cell(indices));
	}

	Shape4D cell600 = Shape4D(vertices, faces, cells);
	return Scene4D(Scene4D::Object("600cell", cell600));
}

Scene4D Scene4D::generate_cross()
{
	Shape4D hypercube = Scene4D::load_scene(L"data/4D/8-cell.obj").objects[0].shape;
	auto the_cells = hypercube.get_cells();
	vector<Point4D> cells_centers;
	for (int i = 0; i < the_cells.size(); i++)
	{
		cells_centers.push_back(Point4D());
		auto vertices = hypercube.vertices_of_cell(the_cells[i]);
		for (auto it = vertices.begin(); it != vertices.end(); it++)
			cells_centers[i] += **it;
		cells_centers[i] /= vertices.size();
	}

	int min_x = 0, max_x = 0, min_y = 0, max_y = 0, min_z = 0, max_z = 0, min_w = 0, max_w = 0;
	for (int i = 0; i < cells_centers.size(); i++)
	{
		if (cells_centers[i].x < cells_centers[min_x].x)
			min_x = i;
		if (cells_centers[i].x > cells_centers[max_x].x)
			max_x = i;
		if (cells_centers[i].y < cells_centers[min_y].y)
			min_y = i;
		if (cells_centers[i].y > cells_centers[max_y].y)
			max_y = i;
		if (cells_centers[i].z < cells_centers[min_z].z)
			min_z = i;
		if (cells_centers[i].z > cells_centers[max_z].z)
			max_z = i;
		if (cells_centers[i].w < cells_centers[min_w].w)
			min_w = i;
		if (cells_centers[i].w > cells_centers[max_w].w)
			max_w = i;
	}

	auto shape_vertices = hypercube.get_vertices();

	set<Point4D*> x_plus_verts = hypercube.vertices_of_cell(the_cells[max_x]);
	set<Point4D*> x_minus_verts = hypercube.vertices_of_cell(the_cells[min_x]);
	set<Point4D*> y_plus_verts = hypercube.vertices_of_cell(the_cells[max_y]);
	set<Point4D*> y_minus_verts = hypercube.vertices_of_cell(the_cells[min_y]);
	set<Point4D*> z_plus_verts = hypercube.vertices_of_cell(the_cells[max_z]);
	set<Point4D*> z_minus_verts = hypercube.vertices_of_cell(the_cells[min_z]);
	set<Point4D*> w_plus_verts = hypercube.vertices_of_cell(the_cells[max_w]);
	set<Point4D*> w_minus_verts = hypercube.vertices_of_cell(the_cells[min_w]);

	vector<int> x_plus_indices;
	for (auto it = x_plus_verts.begin(); it != x_plus_verts.end(); it++)
		x_plus_indices.push_back(std::find(shape_vertices.begin(), shape_vertices.end(), *it) - shape_vertices.begin());
	vector<int> x_minus_indices;
	for (auto it = x_minus_verts.begin(); it != x_minus_verts.end(); it++)
		x_minus_indices.push_back(std::find(shape_vertices.begin(), shape_vertices.end(), *it) - shape_vertices.begin());
	vector<int> y_plus_indices;
	for (auto it = y_plus_verts.begin(); it != y_plus_verts.end(); it++)
		y_plus_indices.push_back(std::find(shape_vertices.begin(), shape_vertices.end(), *it) - shape_vertices.begin());
	vector<int> y_minus_indices;
	for (auto it = y_minus_verts.begin(); it != y_minus_verts.end(); it++)
		y_minus_indices.push_back(std::find(shape_vertices.begin(), shape_vertices.end(), *it) - shape_vertices.begin());
	vector<int> z_plus_indices;
	for (auto it = z_plus_verts.begin(); it != z_plus_verts.end(); it++)
		z_plus_indices.push_back(std::find(shape_vertices.begin(), shape_vertices.end(), *it) - shape_vertices.begin());
	vector<int> z_minus_indices;
	for (auto it = z_minus_verts.begin(); it != z_minus_verts.end(); it++)
		z_minus_indices.push_back(std::find(shape_vertices.begin(), shape_vertices.end(), *it) - shape_vertices.begin());
	vector<int> w_plus_indices;
	for (auto it = w_plus_verts.begin(); it != w_plus_verts.end(); it++)
		w_plus_indices.push_back(std::find(shape_vertices.begin(), shape_vertices.end(), *it) - shape_vertices.begin());
	vector<int> w_minus_indices;
	for (auto it = w_minus_verts.begin(); it != w_minus_verts.end(); it++)
		w_minus_indices.push_back(std::find(shape_vertices.begin(), shape_vertices.end(), *it) - shape_vertices.begin());

	vector<int> x_plus = extrude(hypercube, x_plus_indices);
	for (int i = 0; i < x_plus.size(); i++)
		hypercube.get_vertices()[x_plus[i]]->x += 2;

	vector<int> x_minus = extrude(hypercube, x_minus_indices);
	for (int i = 0; i < x_minus.size(); i++)
		hypercube.get_vertices()[x_minus[i]]->x -= 2;

	vector<int> y_plus = extrude(hypercube, y_plus_indices);
	for (int i = 0; i < y_plus.size(); i++)
		hypercube.get_vertices()[y_plus[i]]->y += 2;
	
	vector<int> y_minus = extrude(hypercube, y_minus_indices);
	for (int i = 0; i < y_minus.size(); i++)
		hypercube.get_vertices()[y_minus[i]]->y -= 4;

	vector<int> z_plus = extrude(hypercube, z_plus_indices);
	for (int i = 0; i < z_plus.size(); i++)
		hypercube.get_vertices()[z_plus[i]]->z += 2;

	vector<int> z_minus = extrude(hypercube, z_minus_indices);
	for (int i = 0; i < z_minus.size(); i++)
		hypercube.get_vertices()[z_minus[i]]->z -= 2;
	
	vector<int> w_plus = extrude(hypercube, w_plus_indices);
	for (int i = 0; i < w_plus.size(); i++)
		hypercube.get_vertices()[w_plus[i]]->w += 2;

	vector<int> w_minus = extrude(hypercube, w_minus_indices);
	for (int i = 0; i < w_minus.size(); i++)
		hypercube.get_vertices()[w_minus[i]]->w -= 2;

	return Scene4D(Scene4D::Object("cross", hypercube));
}

vector<int> Scene4D::extrude(Shape4D& shape, vector<int> vertices)
{
	vector<int> extruded;
	vector<Point4D*> shape_vertices = shape.get_vertices();
	vector<Shape4D::Edge*> shape_edges = shape.get_edges();
	vector<Shape4D::Face*> shape_faces = shape.get_faces();
	vector<Shape4D::Cell*> shape_cells = shape.get_cells();

	map<int, int> old_to_extruded_v;
	map<int, int> old_to_extruded_f;

	vector<Point4D> new_vertices;
	for (int i = 0; i < shape_vertices.size(); i++)
		new_vertices.push_back(*shape_vertices[i]);

	for (int i = 0; i < vertices.size(); i++)
	{
		new_vertices.push_back(*shape_vertices[vertices[i]]);
		extruded.push_back(new_vertices.size() - 1);
		old_to_extruded_v.insert(make_pair(vertices[i], new_vertices.size() - 1));
	}

	vector<Shape4D::Face> new_faces;
	for (int i = 0; i < shape_faces.size(); i++)
		new_faces.push_back(*shape_faces[i]);

	vector<Shape4D::Cell> new_cells;
	for (int i = 0; i < shape_cells.size(); i++)
		new_cells.push_back(*shape_cells[i]);

	for (int i = 0; i < shape_edges.size(); i++)
		if (std::find(vertices.begin(), vertices.end(), shape_edges[i]->first) != vertices.end() &&
			std::find(vertices.begin(), vertices.end(), shape_edges[i]->second) != vertices.end())
		{
			new_faces.push_back(Shape4D::Face(vector<int>{shape_edges[i]->first, shape_edges[i]->second, 
				old_to_extruded_v[shape_edges[i]->second], old_to_extruded_v[shape_edges[i]->first]}));
		}

	for (int i = 0; i < shape_faces.size(); i++)
	{
		bool is_face_extruded = true;
		auto face_indices = shape_faces[i]->get_indices_cycle();
		for (int j = 0; j < face_indices.size() && is_face_extruded; j++)
			if (std::find(vertices.begin(), vertices.end(), face_indices[j]) == vertices.end())
				is_face_extruded = false;
		if (is_face_extruded)
		{
			vector<int> extruded_face;
			for (int j = 0; j < face_indices.size(); j++)
				extruded_face.push_back(old_to_extruded_v[face_indices[j]]);
			new_faces.push_back(extruded_face);
			old_to_extruded_f.insert(make_pair(i, new_faces.size() - 1));

			set<int> new_cell_faces;
			new_cell_faces.insert(i);
			new_cell_faces.insert(new_faces.size() - 1);
			auto face_edges = shape.edges_cycle_of_face(shape_faces[i]);
			for (int j = 0; j < face_edges.size(); j++)
			{
				auto edge_face = Shape4D::Face(vector<int>{face_edges[j]->first, face_edges[j]->second,
					old_to_extruded_v[face_edges[j]->second], old_to_extruded_v[face_edges[j]->first]});
				new_cell_faces.insert(std::find(new_faces.begin(), new_faces.end(), edge_face) - new_faces.begin());
			}
			new_cells.push_back(new_cell_faces);
		}
	}

	for (int i = 0; i < shape_cells.size(); i++)
	{
		bool is_cell_extruded = true;
		auto cell_faces = shape_cells[i]->get_faces();
		for (auto it = cell_faces.begin(); it != cell_faces.end(); it++)
		{
			auto cell_face_indices = shape_faces[*it]->get_indices_cycle();
			for (int j = 0; j < cell_face_indices.size() && is_cell_extruded; j++)
				if (std::find(vertices.begin(), vertices.end(), cell_face_indices[j]) == vertices.end())
					is_cell_extruded = false;
		}
		if (is_cell_extruded)
		{
			new_cells.erase(new_cells.begin() + i);
			set<int> new_cell_faces;
			for (auto it = cell_faces.begin(); it != cell_faces.end(); it++)
				new_cell_faces.insert(old_to_extruded_f[*it]);
			new_cells.push_back(new_cell_faces);
		}
	}

	shape = Shape4D(new_vertices, new_faces, new_cells);
	return extruded;
}