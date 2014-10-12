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

#include "basic_gui/muil_basic_gui.hpp"
#include "muil_utils.hpp"

namespace muil {

// fwd.
class Form;
class WidgetsForm;
struct FontInfo;
struct FormTouchScreenEventData;
struct StringSelectorFormData;
struct FormPaintData;
struct WidgetPaintData;

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

enum class EventType
{
	TouchscreenDown,
	TouchscreenUp,
	TouchscreenRepeat,
	TouchscreenMove,
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

enum class ModalResult
{
	None,
	Ok,
	Cancel,
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
	virtual void paint(WidgetsForm &form, FormPaintData &paint_data, const WidgetPaintData &widget_pd) = 0;
	virtual Color get_default_color(const FormColors &colors) const = 0;
	virtual void touch_screen_event(EventType type, const Point pt, const Size &size, Form *form) {}
	void refresh();

	const static uint32_t FLAG_INVALID = 0x0001;

protected:
	Flags<uint32_t> flags_;

	friend class WidgetsPaintVisitor;
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class Indicator : public Widget
{
public:
	Indicator(uint32_t align_flags = FLAG_ALIGN_LEFT)
	{
		flags_.on(align_flags & (FLAG_ALIGN_LEFT | FLAG_ALIGN_RIGHT | FLAG_ALIGN_CENTER));
	}

	void paint(WidgetsForm &form, FormPaintData &paint_data, const WidgetPaintData &widget_pd);
	Color get_default_color(const FormColors &colors) const override;

	static const uint32_t FLAG_ALIGN_LEFT = 0x10000;
	static const uint32_t FLAG_ALIGN_RIGHT = 0x20000;
	static const uint32_t FLAG_ALIGN_CENTER = 0x40000;

protected:
	virtual const CharactersProvider& get_text_provider() const = 0;
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class ValueIndicator : public Indicator
{
public:
	ValueIndicator(int value = 0, int8_t dec_pt = 0, uint32_t align_flags = FLAG_ALIGN_LEFT) 
		: 
		Indicator(align_flags),
		provider_(value, dec_pt) {}

	void set_value(int value);

protected:
	const CharactersProvider& get_text_provider() const override
	{
		return provider_;
	}

private:
	Integer10CharactersProvider provider_;
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class Label : public Indicator
{
public:
	Label(const wchar_t* text, uint32_t align_flags = FLAG_ALIGN_LEFT)
		: Indicator(align_flags), 
		provider_(text) {}

protected:
	const CharactersProvider& get_text_provider() const override
	{
		return provider_;
	}

private:
	TextCharactersProvider provider_;
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class PressibleWidget : public Widget
{
public:
	static const uint32_t FLAG_PRESSED = 0x0002;

	void touch_screen_event(EventType type, const Point pt, const Size &size, Form *form) override;

protected:
	virtual void pressed(Form *form) = 0;
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class Button : public PressibleWidget
{
public:
	Button(bool default_btn = false)
	{
		flags_.set(FLAG_DEFAULT, default_btn);
	}

	static const uint32_t FLAG_DEFAULT = 0x10000;

	void paint(WidgetsForm &form, FormPaintData &paint_data, const WidgetPaintData &widget_pd) override;
	Color get_default_color(const FormColors &colors) const override;

protected:
	void pressed(Form *form) {}
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class CheckBox : public PressibleWidget
{
public:
	CheckBox(bool checked = false)
	{
		flags_.set(FLAG_CHECKED, checked);
	}

	static const uint32_t FLAG_CHECKED = 0x10000;

	void paint(WidgetsForm &form, FormPaintData &paint_data, const WidgetPaintData &widget_pd) override;
	Color get_default_color(const FormColors &colors) const override;

protected:
	void pressed(Form *form);

private:
	static void paint_check(FormPaintData &paint_data, const Rect &rect);
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class UpDownWidget : public Widget
{
public:
	UpDownWidget(int8_t dec_pt = 0, int value = 0, int min = INT_MIN, int max = INT_MAX) :
		value_ (value),
		max_   (max),
		min_   (min),
		dec_pt_(dec_pt) {}

	void set_value(int value);

	void paint(WidgetsForm &form, FormPaintData &paint_data, const WidgetPaintData &widget_pd) override;
	Color get_default_color(const FormColors &colors) const override;

protected:
	void touch_screen_event(EventType type, const Point pt, const Size &size, Form *form) override;

	static const uint32_t FLAG_UP_BTN_PRESSED = 0x10000;
	static const uint32_t FLAG_DOWN_BTN_PRESSED = 0x20000;

private:
	int value_;
	int max_;
	int min_;
	int8_t dec_pt_;

	void get_buttons_rects(const Size &size, Rect &up_btn_rect, Rect &down_btn_rect);
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class IStringItemsProvider
{
public:
	virtual size_t get_items_count() const = 0;
	virtual const wchar_t* get_item(size_t index) const = 0;
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class ArrayOfStrings : public IStringItemsProvider
{
public:
	ArrayOfStrings(const wchar_t** items) : items_(items) {}

	size_t get_items_count() const;
	const wchar_t* get_item(size_t index) const;

private:
	const wchar_t** items_;
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class Choice : public PressibleWidget
{
public:
	Choice(const wchar_t *caption, const IStringItemsProvider &items_provider, int selection) :
		caption_(caption),
		items_provider_(items_provider),
		selection_(selection) {}

	int get_selection() const { return selection_; }
	void set_selection(int selection);

	void paint(WidgetsForm &form, FormPaintData &paint_data, const WidgetPaintData &widget_pd) override;
	Color get_default_color(const FormColors &colors) const override;

protected:
	void pressed(Form *form);

private:
	const wchar_t *caption_;
	const IStringItemsProvider &items_provider_;
	int selection_;
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class Form
{
public:
	Form(const wchar_t *caption, const FontInfo &font, const FormColors *colors = NULL);

	void show();

	const FontInfo& get_font() const { return font_; }

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

	void paint(bool widgets_only, bool force_repaint_all_widgets);

protected:
	virtual void paint_client_area(FormPaintData &paint_data, const Rect &client_rect, bool force_repaint_all_widgets) = 0;

	virtual void touch_screen_event(FormTouchScreenEventData &event_data) {}
	virtual void before_show() {}

	int16_t get_caption_height() const;

	void get_form_rects(Rect *caption_rect, Rect *client_rect) const;

	const FormColors *colors_;
	Flags<uint32_t> flags_;

private:
	const wchar_t *caption_;
	const FontInfo &font_;
	volatile ModalResult modal_result_;
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class IWidgetVisitor
{
public:
	virtual void visit(Widget &widget, uint8_t x, uint8_t y, uint8_t width, uint8_t height, const wchar_t *text = NULL) = 0;
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class WidgetsForm : public Form
{
public:
	WidgetsForm(const wchar_t *caption, const FontInfo &font) :
		Form(caption, font),
		last_pressed_widget_(NULL) {}

	void handle_touch_screen_event(FormTouchScreenEventData &event_data) override;

protected:
	virtual void visit_all_widgets(IWidgetVisitor &visitor) = 0;
	virtual void widget_event(EventType type, const Widget *widget) {}
	virtual void get_widget_color(const Widget *widget, Color &color) {}

	void paint_client_area(FormPaintData &paint_data, const Rect &client_rect, bool force_repaint_all_widgets) override;

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
	StringSelectorForm(const wchar_t *caption, const FontInfo &font, const IStringItemsProvider *items_provider, int selection) :
		Form(caption, font),
		items_provider_(items_provider),
		selection_(selection),
		top_item_index_(0),
		scroll_drag_start_y_(-1) {}

	void handle_touch_screen_event(FormTouchScreenEventData &event_data) override;

	int get_selection() const { return selection_; }

protected:
	void paint_client_area(FormPaintData &paint_data, const Rect &client_rect, bool force_repaint_all_widgets) override;
	void before_show() override;

	int16_t paint_item(int item_index, FormPaintData &paint_data, const Rect &client_rect, int items_count, uint16_t item_height, uint16_t scr_bar_width);

private:
	const IStringItemsProvider *items_provider_;
	int selection_;
	int top_item_index_;
	int16_t scroll_drag_start_y_;
	int16_t scroll_drag_start_top_item_index_;

	static const uint32_t FLAG_WAS_SCROLLED = 0x10000;

	void get_form_data(StringSelectorFormData &data);
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class DialogForm : public WidgetsForm
{
public:
	DialogForm(const wchar_t *caption, const FontInfo &font) : WidgetsForm(caption, font) {}

protected:
	void visit_all_widgets(IWidgetVisitor &visitor) override;
	void widget_event(muil::EventType type, const muil::Widget *widget) override;
	void get_widget_color(const muil::Widget *widget, muil::Color &color) override;

private:
	Button btn_ok_;
	Button btn_cancel_;
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
