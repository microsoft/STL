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

// Validate that uninitialized_copy_n_result aliases in_out_result
STATIC_ASSERT(same_as<ranges::uninitialized_copy_n_result<int, double>, ranges::in_out_result<int, double>>);

enum class CanThrowAtCopyConstruction : bool { no, yes };

template <CanThrowAtCopyConstruction CanThrow>
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

    int_wrapper(const int_wrapper& that) noexcept(!static_cast<bool>(CanThrow)) {
        if constexpr (CanThrow == CanThrowAtCopyConstruction::yes) {
            if (that.val == magic_throwing_val) {
                throw magic_throwing_val;
            }
        }

        val = that.val;
        ++constructions;
    }

    ~int_wrapper() {
        ++destructions;
    }

    int_wrapper& operator=(const int_wrapper&) {
        // Shall never be used as we construct in place
        throw magic_throwing_val + 1;
    }

    auto operator<=>(const int_wrapper&) const = default;
};

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

template <CanThrowAtCopyConstruction CanThrow>
struct instantiator {
    static constexpr int expected_output[] = {13, 55, 12345};
    static constexpr int expected_input[]  = {13, 55, 12345};

    template <ranges::input_range Read, ranges::forward_range Write>
    static void call() {
        using ranges::uninitialized_copy_n, ranges::uninitialized_copy_n_result, ranges::equal, ranges::equal_to,
            ranges::iterator_t;
        using wrapper = int_wrapper<CanThrow>;

        { // Validate iterator overload
            wrapper input[3] = {13, 55, 12345};
            Read wrapped_input{input};
            holder<wrapper, 3> mem;
            Write wrapped_output{mem.as_span()};

            wrapper::clear_counts();
            const same_as<uninitialized_copy_n_result<iterator_t<Read>, iterator_t<Write>>> auto result =
                uninitialized_copy_n(wrapped_input.begin(), 3, wrapped_output.begin(), wrapped_output.end());
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
    using wrapper                         = int_wrapper<CanThrowAtCopyConstruction::yes>;
    static constexpr int expected_input[] = {13, 55, wrapper::magic_throwing_val, 12345};

    template <ranges::input_range Read, ranges::forward_range Write>
    static void call() {
        wrapper input[] = {13, 55, wrapper::magic_throwing_val, 12345};
        Read wrapped_input{input};
        holder<wrapper, 4> mem;
        Write wrapped_output{mem.as_span()};

        wrapper::clear_counts();
        try {
            (void) ranges::uninitialized_copy_n(wrapped_input.begin(), 4, wrapped_output.begin(), wrapped_output.end());
            assert(false);
        } catch (int i) {
            assert(i == wrapper::magic_throwing_val);
        } catch (...) {
            assert(false);
        }
        assert(wrapper::constructions == 2);
        assert(wrapper::destructions == 2);
    }
};

template <test::ProxyRef IsProxy, CanThrowAtCopyConstruction CanThrow>
using test_input = test::range<test::input, int_wrapper<CanThrow>, test::Sized::no, test::CanDifference::no,
    test::Common::no, test::CanCompare::yes, IsProxy>;
template <CanThrowAtCopyConstruction CanThrow>
using test_output = test::range<test::fwd, int_wrapper<CanThrow>, test::Sized::no, test::CanDifference::no,
    test::Common::no, test::CanCompare::yes, test::ProxyRef::no>;

int main() {
    // The algorithm is oblivious to non-required category, size, difference. It _is_ sensitive to proxyness in that it
    // requires non-proxy references for the input range.

    instantiator<CanThrowAtCopyConstruction::yes>::call<test_input<test::ProxyRef::no, CanThrowAtCopyConstruction::yes>,
        test_output<CanThrowAtCopyConstruction::yes>>();
    instantiator<CanThrowAtCopyConstruction::yes>::call<
        test_input<test::ProxyRef::yes, CanThrowAtCopyConstruction::yes>,
        test_output<CanThrowAtCopyConstruction::yes>>();
    instantiator<CanThrowAtCopyConstruction::no>::call<test_input<test::ProxyRef::no, CanThrowAtCopyConstruction::no>,
        test_output<CanThrowAtCopyConstruction::no>>();
    instantiator<CanThrowAtCopyConstruction::no>::call<test_input<test::ProxyRef::yes, CanThrowAtCopyConstruction::no>,
        test_output<CanThrowAtCopyConstruction::no>>();

    throwing_test::call<test_input<test::ProxyRef::no, CanThrowAtCopyConstruction::yes>,
        test_output<CanThrowAtCopyConstruction::yes>>();
    throwing_test::call<test_input<test::ProxyRef::yes, CanThrowAtCopyConstruction::yes>,
        test_output<CanThrowAtCopyConstruction::yes>>();
}
