// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

//
// Tests the new functions added as part of P0586R2, "Integer Comparison Functions"
//

#include <assert.h>
#include <limits>
#include <utility>

using namespace std;

constexpr bool test() {
    assert(cmp_equal(1L, 1));
    assert(!cmp_equal(1, -1));
    assert(!cmp_equal(1, 42));
    assert(!cmp_equal(42, 1));

    assert(!cmp_not_equal(1L, 1));
    assert(cmp_not_equal(1, -1));
    assert(cmp_not_equal(1, 42));
    assert(cmp_not_equal(42, 1));

    assert(!cmp_less(1L, 1));
    assert(!cmp_less(1, -1));
    assert(cmp_less(1, 42));
    assert(!cmp_less(42, 1));

    assert(cmp_less_equal(1L, 1));
    assert(!cmp_less_equal(1, -1));
    assert(cmp_less_equal(1, 42));
    assert(!cmp_less_equal(42, 1));

    assert(!cmp_greater(1L, 1));
    assert(cmp_greater(1, -1));
    assert(!cmp_greater(1, 42));
    assert(cmp_greater(42, 1));

    assert(cmp_greater_equal(1L, 1));
    assert(cmp_greater_equal(1, -1));
    assert(!cmp_greater_equal(1, 42));
    assert(cmp_greater_equal(42, 1));

    assert(in_range<int>(1));
    assert(in_range<int>(1L));
    assert(!in_range<unsigned int>(-1));
    assert(in_range<unsigned int>(0));
    assert(in_range<unsigned int>(42));
    assert(!in_range<int>(numeric_limits<long long>::max()));
    assert(!in_range<int>(numeric_limits<long long>::min()));
    assert(in_range<int>(numeric_limits<int>::max()));
    assert(in_range<int>(numeric_limits<int>::min()));
    assert(in_range<long>(numeric_limits<int>::max()));
    assert(in_range<long>(numeric_limits<int>::min()));

    { // Different signs
        const int i          = -1;
        const unsigned int u = 1;
        assert(!cmp_equal(i, u));
        assert(cmp_not_equal(i, u));
        assert(cmp_less(i, u));
        assert(!cmp_less(u, i));
        assert(cmp_less_equal(i, u));
        assert(!cmp_less_equal(u, i));
        assert(cmp_greater(u, i));
        assert(!cmp_greater(i, u));
        assert(cmp_greater_equal(u, i));
        assert(!cmp_greater_equal(i, u));
    }
    { // Equal
        const int i          = 7;
        const unsigned int u = 7;
        assert(cmp_equal(i, u));
        assert(!cmp_not_equal(i, u));
        assert(!cmp_less(i, u));
        assert(!cmp_less(u, i));
        assert(cmp_less_equal(i, u));
        assert(cmp_less_equal(u, i));
        assert(!cmp_greater(u, i));
        assert(!cmp_greater(i, u));
        assert(cmp_greater_equal(u, i));
        assert(cmp_greater_equal(i, u));
    }
    { // Not equal
        const int i          = 42;
        const unsigned int u = 7;
        assert(!cmp_equal(i, u));
        assert(cmp_not_equal(i, u));
        assert(!cmp_less(i, u));
        assert(cmp_less(u, i));
        assert(!cmp_less_equal(i, u));
        assert(cmp_less_equal(u, i));
        assert(!cmp_greater(u, i));
        assert(cmp_greater(i, u));
        assert(!cmp_greater_equal(u, i));
        assert(cmp_greater_equal(i, u));
    }

    return true;
}

int main() {
    assert(test());
    static_assert(test());
}
