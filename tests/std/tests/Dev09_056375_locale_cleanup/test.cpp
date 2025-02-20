// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#define _SILENCE_CXX20_CODECVT_FACETS_DEPRECATION_WARNING
#define _SILENCE_CXX20_CODECVT_CHAR8_T_FACETS_DEPRECATION_WARNING

#include <cassert>
#include <cstdio>
#include <cwchar>
#include <locale>
#include <type_traits>

#pragma warning(push) // TRANSITION, OS-23694920
#pragma warning(disable : 4668) // 'MEOW' is not defined as a preprocessor macro, replacing with '0' for '#if/#elif'
#include <Windows.h>
#pragma warning(pop)

using namespace std;

#define STATIC_ASSERT(...) static_assert(__VA_ARGS__, #__VA_ARGS__)

STATIC_ASSERT(noexcept(locale{} == locale{})); // strengthened
STATIC_ASSERT(noexcept(locale{} != locale{})); // strengthened

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

// Test mandatory locale::id properties and strengthened exception specification.
STATIC_ASSERT(is_nothrow_default_constructible_v<locale::id>); // strengthened
STATIC_ASSERT(!is_copy_constructible_v<locale::id>);
STATIC_ASSERT(!is_move_constructible_v<locale::id>);
STATIC_ASSERT(!is_copy_assignable_v<locale::id>);
STATIC_ASSERT(!is_move_assignable_v<locale::id>);
STATIC_ASSERT(is_nothrow_destructible_v<locale::id>);

// Test that non-Standard locale::id constructor and conversion function are not user-visible.
STATIC_ASSERT(!is_constructible_v<locale::id, size_t>);
STATIC_ASSERT(!is_constructible_v<size_t, locale::id>);
STATIC_ASSERT(!is_constructible_v<size_t, locale::id&>);
STATIC_ASSERT(!is_convertible_v<size_t, locale::id>);
STATIC_ASSERT(!is_convertible_v<locale::id, size_t>);
STATIC_ASSERT(!is_convertible_v<locale::id&, size_t>);

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

#ifndef _M_CEE_PURE
locale make_unnamed_locale() {
    locale result{locale{"C"}, &use_facet<numpunct<char>>(locale{"C"})};
    assert(result.name() == "*");
    return result;
}

template <class Facet>
void test_locale_name_with_facet_pointer_one() {
    {
        locale result{locale{"C"}, static_cast<Facet*>(nullptr)};
        assert(result.name() == "C");
    }
    {
        locale result{make_unnamed_locale(), static_cast<Facet*>(nullptr)};
        assert(result.name() == "*");
    }
    {
        locale le{"C"};
        locale result{le, &use_facet<Facet>(le)};
        assert(result.name() == "*");
    }
    {
        locale lunnamed{make_unnamed_locale()};
        locale result{lunnamed, &use_facet<Facet>(lunnamed)};
        assert(result.name() == "*");
    }
}

void test_locale_name_with_facet_pointer_all() {
    test_locale_name_with_facet_pointer_one<collate<char>>();
    test_locale_name_with_facet_pointer_one<collate<wchar_t>>();

    test_locale_name_with_facet_pointer_one<ctype<char>>();
    test_locale_name_with_facet_pointer_one<ctype<wchar_t>>();
    test_locale_name_with_facet_pointer_one<codecvt<char, char, mbstate_t>>();
    test_locale_name_with_facet_pointer_one<codecvt<char16_t, char, mbstate_t>>();
    test_locale_name_with_facet_pointer_one<codecvt<char32_t, char, mbstate_t>>();
#ifdef __cpp_char8_t
    test_locale_name_with_facet_pointer_one<codecvt<char16_t, char8_t, mbstate_t>>();
    test_locale_name_with_facet_pointer_one<codecvt<char32_t, char8_t, mbstate_t>>();
#endif // __cpp_char8_t
    test_locale_name_with_facet_pointer_one<codecvt<wchar_t, char, mbstate_t>>();

    test_locale_name_with_facet_pointer_one<moneypunct<char>>();
    test_locale_name_with_facet_pointer_one<moneypunct<wchar_t>>();
    test_locale_name_with_facet_pointer_one<moneypunct<char, true>>();
    test_locale_name_with_facet_pointer_one<moneypunct<wchar_t, true>>();
    test_locale_name_with_facet_pointer_one<money_get<char>>();
    test_locale_name_with_facet_pointer_one<money_get<wchar_t>>();
    test_locale_name_with_facet_pointer_one<money_put<char>>();
    test_locale_name_with_facet_pointer_one<money_put<wchar_t>>();

    test_locale_name_with_facet_pointer_one<numpunct<char>>();
    test_locale_name_with_facet_pointer_one<numpunct<wchar_t>>();
    test_locale_name_with_facet_pointer_one<num_get<char>>();
    test_locale_name_with_facet_pointer_one<num_get<wchar_t>>();
    test_locale_name_with_facet_pointer_one<num_put<char>>();
    test_locale_name_with_facet_pointer_one<num_put<wchar_t>>();

    test_locale_name_with_facet_pointer_one<time_get<char>>();
    test_locale_name_with_facet_pointer_one<time_get<wchar_t>>();
    test_locale_name_with_facet_pointer_one<time_put<char>>();
    test_locale_name_with_facet_pointer_one<time_put<wchar_t>>();

    test_locale_name_with_facet_pointer_one<messages<char>>();
    test_locale_name_with_facet_pointer_one<messages<wchar_t>>();
}

void test_locale_name_with_another_locale_and_cats() {
    locale lc{"C"};
    locale lunnamed{make_unnamed_locale()};
    {
        locale result{lc, lc, locale::none};
        assert(result.name() != "*");
    }
    {
        locale result{lc, lunnamed, locale::none};
        assert(result.name() != "*");
    }
    {
        locale result{lunnamed, lc, locale::none};
        assert(result.name() == "*");
    }
    {
        locale result{lunnamed, lunnamed, locale::none};
        assert(result.name() == "*");
    }

    constexpr int cats_masks_count = 6; // collate | ctype | monetary | numeric | time | messages
    for (int precats = 1; precats < (1 << cats_masks_count); ++precats) {
        const locale::category cats = ((precats & (1 << 0)) != 0 ? locale::collate : locale::none)
                                    | ((precats & (1 << 1)) != 0 ? locale::ctype : locale::none)
                                    | ((precats & (1 << 2)) != 0 ? locale::monetary : locale::none)
                                    | ((precats & (1 << 3)) != 0 ? locale::numeric : locale::none)
                                    | ((precats & (1 << 4)) != 0 ? locale::time : locale::none)
                                    | ((precats & (1 << 5)) != 0 ? locale::messages : locale::none);
        {
            locale result{lc, lc, cats};
            assert(result.name() != "*");
        }
        {
            locale result{lc, lunnamed, cats};
            assert(result.name() == "*");
        }
        {
            locale result{lunnamed, lc, cats};
            assert(result.name() == "*");
        }
        {
            locale result{lunnamed, lunnamed, cats};
            assert(result.name() == "*");
        }
    }
}
#endif // _M_CEE_PURE

int main() {
    test_exe_part1();
    test_dll();
    test_exe_part2();

#ifndef _M_CEE_PURE
    // test coverage for LWG-2295
    test_locale_name_with_facet_pointer_all();
    test_locale_name_with_another_locale_and_cats();
#endif // _M_CEE_PURE
}
