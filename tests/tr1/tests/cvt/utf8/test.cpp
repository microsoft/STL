// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "tdefs.h"
#include <cvt/utf8>

#define NCHARS   0x10000
#define MYWC_MAX 0xffff
#define MYFILE   "utf8"
#define MYNAME   stdext::cvt::codecvt_utf8<Mywchar>
#include <cvt_xtest.h>
