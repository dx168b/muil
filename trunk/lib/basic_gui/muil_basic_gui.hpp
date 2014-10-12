#ifndef MUIL_DISPLAY_HPP_FILE_INCLUDED_
#define MUIL_DISPLAY_HPP_FILE_INCLUDED_

#include <stdint.h>

namespace muil {

enum class HorizAlign
{
	Left,
	Center,
	Right,
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
	Color adjusted(int multiplier, int divider);
	static Color between(const Color &color1, const Color &color2, int16_t value, int16_t max);

	bool operator == (const Color &other) const
	{
		return (r == other.r) && (g == other.g) && (b == other.b);
	}

	bool operator != (const Color &other) const
	{
		return !operator == (other);
	}
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


class CharactersConsumer
{
public:
	virtual void consume(wchar_t character) = 0;
};


class CharactersProvider
{
public:
	virtual void provide(CharactersConsumer &consumer) const = 0;
};


class TextCharactersProvider : public CharactersProvider
{
public:
	explicit TextCharactersProvider(const wchar_t *text);
	void provide(CharactersConsumer &consumer) const override;

private:
	const wchar_t *text_;
};

class IntegerCharactersProvider : public CharactersProvider
{
public:
	IntegerCharactersProvider(int value) : value_(value) {}

	int get_value() const { return value_; }
	void set_value(int value) { value_ = value; }

protected:
	int value_;
};

class Integer10CharactersProvider : public IntegerCharactersProvider
{
public:
	explicit Integer10CharactersProvider(int value, int pt_pos);
	void provide(CharactersConsumer &consumer) const override;

private:
	int pt_pos_;
};


class Integer16CharactersProvider : public IntegerCharactersProvider
{
public:
	explicit Integer16CharactersProvider(unsigned value);
	void provide(CharactersConsumer &consumer) const override;
};


unsigned display_get_width();
unsigned display_get_height();
uint16_t display_get_dpi();
void     display_set_offset(int x, int y);
void     display_set_point(int x, int y, const Color &color);
void     display_fill_rect(const Rect &rect, const Color &color);
void     display_paint_character(int x0, int y0, const uint8_t *data, uint8_t width, uint8_t height, const Color &color, const Color *bg_color);

void     display_fill_rect(int x1, int y1, int x2, int y2, const Color &color);
void     display_draw_rect(const Rect &rect, int16_t width, const Color &color);
void     display_draw_vertical_gradient(const Rect &rect, const Color &color1, const Color &color2);
void     display_draw_horizontal_gradient(const Rect &rect, const Color &color1, const Color &color2);

Size     display_calc_text_size(const FontInfo &font, const CharactersProvider & provider);
void     display_print_text(int x, int y, const CharactersProvider & provider, const FontInfo &font, const Color &color, const Color *bg_color);
void     display_print_text_in_rect(const Rect &rect, HorizAlign align, const CharactersProvider & provider, const FontInfo &font, const Color &color, const Color *bg_color);

void     display_paint_text(int x, int y, const wchar_t *text, const FontInfo &font, const Color &color, const Color *bg_color);
void     display_paint_text_in_rect(const Rect &rect, HorizAlign align, const wchar_t *text, const FontInfo &font, const Color &color, const Color *bg_color);
void     display_paint_integer(int x, int y, int value, int pt_pos, const FontInfo &font, const Color &color, const Color *bg_color);
void     display_paint_integer16(int x, int y, uint32_t value, const FontInfo &font, const Color &color, const Color *bg_color);
void     display_fill_triangle(int x1, int y1, int x2, int y2, int x3, int y3, const Color &color);
Size     display_get_text_size(const FontInfo &font, const wchar_t *text);

void     default_display_paint_character(int x0, int y0, const uint8_t *data, uint8_t width, uint8_t height, const Color &color, const Color *bg_color);

} // namespace muil

#endif
