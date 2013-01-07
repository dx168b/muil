/*=============================================================================

  Copyright (C) 2012 Denis Artyomov (denis.artyomov@gmail.com)

  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files (the "Software"), to deal
  in the Software without restriction, including without limitation the rights
  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
  copies of the Software, and to permit persons to whom the Software is
  furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in
  all copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
  SOFTWARE.

===============================================================================
  File:      muil_common.cpp
  Author(s): Denis Artyomov
=============================================================================*/

#include "muil_common.hpp"

namespace muil {

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct PaintData
{
	Display          &display;
	const FontInfo   *font;
	const FormColors *colors;

	PaintData(Display &display, const FontInfo *font, const FormColors *colors) :
		display(display),
		font(font),
		colors(colors) {}
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

enum HorizAlign
{
	HA_LEFT,
	HA_CENTER,
	HA_RIGHT,
};

static void paint_text_in_rect(PaintData &paint_data, const Rect &rect, HorizAlign align, const wchar_t *text, const Color &color)
{
	const Size display_size = paint_data.display.get_size();
	if (rect.intersects(Rect(Point(0, 0), display_size))) return;

	const Size text_size = paint_data.display.get_text_size(paint_data.font, text);

	int16_t x;
	int16_t y = (rect.y1 + rect.y2 - text_size.height) / 2;
	int16_t left_right_layer = text_size.height/4;
	switch (align)
	{
	case HA_LEFT:
		x = rect.x1 + left_right_layer;
		break;

	case HA_CENTER:
		x = (rect.x1 + rect.x2 - text_size.width) / 2;
		break;

	case HA_RIGHT:
		x = rect.x2 - text_size.width - left_right_layer;
		break;

	default:
		return;
	}

	paint_data.display.paint_text(paint_data.font, x, y, text, color);
}

static bool print_number(wchar_t *buffer, uint8_t buffer_len, int value, int pt)
{
	bool is_negative = value < 0;
	if (value < 0) value = -value;
	int tmp = value;
	uint8_t len = 0;
	while (tmp) { tmp /= 10; len++; }

	if (len < (pt+1)) len = pt+1;

	if (len + ((is_negative ? 1 : 0) + (pt != 0 ? 1 : 0) + 1) > buffer_len) return false;

	if (is_negative) *buffer++ = '-';

	buffer += len;
	if (pt != 0) buffer++;
	*buffer-- = 0;

	for (uint8_t i = 0; i < len; i++)
	{
		if ((pt != 0) && (i == pt)) *buffer-- = '.';
		*buffer-- = '0' + value % 10;
		value /= 10;
	}

	return true;
}

static void paint_button(PaintData &paint_data, const Rect &rect, bool pressed, bool shadow, bool vert_gradient = true)
{
	Color form_bg = paint_data.colors->form_bg;
	Color bg_color = paint_data.colors->btn_bg;
	Color bg_color1 = bg_color.light(pressed ? -32 : 0);
	Color bg_color2 = bg_color.light(pressed ? 32 : 64);

	if (vert_gradient)
	{
		int16_t my = rect.y1 + rect.height() / 4;
		Rect intern_rect1 = Rect(rect.x1+1, rect.y1+1, rect.x2-1, my);
		Rect intern_rect2 = Rect(rect.x1+1, my+1, rect.x2-1, rect.y2-1);
		paint_data.display.draw_vertical_gradient(intern_rect1, bg_color1, bg_color2);
		paint_data.display.draw_vertical_gradient(intern_rect2, bg_color2, bg_color1);
	}
	else
	{
		int16_t mx = rect.x1 + rect.width() / 4;
		Rect intern_rect1 = Rect(rect.x1+1, rect.y1+1, mx, rect.y2-1);
		Rect intern_rect2 = Rect(mx+1, rect.y1+1, rect.x2, rect.y2-1);
		paint_data.display.draw_horizontal_gradient(intern_rect1, bg_color1, bg_color2);
		paint_data.display.draw_horizontal_gradient(intern_rect2, bg_color2, bg_color1);
	}

	if (shadow)
	{
		Color sh_clr1 = pressed ? form_bg : form_bg.light(-10);
		Color sh_clr2 = pressed ? form_bg : form_bg.light(-40);
		paint_data.display.draw_rect(rect.inflated(2), 2, sh_clr1);
		paint_data.display.draw_rect(rect.inflated(1), 2, sh_clr2);
	}
	paint_data.display.draw_rect(rect, 2, paint_data.colors->ctrl_border);
}

void paint_tirangle(PaintData &paint_data, int16_t layer, int16_t x1, int16_t x2, int16_t y1, int16_t y2, bool up)
{
	x1 += layer;
	x2 -= layer;
	y1 += layer;
	y2 -= layer;
	int16_t mx = (x1 + x2) / 2;
	int16_t top = up ? y1 : y2;
	int16_t bottom = up ? y2 : y1;
	for (int16_t y = y1; y <= y2; y++)
	{
		int16_t left = linear_interpol(y, top, bottom, mx, x1);
		int16_t right = linear_interpol(y, top, bottom, mx, x2);
		paint_data.display.fill_rect(Rect(left, y, right, y), paint_data.colors->ctrl_sign);
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class WidgetsPaintVisitor : public IWidgetVisitor
{
public:
	WidgetsPaintVisitor(PaintData &paint_data, bool force_repaint_all_widgets) :
		paint_data_(paint_data),
		force_repaint_all_widgets_(force_repaint_all_widgets) {}

	void visit(Widget &widget)
	{
		if (widget.flags_.get(Widget::FLAG_INVALID) || force_repaint_all_widgets_)
		{
			paint_data_.display.set_offset(widget.get_pos());
			widget.paint(paint_data_);
			widget.flags_.clear(Widget::FLAG_INVALID);
		}
	}

private:
	PaintData &paint_data_;
	const bool force_repaint_all_widgets_;
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class TouchScreenPressVisitor : public IWidgetVisitor
{
public:
	TouchScreenPressVisitor(Form &form, EventType type, const Point pt, Widget** last_pressed_widget) :
		form_(form),
		type_(type),
		pt_(pt),
		last_pressed_widget_(last_pressed_widget) {}

	void visit(Widget &widget)
	{
		Rect rect(widget.get_pos(), widget.get_size());
		if (rect.contains(pt_))
		{
			const Point widget_pt = Point(pt_.x-rect.x1, pt_.y-rect.y1);
			widget.touch_screen_event(type_, widget_pt, &form_);
			form_.widget_event(type_, &widget);
			if (type_ == EVENT_TOUCHSCREEN_DOWN) *last_pressed_widget_ = &widget;
		}
	}

private:
	Form &form_;
	const EventType type_;
	const Point pt_;
	Widget **last_pressed_widget_;
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Widget::refresh()
{
	flags_.set(FLAG_INVALID, true);
	Application::get_instance()->refresh_active_form();
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Label::paint(PaintData &paint_data)
{
	paint_data.display.paint_text(paint_data.font, 0, 0, text_, Color::black());
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Indicator::paint(PaintData &paint_data)
{
	Rect rect(0, 0, size_.width, size_.height);
	paint_data.display.fill_rect(rect, Color::white());
	paint_text_in_rect(paint_data, rect, HA_CENTER, get_text(), Color::black());
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void ValueIndicator::set_value(int value)
{
	if (value_ == value) return;
	value_ = value;
	refresh();
}

const wchar_t* ValueIndicator::get_text()
{
	static wchar_t buffer[12];
	print_number(buffer, 12, value_, dec_pt_);
	return buffer;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void PressibleWidget::touch_screen_event(EventType type, const Point pt, Form *form)
{
	switch (type)
	{
	case EVENT_TOUCHSCREEN_DOWN:
		flags_.set(FLAG_PRESSED, true);
		refresh();
		break;

	case EVENT_TOUCHSCREEN_UP:
		flags_.set(FLAG_PRESSED, false);
		refresh();
		pressed(form);
		break;

	default:
		break;
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Button::paint(PaintData &paint_data)
{
	Rect rect(0, 0, size_.width, size_.height);

	paint_button(paint_data, rect, flags_.get(FLAG_PRESSED), true);
	paint_text_in_rect(paint_data, rect, HA_CENTER, text_, Color::black());
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void CheckBox::paint(PaintData &paint_data)
{
	Rect check_rect(0, 0, size_.height, size_.height);
	paint_button(paint_data, check_rect, flags_.get(FLAG_PRESSED), true);
	if (flags_.get(FLAG_CHECKED)) paint_check(paint_data, check_rect);

	Rect tect_rect(size_.height+size_.height/6, 0, size_.width, size_.height);
	paint_text_in_rect(paint_data, tect_rect, HA_LEFT, text_, Color::black());
}

void CheckBox::pressed(Form *form)
{
	flags_.toggle(FLAG_CHECKED);
}

void CheckBox::paint_check(PaintData &paint_data, const Rect &rect)
{
	int16_t layer = rect.height()/6;

	int16_t x1 = rect.x1 + layer;
	int16_t y1 = (rect.y1 + rect.y2) / 2;
	int16_t x2 = x1 + layer;
	int16_t y3 = rect.y2 - layer;
	int16_t y2 = y3 - layer;

	int16_t x4 = rect.x2 - layer;
	int16_t y4 = rect.y1 + layer;

	Color check_color = paint_data.colors->ctrl_sign;

	for (int16_t x = x1; x <= x2; x++)
	{
		int16_t yt = linear_interpol(x, x1, x2, y1, y2);
		int16_t yb = linear_interpol(x, x1, x2, y1, y3);
		paint_data.display.fill_rect(Rect(x, yt, x, yb), check_color);
	}

	for (int16_t x = x2; x <= x4; x++)
	{
		int16_t yt = linear_interpol(x, x2, x4, y2, y4);
		int16_t yb = linear_interpol(x, x2, x4, y3, y4);
		paint_data.display.fill_rect(Rect(x, yt, x, yb), check_color);
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void UpDownWidget::paint(PaintData &paint_data)
{
	Rect up_btn_rect, down_btn_rect;

	get_buttons_rects(paint_data.display, up_btn_rect, down_btn_rect);

	const Rect value_rect = Rect(0, 0, up_btn_rect.x1, size_.height);
	paint_data.display.fill_rect(value_rect, Color::white());

	paint_button(paint_data, up_btn_rect.inflated(1), flags_.get(FLAG_UP_BTN_PRESSED), false);
	paint_button(paint_data, down_btn_rect.inflated(1), flags_.get(FLAG_DOWN_BTN_PRESSED), false);

	int16_t layer = (up_btn_rect.width() + up_btn_rect.height()) / 7;
	paint_tirangle(paint_data, layer, up_btn_rect.x1, up_btn_rect.x2, up_btn_rect.y1, up_btn_rect.y2, true);
	paint_tirangle(paint_data, layer, down_btn_rect.x1, down_btn_rect.x2, down_btn_rect.y1, down_btn_rect.y2, false);

	wchar_t text_buf[10] = {0};
	print_number(text_buf, 10, value_, dec_pt_);
	paint_text_in_rect(paint_data, value_rect, HA_CENTER, text_buf, Color::black());
}

void UpDownWidget::get_buttons_rects(Display &display, Rect &up_btn_rect, Rect &down_btn_rect)
{
	uint16_t btn_width = display.get_dpi() / 4;
	int16_t my = size_.height / 2;
	int16_t x1 = size_.width - btn_width;
	up_btn_rect = Rect(x1, 0, size_.width, my);
	down_btn_rect = Rect(x1, my, size_.width, size_.height);
}

void UpDownWidget::touch_screen_event(EventType type, const Point pt, Form *form)
{
	Rect up_btn_rect, down_btn_rect;
	get_buttons_rects(*Application::get_instance()->get_display(), up_btn_rect, down_btn_rect);
	bool hit_up_btn = up_btn_rect.contains(pt);
	bool hit_down_btn = down_btn_rect.contains(pt);

	switch (type)
	{
	case EVENT_TOUCHSCREEN_UP:
		do { flags_.clear(FLAG_UP_BTN_PRESSED); // "do" used to prevent static analyze warning about missing "break"
		flags_.clear(FLAG_DOWN_BTN_PRESSED);

	case EVENT_TOUCHSCREEN_REPEATED:
		if (hit_up_btn) set_value(value_ + 1);
		if (hit_down_btn) set_value(value_ - 1);
		} while (false);
		break;

	case EVENT_TOUCHSCREEN_DOWN:
		flags_.set(FLAG_UP_BTN_PRESSED, hit_up_btn);
		flags_.set(FLAG_DOWN_BTN_PRESSED, hit_down_btn);
		if (hit_up_btn || hit_down_btn) refresh();
		break;
	}
}

void UpDownWidget::set_value(int value)
{
	if (value < min_) value = min_;
	if (value > max_) value = max_;
	if (value == value_) return;
	value_ = value;
	refresh();
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Choice::paint(PaintData &paint_data)
{
	const Size size = get_size();
	Rect btn_rect = Rect(size.width-size.height, 0, size.width, size.height);
	Rect data_rect = Rect(0, 0, btn_rect.x1, size.height);

	paint_data.display.fill_rect(data_rect, paint_data.colors->ctrl_bg);
	int selection = get_selection();
	if (selection != -1)
	{
		paint_text_in_rect(
			paint_data,
			data_rect,
			HA_LEFT,
			get_item(selection),
			paint_data.colors->form_text
		);
	}

	paint_button(paint_data, btn_rect.inflated(1), flags_.get(FLAG_PRESSED), true);
	int16_t layer = (btn_rect.width() + btn_rect.height()) / 6;
	paint_tirangle(paint_data, layer, btn_rect.x1, btn_rect.x2, btn_rect.y1, btn_rect.y2, false);
}

void Choice::pressed(Form *form)
{
	StringSelectorForm selector_form(caption_, form->get_font(), this, selection_);
	selector_form.show_modal();
	selection_ = selector_form.get_selection();
}

void Choice::set_selection(int selection)
{
	if (selection_ == selection) return;
	selection_ = selection;
	refresh();
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

size_t StringsChoice::get_items_count()
{
	size_t result = 0;
	while (items_[result]) result++;
	return result;
}

const wchar_t* StringsChoice::get_item(size_t index)
{
	return items_[index];
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static const FormColors default_colors = {
	Color::white(),
	Color::black(),
	Color(220, 220, 220),
	Color::black(),
	Color(150, 180, 180),
	Color::white(),
	Color::black(),
	Color::black(),
	Color(10, 80, 80),
	Color::white()
};

struct FormTouchScreenEventData
{
	EventType type;
	Point pt;
	Display *display;
};

Form::Form(const wchar_t *caption, const FontInfo *font, const FormColors *colors) :
	caption_     (caption),
	font_        (font),
	modal_result_(MR_NONE)
{
	colors_ = colors ? colors : &default_colors;
}

void Form::paint(Display *display, bool widgets_only, bool force_repaint_all_widgets)
{
	Rect caption_rect, client_rect;

	get_form_rects(display, &caption_rect, &client_rect);

	PaintData paint_data(*display, font_, colors_);

	if (!widgets_only)
	{
		display->set_offset(Point(0, 0));
		display->fill_rect(caption_rect, colors_->caption);
		paint_text_in_rect(paint_data, caption_rect, HA_CENTER, caption_, colors_->caption_text);
	}

	paint_client_area(paint_data, client_rect, force_repaint_all_widgets);
}

int16_t Form::get_caption_height() const
{
	return 3 * font_->heightPages / 2;
}

void Form::get_form_rects(const Display *display, Rect *caption_rect, Rect *client_rect) const
{
	Size disp_size = display->get_size();
	int16_t capt_height = get_caption_height();
	if (caption_rect) *caption_rect = Rect(0, 0, disp_size.width-1, capt_height-1);
	if (client_rect) *client_rect = Rect(0, capt_height, disp_size.width-1, disp_size.height-1);
}

void Form::show()
{
	Application *app = Application::get_instance();
	before_show(*app->get_display());
	app->set_active_form(this);
}

ModalResult Form::show_modal()
{
	Application *app = Application::get_instance();

	Form *prev_form = app->get_active_form();

	show();

	modal_result_ = MR_NONE;
	while (modal_result_ == MR_NONE)
		app->process_touch_screen_events();

	app->set_active_form(prev_form);

	return modal_result_;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void WidgetsForm::handle_touch_screen_event(FormTouchScreenEventData &event_data)
{
	if (event_data.type == EVENT_TOUCHSCREEN_UP)
	{
		if (last_pressed_widget_)
		{
			const Point widget_pos = last_pressed_widget_->get_pos();
			const Point up_rel_pos = event_data.pt.moved(-widget_pos.x, -widget_pos.y);
			last_pressed_widget_->touch_screen_event(EVENT_TOUCHSCREEN_UP, up_rel_pos, this);
			const Rect widget_rect(Point(0, 0), last_pressed_widget_->get_size());
			if (widget_rect.contains(up_rel_pos)) widget_event(EVENT_TOUCHSCREEN_UP, last_pressed_widget_);
			last_pressed_widget_ = NULL;
		}
	}
	else
	{
		TouchScreenPressVisitor touch_screen_visitor(*this, event_data.type, event_data.pt, &last_pressed_widget_);
		visit_all_widgets(touch_screen_visitor);
	}

	touch_screen_event(event_data);
}

void WidgetsForm::paint_client_area(PaintData &paint_data, const Rect &client_rect, bool force_repaint_all_widgets)
{
	if (force_repaint_all_widgets)
	{
		paint_data.display.set_offset(Point(0, 0));
		paint_data.display.fill_rect(client_rect, colors_->form_bg);
	}
	WidgetsPaintVisitor paint_visitor(paint_data, force_repaint_all_widgets);
	visit_all_widgets(paint_visitor);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct StringSelectorFormData
{
	int item_count;
	int item_height;
	int visible_count;
	int offscreen_items_count;

	uint16_t scr_bar_width;

	Rect client_rect;
	Rect scr_bar_area;
	Rect scr_bar_handle;
};

void StringSelectorForm::before_show(const Display &display)
{
	StringSelectorFormData data;

	get_form_data(display, data);

	top_item_index_ = selection_ - data.visible_count / 2;
	if (top_item_index_ < 0) top_item_index_ = 0;
	if (top_item_index_ > data.offscreen_items_count) top_item_index_ = data.offscreen_items_count;
}

void StringSelectorForm::handle_touch_screen_event(FormTouchScreenEventData &event_data)
{
	StringSelectorFormData data;

	get_form_data(*event_data.display, data);

	switch (event_data.type)
	{
	case EVENT_TOUCHSCREEN_DOWN:
		if (!data.scr_bar_area.contains(event_data.pt))
		{
			int16_t rel_y = (event_data.pt.y - data.client_rect.y1);
			if (rel_y < 0) return;
			int index = top_item_index_ + rel_y / data.item_height;
			if (index >= (int)items_provider_->get_items_count()) return;
			selection_ = index;
			paint(event_data.display, true, false);
			scroll_drag_start_y_ = -1;
		}
		else
		{
			scroll_drag_start_y_ = event_data.pt.y;
			scroll_drag_start_top_item_index_ = top_item_index_;
			Application::get_instance()->refresh_active_form();
		}
		break;

	case EVENT_TOUCHSCREEN_MOVE: {
			if (scroll_drag_start_y_ == -1) return;
			int move = (event_data.pt.y - scroll_drag_start_y_) / data.item_height;
			int new_top_item_index = scroll_drag_start_top_item_index_ + move;
			if (new_top_item_index < 0) new_top_item_index = 0;
			else if (new_top_item_index > data.offscreen_items_count) new_top_item_index = data.offscreen_items_count;
			if (new_top_item_index != top_item_index_)
			{
				top_item_index_ = new_top_item_index;
				Application::get_instance()->refresh_active_form();
			}
		}
		break;

	case EVENT_TOUCHSCREEN_UP:
		if (scroll_drag_start_y_ == -1) set_modal_result(MR_OK);
		else
		{
			scroll_drag_start_y_ = -1;
			Application::get_instance()->refresh_active_form();
		}
		break;

	default:
		break;
	}
}

void StringSelectorForm::paint_client_area(
	PaintData  &paint_data,
	const Rect &client_rect,
	bool       force_repaint_all_widgets)
{
	paint_data.display.set_offset(Point(0, 0));

	StringSelectorFormData data;
	get_form_data(paint_data.display, data);

	// items
	int16_t y2 = client_rect.y1;
	for (int i = top_item_index_; i < data.item_count; i++)
	{
		y2 = paint_item(i, paint_data, client_rect, data.item_count, data.item_height, data.scr_bar_width);
		if (y2 >= client_rect.y2) break;
	}

	// area under items
	if (y2 < client_rect.y2)
	{
		paint_data.display.fill_rect(
			Rect(client_rect.x1, y2, client_rect.x2, client_rect.y2),
			paint_data.colors->form_bg
		);
	}

	// scroll bar
	if (data.offscreen_items_count)
	{
		const Color sb_color = paint_data.colors->form_bg.light(-16);
		paint_data.display.fill_rect(
			Rect(data.scr_bar_area.x1, data.scr_bar_area.y1, data.scr_bar_area.x2, data.scr_bar_handle.y1),
			sb_color
		);
		paint_data.display.fill_rect(
			Rect(data.scr_bar_area.x1, data.scr_bar_handle.y2, data.scr_bar_area.x2, data.scr_bar_area.y2),
			sb_color
		);
		paint_button(paint_data, data.scr_bar_handle, scroll_drag_start_y_ != -1, false, false);
	}
}

int16_t StringSelectorForm::paint_item(
	int        item_index,
	PaintData  &paint_data,
	const Rect &client_rect,
	int        items_count,
	uint16_t   item_height,
	uint16_t   scr_bar_width)
{
	if (item_index < 0) return -1;
	if (item_index >= items_count) return -1;

	int y1 = client_rect.y1 + (item_index - top_item_index_) * item_height;
	int y2 = y1 + item_height;

	Rect item_rect(client_rect.x1, y1, client_rect.x2-1-scr_bar_width, y2-1);
	bool is_selected = (item_index == selection_);
	Color text_color = is_selected ? paint_data.colors->selection_text : paint_data.colors->form_text;
	Color sel_color = is_selected ? paint_data.colors->selection_bg : paint_data.colors->form_bg;

	if (!is_selected) paint_data.display.fill_rect(item_rect, sel_color);
	else paint_data.display.draw_vertical_gradient(item_rect, sel_color.light(64), sel_color);
	paint_text_in_rect(paint_data, item_rect, HA_LEFT, items_provider_->get_item(item_index), text_color);

	return y2;
}

void StringSelectorForm::get_form_data(const Display &display, StringSelectorFormData &data)
{
	get_form_rects(&display, NULL, &data.client_rect);
	int16_t client_height = data.client_rect.height();
	data.item_count = items_provider_->get_items_count();
	data.item_height = 3*get_font()->heightPages/2;
	data.visible_count = client_height / data.item_height;

	if (data.item_count < data.visible_count)
	{
		data.offscreen_items_count = 0;
		data.scr_bar_width = 0;
	}
	else
	{
		data.offscreen_items_count = data.item_count - data.visible_count;
		data.scr_bar_width = display.get_dpi() / 4;
		int16_t x1 = data.client_rect.x2 - data.scr_bar_width;
		int16_t x2 = data.client_rect.x2;
		int16_t y1 = data.client_rect.y1 + top_item_index_ * client_height / data.item_count;
		int16_t y2 = y1 + data.visible_count * client_height / data.item_count;
		data.scr_bar_area = Rect(x1, data.client_rect.y1, x2, data.client_rect.y2);
		data.scr_bar_handle = Rect(x1, y1, x2, y2);
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Application* Application::inst_ = NULL;

Application::Application() : active_form_(NULL)
{
	inst_ = this;
	pressed_counter_ = 0;
}

void Application::run()
{
	flags_.set(FLAG_PAINT_ACTIVE_FORM, true);

	prev_pt_ = Point(-1, -1);

	for (;;)
	{
		process_touch_screen_events();
	}
}

void Application::set_active_form(Form *active_form)
{
	active_form_ = active_form;
	flags_.clear(FLAG_PREV_TOUCH_SCREEN_PRESSED);
	flags_.set(FLAG_PAINT_ACTIVE_FORM, true);
}

void Application::process_touch_screen_events()
{
	if (active_form_ == NULL) return;

	Point cur_pt = Point(-1, -1);
	TouchScreen *touch_screen = get_touch_screen();
	bool touch_screen_pressed = touch_screen->is_pressed();

	FormTouchScreenEventData event;

	event.display = get_display();

	if (touch_screen_pressed)
	{
		cur_pt = touch_screen->get_pos();
		if ((cur_pt.x == -1) || (cur_pt.y == -1)) touch_screen_pressed = false;
	}

	if (touch_screen_pressed && !flags_.get(FLAG_PREV_TOUCH_SCREEN_PRESSED))
	{
		event.pt = cur_pt;
		event.type = EVENT_TOUCHSCREEN_DOWN;
		active_form_->handle_touch_screen_event(event);
		pressed_counter_ = 0;
	}

	else if (!touch_screen_pressed && flags_.get(FLAG_PREV_TOUCH_SCREEN_PRESSED))
	{
		event.pt = prev_pt_;
		event.type = EVENT_TOUCHSCREEN_UP;
		active_form_->handle_touch_screen_event(event);
	}

	else if (touch_screen_pressed && flags_.get(FLAG_PREV_TOUCH_SCREEN_PRESSED))
	{
		event.pt = cur_pt;

		if (prev_pt_ != cur_pt)
		{
			event.type = EVENT_TOUCHSCREEN_MOVE;
			active_form_->handle_touch_screen_event(event);
		}

		pressed_counter_++;
		if ((pressed_counter_ > 50) && ((pressed_counter_ % 10) == 0))
		{
			event.type = EVENT_TOUCHSCREEN_REPEATED;
			active_form_->handle_touch_screen_event(event);
		}
	}

	if (touch_screen_pressed) prev_pt_ = cur_pt;
	flags_.set(FLAG_PREV_TOUCH_SCREEN_PRESSED, touch_screen_pressed);

	if (active_form_)
	{
		if (flags_.get(FLAG_PAINT_ACTIVE_FORM))
		{
			flags_.clear(FLAG_PAINT_ACTIVE_FORM);
			active_form_->paint(event.display, false, true);
			flags_.clear(FLAG_REPAINT_WIDGETS_ON_ACTIVE_FORM);
		}

		if (flags_.get(FLAG_REPAINT_WIDGETS_ON_ACTIVE_FORM))
		{
			flags_.clear(FLAG_REPAINT_WIDGETS_ON_ACTIVE_FORM);
			active_form_->paint(event.display, true, false);
		}
	}

	delay_ms(10);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

} // end "namespace muil"
