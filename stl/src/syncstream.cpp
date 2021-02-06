// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

// initialize syncstream mutex map

#include <internal_shared.h>
#include <map>
#include <shared_mutex>

#pragma warning(disable : 4074)
#pragma init_seg(compiler)

_STD_BEGIN
// OBJECT DECLARATIONS
struct _Mutex_count_pair {
    shared_mutex _Mutex;
    uint64_t _Ref_count = 0;
};

template <class _Ty>
class _Crt_allocator {
public:
    using value_type                             = _Ty;
    using propagate_on_container_move_assignment = true_type;
    using is_always_equal                        = true_type;

    constexpr _Crt_allocator() noexcept = default;

    constexpr _Crt_allocator(const _Crt_allocator&) noexcept = default;
    template <class _Other>
    constexpr _Crt_allocator(const _Crt_allocator<_Other>&) noexcept {}

    _NODISCARD __declspec(allocator) _Ty* allocate(_CRT_GUARDOVERFLOW const size_t _Count) {
        const auto _Ptr = _calloc_crt(_Count, sizeof(_Ty));
        if (!_Ptr) {
            throw bad_alloc{};
        }
        return static_cast<_Ty*>(_Ptr);
    }

    void deallocate(_Ty* const _Ptr, size_t) noexcept {
        _free_crt(_Ptr);
    }
};

using _Map_alloc = _Crt_allocator<pair<void* const, _Mutex_count_pair>>;
using _Map_type  = map<void*, _Mutex_count_pair, less<void*>, _Map_alloc>;

static _Map_type _Lookup_map;
static shared_mutex _Lookup_mutex;

extern "C" _NODISCARD shared_mutex* __stdcall _Get_mutex_for_instance(void* _Ptr) noexcept {
    shared_lock _Guard(_Lookup_mutex);
    auto _Instance_mutex_iter = _Lookup_map.find(_Ptr);
    _ASSERT_EXPR(_Instance_mutex_iter != _Lookup_map.end(), "No mutex exists for given instance!");
    return &_Instance_mutex_iter->second._Mutex;
}

extern "C" _NODISCARD bool __stdcall _Acquire_mutex_for_instance(void* _Ptr) noexcept {
    try {
        scoped_lock _Guard(_Lookup_mutex);
        auto& _Refs = _Lookup_map.try_emplace(_Ptr).first->second._Ref_count;
        ++_Refs;
        return true;
    } catch (...) {
        return false;
    }
}

extern "C" void __stdcall _Release_mutex_for_instance(void* _Ptr) noexcept {
    scoped_lock _Guard(_Lookup_mutex);
    const auto _Instance_mutex_iter = _Lookup_map.find(_Ptr);
    _ASSERT_EXPR(_Instance_mutex_iter != _Lookup_map.end(), "No mutex exists for given instance!");
    auto& _Refs = _Instance_mutex_iter->second._Ref_count;
    if (--_Refs == 0) {
        _Lookup_map.erase(_Instance_mutex_iter);
    }
}

_STD_END
