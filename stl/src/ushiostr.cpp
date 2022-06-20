// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

// _UShinit members (unsigned short version)

#include <fstream>

_STD_BEGIN

using ushistream = basic_istream<unsigned short, char_traits<unsigned short>>;
using ushostream = basic_ostream<unsigned short, char_traits<unsigned short>>;
using ushfilebuf = basic_filebuf<unsigned short, char_traits<unsigned short>>;

_STD_END

#define wistream ushistream
#define wostream ushostream
#define wfilebuf ushfilebuf
#define _Winit   _UShinit

#include <iostream>

#if defined(_M_CEE_PURE)
_STD_BEGIN
__PURE_APPDOMAIN_GLOBAL extern wistream* _Ptr_wcin  = nullptr;
__PURE_APPDOMAIN_GLOBAL extern wostream* _Ptr_wcout = nullptr;
__PURE_APPDOMAIN_GLOBAL extern wostream* _Ptr_wcerr = nullptr;
__PURE_APPDOMAIN_GLOBAL extern wostream* _Ptr_wclog = nullptr;
_STD_END
#else
_STD_BEGIN
__PURE_APPDOMAIN_GLOBAL extern _CRTDATA2_IMPORT wistream* _Ptr_wcin  = nullptr;
__PURE_APPDOMAIN_GLOBAL extern _CRTDATA2_IMPORT wostream* _Ptr_wcout = nullptr;
__PURE_APPDOMAIN_GLOBAL extern _CRTDATA2_IMPORT wostream* _Ptr_wcerr = nullptr;
__PURE_APPDOMAIN_GLOBAL extern _CRTDATA2_IMPORT wostream* _Ptr_wclog = nullptr;
_STD_END
#include "wiostrea.cpp"
#endif
