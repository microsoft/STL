// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

// The following code generates data for `_Width_estimate_intervals_v2` in <format>.

#include <charconv>
#include <cstdint>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <regex>
#include <string>
#include <system_error>
#include <vector>
using namespace std;

void verify_impl(bool test, int line, const char* msg) {
    if (!test) {
        cerr << "Error at line " << line << ": " << msg << endl;
        exit(EXIT_FAILURE);
    }
}
#define VERIFY(expr, msg) verify_impl((expr), __LINE__, (msg))
constexpr const char* impl_assertion_failed = "impl assertion failed";

struct range_u {
    uint32_t from;
    uint32_t to;
    range_u(uint32_t f, uint32_t t) : from(f), to(t) {}
    range_u(uint32_t v) : from(v), to(v) {}
};

enum class width_u : bool { is_1 = false, is_2 = true };

class table_u {
    // A valid Unicode code point won't exceed `max_u`.
    static constexpr uint32_t max_u = 0x10'ffff;
    vector<width_u> table;

public:
    table_u() : table(max_u + 1, width_u::is_1) {}
    void fill_range(const range_u rng, const width_u width) {
        const auto [from, to] = rng;
        VERIFY(from <= to && to <= max_u, impl_assertion_failed);
        for (uint32_t u = from; u <= to; ++u) {
            table[u] = width;
        }
    }

    void print_intervals() const {
        // Print table for `_Width_estimate_intervals_v2`.
        int c        = 0;
        width_u last = table[0];
        for (uint32_t u = 0; u <= max_u; ++u) {
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

    void print_clusters_1_vs_2(const table_u& other) const {
        vector<bool> cluster_table(max_u + 1, false);
        for (uint32_t u = 0; u <= max_u; ++u) {
            if (table[u] == width_u::is_1 && other.table[u] == width_u::is_2) {
                cluster_table[u] = true;
            }
        }

        for (uint32_t u = 0; u <= max_u; ++u) {
            if (cluster_table[u]) {
                uint32_t from = u;
                uint32_t to   = from;
                while (to + 1 <= max_u && cluster_table[to + 1]) {
                    ++to;
                }
                if (from == to) {
                    cout << hex << uppercase << "U+" << from << endl;
                } else {
                    cout << hex << uppercase << "U+" << from << "..U+" << to << endl;
                }
                u = to;
            }
        }
    }
};

table_u get_table_cpp20() {
    const range_u std_wide_ranges_cpp20[]{
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

    table_u table;
    for (const range_u rng : std_wide_ranges_cpp20) {
        table.fill_range(rng, width_u::is_2);
    }
    return table;
}

// Read data from "EastAsianWidth.txt".
// The latest version can be found at:
// https://www.unicode.org/Public/UCD/latest/ucd/EastAsianWidth.txt
// The current implementation works for:
// https://www.unicode.org/Public/15.0.0/ucd/EastAsianWidth.txt
// To make this function work, the file should not contain a BOM.
table_u read_from(ifstream& source) {
    table_u table;

    // "The unassigned code points in the following blocks default to "W":"
    const range_u default_wide_ranges[]{
        {0x4E00, 0x9FFF}, {0x3400, 0x4DBF}, {0xF900, 0xFAFF}, {0x20000, 0x2FFFD}, {0x30000, 0x3FFFD}};
    for (const range_u rng : default_wide_ranges) {
        table.fill_range(rng, width_u::is_2);
    }

    // Read explicitly assigned ranges.
    // The lines that are not empty or pure comment are uniformly of the format "HEX(..HEX)?;(A|F|H|N|Na|W) #comment".
    auto get_width = [](const string& str) -> width_u {
        if (str == "F" || str == "W") {
            return width_u::is_2;
        } else {
            VERIFY(str == "A" || str == "H" || str == "N" || str == "Na", impl_assertion_failed);
            return width_u::is_1;
        }
    };
    auto get_value = [](const string& str) -> uint32_t {
        uint32_t value{};
        auto [end, ec] = from_chars(str.data(), str.data() + str.size(), value, 16);
        VERIFY(end == str.data() + str.size() && ec == errc{}, impl_assertion_failed);
        return value;
    };

    VERIFY(!!source, "invalid path");
    string line;
    const regex reg(R"(([0-9A-Z]+)(\.\.[0-9A-Z]+)?;(A|F|H|N|Na|W) *#.*)");
    while (getline(source, line)) {
        if (!line.empty() && !line.starts_with("#")) {
            smatch match;
            VERIFY(regex_match(line, match, reg), "invalid line");
            VERIFY(match[1].matched && match[3].matched, impl_assertion_failed);
            const width_u width = get_width(match[3].str());
            const uint32_t from = get_value(match[1].str());
            if (match[2].matched) {
                // range (HEX..HEX)
                string match2 = match[2].str();
                VERIFY(match2.starts_with(".."), impl_assertion_failed);
                table.fill_range({from, get_value(match2.substr(2))}, width);
            } else {
                // single character (HEX)
                table.fill_range({from}, width);
            }
        }
    }

    return table;
}

table_u get_table_cpp23(ifstream& source) {
    table_u table = read_from(source);

    // Override with ranges specified by the C++ standard.
    const range_u std_wide_ranges_cpp23[]{{0x4DC0, 0x4DFF}, {0x1F300, 0x1F5FF}, {0x1F900, 0x1F9FF}};
    for (const range_u rng : std_wide_ranges_cpp23) {
        table.fill_range(rng, width_u::is_2);
    }

    return table;
}

int main() {
    cout << "Old table:\n";
    const table_u old_table = get_table_cpp20();
    old_table.print_intervals();

    cout << "\nNew table:\nInput path for EastAsianWidth.txt: ";
    string path;
    getline(cin, path);
    ifstream source(path);
    table_u new_table = get_table_cpp23(source);
    new_table.print_intervals();

    cout << "\nWas 1, now 2:\n";
    old_table.print_clusters_1_vs_2(new_table);
    cout << "\nWas 2, now 1:\n";
    new_table.print_clusters_1_vs_2(old_table);
}
