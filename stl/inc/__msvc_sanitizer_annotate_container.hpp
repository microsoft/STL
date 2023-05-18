// __msvc_sanitizer_annotate_container.hpp internal header

// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#pragma once
#ifndef __MSVC_SANITIZER_ANNOTATE_CONTAINER_HPP
#define __MSVC_SANITIZER_ANNOTATE_CONTAINER_HPP
#include <yvals_core.h>
#if _STL_COMPILER_PREPROCESSOR

#pragma pack(push, _CRT_PACKING)
#pragma warning(push, _STL_WARNING_LEVEL)
#pragma warning(disable : _STL_DISABLED_WARNINGS)
_STL_DISABLE_CLANG_WARNINGS
#pragma push_macro("new")
#undef new

#if !defined(_M_CEE_PURE) && !(defined(_DISABLE_STRING_ANNOTATION) && defined(_DISABLE_VECTOR_ANNOTATION))

#ifdef __SANITIZE_ADDRESS__

#define _ACTIVATE_STRING_ANNOTATION
#define _INSERT_STRING_ANNOTATION
#define _ACTIVATE_VECTOR_ANNOTATION
#define _INSERT_VECTOR_ANNOTATION

#elif defined(__clang__) // ^^^ __SANITIZE_ADDRESS__ / __clang__ vvv

#if __has_feature(address_sanitizer)
#define _ACTIVATE_STRING_ANNOTATION
#define _INSERT_STRING_ANNOTATION
#define _ACTIVATE_VECTOR_ANNOTATION
#define _INSERT_VECTOR_ANNOTATION
#pragma comment(linker, "/INFERASANLIBS")
#endif // __has_feature(address_sanitizer)

#else // ^^^ __clang__ / !__clang__ && !__SANITIZE_ADDRESS__ vvv

#ifdef _ANNOTATE_STRING
#define _INSERT_STRING_ANNOTATION
#endif // _ANNOTATE_STRING
#ifdef _ANNOTATE_VECTOR
#define _INSERT_VECTOR_ANNOTATION
#endif // _ANNOTATE_VECTOR

#endif // __SANITIZE_ADDRESS__

#ifdef _DISABLE_STRING_ANNOTATION
#undef _ACTIVATE_STRING_ANNOTATION
#undef _INSERT_STRING_ANNOTATION
#endif // _DISABLE_STRING_ANNOTATION
#ifdef _DISABLE_VECTOR_ANNOTATION
#undef _ACTIVATE_VECTOR_ANNOTATION
#undef _INSERT_VECTOR_ANNOTATION
#endif // _DISABLE_VECTOR_ANNOTATION

#ifndef _INSERT_STRING_ANNOTATION
#pragma detect_mismatch("annotate_string", "0")
#endif // !_INSERT_STRING_ANNOTATION
#ifndef _INSERT_VECTOR_ANNOTATION
#pragma detect_mismatch("annotate_vector", "0")
#endif // !_INSERT_VECTOR_ANNOTATION

#ifdef _ACTIVATE_STRING_ANNOTATION
#pragma comment(lib, "stl_asan")
#pragma detect_mismatch("annotate_string", "1")
#endif // _ACTIVATE_STRING_ANNOTATION
#ifdef _ACTIVATE_VECTOR_ANNOTATION
#pragma comment(lib, "stl_asan")
#pragma detect_mismatch("annotate_vector", "1")
#endif // _ACTIVATE_VECTOR_ANNOTATION

#undef _ACTIVATE_STRING_ANNOTATION
#undef _ACTIVATE_VECTOR_ANNOTATION

extern "C" {
#ifdef _INSERT_VECTOR_ANNOTATION
extern const bool _Asan_vector_should_annotate;
#endif

#ifdef _INSERT_STRING_ANNOTATION
extern const bool _Asan_string_should_annotate;
#endif
}

#if defined(_INSERT_VECTOR_ANNOTATION) || defined(_INSERT_STRING_ANNOTATION)
extern "C" {
void __cdecl __sanitizer_annotate_contiguous_container(
    const void* _First, const void* _End, const void* _Old_last, const void* _New_last);
}

#ifdef _M_ARM64EC
#pragma comment(linker, \
        "/alternatename:#__sanitizer_annotate_contiguous_container=#__sanitizer_annotate_contiguous_container_default")
#pragma comment(linker, \
        "/alternatename:__sanitizer_annotate_contiguous_container=__sanitizer_annotate_contiguous_container_default")
#pragma comment(linker, "/alternatename:#_Asan_vector_should_annotate=#_Asan_vector_should_annotate_default")
#pragma comment(linker, "/alternatename:_Asan_vector_should_annotate=_Asan_vector_should_annotate_default")
#pragma comment(linker, "/alternatename:#_Asan_string_should_annotate=#_Asan_string_should_annotate_default")
#pragma comment(linker, "/alternatename:_Asan_string_should_annotate=_Asan_string_should_annotate_default")
#elif defined(_M_HYBRID)
#pragma comment(linker, \
        "/alternatename:#__sanitizer_annotate_contiguous_container=#__sanitizer_annotate_contiguous_container_default")
#pragma comment(linker, \
        "/alternatename:___sanitizer_annotate_contiguous_container=___sanitizer_annotate_contiguous_container_default")
#pragma comment(linker, "/alternatename:#_Asan_vector_should_annotate=#_Asan_vector_should_annotate_default")
#pragma comment(linker, "/alternatename:__Asan_vector_should_annotate=__Asan_vector_should_annotate_default")
#pragma comment(linker, "/alternatename:#_Asan_string_should_annotate=#_Asan_string_should_annotate_default")
#pragma comment(linker, "/alternatename:__Asan_string_should_annotate=__Asan_string_should_annotate_default")
#elif defined(_M_IX86)
#pragma comment(linker, \
        "/alternatename:___sanitizer_annotate_contiguous_container=___sanitizer_annotate_contiguous_container_default")
#pragma comment(linker, "/alternatename:__Asan_vector_should_annotate=__Asan_vector_should_annotate_default")
#pragma comment(linker, "/alternatename:__Asan_string_should_annotate=__Asan_string_should_annotate_default")
#elif defined(_M_X64) || defined(_M_ARM) || defined(_M_ARM64)
#pragma comment(linker, \
        "/alternatename:__sanitizer_annotate_contiguous_container=__sanitizer_annotate_contiguous_container_default")
#pragma comment(linker, "/alternatename:_Asan_vector_should_annotate=_Asan_vector_should_annotate_default")
#pragma comment(linker, "/alternatename:_Asan_string_should_annotate=_Asan_string_should_annotate_default")
#else // ^^^ known architecture / unknown architecture vvv
#error Unknown architecture
#endif // ^^^ unknown architecture ^^^

#endif // insert asan annotations

#endif // !_M_CEE_PURE && asan not disabled

#pragma pop_macro("new")
_STL_RESTORE_CLANG_WARNINGS
#pragma warning(pop)
#pragma pack(pop)

#endif // _STL_COMPILER_PREPROCESSOR
#endif // __MSVC_SANITIZER_ANNOTATE_CONTAINER_HPP
