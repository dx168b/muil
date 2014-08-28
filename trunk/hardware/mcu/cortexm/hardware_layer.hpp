#ifndef HARDWARE_LAYER_HPP_FILE_INCLUDED
#define HARDWARE_LAYER_HPP_FILE_INCLUDED

/* Include this file in your source after any stm32f**x.h files included */

#include <stddef.h>

#define ADDR_TO_UINT32(ADR) *reinterpret_cast<volatile uint32_t*>(ADR)

namespace hl {

// fwd.
template <unsigned SysFreq> class Delay;

/****[ Bit Banding ]***********************************************************/

template <unsigned Addrress, unsigned Bit>
struct BitBandPeriph
{
	enum {
		addr = PERIPH_BB_BASE + ((Addrress - PERIPH_BASE) * 32) + (Bit * 4)
	};

	inline static bool get()
	{
		return ADDR_TO_UINT32(addr) != 0;
	}

	inline static void on()
	{
		ADDR_TO_UINT32(addr) = 1;
	}

	inline static void off()
	{
		ADDR_TO_UINT32(addr) = 0;
	}
};


/****[ DWT ]*******************************************************************/

class DWTCounter
{
public:
	static void enable()
	{
		ADDR_TO_UINT32(SCB_DEMCR)  |= 0x01000000;
		ADDR_TO_UINT32(DWT_CYCCNT)  = 0;
		ADDR_TO_UINT32(DWT_CONTROL) |= 1;
	}

