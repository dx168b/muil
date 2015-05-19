## A main form example for ILI9320/ADS7843 display and CortexM3 microcontroller ##

First, include necessary library headers:
```
// main library header
#include "muil/lib/muil_common.hpp"

// font, used for text
#include "muil/lib/fonts/tahoma14_ru.hpp"

// screen driver
#include "muil/lib/displays/ILI9320.hpp"

// touchscreen driver
#include "muil/lib/touchscreens/ADS7843.hpp"

// Pins and SPI library for LCD and Touchscreen
#include "muil/cortexm/pin.h"
#include "muil/cortexm/SPI.hpp"
```

And ask the compiler to use the `muil` namespace
```
using namespace muil;
```

Then write code for the main form:
```
const wchar_t *items_to_select[] = {L"One", L"Two", L"Three", NULL};

class MainForm : public WidgetsForm
{
public:
	MainForm() :
		WidgetsForm(L"Main form", &tahoma14ru_FontInfo),
		label_   (Point( 30,  45), L"Label"),
		test_btn_(Point( 30,  80), Size(180, 40), L"Test button"),
		up_down1_(Point( 20, 140), Size( 90, 40), 0, 42),
		up_down2_(Point(120, 140), Size( 90, 40), 2, 314),
		ch_box_  (Point( 30, 190), Size(180, 30), L"Checkbox", true),
		ch_      (Point( 30, 230), Size(180, 30), L"Select", items_to_select, 0)
	{
	}

	// Enumerate all controls for form
	void visit_all_widgets(IWidgetVisitor &visitor)
	{
		visitor.visit(label_);
		visitor.visit(test_btn_);
		visitor.visit(up_down1_);
		visitor.visit(up_down2_);
		visitor.visit(ch_box_);
		visitor.visit(ch_);
	}

	// widget_event is called for each event of every widget
	void widget_event(EventType type, const Widget *widget)
	{
		if ((type == EVENT_TOUCHSCREEN_UP) && (widget == &test_btn_))
		{
			// this code will be executed when "Test button" is pressed
		}
	}

private:
	// Our controls
	Label label_;
	Button test_btn_;
	UpDownWidget up_down1_;
	UpDownWidget up_down2_;
	CheckBox ch_box_;
	StringsChoice ch_;
};
```

Code for the application:
```
// A function that makes a delay for the specified number of milliseconds
void delay_ms_fun(uint16_t milliseconds)
{
	volatile uint32_t counter = milliseconds * 2000;
	while (counter) counter--;
}

// Class for application
// Application maintains one ore more screen forms and processes touch screen presses
class MyApp : public Application
{
	// Types for display driver
	typedef Pin<'A', 4> DisplayCSPin;
	typedef Pin<'A', 3> DisplayResetPin;
	typedef HardwareSPI0<DisplayCSPin> LCD_SPI; // hardware SPI used
	typedef ILI9320Display<LCD_SPI, DisplayResetPin> DisplayDriver;

	// Types for touchscreen driver
	typedef Pin<'B', 12> TS_CSPin;
	typedef Pin<'B', 13> TS_SCKPin;
	typedef Pin<'B', 14> TS_MISOPin;
	typedef Pin<'B', 15> TS_MOSIPin;
	typedef Pin<'C', 6> TS_IRQPin;
	typedef SoftwareSPI<3, TS_MOSIPin, TS_MISOPin, TS_SCKPin, TS_CSPin> TS_SPI; // software SPI for touch screen
	typedef ADS7843TouchScreen<TS_SPI, TS_IRQPin> TouchScreenDriver;

public:
	MyApp()
	{
		// Initialize pins and SPI for LCD
		DisplayCSPin::Mode(OUTPUT);
		DisplayCSPin::Off();
		DisplayResetPin::Mode(OUTPUT);
		LCD_SPI::init_pins(8, CS_Soft, CPHA_2Edge, CPOL_High);

		// Initialize pins and SPI for touch screen
		TS_SPI::init(8, CS_Soft, CPHA_1Edge, CPOL_Low);
		TS_MOSIPin::Mode(OUTPUT);
		TS_MOSIPin::Off();
		TS_MISOPin::Mode(INPUTPULLED);
		TS_MISOPin::PullUp();
		TS_SCKPin::Mode(OUTPUT);
		TS_SCKPin::Off();
		TS_CSPin::Mode(OUTPUT);
		TS_CSPin::On();
		TS_IRQPin::Mode(INPUTPULLED);
		TS_IRQPin::PullUp();

		// Reset and initialize display
		display_.init(delay_ms_fun);

		// Initialize touch screen
		touch_screen_.init();

		// Calibrate touch screen
		touch_screen_.calibrate(display_, &tahoma14ru_FontInfo, L"Press cross to calibrate", delay_ms_fun);
	}

	// returns the display object
	Display* get_display()
	{
		return &display_;
	}

	// returns the object for touch screen
	TouchScreen* get_touch_screen()
	{
		return &touch_screen_;
	}

	// makes a delay
	void delay_ms(uint16_t milliseconds)
	{
		delay_ms_fun(milliseconds);
	}

private:
	// Objects for LCD and touch screen
	DisplayDriver display_;
	TouchScreenDriver touch_screen_;
};
```

And run all of this
```
int main()
{
	MyApp app;
	MainForm main_form;

	main_form.show();
	app.run();

	return 0;
}
```

## Modal form example ##

Attach the code below to the main form example:
```
class ModalForm : public WidgetsForm
{
public:
	ModalForm() :
		WidgetsForm(L"Modal form", &tahoma14ru_FontInfo),
		btn_ok_    (Point(20, 50), Size(80, 40), L"OK", true),
		btn_cancel_(Point(120, 50), Size(100, 40), L"Cancel", false)
	{
	}

	void visit_all_widgets(IWidgetVisitor &visitor)
	{
		visitor.visit(btn_ok_);
		visitor.visit(btn_cancel_);
	}

	void widget_event(EventType type, const Widget *widget)
	{
		if (type == EVENT_TOUCHSCREEN_UP)
		{
			if (widget == &btn_ok_) set_modal_result(MR_OK);
			if (widget == &btn_cancel_) set_modal_result(MR_CANCEL);
		}
	}

private:
	Button btn_ok_;
	Button btn_cancel_;
};
```

and change `widget_event` handler of the main form to this:
```
	if ((type == EVENT_TOUCHSCREEN_UP) && (widget == &test_btn_))
	{
		ModalForm modal_form;
		modal_form.show_modal();
	}
```
Compile, upload, run and press "Test button" on the main form.