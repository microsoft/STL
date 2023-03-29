// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

// implement random_device

#include <cstdlib>
#include <xutility>

_STD_BEGIN
_CRTIMP2_PURE unsigned int __CLRCALL_PURE_OR_CDECL _Random_device() { // return a random value
    unsigned int ans;
    if (_CSTD rand_s(&ans)) {
        _Xout_of_range("invalid random_device value");
    }

    return ans;
}

_STD_END
