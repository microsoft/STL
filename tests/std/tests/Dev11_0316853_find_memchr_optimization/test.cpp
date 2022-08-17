// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

// DevDiv-316853 "<algorithm>: find()'s memchr() optimization is incorrect"
// DevDiv-468500 "<algorithm>: find()'s memchr() optimization is insufficiently aggressive"

#pragma warning(disable : 4389) // signed/unsigned mismatch
#pragma warning(disable : 4805) // '==': unsafe mix of type '_Ty' and type 'const _Ty' in operation
// This test intentionally triggers that warning when one of the inputs to find is bool
#pragma warning(disable : 4984) // 'if constexpr' is a C++17 language extension

#ifdef __clang__
#pragma clang diagnostic ignored "-Wc++17-extensions"
#pragma clang diagnostic ignored "-Wsign-compare"
#endif // __clang__

#include <algorithm>
#include <cassert>
#include <iterator>
#include <limits>
#include <list>
#include <string>
#include <type_traits>
#include <vector>

using namespace std;

constexpr auto long_min = numeric_limits<long>::min();
constexpr auto long_max = numeric_limits<long>::max();

#define STATIC_ASSERT(...) static_assert(__VA_ARGS__, #__VA_ARGS__)

struct Cat {
    explicit Cat(int n) : m_n(n) {}

    int m_n;
};

bool operator==(int x, const Cat& c) {
    return x == c.m_n * 11;
}

template <class ElementType, class ValueType>
void test_limit_check_elements_impl() {
    if constexpr (is_signed_v<ElementType>) {
        using UElementType              = make_unsigned_t<ElementType>;
        constexpr ElementType min_val   = numeric_limits<ElementType>::min();
        constexpr ElementType max_val   = numeric_limits<ElementType>::max();
        constexpr UElementType umax_val = numeric_limits<UElementType>::max();

        const ElementType sc[] = {
            min_val, min_val + 1, min_val + 2, -2, -1, -1, 0, 1, 1, 2, max_val - 2, max_val - 1, max_val};

        if constexpr (is_signed_v<ValueType>) {
            if constexpr (numeric_limits<ValueType>::min() < min_val) {
                assert(find(begin(sc), end(sc), ValueType{ValueType{min_val} - 1}) == end(sc));
            }

            if constexpr (sizeof(ElementType) <= sizeof(ValueType)) {
                assert(find(begin(sc), end(sc), ValueType{min_val}) == begin(sc));
            }
            assert(find(begin(sc), end(sc), ValueType{-1}) == begin(sc) + 4);

            assert(count(begin(sc), end(sc), ValueType{-1}) == 2);
        }

        assert(count(begin(sc), end(sc), ValueType{0}) == 1);

        assert(find(begin(sc), end(sc), ValueType{0}) == begin(sc) + 6);
        assert(find(begin(sc), end(sc), ValueType{5}) == end(sc));

        if constexpr (sizeof(ElementType) <= sizeof(ValueType)) {
            assert(find(begin(sc), end(sc), ValueType{max_val}) == begin(sc) + 12);
            assert(count(begin(sc), end(sc), ValueType{max_val}) == 1);

            if constexpr (sizeof(ElementType) < sizeof(ValueType)) {
                assert(find(begin(sc), end(sc), ValueType{ValueType{max_val} + 1}) == end(sc));
                assert(find(begin(sc), end(sc), ValueType{umax_val}) == end(sc));

                assert(count(begin(sc), end(sc), ValueType{ValueType{max_val} + 1}) == 0);
                assert(count(begin(sc), end(sc), ValueType{umax_val}) == 0);
            }
        }
    } else {
        constexpr ElementType max_val = numeric_limits<ElementType>::max();
        constexpr ValueType max_vt    = numeric_limits<ValueType>::max();

        const ElementType uc[] = {0, 1, 1, 2, max_val - 2, max_val - 1, max_val};

        assert(find(begin(uc), end(uc), ValueType{0}) == begin(uc));
        assert(find(begin(uc), end(uc), ValueType{2}) == begin(uc) + 3);
        assert(find(begin(uc), end(uc), ValueType{6}) == end(uc));

        if constexpr (max_val <= max_vt) {
            assert(find(begin(uc), end(uc), ValueType{max_val - 3}) == end(uc));
            assert(find(begin(uc), end(uc), ValueType{max_val - 2}) == begin(uc) + 4);
            assert(find(begin(uc), end(uc), ValueType{max_val}) == begin(uc) + 6);
            if constexpr (max_val < max_vt) {
                assert(find(begin(uc), end(uc), ValueType{ValueType{max_val} + 1}) == end(uc));
                if constexpr (sizeof(ElementType) < sizeof(ValueType)) {
                    assert(find(begin(uc), end(uc), max_vt) == end(uc));
                }
            }
        }
    }
}

