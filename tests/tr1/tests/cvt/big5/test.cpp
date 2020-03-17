// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "tdefs.h"
#include <cvt/big5>

#define NCHARS   0x10000
#define MYWC_MAX 0xffff
#define MYFILE   "big5"
#define MYNAME   stdext::cvt::codecvt_big5<wchar_t>
#include <cvt_xtest.h>
