// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <chrono>
#include <type_traits>

using namespace std;
using namespace std::chrono;

static_assert(noexcept(year{} / month{}));
static_assert(noexcept(year{} / 0));
static_assert(noexcept(month{} / day{}));
static_assert(noexcept(month{} / 0));
static_assert(noexcept(0 / day{}));
static_assert(noexcept(day{} / month{}));
static_assert(noexcept(day{} / 0));
static_assert(noexcept(month{} / last));
static_assert(noexcept(0 / last));
static_assert(noexcept(last / month{}));
static_assert(noexcept(last / 0));
static_assert(noexcept(month{} / Sunday[1]));
static_assert(noexcept(0 / Sunday[1]));
static_assert(noexcept(Sunday[1] / month{}));
static_assert(noexcept(Sunday[1] / 0));
static_assert(noexcept(month{} / Sunday[last]));
static_assert(noexcept(0 / Sunday[last]));
static_assert(noexcept(Sunday[last] / month{}));
static_assert(noexcept(Sunday[last] / 0));
static_assert(noexcept(year_month{} / day{}));
static_assert(noexcept(year_month{} / 0));
static_assert(noexcept(year{} / month_day{}));
static_assert(noexcept(0 / month_day{}));
static_assert(noexcept(month_day{} / year{}));
static_assert(noexcept(month_day{} / 0));
static_assert(noexcept(year_month{} / last));
static_assert(noexcept(year{} / month_day_last{January}));
static_assert(noexcept(0 / month_day_last{January}));
static_assert(noexcept(month_day_last{January} / year{}));
static_assert(noexcept(month_day_last{January} / 0));
static_assert(noexcept(year_month{} / Sunday[1]));
static_assert(noexcept(year{} / month_weekday{January, Sunday[1]}));
static_assert(noexcept(0 / month_weekday{January, Sunday[1]}));
static_assert(noexcept(month_weekday{January, Sunday[1]} / year{}));
static_assert(noexcept(month_weekday{January, Sunday[1]} / 0));
static_assert(noexcept(year_month{} / Sunday[last]));
static_assert(noexcept(year{} / month_weekday_last{January, Sunday[last]}));
static_assert(noexcept(0 / month_weekday_last{January, Sunday[last]}));
static_assert(noexcept(month_weekday_last{January, Sunday[last]} / year{}));
static_assert(noexcept(month_weekday_last{January, Sunday[last]} / 0));

