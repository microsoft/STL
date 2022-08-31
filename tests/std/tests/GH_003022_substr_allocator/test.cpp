// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <cassert>
#include <cstddef>
#include <memory>
#include <string>
#include <type_traits>
#include <utility>

using namespace std;

#if _HAS_CXX20
#define CONSTEXPR20 constexpr
#else // ^^^ _HAS_CXX20 / !_HAS_CXX20 vvv
#define CONSTEXPR20 inline
#endif // ^^^ !_HAS_CXX20 ^^^

#define TEST_ASSERT(...) assert((__VA_ARGS__))

#ifdef __cpp_char8_t
using char8_type = char8_t;
#else // ^^^ defined(__cpp_char8_t) / !defined(__cpp_char8_t) vvv
using char8_type = unsigned char;
#endif // ^^^ !defined(__cpp_char8_t) ^^^

template <class CharT, enable_if_t<is_same_v<CharT, char>, int> = 0>
constexpr auto statically_widen_impl(
    const char* s, const char8_type*, const char16_t*, const char32_t*, const wchar_t*) noexcept {
    return s;
}

template <class CharT, enable_if_t<is_same_v<CharT, char8_type>, int> = 0>
constexpr auto statically_widen_impl(
    const char*, const char8_type* s8, const char16_t*, const char32_t*, const wchar_t*) noexcept {
    return s8;
}

template <class CharT, enable_if_t<is_same_v<CharT, char16_t>, int> = 0>
constexpr auto statically_widen_impl(
    const char*, const char8_type*, const char16_t* s16, const char32_t*, const wchar_t*) noexcept {
    return s16;
}

template <class CharT, enable_if_t<is_same_v<CharT, char32_t>, int> = 0>
constexpr auto statically_widen_impl(
    const char*, const char8_type*, const char16_t*, const char32_t* s32, const wchar_t*) noexcept {
    return s32;
}

template <class CharT, enable_if_t<is_same_v<CharT, wchar_t>, int> = 0>
constexpr auto statically_widen_impl(
    const char*, const char8_type*, const char16_t*, const char32_t*, const wchar_t* sw) noexcept {
    return sw;
}

#ifdef __cpp_char8_t
#define STATICALLY_WIDEN(CT, S) (statically_widen_impl<CT>(S, u8##S, u##S, U##S, L##S))
#else // ^^^ defined(__cpp_char8_t) / !defined(__cpp_char8_t) vvv
#define STATICALLY_WIDEN(CT, S) (statically_widen_impl<CT>(S, nullptr, u##S, U##S, L##S))
#endif // ^^^ !defined(__cpp_char8_t) ^^^

template <class T>
class payloaded_allocator {
private:
    int payload_ = 0;

public:
    using value_type                             = T;
    using size_type                              = size_t;
    using difference_type                        = ptrdiff_t;
    using propagate_on_container_move_assignment = true_type;
    using propagate_on_container_swap            = true_type;

    payloaded_allocator() = default;
    constexpr explicit payloaded_allocator(int payload) noexcept : payload_{payload} {}

    template <class U>
    constexpr payloaded_allocator(const payloaded_allocator<U>& other) noexcept : payload_{other.get_payload()} {}

    template <class U>
    friend constexpr bool operator==(const payloaded_allocator& lhs, const payloaded_allocator<U>& rhs) noexcept {
        return lhs.get_payload() == rhs.get_payload();
    }

#if !_HAS_CXX20
    template <class U>
    friend constexpr bool operator!=(const payloaded_allocator& lhs, const payloaded_allocator<U>& rhs) noexcept {
        return !(lhs == rhs);
    }
#endif // !_HAS_CXX20

    CONSTEXPR20 T* allocate(const size_t n) {
        return allocator<T>{}.allocate(n);
    }

    CONSTEXPR20 void deallocate(T* const p, const size_t n) {
        return allocator<T>{}.deallocate(p, n);
    }

    constexpr int get_payload() const noexcept {
        return payload_;
    }
};

