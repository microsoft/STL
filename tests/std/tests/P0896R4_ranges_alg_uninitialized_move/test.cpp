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

enum class CanThrowAtMoveConstruction : bool { no, yes };

template <CanThrowAtMoveConstruction CanThrow = CanThrowAtMoveConstruction::yes>
struct int_wrapper {
    inline static int constructions = 0;
    inline static int destructions  = 0;

    static void clear_counts() {
        constructions = 0;
        destructions  = 0;
    }

    static constexpr int magic_throwing_val = 29;
    int val                                 = 10;

    int_wrapper() {
        ++constructions;
    }
    int_wrapper(int x) : val{x} {
        ++constructions;
    }

    int_wrapper(int_wrapper&& that) noexcept(!static_cast<bool>(CanThrow)) {
        if constexpr (CanThrow == CanThrowAtMoveConstruction::yes) {
            if (that.val == magic_throwing_val) {
                throw magic_throwing_val;
            }
        }

        val = exchange(that.val, -1);
        ++constructions;
    }

    ~int_wrapper() {
        ++destructions;
    }

    int_wrapper& operator=(int_wrapper&&) {
        // Shall never be used as we construct in place
        throw magic_throwing_val + 1;
    }

    auto operator<=>(const int_wrapper&) const = default;
};
STATIC_ASSERT(
    movable<int_wrapper<CanThrowAtMoveConstruction::yes>> && !copyable<int_wrapper<CanThrowAtMoveConstruction::yes>>);

template <class T, size_t N>
struct holder {
    STATIC_ASSERT(N < ~size_t{0} / sizeof(T));
    alignas(T) unsigned char space[N * sizeof(T)];

    auto as_span() {
        return span<T, N>{reinterpret_cast<T*>(space + 0), N};
    }
};

template <class R>
void not_ranges_destroy(R&& r) { // TRANSITION, ranges::destroy
    for (auto& e : r) {
        destroy_at(&e);
    }
}

template <CanThrowAtMoveConstruction CanThrow>
struct instantiator {
    static constexpr int expected_output[] = {13, 55, 12345};
    static constexpr int expected_input[]  = {-1, -1, -1};

    template <ranges::input_range R, ranges::forward_range W>
    static void call() {
        using ranges::uninitialized_move, ranges::uninitialized_move_result, ranges::equal, ranges::equal_to,
            ranges::iterator_t;
        using wrapper = int_wrapper<CanThrow>;

        { // Validate range overload
            wrapper input[3] = {13, 55, 12345};
            R wrapped_input{input};
            holder<wrapper, 3> mem;
            W wrapped_output{mem.as_span()};

            wrapper::clear_counts();
            const same_as<uninitialized_move_result<iterator_t<R>, iterator_t<W>>> auto result =
                uninitialized_move(wrapped_input, wrapped_output);
            assert(wrapper::constructions == 3);
            assert(wrapper::destructions == 0);
            assert(result.in == wrapped_input.end());
            assert(result.out == wrapped_output.end());
            assert(equal(wrapped_output, expected_output, equal_to{}, &wrapper::val));
            assert(equal(input, expected_input, equal_to{}, &wrapper::val));
            not_ranges_destroy(wrapped_output);
            assert(wrapper::constructions == 3);
            assert(wrapper::destructions == 3);
        }

        { // Validate iterator overload
            wrapper input[3] = {13, 55, 12345};
            R wrapped_input{input};
            holder<wrapper, 3> mem;
            W wrapped_output{mem.as_span()};

            wrapper::clear_counts();
            const same_as<uninitialized_move_result<iterator_t<R>, iterator_t<W>>> auto result = uninitialized_move(
                wrapped_input.begin(), wrapped_input.end(), wrapped_output.begin(), wrapped_output.end());
            assert(wrapper::constructions == 3);
            assert(wrapper::destructions == 0);
            assert(result.in == wrapped_input.end());
            assert(result.out == wrapped_output.end());
            assert(equal(wrapped_output, expected_output, equal_to{}, &wrapper::val));
            assert(equal(input, expected_input, equal_to{}, &wrapper::val));
            not_ranges_destroy(wrapped_output);
            assert(wrapper::constructions == 3);
            assert(wrapper::destructions == 3);
        }
    }
};

struct throwing_test {
    using wrapper                         = int_wrapper<CanThrowAtMoveConstruction::yes>;
    static constexpr int expected_input[] = {-1, -1, wrapper::magic_throwing_val, 12345};

    template <ranges::input_range R, ranges::forward_range W>
    static void call() {
        // Validate only range overload (one is plenty since they both use the same backend)
        wrapper input[] = {13, 55, wrapper::magic_throwing_val, 12345};
        R wrapped_input{input};
        holder<wrapper, 4> mem;
        W wrapped_output{mem.as_span()};

        wrapper::clear_counts();
        try {
            (void) ranges::uninitialized_move(wrapped_input, wrapped_output);
            assert(false);
        } catch (int i) {
            assert(i == wrapper::magic_throwing_val);
        } catch (...) {
            assert(false);
        }
        assert(wrapper::constructions == 2);
        assert(wrapper::destructions == 2);
        assert(ranges::equal(input, expected_input, ranges::equal_to{}, &wrapper::val));
    }
};

template <test::ProxyRef IsProxy, CanThrowAtMoveConstruction CanThrow>
using test_input = test::range<test::input, int_wrapper<CanThrow>, test::Sized::no, test::CanDifference::no,
    test::Common::no, test::CanCompare::yes, IsProxy>;
template <CanThrowAtMoveConstruction CanThrow>
using test_output = test::range<test::fwd, int_wrapper<CanThrow>, test::Sized::no, test::CanDifference::no,
    test::Common::no, test::CanCompare::yes, test::ProxyRef::no>;
int main() {
    // The algorithm is oblivious to non-required category, size, difference, and "proxyness" of the input range. It
    // _is_ sensitive to proxyness in that it requires non-proxy references for the output range.

    instantiator<CanThrowAtMoveConstruction::yes>::call<test_input<test::ProxyRef::no, CanThrowAtMoveConstruction::yes>,
        test_output<CanThrowAtMoveConstruction::yes>>();
    instantiator<CanThrowAtMoveConstruction::yes>::call<
        test_input<test::ProxyRef::yes, CanThrowAtMoveConstruction::yes>,
        test_output<CanThrowAtMoveConstruction::yes>>();
    instantiator<CanThrowAtMoveConstruction::no>::call<test_input<test::ProxyRef::no, CanThrowAtMoveConstruction::no>,
        test_output<CanThrowAtMoveConstruction::no>>();
    instantiator<CanThrowAtMoveConstruction::no>::call<test_input<test::ProxyRef::yes, CanThrowAtMoveConstruction::no>,
        test_output<CanThrowAtMoveConstruction::no>>();

    throwing_test::call<test_input<test::ProxyRef::no, CanThrowAtMoveConstruction::yes>,
        test_output<CanThrowAtMoveConstruction::yes>>();
    throwing_test::call<test_input<test::ProxyRef::yes, CanThrowAtMoveConstruction::yes>,
        test_output<CanThrowAtMoveConstruction::yes>>();
}
