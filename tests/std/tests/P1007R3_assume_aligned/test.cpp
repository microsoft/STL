// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <cassert>
#include <cstddef>
#include <memory>

constexpr void can_change_value_via_assume_aligned(int* p) {
    int* p1 = std::assume_aligned<256>(p);
    p1[0]   = 10;
    p1[1]   = -15;
}

constexpr int can_read_value_via_assume_aligned_and_sum_it(int const* p, size_t size) {
    int const* p1 = std::assume_aligned<256>(p);
    int result    = 0;
    for (size_t i = 0; i < size; ++i) {
        result += p1[i];
    }
    return result;
}

constexpr bool assume_aligned_can_change_value() {
    alignas(256) int arr[2] = {0, 0};
    can_change_value_via_assume_aligned(&arr[0]);
    assert(arr[0] == 10);
    assert(arr[1] == -15);
    return true;
}

constexpr bool assume_aligned_can_read_value() {
    alignas(256) constexpr int arr[3] = {10, 11, 9};
    assert(can_read_value_via_assume_aligned_and_sum_it(arr, 3) == 30);
    return true;
}

int main() {
    assume_aligned_can_change_value();
    static_assert(assume_aligned_can_change_value());

    assume_aligned_can_read_value();
    static_assert(assume_aligned_can_read_value());
}
