// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

// _UShinit members (unsigned short version)

#ifdef _NATIVE_WCHAR_T_DEFINED
#include <fstream>

#define wistream ushistream
#define wostream ushostream
#define wfilebuf ushfilebuf
#define _Init_wcerr _Init_ushcerr
#define _Init_wcout _Init_ushcout
#define _Init_wclog _Init_ushclog
#define _Init_wcin _Init_ushcin
#define _Winit _UShinit

#include <iostream>

#if defined(_M_CEE_PURE)
_STD_BEGIN
__PURE_APPDOMAIN_GLOBAL extern wistream* _Ptr_wcin  = 0;
__PURE_APPDOMAIN_GLOBAL extern wostream* _Ptr_wcout = 0;
__PURE_APPDOMAIN_GLOBAL extern wostream* _Ptr_wcerr = 0;
__PURE_APPDOMAIN_GLOBAL extern wostream* _Ptr_wclog = 0;
_STD_END
#else
_STD_BEGIN
__PURE_APPDOMAIN_GLOBAL extern _CRTDATA2_IMPORT wistream* _Ptr_wcin  = 0;
__PURE_APPDOMAIN_GLOBAL extern _CRTDATA2_IMPORT wostream* _Ptr_wcout = 0;
__PURE_APPDOMAIN_GLOBAL extern _CRTDATA2_IMPORT wostream* _Ptr_wcerr = 0;
__PURE_APPDOMAIN_GLOBAL extern _CRTDATA2_IMPORT wostream* _Ptr_wclog = 0;
_STD_END
#include "wiostrea.cpp"
#endif
#endif
