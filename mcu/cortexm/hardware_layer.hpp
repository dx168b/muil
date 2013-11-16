#ifndef HARDWARE_LAYER_HPP_FILE_INCLUDED
#define HARDWARE_LAYER_HPP_FILE_INCLUDED

#include "stm32f10x.h"
#include <stddef.h>

namespace hl {

/****[ Bit Banding ]***********************************************************/

template <unsigned Addrress, unsigned Bit>
struct BitBandPeriph
{
	enum {
		addr = PERIPH_BB_BASE + ((Addrress - PERIPH_BASE) * 32) + (Bit * 4)
	};

	inline static bool get()
	{
		return *reinterpret_cast<volatile uint32_t*>(addr) != 0;
	}

	inline static void on()
	{
		*reinterpret_cast<volatile uint32_t*>(addr) = 1;
	}

	inline static void off()
	{
		*reinterpret_cast<volatile uint32_t*>(addr) = 0;
	}
};


/****[ DWT ]*******************************************************************/

class DWTCounter
{
public:
	static void enable()
	{
		*reinterpret_cast<volatile uint32_t*>(SCB_DEMCR)  |= 0x01000000;
		*reinterpret_cast<volatile uint32_t*>(DWT_CYCCNT)  = 0;
		*reinterpret_cast<volatile uint32_t*>(DWT_CONTROL) |= 1;
	}

	inline static uint32_t get()
	{
		return *reinterpret_cast<volatile uint32_t*>(DWT_CYCCNT);
	}

private:
	enum
	{
		DWT_CYCCNT  = 0xE0001004,
		DWT_CONTROL = 0xE0001000,
		SCB_DEMCR   = 0xE000EDFC
	};
};


/****[ GPIO ]******************************************************************/

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

#define	MAKE_PIN_CFG(MODE, CNF)	( (MODE) | (CNF)<<2 )
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
		enum { SHIFT = (PinIndex % 8) << 2 };
		*reinterpret_cast<volatile uint32_t*>(CONF_ADR) =
				(*reinterpret_cast<uint32_t*>(CONF_ADR) & ~(0x0F<<SHIFT)) | (mode<<SHIFT);
	}

	inline static void on()
	{
		OutBitBang::on();
	}

	inline static void off()
	{
		OutBitBang::off();
	}

	inline static bool get_in()
	{
		return InBitBang::get();
	}

	inline bool static get_out()
	{
		return OutBitBang::get();
	}

private:
	enum {
		BASE_ADDR   = Port::BASE_ADDR,
		CONF_ADR    = BASE_ADDR + ((PinIndex < 8) ? offsetof(GPIO_TypeDef, CRL) : offsetof(GPIO_TypeDef, CRH)),
		IDR_ADDR    = BASE_ADDR + offsetof(GPIO_TypeDef, IDR),
		ODR_ADDR    = BASE_ADDR + offsetof(GPIO_TypeDef, ODR)
	};

	typedef BitBandPeriph<IDR_ADDR, PinIndex> InBitBang;
	typedef BitBandPeriph<ODR_ADDR, PinIndex> OutBitBang;
};


/****[ SPI ]*******************************************************************/

enum SPI_BitsCount
{
	SPIBits_8 = 0,
	SPIBits_16 = SPI_CR1_DFF
};

enum SPI_CSMode
{
	CS_Hard = 0,
	CS_Soft = SPI_CR1_SSM,
};

enum SPI_CPHA
{
	CPHA_1Edge = 0,
	CPHA_2Edge = SPI_CR1_CPHA
};

enum SPI_CPOL
{
	CPOL_Low = 0,
	CPOL_High = SPI_CR1_CPOL
};

enum SPI_BRPrescaler
{
	SPI_BaudRatePrescaler_2 = 0x0000,
	SPI_BaudRatePrescaler_4 = 0x0008,
	SPI_BaudRatePrescaler_8 = 0x0010,
	SPI_BaudRatePrescaler_16 = 0x0018,
	SPI_BaudRatePrescaler_32 = 0x0020,
	SPI_BaudRatePrescaler_64 = 0x0028,
	SPI_BaudRatePrescaler_128 = 0x0030,
	SPI_BaudRatePrescaler_256 = 0x0038
};

class EmptyCSPin
{
public:
	static void on() {}
	static void off() {}
	static void configure(...) {}
};

template <
	typename DataType,
	SPI_CPHA CPHA,
	SPI_CPOL CPOL,
	unsigned Delay,
	typename MOSIPin,
	typename MISOPin,
	typename SCKPin,
	typename CSPin = EmptyCSPin
>
class SoftwareSPI
{
public:
	static void init()
	{
		MOSIPin::configure(OUTPUT);
		MOSIPin::off();
		MISOPin::configure(INPUTPULLED);
		MISOPin::on();
		SCKPin::configure(OUTPUT);
		CPOL_Helper<SCKPin, CPOL>::Off();
		CSPin::configure(OUTPUT);
		CSPin::on();
	}

	static DataType write_and_read(DataType value);

	static void write(DataType value)
	{
		write_and_read(value);
	}

	template <typename SoftCSPin> static void cs_high()
	{
		SoftCSPin::on();
	}

	template <typename SoftCSPin> static void cs_low()
	{
		SoftCSPin::off();
	}

private:
	template <typename SCK, SPI_CPOL val> struct CPOL_Helper;

	template<typename SCK> struct CPOL_Helper<SCK, CPOL_Low>
	{
		static void On() { SCK::on(); }
		static void Off() { SCK::off(); }
	};

	template<typename SCK> struct CPOL_Helper<SCK, CPOL_High>
	{
		static void On() { SCK::off(); }
		static void Off() { SCK::on(); }
	};
};

