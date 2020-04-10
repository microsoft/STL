// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <cassert>
#include <chrono>
#include <type_traits>

using namespace std;
using namespace std::chrono;

constexpr void day_test() {
    day d1{0u};
    assert(static_cast<unsigned>(d1) == 0u);
    assert(d1 == 0d);
    assert(++d1 == 1d);
    assert(d1++ == 1d);
    assert(d1 == 2d);

    assert(--d1 == 1d);
    assert(d1-- == 1d);
    assert(d1 == 0d);

    d1 += days{2};
    assert(d1 == 2d);
    d1 -= days{2};
    assert(d1 == 0d);

    day d2{0u};
    assert(d1 == d2++);
    assert(d1 < d2);
    assert(d2 > d1);

    day d3{0u};
    assert(!d3.ok());
    ++d3;
    for (int i = 1; i <= 31; ++i, ++d3) {
        assert(d3.ok());
    }
    assert(!d3.ok());

    const days diff = 10d - 2d;
    assert(diff == days{8});
}

constexpr void month_test() {

    month m1{1u};
    assert(static_cast<unsigned>(m1) == 1u);
    assert(m1 == January);
    assert(++m1 == February);
    assert(m1++ == February);
    assert(m1 == March);

    assert(--m1 == February);
    assert(m1-- == February);
    assert(m1 == January);

    m1 += months{2};
    assert(m1 == March);
    m1 -= months{2};
    assert(m1 == January);

    for (unsigned i = 0; i <= 255; ++i) {
        if (i >= 1 && i <= 12) {
            assert(month{i}.ok());
        } else {
            assert(!month{i}.ok());
        }
    }

    assert(February > m1);
    assert(m1 < February);

    month m2 = February + months{11};
    assert(m2 == January);
    m2 = months{11} + February;
    assert(m2 == January);
    month m3 = February - months{2};
    assert(m3 == December);

    months diff = January - February;
    assert(diff == months{11});
}

