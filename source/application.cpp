#include "application.h"
#include "geometry_master.h"

Application::Application(Mode mode)
{
	ready = false;
	directx_2d = directx_3d = directx_4d_left = directx_4d_right = directx_big_2d = directx_big_3d = NULL;

	this->mode = mode;
	this->display_mode = 3;
	this->fullscreen = false;
	this->input = new Input();
	this->gui = new GUI(1400, 750, fullscreen, input, this);
	this->gui->update_radiogroup(display_mode);
	this->gui->update_axis(show_axis = false);
	this->gui->update_material_i(intersection_opacity = 0.8);
	this->gui->update_material_p(projection_opacity = 0.35);
	this->gui->update_wireframe_i(intersection_wireframe = false);
	this->gui->update_wireframe_p(projection_wireframe = false);

	this->camera_left = new Camera();
	camera_left_distance = 10;
	camera_left->set_position(0, 0, -camera_left_distance);
	this->camera_right = new Camera();
	camera_right_distance = 10;
	camera_right->set_position(0, 0, -camera_left_distance);

	this->resize(1400, 750);
	this->end = false;

	reset_parameters();
	load_scene_3d(L"data/3D/cube.obj");
	load_scene_4d(L"data/4D/8-cell.obj4");

	animate = true;

	ready = true;
}

Application::~Application()
{
	delete directx_2d;
	delete directx_3d;
	delete directx_4d_left;
	delete directx_4d_right;
	delete directx_big_3d;
	delete directx_big_2d;
	delete gui;
	delete camera_left;
	delete camera_right;
	delete input;
}

void Application::set_mode(Mode mode)
{
	this->mode = mode;
	mode == MODE_3D ? update_view_3d() : update_view_4d();
}

void Application::update()
{
	gui->update();
	input->update();

	if (input->is_key_down(VK_ESCAPE))
		end = true;

	if (animate)
	{
		/*d_4d += 0.01;
		if (d_4d > 2.5)
			d_4d = -2.5;

		d_3d += 0.01;
		if (d_3d > 2)
			d_3d = -2;*/

		//rY += 1;
		//rX += 0.2;

		//projection_rYZ += 0.5;

		//recalculate_projections_4d();
		//recalculate_intersections_3d();
		//recalculate_intersections_4d();

		camera_left->rotation.y += 0.01;
		camera_left->rotation.x = GeometryMaster::PI.evaluate()/2 * 0.5 * sin(camera_left->rotation.y/GeometryMaster::PI.evaluate() * 0.5);
	
		camera_right->rotation.y += 0.01;
		camera_right->rotation.x = GeometryMaster::PI.evaluate() / 2 * 0.5 * sin(camera_right->rotation.y / GeometryMaster::PI.evaluate() * 0.5);
	}

	int small_canvas_w, small_canvas_h;
	gui->get_2d_render_size(small_canvas_w, small_canvas_h);

	if (display_mode == 1 || (display_mode == 3 && input->mouse_x < small_canvas_w))
	{
		if (input->mouse_dragged())
		{
			animate = false;
			camera_left->rotation.x = clip(camera_left->rotation.x + input->mouse_dy*0.005, -GeometryMaster::PI.evaluate() / 2, GeometryMaster::PI.evaluate() / 2);
			camera_left->rotation.y += input->mouse_dx*0.005;
		}

		if (input->scroll_delta)
		{
			camera_left_distance *= pow(0.9, input->scroll_delta);
			recalculate_axis_4d();
			mode == MODE_3D ? update_view_3d() : update_view_4d();
		}
	}
	if (display_mode == 2 || (display_mode == 3 && input->mouse_x > small_canvas_w))
	{
		if (input->mouse_dragged())
		{
			animate = false;
			camera_right->rotation.x = clip(camera_right->rotation.x + input->mouse_dy*0.005, -GeometryMaster::PI.evaluate() / 2, GeometryMaster::PI.evaluate() / 2);
			camera_right->rotation.y += input->mouse_dx*0.005;
		}

		if (input->scroll_delta)
		{
			camera_right_distance *= pow(0.9, input->scroll_delta);
			recalculate_axis_4d();
			mode == MODE_3D ? update_view_3d() : update_view_4d();
		}
	}

	camera_left->position.x = -camera_left_distance * cos(camera_left->rotation.x)*sin(camera_left->rotation.y);
	camera_left->position.y = camera_left_distance * sin(camera_left->rotation.x);
	camera_left->position.z = -camera_left_distance * cos(camera_left->rotation.x)*cos(camera_left->rotation.y);

	camera_right->position.x = -camera_right_distance * cos(camera_right->rotation.x)*sin(camera_right->rotation.y);
	camera_right->position.y = camera_right_distance * sin(camera_right->rotation.x);
	camera_right->position.z = -camera_right_distance * cos(camera_right->rotation.x)*cos(camera_right->rotation.y);

	if (mode == MODE_3D)
	{
		if (display_mode == 1)
			directx_big_3d->render();
		if (display_mode == 2)
			directx_big_2d->render();
		if (display_mode == 3)
		{
			directx_3d->render();
			directx_2d->render();
		}
	}
	else
	{
		if (display_mode == 1 || display_mode == 2)
			directx_big_3d->render();
		if (display_mode == 3)
		{
			directx_4d_left->render();
			directx_4d_right->render();
		}
	}
}

