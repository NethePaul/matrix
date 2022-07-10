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
	explicit operator bool()const override;
};

