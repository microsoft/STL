// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

// initialize standard wide log stream (unsigned short version)

#include <fstream>

_STD_BEGIN

using ushistream = basic_istream<unsigned short, char_traits<unsigned short>>;
using ushostream = basic_ostream<unsigned short, char_traits<unsigned short>>;
using ushfilebuf = basic_filebuf<unsigned short, char_traits<unsigned short>>;

_STD_END

#define wistream    ushistream
#define wostream    ushostream
#define wfilebuf    ushfilebuf
#define _Init_wclog _Init_ushclog
#define init_wclog  init_ushclog
#define _Winit      _UShinit

#include "wclog.cpp"
