// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <algorithm>
#include <cassert>
#include <string>

using namespace std;

int main() {
    // Optimized cases
    assert(clamp(2, 4, 7) == 4);
    assert(clamp(6, 4, 7) == 6);
    assert(clamp(8, 4, 7) == 7);

    assert(clamp(2, 4, 7, std::less{}) == 4);
    assert(clamp(6, 4, 7, std::less{}) == 6);
    assert(clamp(8, 4, 7, std::less{}) == 7);

    assert(clamp(2, 4, 7, std::less<int>{}) == 4);
    assert(clamp(6, 4, 7, std::less<int>{}) == 6);
    assert(clamp(8, 4, 7, std::less<int>{}) == 7);

    const int data[100] = {};
    assert(clamp(data + 22, data + 30, data + 70) == data + 30);
    assert(clamp(data + 44, data + 30, data + 70) == data + 44);
    assert(clamp(data + 88, data + 30, data + 70) == data + 70);

    // Generic cases
    assert(clamp(2, 4, 7, std::less<long long>{}) == 4);
    assert(clamp(6, 4, 7, std::less<long long>{}) == 6);
    assert(clamp(8, 4, 7, std::less<long long>{}) == 7);

    assert(clamp<string>("apple", "banana", "cococnut") == "banana");
    assert(clamp<string>("blueberry", "banana", "cococnut") == "blueberry");
    assert(clamp<string>("cucumber", "banana", "cococnut") == "cococnut");

#if _ITERATOR_DEBUG_LEVEL < 2
    // make sure we keep early return for generic cases:
    // either less than min or greater than max input should produce only one comparison
    // for _ITERATOR_DEBUG_LEVEL == 2 we validate predicate, so make extra comparisons

    int comparisons        = 0;
    const auto counted_cmp = [&comparisons](auto a, auto b) { return ++comparisons, a < b; };

    assert(clamp(2, 4, 7, counted_cmp) == 4);
    assert(clamp(6, 4, 7, counted_cmp) == 6);
    assert(clamp(8, 4, 7, counted_cmp) == 7);
    assert(comparisons == 5);

    comparisons = 0;
    assert(clamp<string>("apple", "banana", "cococnut", counted_cmp) == "banana");
    assert(clamp<string>("blueberry", "banana", "cococnut", counted_cmp) == "blueberry");
    assert(clamp<string>("cucumber", "banana", "cococnut", counted_cmp) == "cococnut");
    assert(comparisons == 5);
#endif // _ITERATOR_DEBUG_LEVEL < 2
}
