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

#include <limits>
#include <vector>
#include <string>
#include "gtest/gtest.h"
#include "Base32Code.h"

namespace {
using ::base32::Base32Code;

const int L = 0x0100;
const int N = 0x1000;

typedef std::pair<std::vector<uint8_t>, std::string> TestPair;

std::vector<TestPair> ExpectedEncodings()
{
    return {
        { {}, "" },
        { { 0b00000000 }, "00" },
        { { 0b11111000 }, "Z0" },
        { { 0b11111000, 0b00111110 }, "Z0Z0" },
        { { 0b00001000, 0b10000110 }, "1230" },
        { { 0b00000111, 0b11000001, 0b11110000 }, "0Z0Z0" },
        { { 0b00100001, 0b01001100, 0b01110000 }, "45670" },
        { { 0b11111000, 0b00111110, 0b00001111, 0b10000000 }, "Z0Z0Z00" },
        { { 0b01000010, 0b01010100, 0b10110110, 0b00000000 }, "89ABC00" },
        { { 0b00000111, 0b11000001, 0b11110000, 0b01111100 }, "0Z0Z0Z0" },
        { { 0b01101011, 0b10011111, 0b00001000, 0b11001000 }, "DEFGHJ0" },
        { { 0b11111000, 0b00111110, 0b00001111, 0b10000011, 0b11100000 }, "Z0Z0Z0Z0" },
        { { 0b10011101, 0b00101011, 0b01101011, 0b11100011, 0b00100000 }, "KMNPQRS0" },
        { { 0b00000111, 0b11000001, 0b11110000, 0b01111100, 0b00011111 }, "0Z0Z0Z0Z" },
        { { 0b11010110, 0b11111001, 0b11011111, 0b01111100, 0b00000001 }, "TVWXYZ01" },
        { { 0b11111000, 0b00111110, 0b00001111, 0b10000011, 0b11100000, 0b11111000 }, "Z0Z0Z0Z0Z0" },
    };
};

std::vector<TestPair> ExpectedDecodings()
{
    return {
        { {}, "" },
        { { 0b00000000 }, "0" },
        { { 0b11111000 }, "Z" },
        { { 0b00000111, 0b11000000 }, "0Z" },
        { { 0b11111000, 0b00111110 }, "Z0Z" },
        { { 0b00001000, 0b10000110 }, "123" },
        { { 0b00000111, 0b11000001, 0b11110000 }, "0Z0Z" },
        { { 0b00100001, 0b01001100, 0b01110000 }, "4567" },
        { { 0b11111000, 0b00111110, 0b00001111, 0b10000000 }, "Z0Z0Z" },
        { { 0b01000010, 0b01010100, 0b10110110, 0b00000000 }, "89ABC" },
        { { 0b00000111, 0b11000001, 0b11110000, 0b01111100 }, "0Z0Z0Z" },
        { { 0b01101011, 0b10011111, 0b00001000, 0b11001000 }, "DEFGHJ" },
        { { 0b11111000, 0b00111110, 0b00001111, 0b10000011, 0b11100000 }, "Z0Z0Z0Z" },
        { { 0b10011101, 0b00101011, 0b01101011, 0b11100011, 0b00100000 }, "KMNPQRS" },
        { { 0b00000111, 0b11000001, 0b11110000, 0b01111100, 0b00011111 }, "0Z0Z0Z0Z" },
        { { 0b11010110, 0b11111001, 0b11011111, 0b01111100, 0b00000001 }, "TVWXYZ01" },
        { { 0b11111000, 0b00111110, 0b00001111, 0b10000011, 0b11100000, 0b11111000 }, "Z0Z0Z0Z0Z" },
    };
};

constexpr int TotalDigits()
{
    return 1 + std::numeric_limits<char>::max() - std::numeric_limits<char>::min();
}

const std::string& CanonicalDigits()
{
    static std::string s(std::begin(Base32Code::Digits), std::end(Base32Code::Digits));
    return s;
}

const std::string& NonCanonicalDigits()
{
    static std::string s("oOlLiI" "abcdefghjkmnpqrstvwxyz");
    return s;
}

const std::string& ValidDigits()
{
    static std::string s = CanonicalDigits() + NonCanonicalDigits();
    return s;
}

const std::string& InvalidDigits()
{
    static std::string s = [] {
        std::string invalids;

        for (int digit = std::numeric_limits<char>::min();
             digit <= std::numeric_limits<char>::max();
             ++digit) {
            if (ValidDigits().find(digit) == std::string::npos) {
                invalids.push_back(digit);
            }
        }
        return invalids;
    }();

    return s;
}

TEST(Base32Code, Encode)
{
    for (const auto& x : ExpectedEncodings())
    {
        const auto actual = Base32Code::Encode(x.first);
        const auto expected = Base32Code(x.second);
        ASSERT_EQ(expected, actual);
    }
}

TEST(Base32Code, Decode)
{
    for (const auto& x : ExpectedDecodings())
    {
        const auto& expected = x.first;

        const Base32Code code(x.second);
        std::vector<uint8_t> actual(code.DecodedSize());
        code.Decode(actual.data());

        ASSERT_EQ(expected, actual);
    }
}

TEST(Base32Code, RoundtripChars)
{
    for (int l = 0; l < L; ++l)
    {
        std::string input(l, '!');
        std::vector<uint8_t> tmp(Base32Code::DecodedSize(l));

        const int padding_bits = (static_cast<int>(tmp.size()) * Base32Code::CharBits()) - (l * Base32Code::DigitBits());
        const auto dv = std::div(padding_bits, Base32Code::DigitBits());
        const int padding_digits = dv.quot + (dv.rem ? 1 : 0);

        for (int n = 0; n < N; ++n)
        {
            for (auto& x : input)
            {
                x = Base32Code::Digit(std::rand() % 32);
            }

            Base32Code code(input);
            code.Decode(tmp.data());

            auto output = Base32Code::Encode(tmp).Str();

            for (int i = 0; i < padding_digits; ++i)
            {
                ASSERT_EQ('0', output.back());
                output.pop_back();
            }

            ASSERT_EQ(input, output);
        }
    }
}

TEST(Base32Code, RoundtripBytes)
{
    for (int l = 0; l < L; ++l)
    {
        const bool padding = (l * Base32Code::CharBits()) % 5;
        std::vector<uint8_t> input(l);
        std::vector<uint8_t> output(l);

        for (int n = 0; n < N; ++n)
        {
            if (padding) {
                output.push_back(~0);
            }

            for (auto& x : input)
            {
                x = std::rand();
            }

            auto code = Base32Code::Encode(input);
            ASSERT_EQ(output.size(), code.DecodedSize());
            code.Decode(output.data());

            if (padding)
            {
                ASSERT_EQ(0, output.back());
                output.pop_back();
            }

            ASSERT_EQ(input, output);
        }
    }
}

TEST(Base32Code, IsValid)
{
    ASSERT_TRUE(Base32Code::IsValid(ValidDigits()));

    for (const char digit : ValidDigits())
    {
        ASSERT_TRUE(Base32Code::IsValid(digit));
    }

    for (const char digit : InvalidDigits())
    {
        ASSERT_FALSE(Base32Code::IsValid(digit));
    }

    ASSERT_EQ(TotalDigits(), ValidDigits().size() + InvalidDigits().size());
}

TEST(Base32Code, Canonical)
{
    for (const char digit : CanonicalDigits())
    {
        ASSERT_EQ(digit, Base32Code::Canonical(digit));
    }

    for (const char digit : NonCanonicalDigits())
    {
        ASSERT_NE(digit, Base32Code::Canonical(digit));
    }

    for (const char digit : InvalidDigits())
    {
        ASSERT_EQ(Base32Code::InvalidDigit(), Base32Code::Canonical(digit));
    }
}

TEST(Base32Code, CanonicalForm)
{
    const std::string    input("0123456789abcdefghjkmnpqrstvwxyzoOlLiI");
    const std::string expected("0123456789ABCDEFGHJKMNPQRSTVWXYZ001111");
    ASSERT_EQ(expected, Base32Code(expected).Str());
    ASSERT_EQ(expected, Base32Code(input).Str());

    ASSERT_EQ(CanonicalDigits(), Base32Code(CanonicalDigits()).Str());
}

TEST(Base32Code, Value)
{
    ASSERT_EQ(32, CanonicalDigits().size());

    for (int i = 0; i < CanonicalDigits().size(); ++i)
    {
        ASSERT_EQ(i, Base32Code::Value(CanonicalDigits()[i]));
    }

    for (const char digit : std::string("oO"))
    {
        ASSERT_EQ(Base32Code::Value('0'), Base32Code::Value(digit));
    }

    for (const char digit : std::string("lLiI"))
    {
        ASSERT_EQ(Base32Code::Value('1'), Base32Code::Value(digit));
    }

    for (const char digit : InvalidDigits())
    {
        ASSERT_EQ(Base32Code::InvalidValue(), Base32Code::Value(digit));
    }
}

TEST(Base32Code, DecodedSize)
{
    ASSERT_EQ(0, Base32Code::DecodedSize(0));
    ASSERT_EQ(1, Base32Code::DecodedSize(1));
    ASSERT_EQ(2, Base32Code::DecodedSize(2));
    ASSERT_EQ(2, Base32Code::DecodedSize(3));
    ASSERT_EQ(3, Base32Code::DecodedSize(4));
    ASSERT_EQ(4, Base32Code::DecodedSize(5));
    ASSERT_EQ(4, Base32Code::DecodedSize(6));
    ASSERT_EQ(5, Base32Code::DecodedSize(7));
    ASSERT_EQ(5, Base32Code::DecodedSize(8));
    ASSERT_EQ(6, Base32Code::DecodedSize(9));

    ASSERT_EQ(9, Base32Code::DecodedSize(14));
    ASSERT_EQ(10, Base32Code::DecodedSize(15));
    ASSERT_EQ(10, Base32Code::DecodedSize(16));
    ASSERT_EQ(11, Base32Code::DecodedSize(17));

    ASSERT_EQ(49, Base32Code::DecodedSize(78));
    ASSERT_EQ(50, Base32Code::DecodedSize(79));
    ASSERT_EQ(50, Base32Code::DecodedSize(80));
    ASSERT_EQ(51, Base32Code::DecodedSize(81));
}

} // namespace
