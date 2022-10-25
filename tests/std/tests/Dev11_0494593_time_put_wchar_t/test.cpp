// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <cassert>
#include <ctime>
#include <iterator>
#include <locale>
#include <sstream>
#include <string>

using namespace std;

int main() {
    const tm t = []() -> tm {
        tm ret = {};

        ret.tm_sec   = 57;
        ret.tm_min   = 42;
        ret.tm_hour  = 20;
        ret.tm_mday  = 28;
        ret.tm_mon   = 3;
        ret.tm_year  = 108;
        ret.tm_wday  = 1;
        ret.tm_yday  = 118;
        ret.tm_isdst = 0;

        return ret;
    }();

    {
        const locale l;

        basic_string<char> s(15, 'x');

        basic_stringstream<char> ss;

        const char fill = ' ';

        const char pattern[] = "%Y.%m.%d";

        const basic_string<char>::iterator ret = use_facet<time_put<char, basic_string<char>::iterator>>(l).put(
            s.begin(), ss, fill, &t, begin(pattern), end(pattern));

        assert(ret == s.begin() + 11);

        const basic_string<char> correct("2008.04.28\0xxxx", 15);

        assert(s == correct);
    }

    {
        const locale l;

        basic_string<wchar_t> s(15, L'x');

        basic_stringstream<wchar_t> ss;

        const wchar_t fill = L' ';

        const wchar_t pattern[] = L"%Y.%m.%d";

        const basic_string<wchar_t>::iterator ret =
            use_facet<time_put<wchar_t, basic_string<wchar_t>::iterator>>(l).put(
                s.begin(), ss, fill, &t, begin(pattern), end(pattern));

        assert(ret == s.begin() + 11);

        const basic_string<wchar_t> correct(L"2008.04.28\0xxxx", 15);

        assert(s == correct);
    }
}
