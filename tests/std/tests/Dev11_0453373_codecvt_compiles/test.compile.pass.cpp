// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#define _SILENCE_CXX17_CODECVT_HEADER_DEPRECATION_WARNING

#include <codecvt>
#include <cvt/one_one>
#include <cvt/utf16>
#include <cvt/utf8>
#include <cvt/utf8_utf16>

// Regression test for DevDiv-453373 : codecvt_one_one compile errors

// Make sure we can instantiate the types from the problem headers:
template class stdext::cvt::codecvt_one_one<wchar_t, 0xffffffff, std::little_endian>;
template class stdext::cvt::codecvt_utf16<wchar_t, 0xffffffff, std::little_endian>;
template class stdext::cvt::codecvt_utf8<wchar_t, 0xffffffff, std::little_endian>;
template class stdext::cvt::codecvt_utf8_utf16<wchar_t, 0xffffffff, std::little_endian>;
