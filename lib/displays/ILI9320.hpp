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

#include "../muil_classes.hpp"

namespace muil {

template <typename SPI, typename CSPin>
class ILI9320DisplaySPIConnector
{
public:
	static void write_index(uint8_t index);
	static void write_data(uint16_t data);
	static uint16_t read_data();
	static void fill_pixels(uint16_t value, uint16_t count);

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
	uint16_t result = SPI::read(0) & 0xFF;
	result <<= 8;
	result |= (SPI::read(0) & 0xFF);
	SPI::template cs_high<CSPin>();
	return result;
}

template <typename SPI, typename CSPin>
void ILI9320DisplaySPIConnector<SPI, CSPin>::fill_pixels(uint16_t value, uint16_t count)
{
	const uint8_t high = (value & 0xFF00) >> 8;
	const uint8_t low = (value & 0xFF);
	write_index(0x0022);
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
class ILI9320Display : public Display
{
public:
	enum {
		Width        = 240,
		Height       = 320,
		SupportColor = 1
	};

	static void init(void (*delay_ms)(uint16_t millisoconds));

	Size get_size() const;
	uint16_t get_dpi() const;
	void set_point(int16_t x, int16_t y, const Color &color);
	void fill_rect(const Rect &rect, const Color &color);
	void paint_character(int16_t x0, int16_t y0, const uint8_t *data, uint8_t width, uint8_t height, const Color &color);

private:
	static bool crd_is_ok(int16_t x, int16_t y)
	{
		return ((x >= 0) && (y >= 0) && (x < Width) && (y < Height));
	}

	static void write_reg(uint8_t reg, uint16_t data);
	static uint16_t read_reg(uint8_t reg);
	static void set_cursor(int32_t x, int32_t y);
	static uint16_t rgb_to_value(Color color);
	static void set_direction(Direction dir);

	static Direction cur_dir_;
};

template <typename Connector, typename ResetPin>
void ILI9320Display<Connector, ResetPin>::init(void (*delay_ms)(uint16_t millisoconds))
{
	delay_ms(50);
	ResetPin::On();
	delay_ms(50);
	ResetPin::Off();
	delay_ms(50);
	ResetPin::On();
	delay_ms(50);

	struct RegValue
	{
		uint8_t reg;
		uint16_t value;
	};

	static const RegValue values[] =
	{
		{ 0x00, 0x0000 },
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

/*		{0x0030, 0x0001}, // gamma curves
		{0x0031, 0x0606},
		{0x0032, 0x0304},
		{0x0035, 0x0103},
		{0x0036, 0x011D},
		{0x0037, 0x0404},
		{0x0038, 0x0404},
		{0x0039, 0x0404},
		{0x003C, 0x0700},
		{0x003D, 0x0A1F}, */

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

	set_direction(DIR_RIGHT);
}

template <typename Connector, typename ResetPin>
Size ILI9320Display<Connector, ResetPin>::get_size() const
{
	return Size(Width, Height);
}

template <typename Connector, typename ResetPin>
uint16_t ILI9320Display<Connector, ResetPin>::get_dpi() const
{
	return 96;
}

template <typename Connector, typename ResetPin>
void ILI9320Display<Connector, ResetPin>::set_point(int16_t x, int16_t y, const Color &color)
{
	x += offset_.x;
	y += offset_.y;
	if ( !crd_is_ok(x, y) ) return;
	set_cursor(x, y);
	write_reg(0x0022, rgb_to_value(color));
}

template <typename Connector, typename ResetPin>
void ILI9320Display<Connector, ResetPin>::fill_rect(const Rect &rect, const Color &color)
{
	int16_t x1 = rect.x1 + offset_.x;
	int16_t y1 = rect.y1 + offset_.y;
	int16_t x2 = rect.x2 + offset_.x;
	int16_t y2 = rect.y2 + offset_.y;

	if ((x1 < 0) && (x2 < 0)) return;
	if ((y1 < 0) && (y2 < 0)) return;
	if ((x1 >= Width) && (x2 >= Width)) return;
	if ((y1 >= Height) && (y2 >= Height)) return;

	if (x1 < 0) x1 = 0;
	if (y1 < 0) y1 = 0;
	if (x2 >= Width) x2 = Width-1;
	if (y2 >= Height) y2 = Height-1;

	const uint16_t width = x2 - x1 + 1;
	const uint16_t height = y2 - y1 + 1;
	const uint16_t color_v = rgb_to_value(color);

	if (width > height)
	{
		set_direction(DIR_RIGHT);
		for (int16_t y = y1; y <= y2; y++)
		{
			set_cursor(x1, y);
			Connector::fill_pixels(color_v, width);
		}
	}
	else
	{
		set_direction(DIR_DOWN);
		for (int16_t x = x1; x <= x2; x++)
		{
			set_cursor(x, y1);
			Connector::fill_pixels(color_v, height);
		}
	}
}

template <typename Connector, typename ResetPin>
void ILI9320Display<Connector, ResetPin>::paint_character(
	int16_t       x0,
	int16_t       y0,
	const uint8_t *data,
	uint8_t       width,
	uint8_t       height,
	const Color   &color)
{
	x0 += offset_.x;
	y0 += offset_.y;
	const uint16_t w8 = (width + 7) / 8;
	const uint16_t color_v = rgb_to_value(color);
	int16_t y = y0;
	set_direction(DIR_RIGHT);
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
					set_cursor(x, y);
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
void ILI9320Display<Connector, ResetPin>::set_cursor(int32_t x, int32_t y)
{
	write_reg(0x0020, 0xFFFF & x);
	write_reg(0x0021, 0xFFFF & y);
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
void ILI9320Display<Connector, ResetPin>::set_direction(Direction dir)
{
	if (dir == cur_dir_) return;
	uint16_t am = 0;
	uint16_t id = 0;
	switch (dir)
	{
		case DIR_RIGHT:
			id = 0x03;
			am = 0;
			break;

		case DIR_DOWN:
			id = 0x03;
			am = 1;
			break;

		default:
			break;
	}
	write_reg(0x03, 0x1000 | (am << 3) | (id << 4));
	cur_dir_ = dir;
}

template <typename Connector, typename ResetPin>
Direction ILI9320Display<Connector, ResetPin>::cur_dir_ = DIR_UNDEFINED;

} // end "namespace muil"

#endif
