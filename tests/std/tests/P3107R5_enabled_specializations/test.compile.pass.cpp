// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <chrono>
#include <cstddef>
#include <format>
#include <list>
#include <ranges>
#include <stacktrace>
#include <string>
#include <string_view>
#include <thread>
#include <tuple>
#include <utility>
#include <vector>

using namespace std;

struct unoptimized {};

template <class CharT>
struct std::formatter<unoptimized, CharT> : formatter<string_view, CharT> {
    template <class FormatContext>
    auto format(const unoptimized&, FormatContext& ctx) const {
        return formatter<string_view, CharT>::format("unoptimized", ctx);
    }
};

static_assert(formattable<unoptimized, char>);
static_assert(!enable_nonlocking_formatter_optimization<unoptimized>);

template <class T>
struct myalloc {
    using value_type = T;

    template <class U>
    myalloc(const myalloc<U>&);

    T* allocate(size_t);
    void deallocate(T*, size_t) noexcept;

    template <class U>
    bool operator==(const myalloc<U>&) const;
};

// <chrono> ====================================================================
static_assert(enable_nonlocking_formatter_optimization<chrono::seconds>);
static_assert(!enable_nonlocking_formatter_optimization<chrono::duration<unoptimized>>);

// Other than `duration` itself, other <chrono> types that depend on `duration` should be unconditionally
// enabled since they decompose `duration`s rather than formatting them directly.
static_assert(enable_nonlocking_formatter_optimization<chrono::sys_time<chrono::seconds>>);
static_assert(enable_nonlocking_formatter_optimization<chrono::sys_time<chrono::duration<unoptimized>>>);
static_assert(enable_nonlocking_formatter_optimization<chrono::utc_time<chrono::seconds>>);
static_assert(enable_nonlocking_formatter_optimization<chrono::utc_time<chrono::duration<unoptimized>>>);
static_assert(enable_nonlocking_formatter_optimization<chrono::tai_time<chrono::seconds>>);
static_assert(enable_nonlocking_formatter_optimization<chrono::tai_time<chrono::duration<unoptimized>>>);
static_assert(enable_nonlocking_formatter_optimization<chrono::gps_time<chrono::seconds>>);
static_assert(enable_nonlocking_formatter_optimization<chrono::gps_time<chrono::duration<unoptimized>>>);
static_assert(enable_nonlocking_formatter_optimization<chrono::file_time<chrono::seconds>>);
static_assert(enable_nonlocking_formatter_optimization<chrono::file_time<chrono::duration<unoptimized>>>);
static_assert(enable_nonlocking_formatter_optimization<chrono::local_time<chrono::seconds>>);
static_assert(enable_nonlocking_formatter_optimization<chrono::local_time<chrono::duration<unoptimized>>>);
static_assert(enable_nonlocking_formatter_optimization<decltype(chrono::local_time_format(chrono::local_seconds{}))>);

static_assert(enable_nonlocking_formatter_optimization<chrono::day>);
static_assert(enable_nonlocking_formatter_optimization<chrono::month>);
static_assert(enable_nonlocking_formatter_optimization<chrono::year>);
static_assert(enable_nonlocking_formatter_optimization<chrono::weekday>);
static_assert(enable_nonlocking_formatter_optimization<chrono::weekday_indexed>);
static_assert(enable_nonlocking_formatter_optimization<chrono::weekday_last>);
static_assert(enable_nonlocking_formatter_optimization<chrono::month_day>);
static_assert(enable_nonlocking_formatter_optimization<chrono::month_day_last>);
static_assert(enable_nonlocking_formatter_optimization<chrono::month_weekday>);
static_assert(enable_nonlocking_formatter_optimization<chrono::month_weekday_last>);
static_assert(enable_nonlocking_formatter_optimization<chrono::year_month>);
static_assert(enable_nonlocking_formatter_optimization<chrono::year_month_day>);
static_assert(enable_nonlocking_formatter_optimization<chrono::year_month_day_last>);
static_assert(enable_nonlocking_formatter_optimization<chrono::year_month_weekday>);
static_assert(enable_nonlocking_formatter_optimization<chrono::year_month_weekday_last>);

static_assert(enable_nonlocking_formatter_optimization<chrono::hh_mm_ss<chrono::seconds>>);
static_assert(enable_nonlocking_formatter_optimization<chrono::hh_mm_ss<chrono::duration<unoptimized>>>);

static_assert(enable_nonlocking_formatter_optimization<chrono::sys_info>);
static_assert(enable_nonlocking_formatter_optimization<chrono::local_info>);

static_assert(enable_nonlocking_formatter_optimization<chrono::zoned_time<chrono::seconds>>);
static_assert(enable_nonlocking_formatter_optimization<chrono::zoned_time<chrono::duration<unoptimized>>>);
static_assert(!enable_nonlocking_formatter_optimization<chrono::zoned_time<chrono::seconds, long long>>);

