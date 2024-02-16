// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

// In a module-file, the optional `module;` must appear first; see [cpp.pre].
module;

// This named module expects to be built with classic headers, not header units.
#define _BUILD_STD_MODULE

// The subset of "C headers" [tab:c.headers] corresponding to
// the "C++ headers for C library facilities" [tab:headers.cpp.c]
#include <assert.h>
#include <ctype.h>
#include <errno.h>
#include <fenv.h>
#include <float.h>
#include <inttypes.h>
#include <limits.h>
#include <locale.h>
#include <math.h>
#include <setjmp.h>
#include <signal.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <uchar.h>
#include <wchar.h>
#include <wctype.h>

export module std;

#pragma warning(push)
#pragma warning(disable : 5244) // '#include <meow>' in the purview of module 'std' appears erroneous.

#include <yvals_core.h>
#ifndef _VCRT_EXPORT_STD // TRANSITION, VCRuntime update expected in 17.10 Preview 3

// N4971 [module.interface]/6: "A redeclaration of an entity X is implicitly exported
// if X was introduced by an exported declaration; otherwise it shall not be exported."

// Therefore, we need to introduce exported declarations of <vcruntime_new.h> machinery before including it.

#pragma pack(push, _CRT_PACKING)
#pragma warning(push, _STL_WARNING_LEVEL)
#pragma warning(disable : _STL_DISABLED_WARNINGS)
_STL_DISABLE_CLANG_WARNINGS
#pragma push_macro("new")
#undef new

_STD_BEGIN
export extern "C++" struct nothrow_t;

export extern "C++" const nothrow_t nothrow;

#ifdef __cpp_aligned_new
export extern "C++" enum class align_val_t : size_t;
#endif // ^^^ defined(__cpp_aligned_new) ^^^
_STD_END

export extern "C++" _NODISCARD _Ret_notnull_ _Post_writable_byte_size_(_Size)
_VCRT_ALLOCATOR void* __CRTDECL operator new(size_t _Size);
export extern "C++" _NODISCARD _Ret_maybenull_ _Success_(return != NULL)
    _Post_writable_byte_size_(_Size) _VCRT_ALLOCATOR void* __CRTDECL
    operator new(size_t _Size, const _STD nothrow_t&) noexcept;
export extern "C++" _NODISCARD _Ret_notnull_ _Post_writable_byte_size_(_Size)
_VCRT_ALLOCATOR void* __CRTDECL operator new[](size_t _Size);
export extern "C++" _NODISCARD _Ret_maybenull_ _Success_(return != NULL)
    _Post_writable_byte_size_(_Size) _VCRT_ALLOCATOR void* __CRTDECL
    operator new[](size_t _Size, const _STD nothrow_t&) noexcept;
export extern "C++" void __CRTDECL operator delete(void* _Block) noexcept;
export extern "C++" void __CRTDECL operator delete(void* _Block, size_t _Size) noexcept;
export extern "C++" void __CRTDECL operator delete(void* _Block, const _STD nothrow_t&) noexcept;
export extern "C++" void __CRTDECL operator delete[](void* _Block) noexcept;
export extern "C++" void __CRTDECL operator delete[](void* _Block, size_t _Size) noexcept;
export extern "C++" void __CRTDECL operator delete[](void* _Block, const _STD nothrow_t&) noexcept;

#ifdef __cpp_aligned_new
export extern "C++" _NODISCARD _Ret_notnull_ _Post_writable_byte_size_(_Size)
_VCRT_ALLOCATOR void* __CRTDECL operator new(size_t _Size, _STD align_val_t _Al);
export extern "C++" _NODISCARD _Ret_maybenull_ _Success_(return != NULL)
    _Post_writable_byte_size_(_Size) _VCRT_ALLOCATOR void* __CRTDECL
    operator new(size_t _Size, _STD align_val_t _Al, const _STD nothrow_t&) noexcept;
export extern "C++" _NODISCARD _Ret_notnull_ _Post_writable_byte_size_(_Size)
_VCRT_ALLOCATOR void* __CRTDECL operator new[](size_t _Size, _STD align_val_t _Al);
export extern "C++" _NODISCARD _Ret_maybenull_ _Success_(return != NULL)
    _Post_writable_byte_size_(_Size) _VCRT_ALLOCATOR void* __CRTDECL
    operator new[](size_t _Size, _STD align_val_t _Al, const _STD nothrow_t&) noexcept;
