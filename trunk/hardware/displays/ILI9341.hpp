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
  File:      ILI9341.hpp
  Author(s): Denis Artyomov
=============================================================================*/


#ifndef ILI9341_HPP_FILE_INCLUDED_
#define ILI9341_HPP_FILE_INCLUDED_

#include "muil_basic_gui.hpp"
#include "muil_display_utils.hpp"

namespace muil {

extern void delay_ms(uint16_t value);

template <typename SPI, typename CSPin, typename ResetPin, typename DCPin>
class ILI9341DisplaySPIConnector
{
public:
	static void reset();
	static void write_command(uint8_t index);
	static void write_data(uint8_t data);
	static void write_data16_count(uint16_t data, unsigned count);
	static uint8_t read_reg(uint8_t reg, uint8_t offset);
private:
};

template <typename SPI, typename CSPin, typename ResetPin, typename DCPin>
void  ILI9341DisplaySPIConnector<SPI, CSPin, ResetPin, DCPin>::reset()
{
	CSPin::on();
	DCPin::on();
	ResetPin::on();
	delay_ms(50);
	ResetPin::off();
	delay_ms(50);
	ResetPin::on();
	delay_ms(50);
}

template <typename SPI, typename CSPin, typename ResetPin, typename DCPin>
void ILI9341DisplaySPIConnector<SPI, CSPin, ResetPin, DCPin>::write_command(uint8_t cmd)
{
	DCPin::off();
	SPI::template cs_low<CSPin>();
	SPI::write(cmd);
	SPI::template cs_high<CSPin>();
}

template <typename SPI, typename CSPin, typename ResetPin, typename DCPin>
void ILI9341DisplaySPIConnector<SPI, CSPin, ResetPin, DCPin>::write_data(uint8_t data)
{
	DCPin::on();
	SPI::template cs_low<CSPin>();
	SPI::write(data);
	SPI::template cs_high<CSPin>();
}

template <typename SPI, typename CSPin, typename ResetPin, typename DCPin>
void ILI9341DisplaySPIConnector<SPI, CSPin, ResetPin, DCPin>::write_data16_count(uint16_t data, unsigned count)
{
	const uint8_t byte1 = data >> 8;
	const uint8_t byte2 = data & 0xFF;
	DCPin::on();
	SPI::template cs_low<CSPin>();
	for (unsigned i = 0; i < count; i++)
	{
		SPI::write(byte1);
		SPI::write(byte2);
	}
	SPI::template cs_high<CSPin>();
}

template <typename SPI, typename CSPin, typename ResetPin, typename DCPin>
uint8_t ILI9341DisplaySPIConnector<SPI, CSPin, ResetPin, DCPin>::read_reg(uint8_t reg, uint8_t offset)
{
	write_command(0xD9);
	write_data(0x10 + offset);

	DCPin::off();
	SPI::template cs_low<CSPin>();
	SPI::write(reg);
	DCPin::on();
	uint8_t result = SPI::write_and_read(0);
	SPI::template cs_high<CSPin>();
	return result;
}

template <typename Connector>
class ILI9341Display
{
public:
	enum {
		Width        = 240,
		Height       = 320,
		SupportColor = 1
	};

	uint32_t init();

	unsigned get_width() const;
	unsigned get_height() const;
	static uint16_t get_dpi();
	void set_offset(int x, int y) { offset_x_ = x; offset_y_ = y; }
	void set_point(int x, int y, const Color &color);
	void fill_rect(const Rect &rect, const Color &color);
	void paint_character(int x0, int y0, const uint8_t *data, uint8_t width, uint8_t height, const Color &color, const Color *bg_color);
	void set_rotation(Rotation rotation);

private:
	static void set_window(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2);

	static bool crd_is_ok(int16_t x, int16_t y)
	{
		return ((x >= 0) && (y >= 0) && (x < Width) && (y < Height));
	}

	static uint16_t rgb_to_value(Color color);

	Rotation cur_rotation_ = Rotation::Undefined;
	int offset_x_ = 0;
	int offset_y_ = 0;


	static constexpr uint8_t ILI9341_NOP = 0x00;
	static constexpr uint8_t ILI9341_SWRESET = 0x01;
	static constexpr uint8_t ILI9341_RDDID = 0x04;
	static constexpr uint8_t ILI9341_RDDST = 0x09;

	static constexpr uint8_t ILI9341_SLPIN = 0x10;
	static constexpr uint8_t ILI9341_SLPOUT = 0x11;

	static constexpr uint8_t ILI9341_PTLON = 0x12;
	static constexpr uint8_t ILI9341_NORON = 0x13;

	static constexpr uint8_t ILI9341_RDMODE = 0x0A;
	static constexpr uint8_t ILI9341_RDMADCTL = 0x0B;
	static constexpr uint8_t ILI9341_RDPIXFMT = 0x0C;
	static constexpr uint8_t ILI9341_RDIMGFMT = 0x0A;
	static constexpr uint8_t ILI9341_RDSELFDIAG = 0x0F;

