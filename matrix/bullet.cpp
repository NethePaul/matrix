#include "bullet.h"

Bullet::Bullet(const Object & cpy, int lifetime):Object(cpy),lifetime(lifetime)
{
	explosion_damage = 10;
}

Bullet::Bullet(Game * context, int lifetime):Object(context),lifetime(lifetime)
{
}

void Bullet::update()
{
	Object::update();
	for (auto&t : target) {
		auto l = t.lock();
		if (l&&l->health>0) {
			if ((l->pos - pos).getD() < explosion_radius + l->radius)
				explode();
		}
	}
	if(lifetime)
		lifetime--;
	if (!lifetime)
		{explode(); }
}

Bullet::operator bool() const
{
	return (lifetime||explosion_animation);
}