export extern "C++" void __CRTDECL operator delete(void* _Block, _STD align_val_t _Al) noexcept;
export extern "C++" void __CRTDECL operator delete(void* _Block, size_t _Size, _STD align_val_t _Al) noexcept;
export extern "C++" void __CRTDECL operator delete(void* _Block, _STD align_val_t _Al, const _STD nothrow_t&) noexcept;
export extern "C++" void __CRTDECL operator delete[](void* _Block, _STD align_val_t _Al) noexcept;
export extern "C++" void __CRTDECL operator delete[](void* _Block, size_t _Size, _STD align_val_t _Al) noexcept;
export extern "C++" void __CRTDECL operator delete[](
    void* _Block, _STD align_val_t _Al, const _STD nothrow_t&) noexcept;
#endif // ^^^ defined(__cpp_aligned_new) ^^^

export extern "C++" _NODISCARD _MSVC_CONSTEXPR _Ret_notnull_ _Post_writable_byte_size_(_Size)
    _Post_satisfies_(return == _Where) void* __CRTDECL
    operator new(size_t _Size, _Writable_bytes_(_Size) void* _Where) noexcept;
export extern "C++" _NODISCARD _Ret_notnull_ _Post_writable_byte_size_(_Size)
    _Post_satisfies_(return == _Where) void* __CRTDECL
    operator new[](size_t _Size, _Writable_bytes_(_Size) void* _Where) noexcept;
export extern "C++" void __CRTDECL operator delete(void*, void*) noexcept;
export extern "C++" void __CRTDECL operator delete[](void*, void*) noexcept;

#pragma pop_macro("new")
_STL_RESTORE_CLANG_WARNINGS
#pragma warning(pop)
#pragma pack(pop)

#endif // ^^^ workaround ^^^

// "C++ library headers" [tab:headers.cpp]
#include <algorithm>
#if _HAS_STATIC_RTTI
#include <any>
#endif // _HAS_STATIC_RTTI
#include <array>
#include <atomic>
#include <barrier>
#include <bit>
#include <bitset>
#include <charconv>
#include <chrono>
#include <codecvt>
#include <compare>
#include <complex>
#include <concepts>
#include <condition_variable>
#include <coroutine>
#include <deque>
#include <exception>
#include <execution>
#if _HAS_CXX23
#include <expected>
#endif // _HAS_CXX23
#include <filesystem>
#include <format>
#include <forward_list>
#include <fstream>
#include <functional>
#include <future>
#include <initializer_list>
#include <iomanip>
#include <ios>
#include <iosfwd>
#include <iostream>
#include <istream>
#include <iterator>
#include <latch>
#include <limits>
#include <list>
#include <locale>
#include <map>
#if _HAS_CXX23
#include <mdspan>
#endif // _HAS_CXX23
#include <memory>
#include <memory_resource>
#include <mutex>
#include <new>
#include <numbers>
#include <numeric>
#include <optional>
#include <ostream>
#if _HAS_CXX23
#include <print>
#endif // _HAS_CXX23
#include <queue>
#include <random>
#include <ranges>
#include <ratio>
#include <regex>
#include <scoped_allocator>
#include <semaphore>
#include <set>
#include <shared_mutex>
#include <source_location>
#include <span>
#if _HAS_CXX23
#include <spanstream>
#endif // _HAS_CXX23
#include <sstream>
#include <stack>
#if _HAS_CXX23
#include <stacktrace>
#endif // _HAS_CXX23
#include <stdexcept>
#if _HAS_CXX23
#include <stdfloat>
#endif // _HAS_CXX23
#include <stop_token>
#include <streambuf>
#include <string>
#include <string_view>
#include <strstream>
#include <syncstream>
#include <system_error>
#include <thread>
#include <tuple>
#include <type_traits>
#include <typeindex>
#include <typeinfo>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <valarray>
#include <variant>
#include <vector>
#include <version>

// "C++ headers for C library facilities" [tab:headers.cpp.c]
#include <cassert>
#include <cctype>
#include <cerrno>
#include <cfenv>
#include <cfloat>
#include <cinttypes>
#include <climits>
#include <clocale>
#include <cmath>
#include <csetjmp>
#include <csignal>
#include <cstdarg>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <cuchar>
#include <cwchar>
#include <cwctype>

#pragma warning(pop)
