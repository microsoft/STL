// xpolymorphic_allocator.h internal header

// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#pragma once
#ifndef _XPOLYMORPHIC_ALLOCATOR_H
#define _XPOLYMORPHIC_ALLOCATOR_H
#include <yvals_core.h>
#if _STL_COMPILER_PREPROCESSOR
#include <tuple>
#include <type_traits>
#include <utility>
#include <xmemory>

#pragma pack(push, _CRT_PACKING)
#pragma warning(push, _STL_WARNING_LEVEL)
#pragma warning(disable : _STL_DISABLED_WARNINGS)
_STL_DISABLE_CLANG_WARNINGS
#pragma push_macro("new")
#undef new

_STD_BEGIN

// FUNCTION TEMPLATE _Uses_allocator_construct
template <class _Ty, class _Outer_alloc, class _Inner_alloc, class... _Types>
void _Uses_allocator_construct2(
    true_type, _Ty* const _Ptr, _Outer_alloc& _Outer, _Inner_alloc& _Inner, _Types&&... _Args) {
    // uses-allocator construction of *_Ptr by alloc _Outer propagating alloc _Inner, allocator_arg_t case
    allocator_traits<_Outer_alloc>::construct(_Outer, _Ptr, allocator_arg, _Inner, _STD forward<_Types>(_Args)...);
}

template <class _Ty, class _Outer_alloc, class _Inner_alloc, class... _Types>
void _Uses_allocator_construct2(
    false_type, _Ty* const _Ptr, _Outer_alloc& _Outer, _Inner_alloc& _Inner, _Types&&... _Args) {
    // uses-allocator construction of *_Ptr by alloc _Outer propagating alloc _Inner, non-allocator_arg_t case
    static_assert(is_constructible_v<_Ty, _Types..., _Inner_alloc&>,
        "N4700 23.10.7.2 [allocator.uses.construction]/1 requires "
        "is_constructible_v<T, Args..., Alloc&> when uses_allocator_v<T, Alloc> is true and "
        "is_constructible_v<T, allocator_arg_t, Alloc&, Args...> is false");
    allocator_traits<_Outer_alloc>::construct(_Outer, _Ptr, _STD forward<_Types>(_Args)..., _Inner);
}

template <class _Ty, class _Outer_alloc, class _Inner_alloc, class... _Types>
void _Uses_allocator_construct1(
    true_type, _Ty* const _Ptr, _Outer_alloc& _Outer, _Inner_alloc& _Inner, _Types&&... _Args) {
    // uses-allocator construction of *_Ptr by alloc _Outer propagating alloc _Inner,
    // uses_allocator_v<_Ty, _Inner_alloc> case
    using _IsConstructible = typename is_constructible<_Ty, allocator_arg_t, _Inner_alloc&, _Types...>::type;
    _Uses_allocator_construct2(_IsConstructible{}, _Ptr, _Outer, _Inner, _STD forward<_Types>(_Args)...);
}

template <class _Ty, class _Outer_alloc, class _Inner_alloc, class... _Types>
void _Uses_allocator_construct1(false_type, _Ty* const _Ptr, _Outer_alloc& _Outer, _Inner_alloc&, _Types&&... _Args) {
    // uses-allocator construction of *_Ptr by alloc _Outer, !uses_allocator_v<_Ty, _Inner_alloc> case
    static_assert(is_constructible_v<_Ty, _Types...>,
        "N4700 23.10.7.2 [allocator.uses.construction]/1 requires "
        "is_constructible_v<T, Args...> when uses_allocator_v<T, Alloc> is false");
    allocator_traits<_Outer_alloc>::construct(_Outer, _Ptr, _STD forward<_Types>(_Args)...);
}

template <class _Ty, class _Outer_alloc, class _Inner_alloc, class... _Types,
    enable_if_t<!_Is_specialization_v<_Ty, pair>, int> = 0>
void _Uses_allocator_construct(_Ty* const _Ptr, _Outer_alloc& _Outer, _Inner_alloc& _Inner, _Types&&... _Args) {
    // uses-allocator construction of *_Ptr by alloc _Outer propagating alloc _Inner, non-pair case
    _Uses_allocator_construct1(uses_allocator<_Ty, _Inner_alloc>(), _Ptr, _Outer, _Inner,
        _STD forward<_Types>(_Args)...); // TRANSITION, if constexpr
}

template <class _Alloc, class... _Types>
auto _Uses_allocator_piecewise2(true_type, _Alloc& _Al, tuple<_Types...>&& _Tuple) {
    return _STD tuple_cat(tuple<allocator_arg_t, _Alloc&>(allocator_arg, _Al), _STD move(_Tuple));
}

template <class _Alloc, class... _Types>
auto _Uses_allocator_piecewise2(false_type, _Alloc& _Al, tuple<_Types...>&& _Tuple) {
    return _STD tuple_cat(_STD move(_Tuple), tuple<_Alloc&>(_Al));
}

template <class _Ty, class _Alloc, class... _Types>
auto _Uses_allocator_piecewise(true_type, _Alloc& _Al, tuple<_Types...>&& _Tuple) {
    return _Uses_allocator_piecewise2(
        is_constructible<_Ty, allocator_arg_t, _Alloc&, _Types...>(), _Al, _STD move(_Tuple));
}

