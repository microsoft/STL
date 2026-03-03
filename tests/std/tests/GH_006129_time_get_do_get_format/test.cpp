// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <cassert>
#include <ctime>
#include <ios>
#include <locale>

using I = const char*;

struct time_get_char : std::time_get<char, I> {
    explicit time_get_char(std::size_t refs = 0) : time_get(refs) {}
};

int main() {
    const time_get_char f(1);
    std::ios str(nullptr);
    std::ios_base::iostate err;
    std::tm t;

    // %c in the C locale uses "%a %b %e %T %Y"
    {
        const char in[] = "Thu Jun  6 09:49:10 2009";
        err             = std::ios_base::goodbit;
        t               = std::tm();
        I i             = f.get(I(in), I(in + sizeof(in) - 1), str, err, &t, 'c');
        assert(err == std::ios_base::eofbit);
        assert(i == in + sizeof(in) - 1);
        assert(t.tm_wday == 4);
        assert(t.tm_mon == 5);
        assert(t.tm_mday == 6);
        assert(t.tm_hour == 9);
        assert(t.tm_min == 49);
        assert(t.tm_sec == 10);
        assert(t.tm_year == 109);
    }

    // %x in the C locale uses "%m/%d/%y"
    {
        const char in[] = "03/15/09";
        err             = std::ios_base::goodbit;
        t               = std::tm();
        I i             = f.get(I(in), I(in + sizeof(in) - 1), str, err, &t, 'x');
        assert(err == std::ios_base::eofbit);
        assert(i == in + sizeof(in) - 1);
        assert(t.tm_mon == 2);
        assert(t.tm_mday == 15);
        assert(t.tm_year == 109);
    }
}
