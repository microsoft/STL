// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

// ios::Init members

#include <atomic>
#include <iostream>
_STD_BEGIN

static std::atomic<int> _Init_cnt{0};

_CRTIMP2_PURE void __cdecl ios_base::Init::_Init_ctor(ios_base::Init*) { // initialize standard streams first time
    ++_Init_cnt;
}

_CRTIMP2_PURE void __cdecl ios_base::Init::_Init_dtor(ios_base::Init*) { // flush standard streams last time
    if (--_Init_cnt == 0) { // flush standard streams
        if (_Ptr_cerr != nullptr) {
            _Ptr_cerr->flush();
        }

        if (_Ptr_clog != nullptr) {
            _Ptr_clog->flush();
        }

        if (_Ptr_cout != nullptr) {
            _Ptr_cout->flush();
        }
    }
}
_STD_END
