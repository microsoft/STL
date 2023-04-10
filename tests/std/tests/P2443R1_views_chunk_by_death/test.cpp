// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#define _CONTAINER_DEBUG_LEVEL 1

#include <cassert>
#include <cstddef>
#include <ranges>
#include <vector>

#include <test_death.hpp>
using namespace std;

static int some_ints[] = {0, 1, 2, 3};

struct test_predicate {
    struct tag {};

    test_predicate() = default;
    test_predicate(const test_predicate&) {
        throw tag{};
    }
    test_predicate& operator=(const test_predicate&) = delete;

    constexpr bool operator()(int i, int j) const {
        return i == j;
    }
};

auto with_no_predicate() {
    using V = decltype(ranges::chunk_by_view{some_ints, test_predicate{}});
    V r{some_ints, {}};
    try {
        r = V{some_ints, {}};
    } catch (const test_predicate::tag&) {
    }
    return r;
}

void test_view_predicate() {
    auto r = with_no_predicate();
    (void) r.pred(); // chunk_by_view has no predicate
}

void test_view_begin() {
    auto r = with_no_predicate();
    (void) r.begin(); // cannot call begin on a chunk_by_view that holds no predicate
}

constexpr auto lambda = [](int, int) { return true; };
using V               = decltype(ranges::chunk_by_view{some_ints, lambda});

void test_operator_star_end_iterator() {
    V r{some_ints, lambda};
    ranges::iterator_t<V> i = ranges::next(r.begin(), r.end());
    (void) (*i); // cannot dereference chunk_by_view end iterator
}

void test_operator_preincrement_after_end() {
    V r{some_ints, lambda};
    ranges::iterator_t<V> i = ranges::next(r.begin(), r.end());
    ++i; // cannot increment chunk_by_view iterator past end
}

void test_operator_postincrement_after_end() {
    V r{some_ints, lambda};
    ranges::iterator_t<V> i = ranges::next(r.begin(), r.end());
    i++; // cannot increment chunk_by_view iterator past end
}

void test_operator_predecrement_value_initialized_iterator() {
    ranges::iterator_t<V> i{};
    --i; // cannot decrement value-initialized chunk_by_view iterator
}

void test_operator_predecrement_before_begin() {
    V r{some_ints, lambda};
    ranges::iterator_t<V> i = r.begin();
    --i; // cannot decrement chunk_by_view iterator before begin
}

void test_operator_postdecrement_value_initialized_iterator() {
    ranges::iterator_t<V> i{};
    i--; // cannot decrement value-initialized chunk_by_view iterator
}

void test_operator_postdecrement_before_begin() {
    V r{some_ints, lambda};
    ranges::iterator_t<V> i = r.begin();
    i--; // cannot decrement chunk_by_view iterator before begin
}

int main(int argc, char* argv[]) {
    std_testing::death_test_executive exec;

#if _ITERATOR_DEBUG_LEVEL != 0
    exec.add_death_tests({
        test_view_predicate,
        test_view_begin,

        test_operator_star_end_iterator,
        test_operator_preincrement_after_end,
        test_operator_postincrement_after_end,
        test_operator_predecrement_value_initialized_iterator,
        test_operator_predecrement_before_begin,
        test_operator_postdecrement_value_initialized_iterator,
        test_operator_postdecrement_before_begin,
    });
#else // ^^^ test everything / test only _CONTAINER_DEBUG_LEVEL cases vvv
    exec.add_death_tests({
        test_view_predicate,
        test_view_begin,
    });
#endif // _ITERATOR_DEBUG_LEVEL != 0

    return exec.run(argc, argv);
}
