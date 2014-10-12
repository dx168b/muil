#include "WidgetsForm.hpp"

SampleWidgetsForm::SampleWidgetsForm(const wchar_t *caption, const muil::FontInfo &font) 
	: muil::WidgetsForm(caption, font),
	updown_(2, 12)
{

}

void SampleWidgetsForm::visit_all_widgets(muil::IWidgetVisitor &visitor)
{
	visitor.visit(updown_, 5, 5, 50, 12);
	
}

void SampleWidgetsForm::widget_event(muil::EventType type, const muil::Widget *widget)
{

}
