// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "tdefs.h"
#include <cvt/cp865>

#define NCHARS   0x100
#define MYWC_MAX 0xffff
#define MYFILE   "cp865"
#define MYNAME   stdext::cvt::codecvt_cp865<wchar_t>
#include <cvt_xtest.h>
