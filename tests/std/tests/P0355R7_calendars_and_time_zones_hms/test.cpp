// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <cassert>
#include <chrono>

using namespace std;
using namespace std::chrono;

constexpr void am_pm() {
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
template <intmax_t N, intmax_t D>
constexpr auto width() {
    return hh_mm_ss<duration<int, ratio<N, D>>>::fractional_width;
}

constexpr void fractional_width() {
    static_assert(width<hours>() == 0);
    static_assert(width<minutes>() == 0);
    static_assert(width<hours>() == 0);
    static_assert(width<seconds>() == 0);
    static_assert(width<milliseconds>() == 3);
    static_assert(width<microseconds>() == 6);
    static_assert(width<nanoseconds>() == 9);

    static_assert(width<1, 2>() == 1);
    static_assert(width<1, 3>() == 6);
    static_assert(width<1, 4>() == 2);
    static_assert(width<1, 5>() == 1);
    static_assert(width<1, 6>() == 6);
    static_assert(width<1, 7>() == 6);
    static_assert(width<1, 8>() == 3);
    static_assert(width<1, 9>() == 6);
    static_assert(width<1, 10>() == 1);
    static_assert(width<756, 625>() == 4);
    static_assert(width<3780, 625>() == 3);
}
constexpr void is_negative() {
    assert(hh_mm_ss<days>(days{-1}).is_negative());
    assert(!hh_mm_ss<days>(days{1}).is_negative());

    assert(hh_mm_ss<hours>(-1h).is_negative());
    assert(!hh_mm_ss<hours>(1h).is_negative());

    assert(hh_mm_ss<minutes>(-1min).is_negative());
    assert(!hh_mm_ss<minutes>(1min).is_negative());

    assert(hh_mm_ss<seconds>(-1s).is_negative());
    assert(!hh_mm_ss<seconds>(1s).is_negative());

    assert(hh_mm_ss<milliseconds>(-1ms).is_negative());
    assert(!hh_mm_ss<milliseconds>(1ms).is_negative());

    assert(hh_mm_ss<microseconds>(-1us).is_negative());
    assert(!hh_mm_ss<microseconds>(1us).is_negative());

    assert(hh_mm_ss<nanoseconds>(-1ns).is_negative());
    assert(!hh_mm_ss<nanoseconds>(1ns).is_negative());

}

constexpr auto ones = 1h + 1min + 1s + 1ms;

constexpr void hour() {
    assert(hh_mm_ss(days{1}).hours() == 24h);
    assert(hh_mm_ss(ones).hours() == 1h);
    assert(hh_mm_ss(-ones).hours() == 1h);    
    assert(hh_mm_ss(59min).hours() == 0h);
}

constexpr void mins() {
    assert(hh_mm_ss(ones).minutes() == 1min);
    assert(hh_mm_ss(-ones).minutes() == 1min);
    assert(hh_mm_ss(59s).minutes() == 0min);
}

constexpr void secs() {
    assert(hh_mm_ss(ones).seconds() == 1s);
    assert(hh_mm_ss(-ones).seconds() == 1s);
    assert(hh_mm_ss(999ms).seconds() == 0s);
}

constexpr void subsecs() {
    assert(hh_mm_ss(ones).subseconds() == 1ms);
    assert(hh_mm_ss(-ones).subseconds() == 1ms);
    assert(hh_mm_ss(999us).subseconds() == 999us);
    assert(hh_mm_ss(duration_cast<milliseconds>(999us)).subseconds() == 0ms);
}

constexpr void to_duration() {
    assert(hh_mm_ss(ones).to_duration() == ones);
    assert(hh_mm_ss(-ones).to_duration() == -ones);

    hh_mm_ss<milliseconds> hms(50ms);
    milliseconds mili = static_cast<milliseconds>(hms);
    static_assert(is_same_v<decltype(hms.to_duration()), milliseconds>);
    assert(hms.to_duration() == mili);
}

constexpr bool test() {
    am_pm();
    make12_24();
    fractional_width();
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