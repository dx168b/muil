#ifndef MUIL_WIDGETS_HPP_FILE_INCLUDED_
#define MUIL_WIDGETS_HPP_FILE_INCLUDED_

#include "muil_classes.hpp"

namespace muil {

struct MonochromeBitmap
{
	uint8_t width;
	uint8_t height;
	uint8_t data[];
};

void paint_bitmapped_widget(
	const Rect &rect,
	Color color,
	const MonochromeBitmap &bmp_lt,
	const MonochromeBitmap &bmp_t,
	const MonochromeBitmap &bmp_rt,
	const MonochromeBitmap &bmp_l,
	const MonochromeBitmap &bmp_c,
	const MonochromeBitmap &bmp_r,
	const MonochromeBitmap &bmp_lb,
	const MonochromeBitmap &bmp_b,
	const MonochromeBitmap &bmp_rb
);


enum ButtonStyle
{
	BS_NORMAL,
	BS_PRESSED,
	BS_DISABLED
};

void draw_button(const Rect &rect, Color color, ButtonStyle style);

} // namespace muil

#endif
