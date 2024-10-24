// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <bitset>
#include <cassert>
#include <cstddef>
#include <cstring>
#include <cwchar>
#include <ios>
#include <iosfwd>
#include <sstream>
#include <string>
#include <type_traits>

#if _HAS_CXX17
#include <string_view>
#endif // _HAS_CXX17

#if _HAS_CXX20
#define CONSTEXPR20 constexpr
#else // ^^^ _HAS_CXX20 / !_HAS_CXX20 vvv
#define CONSTEXPR20 inline
#endif // ^^^ !_HAS_CXX20 ^^^

using namespace std;

template <class T>
class odd_char_traits {
private:
    static constexpr unsigned char odd_mask = 0xF;

public:
    using char_type  = T;
    using int_type   = int;
    using off_type   = streamoff;
    using pos_type   = streampos;
    using state_type = mbstate_t;

    static constexpr bool eq(const char_type c, const char_type d) noexcept {
        return ((static_cast<unsigned char>(c) ^ static_cast<unsigned char>(d)) & odd_mask) == 0;
    }

    static constexpr bool lt(const char_type c, const char_type d) noexcept {
        return (static_cast<unsigned char>(c) & odd_mask) < (static_cast<unsigned char>(d) & odd_mask);
    }

    static constexpr int compare(const char_type* const c, const char_type* const d, const size_t n) noexcept {
        for (size_t i = 0; i != n; ++i) {
            int ci = static_cast<unsigned char>(c[i]) & odd_mask;
            int di = static_cast<unsigned char>(d[i]) & odd_mask;
            int r  = ci - di;
            if (r != 0) {
                return r;
            }
        }

        return 0;
    }

    static constexpr size_t length(const char_type* const p) noexcept {
        const char_type* c = p;
        while ((static_cast<unsigned char>(*c) & odd_mask) != 0) {
            ++c;
        }

        return static_cast<size_t>(c - p);
    }

    static constexpr const char_type* find(const char_type* const p, const size_t n, const char_type c) noexcept {
        for (size_t i = 0; i != n; ++i) {
            if (eq(p[i], c)) {
                return p + i;
            }
        }

        return nullptr;
    }

    static CONSTEXPR20 char_type* move(char_type* const s, const char_type* const p, const size_t n) noexcept {
#if _HAS_CXX20
        if (is_constant_evaluated()) {
            bool is_dst_in_src_range = false;
            for (size_t i = 0; i != n; ++i) {
                if (p + i == s) {
                    is_dst_in_src_range = true;
                    break;
                }
            }

            if (is_dst_in_src_range) {
                for (size_t i = n; i != 0;) {
                    --i;
                    s[i] = p[i];
                }
            } else {
                for (size_t i = 0; i != n; ++i) {
                    s[i] = p[i];
                }
            }
        } else
#endif // _HAS_CXX20
        {
            memmove(s, p, n);
        }
        return s;
    }

    static CONSTEXPR20 char_type* copy(char_type* const s, const char_type* const p, const size_t n) noexcept {
#if _HAS_CXX20
        if (is_constant_evaluated()) {
            for (size_t i = 0; i != n; ++i) {
                s[i] = p[i];
            }
        } else
#endif // _HAS_CXX20
        {
            memmove(s, p, n);
        }
        return s;
    }

    static constexpr void assign(char_type& r, const char_type& d) noexcept {
        r = d;
    }
    static CONSTEXPR20 char_type* assign(char_type* const s, const size_t n, const char_type c) noexcept {
#if _HAS_CXX20
        if (is_constant_evaluated()) {
            for (size_t i = 0; i != n; ++i) {
                s[i] = c;
            }
        } else
#endif // _HAS_CXX20
        {
            memset(s, static_cast<unsigned char>(c), n);
        }
        return s;
    }

    static constexpr bool not_eof(const int_type i) noexcept {
        return i != -1;
    }

    static constexpr char_type to_char_type(const int_type i) noexcept {
        return static_cast<char_type>(static_cast<unsigned char>(i));
    }

    static constexpr int_type to_int_type(const char_type i) noexcept {
        return static_cast<unsigned char>(i);
    }

    static constexpr bool eq_int_type(const int_type c, const int_type d) noexcept {
        if (c == -1) {
            return d == -1;
        } else {
            return ((c ^ d) & odd_mask) == 0;
        }
    }

    static constexpr int_type eof() noexcept {
        return -1;
    }
};

enum odd_char : unsigned char {};

template <>
class char_traits<odd_char> : public odd_char_traits<odd_char> {};

