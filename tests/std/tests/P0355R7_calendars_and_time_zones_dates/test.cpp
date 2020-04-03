// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <cassert>
#include <chrono>
#include <type_traits>

using namespace std;
using namespace std::chrono;

constexpr bool day_test() {
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
    assert(d1 == d2);

    return true;
}


int main() {
    day_test();
    static_assert(day_test());
}
