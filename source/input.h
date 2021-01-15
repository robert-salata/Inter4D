#ifndef _INPUTCLASS_H_
#define _INPUTCLASS_H_

class Input
{
public:
	Input();
	void key_down(unsigned int);
	void key_up(unsigned int);
	void mouse_move(int x, int y);
	void left_down();
	void right_down();
	void middle_down();
	void left_up();
	void right_up();
	void middle_up();
	void scroll_change(int delta);

	void update();
	bool is_key_down(unsigned int);
	bool is_mouse_left_down();
	bool is_mouse_right_down();
	bool is_mouse_middle_down();
	bool mouse_dragged();
	bool mouse_moved();
	bool mouse_right_clicked();
	bool mouse_left_clicked();
	bool mouse_middle_clicked();

	int mouse_x;
	int mouse_y;
	int mouse_dx;
	int mouse_dy;
	int scroll_delta;

private:
	bool keys[256];
	bool bleft_down;
	bool bright_down;
	bool bmiddle_down;
	int mouse_scroll;

	int old_mouse_x;
	int old_mouse_y;
	int old_mouse_scroll;
	bool old_left_down;
	bool old_right_down;
	bool old_middle_down;

	bool bmouse_moved;
	bool bmouse_right_clicked;
	bool bmouse_left_clicked;
	bool bmouse_middle_clicked;

};

#endif