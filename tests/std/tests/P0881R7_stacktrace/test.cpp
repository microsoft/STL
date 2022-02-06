// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <cassert>
#include <filesystem>
#include <sstream>
#include <stacktrace>
#include <string>

using namespace std;

// Note: the bellow assumes tail call optimization disabled, which is in case in /Od

stacktrace all_innermost() {
    return stacktrace::current();
}

stacktrace all_inner() {
    return all_innermost();
}

stacktrace all_outer() {
    return all_inner();
}

stacktrace all_outermost() {
    return all_outer();
}

stacktrace all_but_top_innermost() {
    return stacktrace::current(1);
}

stacktrace all_but_top_inner() {
    return all_but_top_innermost();
}

stacktrace all_but_top_outer() {
    return all_but_top_inner();
}

stacktrace all_but_top_outermost() {
    return all_but_top_outer();
}

stacktrace three_excluding_top_innermost() {
    return stacktrace::current(1, 3);
}

stacktrace three_excluding_top_inner() {
    return three_excluding_top_innermost();
}

stacktrace three_excluding_top_outer() {
    return three_excluding_top_inner();
}

stacktrace three_excluding_top_outermost() {
    return three_excluding_top_outer();
}

string trim_past_plus(string str) {
    if (size_t pos = str.rfind("+", string::npos); pos != string::npos) {
        str.resize(pos);
    }
    return str;
}

string to_string_using_low_level_members(const stacktrace& st) {
    stringstream ss;
    for (const auto& i : st) {
        auto l = i.source_line();
        if (l != 0) {
            ss << i.source_file() << "(" << l << "): ";
        }
        ss << i.description() << "\n";
    }
    return ss.str();
}

string to_string_using_stream_entry(const stacktrace& st) {
    stringstream ss;
    for (const auto& i : st) {
        ss << i << "\n";
    }
    return ss.str();
}

string to_string_using_to_string_entry(const stacktrace& st) {
    stringstream ss;
    for (const auto& i : st) {
        ss << to_string(i) << "\n";
    }
    return ss.str();
}

string to_string_using_stream(const stacktrace& st) {
    stringstream ss;
    ss << st << "\n";
    return ss.str();
}

string to_string_using_to_string(const stacktrace& st) {
    return to_string(st) + "\n";
}


int main() {
    auto all = all_outermost();
    assert(all.size() >= 4);
    assert(filesystem::path(all.at(0).source_file()).filename() == "test.cpp"sv);
    assert(filesystem::path(all.at(1).source_file()).filename() == "test.cpp"sv);
    assert(filesystem::path(all.at(2).source_file()).filename() == "test.cpp"sv);
    assert(filesystem::path(all.at(3).source_file()).filename() == "test.cpp"sv);

    assert(all.at(0).source_line() == 15);
    assert(all.at(1).source_line() == 19);
    assert(all.at(2).source_line() == 23);
    assert(all.at(3).source_line() == 27);

    assert(trim_past_plus(all.at(0).description()) == "P0881R7_stacktrace!all_innermost"sv);
    assert(trim_past_plus(all.at(1).description()) == "P0881R7_stacktrace!all_inner"sv);
    assert(trim_past_plus(all.at(2).description()) == "P0881R7_stacktrace!all_outer"sv);
    assert(trim_past_plus(all.at(3).description()) == "P0881R7_stacktrace!all_outermost"sv);

    auto all_but_top = all_but_top_outermost();
    assert(all_but_top.size() >= 3);
    assert(filesystem::path(all_but_top[0].source_file()).filename() == "test.cpp"sv);
    assert(filesystem::path(all_but_top[1].source_file()).filename() == "test.cpp"sv);
    assert(filesystem::path(all_but_top[2].source_file()).filename() == "test.cpp"sv);

    assert(all_but_top[0].source_line() == 35);
    assert(all_but_top[1].source_line() == 39);
    assert(all_but_top[2].source_line() == 43);

    assert(trim_past_plus(all_but_top[0].description()) == "P0881R7_stacktrace!all_but_top_inner"sv);
    assert(trim_past_plus(all_but_top[1].description()) == "P0881R7_stacktrace!all_but_top_outer"sv);
    assert(trim_past_plus(all_but_top[2].description()) == "P0881R7_stacktrace!all_but_top_outermost"sv);

    auto three_excluding_top = three_excluding_top_outermost();
    assert(three_excluding_top.size() == 3);
    assert(filesystem::path(three_excluding_top[0].source_file()).filename() == "test.cpp"sv);
    assert(filesystem::path(three_excluding_top[1].source_file()).filename() == "test.cpp"sv);
    assert(filesystem::path(three_excluding_top[2].source_file()).filename() == "test.cpp"sv);

    assert(three_excluding_top[0].source_line() == 51);
    assert(three_excluding_top[1].source_line() == 55);
    assert(three_excluding_top[2].source_line() == 59);

    assert(trim_past_plus(three_excluding_top[0].description()) == "P0881R7_stacktrace!three_excluding_top_inner"sv);
    assert(trim_past_plus(three_excluding_top[1].description()) == "P0881R7_stacktrace!three_excluding_top_outer"sv);
    assert(
        trim_past_plus(three_excluding_top[2].description()) == "P0881R7_stacktrace!three_excluding_top_outermost"sv);


    try {
        (void) all.at(all.size());
        assert(false); // should have thrown
    } catch (out_of_range) {
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
