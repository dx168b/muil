#include "PINForm.hpp"

PINForm::PINForm(const wchar_t *caption, const muil::FontInfo &font, wchar_t *pin_text_buffer, uint8_t pin_len) :
	muil::WidgetsForm(caption, font),
	pin_    (pin_text_buffer),
	pin_len_(pin_len),
	pin_text_buffer_(pin_text_buffer)
{
	pin_text_buffer_[0] = 0;
}

void PINForm::visit_all_widgets(muil::IWidgetVisitor &visitor)
{
	const int btw = 27;
	const int bth = 15;
	const int x1 = 5;
	const int x2 = 37;
	const int x3 = 69;

	const int y0 = 5;
	const int y1 = 25;
	const int y2 = 43;
	const int y3 = 61;
	const int y4 = 79;

	visitor.visit(pin_,      x1,  y0, 50,   15);
	visitor.visit(btn_clr_,  60,  y0, 35,   15,  L"Clear");
	visitor.visit(btn1_,     x1,  y1, btw,  bth, L"1");
	visitor.visit(btn2_,     x2,  y1, btw,  bth, L"2");
	visitor.visit(btn3_,     x3,  y1, btw,  bth, L"3");
	visitor.visit(btn4_,     x1,  y2, btw,  bth, L"4");
	visitor.visit(btn5_,     x2,  y2, btw,  bth, L"5");
	visitor.visit(btn6_,     x3,  y2, btw,  bth, L"6");
	visitor.visit(btn7_,     x1,  y3, btw,  bth, L"7");
	visitor.visit(btn8_,     x2,  y3, btw,  bth, L"8");
	visitor.visit(btn9_,     x3,  y3, btw,  bth, L"9");
	visitor.visit(btn_esc_,  x1,  y4, btw,  bth, L"Esc");
	visitor.visit(btn0_,     x2,  y4, btw,  bth, L"0");
	visitor.visit(btn_ent_,  x3,  y4, btw,  bth, L"OK");
}

void PINForm::widget_event(muil::EventType type, const muil::Widget *widget)
{
	if (type == muil::EventType::TouchscreenUp)
	{
		if (widget == &btn_ent_)
			set_modal_result(muil::ModalResult::Ok);

		else if (widget == &btn_esc_)
			set_modal_result(muil::ModalResult::Cancel);

		else if (widget == &btn_clr_)
		{
			pin_text_buffer_[0] = 0;
			pin_.refresh();
		}

		else
		{
			char chr = get_char_by_btn(*widget);
			if (chr == 0) return;
			uint8_t text_len = get_text_len();
			if (text_len == pin_len_) return;
			pin_text_buffer_[text_len] = chr;
			pin_text_buffer_[text_len+1] = 0;
			pin_.refresh();
		}
	}
}

void PINForm::get_widget_color(const muil::Widget *widget, muil::Color &color)
{
	if (widget == &btn_ent_) color = muil::Color(180, 210, 180);
	else if (widget == &btn_esc_) color = muil::Color(210, 180, 180);
	else if (widget == &btn_clr_) color = muil::Color(180, 180, 180);
}

char PINForm::get_char_by_btn(const muil::Widget &widget) const
{
	if      (&widget == &btn1_) return '1';
	else if (&widget == &btn2_) return '2';
	else if (&widget == &btn3_) return '3';
	else if (&widget == &btn4_) return '4';
	else if (&widget == &btn5_) return '5';
	else if (&widget == &btn6_) return '6';
	else if (&widget == &btn7_) return '7';
	else if (&widget == &btn8_) return '8';
	else if (&widget == &btn9_) return '9';
	else if (&widget == &btn0_) return '0';
	else return 0;
}

uint8_t PINForm::get_text_len() const
{
	uint8_t text_len = 0;
	while (pin_text_buffer_[text_len] && (text_len < pin_len_)) text_len++;
	return text_len;
}

