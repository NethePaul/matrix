#pragma once
#include"pch.h"

class Get_HWND {
protected:
	HWND m_hwnd=0;
public:
	HWND get_hwnd()const { return m_hwnd; };
};
template<class DERIVED_TYPE>
class CBaseWin:public Get_HWND {
public:
	static LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
		DERIVED_TYPE *pThis = NULL;
		if (uMsg == WM_NCCREATE) {
			CREATESTRUCT* pCreate = (CREATESTRUCT*)lParam;
			pThis = (DERIVED_TYPE*)pCreate->lpCreateParams;
			SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)pThis);

			pThis->m_hwnd = hwnd;
		}
		else
		{
			pThis = (DERIVED_TYPE*)GetWindowLongPtr(hwnd, GWLP_USERDATA);
		}
		if (pThis) {
			return pThis->HandleMessage(uMsg, wParam, lParam);
		}
		else
		{
			return DefWindowProc(hwnd, uMsg, wParam, lParam);
		}
	}
	CBaseWin() {};
	BOOL Create
	(
		LPCWSTR lpWindowName,
		DWORD dwStyle,
		DWORD dwExStyle = 0,
		HWND parentWindow = 0,
		int x=0,int y=0,int w=-1,int h=-1, HMENU hMenu = 0
	)
	{
		WNDCLASS wc = { 0 };

		wc.lpfnWndProc = DERIVED_TYPE::WindowProc;
		wc.hInstance = GetModuleHandle(NULL);
		wc.lpszClassName = ClassName();
		wc.hCursor = (HCURSOR)LoadCursor(NULL, IDC_CROSS);
		RegisterClass(&wc);
		if (w < 0)w = GetSystemMetrics(SM_CXBORDER);
		if (h < 0)h = GetSystemMetrics(SM_CYBORDER);
		
		m_hwnd = CreateWindowEx(
			dwExStyle, ClassName(), lpWindowName, dwStyle, x, y,w ,h, parentWindow, hMenu, GetModuleHandle(NULL), this
		);
		if (!m_hwnd)return false;
		if (dwExStyle&WS_EX_LAYERED)
		{
			return UpdateLayeredWindow(m_hwnd, 0, 0, 0, 0, 0, RGB(0, 0, 0), 0, ULW_COLORKEY);
		}
		return true;
	}
	HWND Window()const { return m_hwnd; }
protected:
	virtual LPCWSTR ClassName()const { return L"Class"; };
	virtual LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) = 0;
};

