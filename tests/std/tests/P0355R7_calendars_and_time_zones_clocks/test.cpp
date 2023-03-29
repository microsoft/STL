// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <algorithm>
#include <cassert>
#include <chrono>
#include <cmath>
#include <compare>
#include <filesystem>
#include <iterator>
#include <type_traits>
#include <utility>

#include <timezone_data.hpp>

using namespace std;
using namespace std::chrono;

struct not_a_clock {
    bool rep();
    static char period;
    int duration();
    static float time_point;
    using is_steady = long;
    static int now;
};

struct real_fake_clock {
    using rep        = bool;
    using period     = char;
    using duration   = float;
    using time_point = int;
    static long is_steady;
    static short now();
};

struct no_rep {
    using period     = char;
    using duration   = float;
    using time_point = int;
    static long is_steady;
    static short now();
};

struct no_period {
    using rep        = bool;
    using duration   = float;
    using time_point = int;
    static long is_steady;
    static short now();
};

struct no_duration {
    using rep        = bool;
    using period     = char;
    using time_point = int;
    static long is_steady;
    static short now();
};

struct no_time_point {
    using rep      = bool;
    using period   = char;
    using duration = float;
    static long is_steady;
    static short now();
};

struct no_steady {
    using rep        = bool;
    using period     = char;
    using duration   = float;
    using time_point = int;
    static short now();
};

struct no_now {
    using rep        = bool;
    using period     = char;
    using duration   = float;
    using time_point = int;
    static long is_steady;
};

static_assert(is_clock<steady_clock>::value, "steady_clock is not a clock");
static_assert(is_clock_v<steady_clock>, "steady_clock is not a clock");
static_assert(is_clock_v<real_fake_clock>, "real_fake_clock is not a clock");
static_assert(!is_clock_v<not_a_clock>, "not_a_clock is a clock");

static_assert(!is_clock_v<no_rep>, "no_rep is a clock");
static_assert(!is_clock_v<no_period>, "no_period is a clock");
static_assert(!is_clock_v<no_duration>, "no_duration is a clock");
static_assert(!is_clock_v<no_time_point>, "no_time_point is a clock");
static_assert(!is_clock_v<no_steady>, "no_steady is a clock");
static_assert(!is_clock_v<no_now>, "no_now is a clock");

void test_is_leap_second(const year_month_day& ymd) {
    const sys_days ls{ymd};
    const auto& leap_seconds = get_tzdb().leap_seconds;
    assert(find(leap_seconds.begin(), leap_seconds.end(), ls + days{1}) != leap_seconds.end());
    assert(get_leap_second_info(utc_clock::from_sys<seconds>(ls) + days{1}).is_leap_second);
}

