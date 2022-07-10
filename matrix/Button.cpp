#include "Button.h"

Button::Button(HWND parent, ID2D1Factory * f, int x, int y, int w, int h, int rows):Message(parent,f,x,y,w,h,rows),hover_bck_color(.1,.1,.1),def_bck_color(0,0,0)
{
	bck_color = def_bck_color;
}

LRESULT Button::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_MOUSEMOVE:
	{
		if (GetCapture() != m_hwnd)
		{
			// mouse has just entered your window
			SetCapture(m_hwnd);
			bck_color = hover_bck_color;
			RedrawWindow(m_hwnd, 0, 0, RDW_INVALIDATE);
		}
		else
		{
			RECT rect;
			GetWindowRect(m_hwnd, &rect);

			POINT pt = { LOWORD(lParam), HIWORD(lParam) };
			ClientToScreen(m_hwnd, &pt);

			if (!PtInRect(&rect, pt))
			{
				// mouse has just left your window, here you releaseCapture()
				bck_color = def_bck_color;
				RedrawWindow(m_hwnd, 0, 0, RDW_INVALIDATE);
				ReleaseCapture();
			}
		}
	}
	break;
	case WM_LBUTTONUP:
		if (on_click)on_click();
		return 0;
	default:return Message::HandleMessage(uMsg, wParam, lParam);
	}
	return LRESULT();
}

Button::~Button()
{
}
