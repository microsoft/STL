// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

// N4140 25.3.11 [alg.rotate]

// template<class ForwardIterator>
// ForwardIterator rotate(ForwardIterator first, ForwardIterator middle, ForwardIterator last);

// -1- Effects: For each non-negative integer i < (last - first), places the element
//     from the position first + i into position:
//                 first + (i + (last - middle)) % (last - first).
// -2- Returns: first + (last - middle).
// -3- Remarks: This is a left rotate.
// -4- Requires: [first, middle) and [middle, last) shall be valid ranges. ForwardIterator shall
//     satisfy the requirements of ValueSwappable(17.6.3.2). The type of *first shall satisfy the
//     requirements of MoveConstructible (Table 20) and the requirements of MoveAssignable (Table 22).
// -5- Complexity: At most last - first swaps.

#include <algorithm>
#include <cassert>
#include <forward_list>
#include <iterator>
#include <list>
#include <numeric>
#include <vector>

using namespace std;

template <typename I>
I canonical_rotate(I first, I mid, I last) { // Triple reverse is guaranteed to be correct and easy to follow.
    reverse(first, mid);
    reverse(mid, last);
    reverse(first, last);

    return first + (last - mid);
}

template <typename T>
void test(size_t pbl_sz) {
    using Value_type = typename T::value_type;
    T container(pbl_sz);

    for (auto i = 0u; i != pbl_sz + 1u; ++i) {
        iota(begin(container), end(container), Value_type(0));
        vector<Value_type> reference(cbegin(container), cend(container));

        const auto canonical =
            canonical_rotate(begin(reference), next(begin(reference), static_cast<ptrdiff_t>(i)), end(reference));
        const auto res = rotate(begin(container), next(begin(container), static_cast<ptrdiff_t>(i)), end(container));

        assert(distance(begin(container), res) == distance(begin(reference), canonical)
               && equal(cbegin(container), cend(container), cbegin(reference)));
    }
}

int main() {
    for (auto sz = 0u; sz != 8u; ++sz) {
        // Test forward iterators.
        test<forward_list<unsigned int>>(sz);

        // Test bidirectional iterators.
        test<list<unsigned int>>(sz);

        // Test random iterators.
        test<vector<unsigned int>>(sz);
    }
}
