// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "tdefs.h"
#include <cvt/jis>

#define NCHARS   0x10000
#define MYWC_MAX 0xffff
#define MYFILE   "jis"
#define MYNAME   stdext::cvt::codecvt_jis<Mywchar>
#include <cvt_xtest.h>
