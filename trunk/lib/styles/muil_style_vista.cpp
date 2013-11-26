#include <stddef>
#include "muil_style_vista.hpp"

namespace muil {


static const uint8_t vista_normal_btn_lt[] = {
	0xff, 0x90, 0x77,
	0x88, 0x91, 0xe8,
	0x75, 0xea, 0xfa
};

static const uint8_t vista_normal_btn_t[] = {
	0x70,
	0xfc,
	0xf2
};

static const uint8_t vista_normal_btn_rt[] = {
	0x77, 0x90, 0xff,
	0xe8, 0x91, 0x88,
	0xfa, 0xea, 0x75
};


static const uint8_t vista_normal_btn_l[] = {
	0x70, 0xfb, 0xf1,
	0x70, 0xfb, 0xef,
	0x70, 0xfa, 0xed,
	0x70, 0xfa, 0xec,
	0x70, 0xf9, 0xe9,
	0x70, 0xf5, 0xdb,
	0x70, 0xf5, 0xd8,
	0x70, 0xf5, 0xd7,
	0x70, 0xf4, 0xd4,
	0x70, 0xf3, 0xd1
};

static const uint8_t vista_normal_btn_c[] = {
	0xf2,
	0xf0,
	0xed,
	0xed,
	0xeb,
	0xdb,
	0xd8,
	0xd8,
	0xd4,
	0xd1
};

static const uint8_t vista_normal_btn_r[] = {
	0xf1, 0xfb, 0x70,
	0xef, 0xfb, 0x70,
	0xed, 0xfa, 0x70,
	0xec, 0xfa, 0x70,
	0xe9, 0xf9, 0x70,
	0xdb, 0xf5, 0x70,
	0xd8, 0xf5, 0x70,
	0xd7, 0xf5, 0x70,
	0xd4, 0xf4, 0x70,
	0xd1, 0xf3, 0x70
};

static const uint8_t vista_normal_btn_lb[] = {
	0x75, 0xe2, 0xec,
	0x88, 0x8f, 0xe4,
	0xff, 0x86, 0x75
};

static const uint8_t vista_normal_btn_b[] = {
	0xcf,
	0xf3,
	0x70
};

static const uint8_t vista_normal_btn_rb[] = {
	0xec, 0xe2, 0x75,
	0xe4, 0x8f, 0x88,
	0x75, 0x86, 0xff
};

void vista_draw_button(const Rect &rect, Color color, ButtonStyle style, bool composite)
{
	if (composite) return;
	switch (style)
	{
		case BS_NORMAL:
			paint_bitmapped_widget(
				rect, color,
				vista_normal_btn_lt, vista_normal_btn_t, vista_normal_btn_rt,
				vista_normal_btn_l, vista_normal_btn_c, vista_normal_btn_r,
				vista_normal_btn_lb, vista_normal_btn_b, vista_normal_btn_rb,
				3, 1, 3,
				3, 10, 3
			);
			break;
	}
}

void vista_draw_choice_rect(const Rect &rect, Color color, ButtonStyle style)
{
	vista_draw_button(rect, color, style, false);
}


static const uint8_t vista_normal_chk_lt[] = {
	0x8e, 0x8e, 0x8e,
	0x8e, 0xf4, 0xf4,
	0x8e, 0xf4, 0xb2
};

static const uint8_t vista_normal_chk_t[] = {
	0x8e, 0x8e, 0x8e,
	0xf4, 0xf4, 0xf4,
	0xb2, 0xb2, 0xb3
};

static const uint8_t vista_normal_chk_rt[] = {
	0x8e, 0x8e, 0x8e,
	0xf4, 0xf4, 0x8e,
	0xbd, 0xf4, 0x8e
};

static const uint8_t vista_normal_chk_l[] = {
	0x8e, 0xf4, 0xb2,
	0x8e, 0xf4, 0xb4,
	0x8e, 0xf4, 0xbc
};

static const uint8_t vista_normal_chk_c[] = {
	0xce, 0xd0, 0xd8,
	0xd5, 0xde, 0xe8,
	0xe3, 0xee, 0xf3
};

static const uint8_t vista_normal_chk_r[] = {
	0xc4, 0xf4, 0x8e,
	0xd0, 0xf4, 0x8e,
	0xdf, 0xf4, 0x8e
};

static const uint8_t vista_normal_chk_lb[] = {
	0x8e, 0xf4, 0xca,
	0x8e, 0xf4, 0xf4,
	0x8e, 0x8e, 0x8e
};

static const uint8_t vista_normal_chk_b[] = {
	0xd8, 0xe3, 0xeb,
	0xf4, 0xf4, 0xf4,
	0x8e, 0x8e, 0x8e
};

static const uint8_t vista_normal_chk_rb[] = {
	0xe9, 0xf4, 0x8e,
	0xf4, 0xf4, 0x8e,
	0x8e, 0x8e, 0x8e
};

void vista_draw_checkbox_rect(const Rect &rect, Color color, ButtonStyle style)
{
	switch (style)
	{
		case BS_NORMAL:
			paint_bitmapped_widget(
				rect, color,
				vista_normal_chk_lt, vista_normal_chk_t, vista_normal_chk_rt,
				vista_normal_chk_l, vista_normal_chk_c, vista_normal_chk_r,
				vista_normal_chk_lb, vista_normal_chk_b, vista_normal_chk_rb,
				3, 3, 3,
				3, 3, 3
			);
			break;
	}
}

static const uint8_t vista_normal_indented_lt[] = {
	0xd6, 0xbd,
	0xe3, 0xeb
};

static const uint8_t vista_normal_indented_t[] = {
	0xad,
	0xfe
};

static const uint8_t vista_normal_indented_rt[] = {
	0xbd, 0xd6,
	0xeb, 0xde
};

static const uint8_t vista_normal_indented_l[] = {
	0xe3, 0xfe
};

static const uint8_t vista_normal_indented_c[] = {
	0xfe
};

static const uint8_t vista_normal_indented_r[] = {
	0xff, 0xde
};

static const uint8_t vista_normal_indented_lb[] = {
	0xe3, 0xeb,
	0xeb, 0xe7
};

static const uint8_t vista_normal_indented_b[] = {
	0xfe,
	0xe7
};

static const uint8_t vista_normal_indented_rb[] = {
	0xeb, 0xde,
	0xe7, 0xeb
};

void vista_draw_indented_ctrl_rect(const Rect &rect, Color color, ButtonStyle style)
{
	paint_bitmapped_widget(
		rect, color,
		vista_normal_indented_lt, vista_normal_indented_t, vista_normal_indented_rt,
		vista_normal_indented_l,  vista_normal_indented_c, vista_normal_indented_r,
		vista_normal_indented_lb, vista_normal_indented_b, vista_normal_indented_rb,
		2, 1, 2,
		2, 1, 2
	);

}

int vista_get_indented_ctrl_border()
{
	return 2;
}


} // namespace muil