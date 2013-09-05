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
  File:      muil_classes.hpp
  Author(s): Denis Artyomov
=============================================================================*/

#ifndef MUIL_CLASSES_HPP_FILE_INCLUDED_
#define MUIL_CLASSES_HPP_FILE_INCLUDED_

#include <stdint.h>

namespace muil {

enum Direction
{
	DIR_UNDEFINED,
	DIR_UP,
	DIR_DOWN,
	DIR_LEFT,
	DIR_RIGHT,
};

enum HorizAlign
{
	HA_LEFT,
	HA_CENTER,
	HA_RIGHT,
};

struct Color
{
	uint8_t r;
	uint8_t g;
	uint8_t b;

	Color() : r(0), g(0), b(0) {}
	Color(uint8_t r, uint8_t g, uint8_t b) : r(r), g(g), b(b) {}

	static Color black() { return Color(0, 0, 0); }
	static Color white() { return Color(255, 255, 255); }
	static Color blue() { return Color(0, 0, 255); }
	static Color red() { return Color(255, 0, 0); }
	static Color green() { return Color(0, 255, 0); }

	Color light(int value) const;
	static Color between(const Color &color1, const Color &color2, int16_t value, int16_t max);
};

struct Size
{
	uint16_t width;
	uint16_t height;

	Size() : width(0), height(0) {}
	Size(uint16_t width, uint16_t height) : width(width), height(height) {}
};

struct Point
{
	int16_t x;
	int16_t y;

	Point() : x(0), y(0) {}
	Point(int16_t x, int16_t y) : x(x), y(y) {}

	Point moved(int16_t dx, int16_t dy) const { return Point(x + dx, y + dy); }

	bool equals(const Point &other) const
	{
		return (x == other.x) && (y == other.y);
	}

	bool operator == (const Point &other) const
	{
		return equals(other);
	}

	bool operator != (const Point &other) const
	{
		return !equals(other);
	}
};

struct Rect
{
	int16_t x1;
	int16_t y1;
	int16_t x2;
	int16_t y2;

	Rect() : x1(0), y1(0), x2(0), y2(0) {}
	Rect(const Point &pt, const Size &size) : x1(pt.x), y1(pt.y), x2(pt.x+size.width), y2(pt.y+size.height) {}
	Rect(int16_t x1, int16_t y1, int16_t x2, int16_t y2) : x1(x1), y1(y1), x2(x2), y2(y2) {}

	Rect inflated(int value) const;
	Rect moved(const Point &offset) const;
	int16_t height() const { return y2-y1; }
	int16_t width() const { return x2-x1; }

	bool contains(const Point pt) const;
	bool intersects(const Rect &other_rect) const;
};

// This structure describes a single character's display information
struct FontCharInfo
{
	const uint16_t character;
	const uint8_t  widthBits; // width, in bits (or pixels), of the character
	const uint16_t offset;    // offset of the character's bitmap, in bytes, into the the FONT_INFO's data array
};

// Describes a single font
struct FontInfo
{
	const uint8_t       heightPages; // height, in pages (8 pixels), of the font's characters
	const uint8_t       spacePixels; // number of pixels that a space character takes up
	const FontCharInfo* charInfo;    // pointer to array of char information
	const uint8_t*      data;        // pointer to generated array of character visual representation
};

Size     display_get_size();
uint16_t display_get_dpi();
void     display_set_offset(int x, int y);
void     display_set_point(int x, int y, const Color &color);
void     display_fill_rect(const Rect &rect, const Color &color);
void     display_paint_character(int x0, int y0, const uint8_t *data, uint8_t width, uint8_t height, const Color &color);

void     display_fill_rect(int x1, int y1, int x2, int y2, const Color &color);
void     display_draw_rect(const Rect &rect, int16_t width, const Color &color);
void     display_draw_vertical_gradient(const Rect &rect, const Color &color1, const Color &color2);
void     display_draw_horizontal_gradient(const Rect &rect, const Color &color1, const Color &color2);
void     display_paint_text(int x, int y, const wchar_t *text, const FontInfo *font, const Color &color);
void     display_paint_text_in_rect(const Rect &rect, HorizAlign align, const wchar_t *text, const FontInfo *font, const Color &color);
void     display_fill_triangle(int x1, int y1, int x2, int y2, int x3, int y3, const Color &color);
Size     display_get_text_size(const FontInfo *font, const wchar_t *text);

struct TouchScreenCalibrData
{
	const FontInfo *font;
	const wchar_t *text;
};

bool  touchscreen_is_pressed();
Point touchscreen_get_pos();
void  touchscreen_calibrate(TouchScreenCalibrData &data);

void delay_ms(uint16_t milliseconds);

} // end "namespace muil"

#endif

