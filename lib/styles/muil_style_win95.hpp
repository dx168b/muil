#ifndef MUIL_STYLE_WIN95_HPP_FILE_INCLUDED_
#define MUIL_STYLE_WIN95_HPP_FILE_INCLUDED_

#include "../muil_widgets.hpp"

namespace muil {

void win95_draw_button(const Rect &rect, Color color, ButtonStyle style, bool composite);
void win95_draw_checkbox_rect(const Rect &rect, Color color, ButtonStyle style);
void win95_draw_choice_rect(const Rect &rect, Color color, ButtonStyle style);
void win95_draw_indented_ctrl_rect(const Rect &rect, Color color, ButtonStyle style);
int win95_get_indented_ctrl_border();

};  // end 'namespace muil'

#define MUIL_IMPLEMENT_WIN95_STYLE_WIDGETS \
void muil::draw_button(const Rect &rect, Color color, ButtonStyle style, bool composite) { muil::win95_draw_button(rect, color, style, composite); } \
void muil::draw_checkbox_rect(const Rect &rect, Color color, ButtonStyle style) { muil::win95_draw_checkbox_rect(rect, color, style); } \
void muil::draw_choice_rect(const Rect &rect, Color color, ButtonStyle style) { muil::win95_draw_choice_rect(rect, color, style); } \
void muil::draw_indented_ctrl_rect(const Rect &rect, Color color, ButtonStyle style) { win95_draw_indented_ctrl_rect(rect, color, style); } \
int muil::get_indented_ctrl_border() { return muil::win95_get_indented_ctrl_border(); }

#endif
