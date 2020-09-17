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

enum class CanThrowAtDefaultConstruction : bool { no, yes };

template <CanThrowAtDefaultConstruction CanThrow>
struct int_wrapper {
    inline static int constructions = 0;
    inline static int destructions  = 0;

    static void clear_counts() {
        constructions = 0;
        destructions  = 0;
    }

    static constexpr int magic_throwing_val = 4;
    int val;

    int_wrapper() noexcept(!static_cast<bool>(CanThrow)) {
        ++constructions;
        if constexpr (CanThrow == CanThrowAtDefaultConstruction::yes) {
            if (constructions == magic_throwing_val) {
                throw magic_throwing_val;
            }
        }
    }

    ~int_wrapper() {
        ++destructions;
    }

    auto operator<=>(const int_wrapper&) const = default;
};
STATIC_ASSERT(default_initializable<int_wrapper<CanThrowAtDefaultConstruction::yes>>);

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

template <CanThrowAtDefaultConstruction CanThrow>
struct instantiator {
    template <ranges::forward_range Write>
    static void call() {
        using ranges::uninitialized_default_construct_n, ranges::equal, ranges::equal_to, ranges::iterator_t;
        using wrapper = int_wrapper<CanThrow>;

        { // Validate iterator overload
            holder<wrapper, 3> mem;
            Write wrapped_input{mem.as_span()};

            wrapper::clear_counts();
            const same_as<iterator_t<Write>> auto result = uninitialized_default_construct_n(wrapped_input.begin(), 3);
            assert(wrapper::constructions == 3);
            assert(wrapper::destructions == 0);
            assert(result == wrapped_input.end());
            not_ranges_destroy(wrapped_input);
            assert(wrapper::constructions == 3);
            assert(wrapper::destructions == 3);
        }
    }
};

struct throwing_test {
    template <ranges::forward_range Write>
    static void call() {
        using wrapper = int_wrapper<CanThrowAtDefaultConstruction::yes>;
        holder<wrapper, wrapper::magic_throwing_val> mem;
        Write wrapped_input{mem.as_span()};

        wrapper::clear_counts();
        try {
            (void) ranges::uninitialized_default_construct_n(wrapped_input.begin(), wrapper::magic_throwing_val);
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

template <CanThrowAtDefaultConstruction CanThrow>
using test_range = test::range<test::fwd, int_wrapper<CanThrow>, test::Sized::no, test::CanDifference::no,
    test::Common::no, test::CanCompare::yes, test::ProxyRef::no>;

int main() {
    // The algorithm is oblivious to non-required category, size, difference. It _is_ sensitive to proxyness in that it
    // requires non-proxy references for the input range.

    instantiator<CanThrowAtDefaultConstruction::yes>::call<test_range<CanThrowAtDefaultConstruction::yes>>();
    instantiator<CanThrowAtDefaultConstruction::no>::call<test_range<CanThrowAtDefaultConstruction::no>>();
    throwing_test::call<test_range<CanThrowAtDefaultConstruction::yes>>();
}
