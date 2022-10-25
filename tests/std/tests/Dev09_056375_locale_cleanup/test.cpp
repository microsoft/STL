// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <cassert>
#include <cstdio>
#include <locale>

#include <Windows.h>

using namespace std;

#define STATIC_ASSERT(...) static_assert(__VA_ARGS__, #__VA_ARGS__)

void test_dll() {
    puts("Calling dll");
#ifdef _M_CEE
    using namespace System;
    using namespace System::Reflection;
    {
        AppDomain ^ ad = AppDomain::CreateDomain("NewDomain");
        Assembly ^ as  = ad->Load("testdll");
        as->GetType("Test")->GetMethod("DllTest")->Invoke(nullptr, nullptr);
        AppDomain::Unload(ad);
    }
#else
    HMODULE hLibrary = LoadLibraryExW(L"testdll.dll", nullptr, 0);
    assert(hLibrary != nullptr);
    typedef void (*TheFuncProc)();
    TheFuncProc pFunc = (TheFuncProc) GetProcAddress(hLibrary, "DllTest");
    assert(pFunc != nullptr);
    pFunc();
    FreeLibrary(hLibrary);
#endif
}

void test_exe_part1() {
    assert(isspace(' ', locale()));
    assert(isspace(L' ', locale()));
    assert(!isspace('Z', locale()));
    assert(!isspace(L'Z', locale()));
}

void test_exe_part2() {
    assert(isspace(' ', locale()));
    assert(isspace(L' ', locale()));
    assert(!isspace('Z', locale()));
    assert(!isspace(L'Z', locale()));
}

int main() {
#ifdef _M_CEE
    using namespace System;
#endif
    test_exe_part1();
    test_dll();
    test_exe_part2();
}
