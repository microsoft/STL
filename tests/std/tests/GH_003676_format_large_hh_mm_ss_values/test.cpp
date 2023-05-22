// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <cassert>
#include <chrono>
#include <format>
#include <iostream>
#include <sstream>

using namespace std;
using namespace chrono;
using namespace chrono_literals;

void assert_equal(const auto& expected, const auto& value) {
    if (expected == value) {
        return;
    }

    cout << "Expected: " << expected << endl;
    cout << "Got:      " << value << endl;
    assert(false);
}

template <typename R, typename P>
void assert_duration_format_equal(const duration<R, P>& duration, const string& str) {
    stringstream ss;
    ss << hh_mm_ss{duration};
    assert_equal(str, ss.str());
    assert_equal(str, format("{:%T}", duration));
}

template <typename R, typename P>
void assert_duration_format_equal_positive(const duration<R, P>& duration, const string& str) {
    assert_duration_format_equal(duration, str);
    assert_duration_format_equal(-duration, '-' + str);
}

int main() {
    assert_duration_format_equal(0ns, "00:00:00.000000000");
    assert_duration_format_equal(-0h, "00:00:00");
    assert_duration_format_equal(years{0}, "00:00:00");

    assert_duration_format_equal_positive(3h, "03:00:00");
    assert_duration_format_equal_positive(10h + 20min + 30s, "10:20:30");
    assert_duration_format_equal_positive(days{3}, "72:00:00");
    assert_duration_format_equal_positive(years{1}, "8765:49:12");
    assert_duration_format_equal_positive(duration<float, days::period>{1.55f}, "37:12:00");
    assert_duration_format_equal_positive(2ms, "00:00:00.002");
    assert_duration_format_equal_positive(60min, "01:00:00");
}
