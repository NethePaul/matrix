#pragma once

#include"cbasewin.h"
#include"pch.h"
#include"matrix.h"


class MainWindow :public CBaseWin<MainWindow>
{
	
	ID2D1HwndRenderTarget	*pRenderTraget;
	ID2D1SolidColorBrush	*pBrush;
	D2D1_SIZE_F				m_size;
	ID2D1Factory			*pFactory;
	IDWriteFactory			*pIDWFactory;
	IDWriteTextFormat		*pIDWFormat;
	D2D1::Matrix3x2F		m;
	int delta_buffer = 0;
	
	Game matrix;

	HRESULT CreateGraphicsResources();
	void    CalculateLayout();
	void    DiscardGraphicsRecources();
	void    OnPaint();
	void    Resize();
	bool a = 0;
	int loadded = -1;
	int moves_needed = -1;
	
public:
	MainWindow();
	

	LPCWSTR ClassName()const { LPCWSTR a = L"RenderingEngineD2D1"; return a; };
	LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
};