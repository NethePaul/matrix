#pragma once
#include"pch.h"
class Game;
class Scrollable;
class Object
{
public:
	Comp<double>pos;
	Comp<double>mov;
	double friction;
	Comp<double>acc;
	double max_acc;
	double min_acc;//negativ percentage of max_acc for backwards movement
	double rotation;//in radians
	double rotation_speed;//in radians
	double rotation_acc;//in radians
	double rotation_friction;
	double max_rotation_acc;
	double health = 100;
	double max_health = 100;

	double radius;
	double acc_percent;

	double explosion_radius = 10;//explosion upon destruction, set to 0 for non-exploding things
	double explosion_damage = 100;//lower the further away from center
	double explosion_repulsion = 10;//adds explosion_propulsion per unit of weight to mov
	Game*context;
	double weight = 1;
	int explosion_animation = 0;
	int explosion_animation_max = 10;
	bool show_stats = 0;
	std::vector<std::weak_ptr<Object>> target;

	bool controlled = 0;
	ID2D1Brush*highlighting=nullptr;
public:
	Object(Game*context=0);
	Object(const Object&) = default;
	Object& operator=(const Object&) = default;

	virtual void draw(ID2D1RenderTarget*,ID2D1Brush*)const;
	void draw_target_highlighting(ID2D1RenderTarget*);
	void draw_full(ID2D1RenderTarget*, ID2D1Brush*)const;
	virtual void update();
	virtual void destroy();
	virtual void draw_explosion(ID2D1RenderTarget*, ID2D1Brush*)const;
	virtual void shoot();
	virtual void draw_stats(ID2D1RenderTarget*)const;//like health or energy
	void explode();
	virtual void be_exploded(Object&by);
	void update_mov();
	void update_rot();
	void update_explode();
	void set_acc(double percentage);
	void set_mov(double speed);
	virtual bool add_target(std::shared_ptr<Object>t);
	virtual bool rm_target(std::shared_ptr<Object>t);
	virtual bool add_rm_target(std::shared_ptr<Object>t);
	virtual std::shared_ptr<Object> highlight(bool set,Comp<double>cpos);
	virtual const std::vector<std::shared_ptr<Object>>&get_sub_objects()const;
	virtual explicit operator bool()const;
	virtual double get_outer_radius()const;
	virtual void set_radius(double r);
	virtual void set_pos(Comp<double>pos);
	virtual void rotate(double x);
	virtual Object*clone()const {return new Object(*this);}
	virtual void update_outer_radius() {};
	virtual Scrollable*details(HWND parent, ID2D1Factory * f, D2D1::Matrix3x2F zoom, double x, double y, double w, double h, Comp<double>relative_pos = 0);
	virtual void AI();
	void set_rotation_acc(double percantage);
};

double get_max_speed(double acc, double fric);