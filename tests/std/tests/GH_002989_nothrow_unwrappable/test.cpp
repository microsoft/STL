// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <filesystem>
#include <list>
#include <type_traits>
#include <vector>
#include <xutility>

#if _HAS_CXX20
#include <ranges>
#endif

using namespace std;
using filesystem::path;

#define STATIC_ASSERT(...) static_assert(__VA_ARGS__, #__VA_ARGS__)

struct Predicate {
    template <class T>
    bool operator()(const T&) const {
        return true;
    }
};

template <class It, bool CopyUnwrapNothrow = true>
void do_single_test() {
    STATIC_ASSERT(_Is_nothrow_unwrappable_v<It>);
    STATIC_ASSERT(_Is_nothrow_unwrappable_v<It&&>);
    STATIC_ASSERT(noexcept(_Get_unwrapped(declval<It>())));

    STATIC_ASSERT(_Is_nothrow_unwrappable_v<const It&> == CopyUnwrapNothrow);
    STATIC_ASSERT(noexcept(_Get_unwrapped(declval<const It&>())) == CopyUnwrapNothrow);
    STATIC_ASSERT(_Is_nothrow_unwrappable_v<const It&&> == CopyUnwrapNothrow);
    STATIC_ASSERT(noexcept(_Get_unwrapped(declval<const It&&>())) == CopyUnwrapNothrow);
}

template <class It, bool CopyUnwrapNothrow = true>
void do_full_test() {
    do_single_test<It, CopyUnwrapNothrow>();
    do_single_test<reverse_iterator<It>, CopyUnwrapNothrow>();
    do_single_test<move_iterator<It>, CopyUnwrapNothrow>();

#ifdef __cpp_lib_concepts // TRANSITION, GH-395
    using R = ranges::subrange<It, It>;

    do_single_test<ranges::iterator_t<R>, CopyUnwrapNothrow>();

    // iterator_t<filter_view> does not define _Unwrapped
    do_single_test<ranges::iterator_t<ranges::filter_view<R, Predicate>>, true>();
    // iterator_t<transform_view> does not define _Unwrapped
    do_single_test<ranges::iterator_t<ranges::transform_view<R, Predicate>>, true>();
    if constexpr (ranges::bidirectional_range<R>) {
        // iterator_t<reverse_view> does not define _Unwrapped
        do_single_test<ranges::iterator_t<ranges::reverse_view<R>>, true>();
    }
#endif // __cpp_lib_concepts
}

int main() {
    do_single_test<int>();
    do_full_test<int*>();
    do_single_test<int[]>();

    do_full_test<vector<int>::iterator>();
    do_full_test<vector<int>::const_iterator>();
    do_full_test<list<int>::iterator>();
    do_full_test<list<int>::const_iterator>();

    do_full_test<path::iterator, false>();
}
