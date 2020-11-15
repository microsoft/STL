// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <cassert>
#include <chrono>
#include <compare>
#include <iterator>
#include <utility>

using namespace std;
using namespace std::chrono;
using namespace std::chrono_literals;

void test_is_leap_second(const year_month_day& ymd) {
    const auto ls            = sys_days{ymd};
    const auto& leap_seconds = get_tzdb().leap_seconds;
    assert(find(leap_seconds.begin(), leap_seconds.end(), ls + days{1}) != leap_seconds.end());
    assert(get_leap_second_info(utc_clock::from_sys<seconds>(ls) + days{1}).is_leap_second);
}

void test_leap_second() {
    constexpr int jun_leap_second_years[] = {1972, 1981, 1982, 1983, 1985, 1992, 1993, 1994, 1997, 2012, 2015};
    constexpr int dec_leap_second_years[] = {
        1972, 1973, 1974, 1975, 1976, 1977, 1978, 1979, 1987, 1989, 1990, 1995, 1998, 2005, 2008, 2016};
    static_assert(size(jun_leap_second_years) + size(dec_leap_second_years) == 27);

    for (const auto& ls_year : jun_leap_second_years) {
        test_is_leap_second(30d / June / year{ls_year});
    }
    for (const auto& ls_year : dec_leap_second_years) {
        test_is_leap_second(31d / December / year{ls_year});
    }

    constexpr leap_second leap{sys_seconds{42s}, true};
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

    static_assert(is_eq(leap <=> equal));
    static_assert(is_lt(leap <=> larger));
    static_assert(is_gt(leap <=> smaller));
    static_assert(is_lteq(leap <=> larger));
    static_assert(is_gteq(leap <=> smaller));
    static_assert(is_lteq(leap <=> equal));
    static_assert(is_gteq(leap <=> equal));
    static_assert(noexcept(leap <=> equal));

    static_assert(noexcept(leap.date()));
    static_assert(noexcept(leap.value()));
    static_assert(leap_second{sys_seconds{42s}, true}.date() == sys_seconds{42s});
    static_assert(leap_second{sys_seconds{42s}, true}.value() == 1s);
    static_assert(leap_second{sys_seconds{42s}, false}.value() == -1s);
}

constexpr bool operator==(const leap_second_info& lhs, const leap_second_info& rhs) {
    return lhs.is_leap_second == rhs.is_leap_second && lhs.elapsed == rhs.elapsed;
}

void test_leap_second_info(const leap_second& leap, seconds accum) {
    const bool is_positive = (leap.value() == 1s);
    // First UTC time when leap is counted, before insertion of a positive leap, after insertion of a negative one.
    const utc_seconds utc_leap{leap.date().time_since_epoch() + accum + (is_positive ? 0s : -1s)};

    auto lsi  = get_leap_second_info(utc_leap - 1s);
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
    auto u = utc_clock::from_sys(leap.date() - Duration{1}); // just before leap second
    assert(utc_clock::from_sys(utc_clock::to_sys(u)) == u);
    if (leap.value() == 1s) {
        u += Duration{1};
        assert(utc_clock::to_sys(u) == leap.date() - Duration{1}); // during
    } else {
        assert(utc_clock::from_sys(utc_clock::to_sys(u)) == u);
    }

    u += 1s;
    assert(utc_clock::from_sys(utc_clock::to_sys(u)) == u); // just after
}

template <class Duration>
void test_file_clock_from_utc(const leap_second& leap) {
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

    // [time.clock.utc.overview]/1 Example 1
    assert(clock_cast<utc_clock>(sys_seconds{sys_days{1970y / January / 1}}).time_since_epoch() == 0s);
    assert(clock_cast<utc_clock>(sys_seconds{sys_days{2000y / January / 1}}).time_since_epoch() == 946'684'822s);
}

static_assert(is_clock_v<utc_clock>);
static_assert(is_clock_v<tai_clock>);
static_assert(is_clock_v<gps_clock>);
static_assert(is_clock_v<file_clock>);

void test_utc_clock_from_sys(const leap_second& leap, seconds offset) {
    // Generalized from [time.clock.utc.members]/3 Example 1.
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
    offset -= duration_cast<seconds>(file_clock::duration{__std_fs_file_time_epoch_adjustment});

    auto u = file_clock::to_utc(t);
    assert(u.time_since_epoch() - t.time_since_epoch() == offset);

    t += 1us;
    u = file_clock::to_utc(t);
    assert(u.time_since_epoch() - t.time_since_epoch() == offset);

    t += 1us;
    u = file_clock::to_utc(t);
    if (leap.date() <= file_time_cutoff)
        offset += leap.value();
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

int main() {
    test_leap_second();

    seconds offset{0};
    for (const auto& leap : get_tzdb().leap_seconds) {
        test_leap_second_info(leap, offset);
        test_utc_clock_to_sys<seconds>(leap);
        test_utc_clock_to_sys<milliseconds>(leap);
        test_file_clock_from_utc<seconds>(leap);
        test_file_clock_from_utc<milliseconds>(leap);
        test_utc_clock_from_sys(leap, offset);
        test_file_clock_to_utc(leap, offset);
        offset += leap.value();
    }
    test_gps_tai_clocks_utc();
    test_file_clock_utc();
    test_clock_cast();

    // a negative leap second when the accumulated offset is positive
    {
        auto my_tzdb   = get_tzdb_list().front();
        auto& leap_vec = my_tzdb.leap_seconds;
        leap_vec.erase(leap_vec.begin() + 27, leap_vec.end());
        leap_vec.emplace_back(sys_days{1d / January / 2020y}, false);
        leap_vec.emplace_back(sys_days{1d / January / 2021y}, true);
        my_tzdb._All_ls_positive = false;
        get_tzdb_list()._Emplace_front(move(my_tzdb));
    }

    offset = 0s;
    for (const auto& leap : get_tzdb().leap_seconds) {
        test_leap_second_info(leap, offset);
        test_utc_clock_to_sys<seconds>(leap);
        test_utc_clock_to_sys<milliseconds>(leap);
        test_file_clock_from_utc<seconds>(leap);
        test_file_clock_from_utc<milliseconds>(leap);
        test_utc_clock_from_sys(leap, offset);
        test_file_clock_to_utc(leap, offset);
        offset += leap.value();
    }

    // positive and negative leap seconds when the accumulated offset is negative
    {
        auto my_tzdb   = get_tzdb_list().front();
        auto& leap_vec = my_tzdb.leap_seconds;
        leap_vec.erase(leap_vec.begin() + 27, leap_vec.end());
        for (int i = 0; i < 30; ++i) {
            leap_vec.emplace_back(sys_days{1d / January / year{i + 2020}}, false);
        }
        leap_vec.emplace_back(sys_days{1d / January / 2060y}, true);
        get_tzdb_list()._Emplace_front(move(my_tzdb));
    }

    offset = 0s;
    for (const auto& leap : get_tzdb().leap_seconds) {
        test_leap_second_info(leap, offset);
        test_utc_clock_to_sys<seconds>(leap);
        test_utc_clock_to_sys<milliseconds>(leap);
        test_file_clock_from_utc<seconds>(leap);
        test_file_clock_from_utc<milliseconds>(leap);
        test_utc_clock_from_sys(leap, offset);
        test_file_clock_to_utc(leap, offset);
        offset += leap.value();
    }

    return 0;
}
