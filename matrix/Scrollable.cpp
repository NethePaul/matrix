#include "Scrollable.h"
#include <assert.h>

Scrollable::~Scrollable()
{
	SafeRelease(&r);
	SafeRelease(&sb_bck);
	SafeRelease(&sb_held);
	SafeRelease(&sb_hover);
	SafeRelease(&sb_color);
}

Scrollable::Scrollable(HWND parent, ID2D1Factory * f, int x, int y, int w, int h, int inner_h):f(f),bck_color(.1,.1,.1),inner_h(inner_h),scroll_state(0)
{
	assert(h <= inner_h);
	Create(L"scrollable", WS_CHILD | WS_CLIPCHILDREN | WS_CLIPSIBLINGS | WS_VISIBLE, WS_EX_RIGHTSCROLLBAR, parent, x, y, w, h);
	ShowWindow(m_hwnd, SW_SHOW); UpdateWindow(m_hwnd);
	auto hr = f->CreateHwndRenderTarget(
		D2D1::RenderTargetProperties(D2D1_RENDER_TARGET_TYPE_SOFTWARE),
		D2D1::HwndRenderTargetProperties(m_hwnd, D2D1::SizeU(w, h)),
		&r
	);
	assert(SUCCEEDED(hr));
	using namespace D2D1;
	hr = r->CreateSolidColorBrush(ColorF(ColorF::DarkGray), &sb_color);
	assert(SUCCEEDED(hr));
	hr = r->CreateSolidColorBrush(ColorF(ColorF::WhiteSmoke), &sb_hover);
	assert(SUCCEEDED(hr));
	hr = r->CreateSolidColorBrush(ColorF(ColorF::Azure), &sb_held);
	assert(SUCCEEDED(hr));
	hr = r->CreateSolidColorBrush(ColorF(ColorF::LightGray), &sb_bck);
	assert(SUCCEEDED(hr));
	resize();
}

void Scrollable::draw()
{
	if (!r)return;
	r->BeginDraw();
	r->Clear(bck_color);
	ID2D1Brush*b;
	switch (ms)
	{
	default:
	case Scrollable::mouse_state::none:
		b = sb_color;
		break;
	case Scrollable::mouse_state::holding:
		b = sb_held;
		break;
	case Scrollable::mouse_state::hovering:
		b = sb_hover;
		break;
	}
	r->FillRectangle(D2D1::RectF(m_size.width*.95, 0, m_size.width, m_size.height), sb_bck);
	ratio = m_size.height / inner_h;
	sb_height = m_size.height*ratio;
	min_h = (m_size.height - sb_height)*scroll_state;
	r->FillRectangle(D2D1::RectF(m_size.width*.95, min_h, m_size.width, sb_height+min_h), b);

	mid_draw();
	r->EndDraw();
}

void Scrollable::resize()
{
	if (r) {
		RECT rc;
		GetClientRect(m_hwnd, &rc);

		m_size = D2D1::SizeF(rc.right, rc.bottom);
		
		D2D1_SIZE_U sizeu = D2D1::SizeU(rc.right, rc.bottom);
		r->Resize(sizeu);

		RedrawWindow(m_hwnd, 0, 0, RDW_INVALIDATE|RDW_ALLCHILDREN);
	}
}

void Scrollable::set_color(D2D1::ColorF bck)
{
	this->bck_color = bck;
}

void Scrollable::set_scrollbar_color(D2D1::ColorF bck, D2D1::ColorF color, D2D1::ColorF hover, D2D1::ColorF held)
{
	SafeRelease(&sb_bck);
	SafeRelease(&sb_held);
	SafeRelease(&sb_hover);
	SafeRelease(&sb_color);
	
	using namespace D2D1;
	auto hr = r->CreateSolidColorBrush(ColorF(ColorF::DarkGray), &sb_color);
	assert(SUCCEEDED(hr));
	hr = r->CreateSolidColorBrush(ColorF(ColorF::WhiteSmoke), &sb_hover);
	assert(SUCCEEDED(hr));
	hr = r->CreateSolidColorBrush(ColorF(ColorF::Azure), &sb_held);
	assert(SUCCEEDED(hr));
	hr = r->CreateSolidColorBrush(ColorF(ColorF::LightGray), &sb_bck);
	assert(SUCCEEDED(hr));

	RedrawWindow(m_hwnd, 0, 0, RDW_INVALIDATE|RDW_ALLCHILDREN);
}

