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
  File:      ILI9320.hpp
  Author(s): Denis Artyomov
=============================================================================*/

#ifndef ILI9320_HPP_FILE_INCLUDED_
#define ILI9320_HPP_FILE_INCLUDED_

#include "muil_basic_gui.hpp"
#include "muil_display_utils.hpp"

namespace muil {

extern void delay_ms(uint16_t value);

template <typename SPI, typename CSPin>
class ILI9320DisplaySPIConnector
{
public:
	static void write_index(uint8_t index);
	static void write_data(uint16_t data);
	static uint16_t read_data();
	static void write_data16(uint16_t value, uint16_t count);

private:
	const static uint8_t SPI_START = 0x70; // Start byte for SPI transfer
	const static uint8_t SPI_RD    = 0x01; // WR bit 1 within start
	const static uint8_t SPI_WR    = 0x00; // WR bit 0 within start
	const static uint8_t SPI_DATA  = 0x02; // RS bit 1 within start byte
	const static uint8_t SPI_INDEX = 0x00; // RS bit 0 within start byte
};

template <typename SPI, typename CSPin>
void ILI9320DisplaySPIConnector<SPI, CSPin>::write_index(uint8_t index)
{
	SPI::template cs_low<CSPin>();
	SPI::write(SPI_START | SPI_WR | SPI_INDEX);
	SPI::write(0);
	SPI::write(index);
	SPI::template cs_high<CSPin>();
}

template <typename SPI, typename CSPin>
void ILI9320DisplaySPIConnector<SPI, CSPin>::write_data(uint16_t data)
{
	SPI::template cs_low<CSPin>();
	SPI::write(SPI_START | SPI_WR | SPI_DATA);
	SPI::write(data >> 8);
	SPI::write(data & 0xFF);
	SPI::template cs_high<CSPin>();
}

template <typename SPI, typename CSPin>
uint16_t ILI9320DisplaySPIConnector<SPI, CSPin>::read_data()
{
	SPI::template cs_low<CSPin>();
	SPI::write(SPI_START | SPI_RD | SPI_DATA);
	SPI::write(0); 
	uint16_t result = SPI::write_and_read(0) & 0xFF;
	result <<= 8;
	result |= (SPI::write_and_read(0) & 0xFF);
	SPI::template cs_high<CSPin>();
	return result;
}

template <typename SPI, typename CSPin>
void ILI9320DisplaySPIConnector<SPI, CSPin>::write_data16(uint16_t value, uint16_t count)
{
	const uint8_t high = (value & 0xFF00) >> 8;
	const uint8_t low = (value & 0xFF);
	SPI::template cs_low<CSPin>();
	SPI::write(SPI_START | SPI_WR | SPI_DATA);
	while (count--)
	{
		SPI::write(high);
		SPI::write(low);
	}
	SPI::template cs_high<CSPin>();
}


template <typename Connector, typename ResetPin>
class ILI9320Display
{
public:
	enum {
		Width        = 240,
		Height       = 320,
		SupportColor = 1
	};

	uint16_t init();

	unsigned get_width() const;
	unsigned get_height() const;
	static uint16_t get_dpi();
	void set_offset(int x, int y) { offset_x_ = x; offset_y_ = y; }
	void set_point(int x, int y, const Color &color);
	void fill_rect(const Rect &rect, const Color &color);
	void paint_character(int x0, int y0, const uint8_t *data, uint8_t width, uint8_t height, const Color &color, const Color *bg_color);

private:
	bool crd_is_ok(int16_t x, int16_t y)
	{
		return ((x >= 0) && (y >= 0) && (x < get_width()) && (y < get_height()));
	}

	static void write_reg(uint8_t reg, uint16_t data);
	static uint16_t read_reg(uint8_t reg);
	void set_window(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2);
	static uint16_t rgb_to_value(Color color);
	void set_direction(Rotation rotation);

