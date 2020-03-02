// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "tdefs.h"
#include <cvt/8859_5>

#define NCHARS   0x100
#define MYWC_MAX 0xffff
#define MYFILE   "8859_5"
#define MYNAME   stdext::cvt::codecvt_8859_5<wchar_t>
#include <cvt_xtest.h>
