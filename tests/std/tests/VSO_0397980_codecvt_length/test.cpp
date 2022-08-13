// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#define _SILENCE_CXX17_CODECVT_HEADER_DEPRECATION_WARNING
#define _SILENCE_CXX20_CODECVT_FACETS_DEPRECATION_WARNING

#undef _ENFORCE_FACET_SPECIALIZATIONS
#define _ENFORCE_FACET_SPECIALIZATIONS 0

#include <cassert>
#include <codecvt>
#include <cstdlib>
#include <iterator>
#include <locale>
#include <ostream>
#include <utility>

using namespace std;

#define STATIC_ASSERT(...) static_assert(__VA_ARGS__, #__VA_ARGS__)

// Also validate deleted stream inserters for char8_t and pointer-to-char8_t, speculatively implemented from P1423R1.
#ifdef __cpp_char8_t
template <class T>
constexpr auto f(int) -> decltype(declval<ostream&>() << declval<T&>(), true) {
    return true;
}
template <class T>
constexpr bool f(...) {
    return false;
}
STATIC_ASSERT(f<int>(0));
STATIC_ASSERT(!f<void>(0));
STATIC_ASSERT(!f<char8_t>(0));
STATIC_ASSERT(!f<char8_t*>(0));
STATIC_ASSERT(!f<const char8_t*>(0));
#endif // __cpp_char8_t

