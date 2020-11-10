// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

// DevDiv-316853 "<algorithm>: find()'s memchr() optimization is incorrect"
// DevDiv-468500 "<algorithm>: find()'s memchr() optimization is insufficiently aggressive"

#pragma warning(disable : 4389) // signed/unsigned mismatch
#pragma warning(disable : 4805) // '==': unsafe mix of type '_Ty' and type 'const _Ty' in operation
// This test intentionally triggers that warning when one of the inputs to find is bool

#include <algorithm>
#include <assert.h>
#include <iterator>
#include <list>
#include <string>
#include <vector>

using namespace std;

#define STATIC_ASSERT(...) static_assert(__VA_ARGS__, #__VA_ARGS__)

struct Cat {
    explicit Cat(int n) : m_n(n) {}

    int m_n;
};

bool operator==(int x, const Cat& c) {
    return x == c.m_n * 11;
}

int main() {
    { // DevDiv-316853 "<algorithm>: find()'s memchr() optimization is incorrect"
        vector<signed char> v;
        v.push_back(22);
        v.push_back(33);
        v.push_back(-1);
        v.push_back(44);
        v.push_back(55);

        assert(find(v.begin(), v.end(), 33) - v.begin() == 1);
        assert(find(v.begin(), v.end(), -1) - v.begin() == 2);
        assert(find(v.begin(), v.end(), 255) - v.begin() == 5);

        assert(find(v.cbegin(), v.cend(), 33) - v.cbegin() == 1);
        assert(find(v.cbegin(), v.cend(), -1) - v.cbegin() == 2);
        assert(find(v.cbegin(), v.cend(), 255) - v.cbegin() == 5);
    }


    { // Optimization inapplicable due to bogus iterator type (although the element type and value type are good)
        list<unsigned char> l;
        l.push_back(11);
        l.push_back(22);
        l.push_back(33);
        l.push_back(44);
        l.push_back(55);

        assert(find(l.begin(), l.end(), 44) == next(l.begin(), 3));
        assert(find(l.begin(), l.end(), 17) == l.end());
    }

    { // Optimization inapplicable due to bogus element type and bogus value type
        vector<string> v;
        v.push_back("cute");
        v.push_back("fluffy");
        v.push_back("kittens");

        assert(find(v.begin(), v.end(), "fluffy") == v.begin() + 1);
        assert(find(v.begin(), v.end(), "zombies") == v.end());
    }

    { // Optimization inapplicable due to bogus element type (although the value types are good)
        vector<unsigned long> v;
        v.push_back(0x10203040UL);
        v.push_back(0x11223344UL);
        v.push_back(0xAABBCCDDUL);

        // Make sure we don't look for 0x11 bytes in the range!

        assert(find(v.begin(), v.end(), 0xAABBCCDDUL) == v.begin() + 2);
        assert(find(v.begin(), v.end(), 0x11UL) == v.end());
    }

    { // Optimization inapplicable due to bogus value type (although the element type is good)
        vector<unsigned char> v;
        v.push_back(11);
        v.push_back(22);
        v.push_back(33);

        assert(find(v.begin(), v.end(), Cat(2)) == v.begin() + 1);
        assert(find(v.begin(), v.end(), Cat(4)) == v.end());
    }


    { // Test optimized element types.
        vector<char> vc;
        vc.push_back('m');
        vc.push_back('e');
        vc.push_back('o');
        vc.push_back('w');

        assert(find(vc.begin(), vc.end(), 'o') == vc.begin() + 2);
        assert(find(vc.begin(), vc.end(), 'X') == vc.end());

        assert(find(vc.cbegin(), vc.cend(), 'o') == vc.cbegin() + 2);
        assert(find(vc.cbegin(), vc.cend(), 'X') == vc.cend());
    }

    { // Test optimized element types.
        vector<signed char> vsc;
        vsc.push_back(17);
        vsc.push_back(29);
        vsc.push_back(-1);
        vsc.push_back(-128);
        vsc.push_back(127);

        assert(find(vsc.begin(), vsc.end(), 17) == vsc.begin());
        assert(find(vsc.begin(), vsc.end(), 29) == vsc.begin() + 1);
        assert(find(vsc.begin(), vsc.end(), -1) == vsc.begin() + 2);
        assert(find(vsc.begin(), vsc.end(), -128) == vsc.begin() + 3);
        assert(find(vsc.begin(), vsc.end(), 127) == vsc.begin() + 4);
        assert(find(vsc.begin(), vsc.end(), 255) == vsc.end());

        assert(find(vsc.cbegin(), vsc.cend(), 17) == vsc.cbegin());
        assert(find(vsc.cbegin(), vsc.cend(), 29) == vsc.cbegin() + 1);
        assert(find(vsc.cbegin(), vsc.cend(), -1) == vsc.cbegin() + 2);
        assert(find(vsc.cbegin(), vsc.cend(), -128) == vsc.cbegin() + 3);
        assert(find(vsc.cbegin(), vsc.cend(), 127) == vsc.cbegin() + 4);
        assert(find(vsc.cbegin(), vsc.cend(), 255) == vsc.cend());
    }

    { // Test optimized element types.
        vector<unsigned char> vuc;
        vuc.push_back(0);
        vuc.push_back(1);
        vuc.push_back(47);
        vuc.push_back(254);
        vuc.push_back(255);

        assert(find(vuc.begin(), vuc.end(), 47) == vuc.begin() + 2);
        assert(find(vuc.begin(), vuc.end(), 255) == vuc.begin() + 4);
        assert(find(vuc.begin(), vuc.end(), -1) == vuc.end());

        assert(find(vuc.cbegin(), vuc.cend(), 47) == vuc.cbegin() + 2);
        assert(find(vuc.cbegin(), vuc.cend(), 255) == vuc.cbegin() + 4);
        assert(find(vuc.cbegin(), vuc.cend(), -1) == vuc.cend());
    }


    { // Test optimized value types.
        const unsigned char arr[] = {10, 20, 0, 255, 30, 40};

        assert(find(begin(arr), end(arr), static_cast<signed char>(30)) == begin(arr) + 4);
        assert(find(begin(arr), end(arr), static_cast<short>(30)) == begin(arr) + 4);
        assert(find(begin(arr), end(arr), static_cast<int>(30)) == begin(arr) + 4);
        assert(find(begin(arr), end(arr), static_cast<long>(30)) == begin(arr) + 4);
        assert(find(begin(arr), end(arr), static_cast<long long>(30)) == begin(arr) + 4);

        assert(find(begin(arr), end(arr), static_cast<unsigned char>(30)) == begin(arr) + 4);
        assert(find(begin(arr), end(arr), static_cast<unsigned short>(30)) == begin(arr) + 4);
        assert(find(begin(arr), end(arr), static_cast<unsigned int>(30)) == begin(arr) + 4);
        assert(find(begin(arr), end(arr), static_cast<unsigned long>(30)) == begin(arr) + 4);
        assert(find(begin(arr), end(arr), static_cast<unsigned long long>(30)) == begin(arr) + 4);

        assert(find(begin(arr), end(arr), static_cast<char>(30)) == begin(arr) + 4);
        assert(find(begin(arr), end(arr), static_cast<wchar_t>(30)) == begin(arr) + 4);

        assert(find(begin(arr), end(arr), false) == begin(arr) + 2);
        assert(find(begin(arr), end(arr), true) == end(arr));
    }


    { // Test limit checks.
        const signed char sc[] = {-128, -127, -126, -2, -1, 0, 1, 2, 125, 126, 127};

        assert(find(begin(sc), end(sc), false) == begin(sc) + 5);
        assert(find(begin(sc), end(sc), true) == begin(sc) + 6);

        assert(find(begin(sc), end(sc), static_cast<signed char>(-128)) == begin(sc));
        assert(find(begin(sc), end(sc), static_cast<signed char>(-1)) == begin(sc) + 4);
        assert(find(begin(sc), end(sc), static_cast<signed char>(5)) == end(sc));
        assert(find(begin(sc), end(sc), static_cast<signed char>(127)) == begin(sc) + 10);

        assert(find(begin(sc), end(sc), static_cast<short>(-129)) == end(sc));
        assert(find(begin(sc), end(sc), static_cast<short>(-128)) == begin(sc));
        assert(find(begin(sc), end(sc), static_cast<short>(-1)) == begin(sc) + 4);
        assert(find(begin(sc), end(sc), static_cast<short>(5)) == end(sc));
        assert(find(begin(sc), end(sc), static_cast<short>(127)) == begin(sc) + 10);
        assert(find(begin(sc), end(sc), static_cast<short>(128)) == end(sc));
        assert(find(begin(sc), end(sc), static_cast<short>(255)) == end(sc));

        assert(find(begin(sc), end(sc), -129) == end(sc));
        assert(find(begin(sc), end(sc), -128) == begin(sc));
        assert(find(begin(sc), end(sc), -1) == begin(sc) + 4);
        assert(find(begin(sc), end(sc), 5) == end(sc));
        assert(find(begin(sc), end(sc), 127) == begin(sc) + 10);
        assert(find(begin(sc), end(sc), 128) == end(sc));
        assert(find(begin(sc), end(sc), 255) == end(sc));

        assert(find(begin(sc), end(sc), -129L) == end(sc));
        assert(find(begin(sc), end(sc), -128L) == begin(sc));
        assert(find(begin(sc), end(sc), -1L) == begin(sc) + 4);
        assert(find(begin(sc), end(sc), 5L) == end(sc));
        assert(find(begin(sc), end(sc), 127L) == begin(sc) + 10);
        assert(find(begin(sc), end(sc), 128L) == end(sc));
        assert(find(begin(sc), end(sc), 255L) == end(sc));

        assert(find(begin(sc), end(sc), -129LL) == end(sc));
        assert(find(begin(sc), end(sc), -128LL) == begin(sc));
        assert(find(begin(sc), end(sc), -1LL) == begin(sc) + 4);
        assert(find(begin(sc), end(sc), 5LL) == end(sc));
        assert(find(begin(sc), end(sc), 127LL) == begin(sc) + 10);
        assert(find(begin(sc), end(sc), 128LL) == end(sc));
        assert(find(begin(sc), end(sc), 255LL) == end(sc));

        assert(find(begin(sc), end(sc), static_cast<unsigned char>(0)) == begin(sc) + 5);
        assert(find(begin(sc), end(sc), static_cast<unsigned char>(8)) == end(sc));
        assert(find(begin(sc), end(sc), static_cast<unsigned char>(127)) == begin(sc) + 10);
        assert(find(begin(sc), end(sc), static_cast<unsigned char>(128)) == end(sc));
        assert(find(begin(sc), end(sc), static_cast<unsigned char>(255)) == end(sc));

        assert(find(begin(sc), end(sc), static_cast<unsigned short>(0)) == begin(sc) + 5);
        assert(find(begin(sc), end(sc), static_cast<unsigned short>(8)) == end(sc));
        assert(find(begin(sc), end(sc), static_cast<unsigned short>(127)) == begin(sc) + 10);
        assert(find(begin(sc), end(sc), static_cast<unsigned short>(128)) == end(sc));
        assert(find(begin(sc), end(sc), static_cast<unsigned short>(255)) == end(sc));

        assert(find(begin(sc), end(sc), 0U) == begin(sc) + 5);
        assert(find(begin(sc), end(sc), 8U) == end(sc));
        assert(find(begin(sc), end(sc), 127U) == begin(sc) + 10);
        assert(find(begin(sc), end(sc), 128U) == end(sc));
        assert(find(begin(sc), end(sc), 255U) == end(sc));

        assert(find(begin(sc), end(sc), 0UL) == begin(sc) + 5);
        assert(find(begin(sc), end(sc), 8UL) == end(sc));
        assert(find(begin(sc), end(sc), 127UL) == begin(sc) + 10);
        assert(find(begin(sc), end(sc), 128UL) == end(sc));
        assert(find(begin(sc), end(sc), 255UL) == end(sc));

        assert(find(begin(sc), end(sc), 0ULL) == begin(sc) + 5);
        assert(find(begin(sc), end(sc), 8ULL) == end(sc));
        assert(find(begin(sc), end(sc), 127ULL) == begin(sc) + 10);
        assert(find(begin(sc), end(sc), 128ULL) == end(sc));
        assert(find(begin(sc), end(sc), 255ULL) == end(sc));
    }

    { // Test limit checks.
        const unsigned char uc[] = {0, 1, 2, 253, 254, 255};

        assert(find(begin(uc), end(uc), false) == begin(uc));
        assert(find(begin(uc), end(uc), true) == begin(uc) + 1);

        assert(find(begin(uc), end(uc), static_cast<signed char>(-1)) == end(uc));
        assert(find(begin(uc), end(uc), static_cast<signed char>(0)) == begin(uc));
        assert(find(begin(uc), end(uc), static_cast<signed char>(2)) == begin(uc) + 2);
        assert(find(begin(uc), end(uc), static_cast<signed char>(6)) == end(uc));

        assert(find(begin(uc), end(uc), static_cast<short>(-1)) == end(uc));
        assert(find(begin(uc), end(uc), static_cast<short>(0)) == begin(uc));
        assert(find(begin(uc), end(uc), static_cast<short>(2)) == begin(uc) + 2);
        assert(find(begin(uc), end(uc), static_cast<short>(6)) == end(uc));
        assert(find(begin(uc), end(uc), static_cast<short>(255)) == begin(uc) + 5);
        assert(find(begin(uc), end(uc), static_cast<short>(256)) == end(uc));

        assert(find(begin(uc), end(uc), -1) == end(uc));
        assert(find(begin(uc), end(uc), 0) == begin(uc));
        assert(find(begin(uc), end(uc), 2) == begin(uc) + 2);
        assert(find(begin(uc), end(uc), 6) == end(uc));
        assert(find(begin(uc), end(uc), 255) == begin(uc) + 5);
        assert(find(begin(uc), end(uc), 256) == end(uc));

        assert(find(begin(uc), end(uc), -1L) == end(uc));
        assert(find(begin(uc), end(uc), 0L) == begin(uc));
        assert(find(begin(uc), end(uc), 2L) == begin(uc) + 2);
        assert(find(begin(uc), end(uc), 6L) == end(uc));
        assert(find(begin(uc), end(uc), 255L) == begin(uc) + 5);
        assert(find(begin(uc), end(uc), 256L) == end(uc));

        assert(find(begin(uc), end(uc), -1LL) == end(uc));
        assert(find(begin(uc), end(uc), 0LL) == begin(uc));
        assert(find(begin(uc), end(uc), 2LL) == begin(uc) + 2);
        assert(find(begin(uc), end(uc), 6LL) == end(uc));
        assert(find(begin(uc), end(uc), 255LL) == begin(uc) + 5);
        assert(find(begin(uc), end(uc), 256LL) == end(uc));

        assert(find(begin(uc), end(uc), static_cast<unsigned char>(0)) == begin(uc));
        assert(find(begin(uc), end(uc), static_cast<unsigned char>(252)) == end(uc));
        assert(find(begin(uc), end(uc), static_cast<unsigned char>(253)) == begin(uc) + 3);
        assert(find(begin(uc), end(uc), static_cast<unsigned char>(255)) == begin(uc) + 5);

        assert(find(begin(uc), end(uc), static_cast<unsigned short>(0)) == begin(uc));
        assert(find(begin(uc), end(uc), static_cast<unsigned short>(252)) == end(uc));
        assert(find(begin(uc), end(uc), static_cast<unsigned short>(253)) == begin(uc) + 3);
        assert(find(begin(uc), end(uc), static_cast<unsigned short>(255)) == begin(uc) + 5);
        assert(find(begin(uc), end(uc), static_cast<unsigned short>(256)) == end(uc));
        assert(find(begin(uc), end(uc), static_cast<unsigned short>(0xFFFF)) == end(uc));

        assert(find(begin(uc), end(uc), 0U) == begin(uc));
        assert(find(begin(uc), end(uc), 252U) == end(uc));
        assert(find(begin(uc), end(uc), 253U) == begin(uc) + 3);
        assert(find(begin(uc), end(uc), 255U) == begin(uc) + 5);
        assert(find(begin(uc), end(uc), 256U) == end(uc));
        assert(find(begin(uc), end(uc), 0xFFFFFFFFU) == end(uc));

        assert(find(begin(uc), end(uc), 0UL) == begin(uc));
        assert(find(begin(uc), end(uc), 252UL) == end(uc));
        assert(find(begin(uc), end(uc), 253UL) == begin(uc) + 3);
        assert(find(begin(uc), end(uc), 255UL) == begin(uc) + 5);
        assert(find(begin(uc), end(uc), 256UL) == end(uc));
        assert(find(begin(uc), end(uc), 0xFFFFFFFFUL) == end(uc));

        assert(find(begin(uc), end(uc), 0ULL) == begin(uc));
        assert(find(begin(uc), end(uc), 252ULL) == end(uc));
        assert(find(begin(uc), end(uc), 253ULL) == begin(uc) + 3);
        assert(find(begin(uc), end(uc), 255ULL) == begin(uc) + 5);
        assert(find(begin(uc), end(uc), 256ULL) == end(uc));
        assert(find(begin(uc), end(uc), 0xFFFFFFFFFFFFFFFFULL) == end(uc));
    }

    { // Test advanced limit checks.
        const signed char sc[] = {-128, -127, -126, -2, -1, 0, 1, 2, 125, 126, 127};

        STATIC_ASSERT(static_cast<signed char>(-1) != static_cast<unsigned short>(0xFFFF));
        STATIC_ASSERT(static_cast<signed char>(-1) == 0xFFFFFFFFUL);
        STATIC_ASSERT(static_cast<signed char>(-2) == 0xFFFFFFFEUL);
        STATIC_ASSERT(static_cast<signed char>(-127) == 0xFFFFFF81UL);
        STATIC_ASSERT(static_cast<signed char>(-128) == 0xFFFFFF80UL);

        assert(find(begin(sc), end(sc), static_cast<unsigned short>(0xFFFF)) == end(sc));

        assert(find(begin(sc), end(sc), 0xFFFFFFFFUL) == begin(sc) + 4);
        assert(find(begin(sc), end(sc), 0xFFFFFFFEUL) == begin(sc) + 3);
        assert(find(begin(sc), end(sc), 0xFFFFFFAAUL) == end(sc));
        assert(find(begin(sc), end(sc), 0xFFFFFF81UL) == begin(sc) + 1);
        assert(find(begin(sc), end(sc), 0xFFFFFF80UL) == begin(sc));
        assert(find(begin(sc), end(sc), 0xFFFFFF7FUL) == end(sc));
        assert(find(begin(sc), end(sc), 0xFFFFFF00UL) == end(sc));

        assert(find(begin(sc), end(sc), 0xFFFFFFFFFFFFFFFFULL) == begin(sc) + 4);
        assert(find(begin(sc), end(sc), 0xFFFFFFFFFFFFFFFEULL) == begin(sc) + 3);
        assert(find(begin(sc), end(sc), 0xFFFFFFFFFFFFFFAAULL) == end(sc));
        assert(find(begin(sc), end(sc), 0xFFFFFFFFFFFFFF81ULL) == begin(sc) + 1);
        assert(find(begin(sc), end(sc), 0xFFFFFFFFFFFFFF80ULL) == begin(sc));
        assert(find(begin(sc), end(sc), 0xFFFFFFFFFFFFFF7FULL) == end(sc));
        assert(find(begin(sc), end(sc), 0xFFFFFFFFFFFFFF00ULL) == end(sc));
    }
}
