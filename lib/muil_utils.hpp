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
  File:      muil_utils.hpp
  Author(s): Denis Artyomov
=============================================================================*/

#ifndef MUIL_UTILS_HPP_FILE_INLUDED_
#define MUIL_UTILS_HPP_FILE_INLUDED_

#include <stdint.h>

namespace muil {

template <typename X, typename Y>
static Y linear_interpol(X x, X x1, X x2, Y y1, Y y2)
{
	return (x - x1) * (y2 - y1) / (x2 - x1) + y1;
}

template<typename T>
class Flags
{
public:
	Flags(T initial_flags = 0) : flags_(initial_flags) {}

	bool get(T flag) const { return (flags_ & flag) != 0; }
	void set(T flag, bool value) { flags_ = (flags_ & ~flag) | (value ? flag : 0); }

	void on(T flag) { flags_ |= flag; }
	void clear(T flag) { flags_ &= ~flag; }
	void toggle(T flag) { flags_ ^= flag; }

private:
	T flags_;
};

// Delay in milliseconds. Application must realize this function
void delay_ms(uint16_t milliseconds);

} // namespace muil

#endif
