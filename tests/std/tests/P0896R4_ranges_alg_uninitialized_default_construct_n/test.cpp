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

struct int_wrapper {
    inline static int constructions = 0;
    inline static int destructions  = 0;

    static void clear_counts() {
        constructions = 0;
        destructions  = 0;
    }

    static constexpr int magic_throwing_val = 4;
    int val;

    int_wrapper() {
        if (++constructions == magic_throwing_val) {
            throw magic_throwing_val;
        }
    }

    ~int_wrapper() {
        ++destructions;
    }

    auto operator<=>(const int_wrapper&) const = default;
};
STATIC_ASSERT(default_initializable<int_wrapper>);

template <class T, size_t N>
struct holder {
    STATIC_ASSERT(N < ~size_t{0} / sizeof(T));
    alignas(T) unsigned char space[N * sizeof(T)];

    auto as_span() {
        return span<T, N>{reinterpret_cast<T*>(space + 0), N};
    }
};

struct instantiator {
    template <ranges::forward_range Write>
    static void call() {
        using ranges::uninitialized_default_construct_n, ranges::destroy, ranges::equal, ranges::equal_to,
            ranges::iterator_t;

        holder<int_wrapper, 3> mem;
        Write wrapped_input{mem.as_span()};

        int_wrapper::clear_counts();
        const same_as<iterator_t<Write>> auto result = uninitialized_default_construct_n(wrapped_input.begin(), 3);
        assert(int_wrapper::constructions == 3);
        assert(int_wrapper::destructions == 0);
        assert(result == wrapped_input.end());
        destroy(wrapped_input);
        assert(int_wrapper::constructions == 3);
        assert(int_wrapper::destructions == 3);
    }
};

struct throwing_test {
    template <ranges::forward_range Write>
    static void call() {
        holder<int_wrapper, int_wrapper::magic_throwing_val> mem;
        Write wrapped_input{mem.as_span()};

        int_wrapper::clear_counts();
        try {
            (void) ranges::uninitialized_default_construct_n(wrapped_input.begin(), int_wrapper::magic_throwing_val);
            assert(false);
        } catch (int i) {
            assert(i == int_wrapper::magic_throwing_val);
        } catch (...) {
            assert(false);
        }
        assert(int_wrapper::constructions == int_wrapper::magic_throwing_val);
        assert(int_wrapper::destructions == int_wrapper::magic_throwing_val - 1);
    }
};

using test_range = test::range<test::fwd, int_wrapper, test::Sized::no, test::CanDifference::no, test::Common::no,
    test::CanCompare::yes, test::ProxyRef::no>;

int main() {
    // The algorithm is oblivious to non-required category, size, difference. It _is_ sensitive to proxyness in that it
    // requires non-proxy references for the input range.

    instantiator::call<test_range>();
    throwing_test::call<test_range>();
}
