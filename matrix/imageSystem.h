#pragma once
#include"pch.h"
class ImageSystem
{
public:
	ID2D1PathGeometry*pJet;
	ID2D1PathGeometry*pStar;
	ID2D1SolidColorBrush*Red,*Green,*Blue,*Yellow,*Orange,*Gold;
	ID2D1SolidColorBrush*highlighting;
	ID2D1SolidColorBrush*stars[100];
	ID2D1SolidColorBrush*clear_color;
	void init(ID2D1RenderTarget *pRT, ID2D1Factory* f);
	void create_jet(ID2D1RenderTarget *pRT, ID2D1Factory* f);
	void create_star(ID2D1RenderTarget *pRT, ID2D1Factory* f);
	void create_colors(ID2D1RenderTarget *pRT);
	~ImageSystem();
};

extern ImageSystem globalImageSystem;
#define gis globalImageSystem

