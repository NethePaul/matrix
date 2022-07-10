#include "Constructor.h"
#include"Turret.h"
#include"object.h"
#include"jet.h"
#include"matrix.h"

Comp<double> get_cp2(D2D1::Matrix3x2F&m, POINT&cp) {
	auto pos = m.TransformPoint(D2D1::Point2F(cp.x, cp.y));
	return pos;
}
Constructor::Constructor(D2D1::Matrix3x2F&last, Game * context):context(context),working_on_(0),last(last)
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
}

Constructor::~Constructor()
{
	auto c = closest.lock();
	if (c)c->highlight(0,0);
	c = constructee_selected.lock();
	if (c)c->controlled = 0;
	if (working_on)working_on->controlled = true;
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
	draw_selection(r);
}

void Constructor::draw_selection(ID2D1RenderTarget *r)
{
	last_inverted = last;
	last_inverted.Invert();
	auto min = last_inverted.TransformPoint(D2D1::Point2F(0, 0));
	auto max = last_inverted.TransformPoint(D2D1::Point2F(WIDTH, HEIGHT));
	auto a = D2D1::Matrix3x2F::Translation(min.x, min.y)*last;
	r->SetTransform(a);
	selectable_inverted = a;
	selectable_inverted.Invert();
	for (auto&p : components_selectable) {
		p->draw_full(r, gis.Yellow);
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
				else
					selected_l = o;
			}
		}
		if (k(VK_BACK) && player&&player != working_on_&&working_on) {
			working_on->detach(player);
		}

		if (k(VK_ESCAPE))
			on_finish();
		break;
	case WM_LBUTTONDOWN:
		if (GetCapture() != main) {
			SetCapture(main);
			update_closest();
			if (!mouse_in_working_area)
			{
				selected_l = std::shared_ptr<Object>(closest.lock()->clone());
				if (working_on)working_on->attach(selected_l);
			}
			else selected_l = closest.lock();//already attached, on reposition and reconfigure
			selected_l->set_pos(get_cp2(last_inverted, cp));
		}		
		break;
	case WM_LBUTTONUP:
		if (GetCapture() == main) {
			SetCapture(0);
			auto p = get_cp2(selectable_inverted, cp);
			if (p.x >= 1000) {
				if (working_on)working_on->detach(selected_l);
			}selected_l = 0;
			if (working_on)working_on->update_outer_radius();
		}
		break;
	case WM_RBUTTONDOWN:
		if (GetCapture() != main) {
			SetCapture(main);
			update_closest();
			if(mouse_in_working_area) selected_r = closest;
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
