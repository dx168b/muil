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
  File:      muil_widgets.hpp
  Author(s): Denis Artyomov
=============================================================================*/

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


enum class ButtonStyle
{
	Normal,
	Pressed
};

void draw_button(const Rect &rect, Color color, ButtonStyle style, bool composite);
void draw_checkbox_rect(const Rect &rect, Color color, ButtonStyle style);
void draw_choice_rect(const Rect &rect, Color color, ButtonStyle style);
void draw_indented_ctrl_rect(const Rect &rect, Color color, ButtonStyle style);
int get_indented_ctrl_border();

} // namespace muil

#endif
