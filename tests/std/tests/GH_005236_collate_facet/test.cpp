// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

// ensure collate<unsigned short> is valid even if wchar_t is a native type
#ifdef _ENFORCE_FACET_SPECIALIZATIONS
#undef _ENFORCE_FACET_SPECIALIZATIONS
#endif

#define _ENFORCE_FACET_SPECIALIZATIONS 0

#include <cassert>
#include <locale>
#include <string>

// skip collate::transform() tests when linking to the DLL in case of
// _ITERATOR_DEBUG_LEVEL mismatch between code and linked DLL
#ifdef _DEBUG
#define DEFAULT_IDL_SETTING 2
#else
#define DEFAULT_IDL_SETTING 0
#endif

#if defined(_DLL) && _ITERATOR_DEBUG_LEVEL != DEFAULT_IDL_SETTING
#define SKIP_COLLATE_TRANSFORM_TESTS
#endif // defined(_DLL) && _ITERATOR_DEBUG_LEVEL != DEFAULT_IDL_SETTING

using namespace std;

// circumvent error C2491 on collate<unsigned short>::id in native wchar_t build
#if defined(_DLL_CPPLIB) && defined(_NATIVE_WCHAR_T_DEFINED)
template __PURE_APPDOMAIN_GLOBAL locale::id collate<unsigned short>::id;
#endif // defined(_DLL_CPPLIB) && defined(_NATIVE_WCHAR_T_DEFINED)

// GH-5236 "std::collate<wchar_t> does not respect collation order when compiled with /MD(d) /Zc:wchar_t-"
void test_gh_5236() {
    const unsigned short Ue = L'\u00DC'; // U+00DC LATIN CAPITAL LETTER U WITH DIARESIS
    const unsigned short U  = L'U';
    const unsigned short V  = L'V';

    // German phonebook order: "U+00DC" is sorted between U and V in collation order
    locale loc("de-DE_phoneb");
    auto& coll = use_facet<collate<unsigned short>>(loc);

    assert(coll.compare(&U, &U + 1, &Ue, &Ue + 1) < 0);
    assert(coll.compare(&V, &V + 1, &Ue, &Ue + 1) > 0);

#ifndef SKIP_COLLATE_TRANSFORM_TESTS
    assert(coll.transform(&U, &U + 1) < coll.transform(&Ue, &Ue + 1));
    assert(coll.transform(&V, &V + 1) > coll.transform(&Ue, &Ue + 1));
#endif // !defined(SKIP_COLLATE_TRANSFORM_TESTS)
}

int main() {
    test_gh_5236();
}
