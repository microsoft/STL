// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <cassert>
#include <chrono>
#include <type_traits>

using namespace std;
using namespace std::chrono;

constexpr int y_min = -32767;
constexpr int y_max = 32767;

// For testing LWG-3260 "year_month* arithmetic rejects durations convertible to years"
using Decades = duration<int, ratio_multiply<ratio<10>, years::period>>;

constexpr void day_test() {
    day d{0u};

    static_assert(noexcept(day{}));
    static_assert(noexcept(day{0u}));

    static_assert(noexcept(++d));
    static_assert(noexcept(d++));
    static_assert(noexcept(--d));
    static_assert(noexcept(d--));
    static_assert(noexcept(d += days{}));
    static_assert(noexcept(d -= days{}));

    static_assert(noexcept(static_cast<unsigned int>(d)));
    static_assert(noexcept(d.ok()));

    static_assert(noexcept(d == d));
    static_assert(noexcept(d <=> d));

    static_assert(noexcept(d + days{}));
    static_assert(noexcept(days{} + d));

    static_assert(noexcept(d - days{}));
    static_assert(noexcept(d - d));

    static_assert(noexcept(0d));

    assert(static_cast<unsigned int>(d) == 0u);
    assert(d == 0d);
    assert(++d == 1d);
    assert(d++ == 1d);
    assert(d == 2d);

    assert(--d == 1d);
    assert(d-- == 1d);
    assert(d == 0d);

    d += days{2};
    assert(d == 2d);
    d -= days{2};
    assert(d == 0d);

    assert(d < 2d);
    assert(2d > d);

    for (unsigned int i = 0; i <= 255; ++i) {
        if (i > 0 && i <= 31) {
            assert(day{i}.ok());
        } else {
            assert(!day{i}.ok());
        }
    }

    assert(5d + days{5} == 10d);
    assert(days{5} + 5d == 10d);
    assert(10d - days{5} == 5d);
    assert(10d - 2d == days{8});
}

constexpr void month_test() {
    month m{1u};

    static_assert(noexcept(month{}));
    static_assert(noexcept(month{0u}));

    static_assert(noexcept(++m));
    static_assert(noexcept(m++));
    static_assert(noexcept(--m));
    static_assert(noexcept(m--));
    static_assert(noexcept(m += months{}));
    static_assert(noexcept(m -= months{}));

    static_assert(noexcept(static_cast<unsigned int>(m)));
    static_assert(noexcept(m.ok()));

    static_assert(noexcept(m == m));
    static_assert(noexcept(m <=> m));

    static_assert(noexcept(m + months{}));
    static_assert(noexcept(months{0} + m));

    static_assert(noexcept(m - months{}));
    static_assert(noexcept(m - m));

    assert(static_cast<unsigned int>(m) == 1u);
    assert(m == January);
    assert(++m == February);
    assert(m++ == February);
    assert(m == March);

    assert(--m == February);
    assert(m-- == February);
    assert(m == January);

    m += months{2};
    assert(m == March);
    m -= months{2};
    assert(m == January);

    for (unsigned int i = 0; i <= 255; ++i) {
        if (i >= 1 && i <= 12) {
            assert(month{i}.ok());
        } else {
            assert(!month{i}.ok());
        }
    }

    assert(February > m);
    assert(m < February);

    assert(February + months{11} == January);
    assert(months{11} + February == January);
    assert(month{0} + months{1} == January);
    assert(month{13} + months{1} == February);
    assert(month{23} + months{1} == December);
    assert(February - months{2} == December);
    assert(January - February == months{11});
}

