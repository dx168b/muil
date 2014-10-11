/*=============================================================================

  Copyright (C) 2014 Denis Artyomov (denis.artyomov@gmail.com)

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
  File:      display_utils.hpp
  Author(s): Denis Artyomov
=============================================================================*/

#ifndef MUIL_DISPLAY_UTILS_HPP_FILE_INCLUDED_
#define MUIL_DISPLAY_UTILS_HPP_FILE_INCLUDED_

#include "muil_basic_gui.hpp"

namespace muil {

enum class Rotation
{
	Undefined,
	Portrait,
	Album,
	Portrait180,
	Album180,
};

namespace detailed {

inline bool check_and_correct_rect_coords(
	const Rect &rect,
	unsigned   scr_width,
	unsigned   scr_height,
	int        offset_x,
	int        offset_y,
	int16_t    &x1,
	int16_t    &y1,
	int16_t    &x2,
	int16_t    &y2)
{
	auto min = [] (int16_t x1, int16_t x2) { return x1 < x2 ? x1 : x2; };
	auto max = [] (int16_t x1, int16_t x2) { return x1 > x2 ? x1 : x2; };

	x1 = min(rect.x1, rect.x2) + offset_x;
	y1 = min(rect.y1, rect.y2) + offset_y;
	x2 = max(rect.x1, rect.x2) + offset_x;
	y2 = max(rect.y1, rect.y2) + offset_y;

	if ((x1 < 0) && (x2 < 0)) return false;
	if ((y1 < 0) && (y2 < 0)) return false;
	if ((x1 >= scr_width) && (x2 >= scr_width)) return false;
	if ((y1 >= scr_height) && (y2 > scr_height)) return false;

	if (x1 < 0) x1 = 0;
	if (y1 < 0) y1 = 0;
	if (x2 >= scr_width) x2 = scr_width-1;
	if (y2 >= scr_height) y2 = scr_height-1;

	return true;
}

} // namespace detailed
} // namespace muil

#endif
