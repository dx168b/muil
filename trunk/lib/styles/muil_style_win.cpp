#include "muil_style_win.hpp"

namespace muil {

// Normal button bitmaps
static const uint8_t normal_btn_lt[] = {
	0x00, 0x00, 0x00,
	0x00, 0xFF, 0xFF,
	0x00, 0xFF, 0xC0
};

static const uint8_t normal_btn_t[] = {
	0x00, 0xFF, 0xC0
};

static const uint8_t normal_btn_rt[] = {
	0x00, 0x00, 0x00,
	0xFF, 0x40, 0x00,
	0x80, 0x40, 0x00
};

static const uint8_t normal_btn_l[] = {
	0x00, 0xFF, 0xC0
};

static const uint8_t normal_btn_c[] = {
	0xC0
};

static const uint8_t normal_btn_r[] = {
	0x80, 0x40, 0x00
};

static const uint8_t normal_btn_lb[] = {
	0x00, 0xFF, 0x80,
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


void win_draw_button(const Rect &rect, Color color, ButtonStyle style)
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

} // namespace muil