template <class CharT>
CONSTEXPR20 bool test_substr_allocator() {
    struct str_test_case {
        const CharT* ntcts;
        size_t offset;
        size_t count;
    };

    constexpr str_test_case substring_test_cases[]{
        {STATICALLY_WIDEN(CharT, "nul"), 1, 0},
        {STATICALLY_WIDEN(CharT, "str"), 1, 2},
        {STATICALLY_WIDEN(CharT, "ful"), 0, 3},
        {STATICALLY_WIDEN(CharT, "Empty"), 4, 0},
        {STATICALLY_WIDEN(CharT, "Shorter"), 5, 2},
        {STATICALLY_WIDEN(CharT, "Longer"), 1, 5},
        {STATICALLY_WIDEN(CharT, "FullStr"), 0, 7},
        {STATICALLY_WIDEN(CharT, "Empty substring"), 12, 0},
        {STATICALLY_WIDEN(CharT, "Short substring"), 9, 3},
        {STATICALLY_WIDEN(CharT, "Mid substring"), 7, 5},
        {STATICALLY_WIDEN(CharT, "Longer string"), 3, 9},
        {STATICALLY_WIDEN(CharT, "Maximal SSO len"), 0, 15},
        {STATICALLY_WIDEN(CharT, "Take an empty substring from a long string"), 35, 0},
        {STATICALLY_WIDEN(CharT, "Take a short substring from a long string"), 21, 2},
        {STATICALLY_WIDEN(CharT, "Take some substring from a long string"), 15, 6},
        {STATICALLY_WIDEN(CharT, "Take a longer substring from a long string"), 20, 13},
        {STATICALLY_WIDEN(CharT, "Take a quite longer substring..."), 10, 20},
        {STATICALLY_WIDEN(CharT, "Take the whole long string as a substring"), 0, 41},
    };

    using string_type = basic_string<CharT, char_traits<CharT>, payloaded_allocator<CharT>>;

    payloaded_allocator<CharT> myator{42};

    for (const auto& test_case : substring_test_cases) {
        string_type full_str{test_case.ntcts, myator};

        const auto offset = test_case.offset;
        const auto count  = test_case.count;

        // Test const lvalue overloads
        TEST_ASSERT(string_type{full_str, offset, count}.get_allocator().get_payload() == 0);
        TEST_ASSERT(string_type{full_str, offset}.get_allocator().get_payload() == 0);

        TEST_ASSERT(full_str.substr(offset, count).get_allocator().get_payload() == 0);

        TEST_ASSERT(string_type{full_str, offset, count, myator}.get_allocator().get_payload() == 42);
        TEST_ASSERT(string_type{full_str, offset, myator}.get_allocator().get_payload() == 42);

        // Test non-const rvalue overloads
        TEST_ASSERT(string_type{string_type{full_str}, offset, count}.get_allocator().get_payload() == 0);
        TEST_ASSERT(string_type{string_type{full_str}, offset}.get_allocator().get_payload() == 0);

        TEST_ASSERT(string_type{full_str}.substr(offset, count).get_allocator().get_payload() == 0);

        TEST_ASSERT(string_type{string_type{full_str}, offset, count, myator}.get_allocator().get_payload() == 42);
        TEST_ASSERT(string_type{string_type{full_str}, offset, myator}.get_allocator().get_payload() == 42);
    }

    // Also test well-formedness change before/after P2438R2
    struct ambiguator {
        string_type str;

        operator const string_type&() {
            return str;
        }

        operator string_type&&() {
            return move(str);
        }
    };

#if _HAS_CXX23 && !defined(__EDG__) // TRANSITION, VSO-1601179
    static_assert(!is_constructible_v<string_type, ambiguator&, size_t, size_t>, "This should be ambiguous");
#else // _HAS_CXX23 && !defined(__EDG__)
    static_assert(is_constructible_v<string_type, ambiguator&, size_t, size_t>, "This should be unambiguous");
#endif // _HAS_CXX23 && !defined(__EDG__)

    return true;
}

#if _HAS_CXX20
static_assert(test_substr_allocator<char>());
#ifdef __cpp_char8_t
static_assert(test_substr_allocator<char8_t>());
#endif // __cpp_char8_t
static_assert(test_substr_allocator<char16_t>());
static_assert(test_substr_allocator<char32_t>());
static_assert(test_substr_allocator<wchar_t>());
#endif // _HAS_CXX20

int main() {
    (void) test_substr_allocator<char>();
#ifdef __cpp_char8_t
    (void) test_substr_allocator<char8_t>();
#endif // __cpp_char8_t
    (void) test_substr_allocator<char16_t>();
    (void) test_substr_allocator<char32_t>();
    (void) test_substr_allocator<wchar_t>();
}
