// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "tdefs.h"
#include <cvt/iceland>

#define NCHARS   0x100
#define MYWC_MAX 0xffff
#define MYFILE   "iceland"
#define MYNAME   stdext::cvt::codecvt_iceland<wchar_t>
#include <cvt_xtest.h>