void Application::play_animation()
{
	animate = true;
}

void Application::load_scene_3d(const wchar_t* file)
{
	this->scene_3d = Scene3D::load_scene(file);

	if (scene_3d.error_info != "")
		gui->show_error(scene_3d.error_info);

	for (int i = 0; i < scene_3d.objects.size(); i++)
	{
		if (!scene_3d.objects[i].shape.is_closed())
		{
			gui->show_error(scene_3d.objects[i].name + " is not a closed shape. Aborting calculations.");
			scene_3d.objects[i].corrupted = true;
		}
		else 
		{
			if (!scene_3d.objects[i].shape.has_valid_faces())
				gui->show_warning("Vertices of " + scene_3d.objects[i].name + " face don't lie in one plane. Calculations may return inaccurate results.");
			if (scene_3d.objects[i].shape.has_concave_faces())
				gui->show_warning("Object " + scene_3d.objects[i].name + " contains faces being concave polygons. Please make sure they do not occur as their presence is unsupported.");
		}
	}

	recalculate_intersections_3d();
}

void Application::load_scene_4d(const wchar_t* file)
{
	//Scene4D::save_scene(&Scene4D::generate_cross(), L"cross.obj");

	this->scene_4d = Scene4D::load_scene(file);
	intersections_4_to_4 = vector<Shape4D>(scene_4d.objects.size());
	intersections_4_to_3 = vector<Shape3D>(scene_4d.objects.size());
	projections_4d = vector<Shape3D>(scene_4d.objects.size());
	projections_of_intersections_4d = vector<Shape3D>(scene_4d.objects.size());

	if (scene_4d.error_info != "")
		gui->show_error(scene_4d.error_info);

	for (int i = 0; i < scene_4d.objects.size(); i++)
	{
		if (!scene_4d.objects[i].shape.is_closed())
		{
			gui->show_error(scene_4d.objects[i].name + " is not a closed shape. Aborting calculations.");
			scene_4d.objects[i].corrupted = true;
		}
		else if (!scene_4d.objects[i].shape.has_valid_cells())
			gui->show_warning("Vertices of " + scene_4d.objects[i].name + " cell don't lie in one space. Calculations may return inaccurate results.");
	}
	recalculate_projections_4d();
	recalculate_intersections_4d();
}

void Application::save_scene_3d(const wchar_t* file)
{
	Scene3D result;
	for (int i = 0; i < scene_3d.objects.size(); i++)
		result.objects.push_back(scene_3d.objects[i]);
	for (int i = 0; i < intersections_3_to_3.size(); i++)
		result.objects.push_back(Scene3D::Object("intersection " + i, intersections_3_to_3[i]));
	for (int i = 0; i < intersections_3_to_2.size(); i++)
	{
		Shape3D flat_as_3d = Shape3D(&intersections_3_to_2[i]);
		flat_as_3d = GeometryMaster::transform(&flat_as_3d, &Matrix3D::translation(4, 0, 0));
		result.objects.push_back(Scene3D::Object("intersection_flat " + i, flat_as_3d));
	}
	Scene3D::save_scene(&result, file);
}

