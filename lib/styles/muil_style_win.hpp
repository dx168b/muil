#ifndef MUIL_STYLE_WIN_HPP_FILE_INCLUDED_
#define MUIL_STYLE_WIN_HPP_FILE_INCLUDED_

#include "../muil_widgets.hpp"

namespace muil {

void win_draw_button(const Rect &rect, Color color, ButtonStyle style);


};  // end 'namespace muil'

#define MUIL_IMPLEMENT_WIN_STYLE_WIDGETS \
void muil::draw_button(const Rect &rect, Color color, ButtonStyle style) { muil::win_draw_button(rect, color, style); }

#endif
