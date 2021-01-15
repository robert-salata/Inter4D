#include "gui.h"
#include "application.h"
#include "resource.h"

Input* GUI::input;
bool GUI::view_3d;
int GUI::display_mode;
Application* GUI::application;

HWND GUI::window;
HWND GUI::current_parent;
int GUI::width;
int GUI::height;
int GUI::margin_right = 280;
int GUI::margin_left = 230;

bool GUI::resizing;
bool GUI::resized_already = false;
int GUI::panel_scroll_y = 0;

double GUI::a_3d, GUI::b_3d, GUI::c_3d, GUI::d_3d, GUI::x_3d, GUI::y_3d, GUI::d0_3d,
GUI::a_4d, GUI::b_4d, GUI::c_4d, GUI::d_4d, GUI::e_4d, GUI::xz_4d, GUI::yz_4d, GUI::xy_4d, GUI::d0_4d, 
GUI::xz_4d_p, GUI::yz_4d_p, GUI::xy_4d_p, GUI::d0_4d_p, GUI::material_i, GUI::material_p;

bool GUI::wireframe_i, GUI::wireframe_p, GUI::axis;

map<string, GUI::Control> GUI::controls;
map<HWND, GUI::Control*> GUI::controls_by_hwnd;
GUI::Control* GUI::selected_edit = NULL;

bool GUI::updated_already;

GUI::GUI(int width, int height, bool fullscreen, Input* input, Application* application)
{
	this->input = input;
	this->application = application;
	ZeroMemory(&msg, sizeof(MSG));
	hinstance = GetModuleHandle(NULL);
	view_3d = true;
	resizing = false;

	WNDCLASSEX main_class;
	main_class.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	main_class.lpfnWndProc = WndProc;
	main_class.cbClsExtra = 0;
	main_class.cbWndExtra = 0;
	main_class.hInstance = hinstance;
	main_class.hIcon = LoadIcon(hinstance, (LPCWSTR)IDI_ICON1);
	main_class.hIconSm = main_class.hIcon;
	main_class.hCursor = LoadCursor(NULL, IDC_ARROW);
	main_class.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	main_class.lpszMenuName = NULL;
	main_class.lpszClassName = L"Main";
	main_class.cbSize = sizeof(WNDCLASSEX);
	RegisterClassEx(&main_class);

	WNDCLASSEX render_class;
	render_class.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	render_class.lpfnWndProc = CanvasProc;// DefWindowProc;
	render_class.cbClsExtra = 0;
	render_class.cbWndExtra = 0;
	render_class.hInstance = hinstance;
	render_class.hIcon = LoadIcon(NULL, IDI_WINLOGO);
	render_class.hIconSm = render_class.hIcon;
	render_class.hCursor = LoadCursor(NULL, IDC_ARROW);
	render_class.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	render_class.lpszMenuName = NULL;
	render_class.lpszClassName = L"Canvas";
	render_class.cbSize = sizeof(WNDCLASSEX);
	RegisterClassEx(&render_class);

	WNDCLASSEX container_class;
	//container_class.style = CS_HREDRAW | CS_VREDRAW;
	container_class.lpfnWndProc = WndProc;// DefWindowProc;
	container_class.cbClsExtra = 0;
	container_class.cbWndExtra = 0;
	container_class.hInstance = hinstance;
	container_class.hIcon = LoadIcon(NULL, IDI_WINLOGO);
	container_class.hIconSm = container_class.hIcon;
	container_class.hCursor = LoadCursor(NULL, IDC_ARROW);
	container_class.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	container_class.lpszMenuName = NULL;
	container_class.lpszClassName = L"Panel";
	container_class.cbSize = sizeof(WNDCLASSEX);
	RegisterClassEx(&container_class);

	int pos_x, pos_y;
	this->fullscreen = fullscreen;
	if (fullscreen)
	{
		DEVMODE dmScreenSettings;
		this->width = GetSystemMetrics(SM_CXSCREEN);
		this->height = GetSystemMetrics(SM_CYSCREEN);
		memset(&dmScreenSettings, 0, sizeof(dmScreenSettings));
		dmScreenSettings.dmSize = sizeof(dmScreenSettings);
		dmScreenSettings.dmPelsWidth = (unsigned long)this->width;
		dmScreenSettings.dmPelsHeight = (unsigned long)this->height;
		dmScreenSettings.dmBitsPerPel = 32;
		dmScreenSettings.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;
		ChangeDisplaySettings(&dmScreenSettings, CDS_FULLSCREEN);
		pos_x = pos_y = 0;
	}
	else
	{
		this->width = width;
		this->height = height;
		pos_x = (GetSystemMetrics(SM_CXSCREEN) - this->width) / 2;
		pos_y = (GetSystemMetrics(SM_CYSCREEN) - this->height) / 2;
	}

	window = CreateWindowEx(WS_EX_APPWINDOW, L"Main", L"Inter4D", WS_OVERLAPPEDWINDOW | WS_CLIPSIBLINGS | WS_CLIPCHILDREN, pos_x, pos_y, this->width, this->height, NULL, NULL, hinstance, NULL);

	InitCommonControls();

	create_layout();

	ShowWindow(window, SW_SHOW);
	SetForegroundWindow(window);
	SetFocus(window);
}

GUI::~GUI()
{
	if (fullscreen)
		ChangeDisplaySettings(NULL, 0);

	DestroyWindow(window);
	destroy_layout();

	UnregisterClass(L"Main", hinstance);
	UnregisterClass(L"Canvas", hinstance);
	UnregisterClass(L"Panel", hinstance);
}

void GUI::create_layout()
{
	set_parent(window);

	create_left_panel(panel_scroll_y);
	if (height <= 580)
		create_vscroll("left_scroll", margin_left - 18, 0, 18, height);

	create_right_panel(panel_scroll_y);

	create_canvas("big_3d_window", margin_left, 0, width - margin_left - margin_right, height);
	create_canvas("big_2d_window", margin_left, 0, width - margin_left - margin_right, height);
	create_canvas("render_3d_window", margin_left, 0, (width - margin_left - margin_right) / 2, height);
	create_canvas("render_2d_window", margin_left + (width - margin_left - margin_right) / 2, 0, (width - margin_left - margin_right) / 2, height);
	create_canvas("render_4d_projection", margin_left, 0, (width - margin_left - margin_right) / 2, height);
	create_canvas("render_4d_intersection", margin_left + (width - margin_left - margin_right) / 2, 0, (width - margin_left - margin_right) / 2, height);

	ShowWindow(controls["big_3d_window"].handle, SW_HIDE);
	ShowWindow(controls["big_2d_window"].handle, SW_HIDE);
	ShowWindow(controls["render_4d_projection"].handle, SW_HIDE);
	ShowWindow(controls["render_4d_intersection"].handle, SW_HIDE);
	ShowWindow(controls["render_2d_window"].handle, SW_HIDE);
	ShowWindow(controls["render_3d_window"].handle, SW_HIDE);
}

void GUI::create_left_panel(int y_start)
{
	int y = y_start + 10;
	int margin_x = 0;
	int w = height <= 580 ? margin_left - 38 : margin_left - 20;
	create_button("button_3d", margin_x + 10, y, (w - 10) / 2, 30, L"3D").with_info("panel_left");
	create_button("button_4d", margin_x + 10 + (w + 10) / 2, y, (w - 10) / 2, 30, L"4D").with_info("panel_left");
	create_button("button_load", margin_x + 10, y += 40, w, 30, L"Load scene").with_info("panel_left");
	create_button("button_save", margin_x + 10, y += 40, w, 30, L"Save view as .obj").with_info("panel_left");
	create_button("button_reset", margin_x + 10, y += 40, w, 30, L"Reset parameters").with_info("panel_left");
	create_button("button_play", margin_x + 10, y += 40, w, 30, L"Play animation").with_info("panel_left");

	create_radiobutton("radio_projection", margin_x + 10, y += 70, w, 30, L"Show projection only", true).with_info("panel_left");
	create_radiobutton("radio_intersection", margin_x + 10, y += 25, w, 30, L"Show intersection only").with_info("panel_left");
	create_radiobutton("radio_both", margin_x + 10, y += 25, w, 30, L"Show both").with_info("panel_left");
	create_checkbox("checkbox_axis", margin_x + 10, y += 40, w, 20, L"Display axis").with_info("panel_left");

	create_label("label_material_p", margin_x + 10, y += 70, w - 60, 20, L"Projection opacity:").with_info("panel_left");
	create_input("edit_material_p", w - 60, y, 70, 20).with_info("panel_left");
	create_slider("slider_material_p", margin_x + 10, y += 22, w, 20).with_info("panel_left");
	create_checkbox("checkbox_wireframe_p", margin_x + 10, y += 20, w, 20, L"Show as wireframe").with_info("panel_left");

	create_label("label_material_i", margin_x + 10, y += 50, w - 60, 20, L"Intersection opacity:").with_info("panel_left");
	create_input("edit_material_i", w - 60, y, 70, 20).with_info("panel_left");
	create_slider("slider_material_i", margin_x + 10, y += 22, w, 20).with_info("panel_left");
	create_checkbox("checkbox_wireframe_i", margin_x + 10, y += 20, w, 20, L"Show as wireframe").with_info("panel_left");
}

