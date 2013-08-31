/*=============================================================================

  Copyright (C) 2012-2013 Denis Artyomov (denis.artyomov@gmail.com)

  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files (the "Software"), to deal
  in the Software without restriction, including without limitation the rights
  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
  copies of the Software, and to permit persons to whom the Software is
  furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in
  all copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
  SOFTWARE.

===============================================================================
  File:      SPI.hpp
  Author(s): Denis Artyomov
=============================================================================*/

#ifndef SPI_HPP_FILE_INCLUDED_
#define SPI_HPP_FILE_INCLUDED_

#include "stm32f10x.h"

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


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class EmptyCSPin
{
public:
	static void On() {}
	static void Off() {}
	static void Mode(...) {}
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
		MOSIPin::Mode(OUTPUT);
		MOSIPin::Off();
		MISOPin::Mode(INPUTPULLED);
		MISOPin::PullUp();
		SCKPin::Mode(OUTPUT);
		CPOL_Helper<SCKPin, CPOL>::Off();
		CSPin::Mode(OUTPUT);
		CSPin::On();
	}

	static DataType write_and_read(DataType value);

	static void write(DataType value)
	{
		write_and_read(value);
	}

	template <typename SoftCSPin> static void cs_high()
	{
		SoftCSPin::On();
	}

	template <typename SoftCSPin> static void cs_low()
	{
		SoftCSPin::Off();
	}

private:
	template <typename SCK, SPI_CPOL val> struct CPOL_Helper;

	template<typename SCK> struct CPOL_Helper<SCK, CPOL_Low>
	{
		static void On() { SCK::On(); }
		static void Off() { SCK::Off(); }
	};

	template<typename SCK> struct CPOL_Helper<SCK, CPOL_High>
	{
		static void On() { SCK::Off(); }
		static void Off() { SCK::On(); }
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
	CSPin::Off();
	const DataType test_bit = 1 << (sizeof(DataType)*8-1);
	for (uint8_t bit = 0; bit < sizeof(DataType)*8; bit++)
	{
		if (value & test_bit) MOSIPin::On();
		else MOSIPin::Off();
		for (volatile unsigned i = 0; i < Delay; i++);
		value <<= 1;
		CPOL_Helper<SCKPin, CPOL>::On();
		result <<= 1;
		for (volatile unsigned i = 0; i < Delay; i++);
		if (MISOPin::Signalled()) result |= 1;
		CPOL_Helper<SCKPin, CPOL>::Off();
	}
	CSPin::On();
	return result;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

template <int N> struct SPI_Helper {};

template <int N>
class SPI
{
public:
	typedef SPI_Helper<N> Helper;

	static void init(
		const SPI_BitsCount   bits_count,
		const SPI_CSMode      cs_mode,
		const SPI_CPHA        cpha,
		const SPI_CPOL        cpol,
		const SPI_BRPrescaler rate_prescaler)
	{
		static const uint16_t SPI_Mode_Master = SPI_CR1_MSTR | SPI_CR1_SSI;

		// initialize pins
		Helper::MISO_Pin::Mode(INPUTPULLED);
		Helper::MISO_Pin::PullUp();
		Helper::MOSI_Pin::Mode(ALT_OUTPUT);
		Helper::SCK_Pin::Mode(ALT_OUTPUT);

		SPI_TypeDef * const addr = (SPI_TypeDef*)Helper::SPI_Mem_Addr;

		uint16_t cr1 = addr->CR1 & 0x3040;

		cr1 |= (rate_prescaler | cs_mode | cpha | cpol | bits_count | SPI_Mode_Master | SPI_CR1_SPE);

		// Configure CS output
		if (cs_mode == CS_Hard)
		{
			Helper::CS_Pin::Mode(ALT_OUTPUT);
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
		CSPin::On();
	}

	template <class CSPin>
	static void cs_low()
	{
		CSPin::Off();
	}
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

template <> struct SPI_Helper<1>
{
	enum {
		SPI_Mem_Addr = SPI1_BASE
	};

	typedef Pin<'A', 7> MOSI_Pin;
	typedef Pin<'A', 6> MISO_Pin;
	typedef Pin<'A', 5> SCK_Pin;
	typedef Pin<'A', 4> CS_Pin;
};

template <> struct SPI_Helper<2>
{
	enum {
		SPI_Mem_Addr = SPI2_BASE
	};

	typedef Pin<'B', 15> MOSI_Pin;
	typedef Pin<'B', 14> MISO_Pin;
	typedef Pin<'B', 13> SCK_Pin;
	typedef Pin<'B', 12> CS_Pin;
};


#endif
