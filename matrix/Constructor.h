#pragma once
#include"Ship.h"
#include"cbasewin.h"
class Game;
class Constructor
{
	std::vector<std::shared_ptr<Object>>components_selectable;
	std::shared_ptr<Object>components_selected;
	std::shared_ptr<Object>constructee_selected;
	std::shared_ptr<Object>working_on_;//ensures working_on will not be deallocated
	Ship*working_on;
	Game*context;
public:
	Constructor(Game*context);
	//will return false if obj is not a Ship
	bool set_working_on(std::shared_ptr<Object>obj);
	void init();
	
	
	void draw(ID2D1RenderTarget*);
	void draw_selection(ID2D1RenderTarget*);
	void click(Comp<double>pos,bool rclick=0);


};