void GUI::create_right_panel(int y_start)
{
	int margin_x = width - margin_right;
	int y = y_start;
	create_label("label_intersection", margin_x + 10, y, 260, 30, L"Intersection settings", 25).with_info("panel_3D");
	create_label("label_plane", margin_x + 10, y += 35, 260, 20, L"Plane: Ax + By + Cz + D = 0").with_info("panel_3D");;

	create_label("label_a", margin_x + 10, y += 25, 30, 20, L" A: ").with_info("panel_3D");;
	create_input("edit_a_3d", margin_x + 40, y, 80, 20).with_info("panel_3D");;
	create_slider("slider_a_3d", margin_x + 120, y, 150, 20).with_info("panel_3D");;

	create_label("label_b", margin_x + 10, y += 25, 30, 20, L" B: ").with_info("panel_3D");;
	create_input("edit_b_3d", margin_x + 40, y, 80, 20).with_info("panel_3D");;
	create_slider("slider_b_3d", margin_x + 120, y, 150, 20).with_info("panel_3D");;

	create_label("label_c", margin_x + 10, y += 25, 30, 20, L" C: ").with_info("panel_3D");;
	create_input("edit_c_3d", margin_x + 40, y, 80, 20).with_info("panel_3D");;
	create_slider("slider_c_3d", margin_x + 120, y, 150, 20).with_info("panel_3D");;

	create_label("label_d", margin_x + 10, y += 25, 30, 20, L" D: ").with_info("panel_3D");;
	create_input("edit_d_3d", margin_x + 40, y, 80, 20).with_info("panel_3D");;
	create_slider("slider_d_3d", margin_x + 120, y, 150, 20).with_info("panel_3D");;

	y = y_start + 200;
	create_label("label_x_rotation", margin_x + 10, y, 130, 20, L"Rotation +Y to +Z: ").with_info("panel_3D");;
	create_input("edit_x_rotation", margin_x + 140, y, 130, 20).with_info("panel_3D");;
	create_slider("slider_x_rotation", margin_x + 10, y += 22, 260, 20).with_info("panel_3D");;

	create_label("label_y_rotation", margin_x + 10, y += 33, 130, 20, L"Rotation +X to +Z: ").with_info("panel_3D");;
	create_input("edit_y_rotation", margin_x + 140, y, 130, 20).with_info("panel_3D");
	create_slider("slider_y_rotation", margin_x + 10, y += 22, 260, 20).with_info("panel_3D");

	create_label("label_distance_3d", margin_x + 10, y += 33, 130, 20, L"Distance to 0: ").with_info("panel_3D");
	create_input("edit_distance_3d", margin_x + 140, y, 130, 20).with_info("panel_3D");
	create_slider("slider_distance_3d", margin_x + 10, y += 22, 260, 20).with_info("panel_3D");

	//4D view
	margin_x = width - margin_right;
	y = y_start;
	create_label("label_intersection_4d", margin_x + 10, y, 260, 30, L"Intersection settings", 25).with_info("panel_4D");
	create_label("label_space", margin_x + 10, y += 35, 260, 20, L"Space: Ax + By + Cz + Dw + E = 0").with_info("panel_4D");

	create_label("label_a_4d", margin_x + 10, y += 25, 30, 20, L" A: ").with_info("panel_4D");;
	create_input("edit_a_4d", margin_x + 40, y, 80, 20).with_info("panel_4D");
	create_slider("slider_a_4d", margin_x + 120, y, 150, 20).with_info("panel_4D");

	create_label("label_b_4d", margin_x + 10, y += 25, 30, 20, L" B: ").with_info("panel_4D");;
	create_input("edit_b_4d", margin_x + 40, y, 80, 20).with_info("panel_4D");
	create_slider("slider_b_4d", margin_x + 120, y, 150, 20).with_info("panel_4D");

	create_label("label_c_4d", margin_x + 10, y += 25, 30, 20, L" C: ").with_info("panel_4D");;
	create_input("edit_c_4d", margin_x + 40, y, 80, 20).with_info("panel_4D");
	create_slider("slider_c_4d", margin_x + 120, y, 150, 20).with_info("panel_4D");

	create_label("label_d_4d", margin_x + 10, y += 25, 30, 20, L" D: ").with_info("panel_4D");;
	create_input("edit_d_4d", margin_x + 40, y, 80, 20).with_info("panel_4D");
	create_slider("slider_d_4d", margin_x + 120, y, 150, 20).with_info("panel_4D");

	create_label("label_e", margin_x + 10, y += 25, 30, 20, L" E: ").with_info("panel_4D");
	create_input("edit_e_4d", margin_x + 40, y, 80, 20).with_info("panel_4D");
	create_slider("slider_e_4d", margin_x + 120, y, 150, 20).with_info("panel_4D");

	y = y_start + 200;
	create_label("label_xz_rotation", margin_x + 10, y, 130, 20, L"Rotation +Y to +W: ").with_info("panel_4D");
	create_input("edit_xz_rotation", margin_x + 140, y, 130, 20).with_info("panel_4D");
	create_slider("slider_xz_rotation", margin_x + 10, y += 22, 260, 20).with_info("panel_4D");

	create_label("label_yz_rotation", margin_x + 10, y += 33, 130, 20, L"Rotation +X to +W: ").with_info("panel_4D");
	create_input("edit_yz_rotation", margin_x + 140, y, 130, 20).with_info("panel_4D");
	create_slider("slider_yz_rotation", margin_x + 10, y += 22, 260, 20).with_info("panel_4D");

	create_label("label_xy_rotation", margin_x + 10, y += 33, 130, 20, L"Rotation +Z to +W: ").with_info("panel_4D");
	create_input("edit_xy_rotation", margin_x + 140, y, 130, 20).with_info("panel_4D");
	create_slider("slider_xy_rotation", margin_x + 10, y += 22, 260, 20).with_info("panel_4D");

	create_label("label_distance_4d", margin_x + 10, y += 33, 130, 20, L"Distance to 0: ").with_info("panel_4D");
	create_input("edit_distance_4d", margin_x + 140, y, 130, 20).with_info("panel_4D");
	create_slider("slider_distance_4d", margin_x + 10, y += 22, 260, 20).with_info("panel_4D");

	y = y_start + 440;
	create_label("label_projection", margin_x + 10, y, 260, 30, L"Projection settings", 25).with_info("panel_4D");

	create_label("label_xz_rotation_p", margin_x + 10, y += 35, 130, 20, L"Rotation +Y to +W: ").with_info("panel_4D");
	create_input("edit_xz_rotation_p", margin_x + 140, y, 130, 20).with_info("panel_4D");
	create_slider("slider_xz_rotation_p", margin_x + 10, y += 22, 260, 20).with_info("panel_4D");

	create_label("label_yz_rotation_p", margin_x + 10, y += 33, 130, 20, L"Rotation +X to +W: ").with_info("panel_4D");
	create_input("edit_yz_rotation_p", margin_x + 140, y, 130, 20).with_info("panel_4D");
	create_slider("slider_yz_rotation_p", margin_x + 10, y += 22, 260, 20).with_info("panel_4D");

	create_label("label_xy_rotation_p", margin_x + 10, y += 33, 130, 20, L"Rotation +Z to +W: ").with_info("panel_4D");
	create_input("edit_xy_rotation_p", margin_x + 140, y, 130, 20).with_info("panel_4D");
	create_slider("slider_xy_rotation_p", margin_x + 10, y += 22, 260, 20).with_info("panel_4D");

	create_label("label_distance_4d_p", margin_x + 10, y += 33, 130, 20, L"Distance to 0: ").with_info("panel_4D");
	create_input("edit_distance_4d_p", margin_x + 140, y, 130, 20).with_info("panel_4D");
	create_slider("slider_distance_4d_p", margin_x + 10, y += 22, 260, 20).with_info("panel_4D");

	for (auto it = controls.begin(); it != controls.end(); it++)
		if (std::find(it->second.info.begin(), it->second.info.end(), view_3d ? "panel_4D" : "panel_3D") != it->second.info.end())
			ShowWindow(it->second.handle, SW_HIDE);
}

