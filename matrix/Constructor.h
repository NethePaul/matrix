#pragma once
#include"Ship.h"
class Game;
class Constructor
{
	std::vector<std::shared_ptr<Object>>components_selectable;
	std::shared_ptr<Object>selected_l;
	std::weak_ptr<Object>selected_r;
	std::weak_ptr<Object>constructee_selected;
	std::shared_ptr<Object>working_on_;//ensures working_on will not be deallocated
	D2D1_SIZE_F m_size;
	Ship*working_on;
	Game*context;
	POINT cp;
	std::weak_ptr<Object>closest;
	D2D1::Matrix3x2F&last;
	D2D1::Matrix3x2F last_inverted;
	D2D1::Matrix3x2F selectable_inverted;
	bool mouse_in_working_area=0;
	bool valid = 0;
public:
	operator bool()const { return valid; }
	std::function<void(void)> on_finish;
	Constructor(D2D1::Matrix3x2F&last,Game*context);
	~Constructor();
	//will return false if obj is not a Ship
	bool set_working_on(std::shared_ptr<Object>obj);
	void init();
	
	
	void draw(ID2D1RenderTarget*);
	void draw_selection(ID2D1RenderTarget*);
	void input(UINT msg, POINT cp, WPARAM wParam, LPARAM lParam, int scroll, HWND main);
	void update_closest();

};

