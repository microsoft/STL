// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <cassert>
#include <chrono>
#include <type_traits>

using namespace std;
using namespace std::chrono;

constexpr void day_test() {
    day d{0u};
    assert(static_cast<unsigned>(d) == 0u);
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

    day d2{0u};
    assert(!d2.ok());
    ++d2;
    for (int i = 1; i <= 31; ++i, ++d2) {
        assert(d2.ok());
    }
    assert(!d2.ok());

    assert(10d - 2d == days{8});
}

constexpr void month_test() {

    month m{1u};
    assert(static_cast<unsigned>(m) == 1u);
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

    for (unsigned i = 0; i <= 255; ++i) {
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
    assert(February - months{2} == December);

    assert(January - February == months{11});
}

constexpr void year_test() {
    year y{1};
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

    assert(+y == 1y);
    assert(-y == -1y);

    constexpr int y_min = -32767;
    constexpr int y_max = 32767;
    assert(year::min() == year{y_min});
    assert(year::max() == year{y_max});

    assert(!year{y_min - 1}.ok());
    assert(!year{y_max + 1}.ok());

    for (int i = y_min; i <= y_max; i++) {
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
}

constexpr void weekday_test() {
    assert(weekday{7} == Sunday);
    assert(weekday{sys_days{}} == Thursday);
    assert(weekday{local_days{}} == sys_days{local_days{}.time_since_epoch()});

    weekday wd{0u};
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

    for (unsigned i = 0; i <= 255; ++i) {
        if (i <= 7) {
            assert(weekday{i}.ok());
        } else {
            assert(!weekday{i}.ok());
        }
    }
    assert(Monday + days{6} == Sunday);
    assert(Sunday - Monday == days{6});
}

constexpr void weekday_indexed_test() {
    weekday_indexed wdi1{Monday, 2};
    assert(wdi1.weekday() == Monday);
    assert(wdi1.index() == 2);

    weekday_indexed wdi2 = Monday[2];
    assert(wdi2.weekday() == Monday);
    assert(wdi2.index() == 2);

    assert(wdi1 == wdi2);

    assert(!weekday_indexed(Sunday, 0).ok());
    for (unsigned i = 1; i <= 5; i++) {
        assert(weekday_indexed(Sunday, i).ok());
        assert(weekday_indexed(Monday, i).ok());
        assert(weekday_indexed(Tuesday, i).ok());
        assert(weekday_indexed(Wednesday, i).ok());
        assert(weekday_indexed(Thursday, i).ok());
        assert(weekday_indexed(Friday, i).ok());
        assert(weekday_indexed(Saturday, i).ok());
    }
    assert(!weekday_indexed(Sunday, 6).ok());
}

constexpr void weekday_last_test() {
    assert(weekday_last{Monday}.ok());
    assert(Monday[last].ok());
    assert(Monday[last].weekday() == Monday);
    assert(weekday_last{Monday}.weekday() == Monday);
    assert(weekday_last{Monday} == weekday_last{Monday});
}

constexpr void month_day_test() {
    month_day md{January, 1d};
    assert(md.month() == January);
    assert(md.day() == 1d);

    assert(md < month_day(January, 2d));
    assert(month_day(January, 2d) > md);
    assert(md < month_day(December, 25d));
    assert(month_day(December, 25d) > md);
    assert(md == month_day(January, 1d));

    for (unsigned i = 1; i <= 12; ++i) {
        month m{i};
        for (unsigned d = 0; d <= 32; ++d) {
            if (d == 0) {
                assert(!month_day(m, day{d}).ok());
            } else if (d == 30 && m == February) {
                assert(!month_day(m, day{d}).ok());
                break;
            } else if (d == 31 && (m == April || m == June || m == September || m == November)) {
                assert(!month_day(m, day{d}).ok());
            } else if (d == 32) {
                assert(!month_day(m, day{d}).ok());
            } else {
                assert(month_day(m, day{d}).ok());
            }
        }
    }
}

constexpr void month_day_last_test() {
    assert((February / last).month() == February);

    unsigned i = 0;
    assert(!(month{i++} / last).ok());
    for (; i <= 12; ++i) {
        assert((month{i} / last).ok());
    }
    assert(!(month{i} / last).ok());

    assert(January / last == January / last);
    assert(January / last < February / last);
    assert(December / last > February / last);
}

constexpr void month_weekday_test() {
    const auto mwd1 = January / Monday[2];
    assert(mwd1.month() == January);
    assert(mwd1.weekday_indexed().weekday() == Monday);
    assert(mwd1.weekday_indexed().index() == 2);

    assert(January / Monday[2] == January / Monday[2]);
}

constexpr void month_weekday_last_test() {
    const auto mwdl = January / Monday[last];
    assert(mwdl.month() == January);
    assert(mwdl.weekday_last().weekday() == Monday);
    assert(mwdl == January / Monday[last]);
}

constexpr void year_month_test() {
    auto ym = 2020y / January;
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
}

constexpr void year_month_day_test() {
    year_month_day ymd1{2020y / January / 1d};
    assert(ymd1.year() == 2020y);
    assert(ymd1.month() == January);
    assert(ymd1.day() == 1d);

    year_month_day ymd2{2020y / January / last};
    assert(ymd2.year() == 2020y);
    assert(ymd2.month() == January);
    assert(ymd2.day() == 31d);

    year_month_day epoch{sys_days{}};
    assert(epoch.year() == 1970y);
    assert(epoch.month() == January);
    assert(epoch.day() == 1d);

    local_days ldp;
    sys_days sys{ldp.time_since_epoch()};
    year_month_day ymld{ldp};
    assert((ymld == year_month_day{sys}));

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

    assert(2020y / April / 06d == sys_days{days{18'358}});
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
}

constexpr void year_month_day_last_test() {
    auto ymdl = 2020y / February / last;
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
}

constexpr void year_month_weekday_test() {
    auto ymwd = 2020y / April / Tuesday[2];
    assert(ymwd == 2020y / April / Tuesday[2]);
    assert(ymwd.year() == 2020y);
    assert(ymwd.month() == April);
    assert(ymwd.weekday() == Tuesday);
    assert(ymwd.index() == 2u);
    assert(ymwd.weekday_indexed() == Tuesday[2]);

    const year_month_weekday epoch{sys_days{}};
    assert(epoch == 1970y / January / Thursday[1]);

    local_days ldp;
    sys_days sys{ldp.time_since_epoch()};
    year_month_weekday ymlwd{ldp};
    assert((ymlwd == year_month_weekday{sys}));

    ymwd += months{2};
    assert(ymwd == 2020y / June / Tuesday[2]);
    ymwd -= months{2};
    assert(ymwd == 2020y / April / Tuesday[2]);

    ymwd += years{2};
    assert(ymwd == 2022y / April / Tuesday[2]);
    ymwd -= years{2};
    assert(ymwd == 2020y / April / Tuesday[2]);

    assert(static_cast<sys_days>(epoch) == sys_days{});
    const auto prev = 1970y / January / Thursday[0];
    assert(static_cast<sys_days>(prev) == (sys_days{} - days{7}));
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
}

constexpr void year_month_weekday_last_test() {
    auto ymwdl = 2020y / January / Monday[last];
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

    assert((static_cast<sys_days>(ymwdl) == sys_days{days{18'288}}));
    assert((static_cast<local_days>(ymwdl) == local_days{ymwdl}));

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

void ok_test() {
    // month_weekday
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
    // year_month
    constexpr int y_min = static_cast<int>(year::min());
    constexpr int y_max = static_cast<int>(year::max());

    for (int y = y_min; y <= y_max; y++) {
        for (auto m = 0u; m <= 255u; ++m) {
            const auto ym = year{y} / month{m};
            if (m >= 1 && m <= 12) {
                assert(ym.ok());
            } else {
                assert(!ym.ok());
            }
        }
    }
    // year_month_day
    for (int iy = y_min; iy <= y_max; ++iy) {
        for (auto um = 0u; um <= 255u; ++um) {
            for (auto ud = 0u; ud <= 32u; ++ud) {
                const year y{iy};
                const month m{um};
                const day d{ud};
                if (y.ok() && m.ok() && d >= 1d && d <= (y / m / last).day()) {
                    assert(((y / m / d).ok()));
                } else {
                    assert((!(y / m / d).ok()));
                }
            }
        }
    }
    // year_month_day_last
    for (int iy = y_min; iy <= y_max; ++iy) {
        for (auto m = 0u; m <= 255u; ++m) {
            const year y{iy};
            const auto mdl = month{m} / last;
            if (y.ok() && mdl.ok()) {
                assert((y / mdl).ok());
            } else {
                assert((!(y/ mdl).ok()));
            }
        }
    }
}

int main() {
    test();
    ok_test(); // tests fail to compile in constexpr
    static_assert(test());
}
