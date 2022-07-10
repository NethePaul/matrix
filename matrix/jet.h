#pragma once
#include "object.h"
#include"bullet.h"
class Jet :
	public Object
{
	std::vector<std::shared_ptr<Bullet>>bullets;
	
	int shooting_cooldown = 0;
public:
	Jet(int x, int y,Game*context);
	void draw(ID2D1RenderTarget*pRT, ID2D1Brush*brush)const override;
	void update()override;
	void shoot()override;
	Object*clone()const override { return new Jet(*this); }
};

