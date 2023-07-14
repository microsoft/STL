// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef _M_CEE // TRANSITION, VSO-1659496
#include <cassert>
#include <functional>
#include <iterator>
#include <ranges>
#include <utility>

using namespace std;

// TRANSITION, GH-1596, should use ranges::count
struct my_count_fn {
    template <input_iterator I, sentinel_for<I> S, class T, class Proj = identity>
        requires indirect_binary_predicate<ranges::equal_to, projected<I, Proj>, const T*>
    constexpr iter_difference_t<I> operator()(I first, S last, const T& value, Proj proj = {}) const {
        iter_difference_t<I> counter = 0;
        for (; first != last; ++first) {
            if (std::invoke(proj, *first) == value) { // intentionally qualified to avoid ADL
                ++counter;
            }
        }
        return counter;
    }

    template <ranges::input_range R, class T, class Proj = identity>
        requires indirect_binary_predicate<ranges::equal_to, projected<ranges::iterator_t<R>, Proj>, const T*>
    constexpr ranges::range_difference_t<R> operator()(R&& r, const T& value, Proj proj = {}) const {
        return (*this)(ranges::begin(r), ranges::end(r), value, ref(proj));
    }
};

inline constexpr my_count_fn my_count;

template <class T>
struct Holder {
    T t;
};
struct Incomplete;

static_assert(equality_comparable<Holder<Incomplete>*>);
static_assert(indirectly_comparable<Holder<Incomplete>**, Holder<Incomplete>**, equal_to<>>);
static_assert(sortable<Holder<Incomplete>**>);

constexpr bool test() {
    Holder<Incomplete>* a[10] = {};
    assert(my_count(a, a + 10, nullptr) == 10);
    assert(my_count(a, nullptr) == 10);
    return true;
}

static_assert(test());
#endif // _M_CEE
