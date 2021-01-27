// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <cassert>
#include <chrono>
#include <cstdint>
#include <ratio>
#include <type_traits>

using namespace std;
using namespace std::chrono;

using hms_hours   = hh_mm_ss<hours>;
using f_hours     = duration<float, ratio<3600>>;
using f_hms_hours = hh_mm_ss<f_hours>;

constexpr void am_pm() {
    static_assert(noexcept(is_am(hours{})));
    static_assert(noexcept(is_pm(hours{})));

    for (hours i = 0h; i < 12h; ++i) {
        assert(is_am(i));
        assert(!is_pm(i));
    }
    for (hours i = 12h; i < 24h; ++i) {
        assert(!is_am(i));
        assert(is_pm(i));
    }
}

constexpr void make12_24() {
    static_assert(noexcept(make12(hours{})));
    static_assert(noexcept(make24(hours{}, true)));

    assert(make12(1h) == 1h);
    assert(make12(13h) == 1h);
    assert(make12(0h) == 12h);
    assert(make12(12h) == 12h);

    assert(make24(12h, true) == 12h);
    assert(make24(12h, false) == 0h);
    assert(make24(5h, true) == 17h);
    assert(make24(5h, false) == 5h);
}

template <class D>
constexpr auto width() {
    return hh_mm_ss<D>::fractional_width;
}
template <class T, intmax_t N, intmax_t D>
constexpr auto width() {
    return hh_mm_ss<duration<T, ratio<N, D>>>::fractional_width;
}

constexpr void fractional_width() {
    static_assert(width<days>() == 0);
    static_assert(width<hours>() == 0);
    static_assert(width<minutes>() == 0);
    static_assert(width<seconds>() == 0);
    static_assert(width<milliseconds>() == 3);
    static_assert(width<microseconds>() == 6);
    static_assert(width<nanoseconds>() == 9);

    static_assert(width<int, 1, 2>() == 1);
    static_assert(width<int, 1, 3>() == 6);
    static_assert(width<int, 1, 4>() == 2);
    static_assert(width<int, 1, 5>() == 1);
    static_assert(width<int, 1, 6>() == 6);
    static_assert(width<int, 1, 7>() == 6);
    static_assert(width<int, 1, 8>() == 3);
    static_assert(width<int, 1, 9>() == 6);
    static_assert(width<int, 1, 10>() == 1);
    static_assert(width<int, 756, 625>() == 4);
    static_assert(width<int, 3780, 625>() == 3);
    static_assert(width<int, 1, 400000000000000000LL>() == 6);
    // static_assert(width<int, 1, 2000000000000000000LL>() == 6); overflows

    static_assert(width<float, 1, 2>() == 1);
    static_assert(width<float, 1, 3>() == 6);
    static_assert(width<float, 1, 4>() == 2);
    static_assert(width<float, 1, 5>() == 1);
    static_assert(width<float, 1, 6>() == 6);
    static_assert(width<float, 1, 7>() == 6);
    static_assert(width<float, 1, 8>() == 3);
    static_assert(width<float, 1, 9>() == 6);
    static_assert(width<float, 1, 10>() == 1);
    static_assert(width<float, 756, 625>() == 4);
    static_assert(width<float, 3780, 625>() == 3);
    static_assert(width<float, 1, 400000000000000000LL>() == 6);
    // static_assert(width<float, 1, 2000000000000000000LL>() == 6); overflows
}

constexpr void constructor() {
    static_assert(noexcept(hms_hours{}));
    static_assert(noexcept(f_hms_hours{}));

    assert(hms_hours{}.hours() == hms_hours{hours::zero()}.hours());
    assert(hms_hours{}.minutes() == hms_hours{hours::zero()}.minutes());
    assert(hms_hours{}.seconds() == hms_hours{hours::zero()}.seconds());
    assert(hms_hours{}.subseconds() == hms_hours{hours::zero()}.subseconds());

    assert(f_hms_hours{}.hours() == f_hms_hours{hours::zero()}.hours());
    assert(f_hms_hours{}.minutes() == f_hms_hours{hours::zero()}.minutes());
    assert(f_hms_hours{}.seconds() == f_hms_hours{hours::zero()}.seconds());
    assert(f_hms_hours{}.subseconds() == f_hms_hours{hours::zero()}.subseconds());
}

