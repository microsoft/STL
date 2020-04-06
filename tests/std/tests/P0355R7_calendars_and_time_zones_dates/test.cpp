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

constexpr bool test() {
    day_test();
    month_test();
    year_test();
    weekday_test();
    weekday_indexed_test();
    weekday_last_test();
    month_day_test();
    month_day_last_test();
    year_month_test();
    return true;
}

int main() {
    test();
    static_assert(test());
}