constexpr bool test_leap_second() {
    constexpr int jun_leap_second_years[] = {1972, 1981, 1982, 1983, 1985, 1992, 1993, 1994, 1997, 2012, 2015};
    constexpr int dec_leap_second_years[] = {
        1972, 1973, 1974, 1975, 1976, 1977, 1978, 1979, 1987, 1989, 1990, 1995, 1998, 2005, 2008, 2016};
    static_assert(size(jun_leap_second_years) + size(dec_leap_second_years) == 27);

    if (!is_constant_evaluated()) {
        for (const auto& ls_year : jun_leap_second_years) {
            test_is_leap_second(30d / June / year{ls_year});
        }
        for (const auto& ls_year : dec_leap_second_years) {
            test_is_leap_second(31d / December / year{ls_year});
        }
    }

    constexpr leap_second leap{sys_seconds{42s}, true, 0s};
    leap_second leap2(leap); // copy construct
    leap_second leap3{sys_seconds{41s}, true, 0s};
    leap3 = leap2; // copy assign

    constexpr sys_seconds smaller{41s};
    constexpr sys_seconds equal{42s};
    constexpr sys_seconds larger{43s};

    assert(equal == leap);
    assert(leap == equal);
    static_assert(noexcept(equal == leap));
    static_assert(noexcept(leap == equal));

    assert(leap < larger);
    assert(smaller < leap);
    static_assert(noexcept(leap < larger));
    static_assert(noexcept(smaller < leap));

    assert(larger > leap);
    assert(leap > smaller);
    static_assert(noexcept(larger > leap));
    static_assert(noexcept(leap > smaller));

    assert(equal <= leap);
    assert(smaller <= leap);
    assert(leap <= equal);
    assert(leap <= larger);
    static_assert(noexcept(equal <= leap));
    static_assert(noexcept(leap <= equal));

    assert(equal >= leap);
    assert(larger >= leap);
    assert(leap >= equal);
    assert(leap >= smaller);
    static_assert(noexcept(equal >= leap));
    static_assert(noexcept(leap >= equal));

#ifdef __cpp_lib_concepts
    static_assert(is_eq(leap <=> equal));
    static_assert(is_lt(leap <=> larger));
    static_assert(is_gt(leap <=> smaller));
    static_assert(is_lteq(leap <=> larger));
    static_assert(is_gteq(leap <=> smaller));
    static_assert(is_lteq(leap <=> equal));
    static_assert(is_gteq(leap <=> equal));
    static_assert(noexcept(leap <=> equal));

    static_assert(is_eq(leap <=> leap_second{equal, true, 0s}));
    static_assert(is_lt(leap <=> leap_second{larger, true, 0s}));
    static_assert(is_gt(leap <=> leap_second{smaller, true, 0s}));
    static_assert(is_lteq(leap <=> leap_second{larger, true, 0s}));
    static_assert(is_gteq(leap <=> leap_second{smaller, true, 0s}));
    static_assert(is_lteq(leap <=> leap_second{equal, true, 0s}));
    static_assert(is_gteq(leap <=> leap_second{equal, true, 0s}));
    static_assert(noexcept(leap <=> leap_second{equal, true, 0s}));
#endif // __cpp_lib_concepts

    static_assert(noexcept(leap.date()));
    static_assert(noexcept(leap.value()));
    static_assert(leap_second{sys_seconds{42s}, true, 0s}.date() == sys_seconds{42s});
    static_assert(leap_second{sys_seconds{42s}, true, 0s}.value() == 1s);
    static_assert(leap_second{sys_seconds{42s}, false, 0s}.value() == -1s);

    return true;
}

constexpr bool operator==(const leap_second_info& lhs, const leap_second_info& rhs) {
    return lhs.is_leap_second == rhs.is_leap_second && lhs.elapsed == rhs.elapsed;
}

template <class DurationRep>
void test_leap_second_info(const leap_second& leap, seconds accum) {
    const bool is_positive = (leap.value() == 1s);
    // First UTC time when leap is counted, before insertion of a positive leap, after insertion of a negative one.
    const utc_time<duration<DurationRep>> utc_leap{leap.date().time_since_epoch() + accum + (is_positive ? 0s : -1s)};

    auto lsi = get_leap_second_info(utc_leap - 1s);
    assert(lsi == (leap_second_info{false, accum}));

    lsi = get_leap_second_info(utc_leap - 500ms);
    assert(lsi == (leap_second_info{false, accum}));

    accum += leap.value();
    lsi = get_leap_second_info(utc_leap);
    assert(lsi == (leap_second_info{is_positive, accum}));

    lsi = get_leap_second_info(utc_leap + 500ms);
    assert(lsi == (leap_second_info{is_positive, accum}));

    lsi = get_leap_second_info(utc_leap + 1s);
    assert(lsi == (leap_second_info{false, accum}));
}

