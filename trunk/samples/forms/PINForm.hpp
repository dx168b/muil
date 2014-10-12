#ifndef PIN_FORM_HPP_FILE_INCLUDED_
#define PIN_FORM_HPP_FILE_INCLUDED_

#include "muil.hpp"

class PINForm : public muil::WidgetsForm
{
public:
	PINForm(const wchar_t *caption, const muil::FontInfo &font, wchar_t *pin_text_buffer, uint8_t pin_len);

protected:
	void visit_all_widgets(muil::IWidgetVisitor &visitor) override;
	void widget_event(muil::EventType type, const muil::Widget *widget) override;
	void get_widget_color(const muil::Widget *widget, muil::Color &color) override;

private:
	muil::TextIndicator pin_;
	muil::Button btn_clr_;
	muil::Button btn1_;
	muil::Button btn2_;
	muil::Button btn3_;
	muil::Button btn4_;
	muil::Button btn5_;
	muil::Button btn6_;
	muil::Button btn7_;
	muil::Button btn8_;
	muil::Button btn9_;
	muil::Button btn_esc_;
	muil::Button btn0_;
	muil::Button btn_ent_;
	uint8_t pin_len_;
	wchar_t *pin_text_buffer_;

	char get_char_by_btn(const muil::Widget &widget) const;
	uint8_t get_text_len() const;
};

#endif
