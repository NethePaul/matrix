#include "Constructor.h"
#include"Turret.h"
#include"object.h"
#include"jet.h"
#include"matrix.h"
#include"Scrollable.h"
#include"Scrollbar.h"

Comp<double> get_cp2(D2D1::Matrix3x2F&m, POINT&cp) {
	auto pos = m.TransformPoint(D2D1::Point2F(cp.x, cp.y));
	return pos;
}
void Constructor::set_selected_last()
{
	selected_last = selected_l;
	if (menu)
		DestroyWindow(menu->get_hwnd());
	auto sl = selected_last.lock();
	if (sl)
		menu=sl->details(main, f, zoom, 1000, HEIGHT / 2.0, 600, HEIGHT / 3.0, working_on->pos);
	else menu = 0;
}
Constructor::Constructor(HWND main, ID2D1Factory*f,D2D1::Matrix3x2F&last, D2D1::Matrix3x2F&zoom, Game * context):working_on(0),context(context),working_on_(0),last(last),zoom(zoom),main(main),f(f)
{
	//last.Invert();
	//auto min = last.TransformPoint(D2D1::Point2F(0, 0));
	//auto max = last.TransformPoint(D2D1::Point2F(WIDTH, HEIGHT));
	//last.Invert();
	components_selectable.emplace_back(std::make_shared<Turret>(Turret(context)));
	components_selectable.emplace_back(std::make_shared<Jet>(Jet(0,0,context)));
	components_selectable.emplace_back(std::make_shared<Object>(Object(context)));
	components_selectable.emplace_back(std::make_shared<Ship>(Ship(context)));
	for (int i = 0; i < components_selectable.size(); i++) {
		auto&c = components_selectable[i];
		c->pos = Comp<double>(1300, 100 + 50 * i);
		c->set_radius(20);
	}
	if (context)valid = set_working_on(context->get_controlled().lock());
	constructee_selected = working_on_;
	auto p = zoom.TransformPoint({ 1500.0,50.0 });
	auto p2 = zoom.TransformPoint({ 1550.0,HEIGHT/2-50 });
	auto d = D2D1::Point2F(p2.x - p.x, p2.y - p.y);
	this->components_selection = new Scrollbar(main, f, p.x, p.y, d.x, d.y, d.y);
	this->components_selection->on_update = [this](double d) {this->components_selection_scroll_state = d; };
}

Constructor::~Constructor()
{
	auto c = closest.lock();
	if (c)c->highlight(0,0);
	c = constructee_selected.lock();
	if (c)c->controlled = 0;
	if (working_on)working_on->controlled = true;
	if (menu)
		DestroyWindow(menu->get_hwnd());
	if (this->components_selection)
		DestroyWindow(this->components_selection->get_hwnd());
}

bool Constructor::set_working_on(std::shared_ptr<Object> obj)
{
	auto buffer = dynamic_cast<Ship*>(&*obj);
	if (buffer) {
		working_on_ = obj;
		working_on = buffer;
	}
	return buffer;
}

void Constructor::init()
{
}

void Constructor::draw(ID2D1RenderTarget *r)
{
	if (!working_on)return;
	working_on->draw_full(r, gis.Gold);
	auto constructee = constructee_selected.lock();
	if (constructee)constructee->draw_full(r, gis.Green);
	auto sr = selected_r.lock();
	if (sr) {
		auto p = get_cp2(last_inverted, cp);
		r->DrawLine(D2D1::Point2F(sr->pos.x, sr->pos.y), D2D1::Point2F(p.x, p.y), gis.Green, 5);
	}
	auto sl = selected_last.lock();
	if (sl) { sl->draw(r,gis.Blue); }
	draw_selection(r);
}

void Constructor::draw_selection(ID2D1RenderTarget *r)
{
	auto&scroll = this->components_selection_scroll_state;
	auto inner_dy = 50.0 * this->components_selectable.size();
	auto outer_dy = HEIGHT / 2 - 100;
	auto ddy = inner_dy - outer_dy;
	auto min_y = 100+ddy*scroll;
	auto max_y = min_y+outer_dy;
	int min_i= std::lround(min_y / 50 - 2);
	int max_i = std::lround(max_y / 50 - 2);
	
	last_inverted = last;
	last_inverted.Invert();
	auto min = last_inverted.TransformPoint(D2D1::Point2F(0, 0));
	auto max = last_inverted.TransformPoint(D2D1::Point2F(WIDTH, HEIGHT));
	auto a = D2D1::Matrix3x2F::Translation(min.x, min.y)*last;
	r->SetTransform(a);
	selectable_inverted = a;
	selectable_inverted.Invert();
	for (int i = min_i; i < max_i&&i<this->components_selectable.size();i++) {
		this->components_selectable[i]->pos.y -= min_y-100;
		this->components_selectable[i]->draw_full(r, gis.Yellow);
		this->components_selectable[i]->pos.y += min_y-100;
	}
	r->SetTransform(last);
}

