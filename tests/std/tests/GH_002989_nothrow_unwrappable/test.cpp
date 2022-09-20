// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <list>
#include <type_traits>
#include <vector>
#include <xutility>

#if _HAS_CXX17
#include <filesystem>
#endif

#if _HAS_CXX20
#include <ranges>
#endif

using namespace std;

#if _HAS_CXX17
using filesystem::path;
#endif

#define STATIC_ASSERT(...) static_assert(__VA_ARGS__, #__VA_ARGS__)

struct Predicate {
    template <class T>
    bool operator()(const T&) const {
        return true;
    }
};

template <class It, bool CopyUnwrapNothrow = true>
void do_single_test() {
    STATIC_ASSERT(_Unwrappable_v<It> == _Has_nothrow_unwrapped<It>);
    STATIC_ASSERT(_Unwrappable_v<It> == _Has_nothrow_unwrapped<It&&>);
    STATIC_ASSERT(noexcept(_Get_unwrapped(declval<It>())));

    // !a || b is equivalent to a => b (a implies b)
    // This is written this way to avoid `if constexpr` in C++14 mode.
    STATIC_ASSERT(!_Unwrappable_v<It> || _Has_nothrow_unwrapped<const It&> == CopyUnwrapNothrow);
    STATIC_ASSERT(!_Unwrappable_v<It> || _Has_nothrow_unwrapped<const It&&> == CopyUnwrapNothrow);
    STATIC_ASSERT(noexcept(_Get_unwrapped(declval<const It&>())) == CopyUnwrapNothrow);
    STATIC_ASSERT(noexcept(_Get_unwrapped(declval<const It&&>())) == CopyUnwrapNothrow);
}

template <class It, bool CopyUnwrapNothrow = true>
void do_full_test() {
    do_single_test<It, CopyUnwrapNothrow>();
    do_single_test<reverse_iterator<It>, CopyUnwrapNothrow>();
    do_single_test<move_iterator<It>, CopyUnwrapNothrow>();

#ifdef __cpp_lib_concepts
    using R = ranges::subrange<It, It>;

    // TRANSITION, GH-2997
    do_single_test<ranges::iterator_t<ranges::filter_view<R, Predicate>>, true>();
    // TRANSITION, GH-2997
    do_single_test<ranges::iterator_t<ranges::transform_view<R, Predicate>>, true>();
    if constexpr (bidirectional_iterator<It>) {
        do_single_test<ranges::iterator_t<ranges::reverse_view<R>>, CopyUnwrapNothrow>();
    }
#endif // __cpp_lib_concepts
}

struct BidiIterUnwrapThrowing : vector<int>::iterator {
    using Base = vector<int>::iterator;

    using Base::Base;

    using iterator_concept  = bidirectional_iterator_tag;
    using iterator_category = bidirectional_iterator_tag;

    BidiIterUnwrapThrowing& operator++() {
        Base::operator++();
        return *this;
    }
    BidiIterUnwrapThrowing operator++(int) {
        auto res = *this;
        Base::operator++();
        return res;
    }
    BidiIterUnwrapThrowing& operator--() {
        Base::operator--();
        return *this;
    }
    BidiIterUnwrapThrowing operator--(int) {
        auto res = *this;
        Base::operator--();
        return res;
    }

    using _Prevent_inheriting_unwrap = BidiIterUnwrapThrowing;

    int* _Unwrapped() const& noexcept(false) {
        return Base::_Unwrapped();
    }
    int* _Unwrapped() && noexcept {
        return move(*this).Base::_Unwrapped();
    }

    void _Seek_to(int* p) & noexcept {
        Base::_Seek_to(p);
    }
};

int main() {
    do_single_test<int>();
    do_full_test<int*>();
    do_single_test<int[]>();

    do_full_test<vector<int>::iterator>();
    do_full_test<vector<int>::const_iterator>();
    do_full_test<list<int>::iterator>();
    do_full_test<list<int>::const_iterator>();

    do_full_test<BidiIterUnwrapThrowing, false>();
#if _HAS_CXX17
    do_full_test<path::iterator, false>();
#endif
}
