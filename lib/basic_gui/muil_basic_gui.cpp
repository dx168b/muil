#include <stddef.h>
#include "muil_basic_gui.hpp"

namespace muil {

const FontCharInfo* display_find_char_info(const FontInfo &font, wchar_t chr)
{
	for (size_t i = 0; font.charInfo[i].character; i++)
		if (chr == font.charInfo[i].character) return &font.charInfo[i];
	return NULL;
}


template <typename T> void swap(T &v1, T &v2)
{
	T tmp(v1);
	v1 = v2;
	v2 = tmp;
}


template <typename T> T abs(const T v)
{
	return (v >= 0) ? v : -v;
}


template <typename T> T max(const T v1, const T v2)
{
	return v1 > v2 ? v1 : v2;
}


template <typename T> T min(const T v1, const T v2)
{
	return v1 < v2 ? v1 : v2;
}


static uint8_t correct_color_value(int value)
{
	if (value < 0) return 0;
	if (value > 255) return 255;
	return value & 0xFF;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

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

TextCharactersProvider::TextCharactersProvider(const wchar_t *text) 
	: text_(text)
{}


void TextCharactersProvider::provide(CharactersConsumer &consumer) const
{
	for (auto ptr = text_; *ptr; ptr++) consumer.consume(*ptr);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Integer10CharactersProvider::Integer10CharactersProvider(int value, int pt_pos) 
	: IntegerCharactersProvider(value),
	pt_pos_(pt_pos)
{}


void Integer10CharactersProvider::provide(CharactersConsumer &consumer) const
{
	int value = value_;
	int pt_pos = pt_pos_;

	if (value < 0)
	{
		value = -value;
		consumer.consume('-');
	}

	int comp_value = 1000000000;

	if (pt_pos != -1) pt_pos = 9 - pt_pos;

	bool begin = true;
	do
	{
		wchar_t chr = '0';
		while (value >= comp_value)
		{
			value -= comp_value;
			chr++;
		}
		if (chr != '0') begin = false;
		if (!begin || (comp_value == 1))
			consumer.consume(chr);
		comp_value /= 10;
		if ((pt_pos != -1) && (pt_pos-- == 0))
		{
			consumer.consume('.');
			begin = false;
		}
	} 
	while (comp_value != 0);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Integer16CharactersProvider::Integer16CharactersProvider(unsigned value) 
	: IntegerCharactersProvider(value)
{}


void Integer16CharactersProvider::provide(CharactersConsumer &consumer) const
{
	unsigned value = value_;
	bool begin = true;
	for (unsigned i = 0; i < 8; i++)
	{
		uint32_t val = (value & 0xF0000000) >> 28;
		wchar_t chr = (val < 10) ? (val + '0') : (val - 10 + 'A');
		if ((val != 0) || (i == 7)) begin = false;
		if (!begin) consumer.consume(chr);
		value <<= 4;
	}
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class CharactersPrinter : public CharactersConsumer
{
public:
	CharactersPrinter(
		int            x, 
		int            y, 
		const FontInfo &font, 
		const Color    &color, 
		const Color    *bg_color
	) : 
		x_(x), 
		y_(y), 
		font_(font), 
		color_(color), 
		bg_color_(bg_color)
	{}

	void consume(wchar_t character) override
	{
		if (character == ' ')
		{
			x_ += 2 * font_.spacePixels;
			return;
		}
		const FontCharInfo* char_info = display_find_char_info(font_, character);
		if (char_info == nullptr) char_info = display_find_char_info(font_, '?');
		if (char_info == nullptr) return;
		display_paint_character(
			x_, y_,
			&font_.data[char_info->offset],
			char_info->widthBits,
			font_.heightPages,
			color_,
			bg_color_
			);
		x_ += char_info->widthBits;
		if (bg_color_ && font_.spacePixels)
			display_fill_rect(
				x_, 
				y_, 
				x_ + font_.spacePixels - 1, 
				y_ + font_.heightPages - 1, 
				*bg_color_
			);
		x_ += font_.spacePixels;
	}

private:
	int x_;
	int y_;
	const FontInfo& font_;
	const Color &color_;
	const Color *bg_color_;
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class TextWidthCalculator : public CharactersConsumer
{
public:
	TextWidthCalculator(const FontInfo &font) : font_(font) {}

	void consume(wchar_t character) override
	{
		if (character == ' ')
		{
			width_ += 2 * font_.spacePixels;
			return;
		}
		const FontCharInfo* char_info = display_find_char_info(font_, character);
		if (char_info == nullptr) char_info = display_find_char_info(font_, '?');
		if (char_info == nullptr) return;
		width_ += char_info->widthBits + font_.spacePixels;
	}

	unsigned get_result() const
	{
		return width_;
	}

private:
	const FontInfo& font_;
	unsigned width_ = 0;
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void display_draw_rect(const Rect &rect, int16_t width, const Color &color)
{
	width--;
	display_fill_rect(rect.x1, rect.y1+width, rect.x1+width, rect.y2-width, color);
	display_fill_rect(rect.x2-width, rect.y1+width, rect.x2, rect.y2-width, color);
	display_fill_rect(rect.x1+width, rect.y1, rect.x2-width, rect.y1+width, color);
	display_fill_rect(rect.x1+width, rect.y2-width, rect.x2-width, rect.y2, color);
}


void display_draw_vertical_gradient(const Rect &rect, const Color &color1, const Color &color2)
{
	const uint16_t height = rect.y2 - rect.y1;
	for (int16_t y = rect.y1, i = 0; y <= rect.y2; y++, i++)
		display_fill_rect(rect.x1, y, rect.x2, y, Color::between(color1, color2, i, height));
}


void display_draw_horizontal_gradient(const Rect &rect, const Color &color1, const Color &color2)
{
	const uint16_t width = rect.x2 - rect.x1;
	for (int x = rect.x1, i = 0; x <= rect.x2; x++, i++)
		display_fill_rect(x, rect.y1, x, rect.y2, Color::between(color1, color2, i, width));
}


Size display_calc_text_size(const FontInfo &font, const CharactersProvider & provider)
{
	TextWidthCalculator calculator(font);
	provider.provide(calculator);
	return Size(calculator.get_result(), font.heightPages);
}


void display_print_text(
	int                      x, 
	int                      y, 
	const CharactersProvider &provider, 
	const FontInfo           &font, 
	const Color              &color, 
	const Color              *bg_color)
{
	CharactersPrinter printer(x, y, font, color, bg_color);
	provider.provide(printer);
}


void display_print_text_in_rect(
	const Rect               &rect, 
	HorizAlign               align, 
	const CharactersProvider &provider, 
	const FontInfo           &font, 
	const Color              &color, 
	const Color              *bg_color)
{
	const Size text_size = display_calc_text_size(font, provider);

	int16_t x1;
	int16_t y1 = (rect.y1 + rect.y2 - text_size.height) / 2;
	switch (align)
	{
	case HorizAlign::Left:
		x1 = rect.x1;
		break;

	case HorizAlign::Center:
		x1 = (rect.x1 + rect.x2 - text_size.width) / 2;
		break;

	case HorizAlign::Right:
		x1 = rect.x2 - text_size.width;
		break;
	}

	display_print_text(x1, y1, provider, font, color, bg_color);

	if (bg_color)
	{
		int16_t x2 = x1 + text_size.width - 1;
		int16_t y2 = y1 + text_size.height - 1;
		display_fill_rect(rect.x1, rect.y1, x1 - 1,  rect.y2, *bg_color);
		display_fill_rect(x2 + 1,  rect.y1, rect.x2, rect.y2, *bg_color);
		display_fill_rect(x1,      rect.y1, x2,      y1 - 1,  *bg_color);
		display_fill_rect(x1,      y2 + 1,  x2,      rect.y2, *bg_color);
	}
}

void display_paint_text(
	int            x, 
	int            y, 
	const wchar_t  *text, 
	const FontInfo &font, 
	const Color    &color, 
	const Color    *bg_color)
{
	TextCharactersProvider provider(text);
	display_print_text(x, y, provider, font, color, bg_color);
}


void display_paint_text_in_rect(
	const Rect     &rect, 
	HorizAlign     align, 
	const wchar_t  *text, 
	const FontInfo &font, 
	const Color    &color, 
	const Color    *bg_color)
{
	TextCharactersProvider provider(text);
	display_print_text_in_rect(rect, align, provider, font, color, bg_color);
}


Size display_get_text_size(const FontInfo &font, const wchar_t *text)
{
	TextCharactersProvider provider(text);
	return display_calc_text_size(font, provider);
}


void display_paint_integer(
	int            x, 
	int            y, 
	int            value, 
	int            pt_pos, 
	const FontInfo &font, 
	const Color    &color, 
	const Color    *bg_color)
{
	Integer10CharactersProvider provider(value, pt_pos);
	display_print_text(x, y, provider, font, color, bg_color);
}


void display_paint_integer16(
	int            x, 
	int            y, 
	uint32_t       value, 
	const FontInfo &font, 
	const Color    &color, 
	const Color    *bg_color)
{
	Integer16CharactersProvider provider(value);
	display_print_text(x, y, provider, font, color, bg_color);
}


void display_fill_rect(int x1, int y1, int x2, int y2, const Color &color)
{
	display_fill_rect(Rect(x1, y1, x2, y2), color);
}


void display_fill_triangle(int x1, int y1, int x2, int y2, int x3, int y3, const Color &color)
{
	if (x1 > x2) { swap(x1, x2); swap(y1, y2); }
	if (x2 > x3) { swap(x2, x3); swap(y2, y3); }
	if (x1 > x3) { swap(x1, x3); swap(y1, y3); }

	uint16_t dx1 = x2 - x1;
	int16_t dy1 = y2 - y1;
	uint16_t dy1_abs = dx1 ? (abs(dy1) % dx1) : 0;
	int16_t step1 = y2 > y1 ? 1 : -1;
	int16_t int1 = dx1 ? (dy1 / dx1) : 0;
	int16_t cnt_y1 = dx1 / 2;
	int16_t yy1 = y1;

	uint16_t dx2 = x3 - x1;
	int16_t dy2 = y3 - y1;
	uint16_t dy2_abs = dx2 ? (abs(dy2) % dx2) : 0;
	int16_t step2 = y3 > y1 ? 1 : -1;
	int16_t int2 = dx2 ? (dy2 / dx2) : 0;
	int16_t cnt_y2 = dx2 / 2;
	int16_t yy2 = y1;

	int16_t scr_width = display_get_width();

	for (int16_t x = x1; x <= x3; x++)
	{
		if (x == x2)
		{
			dx1 = x3 - x2;
			if (dx1 == 0) break;
			dy1 = y3 - y2;
			dy1_abs = abs(dy1) % dx1;
			step1 = y3 > y2 ? 1 : -1;
			int1 = dy1 / dx1;
			cnt_y1 = dx1 / 2;
			yy1 = y2;
		}

		if ((x >= 0) || (x < scr_width))
			display_fill_rect(x, min(yy1, yy2), x, max(yy1, yy2), color);

		cnt_y1 -= dy1_abs;
		if (cnt_y1 < 0)
		{
			cnt_y1 += dx1;
			yy1 += step1;
		}
		yy1 += int1;

		cnt_y2 -= dy2_abs;
		if (cnt_y2 < 0)
		{
			cnt_y2 += dx2;
			yy2 += step2;
		}
		yy2 += int2;
	}
}


void default_display_paint_character(
	int           x0, 
	int           y0, 
	const uint8_t *data, 
	uint8_t       width, 
	uint8_t       height, 
	const Color   &color, 
	const Color   *bg_color)
{
	const uint16_t w8 = (width + 7) / 8;
	int16_t y = y0;
	for (; height != 0; height--, y++)
	{
		int32_t x = x0;
		for (uint16_t i = 0; i < w8; i++)
		{
			uint8_t value = *data++;
			for (uint8_t j = 0; j < 8; j++)
			{
				if (value & 0x80) 
					display_set_point(x, y, color);
				else if (bg_color) 
					display_set_point(x, y, *bg_color);
				x++;
				value <<= 1;
			}
		}
	}
}


} // namespace muil
