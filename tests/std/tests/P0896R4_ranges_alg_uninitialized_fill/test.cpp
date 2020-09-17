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

// Validate dangling story
STATIC_ASSERT(same_as<decltype(ranges::uninitialized_fill(borrowed<false>{}, 42)), ranges::dangling>);

enum class CanThrowAtConstruction : bool { no, yes };

template <CanThrowAtConstruction CanThrow>
struct int_wrapper {
    inline static int constructions = 0;
    inline static int destructions  = 0;

    static void clear_counts() {
        constructions = 0;
        destructions  = 0;
    }

    static constexpr int magic_throwing_val = 4;
    int val;

    int_wrapper() = default;

    int_wrapper(const int v) noexcept(!static_cast<bool>(CanThrow)) {
        ++constructions;
        if constexpr (CanThrow == CanThrowAtConstruction::yes) {
            if (constructions == magic_throwing_val) {
                throw magic_throwing_val;
            }
        }
        val = v;
    }

    ~int_wrapper() {
        ++destructions;
    }

    auto operator<=>(const int_wrapper&) const = default;
};
STATIC_ASSERT(default_initializable<int_wrapper<CanThrowAtConstruction::yes>>);

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

template <CanThrowAtConstruction CanThrow>
struct instantiator {
    static constexpr int expected[3] = {42, 42, 42};

    template <ranges::forward_range Write>
    static void call() {
        using ranges::uninitialized_fill, ranges::equal, ranges::equal_to, ranges::iterator_t;
        using wrapper = int_wrapper<CanThrow>;

        { // Validate range overload
            holder<wrapper, 3> mem;
            Write wrapped_input{mem.as_span()};

            wrapper::clear_counts();
            const same_as<iterator_t<Write>> auto result = uninitialized_fill(wrapped_input, 42);
            assert(wrapper::constructions == 3);
            assert(wrapper::destructions == 0);
            assert(result == wrapped_input.end());
            assert(equal(wrapped_input, expected, equal_to{}, &wrapper::val));
            not_ranges_destroy(wrapped_input);
            assert(wrapper::constructions == 3);
            assert(wrapper::destructions == 3);
        }

        { // Validate iterator overload
            holder<wrapper, 3> mem;
            Write wrapped_input{mem.as_span()};

            wrapper::clear_counts();
            const same_as<iterator_t<Write>> auto result =
                uninitialized_fill(wrapped_input.begin(), wrapped_input.end(), 42);
            assert(wrapper::constructions == 3);
            assert(wrapper::destructions == 0);
            assert(result == wrapped_input.end());
            assert(equal(wrapped_input, expected, equal_to{}, &wrapper::val));
            not_ranges_destroy(wrapped_input);
            assert(wrapper::constructions == 3);
            assert(wrapper::destructions == 3);
        }
    }
};

struct throwing_test {
    template <ranges::forward_range Write>
    static void call() {
        // Validate only range overload (one is plenty since they both use the same backend)
        using wrapper = int_wrapper<CanThrowAtConstruction::yes>;
        holder<wrapper, wrapper::magic_throwing_val> mem;
        Write wrapped_input{mem.as_span()};

        wrapper::clear_counts();
        try {
            (void) ranges::uninitialized_fill(wrapped_input, 42);
            assert(false);
        } catch (int i) {
            assert(i == wrapper::magic_throwing_val);
        } catch (...) {
            assert(false);
        }
        assert(wrapper::constructions == wrapper::magic_throwing_val);
        assert(wrapper::destructions == wrapper::magic_throwing_val - 1);
    }
};

template <CanThrowAtConstruction CanThrow>
using test_range = test::range<test::fwd, int_wrapper<CanThrow>, test::Sized::no, test::CanDifference::no,
    test::Common::no, test::CanCompare::yes, test::ProxyRef::no>;

int main() {
    // The algorithm is oblivious to non-required category, size, difference. It _is_ sensitive to proxyness in that it
    // requires non-proxy references for the input range.

    instantiator<CanThrowAtConstruction::yes>::call<test_range<CanThrowAtConstruction::yes>>();
    instantiator<CanThrowAtConstruction::no>::call<test_range<CanThrowAtConstruction::no>>();
    throwing_test::call<test_range<CanThrowAtConstruction::yes>>();
}
