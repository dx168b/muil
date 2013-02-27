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
  File:      muil_common.hpp
  Author(s): Denis Artyomov
=============================================================================*/


#ifndef MUIL_COMMON_HPP_FILE_INCLUDED_
#define MUIL_COMMON_HPP_FILE_INCLUDED_

#include <stddef.h>
#include <limits.h>

#include "muil_classes.hpp"
#include "muil_utils.hpp"

namespace muil {

// fwd.
struct PaintData;
class Form;
class WidgetsForm;
struct FormTouchScreenEventData;
struct StringSelectorFormData;

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

enum EventType
{
	EVENT_TOUCHSCREEN_DOWN,
	EVENT_TOUCHSCREEN_UP,
	EVENT_TOUCHSCREEN_REPEATED,
	EVENT_TOUCHSCREEN_MOVE,
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

enum ModalResult
{
	MR_NONE,
	MR_OK,
	MR_CANCEL,
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct FormColors
{
	const Color caption;
	const Color caption_text;
	const Color form_bg;
	const Color form_text;
	const Color btn_bg;
	const Color ctrl_bg;
	const Color ctrl_border;
	const Color ctrl_sign;
	const Color selection_bg;
	const Color selection_text;
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class Widget
{
public:
	virtual void paint(WidgetsForm &form, PaintData &paint_data, const Size &widget_size, const Color &color) = 0;
	virtual Color get_default_color(const FormColors &colors) const = 0;

	virtual void touch_screen_event(EventType type, const Point pt, const Size &size, Form *form) {}

	void refresh();

	const static uint32_t FLAG_INVALID = 0x0001;


protected:
	Flags<uint32_t> flags_;

	friend class WidgetsPaintVisitor;
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class Label : public Widget
{
public:
	Label(const wchar_t *text, uint32_t align_flags = FLAG_ALIGN_LEFT) : text_(text)
	{
		flags_.on(align_flags & (FLAG_ALIGN_LEFT | FLAG_ALIGN_RIGHT | FLAG_ALIGN_CENTER));
	}

	void paint(WidgetsForm &form, PaintData &paint_data, const Size &widget_size, const Color &color);
	Color get_default_color(const FormColors &colors) const;

	static const uint32_t FLAG_ALIGN_LEFT   = 0x10000;
	static const uint32_t FLAG_ALIGN_RIGHT  = 0x20000;
	static const uint32_t FLAG_ALIGN_CENTER = 0x40000;

private:
	const wchar_t *text_;
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class Indicator : public Widget
{
public:
	void paint(WidgetsForm &form, PaintData &paint_data, const Size &widget_size, const Color &color);
	Color get_default_color(const FormColors &colors) const;

protected:
	virtual const wchar_t* get_text() = 0;
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class ValueIndicator : public Indicator
{
public:
	ValueIndicator(int value = 0, uint8_t dec_pt = 0) :
		value_(value),
		dec_pt_(dec_pt) {}

	void set_value(int value);

protected:
	const wchar_t* get_text();

private:
	int value_;
	uint8_t dec_pt_;
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class TextIndicator : public Indicator
{
public:
	TextIndicator(const wchar_t* text) : text_(text) {}

protected:
	const wchar_t* get_text() { return text_; }

private:
	const wchar_t* text_;
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class PressibleWidget : public Widget
{
public:
	static const uint32_t FLAG_PRESSED = 0x0002;

protected:
	void touch_screen_event(EventType type, const Point pt, const Size &size, Form *form);
	virtual void pressed(Form *form) = 0;
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class Button : public PressibleWidget
{
public:
	Button(const wchar_t *text, bool default_btn = false) : text_(text)
	{
		flags_.set(FLAG_DEFAULT, default_btn);
	}

	static const uint32_t FLAG_DEFAULT = 0x10000;

	void paint(WidgetsForm &form, PaintData &paint_data, const Size &widget_size, const Color &color);
	Color get_default_color(const FormColors &colors) const;

protected:
	void pressed(Form *form) {}

private:
	const wchar_t *text_;
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class CheckBox : public PressibleWidget
{
public:
	CheckBox(const wchar_t *text, bool checked = false) : text_(text)
	{
		flags_.set(FLAG_CHECKED, checked);
	}

	static const uint32_t FLAG_CHECKED = 0x10000;

	void paint(WidgetsForm &form, PaintData &paint_data, const Size &widget_size, const Color &color);
	Color get_default_color(const FormColors &colors) const;

protected:
	void pressed(Form *form);

private:
	const wchar_t *text_;

	static void paint_check(PaintData &paint_data, const Rect &rect);
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class UpDownWidget : public Widget
{
public:
	UpDownWidget(uint8_t dec_pt = 0, int value = 0, int min = INT_MIN, int max = INT_MAX) :
		value_ (value),
		max_   (max),
		min_   (min),
		dec_pt_(dec_pt) {}

	void set_value(int value);

	void paint(WidgetsForm &form, PaintData &paint_data, const Size &widget_size, const Color &color);
	Color get_default_color(const FormColors &colors) const;

protected:
	void touch_screen_event(EventType type, const Point pt, const Size &size, Form *form);

	static const uint32_t FLAG_UP_BTN_PRESSED = 0x10000;
	static const uint32_t FLAG_DOWN_BTN_PRESSED = 0x20000;

private:
	int value_;
	int max_;
	int min_;
	uint8_t dec_pt_;

	void get_buttons_rects(const Size &size, Display &display, Rect &up_btn_rect, Rect &down_btn_rect);
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class IStringItemsProvider
{
public:
	virtual size_t get_items_count() = 0;
	virtual const wchar_t* get_item(size_t index) = 0;
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class Choice : public PressibleWidget, public IStringItemsProvider // TODO: IStringItemsProvider затолкать внутрь
{
public:
	Choice(const wchar_t *caption, int selection) :
		selection_(selection),
		caption_(caption) {}

	int get_selection() const { return selection_; }
	void set_selection(int selection);

	void paint(WidgetsForm &form, PaintData &paint_data, const Size &widget_size, const Color &color);
	Color get_default_color(const FormColors &colors) const;

protected:
	void pressed(Form *form);

private:
	int selection_;
	const wchar_t *caption_;
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class StringsChoice : public Choice
{
public:
	StringsChoice(const wchar_t *caption, const wchar_t** items, int sel_index) :
		Choice(caption, sel_index),
		items_(items) {}

	size_t get_items_count();
	const wchar_t* get_item(size_t index);

private:
	const wchar_t** items_;
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class IWidgetVisitor
{
public:
	virtual void visit(Widget &widget, uint8_t x, uint8_t y, uint8_t width, uint8_t height) = 0;
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class Form
{
public:
	Form(const wchar_t *caption, const FontInfo *font, const FormColors *colors = NULL);

	void show();

	const FontInfo* get_font() const { return font_; }

	ModalResult show_modal();

	void set_modal_result(ModalResult modal_result)
	{
		modal_result_ = modal_result;
	}

	ModalResult get_modal_result() const
	{
		return modal_result_;
	}

	virtual void handle_touch_screen_event(FormTouchScreenEventData &event_data) = 0;

	void paint(Display &display, bool widgets_only, bool force_repaint_all_widgets);

protected:
	virtual void paint_client_area(PaintData &paint_data, const Rect &client_rect, bool force_repaint_all_widgets) = 0;

	virtual void touch_screen_event(FormTouchScreenEventData &event_data) {}
	virtual void before_show(const Display &display) {}

	int16_t get_caption_height() const;

	void get_form_rects(const Display &display, Rect *caption_rect, Rect *client_rect) const;

	const FormColors *colors_;
	Flags<uint32_t> flags_;

private:
	const wchar_t *caption_;
	const FontInfo *font_;
	volatile ModalResult modal_result_;

};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class WidgetsForm : public Form
{
public:
	WidgetsForm(const wchar_t *caption, const FontInfo *font) :
		Form(caption, font),
		last_pressed_widget_(NULL) {}

	void handle_touch_screen_event(FormTouchScreenEventData &event_data);

protected:
	virtual void visit_all_widgets(IWidgetVisitor &visitor) = 0;
	virtual void widget_event(EventType type, const Widget *widget) {}
	virtual void get_widget_color(const Widget *widget, Color &color) {}

	void paint_client_area(PaintData &paint_data, const Rect &client_rect, bool force_repaint_all_widgets);

private:
	Widget *last_pressed_widget_;
	Point last_pressed_widget_pos_;
	Size last_pressed_widget_size_;

	friend class TouchScreenPressVisitor; // for calling widget_event function
	friend class WidgetsPaintVisitor; // for calling get_widget_color
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class StringSelectorForm : public Form
{
public:
	StringSelectorForm(const wchar_t *caption, const FontInfo *font, IStringItemsProvider *items_provider, int selection) :
		Form(caption, font),
		items_provider_(items_provider),
		selection_(selection),
		top_item_index_(0),
		scroll_drag_start_y_(-1) {}

	void handle_touch_screen_event(FormTouchScreenEventData &event_data);

	int get_selection() const { return selection_; }

protected:
	void paint_client_area(PaintData &paint_data, const Rect &client_rect, bool force_repaint_all_widgets);
	void before_show(const Display &display);

	int16_t paint_item(int item_index, PaintData &paint_data, const Rect &client_rect, int items_count, uint16_t item_height, uint16_t scr_bar_width);

private:
	IStringItemsProvider *items_provider_;
	int selection_;
	int top_item_index_;
	int16_t scroll_drag_start_y_;
	int16_t scroll_drag_start_top_item_index_;

	static const uint32_t FLAG_WAS_SCROLLED = 0x10000;

	void get_form_data(const Display &display, StringSelectorFormData &data);
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class Application
{
public:
	Application();

	void run();

	void process_touch_screen_events();

	void set_active_form(Form *active_form);

	Form* get_active_form() { return active_form_; }

	static Application* get_instance() { return inst_; }

	void refresh_active_form() { flags_.set(FLAG_REPAINT_WIDGETS_ON_ACTIVE_FORM, true); }

	virtual Display* get_display() = 0;
	virtual TouchScreen* get_touch_screen() = 0;
	virtual void delay_ms(uint16_t milliseconds) = 0;

private:
	static const uint32_t FLAG_PREV_TOUCH_SCREEN_PRESSED = 0x1;
	static const uint32_t FLAG_REPAINT_WIDGETS_ON_ACTIVE_FORM = 0x2;
	static const uint32_t FLAG_PAINT_ACTIVE_FORM = 0x4;

	Form *active_form_;
	static Application *inst_;
	Point prev_pt_;
	Flags<uint32_t> flags_;
	int32_t pressed_counter_;
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

} // end "namespace muil"

#endif
