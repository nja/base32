/*
 * Copyright (c) 2018 Johan Andersson <nilsjohanandersson@gmail.com>
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use, copy,
 * modify, merge, publish, distribute, sublicense, and/or sell copies
 * of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
 * BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
 * ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include "Base32Code.h"

#include <algorithm>
#include <cctype>
#include <cassert>
#include <stdexcept>

namespace base32 {

const char Base32Code::Digits[32] = {
    '0', '1', '2', '3', '4', '5', '6', '7',
    '8', '9', 'A', 'B', 'C', 'D', 'E', 'F',
    'G', 'H', 'J', 'K', 'M', 'N', 'P', 'Q',
    'R', 'S', 'T', 'V', 'W', 'X', 'Y', 'Z',
};

Base32Code::Base32Code(std::string str) : str_(std::move(str))
{
    for (char& c : str_)
    {
        c = Base32Code::Canonical(c);

        if (c == Base32Code::InvalidDigit()) {
            throw std::invalid_argument("Invalid Base32 digit");
        }
    }
}

char Base32Code::Digit(const char value)
{
    if (value < 0 || sizeof(Digits) <= value)
    {
        return InvalidDigit();
    }

    return Digits[value];
}

signed char Base32Code::Value(const char digit)
{
    const char canonical = Canonical(digit);

    if (InvalidDigit() == canonical)
    {
        return InvalidValue();
    }

    const char* it = std::find(std::cbegin(Digits), std::cend(Digits), canonical);
    const auto value = it - std::begin(Digits);

    assert(0 <= value && value < 32);

    return static_cast<signed char>(value);
}

char Base32Code::Canonical(const char digit)
{
    switch (digit)
    {
    case '0': // zero
    case 'o': // lower case oh
    case 'O': // upper case oh
        return '0';
    case '1': // one
    case 'i': // lower case eye
    case 'I': // upper case eye
    case 'l': // lower case ell
    case 'L': // upper case ell
        return '1';
    case '2':
    case '3':
    case '4':
    case '5':
    case '6':
    case '7':
    case '8':
    case '9':
    case 'A':
    case 'B':
    case 'C':
    case 'D':
    case 'E':
    case 'F':
    case 'G':
    case 'H':
       // 1
    case 'J':
    case 'K':
       // 1
    case 'M':
    case 'N':
       // 0
    case 'P':
    case 'Q':
    case 'R':
    case 'S':
    case 'T':
    // no U
    case 'V':
    case 'W':
    case 'X':
    case 'Y':
    case 'Z':
        return digit;
    case 'a':
    case 'b':
    case 'c':
    case 'd':
    case 'e':
    case 'f':
    case 'g':
    case 'h':
       // 1
    case 'j':
    case 'k':
       // 1
    case 'm':
    case 'n':
       // 0
    case 'p':
    case 'q':
    case 'r':
    case 's':
    case 't':
    // no u
    case 'v':
    case 'w':
    case 'x':
    case 'y':
    case 'z':
        return static_cast<char>(std::toupper(digit));
    default:
        return InvalidDigit();
    }
}

bool Base32Code::IsValid(const std::string& str)
{
    return std::all_of(str.begin(), str.end(), [](auto c) { return IsValid(c); });
}

} // namespace base32
