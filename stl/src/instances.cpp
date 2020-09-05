// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

// force DLL instances

#define __FORCE_INSTANCE

#include <ios>
#include <istream>
#include <locale>
#include <ostream>
#include <streambuf>
#include <xlocale>
#include <xlocmes>
#include <xlocmon>
#include <xlocnum>
#include <xloctime>

_STD_BEGIN

template class _CRTIMP2_PURE_IMPORT _Yarn<char>;
template class _CRTIMP2_PURE_IMPORT _Yarn<wchar_t>;
template class _CRTIMP2_PURE_IMPORT _Yarn<unsigned short>;

_STD_END