void Application::save_scene_4d(const wchar_t* file)
{
	Scene3D result;
	for (int i = 0; i < projections_4d.size(); i++)
		result.objects.push_back(Scene3D::Object("projection of " + scene_4d.objects[i].name, projections_4d[i]));
	for (int i = 0; i < projections_of_intersections_4d.size(); i++)
		result.objects.push_back(Scene3D::Object("projected intersection " + i, projections_of_intersections_4d[i]));
	for (int i = 0; i < intersections_4_to_3.size(); i++)
		result.objects.push_back(Scene3D::Object("intersection " + i, GeometryMaster::transform(&intersections_4_to_3[i], &Matrix3D::translation(4, 0, 0))));
	Scene3D::save_scene(&result, file);
}

void Application::recalculate_intersections_3d()
{
	intersections_3_to_2.clear();
	intersections_3_to_3.clear();
	convex_intersections_3_to_2.clear();
	convex_intersections_3_to_3.clear();
	for (int i = 0; i < scene_3d.objects.size(); i++)
		if (!scene_3d.objects[i].corrupted)
		{
			vector<Shape2D> intersections_2d_temp;
			vector<Shape3D> intersections_3d_temp = MathEngine::intersect_concave(&scene_3d.objects[i].shape,
				rX / 180 * GeometryMaster::PI, rY / 180 * GeometryMaster::PI, d_3d, intersections_2d_temp);
			for (int i = 0; i < intersections_3d_temp.size(); i++)
				intersections_3_to_3.push_back(intersections_3d_temp[i]);
			for (int i = 0; i < intersections_2d_temp.size(); i++)
				intersections_3_to_2.push_back(intersections_2d_temp[i]);

			if (scene_3d.objects[i].shape.is_convex())
			{
				for (int i = 0; i < intersections_3d_temp.size(); i++)
					convex_intersections_3_to_3.push_back(intersections_3d_temp[i]);
				for (int i = 0; i < intersections_2d_temp.size(); i++)
					convex_intersections_3_to_2.push_back(intersections_2d_temp[i]);
			}
			else
			{
				for (int i = 0; i < intersections_2d_temp.size(); i++)
				{
					auto triangulated = MathEngine::triangulate(&intersections_2d_temp[i]);
					vector<Shape3D> triangles_3d;
					for (int j = 0; j < triangulated.size(); j++)
					{
						convex_intersections_3_to_2.push_back(triangulated[j]);
						triangles_3d.push_back(GeometryMaster::transform(&Shape3D(&triangulated[j]),
							&MathEngine::rotation_transformation(rX / 180 * GeometryMaster::PI, rY / 180 * GeometryMaster::PI, d_3d)));
					}
					vector<Point3D> triangles_points;
					vector<Shape3D::Face> triangles_3d_as_faces;
					for (int j = 0; j < triangles_3d.size(); j++)
					{
						auto triangle_vertices = triangles_3d[j].get_vertices();
						vector<int> face_indices;
						for (int k = 0; k < 3; k++)
						{
							int vert_position = std::find(triangles_points.begin(), triangles_points.end(), *triangle_vertices[k]) - triangles_points.begin();
							face_indices.push_back(vert_position);
							if (vert_position == triangles_points.size())
								triangles_points.push_back(*triangle_vertices[k]);
						}
						triangles_3d_as_faces.push_back(Shape3D::Face(face_indices));
					}

					convex_intersections_3_to_3.push_back(Shape3D(triangles_points, triangles_3d_as_faces));
				}
			}
		}
	update_view_3d();
}

void Application::recalculate_intersections_4d()
{
	for (int i = 0; i < scene_4d.objects.size(); i++)
		if (!scene_4d.objects[i].corrupted)
		{
			intersections_4_to_4[i] = MathEngine::intersect_concave(&scene_4d.objects[i].shape,
				rXZ / 180 * GeometryMaster::PI, rYZ / 180 * GeometryMaster::PI, rXY / 180 * GeometryMaster::PI, d_4d, intersections_4_to_3[i]);

			projections_of_intersections_4d[i] = MathEngine::project(&intersections_4_to_4[i],
				projection_rXZ / 180 * GeometryMaster::PI, projection_rYZ / 180 * GeometryMaster::PI, projection_rXY / 180 * GeometryMaster::PI, projection_d_4d);
			projections_of_intersections_4d[i].recalculate_normals();
		}
	update_view_4d();
}

