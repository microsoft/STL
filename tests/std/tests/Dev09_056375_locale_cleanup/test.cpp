// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <cassert>
#include <cstdio>
#include <locale>

#include <Windows.h>

using namespace std;

#define STATIC_ASSERT(...) static_assert(__VA_ARGS__, #__VA_ARGS__)

STATIC_ASSERT(noexcept(locale{} == locale{}));
STATIC_ASSERT(noexcept(locale{} != locale{}));

STATIC_ASSERT(is_nothrow_default_constructible_v<ctype_base>); // strengthened
STATIC_ASSERT(is_nothrow_default_constructible_v<codecvt_base>); // strengthened
STATIC_ASSERT(is_nothrow_default_constructible_v<time_base>); // strengthened
STATIC_ASSERT(is_nothrow_default_constructible_v<money_base>); // strengthened
STATIC_ASSERT(is_nothrow_default_constructible_v<messages_base>); // strengthened

// Test that *_base classes are implicitly default constructible.

template <class T>
void parameter_taker(const T&); // not defined

template <class T, class = void>
constexpr bool is_implicitly_default_constructible = false;

template <class T>
constexpr bool is_implicitly_default_constructible<T, void_t<decltype(parameter_taker<T>({}))>> = true;

STATIC_ASSERT(is_implicitly_default_constructible<ctype_base>);
STATIC_ASSERT(is_implicitly_default_constructible<codecvt_base>);
STATIC_ASSERT(is_implicitly_default_constructible<time_base>);
STATIC_ASSERT(is_implicitly_default_constructible<money_base>);
STATIC_ASSERT(is_implicitly_default_constructible<messages_base>);

STATIC_ASSERT(!is_implicitly_default_constructible<locale::facet>);
STATIC_ASSERT(!is_implicitly_default_constructible<ctype<char>>);
STATIC_ASSERT(!is_implicitly_default_constructible<ctype<wchar_t>>);

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
#else // ^^^ defined(_M_CEE) / !defined(_M_CEE) vvv
    HMODULE hLibrary = LoadLibraryExW(L"testdll.dll", nullptr, 0);
    assert(hLibrary != nullptr);
    typedef void (*TheFuncProc)();
    TheFuncProc pFunc = reinterpret_cast<TheFuncProc>(GetProcAddress(hLibrary, "DllTest"));
    assert(pFunc != nullptr);
    pFunc();
    FreeLibrary(hLibrary);
#endif // ^^^ !defined(_M_CEE) ^^^
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
    test_exe_part1();
    test_dll();
    test_exe_part2();
}