	static constexpr uint8_t ILI9341_INVOFF = 0x20;
	static constexpr uint8_t ILI9341_INVON = 0x21;
	static constexpr uint8_t ILI9341_GAMMASET = 0x26;
	static constexpr uint8_t ILI9341_DISPOFF = 0x28;
	static constexpr uint8_t ILI9341_DISPON = 0x29;

	static constexpr uint8_t ILI9341_CASET = 0x2A;
	static constexpr uint8_t ILI9341_PASET = 0x2B;
	static constexpr uint8_t ILI9341_RAMWR = 0x2C;
	static constexpr uint8_t ILI9341_RAMRD = 0x2E;

	static constexpr uint8_t ILI9341_PTLAR = 0x30;
	static constexpr uint8_t ILI9341_MADCTL = 0x36;
	static constexpr uint8_t ILI9341_PIXFMT = 0x3A;

	static constexpr uint8_t ILI9341_FRMCTR1 = 0xB1;
	static constexpr uint8_t ILI9341_FRMCTR2 = 0xB2;
	static constexpr uint8_t ILI9341_FRMCTR3 = 0xB3;
	static constexpr uint8_t ILI9341_INVCTR = 0xB4;
	static constexpr uint8_t ILI9341_DFUNCTR = 0xB6;

	static constexpr uint8_t ILI9341_PWCTR1 = 0xC0;
	static constexpr uint8_t ILI9341_PWCTR2 = 0xC1;
	static constexpr uint8_t ILI9341_PWCTR3 = 0xC2;
	static constexpr uint8_t ILI9341_PWCTR4 = 0xC3;
	static constexpr uint8_t ILI9341_PWCTR5 = 0xC4;
	static constexpr uint8_t ILI9341_VMCTR1 = 0xC5;
	static constexpr uint8_t ILI9341_VMCTR2 = 0xC7;

	static constexpr uint8_t ILI9341_RDID1 = 0xDA;
	static constexpr uint8_t ILI9341_RDID2 = 0xDB;
	static constexpr uint8_t ILI9341_RDID3 = 0xDC;
	static constexpr uint8_t ILI9341_RDID4 = 0xDD;

	static constexpr uint8_t ILI9341_GMCTRP1 = 0xE0;
	static constexpr uint8_t ILI9341_GMCTRN1 = 0xE1;

// #define ILI9341_PWCTR6  0xFC
};

template <typename Connector>
uint32_t ILI9341Display<Connector>::init()
{
	Connector::reset();

	uint32_t display_code =
		(Connector::read_reg(0xD3, 1) << 16) |
		(Connector::read_reg(0xD3, 2) << 8) |
		Connector::read_reg(0xD3, 3);

	if (display_code != 0x9341) return display_code;

	Connector::write_command(0xEF);
	Connector::write_data(0x03);
	Connector::write_data(0x80);
	Connector::write_data(0x02);

	Connector::write_command(0xCF);
	Connector::write_data(0x00);
	Connector::write_data(0XC1);
	Connector::write_data(0X30);

	Connector::write_command(0xED);
	Connector::write_data(0x64);
	Connector::write_data(0x03);
	Connector::write_data(0X12);
	Connector::write_data(0X81);

	Connector::write_command(0xE8);
	Connector::write_data(0x85);
	Connector::write_data(0x00);
	Connector::write_data(0x78);

	Connector::write_command(0xCB);
	Connector::write_data(0x39);
	Connector::write_data(0x2C);
	Connector::write_data(0x00);
	Connector::write_data(0x34);
	Connector::write_data(0x02);

	Connector::write_command(0xF7);
	Connector::write_data(0x20);

	Connector::write_command(0xEA);
	Connector::write_data(0x00);
	Connector::write_data(0x00);

	Connector::write_command(ILI9341_PWCTR1);    //Power control
	Connector::write_data(0x23);   //VRH[5:0]

	Connector::write_command(ILI9341_PWCTR2);    //Power control
	Connector::write_data(0x10);   //SAP[2:0];BT[3:0]

	Connector::write_command(ILI9341_VMCTR1);    //VCM control
	Connector::write_data(0x3e); //
	Connector::write_data(0x28);

	Connector::write_command(ILI9341_VMCTR2);    //VCM control2
	Connector::write_data(0x86);  //--

	Connector::write_command(ILI9341_MADCTL);    // Memory Access Control
	Connector::write_data(0x48);

	Connector::write_command(ILI9341_PIXFMT);
	Connector::write_data(0x55);

	Connector::write_command(ILI9341_FRMCTR1);
	Connector::write_data(0x00);
	Connector::write_data(0x18);

	Connector::write_command(ILI9341_DFUNCTR);    // Display Function Control
	Connector::write_data(0x08);
	Connector::write_data(0x82);
	Connector::write_data(0x27);

	Connector::write_command(0xF2);    // 3Gamma Function Disable
	Connector::write_data(0x00);

	Connector::write_command(ILI9341_GAMMASET);    //Gamma curve selected
	Connector::write_data(0x01);

	Connector::write_command(ILI9341_GMCTRP1);    //Set Gamma
	Connector::write_data(0x0F);
	Connector::write_data(0x31);
	Connector::write_data(0x2B);
	Connector::write_data(0x0C);
	Connector::write_data(0x0E);
	Connector::write_data(0x08);
	Connector::write_data(0x4E);
	Connector::write_data(0xF1);
	Connector::write_data(0x37);
	Connector::write_data(0x07);
	Connector::write_data(0x10);
	Connector::write_data(0x03);
	Connector::write_data(0x0E);
	Connector::write_data(0x09);
	Connector::write_data(0x00);

	Connector::write_command(ILI9341_GMCTRN1);    //Set Gamma
	Connector::write_data(0x00);
	Connector::write_data(0x0E);
	Connector::write_data(0x14);
	Connector::write_data(0x03);
	Connector::write_data(0x11);
	Connector::write_data(0x07);
	Connector::write_data(0x31);
	Connector::write_data(0xC1);
	Connector::write_data(0x48);
	Connector::write_data(0x08);
	Connector::write_data(0x0F);
	Connector::write_data(0x0C);
	Connector::write_data(0x31);
	Connector::write_data(0x36);
	Connector::write_data(0x0F);

	Connector::write_command(ILI9341_SLPOUT); //Exit Sleep
	delay_ms(120);
	Connector::write_command(ILI9341_DISPON); //Display on

	set_rotation(Rotation::Portrait);

	return display_code;
}

template <typename Connector>
unsigned ILI9341Display<Connector>::get_width() const
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

template <typename Connector>
unsigned ILI9341Display<Connector>::get_height() const
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

template <typename Connector>
uint16_t ILI9341Display<Connector>::get_dpi()
{
	return 96;
}

template <typename Connector>
void ILI9341Display<Connector>::set_point(int x, int y, const Color &color)
{
	x += offset_x_;
	y += offset_y_;
	if ( !crd_is_ok(x, y) ) return;
//	write_reg(0x0022, rgb_to_value(color));
}

template <typename Connector>
void ILI9341Display<Connector>::fill_rect(const Rect &rect, const Color &color)
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
	Connector::write_command(ILI9341_RAMWR);
	Connector::write_data16_count(color_v, (x2 - x1 + 1) * (y2 - y1 + 1));
}