void GUI::destroy_layout()
{
	for (auto it = controls.begin(); it != controls.end(); it++)
		DestroyWindow(it->second.handle);
	controls.clear();
}

GUI::Control& GUI::create_label(string name, int x, int y, int w, int h, LPCWSTR text, int size)
{
	HWND hwnd = CreateWindow(L"Static", text, WS_CHILD | WS_VISIBLE, x, y, w, h, current_parent, (HMENU)(controls.size() + 100), NULL, NULL);
	controls.insert(make_pair(name, Control(L"Static", name, hwnd, controls.size() + 100)));
	controls_by_hwnd.insert(make_pair(hwnd, &controls[name]));

	HFONT hf = CreateFont(size, 0, 0, 0, 0, FALSE, 0, 0, 0, 0, 0, 0, 0, L"Segoe UI");
	SendMessage(hwnd, WM_SETFONT, (WPARAM)hf, (LPARAM)TRUE);

	return controls[name];
}

GUI::Control& GUI::create_button(string name, int x, int y, int w, int h, LPCWSTR text)
{
	HWND hwnd = CreateWindow(L"Button", text, WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, x, y, w, h, current_parent, (HMENU)(controls.size() + 100), NULL, NULL);
	controls.insert(make_pair(name, Control(L"Button", name, hwnd, controls.size() + 100)));
	controls_by_hwnd.insert(make_pair(hwnd, &controls[name]));

	HFONT hf = CreateFont(18, 0, 0, 0, 10000, FALSE, 0, 0, 0, 0, 0, 0, 0, L"Segoe UI");
	SendMessage(hwnd, WM_SETFONT, (WPARAM)hf, (LPARAM)TRUE);

	return controls[name];
}

GUI::Control& GUI::create_slider(string name, int x, int y, int w, int h)
{
	HWND hwnd = CreateWindow(TRACKBAR_CLASS, NULL, WS_CHILD | WS_VISIBLE | TBS_ENABLESELRANGE | TBS_NOTICKS, x, y, w, h, current_parent, (HMENU)(controls.size() + 100), NULL, NULL);
	controls.insert(make_pair(name, Control(L"Trackbar", name, hwnd, controls.size() + 100)));
	controls_by_hwnd.insert(make_pair(hwnd, &controls[name]));

	return controls[name];
}

GUI::Control& GUI::create_vscroll(string name, int x, int y, int w, int h)
{
	HWND hwnd = CreateWindow(L"Scrollbar", NULL, WS_CHILD | WS_VISIBLE | SBS_VERT, x, y, w, h, current_parent, (HMENU)(controls.size() + 100), NULL, NULL);
	controls.insert(make_pair(name, Control(L"V-Scrollbar", name, hwnd, controls.size() + 100)));
	controls_by_hwnd.insert(make_pair(hwnd, &controls[name]));

	SCROLLINFO si;
	si.cbSize = sizeof(si);
	si.fMask = SIF_RANGE | SIF_PAGE;
	si.nMin = 0;
	si.nMax = 100;
	si.nPage = 10;
	SetScrollInfo(hwnd, SB_VERT, &si, TRUE);

	return controls[name];
}

GUI::Control& GUI::create_canvas(string name, int x, int y, int w, int h)
{
	HWND hwnd = CreateWindow(L"Canvas", NULL, WS_VISIBLE | WS_CHILD, x, y, w, h, current_parent, (HMENU)(controls.size() + 100), NULL, NULL);
	controls.insert(make_pair(name, Control(L"Canvas", name, hwnd, controls.size() + 100)));
	controls_by_hwnd.insert(make_pair(hwnd, &controls[name]));

	return controls[name];
}

GUI::Control& GUI::create_input(string name, int x, int y, int w, int h)
{
	HWND hwnd = CreateWindow(L"Edit", NULL, WS_BORDER | WS_VISIBLE | WS_CHILD, x, y, w, h, current_parent, (HMENU)(controls.size() + 100), NULL, NULL);
	controls.insert(make_pair(name, Control(L"Edit", name, hwnd, controls.size() + 100)));
	controls_by_hwnd.insert(make_pair(hwnd, &controls[name]));

	HFONT hf = CreateFont(16, 0, 0, 0, 0, FALSE, 0, 0, 0, 0, 0, 0, 0, L"Calibri Light");
	SendMessage(hwnd, WM_SETFONT, (WPARAM)hf, (LPARAM)TRUE);

	return controls[name];
}

GUI::Control& GUI::create_checkbox(string name, int x, int y, int w, int h, LPCWSTR text)
{
	HWND hwnd = CreateWindow(L"Button", text, WS_CHILD | WS_VISIBLE | BS_CHECKBOX, x, y, w, h, current_parent, (HMENU)(controls.size() + 100), NULL, NULL);
	controls.insert(make_pair(name, Control(L"Checkbox", name, hwnd, controls.size() + 100)));
	controls_by_hwnd.insert(make_pair(hwnd, &controls[name]));

	HFONT hf = CreateFont(19, 0, 0, 0, 0, FALSE, 0, 0, 0, 0, 0, 0, 0, L"Segoe UI");
	SendMessage(hwnd, WM_SETFONT, (WPARAM)hf, (LPARAM)TRUE);

	return controls[name];
}

GUI::Control& GUI::create_radiobutton(string name, int x, int y, int w, int h, LPCWSTR text, bool new_group)
{
	int styles = new_group ? WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON | WS_GROUP : WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON;
	HWND hwnd = CreateWindow(L"Button", text, styles, x, y, w, h, current_parent, (HMENU)(controls.size() + 100), NULL, NULL);
	controls.insert(make_pair(name, Control(L"Radiobutton", name, hwnd, controls.size() + 100)));
	controls_by_hwnd.insert(make_pair(hwnd, &controls[name]));

	HFONT hf = CreateFont(19, 0, 0, 0, 0, FALSE, 0, 0, 0, 0, 0, 0, 0, L"Segoe UI");
	SendMessage(hwnd, WM_SETFONT, (WPARAM)hf, (LPARAM)TRUE);

	return controls[name];
}

GUI::Control& GUI::create_panel(string name, int x, int y, int w, int h)
{
	HWND hwnd = CreateWindow(L"Panel", NULL, WS_VISIBLE | WS_CHILD | WS_VSCROLL, x, y, w, h, current_parent, (HMENU)(controls.size() + 100), NULL, NULL);
	controls.insert(make_pair(name, Control(L"Panel", name, hwnd, controls.size() + 100)));
	controls_by_hwnd.insert(make_pair(hwnd, &controls[name]));

	return controls[name];
}

void GUI::set_parent(HWND hwnd)
{
	current_parent = hwnd;
}

void GUI::get_2d_render_size(int& width, int& height)
{
	width = (this->width - margin_left - margin_right)/2;
	height = this->height;
}

void GUI::get_3d_render_size(int& width, int& height)
{
	width = (this->width - margin_left - margin_right) / 2;
	height = this->height;
}

void GUI::get_4d_render_size(int& width, int& height)
{
	width = (this->width - margin_left - margin_right) / 2;
	height = this->height;
}


void GUI::get_big_render_size(int& width, int& height)
{
	width = (this->width - margin_left - margin_right);
	height = this->height;
}

HWND GUI::get_render_2d_window() { return controls["render_2d_window"].handle; }
HWND GUI::get_render_3d_window() { return controls["render_3d_window"].handle; }
HWND GUI::get_render_4d_projection_window() { return controls["render_4d_projection"].handle; }
HWND GUI::get_render_4d_intersection_window() { return controls["render_4d_intersection"].handle; }
HWND GUI::get_big_3d_window() { return controls["big_3d_window"].handle; }
HWND GUI::get_big_2d_window() { return controls["big_2d_window"].handle; }

void GUI::update()
{
	updated_already = false;

	if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);

		if (msg.message == WM_QUIT)
			application->end = true;
	}

	static Control* old_selected = NULL;
	if (selected_edit && selected_edit != old_selected)
	{
		SendMessage(selected_edit->handle, EM_SETSEL, (WPARAM)0, (LPARAM)-1);
		old_selected = selected_edit;
	}
}

