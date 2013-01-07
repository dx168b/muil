/*=============================================================================

  Copyright (C) 2012 Denis Artyomov (denis.artyomov@gmail.com)

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

uint8_t correct_color_value(int value)
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

Color Color::between(const Color &color1, const Color &color2, unsigned coeff)
{
	return Color(
		correct_color_value(color1.r + coeff * (color2.r - color1.r) / 256),
		correct_color_value(color1.g + coeff * (color2.g - color1.g) / 256),
		correct_color_value(color1.b + coeff * (color2.b - color1.b) / 256)
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

static Color get_intermediate_color(const Color &color1, const Color &color2, int16_t value, int16_t max)
{
	int16_t r = value*(color2.r-color1.r)/max + color1.r;
	int16_t g = value*(color2.g-color1.g)/max + color1.g;
	int16_t b = value*(color2.b-color1.b)/max + color1.b;
	return Color(r, g, b);
}

void Display::draw_vertical_gradient(const Rect &rect, const Color &color1, const Color &color2)
{
	const uint16_t height = rect.y2 - rect.y1;
	for (int16_t y = rect.y1, i = 0; y <= rect.y2; y++, i++)
		fill_rect(Rect(rect.x1, y, rect.x2, y), get_intermediate_color(color1, color2, i, height));
}

void Display::draw_horizontal_gradient(const Rect &rect, const Color &color1, const Color &color2)
{
	const uint16_t width = rect.x2 - rect.x1;
	for (int x = rect.x1, i = 0; x <= rect.x2; x++, i++)
		fill_rect(Rect(x, rect.y1, x, rect.y2), get_intermediate_color(color1, color2, i, width));
}

void Display::paint_text(const FontInfo *font, int16_t x, int16_t y, const wchar_t *text, const Color &color)
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
		paint_character(x, y, &font->data[char_info->offset], char_info->widthBits,
				font->heightPages, color);
		x += char_info->widthBits;
		x += font->spacePixels;
	}
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
		width += font->spacePixels;
	}
	return Size(width, font->heightPages);
}

const FontCharInfo* Display::find_char_info(const FontInfo *font, wchar_t chr)
{
	for (size_t i = 0; font->charInfo[i].character; i++)
		if (chr == font->charInfo[i].character) return &font->charInfo[i];
	return NULL;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

} // end "namespace muil"
