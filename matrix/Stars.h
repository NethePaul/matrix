#pragma once
#include<vector>
#include"pch.h"
class Stars
{
	int i;
	struct Star {
		double x, y;int a;
		double b; double rot; double r;
		double mx, my;
	};
	std::vector<Star>stars;
public:
	Stars(int n);
	void draw(ID2D1RenderTarget*pRT)const;
	void update();
};

