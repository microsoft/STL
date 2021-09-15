// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

// This file is built into msvcurt.lib
#if !defined(_M_CEE_PURE)
#error This file must be built with /clr:pure.
#endif

#include <iostream>

#pragma warning(disable : 4074)
#pragma init_seg(compiler)

_STD_BEGIN

__PURE_APPDOMAIN_GLOBAL extern istream* _Ptr_cin  = nullptr;
__PURE_APPDOMAIN_GLOBAL extern ostream* _Ptr_cout = nullptr;
__PURE_APPDOMAIN_GLOBAL extern ostream* _Ptr_cerr = nullptr;
__PURE_APPDOMAIN_GLOBAL extern ostream* _Ptr_clog = nullptr;

__PURE_APPDOMAIN_GLOBAL extern wistream* _Ptr_wcin  = nullptr;
__PURE_APPDOMAIN_GLOBAL extern wostream* _Ptr_wcout = nullptr;
__PURE_APPDOMAIN_GLOBAL extern wostream* _Ptr_wcerr = nullptr;
__PURE_APPDOMAIN_GLOBAL extern wostream* _Ptr_wclog = nullptr;

__PURE_APPDOMAIN_GLOBAL int ios_base::Init::_Init_cnt = -1;
_STD_END
