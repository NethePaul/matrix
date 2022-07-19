#include "Scrollbar.h"
#include <assert.h>

Scrollbar::~Scrollbar()
{
	SafeRelease(&r);
	SafeRelease(&sb_held);
	SafeRelease(&sb_hover);
	SafeRelease(&sb_color);
}

Scrollbar::Scrollbar(HWND parent, ID2D1Factory * f, int x, int y, int w, int h, int inner_h) :f(f), inner_h(inner_h), bck_color(D2D1::ColorF::LightGray), scroll_state(0)
{
	if (inner_h < h)inner_h = h;
	Create(L"Scrollbar", WS_CHILD | WS_CLIPCHILDREN | WS_CLIPSIBLINGS | WS_VISIBLE, WS_EX_RIGHTSCROLLBAR, parent, x, y, w, h);
	ShowWindow(m_hwnd, SW_SHOW); UpdateWindow(m_hwnd);
	auto hr = f->CreateHwndRenderTarget(
		D2D1::RenderTargetProperties(D2D1_RENDER_TARGET_TYPE_SOFTWARE),
		D2D1::HwndRenderTargetProperties(m_hwnd, D2D1::SizeU(w, h)),
		&r
	);
	assert(r);
	assert(SUCCEEDED(hr));
	using namespace D2D1;
	hr = r->CreateSolidColorBrush(ColorF(ColorF::DarkGray), &sb_color);
	assert(SUCCEEDED(hr));
	hr = r->CreateSolidColorBrush(ColorF(ColorF::WhiteSmoke), &sb_hover);
	assert(SUCCEEDED(hr));
	hr = r->CreateSolidColorBrush(ColorF(ColorF::Azure), &sb_held);
	assert(SUCCEEDED(hr));
	resize();
}

void Scrollbar::set_inner_h(double inner)
{
	if (inner <= m_size.height) { scroll_state = 0; inner = m_size.height; ShowWindow(m_hwnd, SW_HIDE); }
	else if (inner_h <= m_size.height) { ShowWindow(m_hwnd, SW_SHOW); }
	scroll_state *= inner_h / inner;
	if (scroll_state < 0)scroll_state = 0;
	if (scroll_state > 1)scroll_state = 1;
	if (on_update)on_update(scroll_state);
	inner_h = inner;
	RedrawWindow(m_hwnd, 0, 0, RDW_INVALIDATE);
}

void Scrollbar::draw()
{
	if (!r)return;
	r->BeginDraw();
	r->Clear(bck_color);
	ID2D1Brush*b;
	switch (ms)
	{
	default:
	case Scrollbar::mouse_state::none:
		b = sb_color;
		break;
	case Scrollbar::mouse_state::holding:
		b = sb_held;
		break;
	case Scrollbar::mouse_state::hovering:
		b = sb_hover;
		break;
	}
	ratio = m_size.height / inner_h;
	sb_height = m_size.height*ratio;
	min_h = (m_size.height - sb_height)*scroll_state;
	r->FillRectangle(D2D1::RectF(0, min_h, m_size.width, sb_height + min_h), b);

	mid_draw();
	r->EndDraw();
}

void Scrollbar::resize()
{
	if (r) {
		RECT rc;
		GetClientRect(m_hwnd, &rc);

		m_size = D2D1::SizeF(rc.right, rc.bottom);

		D2D1_SIZE_U sizeu = D2D1::SizeU(rc.right, rc.bottom);
		r->Resize(sizeu);
		set_inner_h(inner_h);
	}
}

void Scrollbar::set_color(D2D1::ColorF bck, D2D1::ColorF color, D2D1::ColorF hover, D2D1::ColorF held)
{
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
	this->bck_color = bck;

	RedrawWindow(m_hwnd, 0, 0, RDW_INVALIDATE);
}

void Scrollbar::update()
{
	if(on_update)on_update(this->scroll_state);
	RedrawWindow(m_hwnd, 0, 0, RDW_INVALIDATE);
}

LRESULT Scrollbar::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
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
			SetCapture(m_hwnd);

			scroll_state = (cp.y - sb_height / 2) / (m_size.height - sb_height);
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
		else if (GetCapture() != m_hwnd)
		{
			// mouse has just entered your window
			SetCapture(m_hwnd);
			ms = mouse_state::hovering;
			RedrawWindow(m_hwnd, 0, 0, RDW_INVALIDATE);
		}
		else //else or in case of button up
		{
	case WM_LBUTTONUP:GetCursorPos(&cp); ScreenToClient(m_hwnd, &cp);
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
	return DefWindowProc(m_hwnd, uMsg, wParam, lParam);
}
