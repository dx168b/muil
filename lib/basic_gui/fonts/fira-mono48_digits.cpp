#include "fira-mono48_digits.hpp"

namespace muil {

//
//  Font data for Fira Mono 48pt
//

// Character bitmaps for Fira Mono 48pt
const uint8_t firaMono48ptBitmaps[] =
{
	// @0 '+' (24 pixels wide)
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7E, 0x00, 0x00, 0x7E, 0x00, 0x00, 0x7E, 0x00, 0x00, 0x7E, 0x00, 0x00, 0x7E, 0x00, 0x00, 0x7E, 0x00, 0x00, 0x7E, 0x00, 0x00, 0x7E, 0x00, 0x00, 0x7E, 0x00, 0x00, 0x7E, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x7E, 0x00, 0x00, 0x7E, 0x00, 0x00, 0x7E, 0x00, 0x00, 0x7E, 0x00, 0x00, 0x7E, 0x00, 0x00, 0x7E, 0x00, 0x00, 0x7E, 0x00, 0x00, 0x7E, 0x00, 0x00, 0x7E, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,

	// @138 '-' (23 pixels wide)
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFE, 0xFF, 0xFF, 0xFE, 0xFF, 0xFF, 0xFE, 0xFF, 0xFF, 0xFE, 0xFF, 0xFF, 0xFE, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,

	// @276 '.' (12 pixels wide)
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0F, 0x80, 0x3F, 0xC0, 0x7F, 0xE0, 0x7F, 0xE0, 0x7F, 0xF0, 0xFF, 0xF0, 0xFF, 0xF0, 0x7F, 0xF0, 0x7F, 0xE0, 0x7F, 0xE0, 0x3F, 0xC0, 0x0F, 0x80,

	// @368 '0' (29 pixels wide)
	0x00, 0x3F, 0xC0, 0x00, 0x00, 0xFF, 0xF8, 0x00, 0x03, 0xFF, 0xFC, 0x00, 0x07, 0xFF, 0xFE, 0x00, 0x0F, 0xFF, 0xFF, 0x00, 0x0F, 0xF0, 0x7F, 0x80, 0x1F, 0xC0, 0x3F, 0x80, 0x3F, 0x80, 0x1F, 0xC0, 0x3F, 0x00, 0x0F, 0xC0, 0x3F, 0x00, 0x07, 0xE0, 0x7E, 0x00, 0x07, 0xE0, 0x7E, 0x00, 0x07, 0xE0, 0x7E, 0x00, 0x03, 0xF0, 0xFE, 0x00, 0x03, 0xF0, 0xFC, 0x00, 0x03, 0xF0, 0xFC, 0x00, 0x03, 0xF0, 0xFC, 0x00, 0x03, 0xF0, 0xFC, 0x00, 0x01, 0xF8, 0xFC, 0x00, 0x01, 0xF8, 0xFC, 0x0F, 0x81, 0xF8, 0xFC, 0x1F, 0xC1, 0xF8, 0xFC, 0x3F, 0xC1, 0xF8, 0xFC, 0x3F, 0xC1, 0xF8, 0xFC, 0x3F, 0xC1, 0xF8, 0xFC, 0x3F, 0xC1, 0xF8, 0xFC, 0x1F, 0xC1, 0xF8, 0xFC, 0x0F, 0x81, 0xF8, 0xFC, 0x00, 0x01, 0xF8, 0xFC, 0x00, 0x03, 0xF8, 0xFC, 0x00, 0x03, 0xF0, 0xFC, 0x00, 0x03, 0xF0, 0xFC, 0x00, 0x03, 0xF0, 0xFE, 0x00, 0x03, 0xF0, 0x7E, 0x00, 0x03, 0xF0, 0x7E, 0x00, 0x07, 0xE0, 0x7E, 0x00, 0x07, 0xE0, 0x3F, 0x00, 0x07, 0xE0, 0x3F, 0x00, 0x0F, 0xC0, 0x3F, 0x80, 0x1F, 0xC0, 0x1F, 0xC0, 0x3F, 0x80, 0x0F, 0xF0, 0x7F, 0x80, 0x0F, 0xFF, 0xFF, 0x00, 0x07, 0xFF, 0xFE, 0x00, 0x03, 0xFF, 0xFC, 0x00, 0x00, 0xFF, 0xF8, 0x00, 0x00, 0x3F, 0xC0, 0x00,

	// @552 '1' (28 pixels wide)
	0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0xE0, 0x00, 0x00, 0x1F, 0xE0, 0x00, 0x00, 0x3F, 0xE0, 0x00, 0x00, 0xFF, 0xE0, 0x00, 0x03, 0xFF, 0xE0, 0x00, 0x07, 0xFF, 0xE0, 0x00, 0x1F, 0xFF, 0xE0, 0x00, 0x3F, 0xE7, 0xE0, 0x00, 0xFF, 0xC7, 0xE0, 0x00, 0xFF, 0x07, 0xE0, 0x00, 0x7C, 0x07, 0xE0, 0x00, 0x38, 0x07, 0xE0, 0x00, 0x20, 0x07, 0xE0, 0x00, 0x00, 0x07, 0xE0, 0x00, 0x00, 0x07, 0xE0, 0x00, 0x00, 0x07, 0xE0, 0x00, 0x00, 0x07, 0xE0, 0x00, 0x00, 0x07, 0xE0, 0x00, 0x00, 0x07, 0xE0, 0x00, 0x00, 0x07, 0xE0, 0x00, 0x00, 0x07, 0xE0, 0x00, 0x00, 0x07, 0xE0, 0x00, 0x00, 0x07, 0xE0, 0x00, 0x00, 0x07, 0xE0, 0x00, 0x00, 0x07, 0xE0, 0x00, 0x00, 0x07, 0xE0, 0x00, 0x00, 0x07, 0xE0, 0x00, 0x00, 0x07, 0xE0, 0x00, 0x00, 0x07, 0xE0, 0x00, 0x00, 0x07, 0xE0, 0x00, 0x00, 0x07, 0xE0, 0x00, 0x00, 0x07, 0xE0, 0x00, 0x00, 0x07, 0xE0, 0x00, 0x00, 0x07, 0xE0, 0x00, 0x00, 0x07, 0xE0, 0x00, 0x00, 0x07, 0xE0, 0x00, 0x00, 0x07, 0xE0, 0x00, 0x00, 0x07, 0xE0, 0x00, 0x00, 0x07, 0xE0, 0x00, 0x3F, 0xFF, 0xFF, 0xF0, 0x3F, 0xFF, 0xFF, 0xF0, 0x3F, 0xFF, 0xFF, 0xF0, 0x3F, 0xFF, 0xFF, 0xF0, 0x3F, 0xFF, 0xFF, 0xF0, 0x00, 0x00, 0x00, 0x00,

	// @736 '2' (27 pixels wide)
	0x00, 0x7F, 0x80, 0x00, 0x03, 0xFF, 0xF0, 0x00, 0x0F, 0xFF, 0xFC, 0x00, 0x1F, 0xFF, 0xFE, 0x00, 0x3F, 0xFF, 0xFF, 0x00, 0x7F, 0x80, 0xFF, 0x00, 0xFE, 0x00, 0x3F, 0x80, 0x7C, 0x00, 0x1F, 0x80, 0x38, 0x00, 0x1F, 0xC0, 0x10, 0x00, 0x0F, 0xC0, 0x00, 0x00, 0x0F, 0xC0, 0x00, 0x00, 0x0F, 0xC0, 0x00, 0x00, 0x0F, 0xC0, 0x00, 0x00, 0x0F, 0xC0, 0x00, 0x00, 0x0F, 0xC0, 0x00, 0x00, 0x0F, 0xC0, 0x00, 0x00, 0x0F, 0xC0, 0x00, 0x00, 0x1F, 0x80, 0x00, 0x00, 0x1F, 0x80, 0x00, 0x00, 0x3F, 0x80, 0x00, 0x00, 0x3F, 0x00, 0x00, 0x00, 0x7E, 0x00, 0x00, 0x00, 0xFE, 0x00, 0x00, 0x00, 0xFC, 0x00, 0x00, 0x01, 0xFC, 0x00, 0x00, 0x03, 0xF8, 0x00, 0x00, 0x07, 0xF0, 0x00, 0x00, 0x07, 0xE0, 0x00, 0x00, 0x0F, 0xC0, 0x00, 0x00, 0x1F, 0xC0, 0x00, 0x00, 0x3F, 0x80, 0x00, 0x00, 0x7F, 0x00, 0x00, 0x00, 0xFE, 0x00, 0x00, 0x01, 0xFC, 0x00, 0x00, 0x03, 0xF8, 0x00, 0x00, 0x07, 0xF0, 0x00, 0x00, 0x0F, 0xE0, 0x00, 0x00, 0x1F, 0xC0, 0x00, 0x00, 0x3F, 0x80, 0x00, 0x00, 0x7F, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xE0, 0xFF, 0xFF, 0xFF, 0xE0, 0xFF, 0xFF, 0xFF, 0xE0, 0xFF, 0xFF, 0xFF, 0xE0, 0xFF, 0xFF, 0xFF, 0xE0, 0x00, 0x00, 0x00, 0x00,

	// @920 '3' (29 pixels wide)
	0x00, 0x3F, 0xC0, 0x00, 0x01, 0xFF, 0xF8, 0x00, 0x07, 0xFF, 0xFE, 0x00, 0x1F, 0xFF, 0xFF, 0x00, 0x3F, 0xFF, 0xFF, 0x80, 0x7F, 0xC0, 0x7F, 0xC0, 0x3F, 0x00, 0x1F, 0xC0, 0x1C, 0x00, 0x0F, 0xE0, 0x08, 0x00, 0x07, 0xE0, 0x00, 0x00, 0x07, 0xE0, 0x00, 0x00, 0x03, 0xF0, 0x00, 0x00, 0x03, 0xF0, 0x00, 0x00, 0x03, 0xE0, 0x00, 0x00, 0x03, 0xE0, 0x00, 0x00, 0x07, 0xE0, 0x00, 0x00, 0x07, 0xC0, 0x00, 0x00, 0x0F, 0xC0, 0x00, 0x00, 0x1F, 0x80, 0x00, 0x00, 0x7F, 0x00, 0x00, 0x1F, 0xFE, 0x00, 0x00, 0x1F, 0xF8, 0x00, 0x00, 0x1F, 0xF0, 0x00, 0x00, 0x1F, 0xFE, 0x00, 0x00, 0x1F, 0xFF, 0x80, 0x00, 0x20, 0x3F, 0xC0, 0x00, 0x00, 0x0F, 0xE0, 0x00, 0x00, 0x07, 0xF0, 0x00, 0x00, 0x03, 0xF0, 0x00, 0x00, 0x03, 0xF0, 0x00, 0x00, 0x01, 0xF8, 0x00, 0x00, 0x01, 0xF8, 0x00, 0x00, 0x01, 0xF8, 0x00, 0x00, 0x01, 0xF8, 0x00, 0x00, 0x01, 0xF8, 0x00, 0x00, 0x01, 0xF8, 0x00, 0x00, 0x03, 0xF8, 0x00, 0x00, 0x03, 0xF0, 0x10, 0x00, 0x07, 0xF0, 0x78, 0x00, 0x0F, 0xE0, 0xFE, 0x00, 0x1F, 0xE0, 0xFF, 0xC0, 0x7F, 0xC0, 0x7F, 0xFF, 0xFF, 0x80, 0x3F, 0xFF, 0xFF, 0x00, 0x0F, 0xFF, 0xFE, 0x00, 0x03, 0xFF, 0xF8, 0x00, 0x00, 0x7F, 0xC0, 0x00,

	// @1104 '4' (30 pixels wide)
	0x00, 0x06, 0x00, 0x00, 0x00, 0x07, 0x80, 0x00, 0x00, 0x0F, 0xC0, 0x00, 0x00, 0x0F, 0xC0, 0x00, 0x00, 0x1F, 0x80, 0x00, 0x00, 0x1F, 0x80, 0x00, 0x00, 0x1F, 0x00, 0x00, 0x00, 0x3F, 0x00, 0x00, 0x00, 0x3F, 0x00, 0x00, 0x00, 0x7E, 0x00, 0x00, 0x00, 0x7E, 0x00, 0x00, 0x00, 0x7C, 0x00, 0x00, 0x00, 0xFC, 0x00, 0x00, 0x00, 0xFC, 0x00, 0x00, 0x01, 0xF8, 0x00, 0x00, 0x01, 0xF8, 0x00, 0x00, 0x03, 0xF0, 0x00, 0x00, 0x03, 0xF0, 0x00, 0x00, 0x03, 0xE0, 0x1F, 0x00, 0x07, 0xE0, 0x1F, 0x00, 0x07, 0xE0, 0x1F, 0x00, 0x0F, 0xC0, 0x1F, 0x00, 0x0F, 0xC0, 0x1F, 0x00, 0x1F, 0x80, 0x1F, 0x00, 0x1F, 0x80, 0x1F, 0x00, 0x1F, 0x80, 0x1F, 0x00, 0x3F, 0x00, 0x1F, 0x00, 0x3F, 0x00, 0x1F, 0x00, 0x7E, 0x00, 0x1F, 0x00, 0x7E, 0x00, 0x3F, 0x00, 0x7F, 0xFF, 0xFF, 0xFC, 0xFF, 0xFF, 0xFF, 0xFC, 0xFF, 0xFF, 0xFF, 0xFC, 0xFF, 0xFF, 0xFF, 0xFC, 0xFF, 0xFF, 0xFF, 0xFC, 0x00, 0x00, 0x3F, 0x00, 0x00, 0x00, 0x3F, 0x00, 0x00, 0x00, 0x3F, 0x00, 0x00, 0x00, 0x3F, 0x00, 0x00, 0x00, 0x3F, 0x00, 0x00, 0x00, 0x3F, 0x00, 0x00, 0x00, 0x3F, 0x00, 0x00, 0x00, 0x3F, 0x00, 0x00, 0x00, 0x3F, 0x00, 0x00, 0x00, 0x3F, 0x00, 0x00, 0x00, 0x00, 0x00,

	// @1288 '5' (28 pixels wide)
	0x00, 0x00, 0x00, 0x00, 0x1F, 0xFF, 0xFF, 0xC0, 0x1F, 0xFF, 0xFF, 0xC0, 0x1F, 0xFF, 0xFF, 0xC0, 0x1F, 0xFF, 0xFF, 0xC0, 0x1F, 0xFF, 0xFF, 0xC0, 0x1F, 0x80, 0x00, 0x00, 0x1F, 0x80, 0x00, 0x00, 0x1F, 0x80, 0x00, 0x00, 0x1F, 0x80, 0x00, 0x00, 0x1F, 0x80, 0x00, 0x00, 0x1F, 0x80, 0x00, 0x00, 0x1F, 0x80, 0x00, 0x00, 0x1F, 0x80, 0x00, 0x00, 0x1F, 0x80, 0x00, 0x00, 0x1F, 0x80, 0x00, 0x00, 0x1F, 0x80, 0x00, 0x00, 0x1F, 0x8F, 0xE0, 0x00, 0x1F, 0xFF, 0xFC, 0x00, 0x1F, 0xFF, 0xFE, 0x00, 0x1F, 0xFF, 0xFF, 0x00, 0x1F, 0xFF, 0xFF, 0x80, 0x1F, 0xC0, 0x7F, 0xC0, 0x00, 0x00, 0x1F, 0xE0, 0x00, 0x00, 0x0F, 0xE0, 0x00, 0x00, 0x07, 0xE0, 0x00, 0x00, 0x07, 0xF0, 0x00, 0x00, 0x03, 0xF0, 0x00, 0x00, 0x03, 0xF0, 0x00, 0x00, 0x03, 0xF0, 0x00, 0x00, 0x03, 0xF0, 0x00, 0x00, 0x03, 0xF0, 0x00, 0x00, 0x03, 0xF0, 0x00, 0x00, 0x03, 0xF0, 0x00, 0x00, 0x03, 0xF0, 0x00, 0x00, 0x07, 0xF0, 0x00, 0x00, 0x07, 0xE0, 0x18, 0x00, 0x0F, 0xE0, 0x3C, 0x00, 0x1F, 0xC0, 0x7E, 0x00, 0x3F, 0xC0, 0xFF, 0x80, 0xFF, 0x80, 0x7F, 0xFF, 0xFF, 0x00, 0x3F, 0xFF, 0xFE, 0x00, 0x0F, 0xFF, 0xFC, 0x00, 0x03, 0xFF, 0xF0, 0x00, 0x00, 0x7F, 0x80, 0x00,

	// @1472 '6' (29 pixels wide)
	0x00, 0x07, 0xF8, 0x00, 0x00, 0x1F, 0xFF, 0x00, 0x00, 0x7F, 0xFF, 0x80, 0x00, 0xFF, 0xFF, 0xC0, 0x03, 0xFF, 0xFF, 0x80, 0x03, 0xFC, 0x0F, 0x00, 0x07, 0xF0, 0x02, 0x00, 0x0F, 0xE0, 0x00, 0x00, 0x0F, 0xC0, 0x00, 0x00, 0x1F, 0x80, 0x00, 0x00, 0x1F, 0x00, 0x00, 0x00, 0x3F, 0x00, 0x00, 0x00, 0x3E, 0x00, 0x00, 0x00, 0x7E, 0x00, 0x00, 0x00, 0x7E, 0x00, 0x00, 0x00, 0x7C, 0x00, 0x00, 0x00, 0x7C, 0x07, 0xF0, 0x00, 0xFC, 0x1F, 0xFC, 0x00, 0xFC, 0x7F, 0xFF, 0x00, 0xFC, 0xFF, 0xFF, 0x80, 0xFD, 0xFF, 0xFF, 0xC0, 0xFF, 0xF0, 0x3F, 0xC0, 0xFF, 0xC0, 0x0F, 0xE0, 0xFF, 0x80, 0x07, 0xE0, 0xFF, 0x00, 0x07, 0xF0, 0xFE, 0x00, 0x03, 0xF0, 0xFC, 0x00, 0x03, 0xF0, 0xFC, 0x00, 0x03, 0xF0, 0xFC, 0x00, 0x01, 0xF8, 0xFC, 0x00, 0x01, 0xF8, 0xFC, 0x00, 0x01, 0xF8, 0xFC, 0x00, 0x01, 0xF8, 0xFC, 0x00, 0x01, 0xF8, 0x7C, 0x00, 0x03, 0xF0, 0x7E, 0x00, 0x03, 0xF0, 0x7E, 0x00, 0x03, 0xF0, 0x7F, 0x00, 0x03, 0xF0, 0x3F, 0x00, 0x07, 0xE0, 0x3F, 0x80, 0x07, 0xE0, 0x1F, 0xC0, 0x0F, 0xC0, 0x0F, 0xF0, 0x3F, 0xC0, 0x0F, 0xFF, 0xFF, 0x80, 0x07, 0xFF, 0xFF, 0x00, 0x01, 0xFF, 0xFE, 0x00, 0x00, 0xFF, 0xF8, 0x00, 0x00, 0x1F, 0xE0, 0x00,

	// @1656 '7' (27 pixels wide)
	0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xE0, 0xFF, 0xFF, 0xFF, 0xE0, 0xFF, 0xFF, 0xFF, 0xE0, 0xFF, 0xFF, 0xFF, 0xE0, 0xFF, 0xFF, 0xFF, 0xC0, 0x00, 0x00, 0x0F, 0xC0, 0x00, 0x00, 0x0F, 0xC0, 0x00, 0x00, 0x1F, 0x80, 0x00, 0x00, 0x1F, 0x80, 0x00, 0x00, 0x1F, 0x00, 0x00, 0x00, 0x3F, 0x00, 0x00, 0x00, 0x3E, 0x00, 0x00, 0x00, 0x7E, 0x00, 0x00, 0x00, 0x7E, 0x00, 0x00, 0x00, 0xFC, 0x00, 0x00, 0x00, 0xFC, 0x00, 0x00, 0x01, 0xF8, 0x00, 0x00, 0x01, 0xF8, 0x00, 0x00, 0x01, 0xF0, 0x00, 0x00, 0x03, 0xF0, 0x00, 0x00, 0x03, 0xF0, 0x00, 0x00, 0x07, 0xE0, 0x00, 0x00, 0x07, 0xE0, 0x00, 0x00, 0x0F, 0xC0, 0x00, 0x00, 0x0F, 0xC0, 0x00, 0x00, 0x1F, 0x80, 0x00, 0x00, 0x1F, 0x80, 0x00, 0x00, 0x1F, 0x80, 0x00, 0x00, 0x3F, 0x00, 0x00, 0x00, 0x3F, 0x00, 0x00, 0x00, 0x7E, 0x00, 0x00, 0x00, 0x7E, 0x00, 0x00, 0x00, 0xFC, 0x00, 0x00, 0x00, 0xFC, 0x00, 0x00, 0x01, 0xFC, 0x00, 0x00, 0x01, 0xF8, 0x00, 0x00, 0x01, 0xF8, 0x00, 0x00, 0x03, 0xF0, 0x00, 0x00, 0x03, 0xF0, 0x00, 0x00, 0x07, 0xE0, 0x00, 0x00, 0x07, 0xE0, 0x00, 0x00, 0x0F, 0xC0, 0x00, 0x00, 0x0F, 0xC0, 0x00, 0x00, 0x1F, 0xC0, 0x00, 0x00, 0x01, 0x80, 0x00, 0x00,

	// @1840 '8' (30 pixels wide)
	0x00, 0x1F, 0xE0, 0x00, 0x00, 0xFF, 0xFC, 0x00, 0x03, 0xFF, 0xFF, 0x00, 0x07, 0xFF, 0xFF, 0x80, 0x0F, 0xFF, 0xFF, 0xC0, 0x1F, 0xF0, 0x1F, 0xE0, 0x1F, 0xC0, 0x0F, 0xF0, 0x3F, 0x80, 0x07, 0xF0, 0x3F, 0x00, 0x03, 0xF0, 0x3F, 0x00, 0x01, 0xF0, 0x3F, 0x00, 0x01, 0xF8, 0x3F, 0x00, 0x01, 0xF8, 0x3F, 0x00, 0x01, 0xF0, 0x3F, 0x00, 0x01, 0xF0, 0x3F, 0x00, 0x03, 0xF0, 0x1F, 0x80, 0x03, 0xE0, 0x1F, 0xC0, 0x07, 0xE0, 0x0F, 0xF0, 0x0F, 0xC0, 0x0F, 0xFC, 0x1F, 0x80, 0x07, 0xFF, 0x7F, 0x00, 0x03, 0xFF, 0xFC, 0x00, 0x00, 0xFF, 0xF8, 0x00, 0x00, 0xFF, 0xFE, 0x00, 0x03, 0xFF, 0xFF, 0x80, 0x07, 0xE1, 0xFF, 0xC0, 0x0F, 0xC0, 0x7F, 0xE0, 0x1F, 0x80, 0x0F, 0xF0, 0x3F, 0x00, 0x07, 0xF8, 0x7E, 0x00, 0x03, 0xF8, 0x7E, 0x00, 0x01, 0xFC, 0xFE, 0x00, 0x00, 0xFC, 0xFC, 0x00, 0x00, 0xFC, 0xFC, 0x00, 0x00, 0xFC, 0xFC, 0x00, 0x00, 0xFC, 0xFC, 0x00, 0x00, 0xFC, 0xFC, 0x00, 0x00, 0xFC, 0xFE, 0x00, 0x00, 0xFC, 0x7E, 0x00, 0x01, 0xFC, 0x7F, 0x00, 0x03, 0xF8, 0x3F, 0x80, 0x07, 0xF8, 0x3F, 0xE0, 0x1F, 0xF0, 0x1F, 0xFF, 0xFF, 0xE0, 0x0F, 0xFF, 0xFF, 0xC0, 0x07, 0xFF, 0xFF, 0x80, 0x01, 0xFF, 0xFE, 0x00, 0x00, 0x3F, 0xF0, 0x00,

	// @2024 '9' (29 pixels wide)
	0x00, 0x3F, 0xC0, 0x00, 0x00, 0xFF, 0xF8, 0x00, 0x03, 0xFF, 0xFE, 0x00, 0x0F, 0xFF, 0xFF, 0x00, 0x1F, 0xFF, 0xFF, 0x80, 0x1F, 0xE0, 0x7F, 0xC0, 0x3F, 0x80, 0x1F, 0xC0, 0x7F, 0x00, 0x0F, 0xE0, 0x7E, 0x00, 0x07, 0xE0, 0xFC, 0x00, 0x07, 0xF0, 0xFC, 0x00, 0x03, 0xF0, 0xFC, 0x00, 0x03, 0xF0, 0xFC, 0x00, 0x03, 0xF0, 0xFC, 0x00, 0x03, 0xF8, 0xFC, 0x00, 0x03, 0xF8, 0xFC, 0x00, 0x01, 0xF8, 0xFC, 0x00, 0x01, 0xF8, 0xFC, 0x00, 0x01, 0xF8, 0xFC, 0x00, 0x03, 0xF8, 0xFC, 0x00, 0x03, 0xF8, 0xFE, 0x00, 0x07, 0xF8, 0x7F, 0x00, 0x0F, 0xF8, 0x7F, 0x80, 0x1F, 0xF0, 0x3F, 0xC0, 0x7F, 0xF0, 0x1F, 0xFF, 0xFB, 0xF0, 0x0F, 0xFF, 0xFB, 0xF0, 0x07, 0xFF, 0xF3, 0xF0, 0x03, 0xFF, 0xC3, 0xE0, 0x00, 0x7F, 0x07, 0xE0, 0x00, 0x00, 0x07, 0xE0, 0x00, 0x00, 0x0F, 0xC0, 0x00, 0x00, 0x0F, 0xC0, 0x00, 0x00, 0x1F, 0x80, 0x00, 0x00, 0x3F, 0x80, 0x00, 0x00, 0x7F, 0x00, 0x00, 0x00, 0xFE, 0x00, 0x00, 0x01, 0xFE, 0x00, 0x00, 0x07, 0xFC, 0x00, 0x00, 0x1F, 0xF0, 0x00, 0x00, 0x7F, 0xE0, 0x00, 0x01, 0xFF, 0xC0, 0x00, 0x0F, 0xFF, 0x00, 0x00, 0x1F, 0xFC, 0x00, 0x00, 0x0F, 0xF0, 0x00, 0x00, 0x0F, 0x80, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00,
};

// Character descriptors for Fira Mono 48pt
// { [Char width in bits], [Offset into firaMono48ptCharBitmaps in bytes] }
const FontCharInfo firaMono48ptDescriptors[] =
{
	{L'+', 24, 0}, 		// +
	{L'-', 23, 138}, 		// -
	{L'.', 12, 276}, 		// .
	{L'0', 29, 368}, 		// 0
	{L'1', 28, 552}, 		// 1
	{L'2', 27, 736}, 		// 2
	{L'3', 29, 920}, 		// 3
	{L'4', 30, 1104}, 		// 4
	{L'5', 28, 1288}, 		// 5
	{L'6', 29, 1472}, 		// 6
	{L'7', 27, 1656}, 		// 7
	{L'8', 30, 1840}, 		// 8
	{L'9', 29, 2024}, 		// 9
};

// Font information for Fira Mono 48pt
const FontInfo firaMono48ptFontInfo =
{
	46, //  Character height
	2, //  Width, in pixels, of space character
	firaMono48ptDescriptors, //  Character descriptor array
	firaMono48ptBitmaps, //  Character bitmap array
};

} // namespace muil
