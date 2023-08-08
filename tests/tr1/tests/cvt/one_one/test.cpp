// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "tdefs.h"
#include <cvt/one_one>

#define NCHARS   0x10000
#define MYWC_MAX 0xffff
#define MYFILE   "one_one"
#define MYNAME   stdext::cvt::codecvt_one_one<Mywchar>
#include <cvt_xtest.h>
