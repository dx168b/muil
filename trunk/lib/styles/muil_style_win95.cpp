#include "muil_style_win95.hpp"

namespace muil {

int win95_get_indented_ctrl_border()
{
	return 2;
}

// Normal button bitmaps
static const uint8_t normal_btn_lt[] = {
	0x00, 0x00, 0x00,
	0x00, 0xFE, 0xFE,
	0x00, 0xFE, 0xC0
};

static const uint8_t normal_btn_t[] = {
	0x00, 0xFE, 0xC0
};

static const uint8_t normal_btn_rt[] = {
	0x00, 0x00, 0x00,
	0xFE, 0x40, 0x00,
	0x80, 0x40, 0x00
};

static const uint8_t normal_btn_l[] = {
	0x00, 0xFE, 0xC0
};

static const uint8_t normal_btn_c[] = {
	0xC0
};

static const uint8_t normal_btn_r[] = {
	0x80, 0x40, 0x00
};

static const uint8_t normal_btn_lb[] = {
	0x00, 0xFE, 0x80,
	0x00, 0x40, 0x40,
	0x00, 0x00, 0x00
};

static const uint8_t normal_btn_b[] = {
	0x80, 0x40, 0x00
};

static const uint8_t normal_btn_rb[] = {
	0x80, 0x40, 0x00,
	0x40, 0x40, 0x00,
	0x00, 0x00, 0x00
};

// Pressed button bitmaps

static const uint8_t pressed_btn_lt[] = {
	0x00, 0x00,
	0x00, 0x40,
};

static const uint8_t pressed_btn_t[] = {
	0x00,
	0x40
};

static const uint8_t pressed_btn_rt[] = {
	0x00, 0x00,
	0x40, 0x00,
};

static const uint8_t pressed_btn_l[] = {
	0x40, 0x00
};

static const uint8_t pressed_btn_r[] = {
	0x00, 0x40
};

static const uint8_t pressed_btn_lb[] = {
	0x00, 0x40,
	0x00, 0x00,
};

static const uint8_t pressed_btn_b[] = {
	0x40,
	0x00
};

static const uint8_t pressed_btn_rb[] = {
	0x40, 0x00,
	0x00, 0x00,
};


void win95_draw_button(const Rect &rect, Color color, ButtonStyle style)
{
	switch (style)
	{
		case BS_NORMAL:
			paint_bitmapped_widget(
				rect, color,
				normal_btn_lt, normal_btn_t, normal_btn_rt,
				normal_btn_l,  normal_btn_c, normal_btn_r,
				normal_btn_lb, normal_btn_b, normal_btn_rb,
				3, 1, 3,
				3, 1, 3
			);
			break;

		case BS_PRESSED:
			paint_bitmapped_widget(
				rect, color,
				pressed_btn_lt, pressed_btn_t, pressed_btn_rt,
				pressed_btn_l,  normal_btn_c, pressed_btn_r,
				pressed_btn_lb, pressed_btn_b, pressed_btn_rb,
				2, 1, 2,
				2, 1, 2
			);
			break;
	}
}

static const uint8_t normal_check_lt[] = {
	0xA0, 0xA0,
	0xA0, 0x80
};

static const uint8_t normal_check_t[] = {
	0xA0,
	0x80
};

static const uint8_t normal_check_rt[] = {
	0xA0, 0xFE,
	0xC0, 0xFE
};

static const uint8_t normal_check_l[] = {
	0xA0, 0x80
};

static const uint8_t normal_check_c[] = {
	0xFE,
};

static const uint8_t preessed_checl_c[] = {
	0xC0,
};

static const uint8_t normal_check_r[] = {
	0xC0, 0xFE
};

static const uint8_t normal_check_lb[] = {
	0xA0, 0xC0,
	0xFE, 0xFE
};

static const uint8_t normal_check_b[] = {
	0xC0,
	0xFE
};

static const uint8_t normal_check_rb[] = {
	0xC0, 0xFE,
	0xFE, 0xFE
};

void win95_draw_checkbox_rect(const Rect &rect, Color color, ButtonStyle style)
{
	win95_draw_indented_ctrl_rect(rect, color, style);
}

void win95_draw_choice_rect(const Rect &rect, Color color, ButtonStyle style)
{
	win95_draw_indented_ctrl_rect(rect, color, style);
}

void win95_draw_indented_ctrl_rect(const Rect &rect, Color color, ButtonStyle style)
{
	switch (style)
	{
		case BS_NORMAL:
			paint_bitmapped_widget(
				rect, color,
				normal_check_lt, normal_check_t, normal_check_rt,
				normal_check_l,  normal_check_c, normal_check_r,
				normal_check_lb, normal_check_b, normal_check_rb,
				2, 1, 2,
				2, 1, 2
			);
			break;

		case BS_PRESSED:
		case BS_DISABLED:
			paint_bitmapped_widget(
				rect, color,
				normal_check_lt, normal_check_t,   normal_check_rt,
				normal_check_l,  preessed_checl_c, normal_check_r,
				normal_check_lb, normal_check_b,   normal_check_rb,
				2, 1, 2,
				2, 1, 2
			);
			break;
	}
}

} // namespace muil

