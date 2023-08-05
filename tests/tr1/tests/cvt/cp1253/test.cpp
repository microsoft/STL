// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#define _SILENCE_STDEXT_CVT_DEPRECATION_WARNING

#include "tdefs.h"
#include <cvt/cp1253>

#define NCHARS   0x100
#define MYWC_MAX 0xffff
#define MYFILE   "cp1253"
#define MYNAME   stdext::cvt::codecvt_cp1253<wchar_t>
#include <cvt_xtest.h>
