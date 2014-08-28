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

} // end "namespace muil"
