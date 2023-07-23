// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

// The following code generates data for `_Width_estimate_intervals` in <format>.

#include <charconv>
#include <format>
#include <fstream>
#include <iostream>
#include <regex>
#include <vector>

void _verify(bool test, int line, const char* msg) {
    if (!test) {
        std::cerr << "error at line " << line << ":" << msg << std::endl;
        exit(EXIT_FAILURE);
    }
}
#define verify(expr, msg) _verify((expr), __LINE__, (msg))
static const char* impl_assertion_failed = "impl assertion failed";

struct range_u {
    uint32_t from, to;
    range_u(uint32_t f, uint32_t t) : from(f), to(t) {}
    range_u(uint32_t v) : from(v), to(v) {}
};

// The largest possible unicode won't exceed max_u.
const uint32_t max_u = 0x1f'ffff;
using table_u        = std::vector<bool>; // true: wide
table_u make_table() {
    return table_u(max_u + 1, false);
}

// For `_Width_estimate_intervals`.
void print_intervals(const table_u& table) {
    using namespace std;
    cout << endl;
    int c     = 0;
    bool last = table[0];
    for (uint32_t u = 0; u <= max_u; u++) {
        if (table[u] != last) {
            cout << "0x" << hex << uppercase << u << "u, ";
            if (++c == 12) {
                c = 0;
                cout << endl;
            }
        }
        last = table[u];
    }
    cout << endl;
}

void fill_range(table_u& table, const ::range_u rng, bool is_wide) {
    const auto [from, to] = rng;
    verify(from <= to && to <= max_u, impl_assertion_failed);
    for (uint32_t u = from; u <= to; u++) {
        table[u] = is_wide;
    }
}

table_u get_table_cpp20() {
    using namespace std;
    const vector<range_u> std_wide_ranges_cpp20{
        {0x1100, 0x115F},
        {0x2329, 0x232A},
        {0x2E80, 0x303E},
        {0x3040, 0xA4CF},
        {0xAC00, 0xD7A3},
        {0xF900, 0xFAFF},
        {0xFE10, 0xFE19},
        {0xFE30, 0xFE6F},
        {0xFF00, 0xFF60},
        {0xFFE0, 0xFFE6},
        {0x1F300, 0x1F64F},
        {0x1F900, 0x1F9FF},
        {0x20000, 0x2FFFD},
        {0x30000, 0x3FFFD},
    };
    table_u table = make_table();
    for (const range_u rng : std_wide_ranges_cpp20) {
        fill_range(table, rng, true);
    }
    return table;
}

// Read data from a file with the same content as in https://www.unicode.org/Public/15.0.0/ucd/EastAsianWidth.txt
// To make this function work, the file should not contain a BOM.
table_u read_from_source(std::ifstream& source) {
    using namespace std;

    table_u table = make_table();

    // "The unassigned code points in the following blocks default to "W":"
    const vector<range_u> default_wide_ranges{
        {0x4E00, 0x9FFF}, {0x3400, 0x4DBF}, {0xF900, 0xFAFF}, {0x20000, 0x2FFFD}, {0x30000, 0x3FFFD}};
    for (const range_u rng : default_wide_ranges) {
        fill_range(table, rng, true);
    }

    // Read explicitly assigned ranges.
    // The lines that are not empty or pure comment are uniformly of the format "hex(..hex)?;(A|F|H|N|Na|W) #comment".
    auto test_wide = [](const string& str) -> bool {
        if (str == "F" || str == "W") {
            return true;
        } else {
            verify(str == "A" || str == "H" || str == "N" || str == "Na", impl_assertion_failed);
            return false;
        }
    };
    auto get_value = [](const string& str) -> uint32_t {
        uint32_t value{};
        auto [end, ec] = from_chars(str.data(), str.data() + str.size(), value, 16);
        verify(end == str.data() + str.size() && ec == errc{}, impl_assertion_failed);
        return value;
    };

    verify(!!source, "invalid ifstream");
    string line;
    const regex reg(R"(([0-9a-zA-Z]+)(\.\.[0-9a-zA-Z]+)?;(A|F|H|N|Na|W)\s*#.*)");
    while (getline(source, line)) {
        if (!line.empty() && !line.starts_with("#")) {
            smatch match;
            verify(regex_match(line, match, reg), "invalid line");
            verify(match[1].matched && match[3].matched, impl_assertion_failed);
            bool is_wide  = test_wide(match[3].str());
            uint32_t from = get_value(match[1].str());
            if (match[2].matched) {
                // range (hex..hex)
                string match2 = match[2].str();
                verify(match2.starts_with(".."), impl_assertion_failed);
                fill_range(table, {from, get_value(match2.substr(2))}, is_wide);
            } else {
                // single character (hex)
                fill_range(table, {from}, is_wide);
            }
        }
    }

    return table;
}

table_u get_table_cpp23(std::ifstream& source) {
    using namespace std;
    table_u table = read_from_source(source);

    // Override with ranges specified by the C++ standard.
    const vector<range_u> std_wide_ranges_cpp23{{0x4DC0, 0x4DFF}, {0x1F300, 0x1F5FF}, {0x1F900, 0x1F9FF}};
    for (const range_u rng : std_wide_ranges_cpp23) {
        fill_range(table, rng, true);
    }

    return table;
}

// Confirm that we get the same result as in the annex in
// https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2023/p2675r1.pdf
void compare_with_cpp20(const table_u& table /*gotten from get_table_cpp23*/) {
    using namespace std;

    auto print_clusters = [](const table_u& table) {
        for (uint32_t u = 0; u <= max_u; u++) {
            if (table[u]) {
                uint32_t from = u;
                uint32_t to   = from;
                while (to + 1 <= max_u && table[to + 1]) {
                    ++to;
                }
                if (from == to) {
                    cout << hex << uppercase << from << endl;
                } else {
                    cout << hex << uppercase << from << ".." << to << endl;
                }
                u = to;
            }
        }
    };

    const table_u old_table = get_table_cpp20();
    table_u diff_table      = make_table();
    cout << "\nwas 1, now 2:\n";
    for (uint32_t u = 0; u <= max_u; u++) {
        if (!old_table[u] && table[u]) {
            diff_table[u] = true;
        }
    }
    print_clusters(diff_table);

    diff_table = make_table(); // Reset all bits.
    cout << "\nwas 2, now 1:\n";
    for (uint32_t u = 0; u <= max_u; u++) {
        if (old_table[u] && !table[u]) {
            diff_table[u] = true;
        }
    }
    print_clusters(diff_table);
}

int main() {
    // print_intervals(get_table_cpp20());
    // 0x1100u, 0x1160u, 0x2329u, 0x232Bu, 0x2E80u, 0x303Fu, 0x3040u, 0xA4D0u, 0xAC00u, 0xD7A4u, 0xF900u, 0xFB00u,
    // 0xFE10u, 0xFE1Au, 0xFE30u, 0xFE70u, 0xFF00u, 0xFF61u, 0xFFE0u, 0xFFE7u, 0x1F300u, 0x1F650u, 0x1F900u, 0x1FA00u,
    // 0x20000u, 0x2FFFEu, 0x30000u, 0x3FFFEu,

    std::ifstream source("EastAsianWidth.txt");
    table_u table = get_table_cpp23(source);
    print_intervals(table);
    compare_with_cpp20(table);
}
