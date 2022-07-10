#include "Turret.h"

Turret::Turret(Game * context):Object(context)
{
}

Turret::Turret(double x, double y, Game * context):Object(context)
{
	pos = Comp<double>(x, y);
}

void Turret::draw(ID2D1RenderTarget * pRT, ID2D1Brush * brush) const
{
	for (auto&bullet : bullets)bullet->draw_full(pRT, gis.Blue);
	if (controlled)
		Object::draw(pRT, gis.Green);
	else
		Object::draw(pRT, brush);
}

void Turret::update()
{
	shoot();
	Object::update();
	for (auto&bullet : bullets)bullet->update();	
	update_shots();
}
void Turret::update_shots() {
	if (bullets.size() && !*bullets.front())
		bullets.erase(bullets.begin());
	if (shooting_cooldown)shooting_cooldown--;
}
void Turret::shoot()
{
	if (shooting_cooldown)return;
	if (salve_cooldown >= 0) {
		shooting_cooldown = max_shooting_cooldown;
		salve_cooldown--; return;
	}
	std::shared_ptr<Object>t;
	while (true) {
		if (target.size() < 1)return;
		t = target.front().lock();
		if (t)break;
		target.erase(target.begin());
	}
	auto buffer = std::make_shared<Bullet>(context, 100);
	auto&b = *buffer;
	b.pos = pos;
	b.rotation = rotation;
	b.friction = 1;
	b.acc = 0;
	b.rotation_acc = 0;
	b.radius = 4;
	b.target = target;
	b.explosion_damage = 10;

	if(calculate_intercept_course(t,buffer , 20));
		bullets.emplace_back(buffer);
	

	shooting_cooldown = max_shooting_cooldown;
	salve_cooldown--;
	if (salve_cooldown < -max_salve_cooldown)
		salve_cooldown = max_salve_cooldown;
}

bool calculate_intercept_course(std::shared_ptr<Object>target, std::shared_ptr<Object>interceptor, double speed) {
	auto d = (target->pos - interceptor->pos);
	double c = d.x*d.x + d.y*d.y;
	double b = (d.x*target->mov.x + d.y*target->mov.y) * 2;
	double a = (target->mov.y*target->mov.y + target->mov.x*target->mov.x) - speed * speed;

	double dis = b * b - 4 * a*c;
	if (dis < 0) {
		return false;
	}
	if (b == 0) {
		interceptor->rotation = (d).getA();
		interceptor->set_mov(20);
		return true;
	}
	double t = (-b + sqrt(dis)) / (2 * a);
	if (t < 0)t = (-b - sqrt(dis)) / (2 * a);
	if (t <= 0) {
		return false;
	};
	interceptor->mov = CCompbyR<double>(20, (target->pos + target->mov*t - interceptor->pos).getA());
	return true;
}