template <class ElementType>
void test_limit_check_elements() {
    test_limit_check_elements_impl<ElementType, signed char>();
    test_limit_check_elements_impl<ElementType, short>();
    test_limit_check_elements_impl<ElementType, int>();
    test_limit_check_elements_impl<ElementType, long>();
    test_limit_check_elements_impl<ElementType, long long>();

    test_limit_check_elements_impl<ElementType, unsigned char>();
    test_limit_check_elements_impl<ElementType, unsigned short>();
    test_limit_check_elements_impl<ElementType, unsigned int>();
    test_limit_check_elements_impl<ElementType, unsigned long>();
    test_limit_check_elements_impl<ElementType, unsigned long long>();
}

int main() {
    { // DevDiv-316853 "<algorithm>: find()'s memchr() optimization is incorrect"
        vector<signed char> v;
        v.push_back(22);
        v.push_back(33);
        v.push_back(-1);
        v.push_back(44);
        v.push_back(44);
        v.push_back(55);

#ifdef __cpp_lib_concepts
        static_assert(_Vector_alg_in_find_is_safe<decltype(v.begin()), decltype(33)>, "should optimize");
#endif // __cpp_lib_concepts

        assert(find(v.begin(), v.end(), 33) - v.begin() == 1);
        assert(find(v.begin(), v.end(), -1) - v.begin() == 2);
        assert(find(v.begin(), v.end(), 255) - v.begin() == 6);

        assert(find(v.cbegin(), v.cend(), 33) - v.cbegin() == 1);
        assert(find(v.cbegin(), v.cend(), -1) - v.cbegin() == 2);
        assert(find(v.cbegin(), v.cend(), 255) - v.cbegin() == 6);

        assert(count(v.cbegin(), v.cend(), 33) == 1);
        assert(count(v.cbegin(), v.cend(), 44) == 2);
        assert(count(v.cbegin(), v.cend(), 255) == 0);
    }


    { // Optimization inapplicable due to bogus iterator type (although the element type and value type are good)
        list<unsigned char> l;
        l.push_back(11);
        l.push_back(22);
        l.push_back(33);
        l.push_back(44);
        l.push_back(44);
        l.push_back(55);

        static_assert(!_Vector_alg_in_find_is_safe<decltype(l.begin()), decltype(33)>, "should not optimize");

        assert(find(l.begin(), l.end(), 44) == next(l.begin(), 3));
        assert(find(l.begin(), l.end(), 17) == l.end());

        assert(count(l.begin(), l.end(), 44) == 2);
        assert(count(l.begin(), l.end(), 17) == 0);
    }

    { // Optimization inapplicable due to bogus element type and bogus value type
        vector<string> v;
        v.push_back("cute");
        v.push_back("cute");
        v.push_back("fluffy");
        v.push_back("kittens");

        static_assert(!_Vector_alg_in_find_is_safe<decltype(v.begin()), decltype("fluffy")>, "should not optimize");

        assert(find(v.begin(), v.end(), "fluffy") == v.begin() + 2);
        assert(find(v.begin(), v.end(), "zombies") == v.end());

        assert(count(v.begin(), v.end(), "cute") == 2);
        assert(count(v.begin(), v.end(), "zombies") == 0);
    }

    {
        // Optimization with memchr inapplicable due to bogus element type (although the value types are good)
        // Vector optimization still works

        vector<unsigned long> v;
        v.push_back(0x10203040UL);
        v.push_back(0x11223344UL);
        v.push_back(0xAABBCCDDUL);

#ifdef __cpp_lib_concepts
        static_assert(_Vector_alg_in_find_is_safe<decltype(v.begin()), decltype(0x10203040UL)>, "should optimize");
#endif // __cpp_lib_concepts

        // Make sure we don't look for 0x11 bytes in the range!

        assert(find(v.begin(), v.end(), 0xAABBCCDDUL) == v.begin() + 2);
        assert(find(v.begin(), v.end(), 0x11UL) == v.end());

        assert(count(v.begin(), v.end(), 0xAABBCCDDUL) == 1);
        assert(count(v.begin(), v.end(), 0x11UL) == 0);
    }

    { // Optimization inapplicable due to bogus value type (although the element type is good)
        vector<unsigned char> v;
        v.push_back(11);
        v.push_back(22);
        v.push_back(33);
        v.push_back(33);
        v.push_back(33);

        static_assert(!_Vector_alg_in_find_is_safe<decltype(v.begin()), decltype(Cat(3))>, "should not optimize");

        assert(find(v.begin(), v.end(), Cat(2)) == v.begin() + 1);
        assert(find(v.begin(), v.end(), Cat(4)) == v.end());

        assert(count(v.begin(), v.end(), Cat(3)) == 3);
        assert(count(v.begin(), v.end(), Cat(7)) == 0);
    }

    { // Test optimized element types.
        vector<char> vc;
        vc.push_back('m');
        vc.push_back('e');
        vc.push_back('e');
        vc.push_back('o');
        vc.push_back('o');
        vc.push_back('w');

#ifdef __cpp_lib_concepts
        static_assert(_Vector_alg_in_find_is_safe<decltype(vc.begin()), decltype('m')>, "should optimize");
#endif // __cpp_lib_concepts

        assert(find(vc.begin(), vc.end(), 'o') == vc.begin() + 3);
        assert(find(vc.begin(), vc.end(), 'X') == vc.end());

        assert(count(vc.begin(), vc.end(), 'o') == 2);
        assert(count(vc.begin(), vc.end(), 'X') == 0);

        assert(find(vc.cbegin(), vc.cend(), 'o') == vc.cbegin() + 3);
        assert(find(vc.cbegin(), vc.cend(), 'X') == vc.cend());

        assert(count(vc.cbegin(), vc.cend(), 'o') == 2);
        assert(count(vc.cbegin(), vc.cend(), 'X') == 0);
    }

    { // Test optimized element types.
        vector<signed char> vsc;
        vsc.push_back(17);
        vsc.push_back(29);
        vsc.push_back(-1);
        vsc.push_back(-128);
        vsc.push_back(127);

#ifdef __cpp_lib_concepts
        static_assert(_Vector_alg_in_find_is_safe<decltype(vsc.begin()), decltype(29)>, "should optimize");
#endif // __cpp_lib_concepts

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

#ifdef __cpp_lib_concepts
        static_assert(_Vector_alg_in_find_is_safe<decltype(vuc.begin()), decltype(47)>, "should optimize");
#endif // __cpp_lib_concepts

        assert(find(vuc.begin(), vuc.end(), 47) == vuc.begin() + 2);
        assert(find(vuc.begin(), vuc.end(), 255) == vuc.begin() + 4);
        assert(find(vuc.begin(), vuc.end(), -1) == vuc.end());

        assert(find(vuc.cbegin(), vuc.cend(), 47) == vuc.cbegin() + 2);
        assert(find(vuc.cbegin(), vuc.cend(), 255) == vuc.cbegin() + 4);
        assert(find(vuc.cbegin(), vuc.cend(), -1) == vuc.cend());
    }


    { // Test optimized value types.
        const unsigned char arr[] = {10, 20, 0, 255, 30, 40};

        static_assert(_Vector_alg_in_find_is_safe<decltype(begin(arr)), decltype(30)>, "should optimize");

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

    // Test limit checks.
    test_limit_check_elements<char>();
    test_limit_check_elements<signed char>();
    test_limit_check_elements<short>();
    test_limit_check_elements<int>();
    test_limit_check_elements<long>();
    test_limit_check_elements<long long>();
    test_limit_check_elements<unsigned char>();
    test_limit_check_elements<unsigned short>();
    test_limit_check_elements<unsigned int>();
    test_limit_check_elements<unsigned long>();
    test_limit_check_elements<unsigned long long>();

    { // Test advanced limit checks. We love integral promotions.
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

        const short ss[] = {-32768, -32767, -32766, -2, -1, 0, 1, 2, 32765, 32766, 32767};

        STATIC_ASSERT(static_cast<short>(-1) != static_cast<unsigned short>(0xFFFF));
        STATIC_ASSERT(static_cast<short>(-1) == 0xFFFFFFFFUL);
        STATIC_ASSERT(static_cast<short>(-2) == 0xFFFFFFFEUL);
        STATIC_ASSERT(static_cast<short>(-32767) == 0xFFFF8001UL);
        STATIC_ASSERT(static_cast<short>(-32768) == 0xFFFF8000UL);

        assert(find(begin(ss), end(ss), static_cast<unsigned short>(0xFFFF)) == end(ss));

        assert(find(begin(ss), end(ss), 0xFFFFFFFFUL) == begin(ss) + 4);
        assert(find(begin(ss), end(ss), 0xFFFFFFFEUL) == begin(ss) + 3);
        assert(find(begin(ss), end(ss), 0xFFFFAAAAUL) == end(ss));
        assert(find(begin(ss), end(ss), 0xFFFF8001UL) == begin(ss) + 1);
        assert(find(begin(ss), end(ss), 0xFFFF8000UL) == begin(ss));
        assert(find(begin(ss), end(ss), 0xFFFF7FFFUL) == end(ss));
        assert(find(begin(ss), end(ss), 0xFFFF0000UL) == end(ss));

        assert(find(begin(ss), end(ss), 0xFFFFFFFFFFFFFFFFULL) == begin(ss) + 4);
        assert(find(begin(ss), end(ss), 0xFFFFFFFFFFFFFFFEULL) == begin(ss) + 3);
        assert(find(begin(ss), end(ss), 0xFFFFFFFFFFFFAAAAULL) == end(ss));
        assert(find(begin(ss), end(ss), 0xFFFFFFFFFFFF8001ULL) == begin(ss) + 1);
        assert(find(begin(ss), end(ss), 0xFFFFFFFFFFFF8000ULL) == begin(ss));
        assert(find(begin(ss), end(ss), 0xFFFFFFFFFFFF7FFFULL) == end(ss));
        assert(find(begin(ss), end(ss), 0xFFFFFFFFFFFF0000ULL) == end(ss));

        // No longer integral promotions, still repeat the applicable part once to be sure

        const long sl[] = {long_min, long_min + 1, long_min + 2, -2, -1, 0, 1, 2, long_max - 2, long_max - 1, long_max};

        STATIC_ASSERT(static_cast<long>(-1) != static_cast<unsigned short>(0xFFFF));
        STATIC_ASSERT(static_cast<long>(-1) == 0xFFFFFFFFUL);
        STATIC_ASSERT(static_cast<long>(-2) == 0xFFFFFFFEUL);
        STATIC_ASSERT(static_cast<long>(long_min + 1) == 0x80000001UL);
        STATIC_ASSERT(static_cast<long>(long_min) == 0x80000000UL);

        assert(find(begin(sl), end(sl), static_cast<unsigned short>(0xFFFF)) == end(sl));

        assert(find(begin(sl), end(sl), 0xFFFFFFFFUL) == begin(sl) + 4);
        assert(find(begin(sl), end(sl), 0xFFFFFFFEUL) == begin(sl) + 3);
        assert(find(begin(sl), end(sl), 0xAAAAAAAAUL) == end(sl));
        assert(find(begin(sl), end(sl), 0x80000001UL) == begin(sl) + 1);
        assert(find(begin(sl), end(sl), 0x80000000UL) == begin(sl));

        assert(find(begin(sl), end(sl), 0xFFFFFFFFFFFFFFFFULL) == begin(sl) + 4);
        assert(find(begin(sl), end(sl), 0xFFFFFFFFFFFFFFFEULL) == begin(sl) + 3);
        assert(find(begin(sl), end(sl), 0xFFFFFFFFAAAAAAAAULL) == end(sl));
        assert(find(begin(sl), end(sl), 0xFFFFFFFF80000001ULL) == begin(sl) + 1);
        assert(find(begin(sl), end(sl), 0xFFFFFFFF80000000ULL) == begin(sl));
        assert(find(begin(sl), end(sl), 0xFFFFFFFF7FFFFFFFULL) == end(sl));
        assert(find(begin(sl), end(sl), 0xFFFFFFFF00000000ULL) == end(sl));
    }

    { // Test bools
        const bool arr[]{true, true, true, false, true, false};

        static_assert(_Vector_alg_in_find_is_safe<decltype(begin(arr)), decltype(true)>, "should optimize");

        assert(find(begin(arr), end(arr), false) == begin(arr) + 3);
        assert(find(begin(arr), end(arr), true) == begin(arr));
        assert(find(begin(arr), end(arr), 2) == end(arr));

        assert(count(begin(arr), end(arr), false) == 2);
        assert(count(begin(arr), end(arr), true) == 4);
        assert(count(begin(arr), end(arr), 2) == 0);
    }

    { // Test pointers
        const char* s = "xxxyyy";
        const char* arr[]{s, s + 1, s + 1, s + 5, s, s + 4};

        static_assert(_Vector_alg_in_find_is_safe<decltype(begin(arr)), decltype(s + 1)>, "should optimize");

        assert(find(begin(arr), end(arr), s) == begin(arr));
        assert(find(begin(arr), end(arr), s + 1) == begin(arr) + 1);
        assert(find(begin(arr), end(arr), s + 3) == end(arr));

        assert(count(begin(arr), end(arr), s + 1) == 2);
        assert(count(begin(arr), end(arr), s + 5) == 1);
        assert(count(begin(arr), end(arr), s + 3) == 0);
    }
}
