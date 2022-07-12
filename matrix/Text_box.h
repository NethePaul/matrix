#pragma once
#include "Message.h"
class Text_box :
	public Message
{
	int cpos = 0;
	bool render_cursor = 0;
	ID2D1SolidColorBrush*cursor;
	HWND focus_owner = 0;
public:
	std::function<void(const std::wstring&in)>on_enter;
	std::function<void(const std::wstring&in)>on_update;
	std::function<void(const std::wstring&in)>on_escape;
	~Text_box();
	Text_box(HWND parent, ID2D1Factory*f, int x, int y, int w, int h, int r);

	void mid_draw()override;
	
	LPCWSTR ClassName()const override { return L"Text_box_212572"; };
	LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) override;
};

