// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <algorithm>
#include <execution>
#include <ranges>

#include <range_algorithm_support.hpp>

using namespace std;

template <class T, class U>
using second = U;

template <forward_iterator I>
struct iterator_adaptor {
    std::vector<int> v;
    iterator_adaptor(initializer_list<int> il) : v(il) {}

    using iterator       = I;
    using const_iterator = I::Consterator;

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

template <forward_iterator... Is>
struct helper {
    helper(second<Is, initializer_list<int>>... ils) : tup(ils...) {}

    std::tuple<iterator_adaptor<Is>...> tup;
};

template <size_t Idx, forward_iterator... Is>
auto hbegin(helper<Is...>& h) {
    return get<Idx>(h.tup).begin();
}
template <size_t Idx, forward_iterator... Is>
auto hend(helper<Is...>& h) {
    return get<Idx>(h.tup).end();
}

template <size_t Idx, forward_iterator... Is>
auto hcbegin(const helper<Is...>& h) {
    return get<Idx>(h.tup).cbegin();
}
template <size_t Idx, forward_iterator... Is>
auto hcend(const helper<Is...>& h) {
    return get<Idx>(h.tup).cend();
}

template <forward_iterator I1, forward_iterator I2>
void test_algorithms() {
    using execution::seq;
    {
        helper<I1, I2> h{{0, 1, 2, 3, 4, 5}, {0, 1, 3, 4, 5}};
        auto pr = mismatch(seq, hcbegin<0>(h), hcend<0>(h), hcbegin<1>(h));
        assert(distance(hcbegin<0>(h), pr.first) == 2);
        assert(*pr.first == 2);
        assert(*pr.second == 3);
    }
    if constexpr (_Is_fwd_iter_v<I2>) {
        helper<I1, I2> h{{0, 1, 2, 3, 4, 5}, {0, 0, 0, 0, 0, 0}};
        initializer_list expected{0, 1, 2, 4, 5};
        auto it = copy_if(seq, hcbegin<0>(h), hcend<0>(h), hbegin<1>(h), [](int x) { return x != 3; });
        assert(std::equal(hbegin<1>(h), it, expected.begin(), expected.end()));
    }
}

using fwd_iter    = test::iterator<forward_iterator_tag, int>;
using bidi_iter   = test::iterator<bidirectional_iterator_tag, int>;
using random_iter = test::iterator<random_access_iterator_tag, int>;
using cpp17_fwd_iter =
    test::iterator<forward_iterator_tag, int, test::CanDifference::no, test::CanCompare::yes, test::ProxyRef::no>;
using cpp17_bidi_iter =
    test::iterator<bidirectional_iterator_tag, int, test::CanDifference::no, test::CanCompare::yes, test::ProxyRef::no>;
using cpp17_random_iter = test::iterator<random_access_iterator_tag, int, test::CanDifference::yes,
    test::CanCompare::yes, test::ProxyRef::no>;
static_assert(!_Is_fwd_iter_v<fwd_iter> && forward_iterator<fwd_iter>);
static_assert(!_Is_fwd_iter_v<bidi_iter> && bidirectional_iterator<bidi_iter>);
static_assert(!_Is_fwd_iter_v<random_iter> && random_access_iterator<random_iter>);
static_assert(_Is_fwd_iter_v<cpp17_fwd_iter> && forward_iterator<cpp17_fwd_iter>);
static_assert(_Is_bidi_iter_v<cpp17_bidi_iter> && bidirectional_iterator<cpp17_bidi_iter>);
static_assert(_Is_random_iter_v<cpp17_random_iter> && random_access_iterator<cpp17_random_iter>);


template <class I2>
void inst() {
    test_algorithms<fwd_iter, I2>();
    test_algorithms<bidi_iter, I2>();
    test_algorithms<random_iter, I2>();
    test_algorithms<cpp17_fwd_iter, I2>();
    test_algorithms<cpp17_bidi_iter, I2>();
    test_algorithms<cpp17_random_iter, I2>();
};


int main() {
    inst<fwd_iter>();
    inst<bidi_iter>();
    inst<random_iter>();
    inst<cpp17_fwd_iter>();
    inst<cpp17_bidi_iter>();
    inst<cpp17_random_iter>();
}