template <class, class _Alloc, class... _Types>
tuple<_Types...>&& _Uses_allocator_piecewise(false_type, _Alloc&, tuple<_Types...>&& _Tuple) {
    return _STD move(_Tuple);
}

template <class _Ty1, class _Ty2, class _Outer_alloc, class _Inner_alloc, class... _Types1, class... _Types2>
void _Uses_allocator_construct_pair(pair<_Ty1, _Ty2>* const _Ptr, _Outer_alloc& _Outer, _Inner_alloc& _Inner,
    tuple<_Types1...>&& _Val1, tuple<_Types2...>&& _Val2) {
    // uses-allocator construction of pair from _Val1 and _Val2 by alloc _Outer propagating alloc _Inner
    allocator_traits<_Outer_alloc>::construct(_Outer, _Ptr, piecewise_construct,
        _Uses_allocator_piecewise<_Ty1>(uses_allocator<_Ty1, _Inner_alloc>(), _Inner, _STD move(_Val1)),
        _Uses_allocator_piecewise<_Ty2>(uses_allocator<_Ty2, _Inner_alloc>(), _Inner, _STD move(_Val2)));
}

template <class _Ty1, class _Ty2, class _Outer_alloc, class _Inner_alloc, class... _Types1, class... _Types2>
void _Uses_allocator_construct(pair<_Ty1, _Ty2>* const _Ptr, _Outer_alloc& _Outer, _Inner_alloc& _Inner,
    piecewise_construct_t, tuple<_Types1...> _Val1, tuple<_Types2...> _Val2) {
    // uses-allocator construction of pair by alloc _Outer propagating alloc _Inner, piecewise case
    _Uses_allocator_construct_pair(_Ptr, _Outer, _Inner, _STD move(_Val1), _STD move(_Val2));
}

template <class _Ty1, class _Ty2, class _Outer_alloc, class _Inner_alloc>
void _Uses_allocator_construct(pair<_Ty1, _Ty2>* const _Ptr, _Outer_alloc& _Outer, _Inner_alloc& _Inner) {
    // uses-allocator construction of pair by alloc _Outer propagating alloc _Inner, zero-argument case
    _Uses_allocator_construct_pair(_Ptr, _Outer, _Inner, tuple<>{}, tuple<>{});
}

template <class _Ty1, class _Ty2, class _Outer_alloc, class _Inner_alloc, class _Uty, class _Vty>
void _Uses_allocator_construct(
    pair<_Ty1, _Ty2>* const _Ptr, _Outer_alloc& _Outer, _Inner_alloc& _Inner, _Uty&& _Arg1, _Vty&& _Arg2) {
    // uses-allocator construction of pair by alloc _Outer propagating alloc _Inner, two-argument case
    _Uses_allocator_construct_pair(_Ptr, _Outer, _Inner, _STD forward_as_tuple(_STD forward<_Uty>(_Arg1)),
        _STD forward_as_tuple(_STD forward<_Vty>(_Arg2)));
}

template <class _Ty1, class _Ty2, class _Outer_alloc, class _Inner_alloc, class _Uty, class _Vty>
void _Uses_allocator_construct(
    pair<_Ty1, _Ty2>* const _Ptr, _Outer_alloc& _Outer, _Inner_alloc& _Inner, const pair<_Uty, _Vty>& _Pair) {
    // uses-allocator construction of pair by alloc _Outer propagating alloc _Inner, lvalue pair argument
    _Uses_allocator_construct_pair(
        _Ptr, _Outer, _Inner, _STD forward_as_tuple(_Pair.first), _STD forward_as_tuple(_Pair.second));
}

template <class _Ty1, class _Ty2, class _Outer_alloc, class _Inner_alloc, class _Uty, class _Vty>
void _Uses_allocator_construct(
    pair<_Ty1, _Ty2>* const _Ptr, _Outer_alloc& _Outer, _Inner_alloc& _Inner, pair<_Uty, _Vty>&& _Pair) {
    // uses-allocator construction of pair by alloc _Outer propagating alloc _Inner, rvalue pair argument
    _Uses_allocator_construct_pair(_Ptr, _Outer, _Inner, _STD forward_as_tuple(_STD forward<_Uty>(_Pair.first)),
        _STD forward_as_tuple(_STD forward<_Vty>(_Pair.second)));
}

#if _HAS_CXX17
namespace pmr {

    // CLASS memory_resource
    class __declspec(novtable) memory_resource {
    public:
        virtual ~memory_resource() noexcept {}

        _NODISCARD _DECLSPEC_ALLOCATOR void* allocate(_CRT_GUARDOVERFLOW const size_t _Bytes,
            const size_t _Align = alignof(max_align_t)) { // allocate _Bytes bytes of memory with alignment _Align
            _STL_ASSERT(_Is_pow_2(_Align), "memory_resource::allocate(): Alignment must be a power of two.");
            return do_allocate(_Bytes, _Align);
        }

