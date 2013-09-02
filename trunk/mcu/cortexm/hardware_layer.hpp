#ifndef HARDWARE_LAYER_HPP_FILE_INCLUDED
#define HARDWARE_LAYER_HPP_FILE_INCLUDED

#include "stm32f10x.h"
#include <stddef.h>


/****[ Bit Banding ]***********************************************************/

namespace bitband
{
	template <unsigned Addrress, unsigned Bit>
	struct PeriphAddr
	{
		enum {
			result = PERIPH_BB_BASE + ((Addrress - PERIPH_BASE) * 32) + (Bit * 4)
		};
	};
}

/****[ GPIO ]******************************************************************/

namespace gpio
{
	namespace detailed
	{
		template <char Name> struct PortHelper;

		template <> struct PortHelper<'A'>
		{
			enum {
				BASE_ADDR = GPIOA_BASE,
				ENABLE_BIT = RCC_APB2ENR_IOPAEN
			};
		};

		template <> struct PortHelper<'B'>
		{
			enum {
				BASE_ADDR = GPIOB_BASE,
				ENABLE_BIT = RCC_APB2ENR_IOPBEN
			};
		};

		template <> struct PortHelper<'C'>
		{
			enum {
				BASE_ADDR = GPIOC_BASE,
				ENABLE_BIT = RCC_APB2ENR_IOPCEN
			};
		};

		template <> struct PortHelper<'D'>
		{
			enum {
				BASE_ADDR = GPIOD_BASE,
				ENABLE_BIT = RCC_APB2ENR_IOPDEN
			};
		};

	}

	template <char Name>
	struct Port
	{
		inline static void enable()
		{
			RCC->APB2ENR |= detailed::PortHelper<Name>::ENABLE_BIT;
		}

		inline static void disable()
		{
			RCC->APB2ENR &= ~detailed::PortHelper<Name>::ENABLE_BIT;
		}

		enum {
			BASE_ADDR = detailed::PortHelper<Name>::BASE_ADDR
		};
	};

	typedef Port<'A'> PA;
	typedef Port<'B'> PB;
	typedef Port<'C'> PC;
	typedef Port<'D'> PD;

#define		MAKE_PIN_CFG(MODE, CNF)	( (MODE) | (CNF)<<2 )
	enum Mode
	{
		ANALOGINPUT = 			MAKE_PIN_CFG ( 0, 0 ),
		INPUT =					MAKE_PIN_CFG ( 0, 1 ),
		INPUTPULLED =			MAKE_PIN_CFG ( 0, 2 ),

		OUTPUT_10MHZ =			MAKE_PIN_CFG ( 1, 0 ),
		OUTPUT_OD_10MHZ =		MAKE_PIN_CFG ( 1, 1 ),
		ALT_OUTPUT_10MHZ =		MAKE_PIN_CFG ( 1, 2 ),
		ALT_OUTPUT_OD_10MHZ =	MAKE_PIN_CFG ( 1, 3 ),

		OUTPUT_2MHZ =			MAKE_PIN_CFG ( 2, 0 ),
		OUTPUT_OD_2MHZ =		MAKE_PIN_CFG ( 2, 1 ),
		ALT_OUTPUT_2MHZ =		MAKE_PIN_CFG ( 2, 2 ),
		ALT_OUTPUT_OD_2MHZ =	MAKE_PIN_CFG ( 2, 3 ),

		OUTPUT =				MAKE_PIN_CFG ( 3, 0 ),
		OUTPUT_OD =				MAKE_PIN_CFG ( 3, 1 ),
		ALT_OUTPUT =			MAKE_PIN_CFG ( 3, 2 ),
		ALT_OUTPUT_OD =			MAKE_PIN_CFG ( 3, 3 )
	};
#undef MAKE_PIN_CFG


	template <class Port, unsigned PinIndex>
	class Pin
	{
	public:
		inline static void configure(Mode mode)
		{
			*reinterpret_cast<volatile uint32_t*>(CONF_ADR) =
					(*reinterpret_cast<uint32_t*>(CONF_ADR) & ~(0x0F<<SHIFT)) | (mode<<SHIFT);
		}

		inline static void on()
		{
			*reinterpret_cast<volatile uint32_t*>(BB_OUT_ADDR) = 1;
		}

		inline static void off()
		{
			*reinterpret_cast<volatile uint32_t*>(BB_OUT_ADDR) = 0;
		}

		inline static bool get_in()
		{
			return *reinterpret_cast<uint32_t*>(BB_IN_ADDR) != 0;
		}

		inline bool static get_out()
		{
			return *reinterpret_cast<uint32_t*>(BB_OUT_ADDR) != 0;
		}

	private:
		enum {
			BASE_ADDR   = Port::BASE_ADDR,
			SHIFT       = (PinIndex % 8) << 2,
			CONF_ADR    = BASE_ADDR + (PinIndex < 8 ? offsetof(GPIO_TypeDef, CRL) : offsetof(GPIO_TypeDef, CRH)),
			IDR_ADDR    = BASE_ADDR + offsetof(GPIO_TypeDef, IDR),
			ODR_ADDR    = BASE_ADDR + offsetof(GPIO_TypeDef, ODR),
			BB_IN_ADDR  = bitband::PeriphAddr<IDR_ADDR, PinIndex>::result,
			BB_OUT_ADDR = bitband::PeriphAddr<ODR_ADDR, PinIndex>::result
		};
	};

} // end 'namespace gpio'

#endif
