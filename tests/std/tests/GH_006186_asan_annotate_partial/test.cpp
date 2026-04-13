// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

// REQUIRES: x64 || x86 || arm64

#if defined(__clang__) && defined(_M_ARM64) // TRANSITION, LLVM-184902, fixed in Clang 23
#pragma comment(linker, "/INFERASANLIBS")
int main() {}
#else // ^^^ workaround / no workaround vvv

#include <cassert>
#include <vector> // include __msvc_sanitizer_annotate_container.hpp

extern "C" const bool _Asan_vector_should_annotate;
extern "C" const bool _Asan_string_should_annotate;
extern "C" const bool _Asan_optional_should_annotate;

int main() {
#ifdef TEST_ENSURE_VECTOR_ENABLED
    assert(_Asan_vector_should_annotate == true);
#else
    assert(_Asan_vector_should_annotate == false);
#endif

#ifdef TEST_ENSURE_STRING_ENABLED
    assert(_Asan_string_should_annotate == true);
#else
    assert(_Asan_string_should_annotate == false);
#endif

#ifdef TEST_ENSURE_OPTIONAL_ENABLED
    assert(_Asan_optional_should_annotate == true);
#else
    assert(_Asan_optional_should_annotate == false);
#endif
}

#endif // ^^^ no workaround ^^^
