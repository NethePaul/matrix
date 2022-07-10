#pragma once
#include"Message.h"
//warning only deallocates if WM_DESTROY is reveived
class Button:public Message
{
	D2D1::ColorF def_bck_color,hover_bck_color;
public:
	std::function<void(void)>on_click;
	Button(HWND parent, ID2D1Factory*f, int x, int y, int w, int h, int r);
	LPCWSTR ClassName()const override { return L"Button_54353"; };
	LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) override;
	~Button();
};

