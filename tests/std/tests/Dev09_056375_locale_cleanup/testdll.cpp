// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <assert.h>
#include <locale>

#include <test_windows.hpp>

using namespace std;

// Tests for DLL
#ifdef _M_CEE
public
ref struct Test {
    static
#else
extern "C" {
_declspec(dllexport)
#endif

        void
        DllTest() {
        assert(isspace(' ', locale()));
        assert(isspace(L' ', locale()));
        assert(!isspace('Z', locale()));
        assert(!isspace(L'Z', locale()));
    }
};
