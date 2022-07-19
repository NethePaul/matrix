#pragma once
#include "cbasewin.h"
class Scrollbar :
	public CBaseWin< Scrollbar>
{
	ID2D1HwndRenderTarget*r = 0;
	ID2D1SolidColorBrush *sb_color, *sb_hover, *sb_held;
	D2D1_SIZE_F m_size;
	D2D1::ColorF bck_color;
	ID2D1Factory*f;
	double inner_h,
		scroll_state;//between 0 and 1
	double ratio = 0;
	double sb_height = 0;
	double min_h = 0;
	enum class mouse_state {
		none,
		holding,
		hovering
	}ms = mouse_state::none;
public:
	std::function<void(double)>on_update=0;
	~Scrollbar();
	Scrollbar(HWND parent, ID2D1Factory*f, int x, int y, int w, int h, int inner_h);
	void set_inner_h(double inner);

	void draw();
	virtual void mid_draw() {};
	void resize();

	void set_color(D2D1::ColorF bck, D2D1::ColorF color, D2D1::ColorF hover, D2D1::ColorF held);
	void update();


	LPCWSTR ClassName()const override { return L"Scrollbar_98723"; };
	LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) override;
};