template <
	typename DataType,
	SPI_CPHA CPHA,
	SPI_CPOL CPOL,
	unsigned Delay,
	typename MOSIPin,
	typename MISOPin,
	typename SCKPin,
	typename CSPin
>
DataType SoftwareSPI<DataType, CPHA, CPOL, Delay, MOSIPin, MISOPin, SCKPin, CSPin>::write_and_read(DataType value)
{
	DataType result = 0;
	CSPin::off();
	const DataType test_bit = 1 << (sizeof(DataType)*8-1);
	for (uint8_t bit = 0; bit < sizeof(DataType)*8; bit++)
	{
		if (value & test_bit) MOSIPin::on();
		else MOSIPin::off();
		for (volatile unsigned i = 0; i < Delay; i++);
		value <<= 1;
		CPOL_Helper<SCKPin, CPOL>::On();
		result <<= 1;
		for (volatile unsigned i = 0; i < Delay; i++);
		if (MISOPin::get_in()) result |= 1;
		CPOL_Helper<SCKPin, CPOL>::Off();
	}
	CSPin::on();
	return result;
}

namespace detailed
{
	template<int N> struct SPI_Helper;

#ifdef RCC_APB2ENR_SPI1EN
	template<> struct SPI_Helper<1>
	{
		enum
		{
			SPI_Mem_Addr = SPI1_BASE,
			RCC_ABP_Addr = RCC_BASE + offsetof(RCC_TypeDef, APB2ENR),
			RCC_ABP_Mask = RCC_APB2ENR_SPI1EN
		};

		typedef Pin<PA, 7> MOSI_Pin;
		typedef Pin<PA, 6> MISO_Pin;
		typedef Pin<PA, 5> SCK_Pin;
		typedef Pin<PA, 4> CS_Pin;
	};
#endif

#ifdef RCC_APB1ENR_SPI2EN
	template<> struct SPI_Helper<2>
	{
		enum
		{
			SPI_Mem_Addr = SPI2_BASE,
			RCC_ABP_Addr = RCC_BASE + offsetof(RCC_TypeDef, APB1ENR),
			RCC_ABP_Mask = RCC_APB1ENR_SPI2EN
		};

		typedef Pin<PB, 15> MOSI_Pin;
		typedef Pin<PB, 14> MISO_Pin;
		typedef Pin<PB, 13> SCK_Pin;
		typedef Pin<PB, 12> CS_Pin;
	};
#endif
}

template <int N>
class SPI
{
public:
	static void init(
		const SPI_BitsCount   bits_count,
		const SPI_CSMode      cs_mode,
		const SPI_CPHA        cpha,
		const SPI_CPOL        cpol,
		const SPI_BRPrescaler rate_prescaler)
	{
		static const uint16_t SPI_Mode_Master = SPI_CR1_MSTR | SPI_CR1_SSI;

		// initialize pins
		Helper::MISO_Pin::configure(INPUTPULLED);
		Helper::MISO_Pin::on();
		Helper::MOSI_Pin::configure(ALT_OUTPUT);
		Helper::SCK_Pin::configure(ALT_OUTPUT);

		SPI_TypeDef * const addr = (SPI_TypeDef*)Helper::SPI_Mem_Addr;

		uint16_t cr1 = addr->CR1 & 0x3040;

		cr1 |= (rate_prescaler | cs_mode | cpha | cpol | bits_count | SPI_Mode_Master | SPI_CR1_SPE);

		// Configure CS output
		if (cs_mode == CS_Hard)
		{
			Helper::CS_Pin::configure(ALT_OUTPUT);
			addr->CR2 |= SPI_CR2_SSOE;
		}
		else addr->CR2 &= ~SPI_CR2_SSOE;

		addr->CR1 = cr1;
	}

	template <typename T>
	static T write_and_read(T value)
	{
		SPI_TypeDef * const addr = (SPI_TypeDef*)Helper::SPI_Mem_Addr;

		while (!(addr->SR & SPI_SR_TXE)) {}
		volatile uint8_t tmp = addr->DR;
		addr->DR = value;
		while (!(addr->SR & SPI_SR_RXNE)) {}
		return addr->DR;
	}

	template <typename T>
	static void write(T value)
	{
		SPI_TypeDef * const addr = (SPI_TypeDef*)Helper::SPI_Mem_Addr;
		while (!(addr->SR & SPI_SR_TXE)) {}
		addr->DR = value;
	}

	template <class CSPin>
	static void cs_high()
	{
		SPI_TypeDef * const addr = (SPI_TypeDef*)Helper::SPI_Mem_Addr;
		while (!(addr->SR & SPI_SR_TXE) || (addr->SR & SPI_SR_BSY)) {}
		CSPin::on();
	}

	template <class CSPin>
	static void cs_low()
	{
		CSPin::off();
	}

	static void enable()
	{
		*reinterpret_cast<volatile uint32_t*>(Helper::RCC_ABP_Addr) |= Helper::RCC_ABP_Mask;
	}

	static void disable()
	{
		*reinterpret_cast<volatile uint32_t*>(Helper::RCC_ABP_Addr) &= ~Helper::RCC_ABP_Mask;
	}

private:
	typedef detailed::SPI_Helper<N> Helper;
};


/****[ Delay utils ]***********************************************************/

template <unsigned SysFreq>
class Delay
{
public:
	static void exec_ms(uint32_t value)
	{
		wait(value * (SysFreq/1000));
	}

	static void exec_us(uint32_t value)
	{
		wait(value * (SysFreq/1000000));
	}

private:
	static void wait(uint32_t ticks)
	{
		int32_t stop = DWTCounter::get()+ticks;
		while ((int32_t)(DWTCounter::get()-stop) < 0) {}
	}
};


} // end 'namespace hl'

#endif
