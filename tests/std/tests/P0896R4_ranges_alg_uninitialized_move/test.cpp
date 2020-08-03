// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <algorithm>
#include <cassert>
#include <concepts>
#include <memory>
#include <ranges>
#include <span>
#include <utility>

#include <range_algorithm_support.hpp>

using namespace std;

#ifndef _CONSTEXPR20_DYNALLOC // TRANSITION, P0784
#define _CONSTEXPR20_DYNALLOC
#endif

// Validate that uninitialized_move_result aliases in_out_result
STATIC_ASSERT(same_as<ranges::uninitialized_move_result<int, double>, ranges::in_out_result<int, double>>);

// Validate dangling story
STATIC_ASSERT(same_as<decltype(ranges::uninitialized_move(borrowed<false>{}, borrowed<false>{})),
    ranges::uninitialized_move_result<ranges::dangling, ranges::dangling>>);
STATIC_ASSERT(same_as<decltype(ranges::uninitialized_move(borrowed<false>{}, borrowed<true>{})),
    ranges::uninitialized_move_result<ranges::dangling, int*>>);
STATIC_ASSERT(same_as<decltype(ranges::uninitialized_move(borrowed<true>{}, borrowed<false>{})),
    ranges::uninitialized_move_result<int*, ranges::dangling>>);
STATIC_ASSERT(same_as<decltype(ranges::uninitialized_move(borrowed<true>{}, borrowed<true>{})),
    ranges::uninitialized_move_result<int*, int*>>);

struct int_wrapper {
    inline static int constructions = 0;
    inline static int destructions  = 0;

    static constexpr void clear_counts() {
        if (!is_constant_evaluated()) {
            constructions = destructions = 0;
        }
    }

    static constexpr int magic_throwing_val = 29;
    int val                                 = 10;

    constexpr int_wrapper() {
        if (!is_constant_evaluated()) {
            ++constructions;
        }
    }
    constexpr int_wrapper(int x) : val{x} {
        if (!is_constant_evaluated()) {
            ++constructions;
        }
    }
    constexpr int_wrapper(int_wrapper&& that) {
        if (that.val == magic_throwing_val) {
            throw magic_throwing_val;
        }

        val = exchange(that.val, -1);
        if (!is_constant_evaluated()) {
            ++constructions;
        }
    }

    _CONSTEXPR20_DYNALLOC ~int_wrapper() {
        if (!is_constant_evaluated()) {
            ++destructions;
        }
    }

    constexpr int_wrapper& operator=(int_wrapper&& that) {
        if (that.val == magic_throwing_val) {
            throw magic_throwing_val;
        }
        val = exchange(that.val, -1);
        return *this;
    }

    auto operator<=>(const int_wrapper&) const = default;
};
STATIC_ASSERT(movable<int_wrapper> && !copyable<int_wrapper>);

template <class T, size_t N>
struct holder {
    T* ptr = allocator<T>{}.allocate(N);

    holder()              = default;
    holder(const holder&) = delete;
    holder& operator=(const holder&) = delete;

    _CONSTEXPR20_DYNALLOC ~holder() {
        allocator<T>{}.deallocate(ptr, N);
    }

    constexpr auto as_span() const noexcept {
        return span<T, N>{ptr, N};
    }
};

struct instantiator {
    static constexpr int expected_output[] = {13, 55, 12345};
    static constexpr int expected_input[]  = {-1, -1, -1};

