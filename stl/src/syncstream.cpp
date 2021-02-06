// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

// initialize syncstream mutex map

#include <functional>
#include <internal_shared.h>
#include <map>
#include <mutex>
#include <new>
#include <shared_mutex>
#include <type_traits>
#include <utility>

#pragma warning(disable : 4074)
#pragma init_seg(compiler)

namespace {
    // OBJECT DECLARATIONS
    struct _Mutex_count_pair {
        _STD shared_mutex _Mutex;
        size_t _Ref_count = 0;
    };

    template <class _Ty>
    class _Crt_allocator {
    public:
        using value_type                             = _Ty;
        using propagate_on_container_move_assignment = _STD true_type;
        using is_always_equal                        = _STD true_type;

        constexpr _Crt_allocator() noexcept = default;

        constexpr _Crt_allocator(const _Crt_allocator&) noexcept = default;
        template <class _Other>
        constexpr _Crt_allocator(const _Crt_allocator<_Other>&) noexcept {}

        _NODISCARD __declspec(allocator) _Ty* allocate(_CRT_GUARDOVERFLOW const size_t _Count) {
            const auto _Ptr = _calloc_crt(_Count, sizeof(_Ty));
            if (!_Ptr) {
                throw _STD bad_alloc{};
            }
            return static_cast<_Ty*>(_Ptr);
        }

        void deallocate(_Ty* const _Ptr, size_t) noexcept {
            _free_crt(_Ptr);
        }
    };

    using _Map_alloc = _Crt_allocator<_STD pair<void* const, _Mutex_count_pair>>;
    using _Map_type  = _STD map<void*, _Mutex_count_pair, _STD less<void*>, _Map_alloc>;

    _Map_type _Lookup_map;
    _STD shared_mutex _Lookup_mutex;
} // unnamed namespace

_EXTERN_C

_NODISCARD void* __stdcall __std_get_shared_mutex_for_instance(void* _Ptr) noexcept {
    _STD shared_lock _Guard(_Lookup_mutex);
    auto _Instance_mutex_iter = _Lookup_map.find(_Ptr);
    _ASSERT_EXPR(_Instance_mutex_iter != _Lookup_map.end(), "No mutex exists for given instance!");
    auto _Ret = &_Instance_mutex_iter->second._Mutex;
    static_assert(_STD is_same_v<decltype(_Ret), _STD shared_mutex*>);
    return _Ret; // caller will restore type
}

_NODISCARD bool __stdcall __std_acquire_shared_mutex_for_instance(void* _Ptr) noexcept {
    try {
        _STD scoped_lock _Guard(_Lookup_mutex);
        auto& _Refs = _Lookup_map.try_emplace(_Ptr).first->second._Ref_count;
        ++_Refs;
        return true;
    } catch (...) {
        return false;
    }
}

void __stdcall __std_release_shared_mutex_for_instance(void* _Ptr) noexcept {
    _STD scoped_lock _Guard(_Lookup_mutex);
    const auto _Instance_mutex_iter = _Lookup_map.find(_Ptr);
    _ASSERT_EXPR(_Instance_mutex_iter != _Lookup_map.end(), "No mutex exists for given instance!");
    auto& _Refs = _Instance_mutex_iter->second._Ref_count;
    if (--_Refs == 0) {
        _Lookup_map.erase(_Instance_mutex_iter);
    }
}

_END_EXTERN_C
