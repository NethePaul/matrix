#pragma once
#include "object.h"
#include "bullet.h"
class Turret :
	public Object
{
	std::vector<std::shared_ptr<Bullet>>bullets;

	int shooting_cooldown = 0;
	int salve_cooldown = 0;
	int max_salve_cooldown=5;//in shots
	int max_shooting_cooldown=20;//in updates //50 Updates per second
public:
	Turret(Game*context);
	Turret(double x,double y,Game*context);
	void draw(ID2D1RenderTarget*pRT, ID2D1Brush*brush)const override;
	void update()override;
	void shoot()override;
	void update_shots();
};

bool calculate_intercept_course(std::shared_ptr<Object>target, std::shared_ptr<Object>interceptor, double speed);