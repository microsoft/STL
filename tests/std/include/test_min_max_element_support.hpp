// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#pragma once

#include <algorithm>
#include <cassert>
#include <cstddef>
#include <utility>
#include <vector>

#ifdef __cpp_lib_concepts
#include <ranges>
#endif

template <class FwdIt>
FwdIt last_known_good_min_element(FwdIt first, FwdIt last) {
    FwdIt result = first;

    for (; first != last; ++first) {
        if (*first < *result) {
            result = first;
        }
    }

    return result;
}

template <class FwdIt>
FwdIt last_known_good_max_element(FwdIt first, FwdIt last) {
    FwdIt result = first;

    for (; first != last; ++first) {
        if (*result < *first) {
            result = first;
        }
    }

    return result;
}

template <class FwdIt>
std::pair<FwdIt, FwdIt> last_known_good_minmax_element(FwdIt first, FwdIt last) {
    // find smallest and largest elements
    std::pair<FwdIt, FwdIt> found(first, first);

    if (first != last) {
        while (++first != last) { // process one or two elements
            FwdIt next = first;
            if (++next == last) { // process last element
                if (*first < *found.first) {
                    found.first = first;
                } else if (!(*first < *found.second)) {
                    found.second = first;
                }
            } else { // process next two elements
                if (*next < *first) { // test next for new smallest
                    if (*next < *found.first) {
                        found.first = next;
                    }

                    if (!(*first < *found.second)) {
                        found.second = first;
                    }
                } else { // test first for new smallest
                    if (*first < *found.first) {
                        found.first = first;
                    }

                    if (!(*next < *found.second)) {
                        found.second = next;
                    }
                }
                first = next;
            }
        }
    }

    return found;
}

template <class T>
void test_case_min_max_element(const std::vector<T>& input) {
    auto expected_min    = last_known_good_min_element(input.begin(), input.end());
    auto expected_max    = last_known_good_max_element(input.begin(), input.end());
    auto expected_minmax = last_known_good_minmax_element(input.begin(), input.end());
    auto actual_min      = std::min_element(input.begin(), input.end());
    auto actual_max      = std::max_element(input.begin(), input.end());
    auto actual_minmax   = std::minmax_element(input.begin(), input.end());
    assert(expected_min == actual_min);
    assert(expected_max == actual_max);
    assert(expected_minmax == actual_minmax);
#ifdef __cpp_lib_concepts
    using std::ranges::views::take, std::ptrdiff_t;

    auto actual_min_range          = std::ranges::min_element(input);
    auto actual_max_range          = std::ranges::max_element(input);
    auto actual_minmax_range       = std::ranges::minmax_element(input);
    auto actual_min_sized_range    = std::ranges::min_element(take(input, static_cast<ptrdiff_t>(input.size())));
    auto actual_max_sized_range    = std::ranges::max_element(take(input, static_cast<ptrdiff_t>(input.size())));
    auto actual_minmax_sized_range = std::ranges::minmax_element(take(input, static_cast<ptrdiff_t>(input.size())));
    assert(expected_min == actual_min_range);
    assert(expected_max == actual_max_range);
    assert(expected_minmax.first == actual_minmax_range.min);
    assert(expected_minmax.second == actual_minmax_range.max);
    assert(expected_min == actual_min_sized_range);
    assert(expected_max == actual_max_sized_range);
    assert(expected_minmax.first == actual_minmax_sized_range.min);
    assert(expected_minmax.second == actual_minmax_sized_range.max);
#endif // __cpp_lib_concepts
}
