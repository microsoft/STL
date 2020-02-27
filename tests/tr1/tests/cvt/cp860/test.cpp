// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "tdefs.h"
#include <cvt/cp860>

#define NCHARS   0x100
#define MYWC_MAX 0xffff
#define MYFILE   "cp860"
#define MYNAME   stdext::cvt::codecvt_cp860<wchar_t>
#include <cvt_xtest.h>