template <class Duration>
void test_utc_clock_to_sys(const leap_second& leap) {
    sys_time<Duration> before_leap;
    if constexpr (is_integral_v<typename Duration::rep>) {
        before_leap = leap.date() - Duration{1};
    } else {
        before_leap =
            sys_time<Duration>{Duration{nextafter(leap.date().time_since_epoch().count(), typename Duration::rep{0})}};
    }

    auto u = utc_clock::from_sys(before_leap); // just before leap second
    assert(utc_clock::from_sys(utc_clock::to_sys(u)) == u);
    if (leap.value() == 1s) {
        u += Duration{1};
        assert(utc_clock::to_sys(u) == before_leap); // during
    } else {
        assert(utc_clock::from_sys(utc_clock::to_sys(u)) == u);
    }

    u += 1s;
    assert(utc_clock::from_sys(utc_clock::to_sys(u)) == u); // just after
}

template <class Duration>
void test_file_clock_from_utc(const leap_second& leap) {
    static_assert(is_same_v<filesystem::file_time_type::clock, file_clock>);

    const auto file_leap = clock_cast<file_clock>(leap.date());

    auto u = utc_clock::from_sys(leap.date() - Duration{1}); // just before leap second
    assert(file_clock::to_utc(file_clock::from_utc(u)) == u);

    if (leap.value() == 1s && leap.date() <= sys_days{1d / January / 2017y}) {
        u += Duration{1};
        assert(file_clock::from_utc(u) == file_leap - Duration{1}); // during
    } else {
        assert(file_clock::to_utc(file_clock::from_utc(u)) == u);
    }

    u += 1s;
    assert(file_clock::to_utc(file_clock::from_utc(u)) == u); // just after
}

void test_utc_clock_from_sys(const leap_second& leap, seconds offset) {
    // Generalized from N4885 [time.clock.utc.members]/3 Example 1.
    auto t = leap.date() - 2ns;
    auto u = utc_clock::from_sys(t);
    assert(u.time_since_epoch() - t.time_since_epoch() == offset);

    t += 1ns;
    u = utc_clock::from_sys(t);
    assert(u.time_since_epoch() - t.time_since_epoch() == offset);

    t += 1ns;
    u = utc_clock::from_sys(t);
    offset += leap.value();
    assert(u.time_since_epoch() - t.time_since_epoch() == offset);

    t += 1ns;
    u = utc_clock::from_sys(t);
    assert(u.time_since_epoch() - t.time_since_epoch() == offset);
}

void test_file_clock_utc() {
    const auto file_epoch{utc_clock::from_sys(sys_days{January / 1 / 1601})};
    assert(file_clock::to_utc(file_time<seconds>{0s}) == file_epoch);
    assert(file_clock::from_utc(file_epoch) == file_time<seconds>{0s});
}

void test_file_clock_to_utc(const leap_second& leap, seconds offset) {
    // FILETIME counts leap seconds after 1 January 2017, so offset is constant thereafter.
    constexpr sys_days file_time_cutoff{1d / January / 2017y};
    auto t = clock_cast<file_clock>(leap.date()) - 2us;
    if (leap > file_time_cutoff) {
        offset = 27s;
    }

    offset -= duration_cast<seconds>(file_clock::duration{filesystem::__std_fs_file_time_epoch_adjustment});

    auto u = file_clock::to_utc(t);
    assert(u.time_since_epoch() - t.time_since_epoch() == offset);

    t += 1us;
    u = file_clock::to_utc(t);
    assert(u.time_since_epoch() - t.time_since_epoch() == offset);

    t += 1us;
    u = file_clock::to_utc(t);
    if (leap.date() <= file_time_cutoff) {
        offset += leap.value();
    }

    assert(u.time_since_epoch() - t.time_since_epoch() == offset);

    t += 1us;
    u = file_clock::to_utc(t);
    assert(u.time_since_epoch() - t.time_since_epoch() == offset);
}

