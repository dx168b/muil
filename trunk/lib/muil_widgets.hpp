#ifndef MUIL_WIDGETS_HPP_FILE_INCLUDED_
#define MUIL_WIDGETS_HPP_FILE_INCLUDED_

#include "muil_classes.hpp"

namespace muil {

struct MonochromeBitmap
{
	uint8_t width;
	uint8_t height;
	const uint8_t *data;
};

void paint_bitmapped_widget(
	const Rect &rect,
	Color color,
	const uint8_t *bmp_lt,
	const uint8_t *bmp_t,
	const uint8_t *bmp_rt,
	const uint8_t *bmp_l,
	const uint8_t *bmp_c,
	const uint8_t *bmp_r,
	const uint8_t *bmp_lb,
	const uint8_t *bmp_b,
	const uint8_t *bmp_rb,
	int width_l,
	int width_c,
	int width_r,
	int height_t,
	int height_c,
	int height_b
);


enum ButtonStyle
{
	BS_NORMAL,
	BS_PRESSED,
	BS_DISABLED
};

void draw_button(const Rect &rect, Color color, ButtonStyle style);
void draw_checkbox_rect(const Rect &rect, Color color, ButtonStyle style);
void draw_indented_ctrl_rect(const Rect &rect, Color color, ButtonStyle style);
int get_indented_ctrl_border();

} // namespace muil

#endif