	inline static uint32_t get()
	{
		return ADDR_TO_UINT32(DWT_CYCCNT);
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

#ifdef __STM32F10X_STDPERIPH_VERSION
#define STM32_GPIO_V1 1
#else
#define STM32_GPIO_V2 1
#endif

namespace detailed
{
	template <char Name> struct PortHelper;

	template <> struct PortHelper<'A'>
	{
		enum {
			BASE_ADDR = GPIOA_BASE,
#ifdef STM32_GPIO_V1
			ENABLE_BIT = RCC_APB2ENR_IOPAEN,
#endif
#ifdef STM32_GPIO_V2
			ENABLE_BIT = RCC_AHB1ENR_GPIOAEN,
#endif
		};
	};

	template <> struct PortHelper<'B'>
	{
		enum {
			BASE_ADDR = GPIOB_BASE,
#ifdef STM32_GPIO_V1
			ENABLE_BIT = RCC_APB2ENR_IOPBEN,
#endif
#ifdef STM32_GPIO_V2
			ENABLE_BIT = RCC_AHB1ENR_GPIOBEN,
#endif
		};
	};

	template <> struct PortHelper<'C'>
	{
		enum {
			BASE_ADDR = GPIOC_BASE,
#ifdef STM32_GPIO_V1
			ENABLE_BIT = RCC_APB2ENR_IOPCEN,
#endif
#ifdef STM32_GPIO_V2
			ENABLE_BIT = RCC_AHB1ENR_GPIOCEN,
#endif
		};
	};

	template <> struct PortHelper<'D'>
	{
		enum {
			BASE_ADDR = GPIOD_BASE,
#if defined(STM32_GPIO_V1) && defined(RCC_APB2ENR_IOPDAEN)
			ENABLE_BIT = RCC_APB2ENR_IOPDAEN,
#endif
#ifdef STM32_GPIO_V2
			ENABLE_BIT = RCC_AHB1ENR_GPIODEN,
#endif
		};
	};

	template <> struct PortHelper<'E'>
	{
		enum {
			BASE_ADDR = GPIOE_BASE,
#if defined(STM32_GPIO_V1) && defined(RCC_APB2ENR_IOPEEN)
			ENABLE_BIT = RCC_APB2ENR_IOPEEN,
#endif
#ifdef STM32_GPIO_V2
			ENABLE_BIT = RCC_AHB1ENR_GPIOEEN,
#endif
		};
	};

	template <> struct PortHelper<'F'>
	{
		enum {
			BASE_ADDR = GPIOF_BASE,
#if defined(STM32_GPIO_V1) && defined(RCC_APB2ENR_IOPFEN)
			ENABLE_BIT = RCC_APB2ENR_IOPFEN,
#endif
#ifdef STM32_GPIO_V2
			ENABLE_BIT = RCC_AHB1ENR_GPIOFEN,
#endif
		};
	};

	template <> struct PortHelper<'G'>
	{
		enum {
			BASE_ADDR = GPIOG_BASE,
#if defined(STM32_GPIO_V1) && defined(RCC_APB2ENR_IOPGEN)
			ENABLE_BIT = RCC_APB2ENR_IOPGEN,
#endif
#ifdef STM32_GPIO_V2
			ENABLE_BIT = RCC_AHB1ENR_GPIOGEN,
#endif
		};
	};
}

template <char Name>
struct Port
{
	inline static void enable()
	{
		ADDR_TO_UINT32(RCC_APBENR) |= detailed::PortHelper<Name>::ENABLE_BIT;
	}

	inline static void disable()
	{
		ADDR_TO_UINT32(RCC_APBENR) &= ~detailed::PortHelper<Name>::ENABLE_BIT;
	}

	enum
	{
#ifdef STM32_GPIO_V1
		RCC_APBENR = RCC_BASE+offsetof(RCC_TypeDef, APB2ENR),
#endif
#ifdef STM32_GPIO_V2
		RCC_APBENR = RCC_BASE+offsetof(RCC_TypeDef, AHB1ENR),
#endif
		BASE_ADDR = detailed::PortHelper<Name>::BASE_ADDR
	};
};

typedef Port<'A'> PA;
typedef Port<'B'> PB;
typedef Port<'C'> PC;
typedef Port<'D'> PD;
typedef Port<'E'> PE;
typedef Port<'F'> PF;
typedef Port<'G'> PG;

enum OutputType
{
	OT_PULL_PUSH      = 0x0,
	OT_OPEN_DRAIN     = 0x1,
	OT_ALT_PULL_PUSH  = 0x2,
	OT_ALT_OPEN_DRAIN = 0x3,
};

enum OutputSpeed
{
	OS_LOW     = 0,
	OS_QUARTER = 1,
	OS_HALF    = 2,
	OS_FULL    = 3,
};

enum InputType
{
	IT_ANALOG    = 0,
	IT_DIGITAl   = 1,
	IT_PULL_DOWN = 2,
	IT_PULL_UP   = 3,
};

template <class Port, unsigned PinIndex>
class Pin
{
public:
	template<OutputType TYPE, OutputSpeed SPEED>
	inline static void configure_output()
	{
#ifdef STM32_GPIO_V1
		enum { VALUE = (SPEED << SHIFT4) | (TYPE << (SHIFT4+2)) };
		ADDR_TO_UINT32(CONF_ADR) = (ADDR_TO_UINT32(CONF_ADR) & (~MASK4)) | VALUE;
#endif
#ifdef STM32_GPIO_V2
		enum { MODER_VAL = (TYPE == OT_ALT_PULL_PUSH) || (TYPE == OT_ALT_OPEN_DRAIN) ? 0x2 : 0x1 };
		enum { OTYPER_VAL = (TYPE == OT_OPEN_DRAIN) || (TYPE == OT_ALT_OPEN_DRAIN) ? 0x1 : 0x0 };
		ADDR_TO_UINT32(MODER)   = (ADDR_TO_UINT32(MODER)   & (~PAIR_MASK)) | (MODER_VAL  << SHIFT2);
		ADDR_TO_UINT32(OTYPER)  = (ADDR_TO_UINT32(OTYPER)  & (~BIT_MASK))  | (OTYPER_VAL << SHIFT1);
		ADDR_TO_UINT32(OSPEEDR) = (ADDR_TO_UINT32(OSPEEDR) & (~PAIR_MASK)) | (SPEED      << SHIFT2);
#endif
	}

	template<InputType IT>
	inline static void configure_input()
	{
#ifdef STM32_GPIO_V1
		enum { CNF = (IT == IT_ANALOG) ? 0 : (IT == IT_DIGITAl) ? 1 : 2 };
		ADDR_TO_UINT32(CONF_ADR) = (ADDR_TO_UINT32(CONF_ADR) & (~MASK4)) | (CNF << (SHIFT4+2));
		switch (IT)
		{
		case IT_PULL_UP:   on(); break;
		case IT_PULL_DOWN: off(); break;
		default:           break;
		}
#endif
#ifdef STM32_GPIO_V2
		enum { MODER_VAL = (IT == IT_ANALOG) ? 0x3 : 0x0 };
		enum { PUD_VAL = (IT == IT_PULL_DOWN) ? 0x2 : (IT == IT_PULL_UP) ? 0x1 : 0 };
		ADDR_TO_UINT32(MODER) = (ADDR_TO_UINT32(MODER) & (~PAIR_MASK)) | (MODER_VAL << SHIFT2);
		ADDR_TO_UINT32(PUPDR) = (ADDR_TO_UINT32(PUPDR) & (~PAIR_MASK)) | (PUD_VAL   << SHIFT2);
#endif
	}

	inline static void on()
	{
		OutBitBang::on();
	}

	inline static void off()
	{
		OutBitBang::off();
	}

	inline static void set_out(bool value)
	{
		if (value) on();
		else off();
	}

	inline static bool get_in()
	{
		return InBitBang::get();
	}

	inline static bool get_out()
	{
		return OutBitBang::get();
	}

	template <unsigned Fun>
	inline static void remap()
	{
#ifdef STM32_GPIO_V2
		enum { INDEX = PinIndex >> 0x3, VALUE = Fun << SHIFT4 };
		ADDR_TO_UINT32(AFR_ADDR+INDEX) = (ADDR_TO_UINT32(AFR_ADDR+INDEX) & (~MASK4)) | VALUE;
#endif
	}

private:
	enum {
		BASE_ADDR = Port::BASE_ADDR,
		IDR_ADDR  = BASE_ADDR + offsetof(GPIO_TypeDef, IDR),
		ODR_ADDR  = BASE_ADDR + offsetof(GPIO_TypeDef, ODR),
		SHIFT4    = (PinIndex % 8) * 4,
		MASK4     = 0xF << SHIFT4,
#ifdef STM32_GPIO_V1
		CONF_ADR  = BASE_ADDR + ((PinIndex < 8) ? offsetof(GPIO_TypeDef, CRL) : offsetof(GPIO_TypeDef, CRH)),
#endif
#ifdef STM32_GPIO_V2
		MODER     = BASE_ADDR + offsetof(GPIO_TypeDef, MODER),
		OSPEEDR   = BASE_ADDR + offsetof(GPIO_TypeDef, OSPEEDR),
		OTYPER    = BASE_ADDR + offsetof(GPIO_TypeDef, OTYPER),
		PUPDR     = BASE_ADDR + offsetof(GPIO_TypeDef, PUPDR),
		AFR_ADDR  = BASE_ADDR + offsetof(GPIO_TypeDef, AFR),
		SHIFT1    = PinIndex,
		SHIFT2    = PinIndex * 2,
		BIT_MASK  = 0x1 << SHIFT1,
		PAIR_MASK = 0x3 << SHIFT2,
#endif
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
	SPI_Prescaler_2 = 0x0000,
	SPI_Prescaler_4 = 0x0008,
	SPI_Prescaler_8 = 0x0010,
	SPI_Prescaler_16 = 0x0018,
	SPI_Prescaler_32 = 0x0020,
	SPI_Prescaler_64 = 0x0028,
	SPI_Prescaler_128 = 0x0030,
	SPI_Prescaler_256 = 0x0038
};

class EmptyCSPin
{
public:
	static void on() {}
	static void off() {}

	template<OutputType TYPE, OutputSpeed SPEED>
	static void configure_output() {}
};

template <
	typename DataType,
	SPI_CPHA CPHA,
	SPI_CPOL CPOL,
	unsigned SysFreq,
	unsigned DelayValueUC,
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
		MOSIPin::template configure_output<OT_PULL_PUSH, OS_FULL>();
		MOSIPin::off();
		MISOPin::template configure_input<IT_PULL_UP>();
		SCKPin::template configure_output<OT_PULL_PUSH, OS_FULL>();
		CPOL_Helper<SCKPin, CPOL>::Off();
		CSPin::template configure_output<OT_PULL_PUSH, OS_FULL>();
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

	typedef Delay<SysFreq> DelayUtil;
};

template <
	typename DataType,
	SPI_CPHA CPHA,
	SPI_CPOL CPOL,
	unsigned SysFreq,
	unsigned DelayValueUC,
	typename MOSIPin,
	typename MISOPin,
	typename SCKPin,
	typename CSPin
>
DataType SoftwareSPI<DataType, CPHA, CPOL, SysFreq, DelayValueUC, MOSIPin, MISOPin, SCKPin, CSPin>::write_and_read(DataType value)
{
	DataType result = 0;
	CSPin::off();
	const DataType test_bit = 1 << (sizeof(DataType)*8-1);
	for (uint8_t bit = 0; bit < sizeof(DataType)*8; bit++)
	{
		if (value & test_bit) MOSIPin::on();
		else MOSIPin::off();
		DelayUtil::exec_us(DelayValueUC);
		value <<= 1;
		CPOL_Helper<SCKPin, CPOL>::On();
		result <<= 1;
		DelayUtil::exec_us(DelayValueUC);
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

		typedef PA Port;
		typedef Pin<Port, 7> MOSI_Pin;
		typedef Pin<Port, 6> MISO_Pin;
		typedef Pin<Port, 5> SCK_Pin;
		typedef Pin<Port, 4> CS_Pin;
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

		typedef PB Port;
		typedef Pin<Port, 15> MOSI_Pin;
		typedef Pin<Port, 14> MISO_Pin;
		typedef Pin<Port, 13> SCK_Pin;
		typedef Pin<Port, 12> CS_Pin;
	};
#endif

#ifdef RCC_APB2ENR_SPI5EN
	template<> struct SPI_Helper<5>
	{
		enum
		{
			SPI_Mem_Addr = SPI5_BASE,
			RCC_ABP_Addr = RCC_BASE + offsetof(RCC_TypeDef, APB2ENR),
			RCC_ABP_Mask = RCC_APB2ENR_SPI5EN
		};

		typedef PF Port;
		typedef Pin<Port, 9> MOSI_Pin;
		typedef Pin<Port, 8> MISO_Pin;
		typedef Pin<Port, 7> SCK_Pin;
		typedef Pin<Port, 6> CS_Pin; // ???
	};
#endif
}

template <int N>
class SPI
{
private:
	typedef detailed::SPI_Helper<N> Helper;

public:
	typedef typename Helper::Port Port;
	typedef typename Helper::MISO_Pin MISO_Pin;
	typedef typename Helper::MOSI_Pin MOSI_Pin;
	typedef typename Helper::SCK_Pin SCK_Pin;
	typedef typename Helper::CS_Pin CS_Pin;

	static void init(
		const SPI_BitsCount   bits_count,
		const SPI_CSMode      cs_mode,
		const SPI_CPHA        cpha,
		const SPI_CPOL        cpol,
		const SPI_BRPrescaler rate_prescaler)
	{
		static const uint16_t SPI_Mode_Master = SPI_CR1_MSTR | SPI_CR1_SSI;

		// initialize pins
		Helper::MISO_Pin::template configure_input<IT_DIGITAl>();
		Helper::MOSI_Pin::template configure_output<OT_ALT_PULL_PUSH, OS_FULL>();
		Helper::SCK_Pin::template configure_output<OT_ALT_PULL_PUSH, OS_FULL>();

		SPI_TypeDef * const addr = (SPI_TypeDef*)Helper::SPI_Mem_Addr;

		uint16_t cr1 = addr->CR1 & 0x3040;

		cr1 |= (rate_prescaler | cs_mode | cpha | cpol | bits_count | SPI_Mode_Master | SPI_CR1_SPE);

		// Configure CS output
		if (cs_mode == CS_Hard)
		{
			Helper::CS_Pin::template configure_output<OT_ALT_PULL_PUSH, OS_FULL>();
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
		addr->DR; // read but not store
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
		ADDR_TO_UINT32(Helper::RCC_ABP_Addr) |= Helper::RCC_ABP_Mask;
	}

	static void disable()
	{
		ADDR_TO_UINT32(Helper::RCC_ABP_Addr) &= ~Helper::RCC_ABP_Mask;
	}
};


/****[ Delay utils ]***********************************************************/

template <unsigned SysFreq>
class Delay
{
public:
	inline static void exec_ms(uint32_t value)
	{
		exec_ticks(value * (SysFreq/1000));
	}

	inline static void exec_us(uint32_t value)
	{
		exec_ticks(value * (SysFreq/1000000));
	}

private:
	inline static void exec_ticks(uint32_t ticks)
	{
		int32_t stop = DWTCounter::get()+ticks;
		while ((int32_t)(DWTCounter::get()-stop) < 0) {}
	}
};


} // end 'namespace hl'

#endif
