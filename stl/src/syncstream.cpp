// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

// initialize syncstream mutex map

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
static map<void*, _Mutex_count_pair> _Mutex_map{};
static shared_mutex _Mutex{};

extern "C" _CRTIMP2 shared_mutex& _Get_mutex_for_instance(void* _Ptr) {
    scoped_lock _Guard(_Mutex);
    auto _Instance_mutex_itr = _Mutex_map.find(_Ptr);
    _ASSERT_EXPR(_Instance_mutex_itr != _Mutex_map.end(), "No mutex exists for given instance!");
    return _Instance_mutex_itr->second._Mutex;
}
extern "C" _CRTIMP2 void _Add_mutex_for_instance_or_increment(void* _Ptr) noexcept {
    scoped_lock _Guard(_Mutex);
    _Mutex_map.try_emplace(_Ptr).first->second._Ref_count++;
}
extern "C" _CRTIMP2 void _Delete_mutex_for_instance_or_decrement(void* _Ptr) noexcept {
    scoped_lock _Guard(_Mutex);
    auto _Instance_mutex_itr = _Mutex_map.find(_Ptr);
    if (_Instance_mutex_itr == _Mutex_map.end()) {
        return;
    }
    if (--_Instance_mutex_itr->second._Ref_count == 0) {
        _Mutex_map.erase(_Ptr);
    }
}

_STD_END
