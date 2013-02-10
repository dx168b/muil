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
  File:      ADS7843.hpp
  Author(s): Denis Artyomov
=============================================================================*/

#ifndef ADS7843_HPP_FILE_INCLUDED_
#define ADS7843_HPP_FILE_INCLUDED_

#include "../muil_classes.hpp"

namespace muil {

template<class SPI, class CSPin, class PressedPin>
class ADS7843TouchScreen : public TouchScreen
{
public:
	bool is_pressed();
	Point get_pos();
	void calibrate(Display &display, const FontInfo *font, const wchar_t *text, DelayFun delay_ms);

private:
	struct Matrix
	{
		float An, Bn, Cn, Dn, En, Fn, Divider ;
		Matrix() : An(0), Bn(0), Cn(0), Dn(0), En(0), Fn(0), Divider(0) {}
	};

	void wait_for_press();
	static void paint_cross(Display &display, Point pt, const Color &color);
	static int16_t read_coord(uint8_t cmd);
	static void get_filtered_coords(int16_t &x, int16_t &y);
	static int16_t get_filtered_value(int16_t *values);
	bool calc_matrix(const Point *displayPtr, const Point *screenPtr);

	template <typename T> static T abs(T value)
	{
		return (value < 0) ? -value : value;
	}

	static const uint8_t CHX = 0x90; /* channel Y+ selection command */
	static const uint8_t CHY = 0xd0; /* channel X+ selection command*/
	static const uint8_t BufSize = 8;

