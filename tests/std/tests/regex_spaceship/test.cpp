// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <assert.h>
#include <cstdlib>
#include <regex>
#include <string>

struct partial_order_traits : public std::char_traits<char> {
    static constexpr bool eq(char c1, char c2) {
        return c1 % 0x2A == c2 % 0x2A;
    }

    static constexpr bool lt(char c1, char c2) {
        return c1 % 0x2A < c2 % 0x2A;
    }

    static constexpr int compare(const char* s1, const char* s2, std::size_t n) {
        for(; n > 0; --n, ++s1, ++s2)
        {
            if (lt(*s1, *s2)) {
                return -1;
            }

            if (lt(*s2, *s1)) {
                return 1;
            }
        }

        return 0;
    }
};

template <class String>
void regex_test(const String& s1, const String& s2) {
    const std::regex all(".*");
    std::match_results<typename String::const_iterator> m1, m2;

    std::regex_match(s1, m1, all);
    std::regex_match(s2, m2, all);

    std::sub_match<typename String::const_iterator> sm1 = m1[0];
    std::sub_match<typename String::const_iterator> sm2 = m2[0];

    assert(sm1 > sm2);
    assert(sm2 < sm1);
    assert(sm1 >= sm2);
    assert(sm2 <= sm1);
    assert(sm1 == sm1);
    assert(sm1 != sm2);
    assert((sm1 <=> sm2) > 0);
    assert((sm2 <=> sm1) < 0);
    assert((sm1 <=> sm1) == 0);
    static_assert(std::is_same_v<decltype(sm1 <=> sm1), std::compare_three_way_t<String>);
}

int main() {
    {
        const std::string s1 = "Meow";
        const std::string s2 = "meow";

        regex_test(s1, s2);
    }

    {
        const std::basic_string<char, partial_order_traits> s1 = "\x2A";
        const std::basic_string<char, partial_order_traits> s2 = "\x2B";

        regex_test(s1, s2);
    }
}