void Application::recalculate_projections_4d()
{
	for (int i = 0; i < scene_4d.objects.size(); i++)
		if (!scene_4d.objects[i].corrupted)
		{
			projections_4d[i] = MathEngine::project(&scene_4d.objects[i].shape,
				projection_rXZ / 180 * GeometryMaster::PI, projection_rYZ / 180 * GeometryMaster::PI, projection_rXY / 180 * GeometryMaster::PI, projection_d_4d);

			projections_of_intersections_4d[i] = MathEngine::project(&intersections_4_to_4[i],
				projection_rXZ / 180 * GeometryMaster::PI, projection_rYZ / 180 * GeometryMaster::PI, projection_rXY / 180 * GeometryMaster::PI, projection_d_4d);
			projections_of_intersections_4d[i].recalculate_normals();
		}

	recalculate_axis_4d();
	update_view_4d();
}

void Application::set_parameters_3d(double A, double B, double C, double D)
{
	if (!ready) return;
	params_3d_to_rotation(A, B, C, D, rX, rY, d_3d);
	recalculate_intersections_3d();
}

void Application::set_parameters_4d(double A, double B, double C, double D, double E)
{
	if (!ready) return;
	params_4d_to_rotation(A, B, C, D, E, rXZ, rYZ, rXY, d_4d);
	recalculate_intersections_4d();
}

void Application::set_rotation_3d(double rX, double rY, double d)
{
	if (!ready) return;
	this->rX = rX;
	this->rY = rY;
	this->d_3d = d;
	recalculate_intersections_3d();
}

void Application::set_rotation_4d(double rXZ, double rYZ, double rXY, double d)
{
	if (!ready) return;
	this->rXZ = rXZ;
	this->rYZ = rYZ;
	this->rXY = rXY;
	this->d_4d = d;
	recalculate_intersections_4d();
}

void Application::set_projection_rotation_4d(double rXZ, double rYZ, double rXY, double d)
{
	if (!ready) return;
	this->projection_rXZ = rXZ;
	this->projection_rYZ = rYZ;
	this->projection_rXY = rXY;
	this->projection_d_4d = d;
	recalculate_projections_4d();
}

void Application::params_3d_to_rotation(double A, double B, double C, double D, double& rX, double& rY, double& d)
{
	RealNumber realX, realY, realD;
	MathEngine::get_rotation_params(&Plane3D(A, B, C, D), realX, realY, realD);
	rX = realX.evaluate()*180 / GeometryMaster::PI.evaluate();
	rY = realY.evaluate()*180 / GeometryMaster::PI.evaluate();
	d = realD.evaluate();
	while (rY < 0)
		rY += 360;
	while (rY > 360)
		rY -= 360;
}

void Application::set_projection_opacity(double value)
{
	if (!ready) return;
	projection_opacity = value;
	mode == MODE_3D ? update_view_3d() : update_view_4d();
}

void Application::set_intersection_opacity(double value)
{
	if (!ready) return;
	intersection_opacity = value;
	mode == MODE_3D ? update_view_3d() : update_view_4d();
}

void Application::set_projection_wireframe(bool value)
{
	if (!ready) return;
	projection_wireframe = value;
	mode == MODE_3D ? update_view_3d() : update_view_4d();
}

void Application::set_intersection_wireframe(bool value)
{
	if (!ready) return;
	intersection_wireframe = value;
	mode == MODE_3D ? update_view_3d() : update_view_4d();
}

void Application::set_axis(bool value)
{
	if (!ready) return;
	show_axis = value;
	mode == MODE_3D ? update_view_3d() : update_view_4d();
}

void Application::set_display_mode(int nr)
{
	if (!ready) return;
	display_mode = nr;
	if (display_mode == 1)
		directx_big_3d->set_camera(camera_left);
	if (display_mode == 2)
		directx_big_3d->set_camera(camera_right);
	recalculate_axis_4d();
	mode == MODE_3D ? update_view_3d() : update_view_4d();
}