void GUI::update_radiogroup(int nr)
{
	if (nr == 1)
	{
		ShowWindow(controls["render_4d_projection"].handle, SW_HIDE);
		ShowWindow(controls["render_4d_intersection"].handle, SW_HIDE);
		ShowWindow(controls["render_3d_window"].handle, SW_HIDE);
		ShowWindow(controls["render_2d_window"].handle, SW_HIDE);
		ShowWindow(controls["big_3d_window"].handle, SW_SHOW);

		CheckDlgButton(window, controls["radio_projection"].id, true);
	}
	if (nr == 2)
	{
		ShowWindow(controls["render_4d_projection"].handle, SW_HIDE);
		ShowWindow(controls["render_4d_intersection"].handle, SW_HIDE);
		ShowWindow(controls["render_3d_window"].handle, SW_HIDE);
		ShowWindow(controls["render_2d_window"].handle, SW_HIDE);
		view_3d ? ShowWindow(controls["big_2d_window"].handle, SW_SHOW) : ShowWindow(controls["big_3d_window"].handle, SW_SHOW);

		CheckDlgButton(window, controls["radio_intersection"].id, true);
	}
	if (nr == 3)
	{
		ShowWindow(controls["big_2d_window"].handle, SW_HIDE);
		ShowWindow(controls["big_3d_window"].handle, SW_HIDE);

		if (view_3d)
		{
			ShowWindow(controls["render_3d_window"].handle, SW_SHOW);
			ShowWindow(controls["render_2d_window"].handle, SW_SHOW);
		}
		else
		{
			ShowWindow(controls["render_4d_projection"].handle, SW_SHOW);
			ShowWindow(controls["render_4d_intersection"].handle, SW_SHOW);
		}
		CheckDlgButton(window, controls["radio_both"].id, true);
	}

	display_mode = nr;
	application->set_display_mode(nr);
}

void GUI::update_material_i(double value, bool with_edits)
{
	material_i = value;
	if (with_edits)
		SetWindowTextW(controls["edit_material_i"].handle, to_wstring(value).c_str());
	
	SendMessage(controls["slider_material_i"].handle, TBM_SETPOS, (WPARAM)TRUE, (int)clip(value * 100, 0, 100));
	application->set_intersection_opacity(value);
}

void GUI::update_material_p(double value, bool with_edits)
{
	material_p = value;
	if (with_edits)
		SetWindowTextW(controls["edit_material_p"].handle, to_wstring(value).c_str());

	SendMessage(controls["slider_material_p"].handle, TBM_SETPOS, (WPARAM)TRUE, (int)clip(value * 100, 0, 100));
	application->set_projection_opacity(value);
}

void GUI::update_wireframe_i(bool value)
{
	CheckDlgButton(window, controls["checkbox_wireframe_i"].id, value ? BST_CHECKED : BST_UNCHECKED);
	application->set_intersection_wireframe(value);
	wireframe_i = value;
}

void GUI::update_wireframe_p(bool value)
{
	CheckDlgButton(window, controls["checkbox_wireframe_p"].id, value ? BST_CHECKED : BST_UNCHECKED);
	application->set_projection_wireframe(value);
	wireframe_p = value;
}

void GUI::update_axis(bool value)
{
	CheckDlgButton(window, controls["checkbox_axis"].id, value ? BST_CHECKED : BST_UNCHECKED);
	application->set_axis(value);
	axis = value;
}

void GUI::update_parameters_3d(double A, double B, double C, double D, bool with_edits)
{
	a_3d = A;
	b_3d = B;
	c_3d = C;
	d_3d = D;
	if (with_edits)
	{
		SetWindowTextW(controls["edit_a_3d"].handle, to_wstring(A).c_str());
		SetWindowTextW(controls["edit_b_3d"].handle, to_wstring(B).c_str());
		SetWindowTextW(controls["edit_c_3d"].handle, to_wstring(C).c_str());
		SetWindowTextW(controls["edit_d_3d"].handle, to_wstring(D).c_str());
	}
	SendMessage(controls["slider_a_3d"].handle, TBM_SETPOS, (WPARAM)TRUE, (int)clip(A * 50 + 50, 0, 100));
	SendMessage(controls["slider_b_3d"].handle, TBM_SETPOS, (WPARAM)TRUE, (int)clip(B * 50 + 50, 0, 100));
	SendMessage(controls["slider_c_3d"].handle, TBM_SETPOS, (WPARAM)TRUE, (int)clip(C * 50 + 50, 0, 100));
	SendMessage(controls["slider_d_3d"].handle, TBM_SETPOS, (WPARAM)TRUE, (int)clip(D * 50 + 50, 0, 100));
	application->set_parameters_3d(A, B, C, D);

	application->params_3d_to_rotation(A, B, C, D, x_3d, y_3d, d0_3d);
	SetWindowTextW(controls["edit_x_rotation"].handle, to_wstring(x_3d).c_str());
	SetWindowTextW(controls["edit_y_rotation"].handle, to_wstring(y_3d).c_str());
	SetWindowTextW(controls["edit_distance_3d"].handle, to_wstring(d0_3d).c_str());
	SendMessage(controls["slider_x_rotation"].handle, TBM_SETPOS, (WPARAM)TRUE, (int)clip(x_3d / 90 * 100, 0, 100));
	SendMessage(controls["slider_y_rotation"].handle, TBM_SETPOS, (WPARAM)TRUE, (int)clip(y_3d / 360 * 100, 0, 100));
	SendMessage(controls["slider_distance_3d"].handle, TBM_SETPOS, (WPARAM)TRUE, (int)clip(d0_3d * 25 + 50, 0, 100));
}

void GUI::update_parameters_4d(double A, double B, double C, double D, double E, bool with_edits)
{
	a_4d = A;
	b_4d = B;
	c_4d = C;
	d_4d = D;
	e_4d = E;
	if (with_edits)
	{
		SetWindowTextW(controls["edit_a_4d"].handle, to_wstring(A).c_str());
		SetWindowTextW(controls["edit_b_4d"].handle, to_wstring(B).c_str());
		SetWindowTextW(controls["edit_c_4d"].handle, to_wstring(C).c_str());
		SetWindowTextW(controls["edit_d_4d"].handle, to_wstring(D).c_str());
		SetWindowTextW(controls["edit_e_4d"].handle, to_wstring(E).c_str());
	}
	SendMessage(controls["slider_a_4d"].handle, TBM_SETPOS, (WPARAM)TRUE, (int)clip(A * 50 + 50, 0, 100));
	SendMessage(controls["slider_b_4d"].handle, TBM_SETPOS, (WPARAM)TRUE, (int)clip(B * 50 + 50, 0, 100));
	SendMessage(controls["slider_c_4d"].handle, TBM_SETPOS, (WPARAM)TRUE, (int)clip(C * 50 + 50, 0, 100));
	SendMessage(controls["slider_d_4d"].handle, TBM_SETPOS, (WPARAM)TRUE, (int)clip(D * 50 + 50, 0, 100));
	SendMessage(controls["slider_e_4d"].handle, TBM_SETPOS, (WPARAM)TRUE, (int)clip(E * 50 + 50, 0, 100));
	application->set_parameters_4d(A, B, C, D, E);

	application->params_4d_to_rotation(A, B, C, D, E, xz_4d, yz_4d, xy_4d, d0_4d);
	SetWindowTextW(controls["edit_xz_rotation"].handle, to_wstring(xz_4d).c_str());
	SetWindowTextW(controls["edit_yz_rotation"].handle, to_wstring(yz_4d).c_str());
	SetWindowTextW(controls["edit_xy_rotation"].handle, to_wstring(xy_4d).c_str());
	SetWindowTextW(controls["edit_distance_4d"].handle, to_wstring(d0_4d).c_str());
	SendMessage(controls["slider_xz_rotation"].handle, TBM_SETPOS, (WPARAM)TRUE, (int)clip((xz_4d + 90) / 180 * 100, 0, 100));
	SendMessage(controls["slider_yz_rotation"].handle, TBM_SETPOS, (WPARAM)TRUE, (int)clip((yz_4d + 90) / 180 * 100, 0, 100));
	SendMessage(controls["slider_xy_rotation"].handle, TBM_SETPOS, (WPARAM)TRUE, (int)clip((xy_4d + 90) / 180 * 100, 0, 100));
	SendMessage(controls["slider_distance_4d"].handle, TBM_SETPOS, (WPARAM)TRUE, (int)clip(d0_4d * 25 + 50, 0, 100));
}