template <typename Connector>
void ILI9341Display<Connector>::paint_character(
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
	const uint16_t color_v = rgb_to_value(color);
	const uint16_t bg_color_v = bg_color ? rgb_to_value(*bg_color) : 0;
	int16_t y = y0;

	if (bg_color)
	{
		set_window(x0, y0, x0 + width - 1, y0 + Height - 1);
		Connector::write_command(ILI9341_RAMWR);
	}

	for (; height != 0; height--, y++)
	{
		int32_t x = x0;

		uint8_t value = 0x80;

		for (uint8_t i = 0; i < width; i++)
		{
			bool on;

			if (value == 0x80)
			{
				value = *data++;
				on = value & 0x80;
				value = (value << 1) | 1;
			}
			else
			{
				on = value & 0x80;
				value <<= 1;
			}

			if (crd_is_ok(x, y))
			{
				if (on)
				{
					if (!bg_color)
					{
						set_window(x, y, x, y);
						Connector::write_command(ILI9341_RAMWR);
					}
					Connector::write_data16_count(color_v, 1);
				}
				else if (bg_color)
					Connector::write_data16_count(bg_color_v, 1);
			}
			x++;
		}
	}
}

template <typename Connector>
void ILI9341Display<Connector>::set_window(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2)
{
	Connector::write_command(ILI9341_CASET);
	Connector::write_data(x1 >> 8);
	Connector::write_data(x1 & 0xFF);
	Connector::write_data(x2 >> 8);
	Connector::write_data(x2 & 0xFF);

	Connector::write_command(ILI9341_PASET);
	Connector::write_data(y1 >> 8);
	Connector::write_data(y1 & 0xFF);
	Connector::write_data(y2 >> 8);
	Connector::write_data(y2 & 0xFF);
}

template <typename Connector>
uint16_t ILI9341Display<Connector>::rgb_to_value(Color color)
{
	uint16_t r16 = (color.r >> 3);
	uint16_t g16 = (color.g >> 2);
	uint16_t b16 = (color.b >> 3);
	return (r16 << 11) | (g16 << 5) | b16;
}

template <typename Connector>
void ILI9341Display<Connector>::set_rotation(Rotation rotation)
{
	if (rotation == cur_rotation_) return;

	Connector::write_command(ILI9341_MADCTL);

	switch (rotation)
	{
	case Rotation::Portrait:
		Connector::write_data(0x48);
		break;

	case Rotation::Album:
		Connector::write_data(0x28);
		break;

	case Rotation::Portrait180:
		Connector::write_data(0x88);
		break;

	case Rotation::Album180:
		Connector::write_data(0x80|0x40|0x20|0x08);
		break;
	}

	cur_rotation_ = rotation;
}

} // end "namespace muil"

#define IMPLEMENT_ILI9341_DISPLAY(OBJ) \
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