constexpr void year_test() {
    year y1{1};
    assert(static_cast<int>(y1) == 1);
    assert(y1 == 1y);
    assert(++y1 == 2y);
    assert(y1++ == 2y);
    assert(y1 == 3y);

    assert(--y1 == 2y);
    assert(y1-- == 2y);
    assert(y1 == 1y);

    y1 += years{2};
    assert(y1 == 3y);
    y1 -= years{2};
    assert(y1 == 1y);

    assert(+y1 == 1y);
    assert(-y1 == -1y);

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
    assert(y1 < 2y);
    assert(2y > y1);

    year y2 = y1 + years{4};
    assert(y2 == 5y);
    y2 = years{4} + y1;
    assert(y2 == 5y);

    year y3 = y2 - years{4};
    assert(y3 == 1y);
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

    // error C2131: expression did not evaluate to a constant
    // failure was caused by evaluation exceeding step limit of 1048576
    // for (unsigned m = 0; m <= 255; ++m) {
    //    for (unsigned wd = 0; wd <= 255; ++wd) {
    //        for (unsigned wdi = 0; wdi <= 6; ++wdi) {
    //            const month_weekday mwd2{month{m}, weekday_indexed{weekday{wd}, wdi}};
    //            if (m >= 1 && m <= 12 && wd <= 7 && wdi >= 1 && wdi <= 5) {
    //                assert(mwd2.ok());
    //            } else {
    //                assert(!mwd2.ok());
    //            }
    //        }
    //    }
    //}

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

    // error C2131: expression did not evaluate to a constant
    // failure was caused by evaluation exceeding step limit of 1048576
    // for (int i = static_cast<int>(year::min()); i <= static_cast<int>(year::max()); i++) {
    //    for (unsigned j = 0; j <= 255; ++j) {
    //        const year_month ym_is{year{i}, month{j}};
    //        if (j >= 1 && j <= 12) {
    //            assert(ym_is.ok());
    //        } else {
    //            assert(!ym_is.ok());
    //        }
    //    }
    //}

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
    year_month_day ymd1{year{2020}, month{1}, day{1}};
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

    assert((year_month_day{2020y, April, 06d} == sys_days{days{18'358}}));
    assert(year_month_day{sys_days{2017y / January / 0}} == 2016y / December / 31);
    assert(year_month_day{sys_days{2017y / January / 31}} == 2017y / January / 31);
    assert(year_month_day{sys_days{2017y / January / 32}} == 2017y / February / 1);

    assert(static_cast<local_days>(ymld) == local_days{});

    constexpr int y_min = static_cast<int>(year::min());
    constexpr int y_max = static_cast<int>(year::max());

    // for (int iy = y_min; iy <= y_max; ++iy) {
    //    for (auto um = 0u; um <= 255u; ++um) {
    //        for (auto ud = 0u; ud <= 32u; ++ud) {
    //            year y{iy};
    //            month m{um};
    //            day d{ud};
    //            if (y.ok() && m.ok() && d >= 1d && d <= (y / m / last).day()) {
    //                assert(((y / m / d).ok()));
    //            } else {
    //                assert((!(y / m / d).ok()));
    //            }
    //        }
    //    }
    //}

    assert((year_month_day{2020y, January, 1d} == year_month_day{2020y, January, 1d}));
    assert((year_month_day{2019y, January, 1d} < year_month_day{2020y, January, 1d}));
    assert((year_month_day{2020y, January, 1d} < year_month_day{2020y, February, 1d}));
    assert((year_month_day{2020y, January, 1d} < year_month_day{2020y, January, 2d}));
    assert((year_month_day{2020y, January, 1d} > year_month_day{2019y, January, 1d}));
    assert((year_month_day{2020y, February, 1d} > year_month_day{2020y, January, 1d}));
    assert((year_month_day{2020y, January, 2d} > year_month_day{2020y, January, 1d}));

    const auto ymd3 = year_month_day{2019y, December, 31d} + months{2};
    assert((ymd3 == year_month_day{2020y, February, 31d}));
    assert(!ymd3.ok());
    assert((months{2} + year_month_day{2019y, December, 31d} == ymd3));

    const auto ymd4 = year_month_day{2020y, January, 1d} - months{2};
    assert((ymd4 == year_month_day{2019y, November, 1d}));

    const auto ymd5 = year_month_day{2020y, January, 1d} + years{2};
    assert((ymd5 == year_month_day{2022y, January, 1d}));
    assert((years{2} + year_month_day{2020y, January, 1d} == ymd5));

    const auto ymd6 = year_month_day{2020y, January, 1d} - years{2};
    assert((ymd6 == year_month_day{2018y, January, 1d}));
}

constexpr void year_month_day_last_test() {
    year_month_day_last ymdl{2020y, month_day_last{February}};
    assert(ymdl.year() == 2020y);
    assert(ymdl.month() == February);
    assert((ymdl.month_day_last() == month_day_last{February}));
    assert(ymdl.day() == 29d);

    ymdl += months{2};
    assert(ymdl.year() == 2020y);
    assert(ymdl.month() == April);
    assert((ymdl.month_day_last() == month_day_last{April}));
    assert(ymdl.day() == 30d);

    ymdl -= months{2};
    assert(ymdl.year() == 2020y);
    assert(ymdl.month() == February);
    assert((ymdl.month_day_last() == month_day_last{February}));
    assert(ymdl.day() == 29d);

    ymdl += years{2};
    assert(ymdl.year() == 2022y);
    assert(ymdl.month() == February);
    assert((ymdl.month_day_last() == month_day_last{February}));
    assert(ymdl.day() == 28d);

    ymdl -= years{2};
    assert(ymdl.year() == 2020y);
    assert(ymdl.month() == February);
    assert((ymdl.month_day_last() == month_day_last{February}));
    assert(ymdl.day() == 29d);

    assert((year_month_day_last{2020y, month_day_last{April}} == sys_days{days{18'382}}));
    assert(static_cast<local_days>(ymdl) == local_days{ymdl});

    // constexpr int y_min = static_cast<int>(year::min());
    // constexpr int y_max = static_cast<int>(year::max());
    // for (int iy = y_min; iy <= y_max; ++iy) {
    //    for (unsigned m = 0; m <= 255; ++m) {
    //        year y{iy};
    //        month_day_last mdl{month{m}};
    //        if (y.ok() && mdl.ok()) {
    //            assert((year_month_day_last{y, mdl}.ok()));
    //        } else {
    //            assert((!year_month_day_last{y, mdl}.ok()));
    //        }
    //    }
    //}

    assert((ymdl == year_month_day_last{2020y, February / last}));
    assert((ymdl < year_month_day_last{2021y, February / last}));
    assert((ymdl < year_month_day_last{2020y, March / last}));
    assert((year_month_day_last{2021y, February / last} > ymdl));
    assert((year_month_day_last{2020y, March / last} > ymdl));

    const auto ymdl2 = ymdl + months{2};
    assert((ymdl2 == year_month_day_last{2020y, April / last}));
    assert((ymdl2 == months{2} + ymdl));

    const auto ymdl3 = ymdl - months{2};
    assert((ymdl3 == year_month_day_last{2019y, December / last}));

    const auto ymdl4 = ymdl + years{2};
    assert((ymdl4 == year_month_day_last{2022y, February / last}));
    assert((ymdl4 == years{2} + ymdl));

    const auto ymdl5 = ymdl - years{2};
    assert((ymdl5 == year_month_day_last{2018y, February / last}));
}

constexpr void year_month_weekday_test() {
    year_month_weekday ymwd{2020y, April, Tuesday[2]};
    assert(ymwd.year() == 2020y);
    assert(ymwd.month() == April);
    assert(ymwd.weekday() == Tuesday);
    assert(ymwd.index() == 2u);
    assert((ymwd.weekday_indexed() == weekday_indexed{Tuesday, 2}));

    const year_month_weekday epoch{sys_days{}};
    assert(epoch.year() == 1970y);
    assert(epoch.month() == January);
    assert(epoch.weekday() == Thursday);
    assert(epoch.index() == 1u);
    assert((epoch.weekday_indexed() == weekday_indexed{Thursday, 1}));

    local_days ldp;
    sys_days sys{ldp.time_since_epoch()};
    year_month_weekday ymlwd{ldp};
    assert((ymlwd == year_month_weekday{sys}));

    ymwd += months{2};
    assert(ymwd.year() == 2020y);
    assert(ymwd.month() == June);
    assert(ymwd.weekday() == Tuesday);
    assert(ymwd.index() == 2u);
    assert((ymwd.weekday_indexed() == weekday_indexed{Tuesday, 2}));

    ymwd -= months{2};
    assert(ymwd.year() == 2020y);
    assert(ymwd.month() == April);
    assert(ymwd.weekday() == Tuesday);
    assert(ymwd.index() == 2u);
    assert((ymwd.weekday_indexed() == weekday_indexed{Tuesday, 2}));

    ymwd += years{2};
    assert(ymwd.year() == 2022y);
    assert(ymwd.month() == April);
    assert(ymwd.weekday() == Tuesday);
    assert(ymwd.index() == 2u);
    assert((ymwd.weekday_indexed() == weekday_indexed{Tuesday, 2}));

    ymwd -= years{2};
    assert(ymwd.year() == 2020y);
    assert(ymwd.month() == April);
    assert(ymwd.weekday() == Tuesday);
    assert(ymwd.index() == 2u);
    assert((ymwd.weekday_indexed() == weekday_indexed{Tuesday, 2}));

    assert(static_cast<sys_days>(epoch) == sys_days{});
    year_month_weekday prev{1970y / January / Thursday[0]};
    assert(static_cast<sys_days>(prev) == (sys_days{} - days{7}));
    assert(static_cast<local_days>(ymwd) == local_days{ymwd});


    assert((year_month_weekday{2020y / April / Wednesday[5]}.ok()));
    assert((!year_month_weekday{2020y / April / Tuesday[5]}.ok()));

    assert((ymwd == year_month_weekday{2020y, April, Tuesday[2]}));

    const auto ymwd2 = ymwd + months{2};
    assert((ymwd2 == year_month_weekday{2020y, June, Tuesday[2]}));
    const auto ymwd3 = months{2} + ymwd;
    assert(ymwd2 == ymwd3);

    const auto ymwd4 = ymwd - months{2};
    assert((ymwd4 == year_month_weekday{2020y, February, Tuesday[2]}));

    const auto ymwd5 = ymwd + years{2};
    assert((ymwd5 == year_month_weekday{2022y, April, Tuesday[2]}));
    const auto ymwd6 = years{2} + ymwd;
    assert(ymwd5 == ymwd6);

    const auto ymwd7 = ymwd - years{2};
    assert((ymwd7 == year_month_weekday{2018y, April, Tuesday[2]}));
}

constexpr void year_month_weekday_last_test() {
    year_month_weekday_last ymwdl{2020y, January, Monday[last]};
    assert(ymwdl.year() == 2020y);
    assert(ymwdl.month() == January);
    assert(ymwdl.weekday() == Monday);
    assert(ymwdl.weekday_last() == Monday[last]);

    ymwdl += months{2};
    assert(ymwdl.year() == 2020y);
    assert(ymwdl.month() == March);
    assert(ymwdl.weekday() == Monday);
    assert(ymwdl.weekday_last() == Monday[last]);

    ymwdl -= months{2};
    assert(ymwdl.year() == 2020y);
    assert(ymwdl.month() == January);
    assert(ymwdl.weekday() == Monday);
    assert(ymwdl.weekday_last() == Monday[last]);

    ymwdl += years{2};
    assert(ymwdl.year() == 2022y);
    assert(ymwdl.month() == January);
    assert(ymwdl.weekday() == Monday);
    assert(ymwdl.weekday_last() == Monday[last]);

    ymwdl -= years{2};
    assert(ymwdl.year() == 2020y);
    assert(ymwdl.month() == January);
    assert(ymwdl.weekday() == Monday);
    assert(ymwdl.weekday_last() == Monday[last]);

    assert((static_cast<sys_days>(ymwdl) == sys_days{days{18'288}}));
    assert((static_cast<local_days>(ymwdl) == local_days{ymwdl}));

    const auto ymwdl2 = ymwdl + months{2};
    assert((ymwdl2 == 2020y / March / Monday[last]));
    const auto ymwdl3 = months{2} + ymwdl;
    assert(ymwdl3 == ymwdl2);

    const auto ymwdl4 = ymwdl - months{2};
    assert(ymwdl4 == 2019y / November / Monday[last]);

    const auto ymwdl5 = ymwdl + years{2};
    assert(ymwdl5 == 2022y / January / Monday[last]);
    const auto ymwdl6 = years{2} + ymwdl;
    assert(ymwdl6 == ymwdl5);
    const auto ymwdl7 = ymwdl - years{2};
    assert(ymwdl7 == 2018y / January / Monday[last]);
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
