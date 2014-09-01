#include "MUIL-win32.hpp"
#include "../../lib/styles/muil_style_vista.hpp"
#include "../../lib/basic_gui/fonts/tahoma14_ru.hpp"
#include "../../samples/forms/PINForm.hpp"

MUIL_IMPLEMENT_VISTA_STYLE_WIDGETS

void show_pin_form()
{
	wchar_t pin[5];
	PINForm pin_form(L"Enter the PIN", &muil::tahoma14ru_FontInfo, pin, 4);
	pin_form.show_modal();
}

int main()
{
	init_for_win32();
	muil::Application app;
	show_pin_form();
	app.run();
}