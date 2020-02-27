// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "tdefs.h"
#include <cvt/utf16>

#define NCHARS   0x10000
#define MYWC_MAX 0x10ffff
#define MYFILE   "utf16"
#define MYNAME   stdext::cvt::codecvt_utf16<Mywchar>
#include <cvt_xtest.h>
