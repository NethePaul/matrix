#include "Ship.h"
#include"matrix.h"
#include<fstream>

void Ship::draw(ID2D1RenderTarget *r, ID2D1Brush *b) const
{
	if (controlled)b = gis.Green;
	Object::draw(r, b);
	for (int i = 0; i < connections.size(); i++)
		if (!connections[i].draw(r, b))
			const_cast<std::vector<connector>&>(connections).erase(connections.begin() + i--);
	for (const auto&part : parts)part->draw_full(r, b);

	
}

void Ship::draw_explosion(ID2D1RenderTarget *r, ID2D1Brush *b) const
{
	Object::draw_explosion(r, b);
	for (const auto&part : parts)part->draw_explosion(r, b);
}

void Ship::update()
{
	bool need_to_delete_one = false;
	for (auto&part : parts) {
		part->mov = mov;
		part->acc = acc;
		part->friction = friction;
		part->update();
		if (!(part->health>0))
			need_to_delete_one = true;
	}
	if (need_to_delete_one)
		for (int i = 0; i < parts.size(); i++)
			if (!(parts[i]->health>0))
				parts.erase(parts.begin() + i--);
	auto pb = pos;
	auto rb = rotation;
	Object::update();
	for (auto&part : parts) {
		part->pos = D2D1::Matrix3x2F::Rotation((rotation - rb) * 180 / PI, D2D1::Point2F(pos.x, pos.y)).TransformPoint(D2D1::Point2F(part->pos.x, part->pos.y));
		part->rotation += rotation - rb;
	}
}

void Ship::update_outer_radius()
{
	double&r = outer_radius;r = radius;
	for (auto&part : parts) {
		part->update_outer_radius();
		double r2 = (part->pos - pos).getD() + part->get_outer_radius();
		if (r2 > r)r = r2;
	}
}

void Ship::be_exploded(Object & by)
{
	Object::be_exploded(by);
	for (auto&part : parts)part->be_exploded(by);
}

bool Ship::load(std::string filename)
{
	std::ifstream in(filename,std::ios::binary);
	char*buffer = new char[1024]{};
	if (in.is_open()) {
		while (!in.eof()) {
			in.getline(buffer, 1023);
			
		}
		
	}
	else goto failure;
	in.close();
	delete[]buffer;
	return true;
failure:
	in.close();
	delete[]buffer;
	return false;
}

bool Ship::save(std::string filename) const
{
	return false;
}

void Ship::attach(std::shared_ptr<Object> o)
{
	if (!o)return;
	parts.push_back(o);
	Comp<double>d = o->pos-pos;
	double nr = d.getD()+o->radius;
	if (nr > outer_radius)outer_radius = nr;
}

void Ship::detach(std::shared_ptr<Object> o)
{
	for (int i = 0; i < parts.size(); i++) {
		if (parts[i] == o)
		{
			auto a = dynamic_cast<Ship*>(&*parts[i]);
			if (a)a->detach(o);
			parts.erase(parts.begin() + i--);
		}
	}
}

void Ship::add_connector(std::shared_ptr<Object> obj1, std::shared_ptr<Object> obj2)
{
	connector c;
	c.between[0] = obj1;
	c.between[1] = obj2;
	connections.push_back(c);
}

bool Ship::add_target(std::shared_ptr<Object> t)
{
	bool a=Object::add_target(t);
	for (auto&part : parts)part->add_target(t);
	return a;
}

bool Ship::rm_target(std::shared_ptr<Object> t)
{
	bool a = Object::rm_target(t);
	for (auto&part : parts)part->rm_target(t);
	return a;
}

bool Ship::add_rm_target(std::shared_ptr<Object> t)
{
	bool a = Object::add_rm_target(t);
	if(a)for (auto&part : parts)part->add_target(t);
	else for(auto&part : parts)part->rm_target(t);
	return a;
}

double Ship::get_outer_radius() const
{
	return outer_radius;
}

void Ship::set_radius(double r)
{
	radius = r;
	if (radius > outer_radius)outer_radius = radius;
}

void Ship::set_pos(Comp<double> pos)
{
	auto p = pos-this->pos;
	this->pos = pos;
	for (auto&part : parts) {
		part->set_pos(part->pos+p);
	}
}

void Ship::rotate(double x)
{
	Object::rotate(x);
	for (auto&part : parts)
		part->rotate(x);
}

std::shared_ptr<Object> Ship::highlight(bool set, Comp<double> cpos)
{
	if (!set) {
		Object::highlight(0, 0);
		for (auto&obj : parts)
			obj->highlight(0, 0);
		return 0;
	}
	bool a = false;
	if (controlled)a = true;
	if (context) {//if this is being targeted by the player or if this is the player -> a=true
		auto c = context->get_controlled();
		auto c2 = c.lock();
		if (c2) {
			auto targets = c2->target;
			for (auto&t : targets) {
				if (&*t.lock() == this)
					a = true;
			}
		}
	}
	if (!a) Object::highlight(true, cpos);
	double d = min((pos-cpos).getD(),50);
	std::shared_ptr<Object>o,n;
	if (a) {
		for (auto&obj : parts) {
			obj->highlight(0, 0);
			auto d2 = (obj->pos - cpos).getD();
			if (d2 < d) {
				d = d2;
				o = obj;
			}
		}
	}
	if (o) {
		n = o->highlight(set, cpos);
		Object::highlight(0, 0);
	}
	if(n)
		return n;
	return o;
}

const std::vector<std::shared_ptr<Object>>& Ship::get_sub_objects() const
{
	return parts;
}

void Ship::shoot()
{
	for (auto&part : parts)part->shoot();
}

Ship::Ship(Game * context):Object(context),outer_radius(radius)
{
}

bool Ship::connector::draw(ID2D1RenderTarget *r, ID2D1Brush *b)const
{
	auto p1 = between[0].lock();
	auto p2 = between[1].lock();
	if (!p1 || !p2)return false;
	float x1=p1->pos.x, y1 = p1->pos.y, x2= p2->pos.x, y2= p2->pos.y;
	r->DrawLine(D2D1::Point2F(x1, y1), D2D1::Point2F(x2, y2), b, min(p1->radius, p2->radius));
	return true;
}
