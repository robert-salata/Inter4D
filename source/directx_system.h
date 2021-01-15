#ifndef _SYSTEMCLASS_H_
#define _SYSTEMCLASS_H_

#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include "input.h"
#include "directx_base.h"
#include "camera.h"
#include "models.h"
#include "shaders.h"
#include "material.h"
#include "shape3D.h"
#include "segment3D.h"

const bool VSYNC_ENABLED = true;
const float SCREEN_DEPTH = 1000.0f;
const float SCREEN_NEAR = 0.1f;

class DirectX
{
public:
	enum Mode { MODE_3D, MODE_2D };
	DirectX::DirectX(int screen_width, int screen_height, HWND window, bool fullscreen, Mode mode = MODE_3D);
	~DirectX();
	void render();
	void place_shape(Shape3D* shape, Material material, bool convex);
	void place_shape(Shape2D* shape, Material material);
	void place_inner_shape(Shape3D* shape, Material material, bool convex);
	void place_line(Segment3D* line, Material material);
	void place_wireframe(Shape3D* shape, Material material);
	void place_wireframe(Shape2D* shape, Material material);
	void reset_scene();
	void set_camera(Camera* camera);
	void set_ambient_color(Color color);

private:
	void render_3d();
	void render_2d();
	void DirectX::display_as_convex(Models::ModelInfo model, D3DXMATRIX& wm, D3DXMATRIX& vm, D3DXMATRIX& pm, D3DXVECTOR3& ldirection);
	void DirectX::display_as_concave(Models::ModelInfo model, D3DXMATRIX& wm, D3DXMATRIX& vm, D3DXMATRIX& pm, D3DXVECTOR3& ldirection);

	Mode mode;

	vector<Models::ModelInfo> shapes;
	vector<Models::ModelInfo> inner_shapes;
	vector<Models::ModelInfo> lines;

	Color ambient_color;

	DirectBase* directx;
	Camera* camera;
	Models* models;
	Shaders* shaders;
};

#endif