void GUI::update_rotation_3d(double rotation_x, double rotation_y, double distance, bool with_edits)
{
	x_3d = rotation_x;
	y_3d = rotation_y;
	d0_3d = distance;
	if (with_edits)
	{
		SetWindowTextW(controls["edit_x_rotation"].handle, to_wstring(rotation_x).c_str());
		SetWindowTextW(controls["edit_y_rotation"].handle, to_wstring(rotation_y).c_str());
		SetWindowTextW(controls["edit_distance_3d"].handle, to_wstring(distance).c_str());
	}
	SendMessage(controls["slider_x_rotation"].handle, TBM_SETPOS, (WPARAM)TRUE, (int)clip(rotation_x / 90 * 100, 0, 100));
	SendMessage(controls["slider_y_rotation"].handle, TBM_SETPOS, (WPARAM)TRUE, (int)clip(rotation_y / 360 * 100, 0, 100));
	SendMessage(controls["slider_distance_3d"].handle, TBM_SETPOS, (WPARAM)TRUE, (int)clip(distance * 25 + 50, 0, 100));
	application->set_rotation_3d(rotation_x, rotation_y, distance);

	application->rotation_3d_to_params(rotation_x, rotation_y, distance, a_3d, b_3d, c_3d, d_3d);
	SetWindowTextW(controls["edit_a_3d"].handle, to_wstring(a_3d).c_str());
	SetWindowTextW(controls["edit_b_3d"].handle, to_wstring(b_3d).c_str());
	SetWindowTextW(controls["edit_c_3d"].handle, to_wstring(c_3d).c_str());
	SetWindowTextW(controls["edit_d_3d"].handle, to_wstring(d_3d).c_str());
	SendMessage(controls["slider_a_3d"].handle, TBM_SETPOS, (WPARAM)TRUE, (int)clip(a_3d * 50 + 50, 0, 100));
	SendMessage(controls["slider_b_3d"].handle, TBM_SETPOS, (WPARAM)TRUE, (int)clip(b_3d * 50 + 50, 0, 100));
	SendMessage(controls["slider_c_3d"].handle, TBM_SETPOS, (WPARAM)TRUE, (int)clip(c_3d * 50 + 50, 0, 100));
	SendMessage(controls["slider_d_3d"].handle, TBM_SETPOS, (WPARAM)TRUE, (int)clip(d_3d * 50 + 50, 0, 100));
}

void GUI::update_rotation_4d(double rotation_xz, double rotation_yz, double rotation_xy, double distance, bool with_edits)
{
	xz_4d = rotation_xz;
	yz_4d = rotation_yz;
	xy_4d = rotation_xy;
	d0_4d = distance;
	if (with_edits)
	{
		SetWindowTextW(controls["edit_xz_rotation"].handle, to_wstring(xz_4d).c_str());
		SetWindowTextW(controls["edit_yz_rotation"].handle, to_wstring(yz_4d).c_str());
		SetWindowTextW(controls["edit_xy_rotation"].handle, to_wstring(xy_4d).c_str());
		SetWindowTextW(controls["edit_distance_4d"].handle, to_wstring(d0_4d).c_str());
	}
	SendMessage(controls["slider_xz_rotation"].handle, TBM_SETPOS, (WPARAM)TRUE, (int)clip((xz_4d + 90) / 180 * 100, 0, 100));
	SendMessage(controls["slider_yz_rotation"].handle, TBM_SETPOS, (WPARAM)TRUE, (int)clip((yz_4d + 90) / 180 * 100, 0, 100));
	SendMessage(controls["slider_xy_rotation"].handle, TBM_SETPOS, (WPARAM)TRUE, (int)clip((xy_4d + 90) / 180 * 100, 0, 100));
	SendMessage(controls["slider_distance_4d"].handle, TBM_SETPOS, (WPARAM)TRUE, (int)clip(d0_4d * 25 + 50, 0, 100));
	application->set_rotation_4d(rotation_xz, rotation_yz, rotation_xy, distance);

	application->rotation_4d_to_params(rotation_xz, rotation_yz, rotation_xy, distance, a_4d, b_4d, c_4d, d_4d, e_4d);
	SetWindowTextW(controls["edit_a_4d"].handle, to_wstring(a_4d).c_str());
	SetWindowTextW(controls["edit_b_4d"].handle, to_wstring(b_4d).c_str());
	SetWindowTextW(controls["edit_c_4d"].handle, to_wstring(c_4d).c_str());
	SetWindowTextW(controls["edit_d_4d"].handle, to_wstring(d_4d).c_str());
	SetWindowTextW(controls["edit_e_4d"].handle, to_wstring(e_4d).c_str());
	SendMessage(controls["slider_a_4d"].handle, TBM_SETPOS, (WPARAM)TRUE, (int)clip(a_4d * 50 + 50, 0, 100));
	SendMessage(controls["slider_b_4d"].handle, TBM_SETPOS, (WPARAM)TRUE, (int)clip(b_4d * 50 + 50, 0, 100));
	SendMessage(controls["slider_c_4d"].handle, TBM_SETPOS, (WPARAM)TRUE, (int)clip(c_4d * 50 + 50, 0, 100));
	SendMessage(controls["slider_d_4d"].handle, TBM_SETPOS, (WPARAM)TRUE, (int)clip(d_4d * 50 + 50, 0, 100));
	SendMessage(controls["slider_e_4d"].handle, TBM_SETPOS, (WPARAM)TRUE, (int)clip(e_4d * 50 + 50, 0, 100));
}

void GUI::update_rotation_4d_p(double rotation_xz, double rotation_yz, double rotation_xy, double distance, bool with_edits)
{
	xz_4d_p = rotation_xz;
	yz_4d_p = rotation_yz;
	xy_4d_p = rotation_xy;
	d0_4d_p = distance;
	if (with_edits)
	{
		SetWindowTextW(controls["edit_xz_rotation_p"].handle, to_wstring(xz_4d_p).c_str());
		SetWindowTextW(controls["edit_yz_rotation_p"].handle, to_wstring(yz_4d_p).c_str());
		SetWindowTextW(controls["edit_xy_rotation_p"].handle, to_wstring(xy_4d_p).c_str());
		SetWindowTextW(controls["edit_distance_4d_p"].handle, to_wstring(d0_4d_p).c_str());
	}
	SendMessage(controls["slider_xz_rotation_p"].handle, TBM_SETPOS, (WPARAM)TRUE, (int)clip((xz_4d_p + 90) / 180 * 100, 0, 100));
	SendMessage(controls["slider_yz_rotation_p"].handle, TBM_SETPOS, (WPARAM)TRUE, (int)clip((yz_4d_p + 90) / 180 * 100, 0, 100));
	SendMessage(controls["slider_xy_rotation_p"].handle, TBM_SETPOS, (WPARAM)TRUE, (int)clip((xy_4d_p + 90) / 180 * 100, 0, 100));
	SendMessage(controls["slider_distance_4d_p"].handle, TBM_SETPOS, (WPARAM)TRUE, (int)clip(d0_4d_p * 25, 0, 100));
	application->set_projection_rotation_4d(rotation_xz, rotation_yz, rotation_xy, distance);
}

void GUI::restore_values()
{
	update_radiogroup(display_mode);
	update_material_p(material_p);
	update_material_i(material_i);
	update_wireframe_p(wireframe_p);
	update_wireframe_i(wireframe_i);
	update_axis(axis);
	update_parameters_3d(a_3d, b_3d, c_3d, d_3d);
	update_parameters_4d(a_4d, b_4d, c_4d, d_4d, e_4d);
	update_rotation_4d_p(xz_4d_p, yz_4d_p, xy_4d_p, d0_4d_p);
}

void GUI::show_error(string message)
{
	MessageBox(NULL, wstring(message.begin(), message.end()).c_str(), L"Error", MB_ICONERROR);
}

void GUI::show_warning(string message)
{
	MessageBox(NULL, wstring(message.begin(), message.end()).c_str(), L"Warning", MB_ICONEXCLAMATION);
}

