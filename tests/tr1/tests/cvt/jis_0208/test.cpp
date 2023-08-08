// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "tdefs.h"
#include <cvt/jis_0208>

#define NCHARS   0x10000
#define MYWC_MAX 0x7e7e
#define MYFILE   "jis_0208"
#define MYNAME   stdext::cvt::codecvt_jis_0208<Mywchar>
#include <cvt_xtest.h>
