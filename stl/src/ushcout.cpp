// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

// initialize standard wide error stream (unsigned short version)

#ifdef _NATIVE_WCHAR_T_DEFINED
#include <fstream>

#ifndef wistream
#define wistream ushistream
#define wostream ushostream
#define wfilebuf ushfilebuf
#define _Init_wcerr _Init_ushcerr
#define _Init_wcout _Init_ushcout
#define _Init_wclog _Init_ushclog
#define _Init_wcin _Init_ushcin
#define _Winit _UShinit
#define init_wcerr init_ushcerr
#define init_wcout init_ushcout
#define init_wclog init_ushclog
#define init_wcin init_ushcin
#endif

#include "wcout.cpp"
#include <iostream>
#endif
