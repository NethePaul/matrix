#include "Message.h"
#include<assert.h>
using namespace D2D1;

void Message::set_text_(const std::wstring&str)
{
	SafeRelease(&text_layout);
	wf->CreateTextLayout(txt.c_str(), txt.length(), text_format, m_size.width*.9, m_size.height*.9, &text_layout);
	DWRITE_TRIMMING dwt{};
	dwt.granularity =
		DWRITE_TRIMMING_GRANULARITY::DWRITE_TRIMMING_GRANULARITY_CHARACTER;
	if (text_layout)text_layout->SetTrimming(&dwt, 0);
	RedrawWindow(m_hwnd, 0, 0, RDW_INVALIDATE);
}

Message::~Message()
{
	SafeRelease(&r);
	SafeRelease(&text_color);
	SafeRelease(&text_format);
	SafeRelease(&wf);
	SafeRelease(&text_layout);
}

Message::Message(HWND parent, ID2D1Factory * f, int x,int y,int w, int h, int rows) :rows(rows),f(f),text_format(0),text_layout(0),bck_color(0,0,0)
{
	Create(L"message_box", WS_CHILD|WS_CLIPCHILDREN|WS_CLIPSIBLINGS,WS_EX_RIGHTSCROLLBAR,parent,x,y,w,h);
	ShowWindow(m_hwnd, SW_SHOW); UpdateWindow(m_hwnd);
	//SetWindowPos(m_hwnd, HWND_TOP, 10, 10, 100, 100, SWP_SHOWWINDOW);
	auto hr=f->CreateHwndRenderTarget(
		D2D1::RenderTargetProperties(D2D1_RENDER_TARGET_TYPE_SOFTWARE),
		D2D1::HwndRenderTargetProperties(m_hwnd,D2D1::SizeU(w,h)),
		&r
	);
	assert(SUCCEEDED(hr));
	hr=r->CreateSolidColorBrush(ColorF(ColorF::LightGray),&text_color);
	assert(SUCCEEDED(hr));
	resize();
}

void Message::draw()
{
	if (!r)return;
	r->BeginDraw();
	r->Clear(bck_color);
	r->DrawRectangle(D2D1::RectF(m_size.width*0.0125, m_size.height*0.0125, m_size.width*(1-0.0125), m_size.height*(1 - 0.0125)), text_color, m_size.width*0.00625);
	/*
	r->DrawTextW(
		txt.c_str(),
		txt.length(),
		text_format,
		D2D1::RectF(m_size.width*0.05, m_size.height*0.05, m_size.width*(1 - 0.05), m_size.height*(1 - 0.05)),
		text_color

	);*/
	r->DrawTextLayout(D2D1::Point2F(m_size.width*0.05, m_size.height*0.05), text_layout, text_color);
	mid_draw();
	r->EndDraw();
}

void Message::resize()
{
	if (r) {
		RECT rc;
		GetClientRect(m_hwnd, &rc);

		m_size = D2D1::SizeF(rc.right, rc.bottom);
		SafeRelease(&text_format);
		IDW::CreateDeviceIndependentResources(f, &wf, &text_format, m_size.height*0.7 / double(rows));
		D2D1_SIZE_U sizeu = D2D1::SizeU(rc.right, rc.bottom);
		r->Resize(sizeu);
		
		calculate_layout();
	}
}

void Message::calculate_layout()
{
	RedrawWindow(m_hwnd, 0, 0, RDW_INVALIDATE);
}

void Message::set_color(D2D1::ColorF txt, D2D1::ColorF bck)
{
	text_color->SetColor(txt);
	bck_color=bck;
}

void Message::set_text(const std::wstring&txt)
{
	this->txt = txt;
	advanced = 0;
	set_text_(txt);
}

void Message::set_auto_destroy(bool p)
{
	auto_destroy = p;
}

LRESULT Message::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg) {
	case WM_LBUTTONUP:
		if (text_layout) {
			DWRITE_HIT_TEST_METRICS m{}; BOOL a, b;
			text_layout->HitTestPoint(text_layout->GetMaxWidth(), text_layout->GetMaxHeight(), &a, &b, &m);

			advanced += m.textPosition + 1;
			if (advanced >= txt.length()) {
				if (auto_destroy) { DestroyWindow(m_hwnd); break; }
				else {
					advanced = 0;
				}
			}
			set_text_(txt.c_str() + advanced);
		}
		break;
	case WM_DESTROY:
		if (on_destruction)on_destruction();
		return 0;
	case WM_NCDESTROY:
		delete this;
		return 0;
	case WM_PAINT:
		draw();
		break;
	case WM_SIZING:
	case WM_SIZE:
		resize();
		break;
	}
	return DefWindowProc(m_hwnd, uMsg, wParam, lParam);
}
