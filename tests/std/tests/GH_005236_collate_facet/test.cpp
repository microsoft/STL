// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

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

// GH-5236 "std::collate<wchar_t> does not respect collation order when compiled with /MD(d) /Zc:wchar_t-"
void test_gh_5236() {
    const wchar_t Ue = L'\u00DC'; // U+00DC LATIN CAPITAL LETTER U WITH DIARESIS
    const wchar_t U  = L'U';
    const wchar_t V  = L'V';

    // German phonebook order: "U+00DC" is sorted between U and V in collation order
    const locale loc("de-DE_phoneb");
    const auto& coll = use_facet<collate<wchar_t>>(loc);

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