constexpr void year_test() {
    year y{1};

    static_assert(noexcept(year{}));
    static_assert(noexcept(year{0}));

    static_assert(noexcept(++y));
    static_assert(noexcept(y++));
    static_assert(noexcept(--y));
    static_assert(noexcept(y--));
    static_assert(noexcept(y += years{}));
    static_assert(noexcept(y -= years{}));
    static_assert(noexcept(+y));
    static_assert(noexcept(-y));

    static_assert(noexcept(y.is_leap()));
    static_assert(noexcept(static_cast<int>(y)));
    static_assert(noexcept(y.ok()));
    static_assert(noexcept(year::min()));
    static_assert(noexcept(year::max()));

    static_assert(noexcept(y == y));
    static_assert(noexcept(y <=> y));

    static_assert(noexcept(y + years{}));
    static_assert(noexcept(years{} + y));

    static_assert(noexcept(y - years{}));
    static_assert(noexcept(y - y));

    static_assert(noexcept(0y));

    assert(static_cast<int>(y) == 1);
    assert(y == 1y);
    assert(++y == 2y);
    assert(y++ == 2y);
    assert(y == 3y);

    assert(--y == 2y);
    assert(y-- == 2y);
    assert(y == 1y);

    y += years{2};
    assert(y == 3y);
    y -= years{2};
    assert(y == 1y);

    year extreme{-30'000};
    extreme += years{60'000};
    assert(extreme == 30'000y);
    extreme -= years{60'000};
    assert(extreme == -30'000y);

    assert(+y == 1y);
    assert(-y == year{-1});
    auto y2 = -y;
    assert(-y2 == y);
    assert(+y2 == -y);

    assert(year::min() == year{y_min});
    assert(year::max() == year{y_max});

    assert(!year{y_min - 1}.ok());
    assert(!year{y_max + 1}.ok());

    for (int i = y_min; i <= y_max; ++i) {
        assert(year{i}.ok());
        if (i % 4 == 0 && (i % 100 != 0 || i % 400 == 0)) {
            assert(year{i}.is_leap());
        } else {
            assert(!year{i}.is_leap());
        }
    }

    assert(y < 2y);
    assert(2y > y);

    assert(y + years{4} == 5y);
    assert(years{4} + y == 5y);

    assert(y - years{4} == -3y);
    assert(year{10} - year{5} == years{5});
    assert(year{-5} - year{-10} == years{5});
}

constexpr void weekday_test() {
    weekday wd{0u};

    static_assert(noexcept(weekday{}));
    static_assert(noexcept(weekday{0u}));
    static_assert(noexcept(weekday{sys_days{}}));
    static_assert(noexcept(weekday{local_days{}}));

    static_assert(noexcept(++wd));
    static_assert(noexcept(wd++));
    static_assert(noexcept(--wd));
    static_assert(noexcept(wd--));
    static_assert(noexcept(wd += days{}));
    static_assert(noexcept(wd -= days{}));

    static_assert(noexcept(wd.c_encoding()));
    static_assert(noexcept(wd.iso_encoding()));
    static_assert(noexcept(wd.ok()));
    static_assert(noexcept(wd[0u]));
    static_assert(noexcept(wd[last]));

    static_assert(noexcept(wd == wd));

    static_assert(noexcept(wd + days{}));
    static_assert(noexcept(days{} + wd));

    static_assert(noexcept(wd - days{}));
    static_assert(noexcept(wd - wd));

    assert(weekday{7} == Sunday);
    assert(weekday{sys_days{}} == Thursday);
    assert(weekday{local_days{}} == sys_days{local_days{}.time_since_epoch()});

    assert(wd == Sunday);
    assert(++wd == Monday);
    assert(wd++ == Monday);
    assert(wd == Tuesday);

    assert(--wd == Monday);
    assert(wd-- == Monday);
    assert(wd == Sunday);

    wd += days{2};
    assert(wd == Tuesday);
    wd -= days{3};
    assert(wd == Saturday);

    assert(Sunday.c_encoding() == 0u);
    assert(Sunday.iso_encoding() == 7u);

    for (unsigned int i = 0; i <= 255; ++i) {
        if (i <= 7) {
            assert(weekday{i}.ok());
        } else {
            assert(!weekday{i}.ok());
        }
    }
    assert(Monday + days{6} == Sunday);
    assert(Monday + days{8} == Tuesday);
    assert(Wednesday + days{14} == Wednesday);
    assert(Sunday - Monday == days{6});
    assert(Sunday - Tuesday == days{5});
    assert(Wednesday - Thursday == days{6});

    // GH-5153 "<chrono>: integer overflow in weekday::weekday(sys_days::max())"
    assert(weekday{sys_days::max()} == weekday{sys_days::max() - days{7}});
    assert(weekday{local_days::max()} == weekday{local_days::max() - days{7}});
}

constexpr void weekday_indexed_test() {
    const weekday_indexed wdi1{Monday, 2};

    static_assert(noexcept(weekday_indexed{}));
    static_assert(noexcept(weekday_indexed{weekday{}, 0u}));

    static_assert(noexcept(wdi1.weekday()));
    static_assert(noexcept(wdi1.index()));
    static_assert(noexcept(wdi1.ok()));

    static_assert(noexcept(wdi1 == wdi1));

    assert(wdi1.weekday() == Monday);
    assert(wdi1.index() == 2);

    weekday_indexed wdi2 = Monday[2];
    assert(wdi2.weekday() == Monday);
    assert(wdi2.index() == 2);

    assert(wdi1 == wdi2);

    assert((!weekday_indexed{Sunday, 0}.ok()));
    for (unsigned int i = 1; i <= 5; ++i) {
        assert((weekday_indexed{Sunday, i}.ok()));
        assert((weekday_indexed{Monday, i}.ok()));
        assert((weekday_indexed{Tuesday, i}.ok()));
        assert((weekday_indexed{Wednesday, i}.ok()));
        assert((weekday_indexed{Thursday, i}.ok()));
        assert((weekday_indexed{Friday, i}.ok()));
        assert((weekday_indexed{Saturday, i}.ok()));
    }
    assert((!weekday_indexed{Sunday, 6}.ok()));
    assert((!weekday_indexed{Sunday, 7}.ok()));
}

constexpr void weekday_last_test() {
    const weekday_last wdl{Monday};

    static_assert(noexcept(weekday_last{weekday{}}));

    static_assert(noexcept(wdl.weekday()));
    static_assert(noexcept(wdl.ok()));

    static_assert(noexcept(wdl == wdl));

    assert(wdl.ok());
    assert(Monday[last].ok());
    assert(Monday[last].weekday() == Monday);
    assert(wdl.weekday() == Monday);
    assert(wdl == weekday_last{Monday});
}

constexpr void month_day_test() {
    const month_day md{January, 1d};

    static_assert(noexcept(month_day{}));
    static_assert(noexcept(month_day{month{}, day{}}));

    static_assert(noexcept(md.month()));
    static_assert(noexcept(md.day()));
    static_assert(noexcept(md.ok()));

    static_assert(noexcept(md == md));
    static_assert(noexcept(md <=> md));

    assert(md.month() == January);
    assert(md.day() == 1d);

    assert((md < month_day{January, 2d}));
    assert((month_day{January, 2d} > md));
    assert((md < month_day{December, 25d}));
    assert((month_day{December, 25d} > md));
    assert((md == month_day{January, 1d}));

    if (is_constant_evaluated()) {
        static_assert((January / 31).ok());
        static_assert((February / 29).ok());
        static_assert((April / 30).ok());
        static_assert(!(January / 32).ok());
        static_assert(!(February / 30).ok());
        static_assert(!(April / 31).ok());
    } else {
        for (unsigned int i = 0; i <= 255; ++i) {
            month m{i};
            for (unsigned int d = 0; d <= 255; ++d) {
                if (d < 1 || d > 31 || i < 1 || i > 12) {
                    assert((!month_day{m, day{d}}.ok()));
                } else if (d == 30 && m == February) {
                    assert((!month_day{m, day{d}}.ok()));
                } else if (d == 31 && (m == February || m == April || m == June || m == September || m == November)) {
                    assert((!month_day{m, day{d}}.ok()));
                } else {
                    assert((month_day{m, day{d}}.ok()));
                }
            }
        }
    }
}

constexpr void month_day_last_test() {
    const month_day_last mdl{January};

    static_assert(noexcept(month_day_last{month{}}));
    static_assert(noexcept(mdl.month()));
    static_assert(noexcept(mdl.ok()));

    static_assert(noexcept(mdl == mdl));
    static_assert(noexcept(mdl <=> mdl));

    assert((February / last).month() == February);

    assert(!(month{0} / last).ok());
    for (unsigned int i = 1; i <= 12; ++i) {
        assert((month{i} / last).ok());
    }
    assert(!(month{13} / last).ok());

    assert(January / last == January / last);
    assert(January / last < February / last);
    assert(December / last > February / last);
}

constexpr void month_weekday_test() {
    const auto mwd1 = January / Monday[2];

    static_assert(noexcept(month_weekday{month{}, weekday_indexed{Sunday, 0u}}));

    static_assert(noexcept(mwd1.month()));
    static_assert(noexcept(mwd1.weekday_indexed()));
    static_assert(noexcept(mwd1.ok()));

    static_assert(noexcept(mwd1 == mwd1));

    assert(mwd1.month() == January);
    assert(mwd1.weekday_indexed().weekday() == Monday);
    assert(mwd1.weekday_indexed().index() == 2);

    assert(January / Monday[2] == January / Monday[2]);

    if (is_constant_evaluated()) {
        static_assert((January / Monday[1]).ok());
        static_assert((January / Monday[5]).ok());
        static_assert(!(January / Monday[6]).ok());
        static_assert(!(January / Monday[0]).ok());
        static_assert(!(month{0} / Monday[1]).ok());
    } else {
        for (auto m = 0u; m <= 255u; ++m) {
            for (auto wd = 0u; wd <= 255u; ++wd) {
                for (auto wdi = 0u; wdi <= 6u; ++wdi) {
                    const auto mwd = month{m} / weekday{wd}[wdi];
                    if (m >= 1 && m <= 12 && wd <= 7 && wdi >= 1 && wdi <= 5) {
                        assert(mwd.ok());
                    } else {
                        assert(!mwd.ok());
                    }
                }
            }
        }
    }
}

constexpr void month_weekday_last_test() {
    const auto mwdl = January / Monday[last];

    static_assert(noexcept(month_weekday_last{month{}, weekday_last{Sunday}}));
    static_assert(noexcept(mwdl.month()));
    static_assert(noexcept(mwdl.weekday_last()));
    static_assert(noexcept(mwdl.ok()));

    static_assert(noexcept(mwdl == mwdl));

    assert(mwdl.month() == January);
    assert(mwdl.weekday_last().weekday() == Monday);
    assert(mwdl == January / Monday[last]);
}

constexpr void year_month_test() {
    auto ym = 2020y / January;

    static_assert(noexcept(year_month{}));
    static_assert(noexcept(year_month{year{}, month{}}));

    static_assert(noexcept(ym.year()));
    static_assert(noexcept(ym.month()));

    static_assert(noexcept(ym += months{}));
    static_assert(noexcept(ym -= months{}));
    static_assert(noexcept(ym += years{}));
    static_assert(noexcept(ym -= years{}));

    static_assert(noexcept(ym.ok()));

    static_assert(noexcept(ym == ym));
    static_assert(noexcept(ym <=> ym));

    static_assert(noexcept(ym + months{}));
    static_assert(noexcept(months{} + ym));
    static_assert(noexcept(ym - months{}));

    static_assert(noexcept(ym - ym));
    static_assert(noexcept(ym + years{}));
    static_assert(noexcept(years{} + ym));
    static_assert(noexcept(ym - years{}));

    assert(ym.year() == 2020y);
    assert(ym.month() == January);

    ym += months{2};
    assert(ym.year() == 2020y);
    assert(ym.month() == March);
    ym -= months{2};
    assert(ym.year() == 2020y);
    assert(ym.month() == January);

    ym += years{2};
    assert(ym.year() == 2022y);
    assert(ym.month() == January);
    ym -= years{2};
    assert(ym.year() == 2020y);
    assert(ym.month() == January);

    ym += Decades{2};
    assert(ym.year() == 2040y);
    assert(ym.month() == January);
    ym -= Decades{2};
    assert(ym.year() == 2020y);
    assert(ym.month() == January);

    assert(2020y / April == 2020y / April);
    assert(2019y / April < 2020y / April);
    assert(2020y / March < 2020y / April);
    assert(2020y / April > 2019y / April);
    assert(2020y / April > 2020y / March);

    assert(ym + months{2} == 2020y / March);
    assert(months{2} + ym == 2020y / March);

    assert(ym - months{2} == 2019y / November);
    assert(ym - 2019y / January == months{12});

    assert(ym + years{2} == 2022y / January);
    assert(years{2} + ym == 2022y / January);

    assert(ym - years{2} == 2018y / January);

    assert(ym + Decades{2} == 2040y / January);
    assert(Decades{2} + ym == 2040y / January);

    assert(ym - Decades{2} == 2000y / January);

    if (is_constant_evaluated()) {
        static_assert((2020y / 1).ok());
        static_assert(!(2020y / 13).ok());
        static_assert(!(32768y / 1).ok());
    } else {
        for (int y = y_min - 1; y <= y_max + 1; ++y) {
            for (auto m = 0u; m <= 255u; ++m) {
                const auto ym2 = year{y} / month{m};
                if (y == y_min - 1 || y == y_max + 1) {
                    assert(!ym2.ok());
                } else if (m >= 1 && m <= 12) {
                    assert(ym2.ok());
                } else {
                    assert(!ym2.ok());
                }
            }
        }
    }
}

constexpr void year_month_day_test() {
    year_month_day ymd1{2020y / January / 1d};

    static_assert(noexcept(year_month_day{}));
    static_assert(noexcept(year_month_day{year{}, month{}, day{}}));
    static_assert(noexcept(year_month_day{year_month_day_last{year{}, month_day_last{January}}}));
    static_assert(noexcept(year_month_day{sys_days{}}));
    static_assert(noexcept(year_month_day{local_days{}}));

    static_assert(noexcept(ymd1 += months{}));
    static_assert(noexcept(ymd1 -= months{}));
    static_assert(noexcept(ymd1 += years{}));
    static_assert(noexcept(ymd1 -= years{}));

    static_assert(noexcept(ymd1.year()));
    static_assert(noexcept(ymd1.month()));
    static_assert(noexcept(ymd1.day()));

    static_assert(noexcept(static_cast<sys_days>(ymd1)));
    static_assert(noexcept(static_cast<local_days>(ymd1)));
    static_assert(noexcept(ymd1.ok()));

    static_assert(noexcept(ymd1 == ymd1));
    static_assert(noexcept(ymd1 <=> ymd1));

    static_assert(noexcept(ymd1 + months{}));
    static_assert(noexcept(months{} + ymd1));
    static_assert(noexcept(ymd1 - months{}));

    static_assert(noexcept(ymd1 + years{}));
    static_assert(noexcept(years{} + ymd1));
    static_assert(noexcept(ymd1 - years{}));

    assert(ymd1.year() == 2020y);
    assert(ymd1.month() == January);
    assert(ymd1.day() == 1d);

    year_month_day ymd2{2020y / January / last};
    assert(ymd2.year() == 2020y);
    assert(ymd2.month() == January);
    assert(ymd2.day() == 31d);

    year_month_day epoch{sys_days{}};
    assert(epoch == year_month_day{sys_days{epoch}});
    assert(epoch.year() == 1970y);
    assert(epoch.month() == January);
    assert(epoch.day() == 1d);

    local_days ldp;
    sys_days sys{ldp.time_since_epoch()};
    year_month_day ymld{ldp};
    assert(ymld == year_month_day{sys});

    ymd1 += months{2};
    assert(ymd1.year() == 2020y);
    assert(ymd1.month() == March);
    assert(ymd1.day() == 1d);

    ymd1 -= months{2};
    assert(ymd1.year() == 2020y);
    assert(ymd1.month() == January);
    assert(ymd1.day() == 1d);

    ymd1 += years{2};
    assert(ymd1.year() == 2022y);
    assert(ymd1.month() == January);
    assert(ymd1.day() == 1d);

    ymd1 -= years{2};
    assert(ymd1.year() == 2020y);
    assert(ymd1.month() == January);
    assert(ymd1.day() == 1d);

    ymd1 += Decades{2};
    assert(ymd1.year() == 2040y);
    assert(ymd1.month() == January);
    assert(ymd1.day() == 1d);

    ymd1 -= Decades{2};
    assert(ymd1.year() == 2020y);
    assert(ymd1.month() == January);
    assert(ymd1.day() == 1d);

    assert(2020y / April / 6d == sys_days{days{18'358}});
    assert(sys_days{2017y / January / 0} == 2016y / December / 31);
    assert(sys_days{2017y / January / 31} == 2017y / January / 31);
    assert(sys_days{2017y / January / 32} == 2017y / February / 1);

    assert(static_cast<local_days>(ymld) == local_days{});

    assert(2020y / January / 1d == 2020y / January / 1d);
    assert(2019y / January / 1d < 2020y / January / 1d);
    assert(2020y / January / 1d < 2020y / February / 1d);
    assert(2020y / January / 1d < 2020y / January / 2d);
    assert(2020y / January / 1d > 2019y / January / 1d);
    assert(2020y / February / 1d > 2020y / January / 1d);
    assert(2020y / January / 2d > 2020y / January / 1d);

    const auto ymd3 = 2019y / December / 31d + months{2};
    assert(ymd3 == 2020y / February / 31d);
    assert(!ymd3.ok());
    assert(months{2} + 2019y / December / 31d == ymd3);


    assert(2020y / January / 1d - months{2} == 2019y / November / 1d);

    assert(2020y / January / 1d + years{2} == 2022y / January / 1d);
    assert(years{2} + 2020y / January / 1d == 2022y / January / 1d);

    assert(2020y / January / 1d - years{2} == 2018y / January / 1d);

    assert(2020y / January / 1d + Decades{2} == 2040y / January / 1d);
    assert(Decades{2} + 2020y / January / 1d == 2040y / January / 1d);

    assert(2020y / January / 1d - Decades{2} == 2000y / January / 1d);

    if (is_constant_evaluated()) {
        static_assert(!(-32768y / 1 / 1).ok());
        static_assert(!(32768y / 1 / 1).ok());
        static_assert((2020y / 1 / 1).ok());

        static_assert(!(2020y / 0 / 1).ok());
        static_assert(!(2020y / 13 / 1).ok());
        static_assert((2020y / 5 / 1).ok());

        static_assert(!(2020y / 2 / 30).ok());
        static_assert(!(2019y / 2 / 29).ok());
        static_assert(!(2020y / 1 / 0).ok());
        static_assert(!(2020y / 1 / 32).ok());
        static_assert((2020y / 2 / 29).ok());
        static_assert((2020y / 7 / 31).ok());
    } else {
        for (int iy = -3000; iy <= 3000; ++iy) { // instead of [y_min, y_max], to limit the number of iterations
            for (auto um = 0u; um <= 13u; ++um) { // instead of [0, 255], to limit the number of iterations
                for (auto ud = 0u; ud <= 32u; ++ud) {
                    const year y{iy};
                    const month m{um};
                    const day d{ud};
                    if (y.ok() && m.ok() && d >= 1d && d <= (y / m / last).day()) {
                        assert((y / m / d).ok());
                    } else {
                        assert(!(y / m / d).ok());
                    }
                }
            }
        }
    }

    if (is_constant_evaluated()) {
        static_assert(sys_days{2000y / 1 / 1} == sys_days{days{10957}});
        static_assert(sys_days{2000y / 1 / 31} == sys_days{days{10987}});
        static_assert(sys_days{2000y / 2 / 1} == sys_days{days{10988}});
        static_assert(sys_days{2000y / 2 / 29} == sys_days{days{11016}});
        static_assert(sys_days{2000y / 3 / 1} == sys_days{days{11017}});
        static_assert(sys_days{2000y / 3 / 31} == sys_days{days{11047}});
        static_assert(sys_days{2000y / 4 / 1} == sys_days{days{11048}});
        static_assert(sys_days{2000y / 4 / 30} == sys_days{days{11077}});
        static_assert(sys_days{2000y / 5 / 1} == sys_days{days{11078}});
        static_assert(sys_days{2000y / 5 / 31} == sys_days{days{11108}});
        static_assert(sys_days{2000y / 6 / 1} == sys_days{days{11109}});
        static_assert(sys_days{2000y / 6 / 30} == sys_days{days{11138}});
        static_assert(sys_days{2000y / 7 / 1} == sys_days{days{11139}});
        static_assert(sys_days{2000y / 7 / 31} == sys_days{days{11169}});
        static_assert(sys_days{2000y / 8 / 1} == sys_days{days{11170}});
        static_assert(sys_days{2000y / 8 / 31} == sys_days{days{11200}});
        static_assert(sys_days{2000y / 9 / 1} == sys_days{days{11201}});
        static_assert(sys_days{2000y / 9 / 30} == sys_days{days{11230}});
        static_assert(sys_days{2000y / 10 / 1} == sys_days{days{11231}});
        static_assert(sys_days{2000y / 10 / 31} == sys_days{days{11261}});
        static_assert(sys_days{2000y / 11 / 1} == sys_days{days{11262}});
        static_assert(sys_days{2000y / 11 / 30} == sys_days{days{11291}});
        static_assert(sys_days{2000y / 12 / 1} == sys_days{days{11292}});
        static_assert(sys_days{2000y / 12 / 31} == sys_days{days{11322}});
        static_assert(sys_days{-400y / 2 / 29} == sys_days{days{-865566}});
        static_assert(sys_days{-400y / 3 / 1} == sys_days{days{-865565}});
        static_assert(sys_days{-1y / 2 / 28} == sys_days{days{-719835}});
        static_assert(sys_days{-1y / 3 / 1} == sys_days{days{-719834}});
        static_assert(sys_days{-1y / 12 / 31} == sys_days{days{-719529}});
        static_assert(sys_days{0y / 1 / 1} == sys_days{days{-719528}});
        static_assert(sys_days{0y / 2 / 29} == sys_days{days{-719469}});
        static_assert(sys_days{0y / 3 / 1} == sys_days{days{-719468}});
        static_assert(sys_days{1900y / 3 / 1} == sys_days{days{-25508}});
        static_assert(sys_days{1901y / 2 / 28} == sys_days{days{-25144}});
        static_assert(sys_days{1903y / 3 / 1} == sys_days{days{-24413}});
        static_assert(sys_days{1904y / 2 / 29} == sys_days{days{-24048}});
        static_assert(sys_days{1996y / 3 / 1} == sys_days{days{9556}});
        static_assert(sys_days{1997y / 2 / 28} == sys_days{days{9920}});
        static_assert(sys_days{1999y / 3 / 1} == sys_days{days{10651}});
        static_assert(sys_days{2000y / 2 / 29} == sys_days{days{11016}});
        static_assert(sys_days{2000y / 3 / 1} == sys_days{days{11017}});
        static_assert(sys_days{2001y / 2 / 28} == sys_days{days{11381}});
        static_assert(sys_days{2003y / 3 / 1} == sys_days{days{12112}});
        static_assert(sys_days{2004y / 2 / 29} == sys_days{days{12477}});
        static_assert(sys_days{2096y / 3 / 1} == sys_days{days{46081}});
        static_assert(sys_days{2097y / 2 / 28} == sys_days{days{46445}});
        static_assert(sys_days{2099y / 3 / 1} == sys_days{days{47176}});
        static_assert(sys_days{2100y / 2 / 28} == sys_days{days{47540}});
        static_assert(sys_days{-32767y / 1 / 1} == sys_days{days{-12687428}});
        static_assert(sys_days{32767y / 12 / 31} == sys_days{days{11248737}});

        static_assert(2000y / 1 / 1 == year_month_day{sys_days{days{10957}}});
        static_assert(2000y / 1 / 31 == year_month_day{sys_days{days{10987}}});
        static_assert(2000y / 2 / 1 == year_month_day{sys_days{days{10988}}});
        static_assert(2000y / 2 / 29 == year_month_day{sys_days{days{11016}}});
        static_assert(2000y / 3 / 1 == year_month_day{sys_days{days{11017}}});
        static_assert(2000y / 3 / 31 == year_month_day{sys_days{days{11047}}});
        static_assert(2000y / 4 / 1 == year_month_day{sys_days{days{11048}}});
        static_assert(2000y / 4 / 30 == year_month_day{sys_days{days{11077}}});
        static_assert(2000y / 5 / 1 == year_month_day{sys_days{days{11078}}});
        static_assert(2000y / 5 / 31 == year_month_day{sys_days{days{11108}}});
        static_assert(2000y / 6 / 1 == year_month_day{sys_days{days{11109}}});
        static_assert(2000y / 6 / 30 == year_month_day{sys_days{days{11138}}});
        static_assert(2000y / 7 / 1 == year_month_day{sys_days{days{11139}}});
        static_assert(2000y / 7 / 31 == year_month_day{sys_days{days{11169}}});
        static_assert(2000y / 8 / 1 == year_month_day{sys_days{days{11170}}});
        static_assert(2000y / 8 / 31 == year_month_day{sys_days{days{11200}}});
        static_assert(2000y / 9 / 1 == year_month_day{sys_days{days{11201}}});
        static_assert(2000y / 9 / 30 == year_month_day{sys_days{days{11230}}});
        static_assert(2000y / 10 / 1 == year_month_day{sys_days{days{11231}}});
        static_assert(2000y / 10 / 31 == year_month_day{sys_days{days{11261}}});
        static_assert(2000y / 11 / 1 == year_month_day{sys_days{days{11262}}});
        static_assert(2000y / 11 / 30 == year_month_day{sys_days{days{11291}}});
        static_assert(2000y / 12 / 1 == year_month_day{sys_days{days{11292}}});
        static_assert(2000y / 12 / 31 == year_month_day{sys_days{days{11322}}});
        static_assert(-400y / 2 / 29 == year_month_day{sys_days{days{-865566}}});
        static_assert(-400y / 3 / 1 == year_month_day{sys_days{days{-865565}}});
        static_assert(-1y / 2 / 28 == year_month_day{sys_days{days{-719835}}});
        static_assert(-1y / 3 / 1 == year_month_day{sys_days{days{-719834}}});
        static_assert(-1y / 12 / 31 == year_month_day{sys_days{days{-719529}}});
        static_assert(0y / 1 / 1 == year_month_day{sys_days{days{-719528}}});
        static_assert(0y / 2 / 29 == year_month_day{sys_days{days{-719469}}});
        static_assert(0y / 3 / 1 == year_month_day{sys_days{days{-719468}}});
        static_assert(1900y / 3 / 1 == year_month_day{sys_days{days{-25508}}});
        static_assert(1901y / 2 / 28 == year_month_day{sys_days{days{-25144}}});
        static_assert(1903y / 3 / 1 == year_month_day{sys_days{days{-24413}}});
        static_assert(1904y / 2 / 29 == year_month_day{sys_days{days{-24048}}});
        static_assert(1996y / 3 / 1 == year_month_day{sys_days{days{9556}}});
        static_assert(1997y / 2 / 28 == year_month_day{sys_days{days{9920}}});
        static_assert(1999y / 3 / 1 == year_month_day{sys_days{days{10651}}});
        static_assert(2000y / 2 / 29 == year_month_day{sys_days{days{11016}}});
        static_assert(2000y / 3 / 1 == year_month_day{sys_days{days{11017}}});
        static_assert(2001y / 2 / 28 == year_month_day{sys_days{days{11381}}});
        static_assert(2003y / 3 / 1 == year_month_day{sys_days{days{12112}}});
        static_assert(2004y / 2 / 29 == year_month_day{sys_days{days{12477}}});
        static_assert(2096y / 3 / 1 == year_month_day{sys_days{days{46081}}});
        static_assert(2097y / 2 / 28 == year_month_day{sys_days{days{46445}}});
        static_assert(2099y / 3 / 1 == year_month_day{sys_days{days{47176}}});
        static_assert(2100y / 2 / 28 == year_month_day{sys_days{days{47540}}});
        static_assert(-32767y / 1 / 1 == year_month_day{sys_days{days{-12687428}}});
        static_assert(32767y / 12 / 31 == year_month_day{sys_days{days{11248737}}});

        static_assert(sys_days{2000y / 1 / 0} == sys_days{days{10956}});
        static_assert(sys_days{2000y / 1 / 255} == sys_days{days{11211}});
        static_assert(sys_days{2000y / 2 / 0} == sys_days{days{10987}});
        static_assert(sys_days{2000y / 2 / 255} == sys_days{days{11242}});
        static_assert(sys_days{2000y / 3 / 0} == sys_days{days{11016}});
        static_assert(sys_days{2000y / 3 / 255} == sys_days{days{11271}});
        static_assert(sys_days{2000y / 4 / 0} == sys_days{days{11047}});
        static_assert(sys_days{2000y / 4 / 255} == sys_days{days{11302}});
        static_assert(sys_days{2000y / 5 / 0} == sys_days{days{11077}});
        static_assert(sys_days{2000y / 5 / 255} == sys_days{days{11332}});
        static_assert(sys_days{2000y / 6 / 0} == sys_days{days{11108}});
        static_assert(sys_days{2000y / 6 / 255} == sys_days{days{11363}});
        static_assert(sys_days{2000y / 7 / 0} == sys_days{days{11138}});
        static_assert(sys_days{2000y / 7 / 255} == sys_days{days{11393}});
        static_assert(sys_days{2000y / 8 / 0} == sys_days{days{11169}});
        static_assert(sys_days{2000y / 8 / 255} == sys_days{days{11424}});
        static_assert(sys_days{2000y / 9 / 0} == sys_days{days{11200}});
        static_assert(sys_days{2000y / 9 / 255} == sys_days{days{11455}});
        static_assert(sys_days{2000y / 10 / 0} == sys_days{days{11230}});
        static_assert(sys_days{2000y / 10 / 255} == sys_days{days{11485}});
        static_assert(sys_days{2000y / 11 / 0} == sys_days{days{11261}});
        static_assert(sys_days{2000y / 11 / 255} == sys_days{days{11516}});
        static_assert(sys_days{2000y / 12 / 0} == sys_days{days{11291}});
        static_assert(sys_days{2000y / 12 / 255} == sys_days{days{11546}});
        static_assert(sys_days{-400y / 2 / 255} == sys_days{days{-865340}});
        static_assert(sys_days{-400y / 3 / 0} == sys_days{days{-865566}});
        static_assert(sys_days{-1y / 2 / 255} == sys_days{days{-719608}});
        static_assert(sys_days{-1y / 3 / 0} == sys_days{days{-719835}});
        static_assert(sys_days{-1y / 12 / 255} == sys_days{days{-719305}});
        static_assert(sys_days{0y / 1 / 0} == sys_days{days{-719529}});
        static_assert(sys_days{0y / 2 / 255} == sys_days{days{-719243}});
        static_assert(sys_days{0y / 3 / 0} == sys_days{days{-719469}});
        static_assert(sys_days{1900y / 3 / 0} == sys_days{days{-25509}});
        static_assert(sys_days{1901y / 2 / 255} == sys_days{days{-24917}});
        static_assert(sys_days{1903y / 3 / 0} == sys_days{days{-24414}});
        static_assert(sys_days{1904y / 2 / 255} == sys_days{days{-23822}});
        static_assert(sys_days{1996y / 3 / 0} == sys_days{days{9555}});
        static_assert(sys_days{1997y / 2 / 255} == sys_days{days{10147}});
        static_assert(sys_days{1999y / 3 / 0} == sys_days{days{10650}});
        static_assert(sys_days{2000y / 2 / 255} == sys_days{days{11242}});
        static_assert(sys_days{2000y / 3 / 0} == sys_days{days{11016}});
        static_assert(sys_days{2001y / 2 / 255} == sys_days{days{11608}});
        static_assert(sys_days{2003y / 3 / 0} == sys_days{days{12111}});
        static_assert(sys_days{2004y / 2 / 255} == sys_days{days{12703}});
        static_assert(sys_days{2096y / 3 / 0} == sys_days{days{46080}});
        static_assert(sys_days{2097y / 2 / 255} == sys_days{days{46672}});
        static_assert(sys_days{2099y / 3 / 0} == sys_days{days{47175}});
        static_assert(sys_days{2100y / 2 / 255} == sys_days{days{47767}});
        static_assert(sys_days{-32767y / 1 / 0} == sys_days{days{-12687429}});
        static_assert(sys_days{32767y / 12 / 255} == sys_days{days{11248961}});
    } else {
        sys_days sys2{year{y_min} / 1 / 1};

        for (int iy = y_min; iy <= y_max; ++iy) {
            const year y{iy};

            for (auto um = 1u; um <= 12u; ++um) {
                const month m{um};

                const year_month_day ymd_first = y / m / 1;
                assert(sys_days{ymd_first} == sys2);
                assert(year_month_day{sys2} == ymd_first);

                const year_month_day ymd_min = y / m / 0;
                assert(sys_days{ymd_min} == sys2 - days{1});

                const year_month_day ymd_max = y / m / 255;
                assert(sys_days{ymd_max} == sys2 + days{254});

                const year_month_day ymd_last = y / m / last;
                sys2 += ymd_last.day() - 1d;
                assert(sys_days{ymd_last} == sys2);
                assert(year_month_day{sys2} == ymd_last);

                sys2 += days{1};
            }
        }
    }
}

constexpr void year_month_day_last_test() {
    auto ymdl = 2020y / February / last;

    static_assert(noexcept(year_month_day_last{year{}, month_day_last{January}}));

    static_assert(noexcept(ymdl += months{}));
    static_assert(noexcept(ymdl -= months{}));
    static_assert(noexcept(ymdl += years{}));
    static_assert(noexcept(ymdl -= years{}));

    static_assert(noexcept(ymdl.year()));
    static_assert(noexcept(ymdl.month()));
    static_assert(noexcept(ymdl.month_day_last()));
    static_assert(noexcept(ymdl.day()));

    static_assert(noexcept(static_cast<sys_days>(ymdl)));
    static_assert(noexcept(static_cast<local_days>(ymdl)));
    static_assert(noexcept(ymdl.ok()));

    static_assert(noexcept(ymdl == ymdl));
    static_assert(noexcept(ymdl <=> ymdl));

    static_assert(noexcept(ymdl + months{}));
    static_assert(noexcept(months{} + ymdl));
    static_assert(noexcept(ymdl - months{}));

    static_assert(noexcept(ymdl + years{}));
    static_assert(noexcept(years{} + ymdl));
    static_assert(noexcept(ymdl - years{}));

    assert(ymdl == 2020y / February / last);
    assert(ymdl == 2020y / February / 29d);
    assert(ymdl.year() == 2020y);
    assert(ymdl.month() == February);
    assert(ymdl.month_day_last() == February / last);
    assert(ymdl.day() == 29d);

    ymdl += months{2};
    assert(ymdl == 2020y / April / 30d);

    ymdl -= months{2};
    assert(ymdl == 2020y / February / 29d);

    ymdl += years{2};
    assert(ymdl == 2022y / February / 28d);

    ymdl -= years{2};
    assert(ymdl == 2020y / February / 29d);

    ymdl += Decades{2};
    assert(ymdl == 2040y / February / 29d);

    ymdl -= Decades{2};
    assert(ymdl == 2020y / February / 29d);

    assert(2020y / April / last == sys_days{days{18'382}});
    assert(static_cast<local_days>(ymdl) == local_days{ymdl});

    assert(ymdl < 2021y / February / last);
    assert(ymdl < 2020y / March / last);
    assert(2021y / February / last > ymdl);
    assert(2020y / March / last > ymdl);

    assert(ymdl + months{2} == 2020y / April / last);
    assert(months{2} + ymdl == 2020y / April / last);

    assert(ymdl - months{2} == 2019y / December / last);

    assert(ymdl + years{2} == 2022y / February / last);
    assert(years{2} + ymdl == 2022y / February / last);

    assert(ymdl - years{2} == 2018y / February / last);

    assert(ymdl + Decades{2} == 2040y / February / last);
    assert(Decades{2} + ymdl == 2040y / February / last);

    assert(ymdl - Decades{2} == 2000y / February / last);

    if (is_constant_evaluated()) {
        static_assert((2020y / 1 / last).ok());
        static_assert(!(2020y / 13 / last).ok());
        static_assert(!(2020y / 0 / last).day().ok()); // implementation-specific assumption, see GH-1647
    } else {
        for (int iy = y_min; iy <= y_max; ++iy) {
            for (auto m = 0u; m <= 255u; ++m) {
                const year y{iy};
                const auto mdl = month{m} / last;
                if (y.ok() && mdl.ok()) {
                    assert((y / mdl).ok());
                } else {
                    assert(!(y / mdl).ok());
                    assert((y / mdl).day().ok() || (y / mdl).day() == day{255}); // implementation-specific assumption
                }
            }
        }
    }
}

constexpr void year_month_weekday_test() {
    auto ymwd = 2020y / April / Tuesday[2];

    static_assert(noexcept(year_month_weekday{}));
    static_assert(noexcept(year_month_weekday{year{}, month{}, weekday_indexed{}}));
    static_assert(noexcept(year_month_weekday{sys_days{}}));
    static_assert(noexcept(year_month_weekday{local_days{}}));

    static_assert(noexcept(ymwd += months{}));
    static_assert(noexcept(ymwd -= months{}));
    static_assert(noexcept(ymwd += years{}));
    static_assert(noexcept(ymwd -= years{}));

    static_assert(noexcept(ymwd.year()));
    static_assert(noexcept(ymwd.month()));
    static_assert(noexcept(ymwd.weekday()));
    static_assert(noexcept(ymwd.index()));
    static_assert(noexcept(ymwd.weekday_indexed()));

    static_assert(noexcept(static_cast<sys_days>(ymwd)));
    static_assert(noexcept(static_cast<local_days>(ymwd)));
    static_assert(noexcept(ymwd.ok()));

    static_assert(noexcept(ymwd == ymwd));

    static_assert(noexcept(ymwd + months{}));
    static_assert(noexcept(months{} + ymwd));
    static_assert(noexcept(ymwd - months{}));

    static_assert(noexcept(ymwd + years{}));
    static_assert(noexcept(years{} + ymwd));
    static_assert(noexcept(ymwd - years{}));

    assert(ymwd == 2020y / April / Tuesday[2]);
    assert(ymwd.year() == 2020y);
    assert(ymwd.month() == April);
    assert(ymwd.weekday() == Tuesday);
    assert(ymwd.index() == 2u);
    assert(ymwd.weekday_indexed() == Tuesday[2]);

    const year_month_weekday epoch{sys_days{}};
    assert(epoch == year_month_weekday{sys_days{epoch}});
    assert(epoch == 1970y / January / Thursday[1]);

    local_days ldp;
    sys_days sys{ldp.time_since_epoch()};
    year_month_weekday ymlwd{ldp};
    assert(ymlwd == year_month_weekday{sys});

    ymwd += months{2};
    assert(ymwd == 2020y / June / Tuesday[2]);
    ymwd -= months{2};
    assert(ymwd == 2020y / April / Tuesday[2]);

    ymwd += years{2};
    assert(ymwd == 2022y / April / Tuesday[2]);
    ymwd -= years{2};
    assert(ymwd == 2020y / April / Tuesday[2]);

    ymwd += Decades{2};
    assert(ymwd == 2040y / April / Tuesday[2]);
    ymwd -= Decades{2};
    assert(ymwd == 2020y / April / Tuesday[2]);

    assert(static_cast<sys_days>(epoch) == sys_days{});
    const auto previous = 1970y / January / Thursday[0];
    assert(static_cast<sys_days>(previous) == (sys_days{} - days{7}));
    assert(static_cast<local_days>(ymwd) == local_days{ymwd});


    assert((2020y / April / Wednesday[5]).ok());
    assert(!(-32768y / April / Wednesday[1]).ok());
    assert(!(2020y / month{0} / Wednesday[1]).ok());
    assert(!(2020y / April / Tuesday[5]).ok());

    assert(ymwd + months{2} == 2020y / June / Tuesday[2]);
    assert(months{2} + ymwd == 2020y / June / Tuesday[2]);

    assert(ymwd - months{2} == 2020y / February / Tuesday[2]);

    assert(ymwd + years{2} == 2022y / April / Tuesday[2]);
    assert(years{2} + ymwd == 2022y / April / Tuesday[2]);

    assert(ymwd - years{2} == 2018y / April / Tuesday[2]);

    assert(ymwd + Decades{2} == 2040y / April / Tuesday[2]);
    assert(Decades{2} + ymwd == 2040y / April / Tuesday[2]);

    assert(ymwd - Decades{2} == 2000y / April / Tuesday[2]);
}

constexpr void year_month_weekday_last_test() {
    auto ymwdl = 2020y / January / Monday[last];

    static_assert(noexcept(year_month_weekday_last{year{}, month{}, weekday_last{Sunday}}));

    static_assert(noexcept(ymwdl += months{}));
    static_assert(noexcept(ymwdl -= months{}));
    static_assert(noexcept(ymwdl += years{}));
    static_assert(noexcept(ymwdl -= years{}));

    static_assert(noexcept(ymwdl.year()));
    static_assert(noexcept(ymwdl.month()));
    static_assert(noexcept(ymwdl.weekday()));
    static_assert(noexcept(ymwdl.weekday_last()));

    static_assert(noexcept(static_cast<sys_days>(ymwdl)));
    static_assert(noexcept(static_cast<local_days>(ymwdl)));
    static_assert(noexcept(ymwdl.ok()));

    static_assert(noexcept(ymwdl == ymwdl));

    static_assert(noexcept(ymwdl + months{}));
    static_assert(noexcept(months{} + ymwdl));
    static_assert(noexcept(ymwdl - months{}));

    static_assert(noexcept(ymwdl + years{}));
    static_assert(noexcept(years{} + ymwdl));
    static_assert(noexcept(ymwdl - years{}));

    assert(ymwdl == 2020y / January / Monday[last]);
    assert(ymwdl.year() == 2020y);
    assert(ymwdl.month() == January);
    assert(ymwdl.weekday() == Monday);
    assert(ymwdl.weekday_last() == Monday[last]);

    ymwdl += months{2};
    assert(ymwdl == 2020y / March / Monday[last]);
    ymwdl -= months{2};
    assert(ymwdl == 2020y / January / Monday[last]);

    ymwdl += years{2};
    assert(ymwdl == 2022y / January / Monday[last]);
    ymwdl -= years{2};
    assert(ymwdl == 2020y / January / Monday[last]);

    ymwdl += Decades{2};
    assert(ymwdl == 2040y / January / Monday[last]);
    ymwdl -= Decades{2};
    assert(ymwdl == 2020y / January / Monday[last]);

    assert(static_cast<sys_days>(ymwdl) == sys_days{days{18'288}});
    assert(static_cast<local_days>(ymwdl) == local_days{ymwdl});

    assert((2020y / April / Wednesday[last]).ok());
    assert(!(-32768y / April / Wednesday[last]).ok());
    assert(!(2020y / month{0} / Wednesday[last]).ok());
    assert(!(2020y / April / weekday{8}[last]).ok());

    assert(ymwdl + months{2} == 2020y / March / Monday[last]);
    assert(months{2} + ymwdl == 2020y / March / Monday[last]);

    assert(ymwdl - months{2} == 2019y / November / Monday[last]);

    assert(ymwdl + years{2} == 2022y / January / Monday[last]);
    assert(years{2} + ymwdl == 2022y / January / Monday[last]);

    assert(ymwdl - years{2} == 2018y / January / Monday[last]);

    assert(ymwdl + Decades{2} == 2040y / January / Monday[last]);
    assert(Decades{2} + ymwdl == 2040y / January / Monday[last]);

    assert(ymwdl - Decades{2} == 2000y / January / Monday[last]);
}

constexpr bool test() {
    day_test();
    month_test();
    year_test();
    weekday_test();
    weekday_indexed_test();
    weekday_last_test();
    month_day_test();
    month_day_last_test();
    month_weekday_test();
    month_weekday_last_test();
    year_month_test();
    year_month_day_test();
    year_month_day_last_test();
    year_month_weekday_test();
    year_month_weekday_last_test();
    return true;
}

int main() {
    test();
    static_assert(test());
}
