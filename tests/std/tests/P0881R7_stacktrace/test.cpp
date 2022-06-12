// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <cassert>
#include <cstddef>
#include <filesystem>
#include <iterator>
#include <sstream>
#include <stacktrace>
#include <stdexcept>
#include <string>
#include <string_view>
#include <thread>

#ifdef HAS_EXPORT
#define MAYBE_EXPORT __declspec(dllexport)
#else // ^^^ HAS_EXPORT ^^^ / vvv !HAS_EXPORT vvv
#define MAYBE_EXPORT
#endif // ^^^ !HAS_EXPORT ^^^

using namespace std;

[[maybe_unused]] const int base_line = __LINE__;

// Note: the below assumes that tail call optimization is disabled, which is the case in /Od

MAYBE_EXPORT stacktrace all_innermost() {
    return stacktrace::current();
}

MAYBE_EXPORT stacktrace all_inner() {
    return all_innermost();
}

MAYBE_EXPORT stacktrace all_outer() {
    return all_inner();
}

MAYBE_EXPORT stacktrace all_outermost() {
    return all_outer();
}

MAYBE_EXPORT stacktrace all_but_top_innermost() {
    return stacktrace::current(1);
}

MAYBE_EXPORT stacktrace all_but_top_inner() {
    return all_but_top_innermost();
}

MAYBE_EXPORT stacktrace all_but_top_outer() {
    return all_but_top_inner();
}

MAYBE_EXPORT stacktrace all_but_top_outermost() {
    return all_but_top_outer();
}

MAYBE_EXPORT stacktrace three_excluding_top_innermost() {
    return stacktrace::current(1, 3);
}

MAYBE_EXPORT stacktrace three_excluding_top_inner() {
    return three_excluding_top_innermost();
}

MAYBE_EXPORT stacktrace three_excluding_top_outer() {
    return three_excluding_top_inner();
}

MAYBE_EXPORT stacktrace three_excluding_top_outermost() {
    return three_excluding_top_outer();
}

string trim_past_plus(string str) {
    if (size_t pos = str.rfind("+", string::npos); pos != string::npos) {
        str.resize(pos);
    }
    return str;
}

string to_string_using_low_level_members(const stacktrace& st) {
    ostringstream oss;
    int n = 0;
    for (const auto& i : st) {
        oss << n << "> ";
        ++n;
        auto l = i.source_line();
        if (l != 0) {
            oss << i.source_file() << "(" << l << "): ";
        }
        oss << i.description() << "\n";
    }
    return oss.str();
}

string to_string_using_stream_entry(const stacktrace& st) {
    ostringstream oss;
    int n = 0;
    for (const auto& i : st) {
        oss << n << "> ";
        ++n;
        oss << i << "\n";
    }
    return oss.str();
}

string to_string_using_to_string_entry(const stacktrace& st) {
    ostringstream oss;
    int n = 0;
    for (const auto& i : st) {
        oss << n << "> ";
        ++n;
        oss << to_string(i) << "\n";
    }
    return oss.str();
}

string to_string_using_stream(const stacktrace& st) {
    stringstream oss;
    oss << st << "\n";
    return oss.str();
}

string to_string_using_to_string(const stacktrace& st) {
    return to_string(st) + "\n";
}

#if defined(HAS_DEBUG_INFO) || defined(HAS_EXPORT)
#define HAS_NAMES
#endif // ^^^ defined(HAS_DEBUG_INFO) || defined(HAS_EXPORT) ^^^