    template <ranges::input_range R, ranges::forward_range W>
    static _CONSTEXPR20_DYNALLOC void call() {
        using ranges::uninitialized_move, ranges::uninitialized_move_result, ranges::equal, ranges::iterator_t;

        { // Validate range overload
            int_wrapper input[3] = {13, 55, 12345};
            int_wrapper::clear_counts();
            R wrapped_input{input};
            holder<int_wrapper, 3> mem;
            W wrapped_output{mem.as_span()};

            const same_as<uninitialized_move_result<iterator_t<R>, iterator_t<W>>> auto result =
                uninitialized_move(wrapped_input, wrapped_output);
            if (!is_constant_evaluated()) {
                assert(int_wrapper::constructions == 3);
                assert(int_wrapper::destructions == 0);
            }
            assert(result.in == wrapped_input.end());
            assert(result.out == wrapped_output.end());
            assert(equal(wrapped_output, expected_output, ranges::equal_to{}, &int_wrapper::val));
            assert(equal(input, expected_input, ranges::equal_to{}, &int_wrapper::val));
            ranges::destroy(wrapped_output);
            if (!is_constant_evaluated()) {
                assert(int_wrapper::constructions == 3);
                assert(int_wrapper::destructions == 3);
            }
        }

        { // Validate iterator overload
            int_wrapper input[3] = {13, 55, 12345};
            int_wrapper::clear_counts();
            R wrapped_input{input};
            holder<int_wrapper, 3> mem;
            W wrapped_output{mem.as_span()};

            const same_as<uninitialized_move_result<iterator_t<R>, iterator_t<W>>> auto result = uninitialized_move(
                wrapped_input.begin(), wrapped_input.end(), wrapped_output.begin(), wrapped_output.end());
            if (!is_constant_evaluated()) {
                assert(int_wrapper::constructions == 3);
                assert(int_wrapper::destructions == 0);
            }
            assert(result.in == wrapped_input.end());
            assert(result.out == wrapped_output.end());
            assert(equal(wrapped_output, expected_output, ranges::equal_to{}, &int_wrapper::val));
            assert(equal(input, expected_input, ranges::equal_to{}, &int_wrapper::val));
            ranges::destroy(wrapped_output);
            if (!is_constant_evaluated()) {
                assert(int_wrapper::constructions == 3);
                assert(int_wrapper::destructions == 3);
            }
        }
    }
};

struct throwing_test {
    static constexpr int expected_input[] = {-1, -1, int_wrapper::magic_throwing_val, 12345};

    template <ranges::input_range R, ranges::forward_range W>
    static void call() {
        using ranges::uninitialized_move, ranges::uninitialized_move_result, ranges::equal, ranges::iterator_t;
        // Validate only range overload (one is plenty since they are backends)
        int_wrapper input[] = {13, 55, int_wrapper::magic_throwing_val, 12345};
        int_wrapper::clear_counts();
        R wrapped_input{input};
        holder<int_wrapper, 4> mem;
        W wrapped_output{mem.as_span()};

        try {
            (void) uninitialized_move(wrapped_input, wrapped_output);
            assert(false);
        } catch (int i) {
            assert(i == int_wrapper::magic_throwing_val);
        } catch (...) {
            assert(false);
        }
        assert(int_wrapper::constructions == 2);
        assert(int_wrapper::destructions == 2);
        assert(equal(input, expected_input, ranges::equal_to{}, &int_wrapper::val));
    }
};

template <test::ProxyRef IsProxy>
using test_input  = test::range<test::input, int_wrapper, test::Sized::no, test::CanDifference::no, test::Common::no,
    test::CanCompare::yes, IsProxy>;
using test_output = test::range<test::fwd, int_wrapper, test::Sized::no, test::CanDifference::no, test::Common::no,
    test::CanCompare::yes, test::ProxyRef::no>;

constexpr void run_tests() {
    // The algorithm is oblivious to non-required category, size, difference, and requires non-proxy references for the
    // output range.

    if (!is_constant_evaluated()) { // TRANSITION, P0784
        instantiator::call<test_input<test::ProxyRef::no>, test_output>();
        instantiator::call<test_input<test::ProxyRef::yes>, test_output>();
    }

    if (!is_constant_evaluated()) {
        throwing_test::call<test_input<test::ProxyRef::no>, test_output>();
        throwing_test::call<test_input<test::ProxyRef::yes>, test_output>();
    }
}

int main() {
    STATIC_ASSERT((run_tests(), true));
    run_tests();
}
