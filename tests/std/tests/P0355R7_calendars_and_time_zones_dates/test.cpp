// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <cassert>
#include <chrono>
#include <type_traits>

using namespace std;
using namespace std::chrono;

constexpr void day_test() {
    static_assert(is_trivially_copyable_v<day>, "chrono::day is not trivially copyable");
    static_assert(is_standard_layout_v<day>, "chrono::day is not standard layout");

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
    static_assert(is_trivially_copyable_v<month>, "chrono::month is not trivially copyable");
    static_assert(is_standard_layout_v<month>, "chrono::month is not standard layout");

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
    static_assert(is_trivially_copyable_v<year>, "chrono::year is not trivially copyable");
    static_assert(is_standard_layout_v<year>, "chrono::year is not standard layout");

    year y1{1};
    assert(static_cast<int>(y1) == 1);
    assert(++y1 == year{2});
    assert(y1++ == year{2});
    assert(y1 == year{3});

    assert(--y1 == year{2});
    assert(y1-- == year{2});
    assert(y1 ==   year{1});

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

constexpr bool test() {
    day_test();
    month_test();
    year_test();
    return true;
}

int main() {
    test();
    // static_assert(test());
}
