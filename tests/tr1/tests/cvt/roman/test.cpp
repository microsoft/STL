// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "tdefs.h"
#include <cvt/roman>

#define NCHARS   0x100
#define MYWC_MAX 0xffff
#define MYFILE   "roman"
#define MYNAME   stdext::cvt::codecvt_roman<wchar_t>
#include <cvt_xtest.h>
