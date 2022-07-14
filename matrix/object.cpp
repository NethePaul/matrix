#include "object.h"
#include"matrix.h"
#include"Scrollable.h"
#include"Text_box.h"
#include"slider.h"
#include<sstream>

double get_max_speed(double acc, double fric) {
	return acc * fric / (fric - 1);
}

Object::Object(Game*context)
:pos(0,0)
,mov(0,0)
,friction(1.1)
,acc(0,0)
,max_acc(1)
,min_acc(-40)
,rotation(0)
,rotation_speed(0)
,rotation_acc(0)
,rotation_friction(1.3)
,max_rotation_acc(0.03)
,acc_percent(0)
,radius(10)
,context(context)
{
}

void Object::draw(ID2D1RenderTarget *pRT, ID2D1Brush* brush)const
{
	pRT->FillEllipse(D2D1::Ellipse(D2D1::Point2F(pos.x,pos.y),radius,radius),brush);
}

void Object::draw_target_highlighting(ID2D1RenderTarget *r)
{
	ID2D1Brush*b = gis.Red; bool d = false;
	for (auto&wt : target) {
		auto t = wt.lock();
		if (t) {
			r->DrawRectangle(D2D1::RectF(t->pos.x - t->get_outer_radius() -3, t->pos.y - t->get_outer_radius() -3, t->pos.x + t->get_outer_radius() +3, t->pos.y + t->get_outer_radius() +3), b, 3);
			if(!t->show_stats)
				t->draw_stats(r);
			b = gis.Orange;
		}
		else d = true;
	}
	if (d)for (int i = 0; i < target.size(); i++)
		if (target[i].expired())
			target.erase(target.begin()+i--);
}

void Object::draw_full(ID2D1RenderTarget *r, ID2D1Brush *b) const
{
	if (highlighting)r->DrawRectangle(D2D1::RectF(pos.x - get_outer_radius(), pos.y - get_outer_radius(), pos.x + get_outer_radius(), pos.y + get_outer_radius()), highlighting, 3);
	if (health!=health)draw_explosion(r,gis.Red);
	else { draw(r, b); if(show_stats||highlighting)draw_stats(r); }
	if (controlled)const_cast<Object&>(*this).draw_target_highlighting(r);
}

void Object::update()
{
	update_mov();
	update_rot();
	update_explode();
}

void Object::destroy()
{
	health = 0;
	explode();
}

void Object::update_mov()
{
	pos += mov;
	mov /= friction;
	mov += acc;
}
void Object::update_rot() {
	if (rotation_speed < .02&&rotation_speed>-.02 && rotation_acc < .01&&rotation_acc>-.01)return;
	rotation += rotation_speed;
	rotation_speed /= rotation_friction;
	rotation_speed += rotation_acc;
	acc = CCompbyR(max_acc*acc_percent / 100.0, rotation);
}

void Object::set_acc(double percentage)
{
	if (percentage > 100)percentage = 100;
	else if (percentage < 0)
		if (percentage <= -100)percentage = min_acc;
		else percentage = -min_acc * percentage / 100;
	acc_percent = percentage;
	acc = CCompbyR(max_acc*percentage / 100.0, rotation);
}

void Object::set_mov(double speed)
{
	mov = CCompbyR(speed, rotation);
}

bool Object::add_target(std::shared_ptr<Object> t)
{
	for (int i=0;i<target.size();i++)
		if (target[i].lock() == t) {
			if (i == 0)return false;//already in list
			std::swap(target[i], target[0]);
			return true;//already in list but not as primary target
		}
	target.push_back(t);
	auto&a = t->get_sub_objects();
	for (auto&sub : a) {
		add_target(sub);
	}
	return true;
}

bool Object::rm_target(std::shared_ptr<Object> t)
{
	bool a = false;
	for (int i=0;i<target.size();i++)
		if (target[i].lock() == t) {
			target.erase(target.begin() + i--);
			a = true;
		}
	auto&b = t->get_sub_objects();
	for (auto&sub : b) {
		rm_target(sub);
	}
	return a;
}

bool Object::add_rm_target(std::shared_ptr<Object> t)
{
	if (target.size() && target[0].lock() == t) {//remove target
		target.erase(target.begin()); 
		auto&a = t->get_sub_objects();
		for (auto&sub : a) {
			rm_target(sub);
		}
		return false;
	}
	//if it is a secondary target, set it to main target
	std::shared_ptr<Object>main_target;
	for (int i = 1; i < target.size(); i++) {
		auto t2 = target[i].lock();
		if (t2 == t) {
			main_target = t2;
			target.erase(target.begin()+i);
			break;
		}
	}

	//add target
	if(!main_target)
	target.push_back(t);

	auto&a = t->get_sub_objects();
	for (auto&sub : a) {
		add_target(sub);
	}
	if (main_target)target.insert(target.begin(), main_target);
	return true;
}

std::shared_ptr<Object> Object::highlight(bool set, Comp<double>cpos)
{
	if (set)highlighting = gis.highlighting;
	else highlighting = 0;
	return 0;
}

const std::vector<std::shared_ptr<Object>>& Object::get_sub_objects()const
{
	std::vector<std::shared_ptr<Object>>a;
	return a;
}

Object::operator bool() const
{
	return(health != health) || explosion_animation;
}

double Object::get_outer_radius() const
{
	return radius;
}

void Object::set_radius(double r)
{
	radius = r;
}

void Object::set_pos(Comp<double> pos)
{
	this->pos = pos;
}

void Object::rotate(double x)
{
	rotation += x;
}

