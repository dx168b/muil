#ifndef MUIL_DISPLAY_HPP_FILE_INCLUDED_
#define MUIL_DISPLAY_HPP_FILE_INCLUDED_

#include "muil_classes.hpp"

namespace muil {

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

} // namespace muil

#endif
