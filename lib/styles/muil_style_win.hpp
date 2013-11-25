#ifndef MUIL_STYLE_WIN_HPP_FILE_INCLUDED_
#define MUIL_STYLE_WIN_HPP_FILE_INCLUDED_

#include "../muil_widgets.hpp"

namespace muil {

void win_draw_button(const Rect &rect, Color color, ButtonStyle style);
void win_draw_checkbox_rect(const Rect &rect, Color color, ButtonStyle style);
void win_draw_indented_ctrl_rect(const Rect &rect, Color color, ButtonStyle style);
int win_get_indented_ctrl_border();

};  // end 'namespace muil'

#define MUIL_IMPLEMENT_WIN_STYLE_WIDGETS \
void muil::draw_button(const Rect &rect, Color color, ButtonStyle style) { muil::win_draw_button(rect, color, style); } \
void muil::draw_checkbox_rect(const Rect &rect, Color color, ButtonStyle style) { muil::win_draw_checkbox_rect(rect, color, style); } \
void muil::draw_indented_ctrl_rect(const Rect &rect, Color color, ButtonStyle style) { win_draw_indented_ctrl_rect(rect, color, style); } \
int muil::get_indented_ctrl_border() { return muil::win_get_indented_ctrl_border(); }

#endif