	Rotation cur_rotation_ = Rotation::Undefined;
	int offset_x_ = 0;
	int offset_y_ = 0;
};

template <typename Connector, typename ResetPin>
uint16_t ILI9320Display<Connector, ResetPin>::init()
{
	delay_ms(50);
	ResetPin::on();
	delay_ms(50);
	ResetPin::off();
	delay_ms(50);
	ResetPin::on();
	delay_ms(50);

	struct RegValue
	{
		uint8_t reg;
		uint16_t value;
	};

	write_reg(0x00, 0x0001); // Set the Vcore voltage and this setting is must
	write_reg(0xE5, 0x8000); // Start internal OSC

	delay_ms(10);

	uint16_t display_code = read_reg(0);
	if (display_code != 0x9320) return display_code;

	static const RegValue values[] =
	{
		{ 0x01, 0x0100 }, /* Driver Output Contral */

		{ 0x02, 0x0700 }, /* LCD Driver Waveform Contral */
		{ 0x04, 0x0000 }, /* Scalling Contral */
		{ 0x08, 0x0202 }, /* Display Contral 2 */
		{ 0x09, 0x0000 }, /* Display Contral 3 */
		{ 0x0a, 0x0000 }, /* Frame Cycle Contal */
		{ 0x0c, (1<<0) }, /* Extern Display Interface Contral 1 */
		{ 0x0d, 0x0000 }, /* Frame Maker Position */
		{ 0x0f, 0x0000 }, /* Extern Display Interface Contral 2 */
		{ 0x07, 0x0101 }, /* Display Contral */
		{ 0xFF },
		{ 0x10, (1<<12)|(0<<8)|(1<<7)|(1<<6)|(0<<4) }, /* Power Control 1 */
		{ 0x11, 0x0007 },                              /* Power Control 2 */
		{ 0x12, (1<<8)|(1<<4)|(0<<0) },                /* Power Control 3 */
		{ 0x13, 0x0b00 },                              /* Power Control 4 */
		{ 0x29, 0x0000 },                              /* Power Control 7 */

		{ 0x2b, (1<<14)|(1<<4) },
		{ 0x50, 0 },      /* Set X Start */
		{ 0x51, 239 },    /* Set X End */
		{ 0x52, 0 },	     /* Set Y Start */
		{ 0x53, 319 },    /* Set Y End */
		{ 0xFF },

		{ 0x60, 0x2700 }, /* Driver Output Control */
		{ 0x61, 0x0001 }, /* Driver Output Control */
		{ 0x6a, 0x0000 }, /* Vertical Srcoll Control */

		{ 0x80, 0x0000 }, /* Display Position? Partial Display 1 */
		{ 0x81, 0x0000 }, /* RAM Address Start? Partial Display 1 */
		{ 0x82, 0x0000 }, /* RAM Address End-Partial Display 1 */
		{ 0x83, 0x0000 }, /* Displsy Position? Partial Display 2 */
		{ 0x84, 0x0000 }, /* RAM Address Start? Partial Display 2 */
		{ 0x85, 0x0000 }, /* RAM Address End? Partial Display 2 */

		{ 0x90, (0<<7)|(16<<0) }, /* Frame Cycle Contral */
		{ 0x92, 0x0000 },         /* Panel Interface Contral 2 */
		{ 0x93, 0x0001 },         /* Panel Interface Contral 3 */
		{ 0x95, 0x0110 },         /* Frame Cycle Contral */
		{ 0x97, (0<<8) },
		{ 0x98, 0x0000 },         /* Frame Cycle Contral */
		{ 0x07, 0x0133 },
		{ 0xFF },
	};

	const RegValue *val_ptr = values;
	for (uint16_t i = 0; i < sizeof(values)/sizeof(*values); i++)
	{
		if (val_ptr->reg == 0xFF) delay_ms(50);
		else write_reg(val_ptr->reg, val_ptr->value);
		val_ptr++;
	}

	set_direction(Rotation::Album);

	return display_code;
}

template <typename Connector, typename ResetPin>
unsigned ILI9320Display<Connector, ResetPin>::get_width() const
{
	switch (cur_rotation_)
	{
	case Rotation::Portrait:
	case Rotation::Portrait180:
		return Width;

	case Rotation::Album:
	case Rotation::Album180:
		return Height;
	}

	return Width;
}

template <typename Connector, typename ResetPin>
unsigned ILI9320Display<Connector, ResetPin>::get_height() const
{
	switch (cur_rotation_)
	{
	case Rotation::Portrait:
	case Rotation::Portrait180:
		return Height;

	case Rotation::Album:
	case Rotation::Album180:
		return Width;
	}

	return Height;
}

template <typename Connector, typename ResetPin>
uint16_t ILI9320Display<Connector, ResetPin>::get_dpi()
{
	return 96;
}

template <typename Connector, typename ResetPin>
void ILI9320Display<Connector, ResetPin>::set_point(int x, int y, const Color &color)
{
	x += offset_x_;
	y += offset_y_;
	if ( !crd_is_ok(x, y) ) return;
	set_window(x, y, x, y);
	write_reg(0x0022, rgb_to_value(color));
}

template <typename Connector, typename ResetPin>
void ILI9320Display<Connector, ResetPin>::fill_rect(const Rect &rect, const Color &color)
{
	int16_t x1, y1, x2, y2;
	bool ok = detailed::check_and_correct_rect_coords(
		rect,
		get_width(),
		get_height(),
		offset_x_,
		offset_y_,
		x1, y1, x2, y2
	);

	if (!ok) return;
	const uint16_t color_v = rgb_to_value(color);
	set_window(x1, y1, x2, y2);
	Connector::write_index(0x0022);
	Connector::write_data16(color_v, (x2 - x1 + 1) * (y2 - y1 + 1));
}

template <typename Connector, typename ResetPin>
void ILI9320Display<Connector, ResetPin>::paint_character(
	int           x0,
	int           y0,
	const uint8_t *data,
	uint8_t       width,
	uint8_t       height,
	const Color   &color,
	const Color   *bg_color)
{
	x0 += offset_x_;
	y0 += offset_y_;
	const uint16_t w8 = (width + 7) / 8;
	const uint16_t color_v = rgb_to_value(color);
	int16_t y = y0;
	for (; height != 0; height--, y++)
	{
		int32_t x = x0;
		for (uint16_t i = 0; i < w8; i++)
		{
			uint8_t value = *data++;
			for (uint8_t j = 0; j < 8; j++)
			{
				if (crd_is_ok(x, y) && (value & 0x80))
				{
					set_window(x, y, x, y);
					write_reg(0x0022, color_v);
				}
				x++;
				value <<= 1;
			}
		}
	}
}

template <typename Connector, typename ResetPin>
void ILI9320Display<Connector, ResetPin>::write_reg(uint8_t reg, uint16_t data)
{
	Connector::write_index(reg);
	Connector::write_data(data);
}

template <typename Connector, typename ResetPin>
uint16_t ILI9320Display<Connector, ResetPin>::read_reg(uint8_t reg)
{
	Connector::write_index(reg);
	return Connector::read_data();
}

template <typename Connector, typename ResetPin>
void ILI9320Display<Connector, ResetPin>::set_window(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2)
{
	switch (cur_rotation_)
	{
		case Rotation::Portrait:
			write_reg(0x50, x1);
			write_reg(0x51, x2);
			write_reg(0x52, y1);
			write_reg(0x53, y2);
			break;

		case Rotation::Album:
			write_reg(0x50, Width - y2);
			write_reg(0x51, Width - y1);
			write_reg(0x52, x1);
			write_reg(0x53, x2);
			break;

		case Rotation::Portrait180:
			write_reg(0x50, Width - x2);
			write_reg(0x51, Width - y1);
			write_reg(0x52, Height - x2);
			write_reg(0x53, Height - x1);
			break;

		case Rotation::Album180:
			write_reg(0x50, y1);
			write_reg(0x51, y2);
			write_reg(0x52, Height - x2);
			write_reg(0x53, Height - x1);
			break;
	}
}

template <typename Connector, typename ResetPin>
uint16_t ILI9320Display<Connector, ResetPin>::rgb_to_value(Color color)
{
	uint16_t r16 = (color.r >> 3);
	uint16_t g16 = (color.g >> 2);
	uint16_t b16 = (color.b >> 3);
	return (r16 << 11) | (g16 << 5) | b16;
}

template <typename Connector, typename ResetPin>
void ILI9320Display<Connector, ResetPin>::set_direction(Rotation rotation)
{
	if (rotation == cur_rotation_) return;
	switch (rotation)
	{
		case Rotation::Portrait:
			write_reg(0x03, 0x10b0);
			break;

		case Rotation::Album:
			write_reg(0x03, 0x10a8);
			break;

		case Rotation::Portrait180:
			write_reg(0x03, 0x1080);
			break;

		case Rotation::Album180:
			write_reg(0x03, 0x1098);
			break;
	}
	cur_rotation_ = rotation;
}

} // end "namespace muil"

#define IMPLEMENT_ILI9320_DISPLAY(OBJ) \
namespace muil { \
	unsigned display_get_width() { return OBJ.get_width(); } \
	unsigned display_get_height() { return OBJ.get_height(); } \
	uint16_t display_get_dpi() { return OBJ.get_dpi(); } \
	void display_set_offset(int x, int y) { OBJ.set_offset(x, y); } \
	void display_set_point(int x, int y, const Color &color) { OBJ.set_point(x, y, color); } \
	void display_fill_rect(const Rect &rect, const Color &color) { OBJ.fill_rect(rect, color); } \
	void display_paint_character(int x0, int y0, const uint8_t *data, uint8_t width, uint8_t height, const Color &color, const Color *bg_color) { OBJ.paint_character(x0, y0, data, width, height, color, bg_color); } \
}


#endif