LRESULT CALLBACK GUI::CanvasProc(HWND hwnd, UINT umessage, WPARAM wparam, LPARAM lparam)
{
	switch (umessage)
	{
	case WM_MOUSEMOVE:
		if (hwnd == controls["render_2d_window"].handle)
			input->mouse_move((width - margin_left - margin_right) / 2 + (int)LOWORD(lparam), (int)HIWORD(lparam));
		else if (hwnd == controls["render_4d_intersection"].handle)
			input->mouse_move((width - margin_left - margin_right) / 2 + (int)LOWORD(lparam), (int)HIWORD(lparam));
		else
			input->mouse_move((int)LOWORD(lparam), (int)HIWORD(lparam));
		return 0;

	case WM_LBUTTONDOWN:
		if (wparam & MK_LBUTTON)
			input->left_down();
		return 0;

	case WM_RBUTTONDOWN:
		if (wparam & MK_RBUTTON)
			input->right_down();
		return 0;

	case WM_MBUTTONDOWN:
		if (wparam & MK_MBUTTON)
			input->middle_down();
		return 0;

	case WM_LBUTTONUP:
		input->left_up();
		return 0;

	case WM_RBUTTONUP:
		input->right_up();
		return 0;

	case WM_MBUTTONUP:
		input->middle_up();
		return 0;

	case WM_MOUSEWHEEL:
		input->scroll_change(GET_WHEEL_DELTA_WPARAM(wparam) / 120);
		return 0;

	default:
		return DefWindowProc(hwnd, umessage, wparam, lparam);
	}
}

