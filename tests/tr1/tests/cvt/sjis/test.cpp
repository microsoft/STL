// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#define _SILENCE_STDEXT_CVT_DEPRECATION_WARNING

#include "tdefs.h"
#include <cvt/sjis>

#define NCHARS   0x10000
#define MYWC_MAX 0xffff
#define MYFILE   "sjis"
#define MYNAME   stdext::cvt::codecvt_sjis<wchar_t>
#include <cvt_xtest.h>