void Scrollable::update()
{
	double min_h2 = (m_size.height - sb_height)*scroll_state;
	double d = min_h2 - min_h;
	min_h = min_h2;
	struct a {
		double d;
		Scrollable*t;
	}p{d,this};
	WNDENUMPROC f = [](HWND hwnd, LPARAM p) {
		auto s = (a*)p;
		if (!hwnd)return FALSE;
		RECT r;
		GetWindowRect(hwnd, &r);
		ScreenToClient(s->t->m_hwnd, reinterpret_cast<POINT*>(&r.left)); 
		ScreenToClient(s->t->m_hwnd, reinterpret_cast<POINT*>(&r.right));
		r.top -= s->d;
		r.bottom -= s->d;
		SetWindowPos(hwnd, 0, r.left, r.top, r.right - r.left, r.bottom - r.top,SWP_SHOWWINDOW);
		return TRUE;
	};
	EnumChildWindows(m_hwnd, f, (LPARAM)&p);
	RedrawWindow(m_hwnd, 0, 0, RDW_INVALIDATE | RDW_ALLCHILDREN);
}

LRESULT Scrollable::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	POINT cp;
	switch (uMsg)
	{
	case WM_NCDESTROY:delete this; return 0;
	case WM_PAINT:draw(); break;
	case WM_SIZE:
	case WM_SIZING:
		resize();
		break;
	case WM_LBUTTONDOWN:
		if (ms == mouse_state::hovering) {
			
			GetCursorPos(&cp); ScreenToClient(m_hwnd, &cp);
			if (cp.x < m_size.width*.95)break;
			
			SetCapture(m_hwnd);

			scroll_state = (cp.y-sb_height/2) / (m_size.height-sb_height);
			if (scroll_state > 1)scroll_state = 1;
			if (scroll_state < 0)scroll_state = 0;
			update();
			ms = mouse_state::holding;
		}
		return 0;

	case WM_MOUSEMOVE:
	{
		GetCursorPos(&cp); ScreenToClient(m_hwnd, &cp);
		if (ms == mouse_state::holding) {
			scroll_state = (cp.y - sb_height / 2) / (m_size.height - sb_height);
			if (scroll_state > 1)scroll_state = 1;
			if (scroll_state < 0)scroll_state = 0;
			update();
		}
		else if (GetCapture() != m_hwnd&& cp.x > m_size.width*.95)
		{
			// mouse has just entered your window
			SetCapture(m_hwnd);
			ms = mouse_state::hovering;
			RedrawWindow(m_hwnd, 0, 0, RDW_INVALIDATE | RDW_ALLCHILDREN);
		}
		else //else or in case of button up
		{
	case WM_LBUTTONUP:GetCursorPos(&cp); ScreenToClient(m_hwnd, &cp);
		ms = mouse_state::hovering;
		RECT rect;
		GetWindowRect(m_hwnd, &rect);

		POINT pt = { LOWORD(lParam), HIWORD(lParam) };
		ClientToScreen(m_hwnd, &pt);

		if (!PtInRect(&rect, pt)||cp.x<m_size.width*.95)
		{
			// mouse has just left your window, here you releaseCapture()
			ms = mouse_state::none;
			RedrawWindow(m_hwnd, 0, 0, RDW_INVALIDATE|RDW_ALLCHILDREN);
			ReleaseCapture();
		}
		}
	}
	return 0;
	default:
		break;
	}
return DefWindowProc(m_hwnd, uMsg, wParam, lParam);
}
