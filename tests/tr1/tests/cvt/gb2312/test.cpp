// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "tdefs.h"
#include <cvt/gb2312>

#define NCHARS   0x10000
#define MYWC_MAX 0xffff
#define MYFILE   "gb2312"
#define MYNAME   stdext::cvt::codecvt_gb2312<wchar_t>
#include <cvt_xtest.h>