#define TRIVIAL_COPY_STANDARD_LAYOUT(TYPE)                                                       \
    static_assert(is_trivially_copyable_v<TYPE>, "chrono::" #TYPE " is not trivially copyable"); \
    static_assert(is_standard_layout_v<TYPE>, "chrono::" #TYPE " is not standard-layout");

TRIVIAL_COPY_STANDARD_LAYOUT(day)
TRIVIAL_COPY_STANDARD_LAYOUT(month)
TRIVIAL_COPY_STANDARD_LAYOUT(year)
TRIVIAL_COPY_STANDARD_LAYOUT(weekday)
TRIVIAL_COPY_STANDARD_LAYOUT(weekday_indexed)
TRIVIAL_COPY_STANDARD_LAYOUT(weekday_last)
TRIVIAL_COPY_STANDARD_LAYOUT(month_day)
TRIVIAL_COPY_STANDARD_LAYOUT(month_day_last)
TRIVIAL_COPY_STANDARD_LAYOUT(month_weekday)
TRIVIAL_COPY_STANDARD_LAYOUT(month_weekday_last)
TRIVIAL_COPY_STANDARD_LAYOUT(year_month)
TRIVIAL_COPY_STANDARD_LAYOUT(year_month_day)
TRIVIAL_COPY_STANDARD_LAYOUT(year_month_day_last)
TRIVIAL_COPY_STANDARD_LAYOUT(year_month_weekday)
TRIVIAL_COPY_STANDARD_LAYOUT(year_month_weekday_last)

#define TYPE_ASSERT(TYPE, EXPR) \
    static_assert(is_same_v<TYPE, remove_const_t<decltype(EXPR)>>, #EXPR " is not chrono::" #TYPE);

TYPE_ASSERT(day, 0d)
TYPE_ASSERT(year, 0y)
TYPE_ASSERT(month, January)
TYPE_ASSERT(month, February)
TYPE_ASSERT(month, March)
TYPE_ASSERT(month, April)
TYPE_ASSERT(month, May)
TYPE_ASSERT(month, June)
TYPE_ASSERT(month, July)
TYPE_ASSERT(month, August)
TYPE_ASSERT(month, September)
TYPE_ASSERT(month, October)
TYPE_ASSERT(month, November)
TYPE_ASSERT(month, December)
TYPE_ASSERT(weekday, Sunday)
TYPE_ASSERT(weekday, Monday)
TYPE_ASSERT(weekday, Tuesday)
TYPE_ASSERT(weekday, Wednesday)
TYPE_ASSERT(weekday, Thursday)
TYPE_ASSERT(weekday, Friday)
TYPE_ASSERT(weekday, Saturday)
TYPE_ASSERT(last_spec, last)
TYPE_ASSERT(weekday_indexed, declval<weekday>()[1])
TYPE_ASSERT(weekday_last, declval<weekday>()[last])

TYPE_ASSERT(year_month, 2020y / January)
TYPE_ASSERT(year_month, 2020y / 1)

TYPE_ASSERT(month_day, January / 1d)
TYPE_ASSERT(month_day, January / 1)
TYPE_ASSERT(month_day, 1 / 1d)
TYPE_ASSERT(month_day, 1d / January)
TYPE_ASSERT(month_day, 1d / 1)

TYPE_ASSERT(month_day_last, January / last)
TYPE_ASSERT(month_day_last, 1 / last)
TYPE_ASSERT(month_day_last, last / January)
TYPE_ASSERT(month_day_last, last / 1)

TYPE_ASSERT(month_weekday, January / Monday[1])
TYPE_ASSERT(month_weekday, 1 / Monday[1])
TYPE_ASSERT(month_weekday, Monday[1] / January)
TYPE_ASSERT(month_weekday, Monday[1] / 1)

TYPE_ASSERT(month_weekday_last, January / Monday[last])
TYPE_ASSERT(month_weekday_last, 1 / Monday[last])
TYPE_ASSERT(month_weekday_last, Monday[last] / January)
TYPE_ASSERT(month_weekday_last, Monday[last] / 1)

TYPE_ASSERT(year_month_day, 2020y / January / 1d)
TYPE_ASSERT(year_month_day, 2020y / January / 1)
constexpr auto md = January / 1;
TYPE_ASSERT(year_month_day, 2020y / md)
TYPE_ASSERT(year_month_day, 2020 / md)
TYPE_ASSERT(year_month_day, January / 1 / 2020y)
TYPE_ASSERT(year_month_day, January / 1 / 2020)
TYPE_ASSERT(year_month_day, 1d / January / 2020y)
TYPE_ASSERT(year_month_day, 1d / January / 2020)
TYPE_ASSERT(year_month_day, 1d / 1 / 2020)
TYPE_ASSERT(year_month_day, 1d / 1 / 2020y)

TYPE_ASSERT(year_month_day_last, 2020y / January / last)
constexpr auto mdl = January / last;
TYPE_ASSERT(year_month_day_last, 2020y / mdl)
TYPE_ASSERT(year_month_day_last, 2020 / mdl)
TYPE_ASSERT(year_month_day_last, last / January / 2020y)
TYPE_ASSERT(year_month_day_last, January / last / 2020)

TYPE_ASSERT(year_month_weekday, 2020y / January / Monday[1])
constexpr auto mwd = January / Monday[1];
TYPE_ASSERT(year_month_weekday, 2020y / mwd)
TYPE_ASSERT(year_month_weekday, 2020 / mwd)
TYPE_ASSERT(year_month_weekday, January / Monday[1] / 2020y)
TYPE_ASSERT(year_month_weekday, January / Monday[1] / 2020)
TYPE_ASSERT(year_month_weekday, Monday[1] / January / 2020y)
TYPE_ASSERT(year_month_weekday, Monday[1] / January / 2020)

TYPE_ASSERT(year_month_weekday_last, 2020y / January / Monday[last])
constexpr auto mwdl = January / Monday[last];
TYPE_ASSERT(year_month_weekday_last, 2020y / mwdl)
TYPE_ASSERT(year_month_weekday_last, 2020 / mwdl)
TYPE_ASSERT(year_month_weekday_last, January / Monday[last] / 2020y)
TYPE_ASSERT(year_month_weekday_last, January / Monday[last] / 2020)
TYPE_ASSERT(year_month_weekday_last, 1 / Monday[last] / 2020)
TYPE_ASSERT(year_month_weekday_last, 1 / Monday[last] / 2020y)
TYPE_ASSERT(year_month_weekday_last, Monday[last] / 1 / 2020y)
TYPE_ASSERT(year_month_weekday_last, Monday[last] / 1 / 2020)
TYPE_ASSERT(year_month_weekday_last, Monday[last] / January / 2020)
TYPE_ASSERT(year_month_weekday_last, Monday[last] / January / 2020y)

#define VALUE_ASSERT(VALUE, EXPECTED) static_assert(VALUE == EXPECTED, "chrono::" #VALUE " is not " #EXPECTED);
VALUE_ASSERT(month{1}, January)
VALUE_ASSERT(month{2}, February)
VALUE_ASSERT(month{3}, March)
VALUE_ASSERT(month{4}, April)
VALUE_ASSERT(month{5}, May)
VALUE_ASSERT(month{6}, June)
VALUE_ASSERT(month{7}, July)
VALUE_ASSERT(month{8}, August)
VALUE_ASSERT(month{9}, September)
VALUE_ASSERT(month{10}, October)
VALUE_ASSERT(month{11}, November)
VALUE_ASSERT(month{12}, December)

VALUE_ASSERT(weekday{0}, Sunday)
VALUE_ASSERT(weekday{1}, Monday)
VALUE_ASSERT(weekday{2}, Tuesday)
VALUE_ASSERT(weekday{3}, Wednesday)
VALUE_ASSERT(weekday{4}, Thursday)
VALUE_ASSERT(weekday{5}, Friday)
VALUE_ASSERT(weekday{6}, Saturday)
