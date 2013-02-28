/*=============================================================================

  Copyright (C) 2012-2013 Denis Artyomov (denis.artyomov@gmail.com)

  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files (the "Software"), to deal
  in the Software without restriction, including without limitation the rights
  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
  copies of the Software, and to permit persons to whom the Software is
  furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in
  all copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
  SOFTWARE.

===============================================================================
  File:      muil_classes.cpp
  Author(s): Denis Artyomov
=============================================================================*/

#include <stddef.h>

#include "muil_classes.hpp"
#include "muil_utils.hpp"

namespace muil {

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

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

Color Color::between(const Color &color1, const Color &color2, int16_t value, int16_t max)
{
	return Color(
		correct_color_value(value*(color2.r-color1.r)/max + color1.r),
		correct_color_value(value*(color2.g-color1.g)/max + color1.g),
		correct_color_value(value*(color2.b-color1.b)/max + color1.b)
	);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Rect Rect::inflated(int16_t value) const
{
	return Rect(x1-value, y1-value, x2+value, y2+value);
}

Rect Rect::moved(const Point offset) const
{
	return Rect(x1+offset.x, y1+offset.y, x2+offset.x, y2+offset.y);
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

void Display::draw_rect(const Rect &rect, int16_t width, const Color &color)
{
	width--;
	fill_rect(Rect(rect.x1, rect.y1+width, rect.x1+width, rect.y2-width), color);
	fill_rect(Rect(rect.x2-width, rect.y1+width, rect.x2, rect.y2-width), color);
	fill_rect(Rect(rect.x1+width, rect.y1, rect.x2-width, rect.y1+width), color);
	fill_rect(Rect(rect.x1+width, rect.y2-width, rect.x2-width, rect.y2), color);
}

void Display::draw_vertical_gradient(const Rect &rect, const Color &color1, const Color &color2)
{
	const uint16_t height = rect.y2 - rect.y1;
	for (int16_t y = rect.y1, i = 0; y <= rect.y2; y++, i++)
		fill_rect(Rect(rect.x1, y, rect.x2, y), Color::between(color1, color2, i, height));
}

void Display::draw_horizontal_gradient(const Rect &rect, const Color &color1, const Color &color2)
{
	const uint16_t width = rect.x2 - rect.x1;
	for (int x = rect.x1, i = 0; x <= rect.x2; x++, i++)
		fill_rect(Rect(x, rect.y1, x, rect.y2), Color::between(color1, color2, i, width));
}

void Display::paint_text(int16_t x, int16_t y, const wchar_t *text, const FontInfo *font, const Color &color)
{
	for (size_t i = 0; text[i]; i++)
	{
		if (text[i] == ' ')
		{
			x += 2*font->spacePixels;
			continue;
		}
		const FontCharInfo* char_info = find_char_info(font, text[i]);
		if (char_info == NULL) char_info = find_char_info(font, '?');
		paint_character(
			x, y,
			&font->data[char_info->offset],
			char_info->widthBits,
			font->heightPages,
			color
		);
		x += char_info->widthBits + font->spacePixels;
	}
}

void Display::paint_text_in_rect(const Rect &rect, HorizAlign align, const wchar_t *text, const FontInfo *font, const Color &color)
{
	const Size display_size = get_size();
	if (rect.intersects(Rect(Point(0, 0), display_size))) return;

	const Size text_size = get_text_size(font, text);

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

	paint_text(x, y, text, font, color);
}

Size Display::get_text_size(const FontInfo *font, const wchar_t *text)
{
	int width = 0;
	for (size_t i = 0; text[i]; i++)
	{
		if (text[i] == ' ')
		{
			width += 2*font->spacePixels;
			continue;
		}
		const FontCharInfo* char_info = find_char_info(font, text[i]);
		if (char_info == NULL) char_info = find_char_info(font, '?');
		width += char_info->widthBits;
		if (i != 0) width += font->spacePixels;
	}
	return Size(width, font->heightPages);
}

const FontCharInfo* Display::find_char_info(const FontInfo *font, wchar_t chr)
{
	for (size_t i = 0; font->charInfo[i].character; i++)
		if (chr == font->charInfo[i].character) return &font->charInfo[i];
	return NULL;
}

void Display::fill_triangle(Point pt1, Point pt2, Point pt3, const Color &color)
{
	if (pt1.x > pt2.x) swap(pt1, pt2);
	if (pt2.x > pt3.x) swap(pt2, pt3);
	if (pt1.x > pt3.x) swap(pt1, pt3);

	uint16_t dx1 = pt2.x - pt1.x;
	int16_t dy1 = pt2.y - pt1.y;
	uint16_t dy1_abs = dx1 ? (abs(dy1) % dx1) : 0;
	int16_t step1 = pt2.y > pt1.y ? 1 : -1;
	int16_t int1 = dx1 ? (dy1 / dx1) : 0;
	int16_t cnt_y1 = dx1 / 2;
	int16_t y1 = pt1.y;

	uint16_t dx2 = pt3.x - pt1.x;
	int16_t dy2 = pt3.y - pt1.y;
	uint16_t dy2_abs = dx2 ? (abs(dy2) % dx2) : 0;
	int16_t step2 = pt3.y > pt1.y ? 1 : -1;
	int16_t int2 = dx2 ? (dy2 / dx2) : 0;
	int16_t cnt_y2 = dx2 / 2;
	int16_t y2 = pt1.y;

	int16_t scr_width = get_size().width;

	for (int16_t x = pt1.x; x <= pt3.x; x++)
	{
		if (x == pt2.x)
		{
			dx1 = pt3.x - pt2.x;
			if (dx1 == 0) break;
			dy1 = pt3.y - pt2.y;
			dy1_abs = abs(dy1) % dx1;
			step1 = pt3.y > pt2.y ? 1 : -1;
			int1 = dy1 / dx1;
			cnt_y1 = dx1 / 2;
			y1 = pt2.y;
		}

		if ((x >= 0) || (x < scr_width))
			fill_rect(Rect(x, min(y1, y2), x, max(y1, y2)), color);

		cnt_y1 -= dy1_abs;
		if (cnt_y1 < 0)
		{
			cnt_y1 += dx1;
			y1 += step1;
		}
		y1 += int1;

		cnt_y2 -= dy2_abs;
		if (cnt_y2 < 0)
		{
			cnt_y2 += dx2;
			y2 += step2;
		}
		y2 += int2;
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

} // end "namespace muil"
