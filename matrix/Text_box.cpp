#include "Text_box.h"

Text_box::~Text_box()
{
	SafeRelease(&cursor);
}

Text_box::Text_box(HWND parent, ID2D1Factory * f, int x, int y, int w, int h, int rows):Message(parent,f,x,y,w,h,rows)
{
	set_auto_destroy(false);
	r->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Azure),&cursor);
}

void Text_box::mid_draw()
{
	FLOAT x=0, y=0;
	DWRITE_HIT_TEST_METRICS m{};
	if (cpos > 0)
		text_layout->HitTestTextPosition(cpos - 1, true, &x, &y, &m);
	else text_layout->HitTestTextPosition(0, 0, &x, &y, &m);
	x += m_size.width*0.05; y += m_size.height*0.05;
	DWRITE_TEXT_METRICS m2{};
	text_layout->GetMetrics(&m2);
	if(render_cursor)
		r->DrawLine(D2D1::Point2F(x, y), D2D1::Point2F(x, y + m2.height), cursor, m_size.width*0.003125);
	
}

LRESULT Text_box::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
#define update set_text_(txt);if(on_update)on_update(txt);
	switch (uMsg)
	{
	case WM_DESTROY:
		KillTimer(m_hwnd, 1);
		break;
	case WM_TIMER:
		if (GetFocus() != m_hwnd) { KillTimer(m_hwnd, 1); render_cursor = 0; }
		else render_cursor = !render_cursor;
		RedrawWindow(m_hwnd, 0, 0, RDW_INVALIDATE);
		return 0;
	case WM_CHAR:
		if(iswprint(wParam))
			txt.insert(txt.begin() + cpos++, (wchar_t)wParam);
		update;
		return 0;
	case WM_KEYDOWN:
		switch (wParam)
		{
		case VK_BACK:
			if (cpos > 0) {
				txt.erase(txt.begin() + --cpos);
				update;
			}
			else {
			case VK_DELETE:
				if (txt.length() > cpos) {
					txt.erase(txt.begin() + cpos);
					update;
					
				}
			}
			break;
		case VK_ESCAPE:SetFocus(focus_owner); if (on_escape) on_escape(txt); render_cursor = 0; RedrawWindow(m_hwnd, 0, 0, RDW_INVALIDATE);
			break;
		case VK_RETURN:SetFocus(focus_owner);if(on_enter) on_enter(txt); render_cursor = 0; RedrawWindow(m_hwnd, 0, 0, RDW_INVALIDATE);
			break;
		case VK_LEFT:
			if (cpos)cpos--;
			render_cursor = true;
			RedrawWindow(m_hwnd, 0, 0, RDW_INVALIDATE);
			break;
		case VK_RIGHT:
			if (cpos < txt.length())cpos++;
			render_cursor = true;
			RedrawWindow(m_hwnd, 0, 0, RDW_INVALIDATE);
			break;
		}
		return 0;
	case WM_LBUTTONDOWN:
		focus_owner=SetFocus(m_hwnd);
		SetTimer(m_hwnd, 1, 1000, 0);
		if (text_layout) {
			DWRITE_HIT_TEST_METRICS m{}; BOOL trailing, inside;
			auto x = GET_X_LPARAM(lParam);
			auto y = GET_Y_LPARAM(lParam);
			x -= m_size.width*0.05; y -= m_size.height*0.05;
			text_layout->HitTestPoint(x,y , &trailing, &inside, &m);
			
			cpos = m.textPosition + (trailing||inside);
			render_cursor = true;
			RedrawWindow(m_hwnd, 0, 0, RDW_INVALIDATE);
		}
		return 0;
	case WM_ACTIVATE:
		if (wParam == WA_INACTIVE) {
			render_cursor = false;
		}
		return 0;
	}
	return Message::HandleMessage(uMsg,wParam,lParam);
}
