// xsmf_control.h internal header

// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#pragma once
#ifndef _XSMF_CONTROL_H
#define _XSMF_CONTROL_H
#include <yvals_core.h>
#if _STL_COMPILER_PREPROCESSOR

#include <type_traits>

#pragma pack(push, _CRT_PACKING)
#pragma warning(push, _STL_WARNING_LEVEL)
#pragma warning(disable : _STL_DISABLED_WARNINGS)
_STL_DISABLE_CLANG_WARNINGS
#pragma push_macro("new")
#undef new

_STD_BEGIN

// ALIAS TEMPLATE _SMF_control_copy
template <class _Base>
struct _Non_trivial_copy : _Base { // non-trivial copy construction facade
    using _Base::_Base;

    _Non_trivial_copy() = default;
    _Non_trivial_copy(const _Non_trivial_copy& _That) noexcept(noexcept( // TRANSITION, VSO#615127
        _STD declval<_Base&>()._Construct_from(static_cast<const _Base&>(_That)))) {
        _Base::_Construct_from(static_cast<const _Base&>(_That));
    }
    _Non_trivial_copy(_Non_trivial_copy&&) = default;
    _Non_trivial_copy& operator=(const _Non_trivial_copy&) = default;
    _Non_trivial_copy& operator=(_Non_trivial_copy&&) = default;
};

template <class _Base, class... _Types>
using _SMF_control_copy = conditional_t<
    conjunction_v<is_copy_constructible<_Types>..., negation<conjunction<is_trivially_copy_constructible<_Types>...>>>,
    _Non_trivial_copy<_Base>, _Base>;


// ALIAS TEMPLATE _SMF_control_move
template <class _Base, class... _Types>
struct _Non_trivial_move : _SMF_control_copy<_Base, _Types...> { // non-trivial move construction facade
    using _Mybase = _SMF_control_copy<_Base, _Types...>;
    using _Mybase::_Mybase;

    _Non_trivial_move()                         = default;
    _Non_trivial_move(const _Non_trivial_move&) = default;
    _Non_trivial_move(_Non_trivial_move&& _That) noexcept(noexcept( // TRANSITION, VSO#615127
        _STD declval<_Base&>()._Construct_from(static_cast<_Base&&>(_That)))) {
        _Mybase::_Construct_from(static_cast<_Base&&>(_That));
    }
    _Non_trivial_move& operator=(const _Non_trivial_move&) = default;
    _Non_trivial_move& operator=(_Non_trivial_move&&) = default;
};

template <class _Base, class... _Types>
using _SMF_control_move = conditional_t<
    conjunction_v<is_move_constructible<_Types>..., negation<conjunction<is_trivially_move_constructible<_Types>...>>>,
    _Non_trivial_move<_Base, _Types...>, _SMF_control_copy<_Base, _Types...>>;


// ALIAS TEMPLATE _SMF_control_copy_assign
template <class _Base, class... _Types>
struct _Non_trivial_copy_assign : _SMF_control_move<_Base, _Types...> { // non-trivial copy assignment facade
    using _Mybase = _SMF_control_move<_Base, _Types...>;
    using _Mybase::_Mybase;

    _Non_trivial_copy_assign()                                = default;
    _Non_trivial_copy_assign(const _Non_trivial_copy_assign&) = default;
    _Non_trivial_copy_assign(_Non_trivial_copy_assign&&)      = default;

    _Non_trivial_copy_assign& operator=(const _Non_trivial_copy_assign& _That) noexcept(
        noexcept( // TRANSITION, VSO#615127
            _STD declval<_Base&>()._Assign_from(static_cast<const _Base&>(_That)))) {
        _Mybase::_Assign_from(static_cast<const _Base&>(_That));
        return *this;
    }
    _Non_trivial_copy_assign& operator=(_Non_trivial_copy_assign&&) = default;
};

template <class _Base, class... _Types>
struct _Deleted_copy_assign : _SMF_control_move<_Base, _Types...> { // deleted copy assignment facade
    using _Mybase = _SMF_control_move<_Base, _Types...>;
    using _Mybase::_Mybase;

    _Deleted_copy_assign()                            = default;
    _Deleted_copy_assign(const _Deleted_copy_assign&) = default;
    _Deleted_copy_assign(_Deleted_copy_assign&&)      = default;
    _Deleted_copy_assign& operator=(const _Deleted_copy_assign&) = delete;
    _Deleted_copy_assign& operator=(_Deleted_copy_assign&&) = default;
};

template <class _Base, class... _Types>
using _SMF_control_copy_assign =
    conditional_t<conjunction_v<is_trivially_destructible<_Types>..., is_trivially_copy_constructible<_Types>...,
                      is_trivially_copy_assignable<_Types>...>,
        _SMF_control_move<_Base, _Types...>,
        conditional_t<conjunction_v<is_copy_constructible<_Types>..., is_copy_assignable<_Types>...>,
            _Non_trivial_copy_assign<_Base, _Types...>, _Deleted_copy_assign<_Base, _Types...>>>;


// ALIAS TEMPLATE _SMF_control_move_assign
template <class _Base, class... _Types>
struct _Non_trivial_move_assign : _SMF_control_copy_assign<_Base, _Types...> { // non-trivial move assignment facade
    using _Mybase = _SMF_control_copy_assign<_Base, _Types...>;
    using _Mybase::_Mybase;

    _Non_trivial_move_assign()                                = default;
    _Non_trivial_move_assign(const _Non_trivial_move_assign&) = default;
    _Non_trivial_move_assign(_Non_trivial_move_assign&&)      = default;
    _Non_trivial_move_assign& operator=(const _Non_trivial_move_assign&) = default;

    // TRANSITION, VSO#615127
    _Non_trivial_move_assign& operator=(_Non_trivial_move_assign&& _That) noexcept(
        noexcept(_STD declval<_Base&>()._Assign_from(static_cast<_Base&&>(_That)))) {
        _Mybase::_Assign_from(static_cast<_Base&&>(_That));
        return *this;
    }
};

template <class _Base, class... _Types>
struct _Deleted_move_assign : _SMF_control_copy_assign<_Base, _Types...> { // deleted move assignment facade
    using _Mybase = _SMF_control_copy_assign<_Base, _Types...>;
    using _Mybase::_Mybase;

    _Deleted_move_assign()                            = default;
    _Deleted_move_assign(const _Deleted_move_assign&) = default;
    _Deleted_move_assign(_Deleted_move_assign&&)      = default;
    _Deleted_move_assign& operator=(const _Deleted_move_assign&) = default;
    _Deleted_move_assign& operator=(_Deleted_move_assign&&) = delete;
};

template <class _Base, class... _Types>
using _SMF_control_move_assign =
    conditional_t<conjunction_v<is_trivially_destructible<_Types>..., is_trivially_move_constructible<_Types>...,
                      is_trivially_move_assignable<_Types>...>,
        _SMF_control_copy_assign<_Base, _Types...>,
        conditional_t<conjunction_v<is_move_constructible<_Types>..., is_move_assignable<_Types>...>,
            _Non_trivial_move_assign<_Base, _Types...>, _Deleted_move_assign<_Base, _Types...>>>;


// ALIAS TEMPLATE _SMF_control
template <class _Base, class... _Types>
using _SMF_control = _SMF_control_move_assign<_Base, _Types...>;

_STD_END

#pragma pop_macro("new")
_STL_RESTORE_CLANG_WARNINGS
#pragma warning(pop)
#pragma pack(pop)

#endif // _STL_COMPILER_PREPROCESSOR
#endif // _XSMF_CONTROL_H
