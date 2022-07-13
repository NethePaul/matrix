#include "matrix.h"
#include"Turret.h"
#include"Ship.h"
#include"jet.h"
#include"Message.h"
#include"Button.h"
#include<limits>
using namespace D2D1;
Comp<double> getCP(D2D1::Matrix3x2F&m, POINT&cp) {
	m.Invert();
	auto pos = m.TransformPoint(Point2F(cp.x, cp.y));
	m.Invert();
	return pos;
}
Game::Game() : stars(200),factory(0)
{
	auto s=std::make_shared<Ship>(this);
	s->pos = Comp<double>(1000, 100);
	s->attach(std::make_shared<Turret>(1000-10, 100+30, this));
	s->attach(std::make_shared<Turret>(1000-10, 100-30, this));
	s->attach(std::make_shared<Jet>(1000+50, 100, this));
	Ship::connector buffer;
	buffer.between[0] = s->parts[0];
	buffer.between[1] = s->parts[1];
	s->connections.push_back(buffer);

	buffer.between[0] = s->parts[2];
	buffer.between[1] = s->parts[1];
	s->connections.push_back(buffer);

	buffer.between[0] = s->parts[0];
	buffer.between[1] = s->parts[2];
	s->connections.push_back(buffer);

	buffer.between[0] = s->parts[0];
	buffer.between[1] = s;
	s->connections.push_back(buffer);
	buffer.between[0] = s->parts[1];
	buffer.between[1] = s;
	s->connections.push_back(buffer);
	buffer.between[0] = s->parts[2];
	buffer.between[1] = s;
	s->connections.push_back(buffer);

	objects.emplace_back(s);
	objects.back()->show_stats = true;
	objects.emplace_back(std::make_shared<Jet>(200, 100, this));
	objects.back()->show_stats = true;
	objects.emplace_back(std::make_shared<Jet>(300, 100, this));
	objects.back()->show_stats = true;
	objects.emplace_back(std::make_shared<Turret>(this));
	objects.back()->add_target(objects[0]);
	objects.back()->show_stats = true;
	controlled = focus_point = objects.front();
	objects.front()->controlled = true;
}
void Game::update_closest() {
	auto p = getCP(last, cp);
	double d = std::numeric_limits<double>::infinity();
	std::shared_ptr<Object>o;
	for (auto&obj : objects) {
		auto d2 = (obj->pos - p).getD();
		if (d2 < d) {
			d = d2;
			o = obj;
		}
	}
	auto o_old = highlighted.lock();
	if (o_old)o_old->highlight(false, p);
	closest = o;

	auto n = o ? o->highlight(true, p) : 0;
	if (n)highlighted = n; else highlighted = o;
}

void Game::set_pause(bool p)
{
	if (p)pause++;
	else if(pause)pause--;
}

void Game::unpause() {
	pause = 0;
}

void Game::set_no_render_obj(bool p)
{
	if (p)no_render_objects++;
	else if (no_render_objects)no_render_objects--;
}
void Game::enable_render_obj() {
	no_render_objects = 0;
}
void Game::set_no_render(bool p)
{
	if (p)no_render++;
	else if (no_render)no_render--;
}
void Game::enable_render() {
	no_render = 0;
}
void Game::update()
{
	if (pause)return;
	stars.update();
	
	for (auto obj : objects)obj->update();
	if (notified) {
		remove_obj();
		notified = false;
	}for (auto obj : objects)obj->AI();

	update_closest();
}
void Game::init(ID2D1RenderTarget *pRT, ID2D1Factory* f,HWND hwnd)
{
	main = hwnd;
	factory = f;
	globalImageSystem.init(pRT,f);
}
 
void Game::draw(ID2D1RenderTarget *pRT, D2D1_SIZE_F size)
{
	
	D2D1_MATRIX_3X2_F&m=zoom;
	pRT->GetTransform(&m);
	pRT->FillRectangle(D2D1::RectF(0, 0, WIDTH, HEIGHT), gis.clear_color);
	
	auto fp = focus_point.lock();
	if (constructor)fp = controlled.lock();
	if(fp)
		pRT->SetTransform(D2D1::Matrix3x2F::Translation(-fp->pos.x + (WIDTH-(constructor?600:0)) / 2, -fp->pos.y + HEIGHT / 2)*m);
	pRT->GetTransform(&last);
	if (constructor)constructor->draw(pRT);
	else if (!no_render) {
		stars.draw(pRT);
		if (!no_render_objects)
			for (const auto&obj : objects)
				obj->draw_full(pRT, gis.Yellow);
	}
	pRT->DrawEllipse(D2D1::Ellipse(D2D1::Point2F(), 100, 100), gis.Red);
	pRT->SetTransform(&m);

	//if (msg)msg->draw();
}