void test_gps_tai_clocks_utc() {
    const auto tai_epoch{utc_clock::from_sys(sys_days{January / 1 / 1958} - seconds{10})};
    const auto gps_epoch{utc_clock::from_sys(sys_days{January / Sunday[1] / 1980})};

    assert(tai_clock::to_utc(tai_seconds{0s}) == tai_epoch);
    assert(tai_clock::from_utc(tai_epoch) == tai_seconds{0s});

    assert(gps_clock::to_utc(gps_seconds{0s}) == gps_epoch);
    assert(gps_clock::from_utc(gps_epoch) == gps_seconds{0s});
}

void test_clock_now() {
    auto sys_now  = system_clock::now();
    auto utc_now  = utc_clock::now();
    auto tai_now  = tai_clock::now();
    auto gps_now  = gps_clock::now();
    auto file_now = file_clock::now();

    static_assert(is_same_v<time_point<system_clock>, decltype(sys_now)>);
    static_assert(is_same_v<time_point<utc_clock>, decltype(utc_now)>);
    static_assert(is_same_v<time_point<tai_clock>, decltype(tai_now)>);
    static_assert(is_same_v<time_point<gps_clock>, decltype(gps_now)>);
    static_assert(is_same_v<time_point<file_clock>, decltype(file_now)>);
}

void test_clock_cast() {
    sys_days st(2020y / January / 1);
    const auto ut = utc_clock::from_sys(st);
    const auto tt = tai_clock::from_utc(ut);
    const auto gt = gps_clock::from_utc(ut);
    const auto ft = file_clock::from_utc(ut);

    assert(clock_cast<utc_clock>(ut) == ut);
    assert(clock_cast<utc_clock>(st) == ut);
    assert(clock_cast<utc_clock>(tt) == ut);
    assert(clock_cast<utc_clock>(gt) == ut);
    assert(clock_cast<utc_clock>(ft) == ut);

    assert(clock_cast<system_clock>(ut) == st);
    assert(clock_cast<system_clock>(st) == st);
    assert(clock_cast<system_clock>(tt) == st);
    assert(clock_cast<system_clock>(gt) == st);
    assert(clock_cast<system_clock>(ft) == st);

    assert(clock_cast<tai_clock>(ut) == tt);
    assert(clock_cast<tai_clock>(st) == tt);
    assert(clock_cast<tai_clock>(tt) == tt);
    assert(clock_cast<tai_clock>(gt) == tt);
    assert(clock_cast<tai_clock>(ft) == tt);

    assert(clock_cast<gps_clock>(ut) == gt);
    assert(clock_cast<gps_clock>(st) == gt);
    assert(clock_cast<gps_clock>(tt) == gt);
    assert(clock_cast<gps_clock>(gt) == gt);
    assert(clock_cast<gps_clock>(ft) == gt);

    assert(clock_cast<file_clock>(ut) == ft);
    assert(clock_cast<file_clock>(st) == ft);
    assert(clock_cast<file_clock>(tt) == ft);
    assert(clock_cast<file_clock>(gt) == ft);
    assert(clock_cast<file_clock>(ft) == ft);

    // N4885 [time.clock.utc.overview]/1 Example 1
    assert(clock_cast<utc_clock>(sys_seconds{sys_days{1970y / January / 1}}).time_since_epoch() == 0s);
    assert(clock_cast<utc_clock>(sys_seconds{sys_days{2000y / January / 1}}).time_since_epoch() == 946'684'822s);
}

static_assert(is_clock_v<utc_clock>);
static_assert(is_clock_v<tai_clock>);
static_assert(is_clock_v<gps_clock>);
static_assert(is_clock_v<file_clock>);

tzdb copy_tzdb() {
    const auto& my_tzdb = get_tzdb_list().front();
    vector<time_zone> zones;
    vector<time_zone_link> links;
    transform(my_tzdb.zones.begin(), my_tzdb.zones.end(), back_inserter(zones),
        [](const auto& tz) { return time_zone{tz.name()}; });
    transform(my_tzdb.links.begin(), my_tzdb.links.end(), back_inserter(links), [](const auto& link) {
        return time_zone_link{link.name(), link.target()};
    });

    return {my_tzdb.version, move(zones), move(links), my_tzdb.leap_seconds, my_tzdb._All_ls_positive};
}