LRESULT CALLBACK GUI::WndProc(HWND hwnd, UINT umessage, WPARAM wparam, LPARAM lparam)
{
	string control_name;
	UINT control_id;
	LPCWSTR class_name;
	switch (umessage)
	{
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;

	case WM_CLOSE:
		PostQuitMessage(0);
		return 0;

	case WM_KEYDOWN:
		input->key_down((unsigned int)wparam);
		return 0;

	case WM_KEYUP:
		input->key_up((unsigned int)wparam);
		return 0;

	case WM_MOUSEACTIVATE:
		SetFocus(window);
		return 0;

	case WM_HSCROLL:
		updated_already = true;
		control_name = (*controls_by_hwnd[(HWND)lparam]).name;

		if (control_name == "slider_material_i")
			update_material_i(material_i = (double)(SendMessage(controls["slider_material_i"].handle, TBM_GETPOS, 0, 0)) / 100);
		if (control_name == "slider_material_p")
			update_material_p(material_p = (double)(SendMessage(controls["slider_material_p"].handle, TBM_GETPOS, 0, 0)) / 100);

		else if (control_name == "slider_a_3d")
			update_parameters_3d(a_3d = (double)(SendMessage(controls["slider_a_3d"].handle, TBM_GETPOS, 0, 0) - 50) / 50, b_3d, c_3d, d_3d);
		else if (control_name == "slider_b_3d")
			update_parameters_3d(a_3d, b_3d = (double)(SendMessage(controls["slider_b_3d"].handle, TBM_GETPOS, 0, 0) - 50) / 50, c_3d, d_3d);
		else if (control_name == "slider_c_3d")
			update_parameters_3d(a_3d, b_3d, c_3d = (double)(SendMessage(controls["slider_c_3d"].handle, TBM_GETPOS, 0, 0) - 50) / 50, d_3d);
		else if (control_name == "slider_d_3d")
			update_parameters_3d(a_3d, b_3d, c_3d, d_3d = (double)(SendMessage(controls["slider_d_3d"].handle, TBM_GETPOS, 0, 0) - 50) / 50);

		else if (control_name == "slider_x_rotation")
			update_rotation_3d(x_3d = (double)(SendMessage(controls["slider_x_rotation"].handle, TBM_GETPOS, 0, 0)) / 100 * 90, y_3d, d0_3d);
		else if (control_name == "slider_y_rotation")
			update_rotation_3d(x_3d, y_3d = (double)(SendMessage(controls["slider_y_rotation"].handle, TBM_GETPOS, 0, 0)) / 100 * 360, d0_3d);
		else if (control_name == "slider_distance_3d")
			update_rotation_3d(x_3d, y_3d, d0_3d = (double)(SendMessage(controls["slider_distance_3d"].handle, TBM_GETPOS, 0, 0) - 50) / 25);

		else if (control_name == "slider_a_4d")
			update_parameters_4d(a_4d = (double)(SendMessage(controls["slider_a_4d"].handle, TBM_GETPOS, 0, 0) - 50) / 50, b_4d, c_4d, d_4d, e_4d);
		else if (control_name == "slider_b_4d")
			update_parameters_4d(a_4d, b_4d = (double)(SendMessage(controls["slider_b_4d"].handle, TBM_GETPOS, 0, 0) - 50) / 50, c_4d, d_4d, e_4d);
		else if (control_name == "slider_c_4d")
			update_parameters_4d(a_4d, b_4d, c_4d = (double)(SendMessage(controls["slider_c_4d"].handle, TBM_GETPOS, 0, 0) - 50) / 50, d_4d, e_4d);
		else if (control_name == "slider_d_4d")
			update_parameters_4d(a_4d, b_4d, c_4d, d_4d = (double)(SendMessage(controls["slider_d_4d"].handle, TBM_GETPOS, 0, 0) - 50) / 50, e_4d);
		else if (control_name == "slider_e_4d")
			update_parameters_4d(a_4d, b_4d, c_4d, d_4d, e_4d = (double)(SendMessage(controls["slider_e_4d"].handle, TBM_GETPOS, 0, 0) - 50) / 50);

		else if (control_name == "slider_xz_rotation")
			update_rotation_4d(xz_4d = (double)(SendMessage(controls["slider_xz_rotation"].handle, TBM_GETPOS, 0, 0)) / 100 * 180 - 90, yz_4d, xy_4d, d0_4d);
		else if (control_name == "slider_yz_rotation")
			update_rotation_4d(xz_4d, yz_4d = (double)(SendMessage(controls["slider_yz_rotation"].handle, TBM_GETPOS, 0, 0)) / 100 * 180 - 90, xy_4d, d0_4d);
		else if (control_name == "slider_xy_rotation")
			update_rotation_4d(xz_4d, yz_4d, xy_4d = (double)(SendMessage(controls["slider_xy_rotation"].handle, TBM_GETPOS, 0, 0)) / 100 * 180 - 90, d0_4d);
		else if (control_name == "slider_distance_4d")
			update_rotation_4d(xz_4d, yz_4d, xy_4d, d0_4d = (double)(SendMessage(controls["slider_distance_4d"].handle, TBM_GETPOS, 0, 0) - 50) / 25);
		
		else if (control_name == "slider_xz_rotation_p")
			update_rotation_4d_p(xz_4d_p = (double)(SendMessage(controls["slider_xz_rotation_p"].handle, TBM_GETPOS, 0, 0)) / 100 * 180 - 90, yz_4d_p, xy_4d_p, d0_4d_p);
		else if (control_name == "slider_yz_rotation_p")
			update_rotation_4d_p(xz_4d_p, yz_4d_p = (double)(SendMessage(controls["slider_yz_rotation_p"].handle, TBM_GETPOS, 0, 0)) / 100 * 180 - 90, xy_4d_p, d0_4d_p);
		else if (control_name == "slider_xy_rotation_p")
			update_rotation_4d_p(xz_4d_p, yz_4d_p, xy_4d_p = (double)(SendMessage(controls["slider_xy_rotation_p"].handle, TBM_GETPOS, 0, 0)) / 100 * 180 - 90, d0_4d_p);
		else if (control_name == "slider_distance_4d_p")
			update_rotation_4d_p(xz_4d_p, yz_4d_p, xy_4d_p, d0_4d_p = (double)(SendMessage(controls["slider_distance_4d_p"].handle, TBM_GETPOS, 0, 0)) / 25);

		return 0;

	/*case WM_VSCROLL:
	{
		SCROLLINFO si;
		si.cbSize = sizeof(si);
		si.fMask = SIF_ALL;
		GetScrollInfo(controls["left_scroll"].handle, SB_VERT, &si);
		
		int pos = GetScrollPos(controls["left_scroll"].handle, SB_CTL);
		for (auto it = controls.begin(); it != controls.end(); it++)
			if (std::find(it->second.info.begin(), it->second.info.end(), "panel_left") != it->second.info.end())
				DestroyWindow(it->second.handle);
		create_left_panel(pos);

		return 0;
	}*/

	/*case WM_MOUSEWHEEL:
		panel_scroll_y += 10 * GET_WHEEL_DELTA_WPARAM(wparam) / 120;

		for (auto it = controls.begin(); it != controls.end(); it++)
			if (std::find(it->second.info.begin(), it->second.info.end(), "panel_left") != it->second.info.end() ||
				std::find(it->second.info.begin(), it->second.info.end(), "panel_3D") != it->second.info.end() ||
				std::find(it->second.info.begin(), it->second.info.end(), "panel_4D") != it->second.info.end())
			{
				DestroyWindow(it->second.handle);
				controls.erase(it--);
			}
		
		create_left_panel(panel_scroll_y);
		create_right_panel(panel_scroll_y);
		restore_values();

		MSG msg;
		while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);

			if (msg.message == WM_QUIT)
				application->end = true;
		}

		return 0;*/

	case WM_COMMAND:
		if (updated_already)
			return 0;

		control_name = (*controls_by_hwnd[(HWND)lparam]).name;
		control_id = LOWORD(wparam);

		if (HIWORD(wparam) == EN_CHANGE)
		{
			updated_already = true;
			wchar_t text_a[10], text_b[10], text_c[10], text_d[10], text_e[10];
			wchar_t* p;
			if (control_name == "edit_material_p")
			{
				GetWindowTextW(controls["edit_material_p"].handle, text_a, 10);
				update_material_p(material_p = wcstof(text_a, &p), false);
			}
			else if (control_name == "edit_material_i")
			{
				GetWindowTextW(controls["edit_material_i"].handle, text_a, 10);
				update_material_i(material_i = wcstof(text_a, &p), false);
			}
			else if (control_name == "edit_a_3d" || control_name == "edit_b_3d" || control_name == "edit_c_3d" || control_name == "edit_d_3d")
			{
				GetWindowTextW(controls["edit_a_3d"].handle, text_a, 10);
				GetWindowTextW(controls["edit_b_3d"].handle, text_b, 10);
				GetWindowTextW(controls["edit_c_3d"].handle, text_c, 10);
				GetWindowTextW(controls["edit_d_3d"].handle, text_d, 10);
				if (control_name == "edit_a_3d")
					update_parameters_3d(a_3d = wcstof(text_a, &p), b_3d, c_3d, d_3d, false);
				else if (control_name == "edit_b_3d")
					update_parameters_3d(a_3d, b_3d = wcstof(text_b, &p), c_3d, d_3d, false);
				else if (control_name == "edit_c_3d")
					update_parameters_3d(a_3d, b_3d, c_3d = wcstof(text_c, &p), d_3d, false);
				else if (control_name == "edit_d_3d")
					update_parameters_3d(a_3d, b_3d, c_3d, d_3d = wcstof(text_d, &p), false);
			}
			else if (control_name == "edit_x_rotation" || control_name == "edit_y_rotation" || control_name == "edit_distance_3d")
			{
				GetWindowTextW(controls["edit_x_rotation"].handle, text_a, 10);
				GetWindowTextW(controls["edit_y_rotation"].handle, text_b, 10);
				GetWindowTextW(controls["edit_distance_3d"].handle, text_c, 10);
				if (control_name == "edit_x_rotation")
					update_rotation_3d(x_3d = wcstof(text_a, &p), y_3d, d0_3d, false);
				else if (control_name == "edit_y_rotation")
					update_rotation_3d(x_3d, y_3d = wcstof(text_b, &p), d0_3d, false);
				else if (control_name == "edit_distance_3d")
					update_rotation_3d(x_3d, y_3d, d0_3d = wcstof(text_c, &p), false);
			}
			if (control_name == "edit_a_4d" || control_name == "edit_b_4d" || control_name == "edit_c_4d" || control_name == "edit_d_4d" ||
				control_name == "edit_e_4d")
			{
				GetWindowTextW(controls["edit_a_4d"].handle, text_a, 10);
				GetWindowTextW(controls["edit_b_4d"].handle, text_b, 10);
				GetWindowTextW(controls["edit_c_4d"].handle, text_c, 10);
				GetWindowTextW(controls["edit_d_4d"].handle, text_d, 10);
				GetWindowTextW(controls["edit_e_4d"].handle, text_e, 10);
				if (control_name == "edit_a_4d")
					update_parameters_4d(a_4d = wcstof(text_a, &p), b_4d, c_4d, d_4d, e_4d, false);
				else if (control_name == "edit_b_4d")
					update_parameters_4d(a_4d, b_4d = wcstof(text_b, &p), c_4d, d_4d, e_4d, false);
				else if (control_name == "edit_c_4d")
					update_parameters_4d(a_4d, b_4d, c_4d = wcstof(text_c, &p), d_4d, e_4d, false);
				else if (control_name == "edit_d_4d")
					update_parameters_4d(a_4d, b_4d, c_4d, d_4d = wcstof(text_a, &p), e_4d, false);
				else if (control_name == "edit_e_4d")
					update_parameters_4d(a_4d, b_4d, c_4d, d_4d, e_4d = wcstof(text_a, &p), false);
			}
			else if (control_name == "edit_xz_rotation" || control_name == "edit_yz_rotation" || control_name == "edit_xy_rotation" ||
				control_name == "edit_distance_4d")
			{
				GetWindowTextW(controls["edit_xz_rotation"].handle, text_a, 10);
				GetWindowTextW(controls["edit_yz_rotation"].handle, text_b, 10);
				GetWindowTextW(controls["edit_xy_rotation"].handle, text_c, 10);
				GetWindowTextW(controls["edit_distance_4d"].handle, text_d, 10);
				if (control_name == "edit_xz_rotation")
					update_rotation_4d(xz_4d = wcstof(text_a, &p), yz_4d, xy_4d, d0_4d, false);
				else if (control_name == "edit_yz_rotation")
					update_rotation_4d(xz_4d, yz_4d = wcstof(text_b, &p), xy_4d, d0_4d, false);
				else if (control_name == "edit_xy_rotation")
					update_rotation_4d(xz_4d, yz_4d, xy_4d = wcstof(text_c, &p), d0_4d, false);
				else if (control_name == "edit_distance_4d")
					update_rotation_4d(xz_4d, yz_4d, xy_4d, d0_4d = wcstof(text_d, &p), false);
			}
			else
			{
				GetWindowTextW(controls["edit_xz_rotation_p"].handle, text_a, 10);
				GetWindowTextW(controls["edit_yz_rotation_p"].handle, text_b, 10);
				GetWindowTextW(controls["edit_xy_rotation_p"].handle, text_c, 10);
				GetWindowTextW(controls["edit_distance_4d_p"].handle, text_d, 10);
				if (control_name == "edit_xz_rotation_p")
					update_rotation_4d_p(xz_4d_p = wcstof(text_a, &p), yz_4d_p, xy_4d_p, d0_4d_p, false);
				else if (control_name == "edit_yz_rotation_p")
					update_rotation_4d_p(xz_4d_p, yz_4d_p = wcstof(text_b, &p), xy_4d_p, d0_4d_p, false);
				else if (control_name == "edit_xy_rotation_p")
					update_rotation_4d_p(xz_4d_p, yz_4d_p, xy_4d_p = wcstof(text_c, &p), d0_4d_p, false);
				else if (control_name == "edit_distance_4d_p")
					update_rotation_4d_p(xz_4d_p, yz_4d_p, xy_4d_p, d0_4d_p = wcstof(text_d, &p), false);
			}
			return 0;
		}

		if (control_name == "button_4d")
		{
			for (auto it = controls.begin(); it != controls.end(); it++)
				if (std::find(it->second.info.begin(), it->second.info.end(), "panel_3D") != it->second.info.end())
					ShowWindow(it->second.handle, SW_HIDE);

			for (auto it = controls.begin(); it != controls.end(); it++)
				if (std::find(it->second.info.begin(), it->second.info.end(), "panel_4D") != it->second.info.end())
					ShowWindow(it->second.handle, SW_SHOW);

			if (display_mode == 3)
			{
				ShowWindow(controls["render_2d_window"].handle, SW_HIDE);
				ShowWindow(controls["render_3d_window"].handle, SW_HIDE);

				ShowWindow(controls["render_4d_projection"].handle, SW_SHOW);
				ShowWindow(controls["render_4d_intersection"].handle, SW_SHOW);
			}
			if (display_mode == 2)
			{
				ShowWindow(controls["big_2d_window"].handle, SW_HIDE);
				ShowWindow(controls["big_3d_window"].handle, SW_SHOW);
			}

			application->set_mode(Application::MODE_4D);
			view_3d = false;

			MSG msg;
			while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);

				if (msg.message == WM_QUIT)
					application->end = true;
			}
		}
		if (control_name == "button_3d")
		{
			for (auto it = controls.begin(); it != controls.end(); it++)
				if (std::find(it->second.info.begin(), it->second.info.end(), "panel_3D") != it->second.info.end())
					ShowWindow(it->second.handle, SW_SHOW);

			for (auto it = controls.begin(); it != controls.end(); it++)
				if (std::find(it->second.info.begin(), it->second.info.end(), "panel_4D") != it->second.info.end())
					ShowWindow(it->second.handle, SW_HIDE);

			if (display_mode == 3)
			{
				ShowWindow(controls["render_2d_window"].handle, SW_SHOW);
				ShowWindow(controls["render_3d_window"].handle, SW_SHOW);

				ShowWindow(controls["render_4d_projection"].handle, SW_HIDE);
				ShowWindow(controls["render_4d_intersection"].handle, SW_HIDE);
			}
			if (display_mode == 2)
			{
				ShowWindow(controls["big_2d_window"].handle, SW_SHOW);
				ShowWindow(controls["big_3d_window"].handle, SW_HIDE);
			}

			application->set_mode(Application::MODE_3D);
			view_3d = true;

			MSG msg;
			while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);

				if (msg.message == WM_QUIT)
					application->end = true;
			}
		}
		if (control_name == "button_load")
		{
			CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
			IFileOpenDialog *pFileOpen;
			CoCreateInstance(CLSID_FileOpenDialog, NULL, CLSCTX_ALL, IID_IFileOpenDialog, reinterpret_cast<void**>(&pFileOpen));
			IShellItem *psiFolder;

			WCHAR folder_base[1000];
			int bytes = GetModuleFileName(NULL, folder_base, 1000);
			if (bytes == 0)
				folder_base[0] = NULL;
			else
			{
				int pos = bytes - 1;
				for (pos = bytes - 1; pos >= 0; pos--)
					if (folder_base[pos] == L'\\')
						break;
				folder_base[pos + 1] = NULL;

			}
			LPWSTR folder =  wcscat(folder_base, view_3d ? L"data\\3D" : L"data\\4D");

			SHCreateItemFromParsingName(folder, NULL, IID_PPV_ARGS(&psiFolder));
			pFileOpen->SetFolder(psiFolder);
			pFileOpen->Show(NULL);
			
			IShellItem *pItem;
			HRESULT hr = pFileOpen->GetResult(&pItem);
			if (SUCCEEDED(hr))
			{
				PWSTR pszFilePath;
				hr = pItem->GetDisplayName(SIGDN_FILESYSPATH, &pszFilePath);
				view_3d ? application->load_scene_3d(pszFilePath) : application->load_scene_4d(pszFilePath);
				CoTaskMemFree(pszFilePath);
				pItem->Release();
			}
			psiFolder->Release();
			pFileOpen->Release();
			CoUninitialize();
		}
		if (control_name == "button_save")
		{
			CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
			IFileSaveDialog *pFileSave;
			CoCreateInstance(CLSID_FileSaveDialog, NULL, CLSCTX_ALL, IID_IFileSaveDialog, reinterpret_cast<void**>(&pFileSave));
			COMDLG_FILTERSPEC type_filter[1] = { {L"Object", view_3d ? L"*.obj" : L"*.obj4"} };
			HRESULT h = pFileSave->SetFileTypes(1, type_filter);
			pFileSave->Show(NULL);

			IShellItem *pItem;
			HRESULT hr = pFileSave->GetResult(&pItem);
			if (SUCCEEDED(hr))
			{
				PWSTR pszFilePath;
				hr = pItem->GetDisplayName(SIGDN_FILESYSPATH, &pszFilePath);
				wstring name_with_extension = wstring(pszFilePath);
				if (view_3d && name_with_extension.substr(name_with_extension.size() - 4, 4) != L".obj")
					name_with_extension += L".obj";
				if (!view_3d && name_with_extension.substr(name_with_extension.size() - 4, 4) != L".obj4")
					name_with_extension += L".obj4";
				view_3d ? application->save_scene_3d(name_with_extension.c_str()) : application->save_scene_4d(name_with_extension.c_str());
				CoTaskMemFree(pszFilePath);
				pItem->Release();
			}

			pFileSave->Release();
			CoUninitialize();
		}
		if (control_name == "button_reset")
			application->reset_parameters();
		if (control_name == "button_play")
			application->play_animation();
		if (control_name == "checkbox_wireframe_p")
			update_wireframe_p(!IsDlgButtonChecked(hwnd, controls["checkbox_wireframe_p"].id));			
		if (control_name == "checkbox_wireframe_i")
			update_wireframe_i(!IsDlgButtonChecked(hwnd, controls["checkbox_wireframe_i"].id));
		if (control_name == "checkbox_axis")
			update_axis(!IsDlgButtonChecked(hwnd, controls["checkbox_axis"].id));

		if (control_id == controls["radio_projection"].id || control_id == controls["radio_intersection"].id || 
			control_id == controls["radio_both"].id)
		{
			int nr = IsDlgButtonChecked(hwnd, controls["radio_projection"].id) ? 1 : 
				IsDlgButtonChecked(hwnd, controls["radio_intersection"].id) ? 2 : 3;
			update_radiogroup(nr);
		}

		return 0;

	case WM_CTLCOLORSTATIC:
		class_name = (*controls_by_hwnd[(HWND)lparam]).class_name;

		if (class_name == L"Static" || class_name == L"Edit" || class_name == L"Trackbar")
		{
			HDC hdcStatic = (HDC)wparam;
			SetTextColor(hdcStatic, RGB(0, 0, 0));
			SetBkColor(hdcStatic, RGB(255, 255, 255));
			static HBRUSH hBrush = CreateSolidBrush(RGB(255, 255, 255));
			return (INT_PTR)hBrush;
		}

	case WM_CTLCOLORBTN:
		class_name = (*controls_by_hwnd[(HWND)lparam]).class_name;

		if (class_name == L"Button")
		{
			HDC hdcStatic = (HDC)wparam;
			SetTextColor(hdcStatic, RGB(255, 255, 255));
			SetBkColor(hdcStatic, RGB(50, 150, 255));
			static HBRUSH hBrush = CreateSolidBrush(RGB(50, 150, 255));
			return (INT_PTR)hBrush;
		}
		return 0;
		
		//return (LRESULT)GetSysColorBrush(COLOR_WINDOW + 1);

	/*case WM_NOTIFY:
	{
		LPNMHDR notification = (LPNMHDR)lparam;
		class_name = controls_by_hwnd[notification->hwndFrom].class_name;
		if (notification->code == NM_CUSTOMDRAW && class_name == L"Button")
		{
			LPNMCUSTOMDRAW item = (LPNMCUSTOMDRAW)notification;

			static HBRUSH button_brush = CreateSolidBrush(RGB(255, 255, 255));

			HPEN pen = CreatePen(PS_INSIDEFRAME, 0, RGB(50, 150, 255));

			HGDIOBJ old_pen = SelectObject(item->hdc, pen);
			HGDIOBJ old_brush = SelectObject(item->hdc, button_brush);

			RoundRect(item->hdc, item->rc.left, item->rc.top, item->rc.right, item->rc.bottom, 10, 10);

			SelectObject(item->hdc, old_pen);
			SelectObject(item->hdc, old_brush);
			DeleteObject(pen);

			return CDRF_DODEFAULT;
		}
		return 0;
	}*/
	case WM_KILLFOCUS:
		selected_edit = controls_by_hwnd[(HWND)wparam];

		return 0;

	case WM_SIZE:
		width = LOWORD(lparam);
		height = HIWORD(lparam);
		resizing = true;
		if (resized_already && wparam == 0)
			return 0;

	case WM_EXITSIZEMOVE:
		if (width != 0 && height != 0 && resizing)
		{
			if (height < 700 || width < margin_left + margin_right + 100)
			{
				height = max(700 + 39, height);
				width = max(margin_left + margin_right + 100, width);
				SetWindowPos(hwnd, NULL, 0, 0, width, height, SWP_NOMOVE);
			}
			destroy_layout();
			create_layout();
			restore_values();

			MSG msg;
			while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);

				if (msg.message == WM_QUIT)
					application->end = true;
			}

			application->resize(width, height);
			resizing = false;
			resized_already = true;
		}
		return 0;

	default:
		return DefWindowProc(hwnd, umessage, wparam, lparam);
	}
}

