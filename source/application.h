#ifndef Application_class
#define Application_class

#include "gui.h"
#include "directx_system.h"
#include "material.h"
#include "input.h"
#include "camera.h"
#include "shape3D.h"
#include "plane3D.h"
#include "segment3D.h"
#include "segment4D.h"
#include "scene3D.h"
#include "shape4D.h"
#include "space4D.h"
#include "scene4D.h"
#include "math_engine.h"
#include "common.h"

class Application
{
public:
	enum Mode { MODE_3D, MODE_4D };

	Application::Application(Mode mode = MODE_3D);
	~Application();
	void update();
	void set_mode(Mode mode);
	bool end;

private:
	friend class GUI;
	void set_parameters_3d(double A, double B, double C, double D);
	void set_parameters_4d(double A, double B, double C, double D, double E);
	void set_rotation_3d(double rX, double rY, double d);
	void set_rotation_4d(double rXZ, double rYZ, double rXY, double d);
	void set_projection_rotation_4d(double rXZ, double rYZ, double rXY, double d);
	void set_intersection_opacity(double value);
	void set_projection_opacity(double value);
	void set_projection_wireframe(bool value);
	void set_intersection_wireframe(bool value);
	void set_axis(bool value);
	void set_display_mode(int nr);
	void reset_parameters();
	void load_scene_3d(const wchar_t* file);
	void load_scene_4d(const wchar_t* file);
	void save_scene_3d(const wchar_t* file);
	void save_scene_4d(const wchar_t* file);
	void play_animation();

	void params_3d_to_rotation(double A, double B, double C, double D, double& rX, double& rY, double& d);
	void params_4d_to_rotation(double A, double B, double C, double D, double E, double& rX, double& rY, double& rZ, double& d);
	void rotation_3d_to_params(double rX, double rY, double d, double& A, double& B, double& C, double& D);
	void rotation_4d_to_params(double rX, double rY, double rZ, double d, double& A, double& B, double& C, double& D, double& E);

	void update_view_3d();
	void update_view_4d();
	void resize(int width, int height);
	void recalculate_intersections_3d();
	void recalculate_intersections_4d();
	void recalculate_projections_4d();
	void place_axis(DirectX* directx, double camera_distance);
	void place_axis_4d(DirectX* directx);
	void recalculate_axis_4d();

	bool ready;
	bool fullscreen;

	GUI* gui;
	DirectX* directx_2d;
	DirectX* directx_3d;
	DirectX* directx_4d_right;
	DirectX* directx_4d_left;
	DirectX* directx_big_2d;
	DirectX* directx_big_3d;
	Input* input;

	Scene3D scene_3d;
	double rX, rY, d_3d;
	vector<Shape2D> intersections_3_to_2;
	vector<Shape3D> intersections_3_to_3;
	vector<Shape2D> convex_intersections_3_to_2;
	vector<Shape3D> convex_intersections_3_to_3;

	Scene4D scene_4d;
	double rXZ, rYZ, rXY, d_4d;
	vector<Shape4D> intersections_4_to_4;
	vector<Shape3D> intersections_4_to_3;

	double projection_rXZ, projection_rYZ, projection_rXY, projection_d_4d;
	vector<Shape3D> projections_4d;
	vector<Shape3D> projections_of_intersections_4d;
	vector<Segment3D> projected_axis;

	double intersection_opacity;
	double projection_opacity;
	bool intersection_wireframe;
	bool projection_wireframe;

	Mode mode;
	int display_mode = 3;
	bool show_axis;

	Camera* camera_left;
	Camera* camera_right;
	double camera_left_distance;
	double camera_right_distance;
	bool animate;
};

#endif