Scrollable * Object::details(HWND parent, ID2D1Factory * f, D2D1::Matrix3x2F zoom,double x,double y,double w,double h, Comp<double>relative_pos)
{
	auto p = zoom.TransformPoint({ (FLOAT)x,(FLOAT)y });
	auto p2= zoom.TransformPoint({ (FLOAT)x+ (FLOAT)w,(FLOAT)y+ (FLOAT)h });
	auto d = D2D1::Point2F(p2.x - p.x, p2.y - p.y); 
	double necessary_size=p.x*2;
	auto a = new Scrollable(parent, f, p.x, p.y, d.x, d.y, necessary_size);
	d.x *= .9;
	parent = a->get_hwnd();
	Text_box*b;
	Message*m;
#define setting_pair_tb(y_,a_,v_)\
	m = new Message(parent, f, d.x/2 , y_, d.x / 2, d.y*.3, 2);\
	m->set_auto_destroy(false); m->set_text(L##a_);\
	b = new Text_box(parent, f, 0, y_, d.x / 2, d.y*.3, 2);\
	b->set_text(std::to_wstring(v_));\
	
	setting_pair_tb(0, "position x", pos.x-relative_pos.x);
	b->on_enter = [this,relative_pos](const std::wstring&s) {
		std::wstringstream ss(s);
		double d = 0;
		ss >> d;
		if (d < 0)d = 0;
		if (d > 500)d = 500;
		if (ss.eof() && !ss.fail()) set_pos(relative_pos + d + Comp<double>(.0,pos.y-relative_pos.y));
	};
	setting_pair_tb(d.y*.3, "position y", pos.y - relative_pos.y);
	b->on_enter = [this, relative_pos](const std::wstring&s) {
		std::wstringstream ss(s);
		double d = 0;
		ss >> d;
		if (d < 0)d = 0;
		if (d > 450)d = 450;
		if (ss.eof() && !ss.fail()) set_pos(relative_pos + Comp<double>(.0,d)+pos.x-relative_pos.x);
	}; setting_pair_tb(d.y * .6, "rotation", rotation*180/PI);
	b->on_enter = [this, relative_pos](const std::wstring&s) {
		std::wstringstream ss(s);
		double d = 0;
		ss >> d;
		d /= 180; d *= PI;
		if (ss.eof() && !ss.fail()) rotate(d-rotation);
	}; setting_pair_tb(d.y * .9, "radius", radius);
	b->on_enter = [this, relative_pos](const std::wstring&s) {
		std::wstringstream ss(s);
		double d = 0;
		ss >> d;
		if (ss.eof() && !ss.fail()) set_radius(d);
	};
	setting_pair_tb(d.y * 1.2, "max acceleration", max_acc);
	b->on_enter = [this, relative_pos](const std::wstring&s) {
		std::wstringstream ss(s);
		double d = 0;
		ss >> d;
		if (ss.eof() && !ss.fail()) max_acc=d;
	}; setting_pair_tb(d.y * 1.5, "max rotation acceleration", max_rotation_acc);
	b->on_enter = [this, relative_pos](const std::wstring&s) {
		std::wstringstream ss(s);
		double d = 0;
		ss >> d;
		if (ss.eof() && !ss.fail()) max_rotation_acc=d;
	};


	return a;
}

void Object::AI()
{
	if (controlled)return;
	for (auto&wt : target) {
		auto t = wt.lock(); if (t) {
			auto m = t->pos-pos;
			double angle=m.getA();
			angle -= rotation + rotation_speed;
			angle = std::remainder(angle, PI*2);
			if (max_rotation_acc > abs(angle))
				angle /= max_rotation_acc;
			else if (angle < 0)angle = -1;
			else angle = 1;
			
			set_rotation_acc(angle *100);
			set_acc(100);
			if(!(t->health<=0))shoot();
			return;
		}
	}
	set_mov(0);
	set_acc(0);
}

void Object::set_rotation_acc(double percantage)
{
	if (percantage > 100)percantage = 100;
	else if (percantage < -100)percantage = -100;
	rotation_acc = max_rotation_acc * percantage / 100;
}

void Object::update_explode()
{
	if (health <= 0)explode();
	if (explosion_animation) { explosion_animation--; if (!explosion_animation&&context)context->notifiy_rm_obj(); }
}

void Object::explode()
{
	if (health != health)return;//check if health == NAN
	health = NAN;
	if (context) {
		auto&o = context->objects;
		for (auto&obj : o) {
			if (obj->health < 0)continue;
			obj->be_exploded(*this);
		}
	}
	explosion_animation = explosion_animation_max;
}

void Object::be_exploded(Object & by)
{
	auto d = this->pos - by.pos;
	auto r = d.getD();
	if (r - radius > by.explosion_radius)return;
	if (r == 0)return;
	health -= by.explosion_damage / (1 + exp(+r - radius - by.explosion_radius));
	//mov += d / r * by.explosion_repulsion / (1 + exp(+r - radius - by.explosion_radius));
	if (health < 0)destroy();
}

void Object::draw_explosion(ID2D1RenderTarget *rt, ID2D1Brush *b) const
{
	if (!explosion_animation)return;
	auto buffer = radius;
	const_cast<double&>(radius) = explosion_radius * (explosion_animation_max - explosion_animation) / double(explosion_animation_max);
	Object::draw(rt, b);
	const_cast<double&>(radius) = buffer;
}

void Object::shoot()
{
}

void Object::draw_stats(ID2D1RenderTarget *r)const
{
	double radius = get_outer_radius();
	r->FillRectangle(D2D1::RectF(pos.x - radius - 8, pos.y - radius - 8, pos.x + radius + 8, pos.y - radius - 3), gis.Red);
	r->FillRectangle(D2D1::RectF(pos.x - radius - 8, pos.y - radius - 8, pos.x - radius - 8 + (16+radius*2)*health/max_health, pos.y - radius - 3), gis.Green);
}
