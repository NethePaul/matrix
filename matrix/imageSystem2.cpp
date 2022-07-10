#include"imageSystem.h"
//I don't know why, but intellisense really does not like this. It compiles fine though.
void ImageSystem::create_colors(ID2D1RenderTarget *pRT) {
	ID2D1RenderTarget*r;
#define color(c)\
	if(FAILED(pRT->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::##c),&c))){\
		MessageBoxA(NULL, "Color could not be loaded.", "Error", MB_ICONERROR | MB_OK);\
		abort();\
	}
	color(Green);
	color(Red);
	color(Blue);
	color(Yellow);
	color(Orange);
	color(Gold);
	pRT->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Aqua), &highlighting);
	pRT->CreateSolidColorBrush(D2D1::ColorF(0.1, 0.1, 0.1, .4), &clear_color);
	for (int i = 0; i < 100; i++)
		pRT->CreateSolidColorBrush(D2D1::ColorF(1 - i / 200.0, 1 - i / 200.0, 1 - i / 200.0), &stars[i]);
}