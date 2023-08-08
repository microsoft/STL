// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "tdefs.h"
#include <cvt/euc>

#define NCHARS   0x10000
#define MYWC_MAX 0xffff
#define MYFILE   "euc"
#define MYNAME   stdext::cvt::codecvt_euc<wchar_t>
#include <cvt_xtest.h>
