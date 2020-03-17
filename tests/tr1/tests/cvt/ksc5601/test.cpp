// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "tdefs.h"
#include <cvt/ksc5601>

#define NCHARS   0x10000
#define MYWC_MAX 0xffff
#define MYFILE   "ksc5601"
#define MYNAME   stdext::cvt::codecvt_ksc5601<wchar_t>
#include <cvt_xtest.h>
