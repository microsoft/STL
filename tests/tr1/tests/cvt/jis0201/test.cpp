// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "tdefs.h"
#include <cvt/jis0201>

#define NCHARS   0x100
#define MYWC_MAX 0xffff
#define MYFILE   "jis0201"
#define MYNAME   stdext::cvt::codecvt_jis0201<wchar_t>
#include <cvt_xtest.h>
