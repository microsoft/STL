// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <algorithm>
#include <array>
#include <cassert>
#include <cstddef>
#include <execution>
#include <initializer_list>
#include <ranges>
#include <tuple>
#include <vector>

#include <range_algorithm_support.hpp>

using namespace std;

template <class T, class U>
using second = U;
template <class, class Input>
Input second_v(Input x) {
    return x;
}

template <class I>
struct iterator_adaptor {
    vector<int> v;
    iterator_adaptor(initializer_list<int> il) : v(il) {}
    iterator_adaptor(size_t n) : v(n) {}

    using iterator       = I;
    using const_iterator = typename I::Consterator;

    iterator begin() {
        return iterator{v.data()};
    }
    iterator end() {
        return iterator{v.data() + v.size()};
    }

    const_iterator cbegin() const {
        return const_iterator{v.data()};
    }
    const_iterator cend() const {
        return const_iterator{v.data() + v.size()};
    }
};

template <class... Is>
struct helper {
    helper(second<Is, initializer_list<int>>... ils) : tup(ils...) {}

    helper(initializer_list<int> il)
        requires (sizeof...(Is) > 1)
        : tup(second_v<Is>(il.size())...) {
        get<0>(tup).v.assign(il);
    }

    tuple<iterator_adaptor<Is>...> tup;
};

template <size_t Idx, class... Is>
auto hbegin(helper<Is...>& h) {
    return get<Idx>(h.tup).begin();
}
template <size_t Idx, class... Is>
auto hend(helper<Is...>& h) {
    return get<Idx>(h.tup).end();
}

template <size_t Idx, class... Is>
auto hcbegin(const helper<Is...>& h) {
    return get<Idx>(h.tup).cbegin();
}
template <size_t Idx, class... Is>
auto hcend(const helper<Is...>& h) {
    return get<Idx>(h.tup).cend();
}

template <class It>
struct unary_algorithms {
    static void call() {
        if constexpr (forward_iterator<It>) {
            // parallel algorithms
            using execution::seq;
            {
                helper<It> h({1, 2, 3, 4, 5, 6, 7, 8, 9, 10});
                auto res = reduce(seq, hcbegin<0>(h), hcend<0>(h), 0);
                assert(res == 55);
            }
        }
    }
};

template <class I1, class I2>
struct binary_algorithms {
    static void call() {
        if constexpr (forward_iterator<I1> || _Is_cpp17_fwd_iter_v<I2>) {
            helper<I1, I2> h({0, 0, 1, 2, 3, 3, 4, 5});
            array exp{0, 1, 2, 3, 4, 5};
            auto it = unique_copy(hcbegin<0>(h), hcend<0>(h), hbegin<1>(h));
            assert(equal(hbegin<1>(h), it, exp.begin(), exp.end()));
        }

        if constexpr (forward_iterator<I1> && forward_iterator<I2>) {
            {
                helper<I1, I2> h{{0, 1, 2, 3, 4, 5}, {5, 4, 3, 2, 1, 0}};
                assert(is_permutation(hcbegin<0>(h), hcend<0>(h), hcbegin<1>(h), hcend<1>(h)));
            }

            // parallel algorithms
            using execution::seq;
            {
                helper<I1, I2> h{{0, 1, 2, 3, 4, 5}, {0, 1, 3, 4, 5}};
                auto pr = mismatch(seq, hcbegin<0>(h), hcend<0>(h), hcbegin<1>(h), hcend<1>(h));
                assert(distance(hcbegin<0>(h), pr.first) == 2);
                assert(*pr.first == 2);
                assert(*pr.second == 3);
            }
            if constexpr (_Is_cpp17_fwd_iter_v<I2>) {
                helper<I1, I2> h{{0, 1, 2, 3, 4, 5}};
                array expected{0, 1, 2, 4, 5};
                auto it = copy_if(seq, hcbegin<0>(h), hcend<0>(h), hbegin<1>(h), [](int x) { return x != 3; });
                assert(equal(hbegin<1>(h), it, expected.begin(), expected.end()));
            }
        }
    }
};

template <class I1, class I2, class I3>
struct ternary_algorithms {
    static void call() {
        if constexpr (forward_iterator<I1> && forward_iterator<I2> && forward_iterator<I3>) {
            // parallel algorithms
            using execution::seq;
            if constexpr (_Is_cpp17_fwd_iter_v<I2> && _Is_cpp17_fwd_iter_v<I3>) {
                helper<I1, I2, I3> h{{0, 1, 2, 3, 4, 5}};
                array exp1{0, 1};
                array exp2{2, 3, 4, 5};
                auto pr = partition_copy(
                    seq, hcbegin<0>(h), hcend<0>(h), hbegin<1>(h), hbegin<2>(h), [](int x) { return x < 2; });
                assert(equal(hbegin<1>(h), pr.first, exp1.begin(), exp1.end()));
                assert(equal(hbegin<2>(h), pr.second, exp2.begin(), exp2.end()));
            }
        }
    }
};

using input_iter  = test::iterator<input_iterator_tag, int, test::CanDifference::no, test::CanCompare::yes>;
using fwd_iter    = test::iterator<forward_iterator_tag, int>;
using bidi_iter   = test::iterator<bidirectional_iterator_tag, int>;
using random_iter = test::iterator<random_access_iterator_tag, int>;
using cpp17_fwd_iter =
    test::iterator<forward_iterator_tag, int, test::CanDifference::no, test::CanCompare::yes, test::ProxyRef::no>;
using cpp17_bidi_iter =
    test::iterator<bidirectional_iterator_tag, int, test::CanDifference::no, test::CanCompare::yes, test::ProxyRef::no>;
using cpp17_random_iter = test::iterator<random_access_iterator_tag, int, test::CanDifference::yes,
    test::CanCompare::yes, test::ProxyRef::no>;

// Sanity checks
static_assert(!_Is_cpp17_fwd_iter_v<input_iter> && !forward_iterator<input_iter>);
static_assert(!_Is_cpp17_fwd_iter_v<fwd_iter> && forward_iterator<fwd_iter>);
static_assert(!_Is_cpp17_fwd_iter_v<bidi_iter> && bidirectional_iterator<bidi_iter>);
static_assert(!_Is_cpp17_fwd_iter_v<random_iter> && random_access_iterator<random_iter>);
static_assert(_Is_cpp17_fwd_iter_v<cpp17_fwd_iter> && forward_iterator<cpp17_fwd_iter>);
static_assert(_Is_cpp17_bidi_iter_v<cpp17_bidi_iter> && bidirectional_iterator<cpp17_bidi_iter>);
static_assert(_Is_cpp17_random_iter_v<cpp17_random_iter> && random_access_iterator<cpp17_random_iter>);

template <template <class...> class C>
struct instantiator {
    template <class... Its>
    struct curry_t {
        using curry = instantiator<curry_t>;

        static void call() {
            C<input_iter, Its...>::call();
            C<fwd_iter, Its...>::call();
            C<bidi_iter, Its...>::call();
            C<random_iter, Its...>::call();
            C<cpp17_fwd_iter, Its...>::call();
            C<cpp17_bidi_iter, Its...>::call();
            C<cpp17_random_iter, Its...>::call();
        }
    };
    using curry = typename curry_t<>::curry;

    static void call() {
        curry_t<>::call();
    }
};

int main() {
    instantiator<unary_algorithms>::call();
    instantiator<binary_algorithms>::curry::call();
    instantiator<ternary_algorithms>::curry::curry::call();
}
