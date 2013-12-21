#ifndef MUIL_TOUCHSCREEN_HPP_FILE_INCLUDED_
#define MUIL_TOUCHSCREEN_HPP_FILE_INCLUDED_

#include "muil_classes.hpp"

namespace muil {

struct FontInfo;

struct TouchScreenCalibrData
{
	const FontInfo *font;
	const wchar_t *text;
};

bool  touchscreen_is_pressed();
Point touchscreen_get_pos();
void  touchscreen_calibrate(TouchScreenCalibrData &data);

} // namespace muil

#endif

