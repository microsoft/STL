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
    using size_type                              = size_t;
    using difference_type                        = ptrdiff_t;
    using propagate_on_container_move_assignment = true_type;
    using is_always_equal                        = true_type;

    constexpr _Crt_allocator() noexcept {}

    constexpr _Crt_allocator(const _Crt_allocator&) noexcept = default;
    template <class _Other>
    constexpr _Crt_allocator(const _Crt_allocator<_Other>&) noexcept {}

    _NODISCARD __declspec(allocator) _Ty* allocate(_CRT_GUARDOVERFLOW const size_t _Count) {
        return static_cast<_Ty*>(_calloc_crt(_Count, sizeof(_Ty)));
    }

    void deallocate(_Ty* const _Ptr, const size_t) {
        _free_crt(_Ptr);
    }
};

static map<void*, _Mutex_count_pair, less<void*>, _Crt_allocator<pair<void* const, _Mutex_count_pair>>> _Mutex_map;
static shared_mutex _Mutex;

extern "C" _CRTIMP2 shared_mutex& _Get_mutex_for_instance(void* _Ptr) {
    shared_lock _Guard(_Mutex);
    auto _Instance_mutex_iter = _Mutex_map.find(_Ptr);
    _ASSERT_EXPR(_Instance_mutex_iter != _Mutex_map.end(), "No mutex exists for given instance!");
    return _Instance_mutex_iter->second._Mutex;
}
extern "C" _CRTIMP2 void _Acquire_mutex_for_instance(void* _Ptr) noexcept {
    scoped_lock _Guard(_Mutex);
    _Mutex_map.try_emplace(_Ptr).first->second._Ref_count++;
}
extern "C" _CRTIMP2 void _Release_mutex_for_instance(void* _Ptr) noexcept {
    scoped_lock _Guard(_Mutex);
    auto _Instance_mutex_iter = _Mutex_map.find(_Ptr);
    _ASSERT_EXPR(_Instance_mutex_iter != _Mutex_map.end(), "No mutex exists for given instance!");
    if (--_Instance_mutex_iter->second._Ref_count == 0) {
        _Mutex_map.erase(_Ptr);
    }
}

_STD_END