void Application::reset_parameters()
{
	double A, B, C, D, E;
	A = 0, B = 0, C = -1, D = 0;
	params_3d_to_rotation(A, B, C, D, rX, rY, d_3d);
	this->gui->update_parameters_3d(A, B, C, D);

	A = 0, B = 0, C = 0, D = -1, E = 0;
	params_4d_to_rotation(A, B, C, D, E, rXZ, rYZ, rXY, d_4d);
	projection_rXY = 0;
	projection_rXZ = 0;
	projection_rYZ = 0;
	projection_d_4d = 2;
	this->gui->update_parameters_4d(A, B, C, D, E);
	this->gui->update_rotation_4d_p(projection_rXZ, projection_rYZ, projection_rXY, projection_d_4d);
}

void Application::params_4d_to_rotation(double A, double B, double C, double D, double E, double& rX, double& rY, double &rZ, double& d)
{
	RealNumber realX, realY, realZ, realD;
	MathEngine::get_rotation_params(&Space4D(A, B, C, D, E), realX, realY, realZ, realD);
	rX = realX.evaluate()*180 / GeometryMaster::PI.evaluate();
	rY = realY.evaluate()*180 / GeometryMaster::PI.evaluate();
	rZ = realZ.evaluate()*180 / GeometryMaster::PI.evaluate();
	d = realD.evaluate();
}

void Application::rotation_3d_to_params(double rX, double rY, double d, double& A, double& B, double& C, double& D)
{
	RealNumber RealA, RealB, RealC, RealD;
	GeometryMaster::transform(&Plane3D(0, 0, 1, 0),	
		&MathEngine::rotation_transformation(rX*GeometryMaster::PI / 180, rY*GeometryMaster::PI / 180, d)).get_parameters(RealA, RealB, RealC, RealD);
	A = RealA.evaluate();
	B = RealB.evaluate();
	C = RealC.evaluate();
	D = RealD.evaluate();
}

void Application::rotation_4d_to_params(double rX, double rY, double rZ, double d, double& A, double& B, double& C, double& D, double& E)
{
	RealNumber RealA, RealB, RealC, RealD, RealE;
	GeometryMaster::transform(&Space4D(0, 0, 0, 1, 0),
		&MathEngine::rotation_transformation(rX*GeometryMaster::PI / 180, rY*GeometryMaster::PI / 180, rZ*GeometryMaster::PI / 180, d)).
		get_parameters(RealA, RealB, RealC, RealD, RealE);
	A = RealA.evaluate();
	B = RealB.evaluate();
	C = RealC.evaluate();
	D = RealD.evaluate();
	E = RealE.evaluate();
}

void Application::resize(int width, int height)
{
	SafeDelete(this->directx_2d);
	SafeDelete(this->directx_3d);
	SafeDelete(this->directx_4d_left);
	SafeDelete(this->directx_4d_right);
	SafeDelete(this->directx_big_3d);
	SafeDelete(this->directx_big_2d);

	int render_window_width, render_window_height;
	this->gui->get_3d_render_size(render_window_width, render_window_height);
	this->directx_3d = new DirectX(render_window_width, render_window_height, this->gui->get_render_3d_window(), fullscreen, DirectX::MODE_3D);
	this->gui->get_2d_render_size(render_window_width, render_window_height);
	this->directx_2d = new DirectX(render_window_width, render_window_height, this->gui->get_render_2d_window(), fullscreen, DirectX::MODE_2D);

	this->gui->get_4d_render_size(render_window_width, render_window_height);
	this->directx_4d_left = new DirectX(render_window_width, render_window_height, this->gui->get_render_4d_projection_window(), fullscreen, DirectX::MODE_3D);
	this->gui->get_4d_render_size(render_window_width, render_window_height);
	this->directx_4d_right = new DirectX(render_window_width, render_window_height, this->gui->get_render_4d_intersection_window(), fullscreen, DirectX::MODE_3D);
	
	this->gui->get_big_render_size(render_window_width, render_window_height);
	this->directx_big_2d = new DirectX(render_window_width, render_window_height, this->gui->get_big_2d_window(), fullscreen, DirectX::MODE_2D);
	this->directx_big_3d = new DirectX(render_window_width, render_window_height, this->gui->get_big_3d_window(), fullscreen, DirectX::MODE_3D);

	directx_2d->set_camera(camera_right);
	directx_3d->set_camera(camera_left);
	directx_4d_left->set_camera(camera_left);
	directx_4d_right->set_camera(camera_right);
	directx_big_3d->set_camera(display_mode == 2 ? camera_right : camera_left);
	directx_big_2d->set_camera(camera_right);

	mode == MODE_3D ? update_view_3d() : update_view_4d();
}