void test() {
    assert(test_leap_second());
    static_assert(test_leap_second());

    // This is the only date/time of a leap second insertion that can be represented by a duration<float>.
    assert(utc_clock::to_sys(utc_time<duration<float>>{duration<float>{78796800.0f}}).time_since_epoch().count()
           == nextafter(78796800.0f, 0.0f));

    test_clock_now();

    seconds offset{0};
    for (const auto& leap : get_tzdb().leap_seconds) {
        test_leap_second_info<long long>(leap, offset);
        test_leap_second_info<double>(leap, offset);
        test_utc_clock_to_sys<seconds>(leap);
        test_utc_clock_to_sys<milliseconds>(leap);
        test_utc_clock_to_sys<duration<double>>(leap);
        test_file_clock_from_utc<seconds>(leap);
        test_file_clock_from_utc<milliseconds>(leap);
        test_utc_clock_from_sys(leap, offset);
        test_file_clock_to_utc(leap, offset);
        offset += leap.value();
        assert(leap._Elapsed() == offset);
    }
    test_gps_tai_clocks_utc();
    test_file_clock_utc();
    test_clock_cast();

    // a negative leap second when the accumulated offset is positive
    {
        auto my_tzdb   = copy_tzdb();
        auto& leap_vec = my_tzdb.leap_seconds;
        leap_vec.erase(leap_vec.begin() + 27, leap_vec.end());
        leap_vec.emplace_back(sys_days{1d / January / 2020y}, false, leap_vec.back()._Elapsed());
        leap_vec.emplace_back(sys_days{1d / January / 2021y}, true, leap_vec.back()._Elapsed());
        my_tzdb._All_ls_positive = false;
        get_tzdb_list()._Emplace_front(move(my_tzdb));
    }

    offset = 0s;
    for (const auto& leap : get_tzdb().leap_seconds) {
        test_leap_second_info<long long>(leap, offset);
        test_leap_second_info<double>(leap, offset);
        test_utc_clock_to_sys<seconds>(leap);
        test_utc_clock_to_sys<milliseconds>(leap);
        test_utc_clock_to_sys<duration<double>>(leap);
        test_file_clock_from_utc<seconds>(leap);
        test_file_clock_from_utc<milliseconds>(leap);
        test_utc_clock_from_sys(leap, offset);
        test_file_clock_to_utc(leap, offset);
        offset += leap.value();
        assert(leap._Elapsed() == offset);
    }

    // positive and negative leap seconds when the accumulated offset is negative
    {
        auto my_tzdb   = copy_tzdb();
        auto& leap_vec = my_tzdb.leap_seconds;
        leap_vec.erase(leap_vec.begin() + 27, leap_vec.end());
        for (int i = 0; i < 30; ++i) {
            leap_vec.emplace_back(sys_days{1d / January / year{i + 2020}}, false, leap_vec.back()._Elapsed());
        }
        leap_vec.emplace_back(sys_days{1d / January / 2060y}, true, leap_vec.back()._Elapsed());
        get_tzdb_list()._Emplace_front(move(my_tzdb));
    }

    offset = 0s;
    for (const auto& leap : get_tzdb().leap_seconds) {
        test_leap_second_info<long long>(leap, offset);
        test_leap_second_info<double>(leap, offset);
        test_utc_clock_to_sys<seconds>(leap);
        test_utc_clock_to_sys<milliseconds>(leap);
        test_utc_clock_to_sys<duration<double>>(leap);
        test_file_clock_from_utc<seconds>(leap);
        test_file_clock_from_utc<milliseconds>(leap);
        test_utc_clock_from_sys(leap, offset);
        test_file_clock_to_utc(leap, offset);
        offset += leap.value();
        assert(leap._Elapsed() == offset);
    }
}

int main() {
    run_tz_test([] { test(); });
}
