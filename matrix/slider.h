#pragma once
#include "cbasewin.h"
class Slider :
	public CBaseWin<Slider>{
	ID2D1HwndRenderTarget*r = 0;
	D2D1_SIZE_F m_size;
	double state = 0;//from 0 to 1
	ID2D1SolidColorBrush*sc,*sc_held,*sc_hover;
	D2D1::ColorF bck_color;
	ID2D1Factory*f;
	enum class mouse_state {
		none,
		holding,
		hovering
	}ms=mouse_state::none;
public:
	Slider(HWND parent, ID2D1Factory*f, double x, double y, double w, double h, const std::function<void(double state)>& on_update);
	~Slider();
	std::function<void(double state)>on_update;
	void set_color(D2D1::ColorF sc, D2D1::ColorF held, D2D1::ColorF sc_hover, D2D1::ColorF bck);
	void draw();
	void set_state(double state);//between 0 and 1
	LPCWSTR ClassName()const override{ return L"Slider_52784128"; };
	LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)override;
};

