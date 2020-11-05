// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <cassert>
#include <chrono>
#include <climits>
#include <locale>
#include <ratio>
#include <sstream>
#include <string>
#include <utility>

using namespace std;
using chrono::duration;

template <class CharT, class Rep, class Period>
bool test_duration_basic_out(const duration<Rep, Period>& d, const CharT* expected) {
    basic_ostringstream<CharT> ss;

    ss << d;
    return ss.str() == expected;
}

#define WIDEN(TYPE, STR) get<const TYPE*>(make_pair(STR, L##STR));

template <class CharT>
bool test_duration_locale_out() {
    basic_stringstream<CharT> ss;
    const duration<double> d{0.140625};
    ss.precision(3);
    ss << d;
    ss.setf(ios_base::scientific, ios_base::floatfield);
    ss << ' ' << d;

    basic_string<CharT> expected = WIDEN(CharT, "0.141s 1.406e-01s");

#ifdef _DEBUG
#define DEFAULT_IDL_SETTING 2
#else
#define DEFAULT_IDL_SETTING 0
#endif

#if !defined(_DLL) || _ITERATOR_DEBUG_LEVEL == DEFAULT_IDL_SETTING
    // When linking dynamically, user-defined facets are incompatible with non-default _ITERATOR_DEBUG_LEVEL settings.
    struct comma : numpunct<CharT> {
        CharT do_decimal_point() const {
            return ',';
        }
    };

    ss.imbue(locale(ss.getloc(), new comma));
    ss << ' ' << d;
    expected += WIDEN(CharT, " 1,406e-01s");
#endif // !defined(_DLL) || _ITERATOR_DEBUG_LEVEL == DEFAULT_IDL_SETTING

    return ss.str() == expected;
}

#undef WIDEN

void test_duration_output() {
    using LongRatio = ratio<INTMAX_MAX - 1, INTMAX_MAX>;
    assert(test_duration_basic_out(duration<int, atto>{1}, "1as"));
    assert(test_duration_basic_out(duration<int, femto>{2}, "2fs"));
    assert(test_duration_basic_out(duration<int, pico>{3}, "3ps"));
    assert(test_duration_basic_out(duration<int, nano>{42}, "42ns"));
    assert(test_duration_basic_out(duration<int, micro>{42}, "42us"));
    assert(test_duration_basic_out(duration<int, milli>{42}, "42ms"));
    assert(test_duration_basic_out(duration<int, centi>{42}, "42cs"));
    assert(test_duration_basic_out(duration<int, deci>{42}, "42ds"));
    assert(test_duration_basic_out(duration<int, ratio<1>>{42}, "42s"));
    assert(test_duration_basic_out(duration<int, deca>{42}, "42das"));
    assert(test_duration_basic_out(duration<int, hecto>{42}, "42hs"));
    assert(test_duration_basic_out(duration<int, kilo>{42}, "42ks"));
    assert(test_duration_basic_out(duration<int, mega>{42}, "42Ms"));
    assert(test_duration_basic_out(duration<int, giga>{42}, "42Gs"));
    assert(test_duration_basic_out(duration<int, tera>{42}, "42Ts"));
    assert(test_duration_basic_out(duration<int, peta>{42}, "42Ps"));
    assert(test_duration_basic_out(duration<int, exa>{42}, "42Es"));
    assert(test_duration_basic_out(duration<int, ratio<60>>{42}, "42min"));
    assert(test_duration_basic_out(duration<int, ratio<60 * 60>>{42}, "42h"));
    assert(test_duration_basic_out(duration<int, ratio<60 * 60 * 24>>{42}, "42d"));

    assert(test_duration_basic_out(duration<int, ratio<2>>{24}, "24[2]s"));
    assert(test_duration_basic_out(duration<int, ratio<1, 2>>{24}, "24[1/2]s"));
    assert(test_duration_basic_out(duration<int, ratio<22, 7>>{24}, "24[22/7]s"));
    assert(test_duration_basic_out(duration<int, LongRatio>{24}, "24[9223372036854775806/9223372036854775807]s"));

    assert(test_duration_basic_out(duration<double>{0.140625}, "0.140625s"));
    assert(test_duration_locale_out<char>());

    assert(test_duration_basic_out(duration<int, atto>{1}, L"1as"));
    assert(test_duration_basic_out(duration<int, femto>{2}, L"2fs"));
    assert(test_duration_basic_out(duration<int, pico>{3}, L"3ps"));
    assert(test_duration_basic_out(duration<int, nano>{42}, L"42ns"));
    assert(test_duration_basic_out(duration<int, micro>{42}, L"42us"));
    assert(test_duration_basic_out(duration<int, milli>{42}, "42ms"));
    assert(test_duration_basic_out(duration<int, centi>{42}, L"42cs"));
    assert(test_duration_basic_out(duration<int, deci>{42}, L"42ds"));
    assert(test_duration_basic_out(duration<int, ratio<1>>{42}, L"42s"));
    assert(test_duration_basic_out(duration<int, deca>{42}, L"42das"));
    assert(test_duration_basic_out(duration<int, hecto>{42}, L"42hs"));
    assert(test_duration_basic_out(duration<int, kilo>{42}, L"42ks"));
    assert(test_duration_basic_out(duration<int, mega>{42}, L"42Ms"));
    assert(test_duration_basic_out(duration<int, giga>{42}, L"42Gs"));
    assert(test_duration_basic_out(duration<int, tera>{42}, L"42Ts"));
    assert(test_duration_basic_out(duration<int, peta>{42}, L"42Ps"));
    assert(test_duration_basic_out(duration<int, exa>{42}, L"42Es"));
    assert(test_duration_basic_out(duration<int, ratio<60>>{42}, L"42min"));
    assert(test_duration_basic_out(duration<int, ratio<60 * 60>>{42}, L"42h"));
    assert(test_duration_basic_out(duration<int, ratio<60 * 60 * 24>>{42}, L"42d"));

    assert(test_duration_basic_out(duration<int, ratio<2>>{24}, L"24[2]s"));
    assert(test_duration_basic_out(duration<int, ratio<1, 2>>{24}, L"24[1/2]s"));
    assert(test_duration_basic_out(duration<int, ratio<22, 7>>{24}, L"24[22/7]s"));
    assert(test_duration_basic_out(duration<int, LongRatio>{24}, L"24[9223372036854775806/9223372036854775807]s"));

    assert(test_duration_basic_out(duration<double>{0.140625}, L"0.140625s"));
    assert(test_duration_locale_out<wchar_t>());
}

int main() {
    test_duration_output();
    return 0;
}