void Constructor::input(UINT msg, POINT cp, WPARAM wParam, LPARAM lParam, int scroll, HWND main)
{
	this->cp = cp;
	auto player = constructee_selected.lock();
	auto&highlighted = constructee_selected;
	switch (msg)
	{
	case WM_KEYDOWN:

#define k(vk)GetKeyState(vk)& 0x8000
		if (k(VK_KEY_E)) {
			if (player)
				player->controlled = false;
			auto o = closest.lock();
			if (o) {
				if (mouse_in_working_area) {
					o->controlled = true;
					constructee_selected = o;
				}
			}
		}
		if (k(VK_BACK) && player&&player != working_on_&&working_on) {
			working_on->detach(player);
			auto sl = selected_last.lock();
			if (player == sl)
			{
				selected_l = 0;
				set_selected_last();
			}
		}

		if (k(VK_ESCAPE))
			on_finish();
		break;
	case WM_LBUTTONDOWN:
		selected_l = 0;
		set_selected_last();
		if (GetCapture() != main) {
			SetCapture(main);
			update_closest();
			if (!mouse_in_working_area)
			{
				selected_l = std::shared_ptr<Object>(closest.lock()->clone());
				if (working_on)working_on->attach(selected_l);
			}
			else {
				selected_l = closest.lock();//already attached, on reposition and reconfigure
				if (selected_l == working_on_) {
					selected_l = 0;
					SetCapture(0);
				}
			}
			if(selected_l)selected_l->set_pos(get_cp2(last_inverted, cp));
		}		
		break;
	case WM_LBUTTONUP:
		if (GetCapture() == main) {
			SetCapture(0);
			auto p = get_cp2(selectable_inverted, cp);
			if (p.x >= 1000) {
				if (working_on)working_on->detach(selected_l);
				selected_l = 0;
			}set_selected_last(); selected_l = 0;
			if (working_on)working_on->update_outer_radius();
		}
		break;
	case WM_RBUTTONDOWN:
		if (GetCapture() != main) {
			SetCapture(main);
			update_closest();
			if(mouse_in_working_area) selected_r = closest;
			else {//remove component from list
				auto c = closest.lock();
				int i =std::lround(c->pos.y / 50 - 2);
				this->components_selectable.erase(this->components_selectable.begin() + i);
				for (int j = i; j < this->components_selectable.size(); j++) {
					this->components_selectable[j]->pos.y -= 50;
				}
				auto p = zoom.TransformPoint({ FLOAT(.0),FLOAT(100.0) });
				auto p2 = zoom.TransformPoint({FLOAT(.0),FLOAT(100.0 + 50.0 * this->components_selectable.size())});
				auto d = p2.y - p.y;
				if (this->components_selection)this->components_selection->set_inner_h(d);

			}
		}
		break;
	case WM_RBUTTONUP:
		if (GetCapture() == main) {
			SetCapture(0);
			update_closest();
			auto sr = selected_r.lock();
			auto c = closest.lock();
			if (working_on&&mouse_in_working_area&&sr&&sr!=c) {
				working_on->add_connector(c, sr);
			}
			selected_r = std::weak_ptr<Object>();
		}
		break;
	case WM_MOUSEMOVE:
		update_closest();
		if (selected_l)selected_l->set_pos(get_cp2(last_inverted, cp));
		break;
	case WM_KEYUP:
		switch (wParam) {
		case VK_KEY_C:
			auto sl = selected_last.lock();
			if (sl) {
				auto s = std::shared_ptr<Object>(sl->clone());
				this->components_selectable.push_back(s);
				s->pos = Comp<double>(1300, 100 + 50 * ((int)this->components_selectable.size()-1));
				auto p = zoom.TransformPoint({ (FLOAT).0,(FLOAT)100.0 });
				auto p2 = zoom.TransformPoint({ (FLOAT).0,FLOAT(100.0 + 50.0 * this->components_selectable.size()) });
				auto d = p2.y - p.y;
				if (this->components_selection)this->components_selection->set_inner_h(d);
				s->highlight(false, 0);
				s->controlled = 0;
			}
			break;
		}break;
	default:
		break;
	}
}

void Constructor::update_closest()
{
	auto p = get_cp2(selectable_inverted,cp);
	double d = std::numeric_limits<double>::infinity();
	bool c = p.x > 1000;
	mouse_in_working_area = !c;
	if (!c)p = get_cp2(last_inverted, cp);
	std::shared_ptr<Object>o;
	if (c) {
		auto&scroll = this->components_selection_scroll_state;
		auto inner_dy = 50.0 * this->components_selectable.size();
		auto outer_dy = HEIGHT / 2 - 100;
		auto ddy = inner_dy - outer_dy;
		auto min_y = ddy * scroll;
		p.y += min_y;
		for (auto&obj : this->components_selectable) {
			auto d2 = (obj->pos - p).getD();
			if (d2 < d) {
				d = d2;
				o = obj;
			}
		}
	}
	else {
		o = constructee_selected.lock();
		if(o)
			d = (o->pos - p).getD();
		auto d2 = (working_on_->pos - p).getD();
		if (d2 < d)
			o = working_on_;
	}

	auto o_old = closest.lock();
	if (o_old)o_old->highlight(false, p);
	closest = o;

	auto n = o ? o->highlight(true, p) : 0;
	if (n)closest = n;

	//if (c)constructee_selected = closest;
	//else selected_l = closest;
	return;
}
