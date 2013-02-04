#ifndef PIN_HPP_FILE_INCLUDED_
#define PIN_HPP_FILE_INCLUDED_

template<char port, int pin_no, char active_state = 'H'> class Pin {};

#define PIN_LIB_PORT_SPEC_MACRO(LETTER, DIR, OUT, IN) \
template<int pin_no>                                  \
class Pin<LETTER, pin_no, 'H'> {                      \
public:                                               \
    enum { mask = 1 << pin_no };                      \
	static void ConfIn() { DIR &= ~mask; }            \
	static void ConfOut() { DIR |= mask; }            \
	static void Cpl() { OUT ^= mask; }                \
	static void On() { OUT |= mask; }                 \
	static void Off() { OUT &= ~mask; }               \
	static bool Signalled() { return IN & mask; }     \
};                                                    \
template<int pin_no>                                  \
class Pin<LETTER, pin_no, 'L'> {                      \
public:                                               \
    enum { mask = 1 << pin_no };                      \
	static void ConfIn() { DIR &= ~mask; }            \
	static void ConfOut() { DIR |= mask; }            \
	static void Cpl() { OUT ^= mask; }                \
	static void On() { OUT &= ~mask; }                \
	static void Off() { OUT |= mask; }                \
	static bool Signalled() { return !(IN & mask); }  \
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
