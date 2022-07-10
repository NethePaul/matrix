#pragma once
#include<memory>
#include"pch.h"
#include"Stars.h"
#include"Constructor.h"
#include"Menu.h"
class Message;
class Button;
class Game
{
	HWND main;
	ID2D1Factory*factory;
	std::weak_ptr<Object>controlled;
	std::shared_ptr<Object>static_camera;
	Stars stars;
	D2D1::Matrix3x2F last,zoom;
	std::vector<std::shared_ptr<Object>>objects;//list of all objects
	POINT cp;
	std::weak_ptr<Object>closest,highlighted;
	int pause=0;
	int no_render_objects = 0;
	int no_render = 0;
	
	std::shared_ptr<Menu>menu;
	std::shared_ptr<Constructor>constructor;
	
	friend class Object;
	void remove_obj();
	bool notified=false;
	
public:
	std::weak_ptr<Object>focus_point;
	Game();
	void update();
	void init(ID2D1RenderTarget*,ID2D1Factory*,HWND hwnd);
	void draw(ID2D1RenderTarget*, D2D1_SIZE_F);
	void input(UINT msg, POINT cp, WPARAM wParam, LPARAM lParam, int scroll);
	void notifiy_rm_obj();
	void update_closest();
	void set_pause(bool);
	void unpause();
	void set_no_render(bool);
	void enable_render();
	void set_no_render_obj(bool);
	void enable_render_obj();
	std::weak_ptr<Object> get_controlled();
	Message*msg(int x,int y,int w,int h,int r,std::wstring text);
	Button*button(int x, int y, int w, int h, int r, std::wstring text);
	~Game();
};

Comp<double> getCP(D2D1::Matrix3x2F&m, POINT&cp);