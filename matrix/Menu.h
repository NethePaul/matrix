#pragma once
#include"Button.h"
#include"utility.h"
#include<vector>

class Game;
class Menu
{
	std::vector<Get_HWND*>sub_windows;
	bool open = true;
public:
	operator bool()const { 
		return open;
	}
	std::function<void(void)>close_menu_cb=0,close_cb=0,return_home_cb=0,edit_ship_cb=0;
	void destroy_all();
	Menu(HWND parent, ID2D1Factory*f, D2D1::Matrix3x2F zoom);
	~Menu();
};

