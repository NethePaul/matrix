#include"pch.h"
#include"MainWindow.h"



int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE, PWSTR, int nCmdShow) {
	srand((unsigned)time(NULL));

	MainWindow win;
	if (!win.Create(L"Matrix", WS_VISIBLE |WS_POPUP | WS_CLIPCHILDREN, 0)) {
		return 0;
	}
	ShowWindow(win.Window(), SW_MAXIMIZE);
	UpdateWindow(win.Window());
	SetTimer(win.Window(), 1, 20, 0);
	MSG msg = {};


	while (GetMessage(&msg, NULL, 0, 0)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
		if (msg.message == WM_QUIT) {
			return 0;
		}
	}return 0;
}