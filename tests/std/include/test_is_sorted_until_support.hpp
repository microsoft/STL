// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#pragma once

#include <algorithm>
#include <cassert>
#include <cmath>
#include <cstddef>
#include <type_traits>
#include <utility>
#include <vector>

#if _HAS_CXX20
#include <ranges>
#endif

template <class FwdIt, class Comp>
FwdIt last_known_good_is_sorted_until(FwdIt first, FwdIt last, Comp comp) {
    if (first == last) {
        return last;
    }

    FwdIt next = first;
    for (++next; next != last; ++first, ++next) {
        if (comp(*next, *first)) {
            return next;
        }
    }

    return last;
}

template <class T, class Comp>
void test_case_is_sorted_until(const std::vector<T>& input, Comp comp) {
    auto expected = last_known_good_is_sorted_until(input.begin(), input.end(), comp);
    auto actual   = std::is_sorted_until(input.begin(), input.end(), comp);
    assert(expected == actual);
#if _HAS_CXX20
    auto actual_r = std::ranges::is_sorted_until(input.begin(), input.end(), comp);
    assert(expected == actual_r);
#endif // _HAS_CXX20
}
