#include "imageSystem.h"
using namespace D2D1;
ImageSystem globalImageSystem;

void ImageSystem::create_jet(ID2D1RenderTarget *pRT, ID2D1Factory* f) {
	f->CreatePathGeometry(&pJet);
	ID2D1GeometrySink*sink;
	pJet->Open(&sink);
	sink->BeginFigure(Point2F(0, 0), D2D1_FIGURE_BEGIN_FILLED);
#define l(x,y)sink->AddLine(Point2F(x, y));
	l(-5, -5); l(5, 0); l(-5, 5);
#undef l
	sink->EndFigure(D2D1_FIGURE_END_CLOSED);
	sink->Close();
	SafeRelease(&sink);
}void ImageSystem::create_star(ID2D1RenderTarget *pRT, ID2D1Factory* f) {
	f->CreatePathGeometry(&pStar);
	ID2D1GeometrySink*sink;
	pStar->Open(&sink);
	sink->BeginFigure(Point2F(-5.1, -5.2), D2D1_FIGURE_BEGIN_FILLED);
#define l(x,y)sink->AddLine(Point2F(x, y));
	l(-1.9, 0.2)l(-5.3, 5.3); l(0.1, 2.2) l(4.8, 4.9); l(2.1, 0.1); l(5.2, -5.2); l(0.1, -2.1);
#undef l
	sink->EndFigure(D2D1_FIGURE_END_CLOSED);
	sink->Close();
	SafeRelease(&sink);
}
void ImageSystem::init(ID2D1RenderTarget *pRT, ID2D1Factory* f)
{
	create_colors(pRT);
	create_jet(pRT, f);
	create_star(pRT, f);
}
//DO NOT TOUCH
//weird intellisense error ahead
/*
void ImageSystem::create_colors(ID2D1RenderTarget * pRT)
{
	
#define c(p)pRT->CreateSolidColorBrush(ColorF(ColorF::##p), &p);
	
	c(Red); c(Blue); c(Green); c(Yellow); c(Orange);
	pRT->CreateSolidColorBrush(ColorF(0.1, 0.1, 0.1, .4),&clear_color);
	for (int i = 0; i < 100; i++)
		pRT->CreateSolidColorBrush(ColorF(1-i/200.0,1-i / 200.0,1-i / 200.0), &stars[i]);
}
*/

ImageSystem::~ImageSystem()
{
	SafeRelease(&Red);
	SafeRelease(&pJet);
}