void Application::update_view_3d()
{
	if (display_mode == 1)
	{
		directx_big_3d->reset_scene();
		directx_big_3d->set_ambient_color(Color(0.05, 0.1, 0.2));

		if (show_axis)
			place_axis(directx_big_3d, camera_left_distance);
		for (int i = 0; i < scene_3d.objects.size(); i++)
			if (projection_wireframe)
				directx_big_3d->place_wireframe(&scene_3d.objects[i].shape, Material(MaterialType::SELF_COLOR, Color(0, 0.9, 1, projection_opacity)));
			else
				directx_big_3d->place_shape(&scene_3d.objects[i].shape, Material(MaterialType::NORMAL, Color(0, 0.9, 1, projection_opacity)),
					scene_3d.objects[i].shape.is_convex());
	}
	if (display_mode == 2)
	{
		directx_big_2d->reset_scene();
		directx_big_2d->set_ambient_color(Color(0, 0, 0));

		if (intersection_wireframe)
			for (int i = 0; i < intersections_3_to_2.size(); i++)
				directx_big_2d->place_wireframe(&intersections_3_to_2[i], Material(MaterialType::SELF_COLOR, Color(1, 0.15, 0.2, 1)));
		else
			for (int i = 0; i < convex_intersections_3_to_2.size(); i++)
				directx_big_2d->place_shape(&convex_intersections_3_to_2[i], Material(MaterialType::SELF_COLOR, Color(1, 0.15, 0.2, 1)));
	}
	if (display_mode == 3)
	{
		directx_2d->reset_scene();
		directx_3d->reset_scene();
		directx_2d->set_ambient_color(Color(0, 0, 0));
		directx_3d->set_ambient_color(Color(0.05, 0.1, 0.2));
		if (show_axis)
			place_axis(directx_3d, camera_left_distance);
		for (int i = 0; i < scene_3d.objects.size(); i++)
			if (projection_wireframe)
				directx_3d->place_wireframe(&scene_3d.objects[i].shape, Material(MaterialType::SELF_COLOR, Color(0, 0.9, 1, projection_opacity)));
			else
				directx_3d->place_shape(&scene_3d.objects[i].shape, Material(MaterialType::NORMAL, Color(0, 0.9, 1, projection_opacity)), 
					scene_3d.objects[i].shape.is_convex());

		for (int i = 0; i < intersections_3_to_3.size(); i++)
			if (intersection_wireframe)
				directx_3d->place_wireframe(&intersections_3_to_3[i], Material(MaterialType::SELF_COLOR, Color(0.9, 0.1, 0.2, intersection_opacity)));
			else
				directx_3d->place_inner_shape(&convex_intersections_3_to_3[i], Material(MaterialType::SELF_COLOR, Color(0.9, 0.1, 0.2, intersection_opacity)), true);

		if (intersection_wireframe)
			for (int i = 0; i < intersections_3_to_2.size(); i++)
				directx_2d->place_wireframe(&intersections_3_to_2[i], Material(MaterialType::SELF_COLOR, Color(1, 0.15, 0.2, 1)));
		else
			for (int i = 0; i < convex_intersections_3_to_2.size(); i++)
				directx_2d->place_shape(&convex_intersections_3_to_2[i], Material(MaterialType::SELF_COLOR, Color(1, 0.15, 0.2, 1)));
	}
}

