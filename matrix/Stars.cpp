#include "Stars.h"

Stars::Stars(int n):i(0)
{
	for (int i = n; i--;)
		stars.emplace_back(Star{
		double(rand() % WIDTH), double(rand() % HEIGHT),//x,y
		int(rand() % 100),//a
		rand() / double(RAND_MAX),//b
		rand() / double(RAND_MAX) * 360,//rot
		rand() / double(RAND_MAX)*1.4, //b
		rand() / double(RAND_MAX)/5 - .1 ,rand() / double(RAND_MAX)/5 - .1//mx,my
			});
}

void Stars::draw(ID2D1RenderTarget * pRT)const
{
	D2D1::Matrix3x2F m;
	pRT->GetTransform(&m);
	auto size = pRT->GetSize();
	m.Invert();
	auto min=m.TransformPoint(D2D1::Point2F());
	auto max=m.TransformPoint(D2D1::Point2F(size.width, size.height));
	m.Invert();
	for (const auto&star : stars) {
		double x = star.x + std::floor(min.x / WIDTH) * WIDTH; if (x < min.x)x += WIDTH; else if (x > max.x)x -= WIDTH;
		double y = star.y + std::floor(min.y / HEIGHT) * HEIGHT; if (y < min.y)y += HEIGHT; else if (y > max.y)y -= HEIGHT;
		pRT->SetTransform(D2D1::Matrix3x2F::Scale(star.r,star.r)*D2D1::Matrix3x2F::Rotation(star.rot)*D2D1::Matrix3x2F::Translation(x,y )*m);
		pRT->FillGeometry(gis.pStar, gis.stars[int(i*star.b + star.a) % 100]);
	}
	pRT->SetTransform(&m);
}

void Stars::update()
{
	i++;
	for (auto&star : stars)
	{
		star.x += star.mx;
		if (star.x < 0)star.x += WIDTH; if (star.x > WIDTH)star.x -= WIDTH;
		star.y += star.my;
		if (star.y < 0)star.y += HEIGHT; if (star.y > HEIGHT)star.y -= HEIGHT;
		star.rot += rand()/RAND_MAX/2-.25;

	}

}
