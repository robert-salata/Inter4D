#ifndef Gui_class
#define Gui_class

#pragma comment(linker,"\"/manifestdependency:type='win32' \
name='Microsoft.Windows.Common-Controls' version='6.0.0.0' \
processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"") 

#include <windows.h>
#include <CommCtrl.h>
#include <shobjidl.h> 
#include <shlobj.h>
#include <wchar.h>
#include <string>
#include <map>
#include <vector>
#include "input.h"
#include "common.h"

using namespace std;

class Application;

class GUI
{
public:
	GUI(int width, int height, bool fullscreen, Input* input, Application* application);
	~GUI();
	void get_2d_render_size(int& width, int& height);
	void get_3d_render_size(int& width, int& height);
	void get_4d_render_size(int& width, int& height);
	void get_big_render_size(int& width, int& height);
	HWND get_render_2d_window();
	HWND get_render_3d_window();
	HWND get_render_4d_projection_window();
	HWND get_render_4d_intersection_window();
	HWND get_big_2d_window();
	HWND get_big_3d_window();
	void update();
	void show_error(std::string message);
	void show_warning(std::string message);
	static void update_radiogroup(int nr);
	static void update_material_p(double value, bool with_edits = true);
	static void update_material_i(double value, bool with_edits = true);
	static void update_wireframe_p(bool value);
	static void update_wireframe_i(bool value);
	static void update_axis(bool value);
	static void update_parameters_3d(double A, double B, double C, double D, bool with_edits = true);
	static void update_parameters_4d(double A, double B, double C, double D, double E, bool with_edits = true);
	static void update_rotation_3d(double rotation_x, double rotation_y, double distance, bool with_edits = true);
	static void update_rotation_4d(double rotation_xz, double rotation_yz, double rotation_xy, double distance, bool with_edits = true);
	static void update_rotation_4d_p(double rotation_xz, double rotation_yz, double rotation_xy, double distance, bool with_edits = true);

private:
	class Control
	{
	public:
		Control(): class_name(NULL), name(""), handle(NULL) { };
		Control(LPCWSTR class_name, string name, HWND handle, UINT id) : class_name(class_name), name(name), handle(handle), id(id) { };
		Control& with_info(string in) { info.push_back(in); return *this; };
		LPCWSTR class_name;
		string name;
		UINT id;
		HWND handle;
		vector<string> info;
	};

	static LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
	static LRESULT CALLBACK CanvasProc(HWND, UINT, WPARAM, LPARAM);

	static Control& create_label(string name, int x, int y, int w, int h, LPCWSTR text, int size = 19);
	static Control& create_button(string name, int x, int y, int w, int h, LPCWSTR text);
	static Control& create_slider(string name, int x, int y, int w, int h);
	static Control& create_vscroll(string name, int x, int y, int w, int h);
	static Control& create_canvas(string name, int x, int y, int w, int h);
	static Control& create_input(string name, int x, int y, int w, int h);
	static Control& create_checkbox(string name, int x, int y, int w, int h, LPCWSTR text);
	static Control& create_radiobutton(string name, int x, int y, int w, int h, LPCWSTR text, bool new_group = false);
	static Control& create_panel(string name, int x, int y, int w, int h);
	static void set_parent(HWND parent);
	static HWND current_parent;

	static void create_layout();
	static void create_left_panel(int y);
	static void create_right_panel(int y);
	static void destroy_layout();
	static void restore_values();

	static map<string, Control> controls;
	static map<HWND, Control*> controls_by_hwnd;
	static bool updated_already;
	static Control* selected_edit;

	static int width;
	static int height;
	static int margin_left;
	static int margin_right;
	bool fullscreen;
	static Input* input;
	static Application* application;

	static bool view_3d;
	static int display_mode;
	HINSTANCE hinstance;
	MSG msg;

	static HWND window;

	static bool resizing;
	static bool resized_already;
	static int panel_scroll_y;

	static double a_3d, b_3d, c_3d, d_3d, x_3d, y_3d, d0_3d, 
		a_4d, b_4d, c_4d, d_4d, e_4d, xz_4d, yz_4d, xy_4d, d0_4d, xz_4d_p, yz_4d_p, xy_4d_p, d0_4d_p,
		material_p, material_i;

	static bool wireframe_i, wireframe_p, axis;
};

#endif