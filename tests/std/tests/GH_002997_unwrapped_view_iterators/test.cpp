// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <ranges>
#include <vector>
#include <xutility>

using namespace std;

#define STATIC_ASSERT(...) static_assert(__VA_ARGS__, #__VA_ARGS__)

template <class Derived, bool MoveOnly>
struct UnwrappableBase : vector<int>::iterator {
    using _Base = vector<int>::iterator;

    using _Base::_Base;

    Derived& operator++() {
        _Base::operator++();
        return static_cast<Derived&>(*this);
    }
    Derived operator++(int) {
        auto res = static_cast<Derived&>(*this);
        _Base::operator++();
        return res;
    }
    Derived& operator--() {
        _Base::operator--();
        return static_cast<Derived&>(*this);
    }
    Derived operator--(int) {
        auto res = static_cast<Derived&>(*this);
        _Base::operator--();
        return res;
    }

    using _Prevent_inheriting_unwrap = Derived;
    int* _Unwrapped() const& noexcept(false) requires !MoveOnly {
        return _Base::_Unwrapped();
    }
    int* _Unwrapped() && noexcept {
        return _Base::_Unwrapped();
    }
    void _Seek_to(int* _It) noexcept {
        _Base::_Seek_to(_It);
    }

    bool operator==(const UnwrappableBase&) const  = default;
    auto operator<=>(const UnwrappableBase&) const = default;
};

struct ThrowingUnwrappable : UnwrappableBase<ThrowingUnwrappable, false> {
    using UnwrappableBase<ThrowingUnwrappable, false>::UnwrappableBase;

    bool operator==(const ThrowingUnwrappable&) const  = default;
    auto operator<=>(const ThrowingUnwrappable&) const = default;
};
struct MoveUnwrappable : UnwrappableBase<MoveUnwrappable, true> {
    using UnwrappableBase<MoveUnwrappable, true>::UnwrappableBase;

    bool operator==(const MoveUnwrappable&) const  = default;
    auto operator<=>(const MoveUnwrappable&) const = default;
};

template <template <class...> class V, class It, class... Args>
using view_iter_t = ranges::iterator_t<V<ranges::subrange<It, It>, Args...>>;

template <template <class...> class V, class... Args>
void do_test() {
    using nothrow_unwrappable_t  = view_iter_t<V, vector<int>::iterator, Args...>;
    using throwing_unwrappable_t = view_iter_t<V, ThrowingUnwrappable, Args...>;
    using move_unwrappable_t     = view_iter_t<V, MoveUnwrappable, Args...>;
    using not_unwrappable_t      = view_iter_t<V, int*, Args...>;

    STATIC_ASSERT(_Unwrappable_v<nothrow_unwrappable_t>);
    STATIC_ASSERT(_Unwrappable_v<const nothrow_unwrappable_t&>);
    STATIC_ASSERT(!_Unwrappable_v<_Unwrapped_t<nothrow_unwrappable_t>>);
    STATIC_ASSERT(_Is_nothrow_unwrappable_v<nothrow_unwrappable_t>);
    STATIC_ASSERT(_Is_nothrow_unwrappable_v<const nothrow_unwrappable_t&>);
    STATIC_ASSERT(is_same_v<decltype(declval<nothrow_unwrappable_t>().base()), vector<int>::iterator>);
    STATIC_ASSERT(is_same_v<decltype(declval<_Unwrapped_t<nothrow_unwrappable_t>>().base()), int*>);
    STATIC_ASSERT(is_same_v<decltype(declval<_Unwrapped_t<const nothrow_unwrappable_t&>>().base()), int*>);
    STATIC_ASSERT(noexcept(_Get_unwrapped(declval<nothrow_unwrappable_t>())));
    STATIC_ASSERT(noexcept(_Get_unwrapped(declval<const nothrow_unwrappable_t&>())));

    STATIC_ASSERT(_Unwrappable_v<throwing_unwrappable_t>);
    STATIC_ASSERT(_Unwrappable_v<const throwing_unwrappable_t&>);
    STATIC_ASSERT(!_Unwrappable_v<_Unwrapped_t<nothrow_unwrappable_t>>);
    STATIC_ASSERT(_Is_nothrow_unwrappable_v<throwing_unwrappable_t>);
    STATIC_ASSERT(!_Is_nothrow_unwrappable_v<const throwing_unwrappable_t&>);
    STATIC_ASSERT(is_same_v<decltype(declval<throwing_unwrappable_t>().base()), ThrowingUnwrappable>);
    STATIC_ASSERT(is_same_v<decltype(declval<_Unwrapped_t<throwing_unwrappable_t>>().base()), int*>);
    STATIC_ASSERT(is_same_v<decltype(declval<_Unwrapped_t<const throwing_unwrappable_t&>>().base()), int*>);
    STATIC_ASSERT(noexcept(_Get_unwrapped(declval<throwing_unwrappable_t>())));
    STATIC_ASSERT(!noexcept(_Get_unwrapped(declval<const throwing_unwrappable_t&>())));

    STATIC_ASSERT(_Unwrappable_v<move_unwrappable_t>);
    STATIC_ASSERT(!_Unwrappable_v<const move_unwrappable_t&>);
    STATIC_ASSERT(!_Unwrappable_v<_Unwrapped_t<nothrow_unwrappable_t>>);
    STATIC_ASSERT(_Is_nothrow_unwrappable_v<move_unwrappable_t>);
    STATIC_ASSERT(!_Is_nothrow_unwrappable_v<const move_unwrappable_t&>);
    STATIC_ASSERT(is_same_v<decltype(declval<move_unwrappable_t>().base()), MoveUnwrappable>);
    STATIC_ASSERT(is_same_v<decltype(declval<_Unwrapped_t<move_unwrappable_t>>().base()), int*>);
    STATIC_ASSERT(noexcept(_Get_unwrapped(declval<move_unwrappable_t>())));

    STATIC_ASSERT(!_Unwrappable_v<not_unwrappable_t>);
    STATIC_ASSERT(!_Unwrappable_v<const not_unwrappable_t&>);
    STATIC_ASSERT(!_Is_nothrow_unwrappable_v<not_unwrappable_t>);
    STATIC_ASSERT(!_Is_nothrow_unwrappable_v<const not_unwrappable_t&>);
    STATIC_ASSERT(is_same_v<decltype(declval<not_unwrappable_t>().base()), int*>);
    STATIC_ASSERT(noexcept(_Get_unwrapped(declval<not_unwrappable_t>())));
    STATIC_ASSERT(noexcept(_Get_unwrapped(declval<not_unwrappable_t>())));
}

struct Predicate {
    template <class T>
    bool operator()(T&&) {
        return true;
    }
};

int main() {
    do_test<ranges::transform_view, Predicate>();
}
