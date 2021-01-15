#include "input.h"

Input::Input()
{
	for(int i=0; i<256; i++)
		keys[i] = false;

	bleft_down = old_left_down = false;
	bright_down = old_right_down = false;
	bmiddle_down = old_middle_down = false;

	mouse_x = old_mouse_x = 0;
	mouse_y = old_mouse_y = 0;
	mouse_dx = 0;
	mouse_dy = 0;
	mouse_scroll = old_mouse_scroll = 0;
	scroll_delta = 0;

	bmouse_moved = false;
	bmouse_right_clicked = false;
	bmouse_left_clicked = false;
	bmouse_middle_clicked = false;
}

void Input::key_down(unsigned int input) { keys[input] = true; }
void Input::key_up(unsigned int input) { keys[input] = false; }
void Input::mouse_move(int x, int y) { mouse_x = x; mouse_y = y; }
void Input::left_down() { bleft_down = true; }
void Input::right_down() { bright_down = true; }
void Input::middle_down() { bmiddle_down = true; }
void Input::left_up() { bleft_down = false; }
void Input::right_up() { bright_down = false; }
void Input::middle_up() { bmiddle_down = false; }
void Input::scroll_change(int delta) { mouse_scroll += delta; }

void Input::update()
{
	bmouse_moved = (mouse_x != old_mouse_x || mouse_y != old_mouse_y);
	bmouse_left_clicked = (bleft_down && !old_left_down);
	bmouse_right_clicked = (bright_down && !old_right_down);
	bmouse_middle_clicked = (bmiddle_down && !old_middle_down);
	mouse_dx = mouse_x - old_mouse_x;
	mouse_dy = mouse_y - old_mouse_y;
	scroll_delta = mouse_scroll - old_mouse_scroll;

	old_mouse_x = mouse_x;
	old_mouse_y = mouse_y;
	old_left_down = bleft_down;
	old_right_down = bright_down;
	old_middle_down = bmiddle_down;
	old_mouse_scroll = mouse_scroll;
}

bool Input::is_mouse_left_down() { return bleft_down; }
bool Input::is_mouse_right_down() { return bright_down; }
bool Input::is_mouse_middle_down() { return bmiddle_down; }
bool Input::is_key_down(unsigned int key) { return keys[key]; }
bool Input::mouse_moved() { return bmouse_moved; }
bool Input::mouse_right_clicked() { return bmouse_right_clicked; }
bool Input::mouse_left_clicked() { return bmouse_left_clicked; }
bool Input::mouse_middle_clicked() { return bmouse_middle_clicked; }
bool Input::mouse_dragged() { return mouse_moved() && is_mouse_left_down(); }