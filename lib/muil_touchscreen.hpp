#ifndef MUIL_TOUCHSCREEN_HPP_FILE_INCLUDED_
#define MUIL_TOUCHSCREEN_HPP_FILE_INCLUDED_

#include <stdint.h>

namespace muil {

struct FontInfo;

struct TouchScreenCalibrData
{
	const FontInfo *font;
	const wchar_t *text;
};

bool  touchscreen_is_pressed();
bool  touchscreen_get_pos(int16_t &x, int16_t &y);
void  touchscreen_calibrate(TouchScreenCalibrData &data);

} // namespace muil

#endif