void Application::update_view_4d()
{
	if (display_mode == 1)
	{
		directx_big_3d->reset_scene();
		directx_big_3d->set_ambient_color(Color(0.05, 0.1, 0.2));
		if (show_axis)
			place_axis_4d(directx_big_3d);

		for (int i = 0; i < projections_4d.size(); i++)
			if (projection_wireframe)
				directx_big_3d->place_wireframe(&projections_4d[i], Material(MaterialType::SELF_COLOR, Color(0, 0.9, 1, projection_opacity)));
			else
				directx_big_3d->place_shape(&projections_4d[i], Material(MaterialType::SELF_COLOR, Color(0, 0.9, 1, projection_opacity)), true);
	}
	if (display_mode == 2)
	{
		directx_big_3d->reset_scene();
		directx_big_3d->set_ambient_color(Color(0.1, 0, 0));
		if (show_axis)
			place_axis(directx_big_3d, camera_right_distance);
		for (int i = 0; i < intersections_4_to_3.size(); i++)
			if (intersection_wireframe)
				directx_big_3d->place_wireframe(&intersections_4_to_3[i], Material(MaterialType::SELF_COLOR, Color(1, 0.1, 0.2, intersection_opacity)));
			else
				directx_big_3d->place_shape(&intersections_4_to_3[i], Material(MaterialType::NORMAL, Color(1, 0.1, 0.2, intersection_opacity)),
					intersections_4_to_3[i].is_convex());
	}
	if (display_mode == 3)
	{
		directx_4d_left->reset_scene();
		directx_4d_right->reset_scene();
		directx_4d_left->set_ambient_color(Color(0.05, 0.1, 0.2));
		directx_4d_right->set_ambient_color(Color(0.1, 0, 0));
		if (show_axis)
		{
			place_axis_4d(directx_4d_left);
			place_axis(directx_4d_right, camera_right_distance);
		}
		for (int i = 0; i < intersections_4_to_3.size(); i++)
			if (intersection_wireframe)
				directx_4d_right->place_wireframe(&intersections_4_to_3[i], Material(MaterialType::SELF_COLOR, Color(1, 0.1, 0.2, intersection_opacity)));
			else
				directx_4d_right->place_shape(&intersections_4_to_3[i], Material(MaterialType::NORMAL, Color(1, 0.1, 0.2, intersection_opacity)),
					intersections_4_to_3[i].is_convex());

		for (int i = 0; i < projections_4d.size(); i++)
			if (projection_wireframe)
				directx_4d_left->place_wireframe(&projections_4d[i], Material(MaterialType::SELF_COLOR, Color(0, 0.9, 1, projection_opacity)));
			else
				directx_4d_left->place_shape(&projections_4d[i], Material(MaterialType::SELF_COLOR, Color(0, 0.9, 1, projection_opacity)), true);

		for (int i = 0; i < projections_of_intersections_4d.size(); i++)
			if (intersection_wireframe)
				directx_4d_left->place_wireframe(&projections_of_intersections_4d[i], Material(MaterialType::SELF_COLOR, Color(1, 0.1, 0.2, intersection_opacity)));
			else
				directx_4d_left->place_inner_shape(&projections_of_intersections_4d[i], Material(MaterialType::NORMAL, Color(1, 0.1, 0.2, intersection_opacity)),
					intersections_4_to_3[i].is_convex());
	}
}

void Application::place_axis(DirectX* directx, double camera_distance)
{
	double scale = display_mode == 3 ? camera_distance*0.24 : camera_distance*0.32;
	double h = scale*0.07;
	double w = scale*0.04;
	Material m = Material(MaterialType::SELF_COLOR, Color(1, 1, 1, 0.4));
	
	//X axis
	directx->place_line(&Segment3D(&Point3D(-scale, 0, 0), &Point3D(scale, 0, 0)), m);

	//X shape
	directx->place_line(&Segment3D(&Point3D(scale - w/2, h/2 + h, 0), &Point3D(scale + w/2, -h/2 + h, 0)), m);
	directx->place_line(&Segment3D(&Point3D(scale - w/2, -h/2 + h, 0), &Point3D(scale + w/2, h/2 + h, 0)), m);

	//Y axis
	directx->place_line(&Segment3D(&Point3D(0, -scale, 0), &Point3D(0, scale, 0)), m);

	//Y shape
	directx->place_line(&Segment3D(&Point3D(w, scale - h / 2, 0), &Point3D(w, scale, 0)), m);
	directx->place_line(&Segment3D(&Point3D(-w/2 + w, scale + h / 2, 0), &Point3D(w, scale, 0)), m);
	directx->place_line(&Segment3D(&Point3D(w/2 + w, scale + h / 2, 0), &Point3D(w, scale, 0)), m);

	//Z axis
	directx->place_line(&Segment3D(&Point3D(0, 0, -scale), &Point3D(0, 0, scale)), m);

	//Z shape
	directx->place_line(&Segment3D(&Point3D(-w / 2 + w, -h / 2 + h, scale), &Point3D(w / 2 + w, -h / 2 + h, scale)), m);
	directx->place_line(&Segment3D(&Point3D(-w / 2 + w, -h / 2 + h, scale), &Point3D(w / 2 + w, h / 2 + h, scale)), m);
	directx->place_line(&Segment3D(&Point3D(-w / 2 + w, h / 2 + h, scale), &Point3D(w / 2 + w, h / 2 + h, scale)), m);
}

