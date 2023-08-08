// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "tdefs.h"
#include <cvt/ebcdic>

#define NCHARS   0x100
#define MYWC_MAX 0xff
#define MYFILE   "ebcdic"
#define MYNAME   stdext::cvt::codecvt_ebcdic<Mywchar>
#include <cvt_xtest.h>
