// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <cassert>
#include <cstddef>
#include <limits>
#include <sstream>
#include <type_traits>

using namespace std;

template <class CharT, size_t N, enable_if_t<is_same_v<CharT, char>, int> = 0>
constexpr const auto& choose_literal(const char (&s)[N], const wchar_t (&)[N]) noexcept {
    return s;
}
template <class CharT, size_t N, enable_if_t<is_same_v<CharT, wchar_t>, int> = 0>
constexpr const auto& choose_literal(const char (&)[N], const wchar_t (&ws)[N]) noexcept {
    return ws;
}

#define STATICALLY_WIDEN(CharT, S) ::choose_literal<CharT>(S, L##S)

template <class CharT, class F>
void test() {
    // LWG-4084 "std::fixed ignores std::uppercase"
    {
        auto s = (basic_ostringstream<CharT>{} << fixed << uppercase << numeric_limits<F>::infinity()).str();
        assert(s == STATICALLY_WIDEN(CharT, "INF"));
    }
    {
        auto s = (basic_ostringstream<CharT>{} << fixed << uppercase << numeric_limits<F>::quiet_NaN()).str();
        assert(s == STATICALLY_WIDEN(CharT, "NAN"));
    }
    // also test other combinations
    {
        auto s = (basic_ostringstream<CharT>{} << fixed << numeric_limits<F>::infinity()).str();
        assert(s == STATICALLY_WIDEN(CharT, "inf"));
    }
    {
        auto s = (basic_ostringstream<CharT>{} << fixed << numeric_limits<F>::quiet_NaN()).str();
        assert(s == STATICALLY_WIDEN(CharT, "nan"));
    }

    {
        auto s = (basic_ostringstream<CharT>{} << uppercase << numeric_limits<F>::infinity()).str();
        assert(s == STATICALLY_WIDEN(CharT, "INF"));
    }
    {
        auto s = (basic_ostringstream<CharT>{} << uppercase << numeric_limits<F>::quiet_NaN()).str();
        assert(s == STATICALLY_WIDEN(CharT, "NAN"));
    }
    {
        auto s = (basic_ostringstream<CharT>{} << numeric_limits<F>::infinity()).str();
        assert(s == STATICALLY_WIDEN(CharT, "inf"));
    }
    {
        auto s = (basic_ostringstream<CharT>{} << numeric_limits<F>::quiet_NaN()).str();
        assert(s == STATICALLY_WIDEN(CharT, "nan"));
    }

    {
        auto s = (basic_ostringstream<CharT>{} << scientific << uppercase << numeric_limits<F>::infinity()).str();
        assert(s == STATICALLY_WIDEN(CharT, "INF"));
    }
    {
        auto s = (basic_ostringstream<CharT>{} << scientific << uppercase << numeric_limits<F>::quiet_NaN()).str();
        assert(s == STATICALLY_WIDEN(CharT, "NAN"));
    }
    {
        auto s = (basic_ostringstream<CharT>{} << scientific << numeric_limits<F>::infinity()).str();
        assert(s == STATICALLY_WIDEN(CharT, "inf"));
    }
    {
        auto s = (basic_ostringstream<CharT>{} << scientific << numeric_limits<F>::quiet_NaN()).str();
        assert(s == STATICALLY_WIDEN(CharT, "nan"));
    }

    {
        auto s = (basic_ostringstream<CharT>{} << hexfloat << uppercase << numeric_limits<F>::infinity()).str();
        assert(s == STATICALLY_WIDEN(CharT, "INF"));
    }
    {
        auto s = (basic_ostringstream<CharT>{} << hexfloat << uppercase << numeric_limits<F>::quiet_NaN()).str();
        assert(s == STATICALLY_WIDEN(CharT, "NAN"));
    }
    {
        auto s = (basic_ostringstream<CharT>{} << hexfloat << numeric_limits<F>::infinity()).str();
        assert(s == STATICALLY_WIDEN(CharT, "inf"));
    }
    {
        auto s = (basic_ostringstream<CharT>{} << hexfloat << numeric_limits<F>::quiet_NaN()).str();
        assert(s == STATICALLY_WIDEN(CharT, "nan"));
    }
}

int main() {
    test<char, float>();
    test<char, double>();
    test<char, long double>();

    test<wchar_t, float>();
    test<wchar_t, double>();
    test<wchar_t, long double>();
}