void Application::recalculate_axis_4d()
{
	vector<Segment4D> result;

	double scale = display_mode == 3 ? 0.24 : 0.32;
	double h = scale*0.07;
	double w = scale*0.04;

	//X axis
	result.push_back(Segment4D(&Point4D(-scale, 0, 0, 0), &Point4D(scale, 0, 0, 0)));

	//X shape
	result.push_back(Segment4D(&Point4D(scale - w / 2, h / 2 + h, 0, 0), &Point4D(scale + w / 2, -h / 2 + h, 0, 0)));
	result.push_back(Segment4D(&Point4D(scale - w / 2, -h / 2 + h, 0, 0), &Point4D(scale + w / 2, h / 2 + h, 0, 0)));

	//Y axis
	result.push_back(Segment4D(&Point4D(0, -scale, 0, 0), &Point4D(0, scale, 0, 0)));

	//Y shape
	result.push_back(Segment4D(&Point4D(w, scale - h / 2, 0, 0), &Point4D(w, scale, 0, 0)));
	result.push_back(Segment4D(&Point4D(-w / 2 + w, scale + h / 2, 0, 0), &Point4D(w, scale, 0, 0)));
	result.push_back(Segment4D(&Point4D(w / 2 + w, scale + h / 2, 0, 0), &Point4D(w, scale, 0, 0)));

	//Z axis
	result.push_back(Segment4D(&Point4D(0, 0, -scale, 0), &Point4D(0, 0, scale, 0)));

	//Z shape
	result.push_back(Segment4D(&Point4D(-w / 2 + w, -h / 2 + h, scale, 0), &Point4D(w / 2 + w, -h / 2 + h, scale, 0)));
	result.push_back(Segment4D(&Point4D(-w / 2 + w, -h / 2 + h, scale, 0), &Point4D(w / 2 + w, h / 2 + h, scale, 0)));
	result.push_back(Segment4D(&Point4D(-w / 2 + w, h / 2 + h, scale, 0), &Point4D(w / 2 + w, h / 2 + h, scale, 0)));

	//W axis
	result.push_back(Segment4D(&Point4D(0, 0, 0, -scale), &Point4D(0, 0, 0, scale)));

	//W shape
	result.push_back(Segment4D(&Point4D(-w / 2 + w, h / 2 + h, 0, scale), &Point4D(-w / 4 + w, -h / 2 + h, 0, scale)));
	result.push_back(Segment4D(&Point4D(-w / 4 + w, -h / 2 + h, 0, scale), &Point4D(0 + w, 0 + h, 0, scale)));
	result.push_back(Segment4D(&Point4D(0 + w, 0 + h, 0, scale), &Point4D(w / 4 + w, -h / 2 + h, 0, scale)));
	result.push_back(Segment4D(&Point4D(w / 4 + w, -h / 2 + h, 0, scale), &Point4D(w / 2 + w, h / 2 + h, 0, scale)));

	projected_axis.clear();
	for (int i = 0; i < result.size(); i++)
	{
		projected_axis.push_back(MathEngine::project(&result[i],
			projection_rXZ / 180 * GeometryMaster::PI, projection_rYZ / 180 * GeometryMaster::PI, projection_rXY / 180 * GeometryMaster::PI, projection_d_4d));
		projected_axis[i] = GeometryMaster::transform(&projected_axis[i], &Matrix3D::scale(camera_left_distance*projection_d_4d));
	}
}

void Application::place_axis_4d(DirectX* directx)
{
	Material m = Material(MaterialType::SELF_COLOR, Color(1, 1, 1, 0.4));

	for (int i = 0; i < projected_axis.size(); i++)
		directx->place_line(&projected_axis[i], m);
}