// GH-4930 "<string>: basic_string<unicorn>::find_meow_of family
// with std::char_traits<unicorn> specialization are not supported"
CONSTEXPR20 bool test_gh_4930() {
    constexpr odd_char s_init[]{static_cast<odd_char>(0x55), static_cast<odd_char>(0x44), static_cast<odd_char>(0x33),
        static_cast<odd_char>(0x22), static_cast<odd_char>(0x11), static_cast<odd_char>(0)};
    constexpr odd_char s2_init[]{static_cast<odd_char>(0x83), static_cast<odd_char>(0x12), static_cast<odd_char>(0)};

#if _HAS_CXX17
    using odd_string_view = basic_string_view<odd_char>;

    odd_string_view sv(s_init);

    assert(sv.length() == 5);

    assert(sv.find(static_cast<odd_char>(0x54)) == 1);
    assert(sv.find(static_cast<odd_char>(0x26)) == sv.npos);

    assert(sv.rfind(static_cast<odd_char>(0x54)) == 1);
    assert(sv.rfind(static_cast<odd_char>(0x26)) == sv.npos);

    odd_string_view sv2(s2_init);

    assert(sv.compare(sv2) > 0);
#if _HAS_CXX20
    assert(!sv.starts_with(sv2));
    assert(!sv.ends_with(sv2));
#if _HAS_CXX23
    assert(sv.contains(sv2));
#endif // _HAS_CXX23
#endif // _HAS_CXX20

    assert(sv.find(sv2) == 2);

    assert(sv.rfind(sv2) == 2);

    assert(sv.find_first_of(sv2) == 2);
    assert(sv2.find_first_of(sv) == 0);

    assert(sv.find_last_of(sv2) == 3);
    assert(sv2.find_last_of(sv) == 1);

    assert(sv.find_first_not_of(sv2) == 0);
    assert(sv2.find_first_not_of(sv) == sv2.npos);

    assert(sv.find_last_not_of(sv2) == 4);
    assert(sv2.find_last_not_of(sv) == sv2.npos);
#endif // _HAS_CXX17

    using odd_string = basic_string<odd_char>;

    odd_string s(s_init);

    assert(s.length() == 5);

    assert(s.find(static_cast<odd_char>(0x54)) == 1);
    assert(s.find(static_cast<odd_char>(0x26)) == s.npos);

    assert(s.rfind(static_cast<odd_char>(0x54)) == 1);
    assert(s.rfind(static_cast<odd_char>(0x26)) == s.npos);

    odd_string s2(s2_init);

    assert(s.compare(s2) > 0);
#if _HAS_CXX20
    assert(!s.starts_with(s2));
    assert(!s.ends_with(s2));
#if _HAS_CXX23
    assert(s.contains(s2));
#endif // _HAS_CXX23
#endif // _HAS_CXX20

    assert(s.find(s2) == 2);

    assert(s.rfind(s2) == 2);

    assert(s.find_first_of(s2) == 2);
    assert(s2.find_first_of(s) == 0);

    assert(s.find_last_of(s2) == 3);
    assert(s2.find_last_of(s) == 1);

    assert(s.find_first_not_of(s2) == 0);
    assert(s2.find_first_not_of(s) == s2.npos);

    assert(s.find_last_not_of(s2) == 4);
    assert(s2.find_last_not_of(s) == s2.npos);

#if _HAS_CXX17
    assert(s.compare(sv2) > 0);
#if _HAS_CXX20
    assert(!s.starts_with(sv2));
    assert(!s.ends_with(sv2));
#if _HAS_CXX23
    assert(s.contains(sv2));
#endif // _HAS_CXX23
#endif // _HAS_CXX20

    assert(s.find(sv2) == 2);

    assert(s.rfind(sv2) == 2);

    assert(s.find_first_of(sv2) == 2);
    assert(s2.find_first_of(sv) == 0);

    assert(s.find_last_of(sv2) == 3);
    assert(s2.find_last_of(sv) == 1);

    assert(s.find_first_not_of(sv2) == 0);
    assert(s2.find_first_not_of(sv) == s2.npos);

    assert(s.find_last_not_of(sv2) == 4);
    assert(s2.find_last_not_of(sv) == s2.npos);
#endif // _HAS_CXX17

    return true;
}

#if _HAS_CXX20
static_assert(test_gh_4930());
#endif // _HAS_CXX20

// GH-4956 "<bitset>: streaming operator >> does not use character traits"
void test_gh_4956() {
    // bitset's stream extraction operator was using `!=` to compare characters instead of `traits::eq`
    basic_string<char, odd_char_traits<char>> s("QQPPQ", 5);
    basic_istringstream<char, odd_char_traits<char>> iss(s);

    bitset<7> bs;
    iss >> bs;

    assert(bs.to_ulong() == 0b11001);
}

int main() {
    assert(test_gh_4930());
    test_gh_4956();
}
