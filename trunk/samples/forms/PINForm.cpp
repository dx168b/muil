#include "PINForm.hpp"

PINForm::PINForm(const muil::FontInfo *font, wchar_t *pin_text_buffer, uint8_t pin_len) :
	muil::WidgetsForm(L"Enter the PIN", font),
	pin_    (pin_text_buffer),
	btn_clr_(L"Clear", false),
	btn1_   (L"1",   false),
	btn2_   (L"2",   false),
	btn3_   (L"3",   false),
	btn4_   (L"4",   false),
	btn5_   (L"5",   false),
	btn6_   (L"6",   false),
	btn7_   (L"7",   false),
	btn8_   (L"8",   false),
	btn9_   (L"9",   false),
	btn_esc_(L"Esc", false),
	btn0_   (L"0",   false),
	btn_ent_(L"Ent", false),
	pin_text_buffer_(pin_text_buffer),
	pin_len_(pin_len)
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
	visitor.visit(btn_clr_,  60,  y0, 35,   15);
	visitor.visit(btn1_,     x1,  y1, btw,  bth);
	visitor.visit(btn2_,     x2,  y1, btw,  bth);
	visitor.visit(btn3_,     x3,  y1, btw,  bth);
	visitor.visit(btn4_,     x1,  y2, btw,  bth);
	visitor.visit(btn5_,     x2,  y2, btw,  bth);
	visitor.visit(btn6_,     x3,  y2, btw,  bth);
	visitor.visit(btn7_,     x1,  y3, btw,  bth);
	visitor.visit(btn8_,     x2,  y3, btw,  bth);
	visitor.visit(btn9_,     x3,  y3, btw,  bth);
	visitor.visit(btn_esc_,  x1,  y4, btw,  bth);
	visitor.visit(btn0_,     x2,  y4, btw,  bth);
	visitor.visit(btn_ent_,  x3,  y4, btw,  bth);
}

void PINForm::widget_event(muil::EventType type, const muil::Widget *widget)
{
	if (type == muil::EVENT_TOUCHSCREEN_UP)
	{
		if (widget == &btn_ent_)
			set_modal_result(muil::MR_OK);

		else if (widget == &btn_esc_)
			set_modal_result(muil::MR_CANCEL);

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
	if (widget == &btn_ent_) color = muil::Color(160, 255, 160);
	else if (widget == &btn_esc_) color = muil::Color(255, 160, 160);
	else if (widget == &btn_clr_) color = muil::Color(200, 200, 200);
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

