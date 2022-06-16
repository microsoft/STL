// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

// initialize standard wide error stream (unsigned short version)

#include <fstream>

_STD_BEGIN

using ushistream = basic_istream<unsigned short, char_traits<unsigned short>>;
using ushostream = basic_ostream<unsigned short, char_traits<unsigned short>>;
using ushfilebuf = basic_filebuf<unsigned short, char_traits<unsigned short>>;

_STD_END

#define wistream   ushistream
#define wostream   ushostream
#define wfilebuf   ushfilebuf
#define _Init_wcin _Init_ushcin
#define _Winit     _UShinit
#define init_wcin  init_ushcin

#include "wcin.cpp"
#include <iostream>
