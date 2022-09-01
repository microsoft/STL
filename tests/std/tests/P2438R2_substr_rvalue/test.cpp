// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <cassert>
#include <cstddef>
#include <memory>
#include <string>
#include <string_view>
#include <type_traits>
#include <utility>

#define TEST_ASSERT(...) assert((__VA_ARGS__))

using namespace std;

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

    constexpr T* allocate(const size_t n) {
        return allocator<T>{}.allocate(n);
    }

    constexpr void deallocate(T* const p, const size_t n) {
        return allocator<T>{}.deallocate(p, n);
    }

    constexpr int get_payload() const noexcept {
        return payload_;
    }
};

template <class CharT>
consteval auto statically_widen_impl(string_view sv,
#ifdef __cpp_char8_t
    u8string_view svu8,
#endif // __cpp_char8_t
    u16string_view svu16, u32string_view svu32, wstring_view svw) noexcept {
    if constexpr (is_same_v<CharT, char>) {
        return sv;
    }
#ifdef __cpp_char8_t
    else if constexpr (is_same_v<CharT, char8_t>) {
        return svu8;
    }
#endif // __cpp_char8_t
    else if constexpr (is_same_v<CharT, char16_t>) {
        return svu16;
    } else if constexpr (is_same_v<CharT, char32_t>) {
        return svu32;
    } else if constexpr (is_same_v<CharT, wchar_t>) {
        return svw;
    } else {
        static_assert(!is_same_v<CharT, CharT>, "Invalid character type");
    }
}

#ifdef __cpp_char8_t
#define STATICALLY_WIDEN(CT, S) (statically_widen_impl<CT>(S##sv, u8##S##sv, u##S##sv, U##S##sv, L##S##sv))
#else // ^^^ defined(__cpp_char8_t) / !defined(__cpp_char8_t) vvv
#define STATICALLY_WIDEN(CT, S) (statically_widen_impl<CT>(S##sv, u##S##sv, U##S##sv, L##S##sv))
#endif // ^^^ !defined(__cpp_char8_t) ^^^

template <class CharT>
constexpr bool test_rvalue_substr() {
    struct str_test_case {
        basic_string_view<CharT> str_view;
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

    for (const auto& [str_view, offset, count] : substring_test_cases) {
        using string_type = basic_string<CharT>;

        const auto substring_view = str_view.substr(offset, count);
        const auto suffix_view    = str_view.substr(offset);

        // Non-portable implementation details
        constexpr size_t sso_buffer_capacity = basic_string<CharT>{}.capacity();

        TEST_ASSERT(string_type{string_type{str_view}, offset, count} == substring_view);
        TEST_ASSERT(string_type{str_view}.substr(offset, count) == substring_view);
        TEST_ASSERT(string_type{string_type{str_view}, offset} == suffix_view);
        TEST_ASSERT(string_type{str_view}.substr(offset) == suffix_view);

        // Test non-portable implementation details
        {
            string_type source_str{str_view};
            const auto old_data = source_str.data();
            string_type dest_str{move(source_str), offset, count};

            TEST_ASSERT((dest_str.data() == old_data) == (dest_str.size() > sso_buffer_capacity));
        }

        {
            string_type source_str{str_view};
            const auto old_data = source_str.data();
            string_type dest_str{move(source_str), offset};

            TEST_ASSERT((dest_str.data() == old_data) == (dest_str.size() > sso_buffer_capacity));
        }

        {
            string_type source_str{str_view};
            const auto old_data = source_str.data();
            auto dest_str       = move(source_str).substr(offset, count);

            TEST_ASSERT((dest_str.data() == old_data) == (dest_str.size() > sso_buffer_capacity));
        }

        // Test non-portable implementation details for stateful allocators

        using payloaded_string_type = basic_string<CharT, char_traits<CharT>, payloaded_allocator<CharT>>;

        payloaded_allocator<CharT> non_default_allocator{42};
        {
            payloaded_string_type source_str{str_view};
            const auto old_data = source_str.data();
            payloaded_string_type dest_str{move(source_str), offset, count};

            TEST_ASSERT((dest_str.data() == old_data) == (dest_str.size() > sso_buffer_capacity));
        }
        {
            payloaded_string_type source_str{str_view, non_default_allocator};
            const auto old_data = source_str.data();
            payloaded_string_type dest_str{move(source_str), offset, count};

            TEST_ASSERT(dest_str.data() != old_data);
        }
        {
            payloaded_string_type source_str{str_view, non_default_allocator};
            const auto old_data = source_str.data();
            payloaded_string_type dest_str{move(source_str), offset, count, non_default_allocator};

            TEST_ASSERT((dest_str.data() == old_data) == (dest_str.size() > sso_buffer_capacity));
        }

        {
            payloaded_string_type source_str{str_view};
            const auto old_data = source_str.data();
            payloaded_string_type dest_str{move(source_str), offset};

            TEST_ASSERT((dest_str.data() == old_data) == (dest_str.size() > sso_buffer_capacity));
        }
        {
            payloaded_string_type source_str{str_view, non_default_allocator};
            const auto old_data = source_str.data();
            payloaded_string_type dest_str{move(source_str), offset};

            TEST_ASSERT(dest_str.data() != old_data);
        }
        {
            payloaded_string_type source_str{str_view, non_default_allocator};
            const auto old_data = source_str.data();
            payloaded_string_type dest_str{move(source_str), offset, non_default_allocator};

            TEST_ASSERT((dest_str.data() == old_data) == (dest_str.size() > sso_buffer_capacity));
        }

        {
            payloaded_string_type source_str{str_view};
            const auto old_data = source_str.data();
            auto dest_str       = move(source_str).substr(offset, count);

            TEST_ASSERT((dest_str.data() == old_data) == (dest_str.size() > sso_buffer_capacity));
        }
        {
            payloaded_string_type source_str{str_view, non_default_allocator};
            const auto old_data = source_str.data();
            auto dest_str       = move(source_str).substr(offset, count);

            TEST_ASSERT(dest_str.data() != old_data);
        }
    }

    return true;
}

static_assert(test_rvalue_substr<char>());
#ifdef __cpp_char8_t
static_assert(test_rvalue_substr<char8_t>());
#endif // __cpp_char8_t
static_assert(test_rvalue_substr<char16_t>());
static_assert(test_rvalue_substr<char32_t>());
static_assert(test_rvalue_substr<wchar_t>());

int main() {
    (void) test_rvalue_substr<char>();
#ifdef __cpp_char8_t
    (void) test_rvalue_substr<char8_t>();
#endif // __cpp_char8_t
    (void) test_rvalue_substr<char16_t>();
    (void) test_rvalue_substr<char32_t>();
    (void) test_rvalue_substr<wchar_t>();
}
