// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <algorithm>
#include <cassert>

using std::max;
using std::min;
#if _HAS_CXX17
using std::clamp;
#endif // _HAS_CXX17

#define STATIC_ASSERT(...) static_assert(__VA_ARGS__, #__VA_ARGS__)

template <bool IsNoexcept>
struct my_int {
    bool operator<(const my_int& rhs) const noexcept(IsNoexcept) {
        return m_val < rhs.m_val;
    }
    int m_val;
};

template <typename T>
void test_noexcept_predicates(const T& val) {
    auto noexcept_pred = [](const T&, const T&) noexcept { return false; };
    auto throwing_pred = [](const T&, const T&) noexcept(false) { return false; };

    STATIC_ASSERT(noexcept(min(val, val, noexcept_pred)));
    STATIC_ASSERT(noexcept(max(val, val, noexcept_pred)));
    STATIC_ASSERT(!noexcept(min(val, val, throwing_pred)));
    STATIC_ASSERT(!noexcept(max(val, val, throwing_pred)));

    (void) val;
}

// Test note:
// std::min/max inheriting noexcept status from operator< or a predicate is not standard behavior.
// Instead, this was requested to help teams using OACR.
// Previous to this change, std::min and std::max would always trigger OACR warnings
// that std::min/max are throwing operations, even when using types that would not.
void test_noexcept() {
    my_int<false> throwing_val;
    my_int<true> noexcept_val;

    STATIC_ASSERT(noexcept(min(4, 5)));
    STATIC_ASSERT(noexcept(max(4, 5)));

    STATIC_ASSERT(!noexcept(min(throwing_val, throwing_val)));
    STATIC_ASSERT(!noexcept(max(throwing_val, throwing_val)));
    test_noexcept_predicates(throwing_val);

    STATIC_ASSERT(noexcept(min(noexcept_val, noexcept_val)));
    STATIC_ASSERT(noexcept(max(noexcept_val, noexcept_val)));
    test_noexcept_predicates(noexcept_val);
}

#if _HAS_CXX17
// Also test P0025R0 "Remarks: Returns the first argument when it is equivalent to one of the boundary arguments."
void test_clamp_returned_argument() {
    struct {
        int val;
        int lower;
        int upper;
    } vals = {42, 42, 42};
    assert(&clamp(vals.val, vals.lower, vals.upper) == &vals.val);

    vals = {42, 42, 128};
    assert(&clamp(vals.val, vals.lower, vals.upper) == &vals.val);

    vals = {128, 42, 128};
    assert(&clamp(vals.val, vals.lower, vals.upper) == &vals.val);
}
#endif // _HAS_CXX17

int main() {
    // test_noexcept is compile-only

#if _HAS_CXX17
    test_clamp_returned_argument();
#endif // _HAS_CXX17
}
