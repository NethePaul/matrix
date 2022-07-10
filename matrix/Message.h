#pragma once
#include"cbasewin.h"
#include"pch.h"
class Message :
	public CBaseWin<Message>
{
protected:
	ID2D1HwndRenderTarget*r=0;
	D2D1_SIZE_F m_size;
	double x, y;int rows;
	std::wstring txt;
	ID2D1SolidColorBrush*text_color;
	D2D1::ColorF bck_color;
	IDWriteTextFormat*text_format;
	IDWriteTextLayout*text_layout;
	IDWriteFactory*wf;
	ID2D1Factory*f;
public:
	std::function<void(void)>on_destruction;
	~Message();
	Message(HWND parent, ID2D1Factory*f, int x, int y, int w, int h, int r);

	void draw();
	void resize();
	void calculate_layout();
	void set_color(D2D1::ColorF txt, D2D1::ColorF bck);
	void set_text(std::wstring txt);


	
	LPCWSTR ClassName()const override{ return L"Message_54352"; };
	LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) override;
};

