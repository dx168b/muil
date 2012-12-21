/*=============================================================================

  Copyright (C) 2012 Denis Artyomov (denis.artyomov@gmail.com)

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

#include "stm32f10x_spi.h"

enum CSMode
{
	CS_Hard,
	CS_Soft,
};

enum CPHA
{
	CPHA_1Edge,
	CPHA_2Edge
};

enum CPOL
{
	CPOL_Low,
	CPOL_High
};


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

template <
	unsigned Delay,
	typename MOSIPin,
	typename MISOPin,
	typename SCKPin,
	typename CSPin
>
class SoftwareSPI
{
public:
	static void init(uint8_t bits_count, CSMode cs, CPHA cpha, CPOL cpol);
	static uint16_t write_and_read(uint16_t value);
	static void write(uint16_t value);
	static void cs_high() { CSPin::On(); }
	static void cs_low() { CSPin::Off(); }

private:
	static uint8_t bits_count_;
	static bool use_soft_cs_;
};

template <unsigned Delay, typename MOSIPin, typename MISOPin, typename SCKPin, typename CSPin>
uint8_t SoftwareSPI<Delay, MOSIPin, MISOPin, SCKPin, CSPin>::bits_count_ = 0;

template <unsigned Delay, typename MOSIPin, typename MISOPin, typename SCKPin, typename CSPin>
bool SoftwareSPI<Delay, MOSIPin, MISOPin, SCKPin, CSPin>::use_soft_cs_ = false;

template <unsigned Delay, typename MOSIPin, typename MISOPin, typename SCKPin, typename CSPin>
void SoftwareSPI<Delay, MOSIPin, MISOPin, SCKPin, CSPin>::init(uint8_t bits_count, CSMode cs, CPHA cpha, CPOL cpol)
{
	use_soft_cs_ = (cs == CS_Soft);
	bits_count_ = bits_count;
}

template <unsigned Delay, typename MOSIPin, typename MISOPin, typename SCKPin, typename CSPin>
uint16_t SoftwareSPI<Delay, MOSIPin, MISOPin, SCKPin, CSPin>::write_and_read(uint16_t value)
{
	uint16_t result = 0;
	if (!use_soft_cs_) CSPin::Off();
	const uint16_t test_bit = 1 << (bits_count_-1);
	for (uint8_t bit = 0; bit < bits_count_; bit++)
	{
		if (value & test_bit) MOSIPin::On();
		else MOSIPin::Off();
		for (volatile unsigned i = 0; i < Delay; i++);
		value <<= 1;
		SCKPin::On();
		result <<= 1;
		for (volatile unsigned i = 0; i < Delay; i++);
		if (MISOPin::Signalled()) result |= 1;
		SCKPin::Off();
	}
	if (!use_soft_cs_) CSPin::On();
	return result;
}


template <unsigned Delay, typename MOSIPin, typename MISOPin, typename SCKPin, typename CSPin>
void SoftwareSPI<Delay, MOSIPin, MISOPin, SCKPin, CSPin>::write(uint16_t value)
{
	write_and_read(value);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

template <typename CSPin>
class HardwareSPI0
{
public:
	static void init_pins(uint8_t bits_count, CSMode cs, CPHA cpha, CPOL cpol)
	{
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);

		SPI_InitTypeDef SPI_Conf;

		typedef Pin<'A', 7> MOSIPin;
		typedef Pin<'A', 6> MISOPin;
		typedef Pin<'A', 5> SCKPin;

		MISOPin::Mode(INPUTPULLED);
		MISOPin::PullUp();
		MOSIPin::Mode(ALT_OUTPUT);
		SCKPin::Mode(ALT_OUTPUT);

		SPI_StructInit(&SPI_Conf);
		SPI_Conf.SPI_Mode = SPI_Mode_Master;
		SPI_Conf.SPI_DataSize = (bits_count == 8) ? SPI_DataSize_8b : SPI_DataSize_16b;
		SPI_Conf.SPI_NSS = (cs == CS_Soft) ? SPI_NSS_Soft : SPI_NSS_Hard;
		switch (cpha)
		{
		case CPHA_1Edge:
			SPI_Conf.SPI_CPHA = SPI_CPHA_1Edge;
			break;

		case CPHA_2Edge:
			SPI_Conf.SPI_CPHA = SPI_CPHA_2Edge;
			break;
		}

		switch (cpol)
		{
		case CPOL_Low:
			SPI_Conf.SPI_CPOL = SPI_CPOL_Low;
			break;

		case CPOL_High:
			SPI_Conf.SPI_CPOL = SPI_CPOL_High;
			break;
		}

		SPI_Conf.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_2;

		SPI_Init(SPI1, &SPI_Conf);

		SPI_SSOutputCmd(SPI1, ENABLE);
		SPI_Cmd(SPI1, ENABLE);
	}

	template <typename T>
	static T write_and_read(T value)
	{
		while (!(SPI1->SR & SPI_SR_TXE)) {}
		volatile uint8_t tmp = SPI1->DR;
		SPI1->DR = value;
		while (!(SPI1->SR & SPI_SR_RXNE)) {}
		return SPI1->DR;
	}

	template <typename T>
	static void write(T value)
	{
		while (!(SPI1->SR & SPI_SR_TXE)) {}
		SPI1->DR = value;
	}

	static void cs_high()
	{
		while (!(SPI1->SR & SPI_SR_TXE) || (SPI1->SR & SPI_SR_BSY)) {}
		CSPin::On();
	}

	static void cs_low()
	{
		CSPin::Off();
	}

private:
	static bool use_soft_cs_;
};

template <typename CSPin>
bool HardwareSPI0<CSPin>::use_soft_cs_ = false;

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#endif