	Matrix matrix_;
	Point last_pos_;
};

template<class SPI, class CSPin, class PressedPin>
bool ADS7843TouchScreen<SPI, CSPin, PressedPin>::is_pressed()
{
	bool result = !PressedPin::Signalled();
	if (!result) last_pos_ = Point(-1, -1);
	return result;
}

template<class SPI, class CSPin, class PressedPin>
Point ADS7843TouchScreen<SPI, CSPin, PressedPin>::get_pos()
{
	if (!is_pressed()) return Point(-1, -1);
	if (matrix_.Divider == 0) return Point(-1, -1);

	int16_t filtered_x, filtered_y;
	get_filtered_coords(filtered_x, filtered_y);
	if ((filtered_x == -1) && (filtered_y == -1)) return Point(-1, -1);

	Point result = Point(
		int16_t(((matrix_.An * filtered_x) + (matrix_.Bn * filtered_y) + matrix_.Cn) / matrix_.Divider),
		int16_t(((matrix_.Dn * filtered_x) + (matrix_.En * filtered_y) + matrix_.Fn) / matrix_.Divider)
	);

	return result;
}

template<class SPI, class CSPin, class PressedPin>
void ADS7843TouchScreen<SPI, CSPin, PressedPin>::calibrate(Display &display, const FontInfo *font, const wchar_t *text, DelayFun delay_ms)
{
	const Color bg_color = Color::white();
	const Size disp_size = display.get_size();

	Point display_points[3];
	Point touchscreen_points[3];
	display_points[0] = Point(disp_size.width/10, disp_size.height/10);
	display_points[1] = Point(disp_size.width/2, 9*disp_size.height/10);
	display_points[2] = Point(9*disp_size.width/10, disp_size.height/2);

	display.fill_rect(Rect(0, 0, disp_size.width-1, disp_size.height-1), bg_color);

	for (size_t i = 0; i < 3; i++)
	{
		for (;;)
		{
			display.paint_text(font, 1, 1, text, Color::black());
			paint_cross(display, display_points[i], Color::blue());

			while (is_pressed());
			wait_for_press();
			delay_ms(50);
			paint_cross(display, display_points[i], Color::red());
			get_filtered_coords(touchscreen_points[i].x, touchscreen_points[i].y);
			delay_ms(500);
			while (is_pressed()) {}
			paint_cross(display, display_points[i], bg_color);

			if ((touchscreen_points[i].x != -1) && (touchscreen_points[i].y != -1)) break;
		}
	}

	calc_matrix(display_points, touchscreen_points);
}

template<class SPI, class CSPin, class PressedPin>
void ADS7843TouchScreen<SPI, CSPin, PressedPin>::paint_cross(Display &display, Point pt, const Color &color)
{
	const int16_t line_size = display.get_dpi()/10;
	display.fill_rect(Rect(pt.x-1, pt.y-line_size, pt.x+1, pt.y+line_size), color);
	display.fill_rect(Rect(pt.x-line_size, pt.y-1, pt.x+line_size, pt.y+1), color);
}

template<class SPI, class CSPin, class PressedPin>
void ADS7843TouchScreen<SPI, CSPin, PressedPin>::wait_for_press()
{
	while (!is_pressed());
}

template<class SPI, class CSPin, class PressedPin>
int16_t ADS7843TouchScreen<SPI, CSPin, PressedPin>::read_coord(uint8_t cmd)
{
	SPI::template cs_low<CSPin>();
	SPI::write(cmd);
	uint32_t result = SPI::write_and_read(0) << 8;
	result |= SPI::write_and_read(0);
	SPI::template cs_high<CSPin>();
	result >>= 3;
	return result & 0xFFF;
}

template<class SPI, class CSPin, class PressedPin>
void ADS7843TouchScreen<SPI, CSPin, PressedPin>::get_filtered_coords(int16_t &x, int16_t &y)
{
	int16_t x_values[BufSize];
	int16_t y_values[BufSize];
	for (size_t i = 0; i < BufSize; i++)
	{
		x_values[i] = read_coord(CHX);
		y_values[i] = read_coord(CHY);
	}

	x = get_filtered_value(x_values);
	y = get_filtered_value(y_values);
}

template<class SPI, class CSPin, class PressedPin>
int16_t ADS7843TouchScreen<SPI, CSPin, PressedPin>::get_filtered_value(int16_t *values)
{
	int16_t aver;
	for (size_t step = 0; step < 2 * BufSize / 3; step++)
	{
		uint32_t total = 0;
		uint32_t cnt = 0;
		for (size_t i = 0; i < BufSize; i++)
		{
			if (values[i] == -1) continue;
			total += values[i];
			cnt++;
		}
		aver = total/cnt;

		int16_t min = 32767;
		size_t min_index = 0;
		int16_t max = -32767;
		size_t max_index = 0;
		for (size_t i = 0; i < BufSize; i++)
		{
			int16_t value = values[i];
			if (value == -1) continue;
			if (value < min)
			{
				min = value;
				min_index = i;
			}
			if (value > max)
			{
				max = value;
				max_index = i;
			}
		}
		int max_diff = abs(aver-max);
		int min_diff = abs(aver-min);
		if (max_diff > min_diff) values[max_index] = -1;
		else values[min_index] = -1;
	}
	return aver;
}

template<class SPI, class CSPin, class PressedPin>
bool ADS7843TouchScreen<SPI, CSPin, PressedPin>::calc_matrix(const Point *displayPtr, const Point *screenPtr)
{
	matrix_.Divider = ((screenPtr[0].x - screenPtr[2].x) * (screenPtr[1].y - screenPtr[2].y))
			- ((screenPtr[1].x - screenPtr[2].x) * (screenPtr[0].y - screenPtr[2].y));
	if (matrix_.Divider == 0)
	{
		return false;
	}
	else
	{
		matrix_.An = ((displayPtr[0].x - displayPtr[2].x) * (screenPtr[1].y - screenPtr[2].y))
				- ((displayPtr[1].x - displayPtr[2].x) * (screenPtr[0].y - screenPtr[2].y));
		matrix_.Bn = ((screenPtr[0].x - screenPtr[2].x) * (displayPtr[1].x - displayPtr[2].x))
				- ((displayPtr[0].x - displayPtr[2].x) * (screenPtr[1].x - screenPtr[2].x));
		matrix_.Cn = (screenPtr[2].x * displayPtr[1].x - screenPtr[1].x * displayPtr[2].x) * screenPtr[0].y
				+ (screenPtr[0].x * displayPtr[2].x - screenPtr[2].x * displayPtr[0].x) * screenPtr[1].y
				+ (screenPtr[1].x * displayPtr[0].x - screenPtr[0].x * displayPtr[1].x) * screenPtr[2].y;
		matrix_.Dn = ((displayPtr[0].y - displayPtr[2].y) * (screenPtr[1].y - screenPtr[2].y))
				- ((displayPtr[1].y - displayPtr[2].y) * (screenPtr[0].y - screenPtr[2].y));
		matrix_.En = ((screenPtr[0].x - screenPtr[2].x) * (displayPtr[1].y - displayPtr[2].y))
				- ((displayPtr[0].y - displayPtr[2].y) * (screenPtr[1].x - screenPtr[2].x));
		matrix_.Fn = (screenPtr[2].x * displayPtr[1].y - screenPtr[1].x * displayPtr[2].y) * screenPtr[0].y
				+ (screenPtr[0].x * displayPtr[2].y - screenPtr[2].x * displayPtr[0].y) * screenPtr[1].y
				+ (screenPtr[1].x * displayPtr[0].y - screenPtr[0].x * displayPtr[1].y) * screenPtr[2].y;
	}
	return true;
}

} // end "namespace muil"

#endif
