// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#define _CONTAINER_DEBUG_LEVEL 1

#include <cassert>
#include <ranges>
#include <span>

#include <range_algorithm_support.hpp>
#include <test_death.hpp>
using namespace std;
using ranges::chunk_view;

using test_range = test::range<input_iterator_tag, const int, test::Sized::yes, test::CanDifference::yes,
    test::Common::yes, test::CanCompare::yes, test::ProxyRef::no>;

static constexpr int some_ints[] = {0, 1, 2, 3};

void test_view_negative_size_forward_range() {
    auto r                  = span<const int>{some_ints};
    [[maybe_unused]] auto v = chunk_view(r, -1); // chunk size must be greater than 0
}

void test_view_negative_size_input_range() {
    auto r                  = test_range{some_ints};
    [[maybe_unused]] auto v = chunk_view(r, -1); // chunk size must be greater than 0
}

void test_outer_iterator_preincrement_past_end() {
    auto r  = test_range{some_ints};
    auto v  = chunk_view(r, 4);
    auto it = v.begin();
    ++it;
    ++it; // cannot increment chunk_view end iterator
}

void test_outer_iterator_postincrement_past_end() {
    auto r  = test_range{some_ints};
    auto v  = chunk_view(r, 4);
    auto it = v.begin();
    it++;
    it++; // cannot increment chunk_view end iterator
}

void test_inner_iterator_preincrement_past_end() {
    auto r  = test_range{some_ints};
    auto v  = chunk_view(r, 1);
    auto it = (*v.begin()).begin();
    ++it;
    ++it; // cannot increment chunk_view end iterator
}

void test_inner_iterator_postincrement_past_end() {
    auto r  = test_range{some_ints};
    auto v  = chunk_view(r, 1);
    auto it = (*v.begin()).begin();
    it++;
    it++; // cannot increment chunk_view end iterator
}

void test_outer_iterator_dereference_at_end() {
    auto r  = test_range{some_ints};
    auto v  = chunk_view(r, 4);
    auto it = v.begin();
    ++it;
    (void) *it; // cannot dereference chunk_view end iterator
}

void test_inner_iterator_dereference_at_end() {
    auto r  = test_range{some_ints};
    auto v  = chunk_view(r, 1);
    auto it = (*v.begin()).begin();
    it++;
    (void) *it; // cannot dereference chunk_view end iterator
}

void test_fwd_iterator_advance_past_end() {
    auto v  = chunk_view{span{some_ints}, 2};
    auto it = v.begin();
    it += 5; // cannot advance chunk_view iterator past end
}

void test_fwd_iterator_advance_past_end_with_integer_overflow() {
    auto v  = chunk_view{span{some_ints}, 2};
    auto it = v.begin();
    it += (numeric_limits<ptrdiff_t>::max)() / 2; // cannot advance chunk_view iterator past end (integer overflow)
}

void test_fwd_iterator_advance_negative_min() {
    auto v  = chunk_view{span{some_ints}, 2};
    auto it = v.begin();
    it -= (numeric_limits<ptrdiff_t>::min)(); // cannot advance chunk_view iterator past end (integer overflow)
}

int main(int argc, char* argv[]) {
    std_testing::death_test_executive exec;

#if _ITERATOR_DEBUG_LEVEL != 0
    exec.add_death_tests({
        test_view_negative_size_forward_range,
        test_view_negative_size_input_range,
        test_outer_iterator_preincrement_past_end,
        test_outer_iterator_postincrement_past_end,
        test_inner_iterator_preincrement_past_end,
        test_inner_iterator_postincrement_past_end,
        test_outer_iterator_dereference_at_end,
        test_inner_iterator_dereference_at_end,
        test_fwd_iterator_advance_past_end,
        test_fwd_iterator_advance_past_end_with_integer_overflow,
        test_fwd_iterator_advance_negative_min,
    });
#else // ^^^ test everything / test only _CONTAINER_DEBUG_LEVEL cases vvv
    exec.add_death_tests({
        test_view_negative_size_forward_range,
        test_view_negative_size_input_range,
    });
#endif // _ITERATOR_DEBUG_LEVEL != 0

    return exec.run(argc, argv);
}
