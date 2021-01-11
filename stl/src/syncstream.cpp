// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

// initialize syncstream mutex map

#include <syncstream>

#ifdef __cpp_lib_syncbuf
#pragma warning(disable : 4074)
#pragma init_seg(compiler)

_STD_BEGIN
// OBJECT DECLARATIONS
static map<void*, _Mutex_count_pair> _Mutex_map{};
static shared_mutex _Mutex{};

shared_mutex& _Get_map_mutex() {
    return _Mutex;
}

map<void*, _Mutex_count_pair>& _Get_mutex_map() {
    return _Mutex_map;
}

_STD_END
#endif
