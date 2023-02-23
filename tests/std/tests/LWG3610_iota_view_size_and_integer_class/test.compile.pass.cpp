// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <ranges>

using namespace std;

template <class R>
concept CanSize = requires(R& r) { ranges::size(r); };

// Validate standard signed integer types
static_assert(CanSize<ranges::iota_view<signed char, _Signed128>>);
static_assert(CanSize<ranges::iota_view<signed char, _Unsigned128>>);
static_assert(CanSize<ranges::iota_view<short, _Signed128>>);
static_assert(CanSize<ranges::iota_view<short, _Unsigned128>>);
static_assert(CanSize<ranges::iota_view<int, _Signed128>>);
static_assert(CanSize<ranges::iota_view<int, _Unsigned128>>);
static_assert(CanSize<ranges::iota_view<long, _Signed128>>);
static_assert(CanSize<ranges::iota_view<long, _Unsigned128>>);
static_assert(CanSize<ranges::iota_view<long long, _Signed128>>);
static_assert(CanSize<ranges::iota_view<long long, _Unsigned128>>);

// Validate standard unsigned integer types
static_assert(CanSize<ranges::iota_view<unsigned char, _Signed128>>);
static_assert(CanSize<ranges::iota_view<unsigned char, _Unsigned128>>);
static_assert(CanSize<ranges::iota_view<unsigned short, _Signed128>>);
static_assert(CanSize<ranges::iota_view<unsigned short, _Unsigned128>>);
static_assert(CanSize<ranges::iota_view<unsigned int, _Signed128>>);
static_assert(CanSize<ranges::iota_view<unsigned int, _Unsigned128>>);
static_assert(CanSize<ranges::iota_view<unsigned long, _Signed128>>);
static_assert(CanSize<ranges::iota_view<unsigned long, _Unsigned128>>);
static_assert(CanSize<ranges::iota_view<unsigned long long, _Signed128>>);
static_assert(CanSize<ranges::iota_view<unsigned long long, _Unsigned128>>);

// Validate other integer types (other than bool)
static_assert(CanSize<ranges::iota_view<char, _Signed128>>);
static_assert(CanSize<ranges::iota_view<char, _Unsigned128>>);
static_assert(CanSize<ranges::iota_view<wchar_t, _Signed128>>);
static_assert(CanSize<ranges::iota_view<wchar_t, _Unsigned128>>);
#ifdef __cpp_char8_t
static_assert(CanSize<ranges::iota_view<char8_t, _Signed128>>);
static_assert(CanSize<ranges::iota_view<char8_t, _Unsigned128>>);
#endif // __cpp_char8_t
static_assert(CanSize<ranges::iota_view<char16_t, _Signed128>>);
static_assert(CanSize<ranges::iota_view<char16_t, _Unsigned128>>);
static_assert(CanSize<ranges::iota_view<char32_t, _Signed128>>);
static_assert(CanSize<ranges::iota_view<char32_t, _Unsigned128>>);
