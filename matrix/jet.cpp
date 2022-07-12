#include "jet.h"
using namespace D2D1;
Jet::Jet(int x, int y, Game*context):Object(context) {
	pos(x, y);
	radius = 20;
}
void Jet::draw(ID2D1RenderTarget * pRT, ID2D1Brush * brush)const
{
	for (auto&bullet : bullets)bullet->draw_full(pRT, gis.Blue);
	D2D1_MATRIX_3X2_F m;
	pRT->GetTransform(&m);
	pRT->SetTransform(Matrix3x2F::Scale(SizeF(radius / 10, radius / 10))*Matrix3x2F::Rotation(rotation * 180 / PI)*Matrix3x2F::Translation(pos.x, pos.y)*m);
	pRT->FillGeometry(gis.pJet, controlled ? gis.Green : brush);
	pRT->SetTransform(&m);
}

void Jet::update()
{
	Object::update();
	for (auto&bullet : bullets)bullet->update();
	if (bullets.size()&&!*bullets.front())
		bullets.erase(bullets.begin());
	if (shooting_cooldown)shooting_cooldown--;
}

void Jet::shoot()
{
	if (shooting_cooldown)return;
	bullets.emplace_back(std::make_shared<Bullet>(Bullet(context,100)));//2s
	auto&b = *bullets.back();
	b.pos = pos;
	b.rotation = rotation;
	b.friction = 1.003;
	b.acc = 0;
	b.set_mov(20);
	b.mov += mov;
	b.radius = 4;
	b.target = target;
	b.explosion_damage = 10;
	shooting_cooldown = 10;
}