constexpr void is_negative() {
    static_assert(noexcept(hms_hours{}.is_negative()));
    static_assert(noexcept(f_hms_hours{}.is_negative()));

    assert(hh_mm_ss<days>(days{-1}).is_negative());
    assert(!hh_mm_ss<days>(days{1}).is_negative());

    assert(hh_mm_ss<hours>{-1h}.is_negative());
    assert(!hh_mm_ss<hours>{1h}.is_negative());

    assert(hh_mm_ss<minutes>{-1min}.is_negative());
    assert(!hh_mm_ss<minutes>{1min}.is_negative());

    assert(hh_mm_ss<seconds>{-1s}.is_negative());
    assert(!hh_mm_ss<seconds>{1s}.is_negative());

    assert(hh_mm_ss<milliseconds>{-1ms}.is_negative());
    assert(!hh_mm_ss<milliseconds>{1ms}.is_negative());

    assert(hh_mm_ss<microseconds>{-1us}.is_negative());
    assert(!hh_mm_ss<microseconds>{1us}.is_negative());

    assert(hh_mm_ss<nanoseconds>{-1ns}.is_negative());
    assert(!hh_mm_ss<nanoseconds>{1ns}.is_negative());

    assert(f_hms_hours{f_hours{-1.f}}.is_negative());
    assert(!f_hms_hours{f_hours{1.f}}.is_negative());
}

constexpr auto ones = 1h + 1min + 1s + 1ms;

constexpr void hour() {
    static_assert(noexcept(hms_hours{}.hours()));
    static_assert(noexcept(f_hms_hours{}.hours()));

    assert(hh_mm_ss(days{1}).hours() == 24h);
    assert(hh_mm_ss(ones).hours() == 1h);
    assert(hh_mm_ss(-ones).hours() == 1h);
    assert(hh_mm_ss(59min).hours() == 0h);
    assert(f_hms_hours{f_hours{1.f}}.hours() == 1h);
}

constexpr void mins() {
    static_assert(noexcept(hms_hours{}.minutes()));
    static_assert(noexcept(f_hms_hours{}.minutes()));

    assert(hh_mm_ss(ones).minutes() == 1min);
    assert(hh_mm_ss(-ones).minutes() == 1min);
    assert(hh_mm_ss(59s).minutes() == 0min);
    assert(f_hms_hours{f_hours{0.0166667f}}.minutes() == 1min);
}

constexpr void secs() {
    static_assert(noexcept(hms_hours{}.seconds()));
    static_assert(noexcept(f_hms_hours{}.seconds()));

    assert(hh_mm_ss(ones).seconds() == 1s);
    assert(hh_mm_ss(-ones).seconds() == 1s);
    assert(hh_mm_ss(999ms).seconds() == 0s);
    assert(f_hms_hours{f_hours{0.000277778f}}.seconds() == 1s);
}

constexpr void subsecs() {
    static_assert(noexcept(hms_hours{}.subseconds()));
    static_assert(noexcept(f_hms_hours{}.subseconds()));

    assert(hh_mm_ss(ones).subseconds() == 1ms);
    assert(hh_mm_ss(-ones).subseconds() == 1ms);
    assert(hh_mm_ss(999us).subseconds() == 999us);
    assert(hh_mm_ss(duration_cast<milliseconds>(999us)).subseconds() == 0ms);
    using f_hms_milli = hh_mm_ss<duration<float, milli>>;
    assert(f_hms_milli{1ms}.subseconds() == 1ms);
}

constexpr void to_duration() {
    using precision   = hms_hours::precision;
    using f_precision = f_hms_hours::precision;

    static_assert(noexcept(hms_hours{}.to_duration()));
    static_assert(noexcept(static_cast<precision>(hms_hours{})));
    static_assert(noexcept(f_hms_hours{}.to_duration()));
    static_assert(noexcept(static_cast<f_precision>(f_hms_hours{})));

    assert(hh_mm_ss(ones).to_duration() == ones);
    assert(hh_mm_ss(-ones).to_duration() == -ones);
    assert(f_hms_hours{f_hours{1.f}}.to_duration() == 1h);
    assert(f_hms_hours{f_hours{-1.f}}.to_duration() == -1h);

    hh_mm_ss<milliseconds> hms(50ms);
    milliseconds milli_val = static_cast<milliseconds>(hms);
    static_assert(is_same_v<decltype(hms.to_duration()), milliseconds>);
    assert(hms.to_duration() == milli_val);

    f_hms_hours fhms{f_hours{1}};
    auto fhours_val = static_cast<f_precision>(fhms);
    static_assert(is_same_v<decltype(fhms.to_duration()), f_precision>);
    static_assert(is_same_v<duration<float>, f_precision>);
    assert(fhms.to_duration() == fhours_val);
}

constexpr bool test() {
    am_pm();
    make12_24();
    fractional_width();
    constructor();
    is_negative();
    hour();
    mins();
    secs();
    subsecs();
    to_duration();
    return true;
}

int main() {
    test();
    static_assert(test());
}
