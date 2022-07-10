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
	void be_exploded(Object&by)override;
	bool load(std::string filename);
	bool save(std::string filename)const;
	void attach(std::shared_ptr<Object>o);

	bool add_target(std::shared_ptr<Object>t)override;
	bool rm_target(std::shared_ptr<Object>t)override;
	bool add_rm_target(std::shared_ptr<Object>t)override;
	double get_outer_radius()const override;
	std::shared_ptr<Object> highlight(bool set, Comp<double>cpos);

	const std::vector<std::shared_ptr<Object>>&get_sub_objects()const override;
	void shoot()override;

	Ship(Game*context);
	Ship(const Ship&)=default;
};

