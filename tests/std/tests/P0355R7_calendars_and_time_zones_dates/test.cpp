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
    assert(static_cast<unsigned>(++d1) == 1);
    assert(static_cast<unsigned>(d1++) == 1);
    assert(static_cast<unsigned>(d1) == 2);

    assert(static_cast<unsigned>(--d1) == 1);
    assert(static_cast<unsigned>(d1--) == 1);
    assert(static_cast<unsigned>(d1) == 0);

    d1 += days{2};
    assert(static_cast<unsigned>(d1) == 2);
    d1 -= days{2};
    assert(static_cast<unsigned>(d1) == 0);

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

    const auto d4 = 0d;
    assert(d1 == d4);
    static_assert(is_same_v<day, decltype(0d)>, "0d is not chrono::day");
    // static_assert(is_same_v<day, decltype(d4)>, "d4 is not chrono::day");
}

constexpr bool test() {
    day_test();
    return true;
}

int main() {
    test();
    static_assert(test());
}
