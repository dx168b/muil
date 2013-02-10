#ifndef PIN_HPP_FILE_INCLUDED_
#define PIN_HPP_FILE_INCLUDED_

/* Examples:

  // Declare InpPin as 0th pin of port B:
  typedef Pin<'B', 0> InpPin;

  // Declare LED1Pin as inverted first pin of port B:
  typedef Pin<'B', 1, 'L'> LED1Pin;

  // Declare LED2Pin as inverted second pin of port B:
  typedef Pin<'B', 2> LED2Pin;

  // Configure InpPin as input with pulled-up resistor
  InpPin::ConfInPulledUp();

  // Configure LED1Pin and LED2Pin as output pins
  LED1Pin::ConfOut();
  LED2Pin::ConfOut();

  // Read pin state from InpPin
  bool pin_state = InpPin::Signaled();

  // Turn on LED1Pin
  LED1Pin::On();

  // Turn off LED2Pin
  LED1Pin::Off();
*/

template<char port, int pin_no, char active_state = 'H'> class Pin {};
template<char port, int pin_no> class PinBase {};

#define PIN_LIB_PORT_SPEC_MACRO(LETTER, DIR, OUT, IN)             \
template<int pin_no>                                              \
class PinBase<LETTER, pin_no>                                     \
{                                                                 \
public:                                                           \
	enum { mask = 1 << pin_no };                                  \
	static void ConfIn() { DIR &= ~mask; }                        \
	static void ConfInPulledUp() { DIR &= ~mask; OUT |= mask; }   \
	static void ConfOut() { DIR |= mask; }                        \
	static void Cpl() { OUT ^= mask; }                            \
};                                                                \
template<int pin_no>                                              \
class Pin<LETTER, pin_no, 'H'> : public PinBase<LETTER, pin_no> { \
public:                                                           \
	using PinBase<LETTER, pin_no>::mask;                          \
	static void On() { OUT |= mask; }                             \
	static void Off() { OUT &= ~mask; }                           \
	static bool Signalled() { return IN & mask; }                 \
	static bool Latched() { return OUT & mask; }                  \
};                                                                \
template<int pin_no>                                              \
class Pin<LETTER, pin_no, 'L'> : public PinBase<LETTER, pin_no> { \
public:                                                           \
	using PinBase<LETTER, pin_no>::mask;                          \
	static void On() { OUT &= ~mask; }                            \
	static void Off() { OUT |= mask; }                            \
	static bool Signalled() { return !(IN & mask); }              \
	static bool Latched() { return !(OUT & mask); }               \
};

#ifdef PORTA
	PIN_LIB_PORT_SPEC_MACRO('A', DDRA, PORTA, PINA)
#endif
#ifdef PORTB
	PIN_LIB_PORT_SPEC_MACRO('B', DDRB, PORTB, PINB)
#endif
#ifdef PORTC
	PIN_LIB_PORT_SPEC_MACRO('C', DDRC, PORTC, PINC)
#endif
#ifdef PORTD
	PIN_LIB_PORT_SPEC_MACRO('D', DDRD, PORTD, PIND)
#endif
#ifdef PORTE
	PIN_LIB_PORT_SPEC_MACRO('E', DDRE, PORTE, PINE)
#endif
#ifdef PORTF
	PIN_LIB_PORT_SPEC_MACRO('F', DDRF, PORTF, PINF)
#endif
#ifdef PORTG
	PIN_LIB_PORT_SPEC_MACRO('G', DDRG, PORTG, PING)
#endif
#ifdef PORTH
	PIN_LIB_PORT_SPEC_MACRO('H', DDRH, PORTH, PINH)
#endif
#ifdef PORTI
	PIN_LIB_PORT_SPEC_MACRO('I', DDRI, PORTI, PINI)
#endif
#ifdef PORTJ
	PIN_LIB_PORT_SPEC_MACRO('J', DDRJ, PORTJ, PINJ)
#endif

#undef PIN_LIB_PORT_SPEC_MACRO

#endif
