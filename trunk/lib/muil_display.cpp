#include <stddef.h>
#include "muil_display.hpp"
#include "muil_utils.hpp"

namespace muil {

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

void display_paint_text(int x, int y, const wchar_t *text, const FontInfo *font, const Color &color)
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
			color
		);
		x += char_info->widthBits + font->spacePixels;
	}
}

void display_paint_text_in_rect(const Rect &rect, HorizAlign align, const wchar_t *text, const FontInfo *font, const Color &color)
{
	const Size display_size = display_get_size();
	if (rect.intersects(Rect(Point(0, 0), display_size))) return;

	const Size text_size = display_get_text_size(font, text);

	int16_t x;
	int16_t y = (rect.y1 + rect.y2 - text_size.height) / 2;
	int16_t left_right_layer = text_size.height/4;
	switch (align)
	{
	case HA_LEFT:
		x = rect.x1 + left_right_layer;
		break;

	case HA_CENTER:
		x = (rect.x1 + rect.x2 - text_size.width) / 2;
		break;

	case HA_RIGHT:
		x = rect.x2 - text_size.width - left_right_layer;
		break;

	default:
		return;
	}

	display_paint_text(x, y, text, font, color);
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

	int16_t scr_width = display_get_size().width;

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

void default_display_paint_character(int x0, int y0, const uint8_t *data, uint8_t width, uint8_t height, const Color &color)
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
				if (value & 0x80) display_set_point(x, y, color);
				x++;
				value <<= 1;
			}
		}
	}
}


} // namespace muil