        void deallocate(void* const _Ptr, const size_t _Bytes, const size_t _Align = alignof(max_align_t)) {
            // deallocate _Ptr, which was returned from allocate(_Bytes, _Align)
            _STL_ASSERT(_Is_pow_2(_Align), "memory_resource::deallocate(): Alignment must be a power of two.");
            return do_deallocate(_Ptr, _Bytes, _Align);
        }

        _NODISCARD bool is_equal(const memory_resource& _That) const noexcept {
            // determine if *this and _That can both deallocate memory allocated by either
            return do_is_equal(_That);
        }

    private:
        virtual void* do_allocate(size_t _Bytes, size_t _Align)               = 0;
        virtual void do_deallocate(void* _Ptr, size_t _Bytes, size_t _Align)  = 0;
        virtual bool do_is_equal(const memory_resource& _That) const noexcept = 0;
    };

    _NODISCARD inline bool operator==(const memory_resource& _Left, const memory_resource& _Right) noexcept {
        return &_Left == &_Right || _Left.is_equal(_Right);
    }

    _NODISCARD inline bool operator!=(const memory_resource& _Left, const memory_resource& _Right) noexcept {
        return !(_Left == _Right);
    }

    // FUNCTION get_default_resource
    extern "C" _CRT_SATELLITE_1 memory_resource* __cdecl _Aligned_get_default_resource() noexcept;
    extern "C" _CRT_SATELLITE_1 memory_resource* __cdecl _Unaligned_get_default_resource() noexcept;

    _NODISCARD inline memory_resource* get_default_resource() noexcept {
#ifdef __cpp_aligned_new
        return _Aligned_get_default_resource();
#else // ^^^ __cpp_aligned_new / !__cpp_aligned_new vvv
        return _Unaligned_get_default_resource();
#endif // __cpp_aligned_new
    }

    // CLASS TEMPLATE polymorphic_allocator
    template <class _Ty>
    class polymorphic_allocator {
    public:
        template <class>
        friend class polymorphic_allocator;

        using value_type = _Ty;

        polymorphic_allocator() noexcept = default;

        /* implicit */ polymorphic_allocator(memory_resource* const _Resource_) noexcept // strengthened
            : _Resource{_Resource_} { // initialize with _Resource_
            _STL_ASSERT(_Resource, "Cannot initialize polymorphic_allocator with null resource (N4810 20.12.3.1 "
                                   "[mem.poly.allocator.ctor]/2)");
        }

        polymorphic_allocator(const polymorphic_allocator&) = default;

        template <class _Uty>
        polymorphic_allocator(const polymorphic_allocator<_Uty>& _That) noexcept
            : _Resource{_That._Resource} { // initialize with _That's resource
        }

        polymorphic_allocator& operator=(const polymorphic_allocator&) = delete;

        _NODISCARD _DECLSPEC_ALLOCATOR _Ty* allocate(_CRT_GUARDOVERFLOW const size_t _Count) {
            // get space for _Count objects of type _Ty from _Resource
            void* const _Vp = _Resource->allocate(_Get_size_of_n<sizeof(_Ty)>(_Count), alignof(_Ty));
            return static_cast<_Ty*>(_Vp);
        }

        void deallocate(_Ty* const _Ptr, const size_t _Count) noexcept /* strengthened */ {
            // return space for _Count objects of type _Ty to _Resource
            // No need to verify that size_t can represent the size of _Ty[_Count].
            _Resource->deallocate(_Ptr, _Count * sizeof(_Ty), alignof(_Ty));
        }

        template <class _Uty, class... _Types>
        void construct(_Uty* const _Ptr, _Types&&... _Args) {
            // propagate allocator *this if uses_allocator_v<_Uty, polymorphic_allocator>
            allocator<char> _Al{};
            _Uses_allocator_construct(_Ptr, _Al, *this, _STD forward<_Types>(_Args)...);
        }

        _NODISCARD polymorphic_allocator select_on_container_copy_construction() const noexcept /* strengthened */ {
            // don't propagate on copy
            return {};
        }

        _NODISCARD memory_resource* resource() const noexcept /* strengthened */ {
            // retrieve this allocator's memory_resource
            return _Resource;
        }

    private:
        memory_resource* _Resource = _STD pmr::get_default_resource();
    };

    template <class _Ty1, class _Ty2>
    _NODISCARD bool operator==(
        const polymorphic_allocator<_Ty1>& _Left, const polymorphic_allocator<_Ty2>& _Right) noexcept {
        // polymorphic_allocators with the same resource are compatible
        return *_Left.resource() == *_Right.resource();
    }

    template <class _Ty1, class _Ty2>
    _NODISCARD bool operator!=(
        const polymorphic_allocator<_Ty1>& _Left, const polymorphic_allocator<_Ty2>& _Right) noexcept {
        return !(_Left == _Right);
    }

} // namespace pmr

#endif // _HAS_CXX17

_STD_END

#pragma pop_macro("new")
_STL_RESTORE_CLANG_WARNINGS
#pragma warning(pop)
#pragma pack(pop)
#endif // _STL_COMPILER_PREPROCESSOR
#endif // _XPOLYMORPHIC_ALLOCATOR_H
