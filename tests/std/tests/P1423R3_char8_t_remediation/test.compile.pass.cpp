// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#define _SILENCE_CXX20_U8PATH_DEPRECATION_WARNING 1
#include <filesystem>
#include <ostream>
#include <string_view>
#include <type_traits>
#include <utility>

#define STATIC_ASSERT(...) static_assert(__VA_ARGS__, #__VA_ARGS__)

int main() {} // COMPILE-ONLY

template <class Stream, class charT, class = void>
constexpr bool stream_insertable = false;
template <class Stream, class charT>
constexpr bool
    stream_insertable<Stream, charT, std::void_t<decltype(std::declval<Stream&>() << std::declval<charT>())>> = true;

STATIC_ASSERT(stream_insertable<std::ostream, int>);
STATIC_ASSERT(stream_insertable<std::ostream, const int>);
STATIC_ASSERT(stream_insertable<std::ostream, int&>);
STATIC_ASSERT(stream_insertable<std::ostream, const int&>);

template <class Stream, class charT>
constexpr bool test_stream_insertion() {
    // validate that expressions of type (possibly-const) charT and (possibly-const) charT* (possibly-const) are
    // equivalently insertable into Stream
    constexpr bool result = stream_insertable<Stream, charT>;
    STATIC_ASSERT(stream_insertable<Stream, const charT> == result);
    STATIC_ASSERT(stream_insertable<Stream, charT&> == result);
    STATIC_ASSERT(stream_insertable<Stream, const charT&> == result);

    using pointer = charT*;
    STATIC_ASSERT(stream_insertable<Stream, pointer> == result);
    STATIC_ASSERT(stream_insertable<Stream, const pointer> == result);
    STATIC_ASSERT(stream_insertable<Stream, pointer&> == result);
    STATIC_ASSERT(stream_insertable<Stream, const pointer&> == result);

    using const_pointer = const charT*;
    STATIC_ASSERT(stream_insertable<Stream, const_pointer> == result);
    STATIC_ASSERT(stream_insertable<Stream, const const_pointer> == result);
    STATIC_ASSERT(stream_insertable<Stream, const_pointer&> == result);
    STATIC_ASSERT(stream_insertable<Stream, const const_pointer&> == result);

    return result;
}

// Control cases
STATIC_ASSERT(test_stream_insertion<std::ostream, char>());
STATIC_ASSERT(test_stream_insertion<std::wostream, wchar_t>());

#ifdef __cpp_char8_t
STATIC_ASSERT(!test_stream_insertion<std::ostream, char8_t>());
STATIC_ASSERT(!test_stream_insertion<std::wostream, char8_t>());
#endif // __cpp_char8_t

#ifdef _NATIVE_WCHAR_T_DEFINED
STATIC_ASSERT(test_stream_insertion<std::ostream, wchar_t>() == !_HAS_CXX20);
#endif // _NATIVE_WCHAR_T_DEFINED
STATIC_ASSERT(test_stream_insertion<std::ostream, char16_t>() == !_HAS_CXX20);
STATIC_ASSERT(test_stream_insertion<std::ostream, char32_t>() == !_HAS_CXX20);
STATIC_ASSERT(test_stream_insertion<std::wostream, char16_t>() == !_HAS_CXX20);
STATIC_ASSERT(test_stream_insertion<std::wostream, char32_t>() == !_HAS_CXX20);

#if _HAS_CXX17 && defined(__cpp_char8_t)
void test_u8path() {
    (void) std::filesystem::u8path(u8"a");
    const std::basic_string_view sv{u8"a"};
    (void) std::filesystem::u8path(sv);
    (void) std::filesystem::u8path(sv.begin(), sv.end());
}
#endif // _HAS_CXX17 && defined(__cpp_char8_t)
