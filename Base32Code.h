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

#ifndef BASE32_BASE32CODE_H
#define BASE32_BASE32CODE_H

#include <string>

namespace base32 {

class Base32Code
{
public:
    // throws std::invalid_argument if str contains any invalid digits.
    Base32Code(std::string str);

    // Returns true if c is a valid Base32 digit (even if it's not in canonical form).
    static bool IsValid(const char c) { return Canonical(c) != InvalidDigit(); }

    // Returns true if all characters of str are valid.
    static bool IsValid(const std::string& str);

    // Create a Base32Code by encoding the (lowest 8 bits of) input bytes.
    // The bytes are encoded 5 bits at a time, MSB order.
    template<class T>
    static Base32Code Encode(T t) { return Encode(std::begin(t), std::end(t)); }

    // Create a Base32Code by encoding the input bytes. The highest bits of 
    // the first byte becomes the first digit of the code.
    template<class InputIt>
    static Base32Code Encode(InputIt first, InputIt last)
    {
        if (first == last) {
            return "";
        }

        std::string str;

        uint16_t tmp = 0;
        int shift = -DigitBits();

        for (auto in = first; in != last; ++in)
        {
            tmp <<= CharBits();
            shift += CharBits();
            tmp &= 0xff00;
            tmp |= 0x00ff & *in;

            do {
                str.push_back(Digit((tmp & (Mask() << shift)) >> shift));
                shift -= DigitBits();
            } while (shift > 0);
        }

        tmp <<= std::abs(shift);
        str.push_back(Digit(tmp & Mask()));

        return Base32Code(std::move(str));
    }

    // Writes DecodedSize() bytes to out. The first digit of the code
    // goes in the highest bits of the first byte written.
    template<class OutputIt>
    OutputIt Decode(OutputIt out) const
    {
        uint16_t tmp = 0;
        int shift = 0;

        for (auto in = str_.begin(); in != str_.end(); ++in)
        {
            tmp <<= DigitBits();
            shift += DigitBits();

            tmp &= ~Mask();
            tmp |= Mask() & Value(*in);

            if (shift >= CharBits())
            {
                shift -= CharBits();
                *out++ = tmp >> shift;
            }
        }

        if (shift > 0)
        {
            tmp <<= CharBits() - shift;
            *out++ = static_cast<uint8_t>(tmp);
        }

        return out;
    }

    const std::string& Str() const { return str_; }
    const size_t Size() const { return str_.size(); }
    const size_t DecodedSize() const { return DecodedSize(Size()); }
    static size_t DecodedSize(size_t digits)
    {
        const auto dv = std::div(static_cast<int>(digits) * DigitBits(), CharBits());
        const auto size = dv.quot + (dv.rem ? 1 : 0);
        return size;
    }

    friend static bool operator==(const Base32Code& x, const Base32Code& y) { return x.str_ == y.str_; }

    // Return the canonical form of digit
    // or InvalidDigit() if digit is not valid.
    static char Canonical(const char digit);

    static constexpr char InvalidDigit() { return '\0'; }

    // Return the Base32 value of the digit
    // or InvalidValue() if digit not valid.
    static signed char Value(const char digit);

    // Return the Base32 digit for value
    // or InvalidDigit() if value is out of range.
    static char Digit(const char value);

    static constexpr signed char InvalidValue() { return -1; }

    // The canonical digits in ascending order of value.
    static const char Digits[32];

    static constexpr int DigitBits() { return 5; }
    static constexpr int CharBits() { return CHAR_BIT; }
private:
    static constexpr uint16_t Mask() { return 0x001f; }

    std::string str_;
};

} // namespace base32

#endif // BASE32_BASE32CODE_H
