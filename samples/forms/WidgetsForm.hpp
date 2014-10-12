#ifndef WIDGETS_FORM_HPP_FILE_INCLUDED_
#define WIDGETS_FORM_HPP_FILE_INCLUDED_

#include "muil.hpp"

class SampleWidgetsForm : public muil::WidgetsForm
{
public:
	SampleWidgetsForm(const wchar_t *caption, const muil::FontInfo &font);

protected:
	void visit_all_widgets(muil::IWidgetVisitor &visitor) override;
	void widget_event(muil::EventType type, const muil::Widget *widget) override;

private:
	muil::UpDownWidget updown_;
};


#endif
