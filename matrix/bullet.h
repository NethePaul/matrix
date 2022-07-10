#pragma once
#include "object.h"
#include <memory>
class Bullet :
	public Object
{
	int lifetime;
public:
	Bullet(const Object&cpy, int lifetime);
	Bullet(Game*context, int lifetime);
	void update()override;
	Object*clone()const override { return new Bullet(*this); }
	explicit operator bool()const override;
};

