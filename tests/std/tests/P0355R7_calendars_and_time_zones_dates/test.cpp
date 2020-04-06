// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <cassert>
#include <chrono>
#include <type_traits>

using namespace std;
using namespace std::chrono;

#define TRIVIAL_COPY_STANDARD_LAYOUT(TYPE)                                                       \
    static_assert(is_trivially_copyable_v<TYPE>, "chrono::" #TYPE " is not trivially copyable"); \
    static_assert(is_standard_layout_v<TYPE>, "chrono::" #TYPE " is not standard layout");

constexpr void day_test() {
    TRIVIAL_COPY_STANDARD_LAYOUT(day)

    day d1{0u};
    assert(static_cast<unsigned>(d1) == 0);
    assert(++d1 == day{1});
    assert(d1++ == day{1});
    assert(d1 == day{2});

    assert(--d1 == day{1});
    assert(d1-- == day{1});
    assert(d1 == day{0});

    d1 += days{2};
    assert(d1 == day{2});
    d1 -= days{2};
    assert(d1 == day{0});

    day d2{0u};
    assert(d1 == d2++);
    assert(d1 < d2);
    assert(d2 > d1);

    day d3{0};
    assert(!d3.ok());
    ++d3;
    for (int i = 1; i <= 31; ++i, ++d3) {
        assert(d3.ok());
    }
    assert(!d3.ok());

    const day d4{2};
    const day d5{10};
    const days diff = d5 - d4;
    assert(diff == days{8});

    const auto d6 = 0d;
    assert(d1 == d6);
    static_assert(is_same_v<day, decltype(0d)>, "0d is not chrono::day");
    static_assert(is_same_v<const day, decltype(d6)>, "d6 is not chrono::day");
}

constexpr void month_test() {
    TRIVIAL_COPY_STANDARD_LAYOUT(month)

    month m1{1u};
    assert(static_cast<unsigned>(m1) == 1);
    assert(++m1 == month{2});
    assert(m1++ == month{2});
    assert(m1 == month{3});

    assert(--m1 == month{2});
    assert(m1-- == month{2});
    assert(m1 == month{1});

    m1 += months{2};
    assert(m1 == month{3});
    m1 -= months{2};
    assert(m1 == month{1});

    for (unsigned i = 0; i <= 255; ++i) {
        if (i >= 1 && i <= 12) {
            assert(month{i}.ok());
        } else {
            assert(!month{i}.ok());
        }
    }

    month m2{1u};
    assert(m1 == m2++);
    assert(m2 > m1);
    assert(m1 < m2);

    month m3 = m2 + months{11};
    assert(m3 == month{1});
    m3 = months{11} + m2;
    assert(m3 == month{1});
    month m4 = m2 - months{2};
    assert(m4 == month{12});

    months diff = m1 - m2;
    assert(diff == months{11});

    static_assert(is_same_v<const month, decltype(January)>);
    static_assert(is_same_v<const month, decltype(February)>);
    static_assert(is_same_v<const month, decltype(March)>);
    static_assert(is_same_v<const month, decltype(April)>);
    static_assert(is_same_v<const month, decltype(May)>);
    static_assert(is_same_v<const month, decltype(June)>);
    static_assert(is_same_v<const month, decltype(July)>);
    static_assert(is_same_v<const month, decltype(August)>);
    static_assert(is_same_v<const month, decltype(September)>);
    static_assert(is_same_v<const month, decltype(October)>);
    static_assert(is_same_v<const month, decltype(November)>);
    static_assert(is_same_v<const month, decltype(December)>);
    assert(month{1} == January);
    assert(month{2} == February);
    assert(month{3} == March);
    assert(month{4} == April);
    assert(month{5} == May);
    assert(month{6} == June);
    assert(month{7} == July);
    assert(month{8} == August);
    assert(month{9} == September);
    assert(month{10} == October);
    assert(month{11} == November);
    assert(month{12} == December);
}

constexpr void year_test() {
    TRIVIAL_COPY_STANDARD_LAYOUT(year)

    year y1{1};
    assert(static_cast<int>(y1) == 1);
    assert(++y1 == year{2});
    assert(y1++ == year{2});
    assert(y1 == year{3});

    assert(--y1 == year{2});
    assert(y1-- == year{2});
    assert(y1 == year{1});

    y1 += years{2};
    assert(y1 == year{3});
    y1 -= years{2};
    assert(y1 == year{1});

    assert(+y1 == year{1});
    assert(-y1 == year{-1});

    constexpr int y_min = -32767;
    constexpr int y_max = 32767;
    assert(year::min() == year{y_min});
    assert(year::max() == year{y_max});

    for (int i = y_min; i <= y_max; i++) {
        assert(year{i}.ok());
        if (i % 4 == 0 && (i % 100 != 0 || i % 400 == 0)) {
            assert(year{i}.is_leap());
        } else {
            assert(!year{i}.is_leap());
        }
    }
    year y2{1};
    assert(y1 == y2++);
    assert(y1 < y2);
    assert(y2 > y1);

    y2 = y1 + years{4};
    assert(y2 == year{5});
    y2 = years{4} + y1;
    assert(y2 == year{5});

    year y3 = y2 - years{4};
    assert(y3 == year{1});

    const auto y4 = 2020y;
    static_assert(is_same_v<year, decltype(0y)>, "0y is not chrono::year");
    static_assert(is_same_v<const year, decltype(y4)>, "y4 is not chrono::year");
}

constexpr void weekday_test() {
    TRIVIAL_COPY_STANDARD_LAYOUT(weekday)

    assert(weekday{7} == weekday{0});
    assert(weekday{sys_days{}} == weekday{4});
    assert(weekday{local_days{}} == sys_days{local_days{}.time_since_epoch()});

    weekday wd{0u};
    assert(wd == weekday{0});
    assert(++wd == weekday{1});
    assert(wd++ == weekday{1});
    assert(wd == weekday{2});

    assert(--wd == weekday{1});
    assert(wd-- == weekday{1});
    assert(wd == weekday{0});

    wd += days{2};
    assert(wd == weekday{2});
    wd -= days{3};
    assert(wd == weekday{6});

    assert(weekday{0}.c_encoding() == 0u);
    assert(weekday{0}.iso_encoding() == 7u);

    for (unsigned i = 0; i <= 255; ++i) {
        if (i <= 7) {
            assert(weekday{i}.ok());
        } else {
            assert(!weekday{i}.ok());
        }
    }
    static_assert(is_same_v<weekday_indexed, decltype(wd[1])>);
    static_assert(is_same_v<weekday_last, decltype(wd[last])>);

    assert(weekday{1} + days{6} == weekday{0});
    assert(weekday{0} - weekday{1} == days{6});

    static_assert(is_same_v<const weekday, decltype(Sunday)>);
    static_assert(is_same_v<const weekday, decltype(Monday)>);
    static_assert(is_same_v<const weekday, decltype(Tuesday)>);
    static_assert(is_same_v<const weekday, decltype(Wednesday)>);
    static_assert(is_same_v<const weekday, decltype(Thursday)>);
    static_assert(is_same_v<const weekday, decltype(Friday)>);
    static_assert(is_same_v<const weekday, decltype(Saturday)>);
    assert(weekday{0} == Sunday);
    assert(weekday{1} == Monday);
    assert(weekday{2} == Tuesday);
    assert(weekday{3} == Wednesday);
    assert(weekday{4} == Thursday);
    assert(weekday{5} == Friday);
    assert(weekday{6} == Saturday);
}

constexpr void weekday_indexed_test() {
    TRIVIAL_COPY_STANDARD_LAYOUT(weekday_indexed)

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
    TRIVIAL_COPY_STANDARD_LAYOUT(weekday_last)

    assert(weekday_last{Monday}.ok());
    assert(Monday[last].ok());
    assert(Monday[last].weekday() == Monday);
    assert(weekday_last{Monday}.weekday() == Monday);
    assert(weekday_last{Monday} == weekday_last{Monday});
}

constexpr void month_day_test() {
    TRIVIAL_COPY_STANDARD_LAYOUT(month_day)

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
    TRIVIAL_COPY_STANDARD_LAYOUT(month_day_last)

    assert(month_day_last{February}.month() == February);
    const auto mdl = January / last;
    static_assert(is_same_v<const month_day_last, decltype(mdl)>, "mdl is not const chrono::month_day_last");

    unsigned i = 0;
    assert(!month_day_last{month{i++}}.ok());
    for (; i <= 12; ++i) {
        assert(month_day_last{month{i}}.ok());
    }
    assert(!month_day_last{month{i}}.ok());

    assert(month_day_last{January} == month_day_last{January});
    assert(month_day_last{January} < month_day_last{February});
    assert(month_day_last{December} > month_day_last{February});
}

constexpr void month_weekday_test() {
    TRIVIAL_COPY_STANDARD_LAYOUT(month_weekday)

    const month_weekday mwd1{January, Monday[2]};
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

    assert((month_weekday{January, Monday[2]} == month_weekday{January, Monday[2]}));
}

constexpr void month_weekday_last_test() {
    TRIVIAL_COPY_STANDARD_LAYOUT(month_weekday_last)

    month_weekday_last mwdl{January, Monday[last]};
    assert(mwdl.month() == January);
    assert(mwdl.weekday_last().weekday() == Monday);
    assert((mwdl == month_weekday_last{January, Monday[last]}));
}

constexpr void year_month_test() {
    TRIVIAL_COPY_STANDARD_LAYOUT(year_month)

    year_month ym{2020y, January};
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

    assert((year_month{2020y, April} == year_month{2020y, April}));
    assert((year_month{2019y, April} < year_month{2020y, April}));
    assert((year_month{2020y, March} < year_month{2020y, April}));
    assert((year_month{2020y, April} > year_month{2019y, April}));
    assert((year_month{2020y, April} > year_month{2020y, March}));

    assert((ym + months{2} == year_month{2020y, March}));
    assert((year_month{2020y, March} == ym + months{2}));

    assert((ym - months{2} == year_month{2019y, November}));

    assert((ym - year_month{2019y, January} == months{12}));

    assert((ym + years{2} == year_month{2022y, January}));
    assert((year_month{2022y, January} == ym + years{2}));


    assert((ym - years{2} == year_month{2018y, January}));
}

constexpr void year_month_day_test() {
    TRIVIAL_COPY_STANDARD_LAYOUT(year_month_day)

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
    TRIVIAL_COPY_STANDARD_LAYOUT(year_month_day_last)

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

    constexpr int y_min = static_cast<int>(year::min());
    constexpr int y_max = static_cast<int>(year::max());
    for (int iy = y_min; iy <= y_max; ++iy) {
        for (unsigned m = 0; m <= 255; ++m) {
            year y{iy};
            month_day_last mdl{month{m}};
            if (y.ok() && mdl.ok()) {
                assert((year_month_day_last{y, mdl}.ok()));
            } else {
                assert((!year_month_day_last{y, mdl}.ok()));
            }
        }
    }

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
    return true;
}

int main() {
    test();
    static_assert(test());
}