// <format> ====================================================================
static_assert(enable_nonlocking_formatter_optimization<char>);
static_assert(enable_nonlocking_formatter_optimization<wchar_t>);
static_assert(enable_nonlocking_formatter_optimization<char*>);
static_assert(enable_nonlocking_formatter_optimization<wchar_t*>);
static_assert(enable_nonlocking_formatter_optimization<const char*>);
static_assert(enable_nonlocking_formatter_optimization<const wchar_t*>);
static_assert(enable_nonlocking_formatter_optimization<char[42]>);
static_assert(enable_nonlocking_formatter_optimization<wchar_t[42]>);
static_assert(enable_nonlocking_formatter_optimization<string>);
static_assert(enable_nonlocking_formatter_optimization<wstring>);
static_assert(enable_nonlocking_formatter_optimization<basic_string<char, char_traits<char>, myalloc<char>>>);
static_assert(enable_nonlocking_formatter_optimization<basic_string<wchar_t, char_traits<wchar_t>, myalloc<wchar_t>>>);
static_assert(enable_nonlocking_formatter_optimization<string_view>);
static_assert(enable_nonlocking_formatter_optimization<wstring_view>);
static_assert(enable_nonlocking_formatter_optimization<bool>);
static_assert(enable_nonlocking_formatter_optimization<signed char>);
static_assert(enable_nonlocking_formatter_optimization<unsigned char>);
static_assert(enable_nonlocking_formatter_optimization<short>);
static_assert(enable_nonlocking_formatter_optimization<unsigned short>);
static_assert(enable_nonlocking_formatter_optimization<int>);
static_assert(enable_nonlocking_formatter_optimization<unsigned int>);
static_assert(enable_nonlocking_formatter_optimization<long>);
static_assert(enable_nonlocking_formatter_optimization<unsigned long>);
static_assert(enable_nonlocking_formatter_optimization<long long>);
static_assert(enable_nonlocking_formatter_optimization<unsigned long long>);
static_assert(enable_nonlocking_formatter_optimization<float>);
static_assert(enable_nonlocking_formatter_optimization<double>);
static_assert(enable_nonlocking_formatter_optimization<long double>);
static_assert(enable_nonlocking_formatter_optimization<nullptr_t>);
static_assert(enable_nonlocking_formatter_optimization<void*>);
static_assert(enable_nonlocking_formatter_optimization<const void*>);
static_assert(enable_nonlocking_formatter_optimization<pair<int, int>>);
static_assert(!enable_nonlocking_formatter_optimization<pair<unoptimized, int>>);
static_assert(!enable_nonlocking_formatter_optimization<pair<int, unoptimized>>);
static_assert(enable_nonlocking_formatter_optimization<tuple<int, int>>);
static_assert(!enable_nonlocking_formatter_optimization<tuple<unoptimized, int>>);
static_assert(!enable_nonlocking_formatter_optimization<tuple<int, unoptimized>>);

// Validate that various ranges are unoptimized
static_assert(!enable_nonlocking_formatter_optimization<list<int>>);
static_assert(!enable_nonlocking_formatter_optimization<list<int, myalloc<int>>>);
static_assert(!enable_nonlocking_formatter_optimization<list<unoptimized>>);
static_assert(!enable_nonlocking_formatter_optimization<list<unoptimized, myalloc<unoptimized>>>);
static_assert(!enable_nonlocking_formatter_optimization<vector<bool>>);
static_assert(!enable_nonlocking_formatter_optimization<vector<bool, myalloc<bool>>>);
static_assert(!enable_nonlocking_formatter_optimization<vector<int>>);
static_assert(!enable_nonlocking_formatter_optimization<vector<int, myalloc<int>>>);
static_assert(!enable_nonlocking_formatter_optimization<vector<unoptimized>>);
static_assert(!enable_nonlocking_formatter_optimization<vector<unoptimized, myalloc<unoptimized>>>);
using R = decltype(vector<int>{} | views::take(3));
static_assert(!enable_nonlocking_formatter_optimization<R>);

// <stacktrace> ================================================================
static_assert(enable_nonlocking_formatter_optimization<stacktrace_entry>);
static_assert(enable_nonlocking_formatter_optimization<stacktrace>);
static_assert(enable_nonlocking_formatter_optimization<basic_stacktrace<myalloc<stacktrace_entry>>>);

// <thread> ====================================================================
static_assert(enable_nonlocking_formatter_optimization<thread::id>);

// <vector> ====================================================================
static_assert(enable_nonlocking_formatter_optimization<vector<bool>::reference>);
static_assert(enable_nonlocking_formatter_optimization<vector<bool, myalloc<bool>>::reference>);
