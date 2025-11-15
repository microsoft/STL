// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <limits>

template <class T>
constexpr bool traps_ = std::numeric_limits<T>::traps;

#if defined(_M_IX86) || defined(_M_X64) && !defined(_M_ARM64EC)
static_assert(traps_<int>,
    "The #ED hardware exception always happens for zero division and for division overflow INT_MIN/-1. "
    "It is translated to the corresponding SEH exceptions");
#elif defined(_M_ARM64) || defined(_M_ARM64EC) || defined(_M_HYBRID_X86_ARM64)
// The hardware does not trap.
#ifdef __clang__
static_assert(!traps_<int>, "Clang compiles code as is, so there's no trap");
#else // ^^^ defined(__clang__) / !defined(__clang__) vvv
static_assert(traps_<int>, "MSVC inserts check for zero to trap zero division. "
                           "It does not insert checks for INT_MIN/-1 division overflow though.");
#endif // ^^^ !defined(__clang__) ^^^
#else // ^^^ defined(_M_ARM64) || defined(_M_ARM64EC) || defined(_M_HYBRID_X86_ARM64) ^^^
#error Unsupported hardware
#endif

static_assert(traps_<unsigned int> == traps_<int> && traps_<unsigned int> == traps_<char32_t>
                  && traps_<long> == traps_<int> && traps_<unsigned long> == traps_<int>
                  && traps_<long long> == traps_<int> && traps_<unsigned long long> == traps_<int>,
    "all non-promoted integers should trap or not trap equally");

static_assert(!traps_<bool>, "bool does not trap for a moot reason; see LWG-554 resolution");

static_assert(!traps_<char> && !traps_<signed char> && !traps_<unsigned char> //
                  && !traps_<short> && !traps_<unsigned short> && !traps_<wchar_t> && !traps_<char16_t>,
    "promoted integers do not trap for a moot reason; see LWG-554 resolution");

#ifdef __cpp_char8_t
static_assert(!traps_<char8_t>, "promoted integers do not trap for a moot reason; see LWG-554 resolution");
#endif

static_assert(!traps_<float> && !traps_<double> && !traps_<long double>,
    "floats don't trap because even if '/fp:except' is passed, it should be enabled at runtime");
