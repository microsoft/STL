// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "tdefs.h"
#include <cvt/cp856>

#define NCHARS   0x100
#define MYWC_MAX 0xffff
#define MYFILE   "cp856"
#define MYNAME   stdext::cvt::codecvt_cp856<wchar_t>
#include <cvt_xtest.h>
