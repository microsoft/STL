// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

// pointer to raise handler

#ifndef _M_CEE_PURE
#define _HAS_EXCEPTIONS 0
#endif // _M_CEE_PURE
#include <exception>
_STD_BEGIN

_Prhand _Raise_handler = 0; // define raise handler pointer

_STD_END