void Game::input(UINT umsg, POINT cp, WPARAM wParam, LPARAM lParam, int scroll)
{
	if (constructor) { constructor->input(umsg, cp, wParam, lParam, scroll,main); return; }
	this->cp = cp;
	auto player = controlled.lock();
	double pm = 1;
	switch (umsg)
	{
	case WM_KEYDOWN:
	case WM_KEYUP:
#define k(vk)GetKeyState(vk)& 0x8000
		if (k(VK_SHIFT))
			pm = 0.5;
		if (player) {
			if (k(VK_KEY_A))player->set_rotation_acc(-100 * pm);
			else if (k(VK_KEY_D))player->set_rotation_acc(100 * pm);
			else player->set_rotation_acc(0);
			if (k(VK_KEY_W))player->set_acc(100 * pm);
			else if (k(VK_KEY_S))player->set_acc(-100 * pm);
			else player->set_acc(0);
			if (k(VK_SPACE))player->shoot();
		}

		if (k(VK_KEY_P))pause = !pause;

		if (k(VK_KEY_E)) {
			if (player)
				player->controlled = false;
			auto o = highlighted.lock();
			if (o) {
				o->controlled = true;
				controlled = o;
				focus_point = o;
			}
		}
		if (k(VK_KEY_F)) {
			auto c= highlighted.lock();
			if (player && c&&player!=c) {
				player->add_rm_target(c);
			}
		}
		if (k(VK_KEY_C)) {
			player->target.clear();
		}
		if (k(VK_ESCAPE)) {
			auto a = std::bind([]() {});
			if (!menu||!menu->operator bool())
				menu = std::make_shared<Menu>(main, factory, zoom);
			menu->close_cb = []() {PostQuitMessage(0); };
			menu->close_menu_cb = [this]() {set_pause(false); menu = 0; };
			menu->edit_ship_cb = [this]() {
				constructor = std::make_shared<Constructor>(Constructor(main,factory,last,zoom,this));
				if (!*constructor) {
					auto m = msg(WIDTH / 2 - 200, HEIGHT*.8, 400, HEIGHT*.1, 2, L"Error: cannot edit this\n press to continue");
					set_pause(true);
					m->set_color(D2D1::ColorF::Red, D2D1::ColorF(.1, .1, .1));
					m->on_destruction = [this]() {set_pause(false); };
					constructor = 0;
				}
				else {
					set_pause(true);
					set_no_render_obj(true);
					constructor->on_finish = [this]() {set_pause(false); set_no_render_obj(false); constructor = 0; };
				}
			};
			set_pause(true);
		}
		break;
	case WM_MOUSEMOVE:
		if(!constructor&&!menu)update_closest();
		break;
	case WM_LBUTTONDOWN:
		static_camera = std::make_shared<Object>();
		static_camera->pos = getCP(last, cp);
		focus_point = static_camera;
		break;
	case WM_RBUTTONDOWN:
		if(player)
		focus_point = player;
		break;
	}
}

void Game::notifiy_rm_obj()
{
	notified = true;
}

std::weak_ptr<Object> Game::get_controlled()
{
	return controlled;
}

Message*Game::msg(int x, int y, int w, int h,int r, std::wstring text)
{
	if (!factory)return 0;
	auto p1 = zoom.TransformPoint({ (float)x,(float)y });
	auto p2= zoom.TransformPoint({ (float)x+w,(float)y+h });
	auto m=new Message(main, factory, p1.x, p1.y, p2.x - p1.x, p2.y - p1.y, r);
	m->set_text(text);
	return m;
}

Button * Game::button(int x, int y, int w, int h, int r, std::wstring text)
{
	if (!factory)return 0;
	auto p1 = zoom.TransformPoint({ (float)x,(float)y });
	auto p2 = zoom.TransformPoint({ (float)x + w,(float)y + h });
	auto m = new Button(main, factory, p1.x, p1.y, p2.x - p1.x, p2.y - p1.y, r);
	m->set_text(text);
	return m;
}

void Game::remove_obj()
{
	for (int i = 0; i < objects.size();i++) {
		if (*objects[i])objects.erase(objects.begin() + i--);
	}
}

Game::~Game()
{
	
}
