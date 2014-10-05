/*=============================================================================

  Copyright (C) 2012-2013 Denis Artyomov (denis.artyomov@gmail.com)

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

#include "basic_gui/muil_basic_gui.hpp"
#include "muil_widgets_gui.hpp"
#include "muil_touchscreen.hpp"
#include "muil.hpp"

namespace muil {

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct FormPaintData
{
	const FontInfo   *font;
	const FormColors *colors;
};

struct WidgetPaintData
{
	const Size &size;
	const Color &color;
	const wchar_t *text;
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static bool print_number(wchar_t *buffer, uint8_t buffer_len, int value, int pt)
{
	bool is_negative = value < 0;
	if (is_negative) value = -value;
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

static void paint_tirangle(
	FormPaintData &paint_data,
	int16_t       layer,
	Rect          rect,
	bool          up)
{
	rect.x1 += layer;
	rect.x2 -= layer;
	rect.y1 += layer;
	rect.y2 -= layer;
	int16_t mx = (rect.x1 + rect.x2) / 2;
	int16_t top = up ? rect.y1 : rect.y2;
	int16_t bottom = up ? rect.y2 : rect.y1;

	display_fill_triangle(rect.x1, bottom, mx, top, rect.x2, bottom, paint_data.colors->ctrl_sign);
}

static Point translate_rel_coord(uint8_t x, uint8_t y, const Rect &client_rect)
{
	return Point(
		x * client_rect.width() / 100 + client_rect.x1,
		y * client_rect.height() / 100 + client_rect.y1
	);
}

static Size translate_rel_size(uint8_t width, uint8_t height, const Rect &client_rect)
{
	return Size(
		width * client_rect.width() / 100,
		height * client_rect.height() / 100
	);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class WidgetsPaintVisitor : public IWidgetVisitor
{
public:
	WidgetsPaintVisitor(WidgetsForm &form, FormPaintData &paint_data, const Rect &client_rect, bool force_repaint_all_widgets) :
		form_(form),
		paint_data_(paint_data),
		client_rect_(client_rect),
		force_repaint_all_widgets_(force_repaint_all_widgets) {}

	void visit(Widget &widget, uint8_t x, uint8_t y, uint8_t width, uint8_t height, const wchar_t *text) override
	{
		if (widget.flags_.get(Widget::FLAG_INVALID) || force_repaint_all_widgets_)
		{
			Size size = translate_rel_size(width, height, client_rect_);
			Point widget_pos = translate_rel_coord(x, y, client_rect_);
			display_set_offset(widget_pos.x, widget_pos.y);
			Color default_color = widget.get_default_color(*paint_data_.colors);
			form_.get_widget_color(&widget, default_color);
			WidgetPaintData paint_data = { size, default_color, text };
			widget.paint(form_, paint_data_, paint_data);
			widget.flags_.clear(Widget::FLAG_INVALID);
		}
	}

private:
	WidgetsForm &form_;
	FormPaintData &paint_data_;
	const Rect &client_rect_;
	const bool force_repaint_all_widgets_;
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class TouchScreenPressVisitor : public IWidgetVisitor
{
public:
	TouchScreenPressVisitor(
		WidgetsForm& form,
		const Rect&  client_rect,
		EventType    type,
		const Point& pt,
		Widget**     last_pressed_widget,
		Point&       last_pressed_widget_pos,
		Size&        last_pressed_widget_size
	) :
		form_(form),
		client_rect_(client_rect),
		type_(type),
		pt_(pt),
		last_pressed_widget_(last_pressed_widget),
		last_pressed_widget_pos_(last_pressed_widget_pos),
		last_pressed_widget_size_(last_pressed_widget_size) {}

	void visit(Widget &widget, uint8_t x, uint8_t y, uint8_t width, uint8_t height, const wchar_t *text) override
	{
		Point pos = translate_rel_coord(x, y, client_rect_);
		Size size = translate_rel_size(width, height, client_rect_);
		Rect rect(pos, size);
		if (rect.contains(pt_))
		{
			const Point widget_pt = Point(pt_.x-rect.x1, pt_.y-rect.y1);
			widget.touch_screen_event(type_, widget_pt, size, &form_);
			form_.widget_event(type_, &widget);
			if (type_ == EventType::TouchscreenDown)
			{
				*last_pressed_widget_ = &widget;
				last_pressed_widget_pos_ = pos;
				last_pressed_widget_size_ = size;
			}
		}
	}

private:
	WidgetsForm &form_;
	const Rect &client_rect_;
	const EventType type_;
	const Point pt_;
	Widget **last_pressed_widget_;
	Point &last_pressed_widget_pos_;
	Size &last_pressed_widget_size_;
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Widget::refresh()
{
	flags_.set(FLAG_INVALID, true);
	Application::get_instance()->refresh_active_form();
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Label::paint(WidgetsForm &form, FormPaintData &paint_data, const WidgetPaintData &widget_pd)
{
	HorizAlign horiz_align = HorizAlign::Left;
	if (flags_.get(FLAG_ALIGN_RIGHT)) horiz_align = HorizAlign::Right;
	else if (flags_.get(FLAG_ALIGN_CENTER)) horiz_align = HorizAlign::Center;

	Rect rect(Point(0, 0), widget_pd.size);
	display_paint_text_in_rect(
		rect,
		horiz_align,
		widget_pd.text,
		paint_data.font,
		paint_data.colors->form_text,
		nullptr
	);
}

Color Label::get_default_color(const FormColors &colors) const
{
	return colors.form_bg;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Indicator::paint(WidgetsForm &form, FormPaintData &paint_data, const WidgetPaintData &widget_pd)
{
	Rect rect(Point(0, 0), widget_pd.size);
	display_fill_rect(rect, widget_pd.color);
	display_paint_text_in_rect(
		rect,
		HorizAlign::Center,
		get_text(),
		paint_data.font,
		paint_data.colors->form_text,
		nullptr
	);
}

Color Indicator::get_default_color(const FormColors &colors) const
{
	return Color::white();
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

void PressibleWidget::touch_screen_event(EventType type, const Point pt, const Size &size, Form *form)
{
	switch (type)
	{
	case EventType::TouchscreenDown:
		flags_.set(FLAG_PRESSED, true);
		refresh();
		break;

	case EventType::TouchscreenUp:
		flags_.set(FLAG_PRESSED, false);
		refresh();
		pressed(form);
		break;

	default:
		break;
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Button::paint(WidgetsForm &form, FormPaintData &paint_data, const WidgetPaintData &widget_pd)
{
	Rect rect(Point(0, 0), widget_pd.size);
	draw_button(rect, widget_pd.color, flags_.get(FLAG_PRESSED) ? ButtonStyle::Pressed : ButtonStyle::Normal, false);
	display_paint_text_in_rect(
		rect,
		HorizAlign::Center,
		widget_pd.text,
		paint_data.font,
		paint_data.colors->form_text,
		nullptr
	);
}

Color Button::get_default_color(const FormColors &colors) const
{
	return colors.btn_bg;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void CheckBox::paint(WidgetsForm &form, FormPaintData &paint_data, const WidgetPaintData &widget_pd)
{
	const Size& widget_size = widget_pd.size;
	Rect check_rect(0, 0, widget_size.height, widget_size.height);
	draw_checkbox_rect(check_rect, widget_pd.color, flags_.get(FLAG_PRESSED) ? ButtonStyle::Pressed : ButtonStyle::Normal);
	if (flags_.get(FLAG_CHECKED)) paint_check(paint_data, check_rect);

	Rect tect_rect(widget_size.height+widget_size.height/6, 0, widget_size.width, widget_size.height);
	display_paint_text_in_rect(
		tect_rect,
		HorizAlign::Left,
		widget_pd.text,
		paint_data.font,
		paint_data.colors->form_text,
		nullptr
	);
}

Color CheckBox::get_default_color(const FormColors &colors) const
{
	return colors.btn_bg;
}

void CheckBox::pressed(Form *form)
{
	flags_.toggle(FLAG_CHECKED);
}

void CheckBox::paint_check(FormPaintData &paint_data, const Rect &rect)
{
	int16_t layer = rect.height()/6;

	int16_t x1 = rect.x1 + layer;
	int16_t y1 = (rect.y1 + rect.y2) / 2;
	int16_t x2 = x1 + layer;
	int16_t y3 = rect.y2 - layer;
	int16_t y2 = y3 - layer;

	int16_t x4 = rect.x2 - layer;
	int16_t y4 = rect.y1 + layer;

	display_fill_triangle(x1, y1, x2, y2, x2, y3, paint_data.colors->ctrl_sign);
	display_fill_triangle(x2, y2, x2, y3, x4, y4, paint_data.colors->ctrl_sign);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void UpDownWidget::paint(WidgetsForm &form, FormPaintData &paint_data, const WidgetPaintData &widget_pd)
{
	Rect up_btn_rect, down_btn_rect;

	get_buttons_rects(widget_pd.size, up_btn_rect, down_btn_rect);

	const Rect value_rect = Rect(Point(0, 0), widget_pd.size);

	draw_indented_ctrl_rect(value_rect, widget_pd.color, ButtonStyle::Normal);

	draw_button(
		up_btn_rect,
		paint_data.colors->btn_bg,
		flags_.get(FLAG_UP_BTN_PRESSED) ? ButtonStyle::Pressed : ButtonStyle::Normal,
		true
	);

	draw_button(
		down_btn_rect,
		paint_data.colors->btn_bg,
		flags_.get(FLAG_DOWN_BTN_PRESSED) ? ButtonStyle::Pressed : ButtonStyle::Normal,
		true
	);

	int16_t layer = (up_btn_rect.width() + up_btn_rect.height()) / 6;

	paint_tirangle(paint_data, layer, up_btn_rect, true);

	paint_tirangle(paint_data, layer, down_btn_rect, false);

	wchar_t text_buf[10] = {0};
	print_number(text_buf, 10, value_, dec_pt_);
	display_paint_text_in_rect(
		Rect(0, 0, up_btn_rect.x1, widget_pd.size.height),
		HorizAlign::Center,
		text_buf,
		paint_data.font,
		paint_data.colors->form_text,
		nullptr
	);
}

Color UpDownWidget::get_default_color(const FormColors &colors) const
{
	return Color::white();
}

void UpDownWidget::get_buttons_rects(const Size &size, Rect &up_btn_rect, Rect &down_btn_rect)
{
	uint16_t btn_width = display_get_dpi() / 4;
	int16_t my = size.height / 2;
	int16_t x1 = size.width - btn_width;
	int border = get_indented_ctrl_border();
	up_btn_rect = Rect(x1, border, size.width-border, my);
	down_btn_rect = Rect(x1, my, size.width-border, size.height-border);
}

void UpDownWidget::touch_screen_event(EventType type, const Point pt, const Size &size, Form *form)
{
	Rect up_btn_rect, down_btn_rect;
	get_buttons_rects(size, up_btn_rect, down_btn_rect);
	bool hit_up_btn = up_btn_rect.contains(pt);
	bool hit_down_btn = down_btn_rect.contains(pt) && !hit_up_btn;

	switch (type)
	{
	case EventType::TouchscreenUp:
		{
			flags_.clear(FLAG_UP_BTN_PRESSED);
			flags_.clear(FLAG_DOWN_BTN_PRESSED);

	case EventType::TouchscreenRepeat:
			if (hit_up_btn) set_value(value_ + 1);
			else if (hit_down_btn) set_value(value_ - 1);
			else refresh();
		}
		break;

	case EventType::TouchscreenDown:
		flags_.set(FLAG_UP_BTN_PRESSED, hit_up_btn);
		flags_.set(FLAG_DOWN_BTN_PRESSED, hit_down_btn);
		if (hit_up_btn || hit_down_btn) refresh();
		break;

	default:
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

size_t ArrayOfStrings::get_items_count() const
{
	size_t result = 0;
	while (items_[result]) result++;
	return result;
}

const wchar_t* ArrayOfStrings::get_item(size_t index) const
{
	return items_[index];
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Choice::paint(WidgetsForm &form, FormPaintData &paint_data, const WidgetPaintData &widget_pd)
{
	const Size& widget_size = widget_pd.size;
	int border = get_indented_ctrl_border();
	Rect btn_rect = Rect(widget_size.width-widget_size.height+border, border, widget_size.width-border, widget_size.height-border);
	Rect data_rect = Rect(Point(0, 0), widget_size);
	ButtonStyle btn_style = flags_.get(FLAG_PRESSED) ? ButtonStyle::Pressed : ButtonStyle::Normal;
	draw_choice_rect(data_rect, widget_pd.color, btn_style);
	int selection = get_selection();
	if (selection != -1)
	{
		display_paint_text_in_rect(
			data_rect,
			HorizAlign::Left,
			items_provider_.get_item(selection),
			paint_data.font,
			paint_data.colors->form_text,
			nullptr
		);
	}
	draw_button(btn_rect, paint_data.colors->btn_bg, btn_style, true);
	int16_t layer = (btn_rect.width() + btn_rect.height()) / 6;
	paint_tirangle(paint_data, layer, btn_rect, false);
}

Color Choice::get_default_color(const FormColors &colors) const
{
	return colors.ctrl_bg;
}

void Choice::pressed(Form *form)
{
	StringSelectorForm selector_form(caption_, form->get_font(), &items_provider_, selection_);
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

static const FormColors default_colors = {
	Color::white(),
	Color::black(),
	Color(210, 210, 210),
	Color::black(),
	Color(190, 190, 205),
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
};

Form::Form(const wchar_t *caption, const FontInfo *font, const FormColors *colors) :
	caption_     (caption),
	font_        (font),
	modal_result_(ModalResult::None)
{
	colors_ = colors ? colors : &default_colors;
}

void Form::paint(bool widgets_only, bool force_repaint_all_widgets)
{
	Rect caption_rect, client_rect;

	get_form_rects(&caption_rect, &client_rect);

	FormPaintData paint_data = { font_, colors_};

	if (!widgets_only)
	{
		display_set_offset(0, 0);
		display_fill_rect(caption_rect, colors_->caption);
		display_paint_text_in_rect(caption_rect, HorizAlign::Center, caption_, font_, colors_->caption_text, nullptr);
	}

	paint_client_area(paint_data, client_rect, force_repaint_all_widgets);
}

int16_t Form::get_caption_height() const
{
	return 3 * font_->heightPages / 2;
}

void Form::get_form_rects(Rect *caption_rect, Rect *client_rect) const
{
	unsigned disp_width = display_get_width();
	unsigned disp_height = display_get_height();
	int16_t capt_height = get_caption_height();
	if (caption_rect) *caption_rect = Rect(0, 0, disp_width - 1, capt_height - 1);
	if (client_rect) *client_rect = Rect(0, capt_height, disp_width - 1, disp_height - 1);
}

void Form::show()
{
	before_show();
	Application::get_instance()->set_active_form(this);
}

ModalResult Form::show_modal()
{
	Application *app = Application::get_instance();

	Form *prev_form = app->get_active_form();

	show();

	modal_result_ = ModalResult::None;
	while (modal_result_ == ModalResult::None)
		app->process_touch_screen_events();

	app->set_active_form(prev_form);

	return modal_result_;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void WidgetsForm::handle_touch_screen_event(FormTouchScreenEventData &event_data)
{
	if (event_data.type == EventType::TouchscreenUp)
	{
		if (last_pressed_widget_)
		{
			const Point up_rel_pos = event_data.pt.moved(-last_pressed_widget_pos_.x, -last_pressed_widget_pos_.y);

			last_pressed_widget_->touch_screen_event(
				EventType::TouchscreenUp,
				up_rel_pos,
				last_pressed_widget_size_,
				this
			);

			const Rect widget_rect(Point(0, 0), last_pressed_widget_size_);
			if (widget_rect.contains(up_rel_pos)) 
				widget_event(EventType::TouchscreenUp, last_pressed_widget_);
			last_pressed_widget_ = NULL;
		}
	}
	else
	{
		Rect client_rect;
		get_form_rects(NULL, &client_rect);
		TouchScreenPressVisitor touch_screen_visitor(
			*this,
			client_rect,
			event_data.type,
			event_data.pt,
			&last_pressed_widget_,
			last_pressed_widget_pos_,
			last_pressed_widget_size_
		);
		visit_all_widgets(touch_screen_visitor);
	}

	touch_screen_event(event_data);
}

void WidgetsForm::paint_client_area(
	FormPaintData &paint_data,
	const Rect    &client_rect,
	bool          force_repaint_all_widgets)
{
	if (force_repaint_all_widgets)
	{
		display_set_offset(0, 0);
		display_fill_rect(client_rect, colors_->form_bg);
	}
	WidgetsPaintVisitor paint_visitor(*this, paint_data, client_rect, force_repaint_all_widgets);
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

	StringSelectorFormData() :
		item_count(0),
		item_height(0),
		visible_count(0),
		offscreen_items_count(0),
		scr_bar_width(0) {}
};

void StringSelectorForm::before_show()
{
	StringSelectorFormData data;

	get_form_data(data);

	top_item_index_ = selection_ - data.visible_count / 2;
	if (top_item_index_ < 0) top_item_index_ = 0;
	if (top_item_index_ > data.offscreen_items_count) top_item_index_ = data.offscreen_items_count;
}

void StringSelectorForm::handle_touch_screen_event(FormTouchScreenEventData &event_data)
{
	StringSelectorFormData data;

	get_form_data(data);

	switch (event_data.type)
	{
	case EventType::TouchscreenDown:
		if (!data.scr_bar_area.contains(event_data.pt))
		{
			int16_t rel_y = (event_data.pt.y - data.client_rect.y1);
			if (rel_y < 0) return;
			int index = top_item_index_ + rel_y / data.item_height;
			if (index >= (int)items_provider_->get_items_count()) return;
			selection_ = index;
			paint(true, false);
		}
		else Application::get_instance()->refresh_active_form();

		scroll_drag_start_y_ = event_data.pt.y;
		scroll_drag_start_top_item_index_ = top_item_index_;

		flags_.clear(FLAG_WAS_SCROLLED);
		break;

	case EventType::TouchscreenMove:
		{
			int move = (event_data.pt.y - scroll_drag_start_y_) / data.item_height;
			int new_top_item_index = scroll_drag_start_top_item_index_ - move;
			if (new_top_item_index < 0) new_top_item_index = 0;
			else if (new_top_item_index > data.offscreen_items_count) 
				new_top_item_index = data.offscreen_items_count;
			if (new_top_item_index != top_item_index_)
			{
				flags_.on(FLAG_WAS_SCROLLED);
				selection_ = -1;
				top_item_index_ = new_top_item_index;
				Application::get_instance()->refresh_active_form();
			}
		}
		break;

	case EventType::TouchscreenUp:
		if (!flags_.get(FLAG_WAS_SCROLLED)) set_modal_result(ModalResult::Ok);
		else Application::get_instance()->refresh_active_form();
		break;

	default:
		break;
	}
}

void StringSelectorForm::paint_client_area(
	FormPaintData &paint_data,
	const Rect    &client_rect,
	bool          force_repaint_all_widgets)
{
	display_set_offset(0, 0);

	StringSelectorFormData data;
	get_form_data(data);

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
		display_fill_rect(
			client_rect.x1, y2, client_rect.x2, client_rect.y2,
			paint_data.colors->form_bg
		);
	}

	// scroll bar
	if (data.offscreen_items_count)
	{
		const Color sb_color = paint_data.colors->form_bg.light(-16);
		display_fill_rect(
			data.scr_bar_area.x1, data.scr_bar_area.y1, data.scr_bar_area.x2, data.scr_bar_handle.y1,
			sb_color
		);
		display_fill_rect(
			data.scr_bar_area.x1, data.scr_bar_handle.y2, data.scr_bar_area.x2, data.scr_bar_area.y2,
			sb_color
		);

		draw_button(
			data.scr_bar_handle,
			paint_data.colors->btn_bg,
			scroll_drag_start_y_ != -1 ? ButtonStyle::Pressed : ButtonStyle::Normal,
			false
		);
	}
}

int16_t StringSelectorForm::paint_item(
	int            item_index,
	FormPaintData& paint_data,
	const Rect&    client_rect,
	int            items_count,
	uint16_t       item_height,
	uint16_t       scr_bar_width)
{
	if (item_index < 0) return -1;
	if (item_index >= items_count) return -1;

	int y1 = client_rect.y1 + (item_index - top_item_index_) * item_height;
	int y2 = y1 + item_height;

	Rect item_rect(client_rect.x1, y1, client_rect.x2-1-scr_bar_width, y2-1);
	bool is_selected = (item_index == selection_);
	Color text_color = is_selected ? paint_data.colors->selection_text : paint_data.colors->form_text;
	Color sel_color = is_selected ? paint_data.colors->selection_bg : paint_data.colors->form_bg;

	if (!is_selected) display_fill_rect(item_rect, sel_color);
	else display_draw_vertical_gradient(item_rect, sel_color.light(64), sel_color);

	display_paint_text_in_rect(
		item_rect,
		HorizAlign::Left,
		items_provider_->get_item(item_index),
		get_font(),
		text_color,
		nullptr
	);

	return y2;
}

void StringSelectorForm::get_form_data(StringSelectorFormData &data)
{
	get_form_rects(NULL, &data.client_rect);
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
		data.scr_bar_width = display_get_dpi() / 8;
		int16_t x1 = data.client_rect.x2 - data.scr_bar_width;
		int16_t x2 = data.client_rect.x2;
		int16_t y1 = data.client_rect.y1 + top_item_index_ * client_height / data.item_count;
		int16_t y2 = y1 + data.visible_count * client_height / data.item_count;
		data.scr_bar_area = Rect(x1, data.client_rect.y1, x2, data.client_rect.y2);
		data.scr_bar_handle = Rect(x1, y1, x2, y2);
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void DialogForm::visit_all_widgets(IWidgetVisitor &visitor)
{
	visitor.visit(btn_ok_,      5, 85, 43, 12, L"OK"    );
	visitor.visit(btn_cancel_, 52, 85, 43, 12, L"Cancel");
}

void DialogForm::widget_event(muil::EventType type, const muil::Widget *widget)
{
	if (type == muil::EventType::TouchscreenUp)
	{
		if (widget == &btn_ok_)
			set_modal_result(muil::ModalResult::Ok);

		else if (widget == &btn_cancel_)
			set_modal_result(muil::ModalResult::Cancel);
	}
}

void DialogForm::get_widget_color(const muil::Widget *widget, muil::Color &color)
{
	if (widget == &btn_ok_) color = muil::Color(160, 255, 160);
	else if (widget == &btn_cancel_) color = muil::Color(255, 160, 160);

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
	bool touch_screen_pressed = touchscreen_is_pressed();

	FormTouchScreenEventData event;

	if (touch_screen_pressed)
	{
		touch_screen_pressed = touchscreen_get_pos(cur_pt.x, cur_pt.y);
	}

	if (touch_screen_pressed && !flags_.get(FLAG_PREV_TOUCH_SCREEN_PRESSED))
	{
		event.pt = cur_pt;
		event.type = EventType::TouchscreenDown;
		active_form_->handle_touch_screen_event(event);
		pressed_counter_ = 0;
	}

	else if (!touch_screen_pressed && flags_.get(FLAG_PREV_TOUCH_SCREEN_PRESSED))
	{
		event.pt = prev_pt_;
		event.type = EventType::TouchscreenUp;
		active_form_->handle_touch_screen_event(event);
	}

	else if (touch_screen_pressed && flags_.get(FLAG_PREV_TOUCH_SCREEN_PRESSED))
	{
		event.pt = cur_pt;

		if (prev_pt_ != cur_pt)
		{
			event.type = EventType::TouchscreenMove;
			active_form_->handle_touch_screen_event(event);
		}

		pressed_counter_++;
		if ((pressed_counter_ > 50) && ((pressed_counter_ % 10) == 0))
		{
			event.type = EventType::TouchscreenRepeat;
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
			active_form_->paint(false, true);
			flags_.clear(FLAG_REPAINT_WIDGETS_ON_ACTIVE_FORM);
		}

		if (flags_.get(FLAG_REPAINT_WIDGETS_ON_ACTIVE_FORM))
		{
			flags_.clear(FLAG_REPAINT_WIDGETS_ON_ACTIVE_FORM);
			active_form_->paint(true, false);
		}
	}

	delay_ms(10);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

} // end "namespace muil"
