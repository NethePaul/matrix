#include "slider.h"
#include<assert.h>
#include<algorithm>

Slider::Slider(HWND parent,ID2D1Factory*f,double x,double y, double w, double h, const std::function<void(double state)>& on_update)
	:on_update(on_update),
	f(f),
	bck_color(.1,.1,.1)
{
	Create(L"slider", WS_CHILD | WS_CLIPCHILDREN | WS_CLIPSIBLINGS, WS_EX_RIGHTSCROLLBAR, parent, x, y, w, h);
	ShowWindow(m_hwnd, SW_SHOW); UpdateWindow(m_hwnd);

	assert(f);
#define assert2(a)assert(SUCCEEDED(a))
	assert2(f->CreateHwndRenderTarget(
		D2D1::RenderTargetProperties(D2D1_RENDER_TARGET_TYPE_SOFTWARE),
		D2D1::HwndRenderTargetProperties(m_hwnd, D2D1::SizeU(w, h)),
		&r
	));
	assert2(r->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::LightGray),&sc));
	assert2(r->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::DarkGray), &sc_hover));
	assert2(r->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Azure), &sc_held));
	resize();
}

Slider::~Slider()
{
	SafeRelease(&sc);
	SafeRelease(&sc_held);
	SafeRelease(&sc_hover);
	SafeRelease(&r);
}

void Slider::set_color(D2D1::ColorF sc, D2D1::ColorF held, D2D1::ColorF sc_hover,D2D1::ColorF bck)
{
	bck_color = bck;

}

void Slider::draw()
{
	if (!r)return;
	r->BeginDraw();
	r->Clear(bck_color);
	r->FillRoundedRectangle(D2D1::RoundedRect(
		D2D1::RectF(m_size.width*.1, m_size.height*.4, m_size.width*.9, m_size.height*.6),
		m_size.height*.04, m_size.height*.04),//rx,ry
		sc);
	auto b = sc;
	switch (ms)
	{
	case mouse_state::holding:b = sc_held; break;
	case mouse_state::hovering:b = sc_hover; break;
	};
#undef min
	auto ra = std::min<double>(m_size.height*.5, m_size.width*.1);
	r->FillEllipse(D2D1::Ellipse(D2D1::Point2F(m_size.width*(state*.8+.1), m_size.height*.5),ra, ra), b);
	r->EndDraw();
}

void Slider::set_state(double state)
{
	if (ms == mouse_state::holding) {
		SetCapture(0);
		ms = mouse_state::none;
	}
	this->state = state;
}

void Slider::resize()
{
	if (r) {
		RECT rc;
		GetClientRect(m_hwnd, &rc);

		m_size = D2D1::SizeF(rc.right, rc.bottom);
		D2D1_SIZE_U sizeu = D2D1::SizeU(rc.right, rc.bottom);
		r->Resize(sizeu);

	}
	RedrawWindow(m_hwnd, 0, 0, RDW_INVALIDATE);
}

LRESULT Slider::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	POINT cp;
	switch (uMsg)
	{
	case WM_NCDESTROY:delete this; return 0;
	case WM_PAINT:draw(); return 0;
	case WM_SIZE:
	case WM_SIZING:
		resize();
		break;
	case WM_LBUTTONDOWN:
		if (ms==mouse_state::hovering) {
			SetCapture(m_hwnd);
			GetCursorPos(&cp); ScreenToClient(m_hwnd, &cp);
			state = (cp.x / m_size.width - .1) / .8;
			if (state > 1)state = 1;
			if (state < 0)state = 0;
			if (on_update)on_update(state);
			ms = mouse_state::holding;
		}
		return 0;
	
	case WM_MOUSEMOVE:
	{
		if (ms == mouse_state::holding) {
			GetCursorPos(&cp); ScreenToClient(m_hwnd, &cp);
			state = (cp.x / m_size.width - .1) / .8;
			if (state > 1)state = 1;
			if (state < 0)state = 0;
			if (on_update)on_update(state);
		}
		else if (GetCapture() != m_hwnd)
		{
			// mouse has just entered your window
			SetCapture(m_hwnd);
			ms = mouse_state::hovering;
			RedrawWindow(m_hwnd, 0, 0, RDW_INVALIDATE);
		}
		else //else or in case of button up
		{
		case WM_LBUTTONUP:
			ms = mouse_state::hovering;
			RECT rect;
			GetWindowRect(m_hwnd, &rect);

			POINT pt = { LOWORD(lParam), HIWORD(lParam) };
			ClientToScreen(m_hwnd, &pt);

			if (!PtInRect(&rect, pt))
			{
				// mouse has just left your window, here you releaseCapture()
				ms = mouse_state::none;
				RedrawWindow(m_hwnd, 0, 0, RDW_INVALIDATE);
				ReleaseCapture();
			}
		}
	}
	return 0;
	default:
		break;
	}
	return DefWindowProc(m_hwnd,uMsg,wParam,lParam);
}
