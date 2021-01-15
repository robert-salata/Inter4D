#include "directx_system.h"
#include <algorithm>

DirectX::DirectX(int screen_width, int screen_height, HWND window, bool fullscreen, DirectX::Mode mode)
{
	directx = new DirectBase(screen_width, screen_height, VSYNC_ENABLED, window, fullscreen, SCREEN_DEPTH, SCREEN_NEAR);
	models = new Models(directx->get_device());
	shaders = new Shaders(directx->get_device(), window, L"vertex.shader", L"pixel.shader");
	this->mode = mode;
}

DirectX::~DirectX()
{
	delete shaders;
	delete models;
	delete directx;
}

void DirectX::render()
{
	if (mode == MODE_3D)
		render_3d();
	else
		render_2d();
}

void DirectX::render_3d()
{
	directx->begin_scene(0.0f, 0.0f, 0.0f, 1.0f);

	camera->render();

	models->render(directx->get_device_context());

	D3DXMATRIX wm, vm, pm;
	camera->get_view_matrix(vm);
	directx->get_world_matrix(wm);
	directx->get_projection_matrix(pm);
	D3DXVECTOR3 light_direction = -camera->position;
	D3DXVec3Normalize(&light_direction, &light_direction);

	auto sorted_shapes = shapes;
	std::sort(sorted_shapes.begin(), sorted_shapes.end(), [this](Models::ModelInfo f1, Models::ModelInfo f2) {
		return D3DXVec3Length(&(f1.center - camera->position)) > D3DXVec3Length(&(f2.center - camera->position));
	});
	auto sorted_inner_shapes = inner_shapes;
	std::sort(sorted_inner_shapes.begin(), sorted_inner_shapes.end(), [this](Models::ModelInfo f1, Models::ModelInfo f2) {
		return D3DXVec3Length(&(f1.center - camera->position)) > D3DXVec3Length(&(f2.center - camera->position));
	});

	for (int i = 0; i < lines.size(); i++)
		display_as_convex(lines[i], wm, vm, pm, light_direction);

	for (int i = 0; i < sorted_shapes.size(); i++)
		sorted_shapes[i].is_convex ? display_as_convex(sorted_shapes[i], wm, vm, pm, light_direction) : 
		display_as_concave(sorted_shapes[i], wm, vm, pm, light_direction);
			
	for (int i = 0; i < sorted_inner_shapes.size(); i++)
		sorted_inner_shapes[i].is_convex ? display_as_convex(sorted_inner_shapes[i], wm, vm, pm, light_direction) : 
		display_as_concave(sorted_inner_shapes[i], wm, vm, pm, light_direction);

	directx->end_scene();
}

void DirectX::display_as_convex(Models::ModelInfo model, D3DXMATRIX& wm, D3DXMATRIX& vm, D3DXMATRIX& pm, D3DXVECTOR3& ldirection)
{
	D3DXVECTOR4 ambient = D3DXVECTOR4(ambient_color.r, ambient_color.g, ambient_color.b, ambient_color.a);
	D3DXVECTOR4 light = D3DXVECTOR4(1, 1, 1, 1);

	directx->set_rasterization(DirectBase::RasterizationType::BACK);
	models->render(directx->get_device_context(), model);
	shaders->render(directx->get_device_context(), model.index_start, model.index_count, model.material, wm, vm, pm, ldirection, light, ambient);

	directx->set_rasterization(DirectBase::RasterizationType::FRONT);
	models->render(directx->get_device_context(), model);
	shaders->render(directx->get_device_context(), model.index_start, model.index_count, model.material, wm, vm, pm, ldirection, light, ambient);
}

