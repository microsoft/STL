// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "tdefs.h"
#include <cvt/baltic>

#define NCHARS   0x100
#define MYWC_MAX 0xffff
#define MYFILE   "baltic"
#define MYNAME   stdext::cvt::codecvt_baltic<wchar_t>
#include <cvt_xtest.h>