void test_impl() {
    auto all = all_outermost();
    assert(all.size() >= 4);

#ifdef HAS_DEBUG_INFO
    assert(filesystem::path(all.at(0).source_file()).filename() == "test.cpp"sv);
    assert(filesystem::path(all.at(1).source_file()).filename() == "test.cpp"sv);
    assert(filesystem::path(all.at(2).source_file()).filename() == "test.cpp"sv);
    assert(filesystem::path(all.at(3).source_file()).filename() == "test.cpp"sv);

    assert(all.at(0).source_line() == base_line + 5);
    assert(all.at(1).source_line() == base_line + 9);
    assert(all.at(2).source_line() == base_line + 13);
    assert(all.at(3).source_line() == base_line + 17);
#else // ^^^ HAS_DEBUG_INFO ^^^ / vvv !HAS_DEBUG_INFO vvv
    assert(filesystem::path(all.at(0).source_file()).filename() == ""sv);
    assert(filesystem::path(all.at(1).source_file()).filename() == ""sv);
    assert(filesystem::path(all.at(2).source_file()).filename() == ""sv);
    assert(filesystem::path(all.at(3).source_file()).filename() == ""sv);

    assert(all.at(0).source_line() == 0);
    assert(all.at(1).source_line() == 0);
    assert(all.at(2).source_line() == 0);
    assert(all.at(3).source_line() == 0);
#endif // ^^^ !HAS_DEBUG_INFO ^^^

#ifdef HAS_NAMES
    assert(trim_past_plus(all.at(0).description()) == "P0881R7_stacktrace!all_innermost"sv);
    assert(trim_past_plus(all.at(1).description()) == "P0881R7_stacktrace!all_inner"sv);
    assert(trim_past_plus(all.at(2).description()) == "P0881R7_stacktrace!all_outer"sv);
    assert(trim_past_plus(all.at(3).description()) == "P0881R7_stacktrace!all_outermost"sv);
#else // ^^^ HAS_NAMES ^^^ / vvv !HAS_NAMES vvv
    assert(trim_past_plus(all.at(0).description()) == "P0881R7_stacktrace"sv);
    assert(trim_past_plus(all.at(1).description()) == "P0881R7_stacktrace"sv);
    assert(trim_past_plus(all.at(2).description()) == "P0881R7_stacktrace"sv);
    assert(trim_past_plus(all.at(3).description()) == "P0881R7_stacktrace"sv);
#endif // ^^^ !HAS_NAMES ^^^

    auto all_but_top = all_but_top_outermost();
    assert(all_but_top.size() >= 3);

#ifdef HAS_DEBUG_INFO
    assert(filesystem::path(all_but_top[0].source_file()).filename() == "test.cpp"sv);
    assert(filesystem::path(all_but_top[1].source_file()).filename() == "test.cpp"sv);
    assert(filesystem::path(all_but_top[2].source_file()).filename() == "test.cpp"sv);

    assert(all_but_top[0].source_line() == base_line + 25);
    assert(all_but_top[1].source_line() == base_line + 29);
    assert(all_but_top[2].source_line() == base_line + 33);
#else // ^^^ HAS_DEBUG_INFO ^^^ / vvv !HAS_DEBUG_INFO vvv
    assert(filesystem::path(all_but_top[0].source_file()).filename() == ""sv);
    assert(filesystem::path(all_but_top[1].source_file()).filename() == ""sv);
    assert(filesystem::path(all_but_top[2].source_file()).filename() == ""sv);

    assert(all_but_top[0].source_line() == 0);
    assert(all_but_top[1].source_line() == 0);
    assert(all_but_top[2].source_line() == 0);
#endif // ^^^ !HAS_DEBUG_INFO ^^^

#ifdef HAS_NAMES
    assert(trim_past_plus(all_but_top[0].description()) == "P0881R7_stacktrace!all_but_top_inner"sv);
    assert(trim_past_plus(all_but_top[1].description()) == "P0881R7_stacktrace!all_but_top_outer"sv);
    assert(trim_past_plus(all_but_top[2].description()) == "P0881R7_stacktrace!all_but_top_outermost"sv);
#else // ^^^ HAS_NAMES ^^^ / vvv !HAS_NAMES vvv
    assert(trim_past_plus(all_but_top[0].description()) == "P0881R7_stacktrace"sv);
    assert(trim_past_plus(all_but_top[1].description()) == "P0881R7_stacktrace"sv);
    assert(trim_past_plus(all_but_top[2].description()) == "P0881R7_stacktrace"sv);
#endif // ^^^ !HAS_NAMES ^^^

    auto three_excluding_top = three_excluding_top_outermost();
    assert(three_excluding_top.size() == 3);
#ifdef HAS_DEBUG_INFO
    assert(filesystem::path(three_excluding_top[0].source_file()).filename() == "test.cpp"sv);
    assert(filesystem::path(three_excluding_top[1].source_file()).filename() == "test.cpp"sv);
    assert(filesystem::path(three_excluding_top[2].source_file()).filename() == "test.cpp"sv);

    assert(three_excluding_top[0].source_line() == base_line + 41);
    assert(three_excluding_top[1].source_line() == base_line + 45);
    assert(three_excluding_top[2].source_line() == base_line + 49);
#else // ^^^ HAS_DEBUG_INFO ^^^ / vvv !HAS_DEBUG_INFO vvv
    assert(filesystem::path(three_excluding_top[0].source_file()).filename() == ""sv);
    assert(filesystem::path(three_excluding_top[1].source_file()).filename() == ""sv);
    assert(filesystem::path(three_excluding_top[2].source_file()).filename() == ""sv);

    assert(three_excluding_top[0].source_line() == 0);
    assert(three_excluding_top[1].source_line() == 0);
    assert(three_excluding_top[2].source_line() == 0);
#endif // ^^^ !HAS_DEBUG_INFO ^^^

#ifdef HAS_NAMES
    assert(trim_past_plus(three_excluding_top[0].description()) == "P0881R7_stacktrace!three_excluding_top_inner"sv);
    assert(trim_past_plus(three_excluding_top[1].description()) == "P0881R7_stacktrace!three_excluding_top_outer"sv);
    assert(
        trim_past_plus(three_excluding_top[2].description()) == "P0881R7_stacktrace!three_excluding_top_outermost"sv);
#else // ^^^ HAS_NAMES ^^^ / vvv !HAS_NAMES vvv
    assert(trim_past_plus(three_excluding_top[0].description()) == "P0881R7_stacktrace"sv);
    assert(trim_past_plus(three_excluding_top[1].description()) == "P0881R7_stacktrace"sv);
    assert(trim_past_plus(three_excluding_top[2].description()) == "P0881R7_stacktrace"sv);
#endif // ^^^ !HAS_NAMES ^^^

    try {
        (void) all.at(all.size());
        assert(false); // should have thrown
    } catch (const out_of_range&) {
    }

    auto all_copy = all;

    assert(all == all_copy);
    assert(all != all_but_top);
    assert(all > all_but_top);
    assert(three_excluding_top < all_but_top);

    assert((all <=> all_copy) == strong_ordering::equal);
    assert((all <=> all_but_top) == strong_ordering::greater);
    assert((three_excluding_top <=> all_but_top) == strong_ordering::less);

    assert(hash<stacktrace>{}(all) == hash<stacktrace>{}(all_copy));
    assert(hash<stacktrace_entry>{}(all[0]) == hash<stacktrace_entry>{}(all_copy[0]));

    assert(!all.empty());
    assert(distance(all.begin(), all.end()) == static_cast<ptrdiff_t>(all.size()));
    assert(distance(all.rbegin(), all.rend()) == static_cast<ptrdiff_t>(all.size()));
    assert(distance(all.cbegin(), all.cend()) == static_cast<ptrdiff_t>(all.size()));
    assert(distance(all.crbegin(), all.crend()) == static_cast<ptrdiff_t>(all.size()));

    stacktrace empty_trace;
    assert(empty_trace.size() == 0);
    assert(empty_trace.begin() == empty_trace.end());
    assert(empty_trace.rbegin() == empty_trace.rend());
    assert(empty_trace.cbegin() == empty_trace.cend());
    assert(empty_trace.crbegin() == empty_trace.crend());

    stacktrace_entry empty_entry;
    assert(empty_entry.description() == ""sv);
    assert(empty_entry.source_file() == ""sv);
    assert(empty_entry.source_line() == 0);

    auto s = to_string_using_low_level_members(all);
    assert(s == to_string_using_stream_entry(all));
    assert(s == to_string_using_stream(all));
    assert(s == to_string_using_to_string_entry(all));
    assert(s == to_string_using_to_string(all));
}

int main() {
    jthread t{test_impl};
    test_impl();
}
