#ifndef MUIL_STYLE_VISTA_HPP_FILE_INCLUDED_
#define MUIL_STYLE_VISTA_HPP_FILE_INCLUDED_

#include "../muil_widgets_gui.hpp"

namespace muil {

void vista_draw_button(const Rect &rect, Color color, ButtonStyle style, bool composite);
void vista_draw_checkbox_rect(const Rect &rect, Color color, ButtonStyle style);
void vista_draw_choice_rect(const Rect &rect, Color color, ButtonStyle style);
void vista_draw_indented_ctrl_rect(const Rect &rect, Color color, ButtonStyle style);
int vista_get_indented_ctrl_border();

};  // end 'namespace muil'

#define MUIL_IMPLEMENT_VISTA_STYLE_WIDGETS \
void muil::draw_button(const Rect &rect, Color color, ButtonStyle style, bool composite) { muil::vista_draw_button(rect, color, style, composite); } \
void muil::draw_checkbox_rect(const Rect &rect, Color color, ButtonStyle style) { muil::vista_draw_checkbox_rect(rect, color, style); } \
void muil::draw_choice_rect(const Rect &rect, Color color, ButtonStyle style) { muil::vista_draw_choice_rect(rect, color, style); } \
void muil::draw_indented_ctrl_rect(const Rect &rect, Color color, ButtonStyle style) { vista_draw_indented_ctrl_rect(rect, color, style); } \
int muil::get_indented_ctrl_border() { return muil::vista_get_indented_ctrl_border(); }

#endif

