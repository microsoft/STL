// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <algorithm>
#include <cassert>
#include <functional>
#include <string>

using namespace std;

int main() {
    // Optimized cases
    assert(clamp(2, 4, 7) == 4);
    assert(clamp(6, 4, 7) == 6);
    assert(clamp(8, 4, 7) == 7);

    assert(clamp(2, 4, 7, less{}) == 4);
    assert(clamp(6, 4, 7, less{}) == 6);
    assert(clamp(8, 4, 7, less{}) == 7);

    assert(clamp(2, 4, 7, less<int>{}) == 4);
    assert(clamp(6, 4, 7, less<int>{}) == 6);
    assert(clamp(8, 4, 7, less<int>{}) == 7);

    const int data[100] = {};
    assert(clamp(data + 22, data + 30, data + 70) == data + 30);
    assert(clamp(data + 44, data + 30, data + 70) == data + 44);
    assert(clamp(data + 88, data + 30, data + 70) == data + 70);

    // Generic cases
    assert(clamp(2, 4, 7, less<long long>{}) == 4);
    assert(clamp(6, 4, 7, less<long long>{}) == 6);
    assert(clamp(8, 4, 7, less<long long>{}) == 7);

    assert(clamp("apple"s, "banana"s, "coconut"s) == "banana"s);
    assert(clamp("blueberry"s, "banana"s, "coconut"s) == "blueberry"s);
    assert(clamp("cucumber"s, "banana"s, "coconut"s) == "coconut"s);

#if _ITERATOR_DEBUG_LEVEL < 2
    // Make sure we keep the early return for generic cases:
    // Either less than min or greater than max input should produce only one comparison.
    // For _ITERATOR_DEBUG_LEVEL == 2 we validate the predicate, so there are extra comparisons.

    int comparisons        = 0;
    const auto counted_cmp = [&comparisons](auto a, auto b) {
        ++comparisons;
        return a < b;
    };

    assert(clamp(2, 4, 7, counted_cmp) == 4);
    assert(clamp(6, 4, 7, counted_cmp) == 6);
    assert(clamp(8, 4, 7, counted_cmp) == 7);
    assert(comparisons == 5);

    comparisons = 0;
    assert(clamp("apple"s, "banana"s, "coconut"s, counted_cmp) == "banana"s);
    assert(clamp("blueberry"s, "banana"s, "coconut"s, counted_cmp) == "blueberry"s);
    assert(clamp("cucumber"s, "banana"s, "coconut"s, counted_cmp) == "coconut"s);
    assert(comparisons == 5);
#endif // _ITERATOR_DEBUG_LEVEL < 2
}
