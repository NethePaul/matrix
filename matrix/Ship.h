#pragma once
#include "object.h"
class Ship :
	public Object
{
public:
	struct connector {
		std::weak_ptr<Object>between[2];
		bool draw(ID2D1RenderTarget*, ID2D1Brush*)const;
	};
	std::vector<std::shared_ptr<Object>>parts;
	std::vector<connector>connections;
	double outer_radius;
public:
	void draw(ID2D1RenderTarget*, ID2D1Brush*)const override;
	void draw_explosion(ID2D1RenderTarget*, ID2D1Brush*)const override;
	void update()override;
	void update_outer_radius()override;
	void be_exploded(Object&by)override;
	bool load(std::string filename);
	bool save(std::string filename)const;
	void attach(std::shared_ptr<Object>o);
	void detach(std::shared_ptr<Object>o);
	Object*clone()const override{ return new Ship(*this); }
	void add_connector(std::shared_ptr<Object>obj1, std::shared_ptr<Object>obj2);

	bool add_target(std::shared_ptr<Object>t)override;
	bool rm_target(std::shared_ptr<Object>t)override;
	bool add_rm_target(std::shared_ptr<Object>t)override;
	double get_outer_radius()const override;
	void set_radius(double r)override;
	void set_pos(Comp<double>pos)override;
	std::shared_ptr<Object> highlight(bool set, Comp<double>cpos);

	const std::vector<std::shared_ptr<Object>>&get_sub_objects()const override;
	void shoot()override;

	Ship(Game*context);
	Ship(const Ship&)=default;
};