int main() {
    {
        static constexpr char str[] = "\x20\xC3\xB4\xEA\xAF\x8D"; // u8"\u0020\u00F4\uABCD"
        constexpr auto length       = static_cast<int>(sizeof(str) - 1);
        {
            auto& cvt = use_facet<codecvt<char16_t, char, mbstate_t>>(locale::classic());
            {
                // validate codecvt<char16_t, char, mbstate_t>::in
                char16_t actual[3];
                const char* last1 = nullptr;
                char16_t* last2   = nullptr;
                mbstate_t state{};
                auto result = cvt.in(state, str, str + length, last1, begin(actual), end(actual), last2);
                assert(result == codecvt_base::ok);
                assert(last1 == str + length);
                assert(last2 == end(actual));
                assert(actual[0] == u'\u0020');
                assert(actual[1] == u'\u00f4');
                assert(actual[2] == u'\uabcd');
            }
            {
                // validate codecvt<char16_t, char, mbstate_t>::length()
                mbstate_t state{};
                int actual = cvt.length(state, str, str + length, ~size_t{0});
                assert(actual == length);
                actual = cvt.length(state, str, str + length, size_t{3});
                assert(actual == length);
            }
        }
        {
            // validate codecvt_utf8<char16_t>::length()
            codecvt_utf8<char16_t> cvt;
            mbstate_t state{};
            int actual = cvt.length(state, str, str + length, ~size_t{0});
            assert(actual == length);
            actual = cvt.length(state, str, str + length, size_t{3});
            assert(actual == length);
        }
        {
            // validate codecvt_utf8<wchar_t>::length()
            codecvt_utf8<wchar_t> cvt;
            mbstate_t state{};
            int actual = cvt.length(state, str, str + length, ~size_t{0});
            assert(actual == length);
            actual = cvt.length(state, str, str + length, size_t{3});
            assert(actual == length);
        }
        {
            auto& cvt = use_facet<codecvt<char32_t, char, mbstate_t>>(locale::classic());
            {
                // validate codecvt<char32_t, char, mbstate_t>::in()
                char32_t actual[3];
                const char* last1 = nullptr;
                char32_t* last2   = nullptr;
                mbstate_t state{};
                auto result = cvt.in(state, str, str + length, last1, begin(actual), end(actual), last2);
                assert(result == codecvt_base::ok);
                assert(last1 == str + length);
                assert(last2 == end(actual));
                assert(actual[0] == U'\u0020');
                assert(actual[1] == U'\u00f4');
                assert(actual[2] == U'\uabcd');
            }
            {
                // validate codecvt<char32_t, char, mbstate_t>::length()
                mbstate_t state{};
                int actual = cvt.length(state, str, str + length, ~size_t{0});
                assert(actual == length);
                actual = cvt.length(state, str, str + length, size_t{3});
                assert(actual == length);
            }
        }
        {
            // validate codecvt_utf8<char32_t>::length()
            codecvt_utf8<char32_t> cvt;
            mbstate_t state{};
            int actual = cvt.length(state, str, str + length, ~size_t{0});
            assert(actual == length);
            actual = cvt.length(state, str, str + length, size_t{3});
            assert(actual == length);
        }
    }
    {
        static constexpr char str[] = "\x20\xf4\xe2";
        constexpr auto length       = static_cast<int>(sizeof(str) - 1);
        {
            auto& cvt = use_facet<codecvt<wchar_t, char, mbstate_t>>(locale::classic());
            {
                // validate codecvt<wchar_t, char, mbstate_t>::in()
                wchar_t actual[3];
                const char* last1 = nullptr;
                wchar_t* last2    = nullptr;
                mbstate_t state{};
                auto result = cvt.in(state, str, str + length, last1, begin(actual), end(actual), last2);
                assert(result == codecvt_base::ok);
                assert(last1 == str + length);
                assert(last2 == end(actual));
                assert(actual[0] == L'\u0020');
                assert(actual[1] == L'\u00f4');
                assert(actual[2] == L'\u00e2');
            }
            {
                // validate codecvt<wchar_t, char, mbstate_t>::length()
                mbstate_t state{};
                int actual = cvt.length(state, str, str + length, ~size_t{0});
                assert(actual == length);
                actual = cvt.length(state, str, str + length, size_t{3});
                assert(actual == length);
            }
        }
        {
            auto& cvt = use_facet<codecvt<unsigned short, char, mbstate_t>>(locale::classic()); // extension
            {
                // validate codecvt<unsigned short, char, mbstate_t>::in()
                unsigned short actual[3];
                const char* last1     = nullptr;
                unsigned short* last2 = nullptr;
                mbstate_t state{};
                auto result = cvt.in(state, str, str + length, last1, begin(actual), end(actual), last2);
                assert(result == codecvt_base::ok);
                assert(last1 == str + length);
                assert(last2 == end(actual));
                assert(actual[0] == L'\u0020');
                assert(actual[1] == L'\u00f4');
                assert(actual[2] == L'\u00e2');
            }
            {
                // validate codecvt<unsigned short, char, mbstate_t>::length()
                mbstate_t state{};
                int actual = cvt.length(state, str, str + length, ~size_t{0});
                assert(actual == length);
                actual = cvt.length(state, str, str + length, size_t{3});
                assert(actual == length);
            }
        }
    }

#if _HAS_CXX17 && defined(__cpp_char8_t)
    {
        static constexpr char8_t str[] = u8"\u0020\u00F4\uABCD";
        constexpr auto length          = static_cast<int>(sizeof(str) - 1);
        {
            auto& cvt = use_facet<codecvt<char32_t, char8_t, mbstate_t>>(locale::classic());
            {
                char32_t actual[3];
                const char8_t* last1 = nullptr;
                char32_t* last2      = nullptr;
                mbstate_t state{};
                auto result = cvt.in(state, str, str + length, last1, begin(actual), end(actual), last2);
                assert(result == codecvt_base::ok);
                assert(last1 == str + length);
                assert(last2 == end(actual));
                assert(actual[0] == U'\u0020');
                assert(actual[1] == U'\u00f4');
                assert(actual[2] == U'\uabcd');
            }
            {
                mbstate_t state{};
                int actual = cvt.length(state, str, str + length, ~size_t{0});
                assert(actual == length);
                actual = cvt.length(state, str, str + length, size_t{3});
                assert(actual == length);
            }
        }
        {
            auto& cvt = use_facet<codecvt<char16_t, char8_t, mbstate_t>>(locale::classic());
            {
                char16_t actual[3];
                const char8_t* last1 = nullptr;
                char16_t* last2      = nullptr;
                mbstate_t state{};
                auto result = cvt.in(state, str, str + length, last1, begin(actual), end(actual), last2);
                assert(result == codecvt_base::ok);
                assert(last1 == str + length);
                assert(last2 == end(actual));
                assert(actual[0] == u'\u0020');
                assert(actual[1] == u'\u00f4');
                assert(actual[2] == u'\uabcd');
            }
            {
                mbstate_t state{};
                int actual = cvt.length(state, str, str + length, ~size_t{0});
                assert(actual == length);
                actual = cvt.length(state, str, str + length, size_t{3});
                assert(actual == length);
            }
        }
    }
#endif // _HAS_CXX17 && defined(__cpp_char8_t)
}
