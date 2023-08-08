// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "tdefs.h"
#include <cvt/cp862>

#define NCHARS   0x100
#define MYWC_MAX 0xffff
#define MYFILE   "cp862"
#define MYNAME   stdext::cvt::codecvt_cp862<wchar_t>
#include <cvt_xtest.h>
