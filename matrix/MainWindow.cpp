#include"MainWindow.h"


void MainWindow::DiscardGraphicsRecources() {
	SafeRelease(&pRenderTraget);
	SafeRelease(&pBrush);
}
HRESULT MainWindow::CreateGraphicsResources() {

	HRESULT hr = S_OK, hr2 = S_OK;
	if (pRenderTraget == NULL) {
		RECT rc;
		GetClientRect(m_hwnd, &rc);
		D2D1_SIZE_U size = D2D1::SizeU(rc.right, rc.bottom);
		hr = pFactory->CreateHwndRenderTarget(
			D2D1::RenderTargetProperties(),
			D2D1::HwndRenderTargetProperties(m_hwnd, size),
			&pRenderTraget
		);
		if (SUCCEEDED(hr)) {
			const D2D1_COLOR_F color = D2D1::ColorF(D2D1::ColorF::SkyBlue);
			hr = pRenderTraget->CreateSolidColorBrush(color, &pBrush);
			if (SUCCEEDED(hr)) {
				IDW::CreateDeviceIndependentResources(pFactory, &pIDWFactory, &pIDWFormat, 10);
				CalculateLayout();
				matrix.init(pRenderTraget, pFactory, m_hwnd);
			}
		}
	}
	return hr;
}
void MainWindow::CalculateLayout() {
	RedrawWindow(m_hwnd, 0, 0, RDW_INVALIDATE);
}
void MainWindow::OnPaint() {
	if (!m_size.height || !m_size.width) { Resize(); }
	SYSTEMTIME time; GetLocalTime(&time);
	HRESULT hr = CreateGraphicsResources();
	if (SUCCEEDED(hr)) {
		PAINTSTRUCT ps;
		BeginPaint(m_hwnd, &ps);
		pRenderTraget->BeginDraw();
		pRenderTraget->SetTransform(m);
		matrix.draw(pRenderTraget, m_size);
		hr = pRenderTraget->EndDraw();
		if (FAILED(hr) || hr == D2DERR_RECREATE_TARGET) {
			DiscardGraphicsRecources();
			MessageBox(NULL, L"Error", L"Error", MB_YESNO);
		}
		EndPaint(m_hwnd, &ps);
	}
}
void MainWindow::Resize() {
	if (pRenderTraget != 0) {
		RECT rc;
		GetClientRect(m_hwnd, &rc);

		m_size = D2D1::SizeF(rc.right, rc.bottom);
		D2D1_SIZE_U sizeu = D2D1::SizeU(rc.right, rc.bottom);
		pRenderTraget->Resize(sizeu);
		if (sizeu.width&&sizeu.height)
			pRenderTraget->SetTransform(D2D1::Matrix3x2F::Scale(D2D1::SizeF(
				double(sizeu.width) / double(WIDTH), double(sizeu.height) / double(HEIGHT))));
		pRenderTraget->GetTransform(&m);
		CalculateLayout();
	}
}
MainWindow::MainWindow() :pFactory(NULL), pRenderTraget(NULL), pBrush(NULL)
{

}
LRESULT MainWindow::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) {
	switch (uMsg)
	{
	case WM_TIMER:
		matrix.update();
		//OnPaint();
		RedrawWindow(m_hwnd, 0, 0, RDW_INVALIDATE);
		return 0;
	case WM_CREATE:
		HRESULT hr;
		hr = D2D1CreateFactory(
			D2D1_FACTORY_TYPE_SINGLE_THREADED, &pFactory
		);
		if (FAILED(hr))
		{
			return -1;
		}
		OnPaint();
		return 0;
	case WM_MOUSEWHEEL:
		delta_buffer = GET_WHEEL_DELTA_WPARAM(wParam); goto input;
	case WM_KEYDOWN:
		if (GetAsyncKeyState(VK_F4) && GetAsyncKeyState(VK_RMENU)) {
			PostQuitMessage(0);
		}
		//if (GetAsyncKeyState(VK_ESCAPE)) { if (MessageBox(0, L"Soll das Programm beendet werden ?", L"Beenden?", MB_ICONQUESTION | MB_YESNO) == IDYES)PostQuitMessage(0); }
	case WM_KEYUP:
	case WM_LBUTTONDOWN:
	case WM_LBUTTONUP:
	case WM_RBUTTONDOWN:
	case WM_RBUTTONUP:
	case WM_MOUSEMOVE:
		delta_buffer = 0;
	input:
		{
			POINT cp;
			GetCursorPos(&cp);
			ScreenToClient(m_hwnd, &cp);
			matrix.input(uMsg, cp, wParam, lParam, delta_buffer);
		}
		return 0;
	case WM_DESTROY:
		KillTimer(m_hwnd, 1);
		DiscardGraphicsRecources();
		SafeRelease(&pFactory);
		PostQuitMessage(0);
		return 0;
	case WM_PAINT:

		OnPaint();
		return 0;
	case WM_SIZING:
	case WM_SIZE:
		Resize();
		return 0;
	}
	return DefWindowProc(m_hwnd, uMsg, wParam, lParam);
}