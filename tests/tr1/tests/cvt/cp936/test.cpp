// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "tdefs.h"
#include <cvt/cp936>

#define NCHARS   0x10000
#define MYWC_MAX 0xffff
#define MYFILE   "cp936"
#define MYNAME   stdext::cvt::codecvt_cp936<wchar_t>
#include <cvt_xtest.h>
