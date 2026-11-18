// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

// This file is intended as a test resource for tools that want to verify that they can parse all MSVC standard
// library headers without warnings. This file disables deprecations, so it should not be included in programs directly.
//
// This file may be changed, renamed, or removed at any time.

#ifndef __MSVC_ALL_PUBLIC_HEADERS_HPP
#define __MSVC_ALL_PUBLIC_HEADERS_HPP

#pragma warning(push)
#pragma warning(1 : 4668) // 'MEOW' is not defined as a preprocessor macro, replacing with '0' for '#if/#elif'

// All STL headers should protect themselves from macroized new.
#if !(defined(__CUDACC__) && defined(__clang__))
#pragma push_macro("new")
#undef new
#define new WILL NOT COMPILE
#endif // !(defined(__CUDACC__) && defined(__clang__))

// VSO-768746: mbctype.h macroizes _MS, _MP, _M1, and _M2. Include it first for test coverage.
#ifndef _MSVC_TESTING_NVCC
#include <mbctype.h>
#endif // !defined(_MSVC_TESTING_NVCC)

#ifndef _SILENCE_CXX17_C_HEADER_DEPRECATION_WARNING
#define _SILENCE_CXX17_C_HEADER_DEPRECATION_WARNING
#endif // !defined(_SILENCE_CXX17_C_HEADER_DEPRECATION_WARNING)

#define _SILENCE_CXX20_CISO646_REMOVED_WARNING

// Core STL Headers
#include <initializer_list>
#include <limits>
#include <ratio>
#include <tuple>
#include <type_traits>
#include <utility>
#include <version>

#ifdef __cpp_lib_coroutine // C++20 or later (or /await:strict in C++14/17)
#include <coroutine>
#endif // defined(__cpp_lib_coroutine)

#if _HAS_CXX20
#include <bit>
#include <compare>
#include <concepts>
#include <numbers>
#include <source_location>
#endif // _HAS_CXX20

#if _HAS_CXX23
#include <stdfloat>
#endif // _HAS_CXX23

// Core C Wrapper Headers
#include <cassert>
#include <cctype>
#include <cerrno>
#include <cfenv>
#include <cfloat>
#include <cinttypes>
#include <climits>
#include <clocale>
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

#ifndef _CORE_HEADERS_ONLY

// Non-Core STL Headers
#include <algorithm>
#include <array>
#include <bitset>
#include <chrono>
#include <codecvt>
#include <complex>
#include <deque>
#include <exception>
#include <forward_list>
#include <fstream>
#include <functional>
#include <iomanip>
#include <ios>
#include <iosfwd>
#include <iostream>
#include <istream>
#include <iterator>
#include <list>
#include <locale>
#include <map>
#include <memory>
#include <new>
#include <numeric>
#include <ostream>
#include <queue>
#include <random>
#include <regex>
#include <scoped_allocator>
#include <set>
#include <sstream>
#include <stack>
#include <stdexcept>
#include <streambuf>
#include <string>
#include <strstream>
#include <system_error>
#include <typeindex>
#include <typeinfo>
#include <unordered_map>
#include <unordered_set>
#include <valarray>
#include <vector>

#if _HAS_CXX17
#include <any>
#include <charconv>
#include <filesystem>
#include <memory_resource>
#include <optional>
#include <string_view>
#include <variant>
#endif // _HAS_CXX17

#if _HAS_CXX20
#include <format>
#include <ranges>
#include <span>
#include <syncstream>
#endif // _HAS_CXX20

#if _HAS_CXX23
#include <expected>
#include <flat_map>
#include <flat_set>
#include <generator>
#include <mdspan>
#include <print>
#include <spanstream>
#include <stacktrace>
#endif // _HAS_CXX23

#ifndef _M_CEE_PURE
#include <atomic>
#include <condition_variable>
#include <future>
#include <mutex>
#include <shared_mutex>
#include <thread>
#if _HAS_CXX17
#include <execution>
#endif // _HAS_CXX17
#if _HAS_CXX20
#include <barrier>
#include <latch>
#include <semaphore>
#include <stop_token>
#endif // _HAS_CXX20
#if _HAS_CXX23
#include <__msvc_cxx_stdatomic.hpp>
#endif // _HAS_CXX23
#endif // !defined(_M_CEE_PURE)

// Non-Core C Wrapper Headers
#include <ccomplex>
#include <ciso646>
#include <cmath>
#include <cstdalign>
#include <cstdbool>
#include <ctgmath>

#endif // !defined(_CORE_HEADERS_ONLY)

#ifndef _MSVC_TESTING_NVCC
#include <assert.h>
#include <conio.h>
#include <crtdbg.h>
#include <ctype.h>
#include <direct.h>
#include <dos.h>
#include <errno.h>
#include <excpt.h>
#include <fcntl.h>
#include <fenv.h>
#include <float.h>
#include <intrin.h>
#include <inttypes.h>
#include <io.h>
#include <iso646.h>
#include <limits.h>
#include <locale.h>
#include <malloc.h>
#include <math.h>
#include <mbstring.h>
#include <memory.h>
#include <minmax.h>
#include <process.h>
#include <safeint.h>
#include <sal.h>
#include <search.h>
#include <setjmp.h>
#include <share.h>
#include <signal.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/locking.h>
#include <sys/stat.h>
#include <sys/timeb.h>
#include <sys/types.h>
#include <sys/utime.h>
#include <tchar.h>
#include <time.h>
#include <uchar.h>
#include <wchar.h>
#include <wctype.h>

#ifndef _CORE_HEADERS_ONLY
#include <complex.h>
#include <new.h>
#endif // !defined(_CORE_HEADERS_ONLY)

#ifndef _M_CEE_PURE
#include <fpieee.h>
#endif // !defined(_M_CEE_PURE)
#endif // !defined(_MSVC_TESTING_NVCC)

#if !(defined(__CUDACC__) && defined(__clang__))
#pragma pop_macro("new")
#endif // !(defined(__CUDACC__) && defined(__clang__))

#pragma warning(pop)

#endif // __MSVC_ALL_PUBLIC_HEADERS_HPP
