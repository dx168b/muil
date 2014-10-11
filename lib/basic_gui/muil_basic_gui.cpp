#include <stddef.h>
#include "muil_basic_gui.hpp"

namespace muil {

template <typename T> void swap(T &v1, T &v2)
{
	T tmp(v1);
	v1 = v2;
	v2 = tmp;
}

template <typename T> T abs(const T v)
{
	return (v >= 0) ? v : -v;
}

template <typename T> T max(const T v1, const T v2)
{
	return v1 > v2 ? v1 : v2;
}

template <typename T> T min(const T v1, const T v2)
{
	return v1 < v2 ? v1 : v2;
}

static uint8_t correct_color_value(int value)
{
	if (value < 0) return 0;
	if (value > 255) return 255;
	return value & 0xFF;
}

Color Color::light(int value) const
{
	return Color(
		correct_color_value(r + value),
		correct_color_value(g + value),
		correct_color_value(b + value)
	);
}

Color Color::adjusted(int multiplier, int divider)
{
	return Color(
		correct_color_value((r * multiplier) / divider),
		correct_color_value((g * multiplier) / divider),
		correct_color_value((b * multiplier) / divider)
	);
}

Color Color::between(const Color &color1, const Color &color2, int16_t value, int16_t max)
{
	return Color(
		correct_color_value(value*(color2.r-color1.r)/max + color1.r),
		correct_color_value(value*(color2.g-color1.g)/max + color1.g),
		correct_color_value(value*(color2.b-color1.b)/max + color1.b)
	);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Rect Rect::inflated(int value) const
{
	return Rect(x1-value, y1-value, x2+value, y2+value);
}

bool Rect::contains(const Point pt) const
{
	return (x1 <= pt.x) && (pt.x <= x2) && (y1 <= pt.y) && (pt.y <= y2);
}

static bool interv_intersects(int16_t a1, int16_t a2, int16_t b1, int16_t b2)
{
	return !(a2 < b1) && !(a1 < b2);
}

bool Rect::intersects(const Rect &other_rect) const
{
	return
		interv_intersects(x1, x2, other_rect.x1, other_rect.x2) &&
		interv_intersects(y1, y2, other_rect.y1, other_rect.y2);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

const FontCharInfo* display_find_char_info(const FontInfo *font, wchar_t chr)
{
	for (size_t i = 0; font->charInfo[i].character; i++)
		if (chr == font->charInfo[i].character) return &font->charInfo[i];
	return NULL;
}

void display_draw_rect(const Rect &rect, int16_t width, const Color &color)
{
	width--;
	display_fill_rect(rect.x1, rect.y1+width, rect.x1+width, rect.y2-width, color);
	display_fill_rect(rect.x2-width, rect.y1+width, rect.x2, rect.y2-width, color);
	display_fill_rect(rect.x1+width, rect.y1, rect.x2-width, rect.y1+width, color);
	display_fill_rect(rect.x1+width, rect.y2-width, rect.x2-width, rect.y2, color);
}

void display_draw_vertical_gradient(const Rect &rect, const Color &color1, const Color &color2)
{
	const uint16_t height = rect.y2 - rect.y1;
	for (int16_t y = rect.y1, i = 0; y <= rect.y2; y++, i++)
		display_fill_rect(rect.x1, y, rect.x2, y, Color::between(color1, color2, i, height));
}

void display_draw_horizontal_gradient(const Rect &rect, const Color &color1, const Color &color2)
{
	const uint16_t width = rect.x2 - rect.x1;
	for (int x = rect.x1, i = 0; x <= rect.x2; x++, i++)
		display_fill_rect(x, rect.y1, x, rect.y2, Color::between(color1, color2, i, width));
}

void display_paint_text(int x, int y, const wchar_t *text, const FontInfo *font, const Color &color, const Color *bg_color)
{
	for (size_t i = 0; text[i]; i++)
	{
		if (text[i] == ' ')
		{
			x += 2*font->spacePixels;
			continue;
		}
		const FontCharInfo* char_info = display_find_char_info(font, text[i]);
		if (char_info == NULL) char_info = display_find_char_info(font, '?');
		display_paint_character(
			x, y,
			&font->data[char_info->offset],
			char_info->widthBits,
			font->heightPages,
			color,
			bg_color
		);
		x += char_info->widthBits + font->spacePixels;
	}
}

void display_paint_text_in_rect(const Rect &rect, HorizAlign align, const wchar_t *text, const FontInfo *font, const Color &color, const Color *bg_color)
{
	const Size text_size = display_get_text_size(font, text);

	int16_t x;
	int16_t y = (rect.y1 + rect.y2 - text_size.height) / 2;
	int16_t left_right_layer = text_size.height/4;
	switch (align)
	{
	case HorizAlign::Left:
		x = rect.x1 + left_right_layer;
		break;

	case HorizAlign::Center:
		x = (rect.x1 + rect.x2 - text_size.width) / 2;
		break;

	case HorizAlign::Right:
		x = rect.x2 - text_size.width - left_right_layer;
		break;

	default:
		return;
	}

	display_paint_text(x, y, text, font, color, bg_color);
}

Size display_get_text_size(const FontInfo *font, const wchar_t *text)
{
	int width = 0;
	for (size_t i = 0; text[i]; i++)
	{
		if (text[i] == ' ')
		{
			width += 2*font->spacePixels;
			continue;
		}
		const FontCharInfo* char_info = display_find_char_info(font, text[i]);
		if (char_info == NULL) char_info = display_find_char_info(font, '?');
		width += char_info->widthBits;
		if (i != 0) width += font->spacePixels;
	}
	return Size(width, font->heightPages);
}

static void paint_charaster_and_move_caret(wchar_t chr, int &x, int y, const FontInfo *font, const Color &color, const Color *bg_color)
{
	if (chr == ' ')
	{
		x += 2*font->spacePixels;
		return;
	}
	const FontCharInfo* char_info = display_find_char_info(font, chr);
	if (char_info == NULL) char_info = display_find_char_info(font, '?');
	display_paint_character(
		x, y,
		&font->data[char_info->offset],
		char_info->widthBits,
		font->heightPages,
		color,
		bg_color
	);

	x += char_info->widthBits;

	if (bg_color)
		display_fill_rect(x, y, x + font->spacePixels-1, y + font->heightPages - 1, *bg_color);

	x += font->spacePixels;
}


template <class Fun>
void print_integer10(int32_t value, int pt_pos, const Fun &out_char_fun)
{
	if (value < 0)
	{
		value = -value;
		out_char_fun('-');
	}

	int comp_value = 1000000000;

	if (pt_pos != -1) pt_pos = 9-pt_pos;

	bool begin = true;
	do
	{
		wchar_t chr = '0';
		while (value >= comp_value)
		{
			value -= comp_value;
			chr++;
		}
		if (chr != '0') begin = false;
		if (!begin || (comp_value == 1))
			out_char_fun(chr);
		comp_value /= 10;
		if ((pt_pos != -1) && (pt_pos-- == 0))
		{
			out_char_fun('.');
			begin = false;
		}
	}
	while (comp_value != 0);
}

void display_paint_integer(int x, int y, int value, int pt_pos, const FontInfo *font, const Color &color, const Color *bg_color)
{
	auto paint_char_fun = [&] (wchar_t chr) {
		paint_charaster_and_move_caret(chr, x, y, font, color, bg_color);
	};

	print_integer10(value, pt_pos, paint_char_fun);
}

template <class Fun>
void print_integer16(uint32_t value, const Fun &out_char_fun)
{
	bool begin = true;
	for (unsigned i = 0; i < 8; i++)
	{
		uint32_t val = (value & 0xF0000000) >> 28;
		wchar_t chr = (val < 10) ? (val + '0') : (val - 10 + 'A');
		if ((val != 0) || (i == 7)) begin = false;
		if (!begin) out_char_fun(chr);
		value <<= 4;
	}
}

void display_paint_integer16(int x, int y, uint32_t value, const FontInfo *font, const Color &color, const Color *bg_color)
{
	auto paint_char_fun = [&] (wchar_t chr) {
		paint_charaster_and_move_caret(chr, x, y, font, color, bg_color);
	};

	print_integer16(value, paint_char_fun);
}

void display_fill_rect(int x1, int y1, int x2, int y2, const Color &color)
{
	display_fill_rect(Rect(x1, y1, x2, y2), color);
}

void display_fill_triangle(int x1, int y1, int x2, int y2, int x3, int y3, const Color &color)
{
	if (x1 > x2) { swap(x1, x2); swap(y1, y2); }
	if (x2 > x3) { swap(x2, x3); swap(y2, y3); }
	if (x1 > x3) { swap(x1, x3); swap(y1, y3); }

	uint16_t dx1 = x2 - x1;
	int16_t dy1 = y2 - y1;
	uint16_t dy1_abs = dx1 ? (abs(dy1) % dx1) : 0;
	int16_t step1 = y2 > y1 ? 1 : -1;
	int16_t int1 = dx1 ? (dy1 / dx1) : 0;
	int16_t cnt_y1 = dx1 / 2;
	int16_t yy1 = y1;

	uint16_t dx2 = x3 - x1;
	int16_t dy2 = y3 - y1;
	uint16_t dy2_abs = dx2 ? (abs(dy2) % dx2) : 0;
	int16_t step2 = y3 > y1 ? 1 : -1;
	int16_t int2 = dx2 ? (dy2 / dx2) : 0;
	int16_t cnt_y2 = dx2 / 2;
	int16_t yy2 = y1;

	int16_t scr_width = display_get_width();

	for (int16_t x = x1; x <= x3; x++)
	{
		if (x == x2)
		{
			dx1 = x3 - x2;
			if (dx1 == 0) break;
			dy1 = y3 - y2;
			dy1_abs = abs(dy1) % dx1;
			step1 = y3 > y2 ? 1 : -1;
			int1 = dy1 / dx1;
			cnt_y1 = dx1 / 2;
			yy1 = y2;
		}

		if ((x >= 0) || (x < scr_width))
			display_fill_rect(x, min(yy1, yy2), x, max(yy1, yy2), color);

		cnt_y1 -= dy1_abs;
		if (cnt_y1 < 0)
		{
			cnt_y1 += dx1;
			yy1 += step1;
		}
		yy1 += int1;

		cnt_y2 -= dy2_abs;
		if (cnt_y2 < 0)
		{
			cnt_y2 += dx2;
			yy2 += step2;
		}
		yy2 += int2;
	}
}

void default_display_paint_character(int x0, int y0, const uint8_t *data, uint8_t width, uint8_t height, const Color &color, const Color *bg_color)
{
	const uint16_t w8 = (width + 7) / 8;
	int16_t y = y0;
	for (; height != 0; height--, y++)
	{
		int32_t x = x0;
		for (uint16_t i = 0; i < w8; i++)
		{
			uint8_t value = *data++;
			for (uint8_t j = 0; j < 8; j++)
			{
				if (value & 0x80) 
					display_set_point(x, y, color);
				else if (bg_color) 
					display_set_point(x, y, *bg_color);
				x++;
				value <<= 1;
			}
		}
	}
}


} // namespace muil
