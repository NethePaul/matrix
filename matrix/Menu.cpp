#include "Menu.h"

void Menu::destroy_all()
{
	for (auto&sub : sub_windows) {
		DestroyWindow(sub->get_hwnd());
	}
	sub_windows.clear();
	if (!open)return;
	open = false;
	if (close_menu_cb)close_menu_cb();
}

Menu::Menu(HWND parent, ID2D1Factory * f, D2D1::Matrix3x2F zoom)
{
	this->close_cb = []() {};
	this->close_menu_cb = []() {};
	this->edit_ship_cb = []() {};
	this->return_home_cb = []() {};
	auto p=zoom.TransformPoint({ WIDTH,HEIGHT });
	
	auto a = new Button(parent, f, p.x*.3, p.y*.1, p.x*.4, p.y*.1, 2);
	a->set_text(L"continue"); a->set_color(D2D1::ColorF(0, 1, 0),D2D1::ColorF::Black); a->on_click=([this]() {
		destroy_all();
	});
	sub_windows.push_back(a);
	
	a = new Button(parent, f, p.x*.3, p.y*.3, p.x*.4, p.y*.1, 2);
	a->set_text(L"edit ship"); a->set_color(D2D1::ColorF(0, 1, 0), D2D1::ColorF::Black); a->on_click=(([this]() {
		if (edit_ship_cb)edit_ship_cb();
		destroy_all();
	}));
	sub_windows.push_back(a);

	a = new Button(parent, f, p.x*.3, p.y*.7, p.x*.4, p.y*.1, 2);
	a->set_text(L"close"); a->set_color(D2D1::ColorF(1, 0, 0), D2D1::ColorF::Black); a->on_click=(([this]() {
		if (close_cb)close_cb();
		destroy_all();
	}));
	sub_windows.push_back(a);

	a = new Button(parent, f, p.x*.3, p.y*.5, p.x*.4, p.y*.1, 2);
	a->set_text(L"home"); a->set_color(D2D1::ColorF(1, 0, 0), D2D1::ColorF::Black); a->on_click=(([this]() {
		if (return_home_cb)return_home_cb();		
		destroy_all();
	}));
	sub_windows.push_back(a);
}
Menu::~Menu()
{
	destroy_all();
}
