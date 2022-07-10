#include "Constructor.h"
#include"Turret.h"
#include"object.h"
#include"jet.h"


Constructor::Constructor(Game * context):context(context),working_on_(0)
{
	components_selectable.emplace_back(std::make_shared<Turret>(Turret(context)));
	components_selectable.emplace_back(std::make_shared<Jet>(Jet(0,0,context)));
	components_selectable.emplace_back(std::make_shared<Object>(Object(context)));
	components_selectable.emplace_back(std::make_shared<Ship>(Ship(context)));
	for (int i = 0; i < components_selectable.size(); i++) {
		auto&c = components_selectable[i];
		c->pos = Comp<double>(1000, 100 + 50 * i);
		c->radius = 20;
	}
}

bool Constructor::set_working_on(std::shared_ptr<Object> obj)
{
	working_on = dynamic_cast<Ship*>(&*obj);
	if (working_on)working_on_ = obj;
	return working_on;
}

void Constructor::init()
{
}

void Constructor::draw(ID2D1RenderTarget *r)
{
	if (!working_on)return;
	working_on->draw(r, gis.Green);
	if (constructee_selected)constructee_selected->draw(r, gis.Gold);
	draw_selection(r);
}

void Constructor::draw_selection(ID2D1RenderTarget *r)
{
	for (auto&p : components_selectable) {
		p->draw_full(r, gis.Yellow);
	}if (components_selected)components_selected->draw(r, gis.highlighting);
}
