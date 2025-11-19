// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <limits>

template <class T>
constexpr bool traps_ = std::numeric_limits<T>::traps;

#if defined(_M_IX86) || defined(_M_X64) && !defined(_M_ARM64EC)
static_assert(traps_<int>,
    "The #ED hardware exception always happens for zero division and for division overflow INT_MIN/-1. "
    "These are translated to STATUS_INTEGER_DIVIDE_BY_ZERO and STATUS_INTEGER_OVERFLOW  SEH exceptions");
#elif defined(_M_ARM64) || defined(_M_ARM64EC) || defined(_M_HYBRID_X86_ARM64)
#ifdef __clang__
static_assert(!traps_<int>, "The hardware does not trap. Clang compiles code as is, so there's no trap");
#else // ^^^ defined(__clang__) / !defined(__clang__) vvv
static_assert(traps_<int>, "The hardware does not trap. MSVC inserts check for zero to trap zero division. "
                           "It does not insert checks for INT_MIN/-1 division overflow though");
#endif // ^^^ !defined(__clang__) ^^^
#else // ^^^ defined(_M_ARM64) || defined(_M_ARM64EC) || defined(_M_HYBRID_X86_ARM64) ^^^
#error Unsupported hardware
#endif

template <class T>
constexpr bool non_promoted_and_traps_if_int_does = std::is_same_v<decltype(~T{}), T> && traps_<T> == traps_<int>;

static_assert(non_promoted_and_traps_if_int_does<unsigned int> //
                  && non_promoted_and_traps_if_int_does<long> && non_promoted_and_traps_if_int_does<unsigned long>
                  && non_promoted_and_traps_if_int_does<long long>
                  && non_promoted_and_traps_if_int_does<unsigned long long>,
    "all non-promoted integers should trap or not trap equally");

template <class T>
constexpr bool promoted_and_does_not_trap = !std::is_same_v<decltype(~T{}), T> && !traps_<T>;

static_assert(!traps_<bool>, "bool does not trap for a moot reason; see LWG-554 resolution");

static_assert(promoted_and_does_not_trap<char> && promoted_and_does_not_trap<signed char>
                  && promoted_and_does_not_trap<unsigned char> //
                  && promoted_and_does_not_trap<short> && promoted_and_does_not_trap<unsigned short>
                  && promoted_and_does_not_trap<wchar_t> && promoted_and_does_not_trap<char16_t>
                  && promoted_and_does_not_trap<char32_t>,
    "promoted integers do not trap for a moot reason; see LWG-554 resolution");

#ifdef __cpp_char8_t
static_assert(
    promoted_and_does_not_trap<char8_t>, "promoted integers do not trap for a moot reason; see LWG-554 resolution");
#endif

static_assert(!traps_<float> && !traps_<double> && !traps_<long double>,
    "floats don't trap because even if '/fp:except' is passed, it should be enabled at runtime");