void DirectX::display_as_concave(Models::ModelInfo model, D3DXMATRIX& wm, D3DXMATRIX& vm, D3DXMATRIX& pm, D3DXVECTOR3& ldirection)
{
	D3DXVECTOR4 ambient = D3DXVECTOR4(ambient_color.r, ambient_color.g, ambient_color.b, ambient_color.a);
	D3DXVECTOR4 light = D3DXVECTOR4(1, 1, 1, 1);

	vector<Models::ModelInfo::Face> sorted_faces = model.faces;
	std::sort(sorted_faces.begin(), sorted_faces.end(), [this](Models::ModelInfo::Face f1, Models::ModelInfo::Face f2) {
		return D3DXVec3Length(&(f1.center - camera->position)) > D3DXVec3Length(&(f2.center - camera->position));
	});

	directx->set_rasterization(DirectBase::RasterizationType::BACK);
	for (int i = 0; i < sorted_faces.size(); i++)
	{
		models->render(directx->get_device_context(), model);
		shaders->render(directx->get_device_context(), sorted_faces[i].index_start, sorted_faces[i].index_count,
			model.material, wm, vm, pm, ldirection, light, ambient);
	}

	directx->set_rasterization(DirectBase::RasterizationType::FRONT);
	for (int i = 0; i < sorted_faces.size(); i++)
	{
		models->render(directx->get_device_context(), model);
		shaders->render(directx->get_device_context(), sorted_faces[i].index_start, sorted_faces[i].index_count,
			model.material, wm, vm, pm, ldirection, light, ambient);
	}
}

void DirectX::render_2d()
{
	directx->begin_scene(0.0f, 0.0f, 0.0f, 1.0f);

	models->render(directx->get_device_context());

	D3DXMATRIX wm, vm, pm;
	D3DXVECTOR3 pos = D3DXVECTOR3(0, 0, -D3DXVec3Length(&camera->position));
	D3DXMatrixLookAtLH(&vm, &pos, &(pos + D3DXVECTOR3(0, 0, 1)), &D3DXVECTOR3(0, 1, 0));
	directx->get_world_matrix(wm);
	directx->get_projection_matrix(pm);
	D3DXVECTOR3 light_direction = D3DXVECTOR3(0, 0, -1);

	directx->set_rasterization(DirectBase::RasterizationType::BOTH);
	for (int i = 0; i < shapes.size(); i++)
		display_as_convex(shapes[i], wm, vm, pm, light_direction);

	directx->end_scene();
}

void DirectX::place_shape(Shape3D* shape, Material material, bool convex)
{
	if (mode == MODE_2D)
		throw "3D object cannot be rendered in 2D mode";
	shapes.push_back(models->load(shape, material));
	shapes[shapes.size() - 1].is_convex = convex;
}

void DirectX::place_shape(Shape2D* shape, Material material)
{
	if (mode == MODE_3D)
		throw "2D object cannot be rendered in 3D mode";
	shapes.push_back(models->load(shape, material));
	shapes[shapes.size() - 1].is_convex = true;
}

void DirectX::place_inner_shape(Shape3D* shape, Material material, bool convex)
{
	if (mode == MODE_2D)
		throw "3D object cannot be rendered in 2D mode";
	inner_shapes.push_back(models->load(shape, material));
	inner_shapes[inner_shapes.size() - 1].is_convex = convex;
}

void DirectX::place_wireframe(Shape3D* shape, Material material)
{
	if (mode == MODE_2D)
		throw "3D object cannot be rendered in 2D mode";
	shapes.push_back(models->load_wireframe(shape, material));
	shapes[shapes.size() - 1].is_convex = true;
}

void DirectX::place_wireframe(Shape2D* shape, Material material)
{
	if (mode == MODE_3D)
		throw "2D object cannot be rendered in 3D mode";
	shapes.push_back(models->load_wireframe(shape, material));
	shapes[shapes.size() - 1].is_convex = true;
}

void DirectX::place_line(Segment3D* line, Material material)
{
	if (mode == MODE_2D)
		throw "3D object cannot be rendered in 2D mode";
	lines.push_back(models->load(line, material));;
	lines[lines.size() - 1].is_convex = true;
}

void DirectX::reset_scene()
{
	shapes.clear();
	inner_shapes.clear();
	lines.clear();
	models->reset();
}

void DirectX::set_camera(Camera* camera)
{
	this->camera = camera;
}

void DirectX::set_ambient_color(Color color)
{
	this->ambient_color = color;
}