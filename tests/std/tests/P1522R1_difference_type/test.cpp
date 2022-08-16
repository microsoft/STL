// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <__msvc_int128.hpp>
#include <cassert>
#include <cstdint>
#include <iterator>
#include <limits>
#include <type_traits>

#if _HAS_CXX20
#include <compare>

#ifndef __EDG__ // TRANSITION, GH-395
#include <concepts>
#endif // __EDG__
#endif // _HAS_CXX20

#if _HAS_CXX20 && !defined(__EDG__) // TRANSITION, GH-395
#define SAME_AS std::same_as
#else // ^^^ has concepts / vvv has no concepts
#define SAME_AS std::is_same_v
#endif // _HAS_CXX20 && !defined(__EDG__)

#define STATIC_ASSERT(...) static_assert(__VA_ARGS__, #__VA_ARGS__)

using std::_Signed128;
using std::_Unsigned128;
using namespace std::literals::_Int128_literals;

template <class I>
constexpr void check_equal(const I& x) {
    assert(x == x);
    assert(!(x != x));
    assert(!(x < x));
    assert(!(x > x));
    assert(x <= x);
    assert(x >= x);
#if _HAS_CXX20
    assert(x <=> x == 0);
#endif // _HAS_CXX20
}

#if _HAS_CXX20
constexpr void check_order(const auto& x, const auto& y, const std::strong_ordering ord) {
    assert((x == y) == (ord == std::strong_ordering::equal));
    assert((x != y) == (ord != std::strong_ordering::equal));
    assert((x < y) == (ord == std::strong_ordering::less));
    assert((x > y) == (ord == std::strong_ordering::greater));
    assert((x <= y) == (ord != std::strong_ordering::greater));
    assert((x >= y) == (ord != std::strong_ordering::less));

    assert((y == x) == (ord == std::strong_ordering::equal));
    assert((y != x) == (ord != std::strong_ordering::equal));
    assert((y < x) == (ord == std::strong_ordering::greater));
    assert((y > x) == (ord == std::strong_ordering::less));
    assert((y <= x) == (ord != std::strong_ordering::less));
    assert((y >= x) == (ord != std::strong_ordering::greater));

    assert((x <=> y) == (ord <=> 0));
    assert((y <=> x) == (0 <=> ord));
}
#endif // _HAS_CXX20

constexpr bool test_unsigned() {
#if _HAS_CXX20 && !defined(__EDG__) // TRANSITION, GH-395
    STATIC_ASSERT(std::regular<_Unsigned128>);
    STATIC_ASSERT(std::three_way_comparable<_Unsigned128, std::strong_ordering>);
#endif // _HAS_CXX20 && !defined(__EDG__)

    STATIC_ASSERT(std::numeric_limits<_Unsigned128>::min() == 0);
    STATIC_ASSERT(std::numeric_limits<_Unsigned128>::max() == ~_Unsigned128{});
    STATIC_ASSERT(std::numeric_limits<_Unsigned128>::digits == 128);
    STATIC_ASSERT(std::numeric_limits<_Unsigned128>::is_modulo);

    STATIC_ASSERT(SAME_AS<std::common_type_t<bool, _Unsigned128>, _Unsigned128>);
    STATIC_ASSERT(SAME_AS<std::common_type_t<char, _Unsigned128>, _Unsigned128>);
    STATIC_ASSERT(SAME_AS<std::common_type_t<signed char, _Unsigned128>, _Unsigned128>);
    STATIC_ASSERT(SAME_AS<std::common_type_t<unsigned char, _Unsigned128>, _Unsigned128>);
    STATIC_ASSERT(SAME_AS<std::common_type_t<wchar_t, _Unsigned128>, _Unsigned128>);
#ifdef __cpp_char8_t
    STATIC_ASSERT(SAME_AS<std::common_type_t<char8_t, _Unsigned128>, _Unsigned128>);
#endif // __cpp_char8_t
    STATIC_ASSERT(SAME_AS<std::common_type_t<char16_t, _Unsigned128>, _Unsigned128>);
    STATIC_ASSERT(SAME_AS<std::common_type_t<char32_t, _Unsigned128>, _Unsigned128>);
    STATIC_ASSERT(SAME_AS<std::common_type_t<short, _Unsigned128>, _Unsigned128>);
    STATIC_ASSERT(SAME_AS<std::common_type_t<unsigned short, _Unsigned128>, _Unsigned128>);
    STATIC_ASSERT(SAME_AS<std::common_type_t<int, _Unsigned128>, _Unsigned128>);
    STATIC_ASSERT(SAME_AS<std::common_type_t<unsigned int, _Unsigned128>, _Unsigned128>);
    STATIC_ASSERT(SAME_AS<std::common_type_t<long, _Unsigned128>, _Unsigned128>);
    STATIC_ASSERT(SAME_AS<std::common_type_t<unsigned long, _Unsigned128>, _Unsigned128>);
    STATIC_ASSERT(SAME_AS<std::common_type_t<long long, _Unsigned128>, _Unsigned128>);
    STATIC_ASSERT(SAME_AS<std::common_type_t<unsigned long long, _Unsigned128>, _Unsigned128>);
    STATIC_ASSERT(SAME_AS<std::common_type_t<_Signed128, _Unsigned128>, _Unsigned128>);

    STATIC_ASSERT(SAME_AS<std::common_type_t<_Unsigned128, bool>, _Unsigned128>);
    STATIC_ASSERT(SAME_AS<std::common_type_t<_Unsigned128, char>, _Unsigned128>);
    STATIC_ASSERT(SAME_AS<std::common_type_t<_Unsigned128, signed char>, _Unsigned128>);
    STATIC_ASSERT(SAME_AS<std::common_type_t<_Unsigned128, unsigned char>, _Unsigned128>);
    STATIC_ASSERT(SAME_AS<std::common_type_t<_Unsigned128, wchar_t>, _Unsigned128>);
#ifdef __cpp_char8_t
    STATIC_ASSERT(SAME_AS<std::common_type_t<_Unsigned128, char8_t>, _Unsigned128>);
#endif // __cpp_char8_t
    STATIC_ASSERT(SAME_AS<std::common_type_t<_Unsigned128, char16_t>, _Unsigned128>);
    STATIC_ASSERT(SAME_AS<std::common_type_t<_Unsigned128, char32_t>, _Unsigned128>);
    STATIC_ASSERT(SAME_AS<std::common_type_t<_Unsigned128, short>, _Unsigned128>);
    STATIC_ASSERT(SAME_AS<std::common_type_t<_Unsigned128, unsigned short>, _Unsigned128>);
    STATIC_ASSERT(SAME_AS<std::common_type_t<_Unsigned128, int>, _Unsigned128>);
    STATIC_ASSERT(SAME_AS<std::common_type_t<_Unsigned128, unsigned int>, _Unsigned128>);
    STATIC_ASSERT(SAME_AS<std::common_type_t<_Unsigned128, long>, _Unsigned128>);
    STATIC_ASSERT(SAME_AS<std::common_type_t<_Unsigned128, unsigned long>, _Unsigned128>);
    STATIC_ASSERT(SAME_AS<std::common_type_t<_Unsigned128, long long>, _Unsigned128>);
    STATIC_ASSERT(SAME_AS<std::common_type_t<_Unsigned128, unsigned long long>, _Unsigned128>);
    STATIC_ASSERT(SAME_AS<std::common_type_t<_Unsigned128, _Signed128>, _Unsigned128>);

#if _HAS_CXX20 && !defined(__EDG__) // TRANSITION, GH-395
    STATIC_ASSERT(std::_Integer_class<_Unsigned128>);
    STATIC_ASSERT(std::_Integer_like<_Unsigned128>);
    STATIC_ASSERT(!std::_Signed_integer_like<_Unsigned128>);
    STATIC_ASSERT(SAME_AS<std::_Make_unsigned_like_t<_Unsigned128>, _Unsigned128>);
    STATIC_ASSERT(SAME_AS<std::_Make_signed_like_t<_Unsigned128>, _Signed128>);
#endif // _HAS_CXX20 && !defined(__EDG__)

    check_equal(_Unsigned128{});
    check_equal(_Unsigned128{42});
    check_equal(_Unsigned128{-42});
    check_equal(_Unsigned128{0x11111111'11111111, 0x22222222'22222222});
    check_equal(0x22222222'22222222'11111111'11111111__u128);

#if _HAS_CXX20
    check_order(_Unsigned128{42}, _Unsigned128{-42}, std::strong_ordering::less);
    check_order(_Unsigned128{0, 42}, _Unsigned128{0, 52}, std::strong_ordering::less); // ordered only by MSW
    check_order(_Unsigned128{42}, _Unsigned128{52}, std::strong_ordering::less); // ordered only by LSW
    check_order(_Unsigned128{0x11111111'11111111, 0x22222222'22222222},
        _Unsigned128{0x01010101'01010101, 0x01010101'01010101}, std::strong_ordering::greater);
    check_order(0x22222222'22222222'11111111'11111111__u128, 0x01010101'01010101'01010101'01010101__u128,
        std::strong_ordering::greater);
#endif // _HAS_CXX20

    {
        _Unsigned128 u{42};
        assert(u._Word[0] == 42);
        assert(u._Word[1] == 0);
        u += _Unsigned128{0};
        assert(u._Word[0] == 42);
        assert(u._Word[1] == 0);

        assert(static_cast<std::uint64_t>(u) == 42);
        assert(static_cast<std::uint32_t>(u) == 42);
        assert(static_cast<std::uint16_t>(u) == 42);
        assert(static_cast<std::uint8_t>(u) == 42);

        assert(static_cast<std::int64_t>(u) == 42);
        assert(static_cast<std::int32_t>(u) == 42);
        assert(static_cast<std::int16_t>(u) == 42);
        assert(static_cast<std::int8_t>(u) == 42);

        _Unsigned128 nu{-42};
        assert(nu._Word[0] == 0ull - 42);
        assert(nu._Word[1] == ~0ull);

        assert(static_cast<std::uint64_t>(nu) == 0ull - 42);
        assert(static_cast<std::uint32_t>(nu) == 0u - 42);
        assert(static_cast<std::uint16_t>(nu) == 65536 - 42);
        assert(static_cast<std::uint8_t>(nu) == 256 - 42);

        assert(static_cast<std::int64_t>(nu) == -42);
        assert(static_cast<std::int32_t>(nu) == -42);
        assert(static_cast<std::int16_t>(nu) == -42);
        assert(static_cast<std::int8_t>(nu) == -42);
        {
            _Unsigned128 sum = u + nu;
            assert(sum._Word[0] == 0);
            assert(sum._Word[1] == 0);

            --sum;
            assert(sum._Word[0] == 0ull - 1);
            assert(sum._Word[1] == 0ull - 1);
            --sum;
            assert(sum._Word[0] == 0ull - 2);
            assert(sum._Word[1] == 0ull - 1);
            ++sum;
            assert(sum._Word[0] == 0ull - 1);
            assert(sum._Word[1] == 0ull - 1);
            ++sum;
            assert(sum._Word[0] == 0);
            assert(sum._Word[1] == 0);
        }
        {
            _Unsigned128 product = u * u;
            assert(product._Word[0] == 42 * 42);
            assert(product._Word[1] == 0);

            product = nu * nu;
            assert(product._Word[0] == 42 * 42);
            assert(product._Word[1] == 0);

            product = _Unsigned128{0x01010101'01010101, 0x01010101'01010101} * 5;
            assert(product._Word[0] == 0x05050505'05050505);
            assert(product._Word[1] == 0x05050505'05050505);
            assert(product == 0x05050505'05050505'05050505'05050505__u128);

            product = 5 * _Unsigned128{0x01010101'01010101, 0x01010101'01010101};
            assert(product._Word[0] == 0x05050505'05050505);
            assert(product._Word[1] == 0x05050505'05050505);

            product = _Unsigned128{0x01010101'01010101, 0x01010101'01010101};
            product *= product;
            assert(product._Word[0] == 0x08070605'04030201);
            assert(product._Word[1] == 0x100f0e0d'0c0b0a09);
            assert(product == 0x100f0e0d'0c0b0a09'08070605'04030201__u128);
            assert(+product == product);
            assert(-product + product == 0);

            product = _Unsigned128{0x01020304'05060708, 0x090a0b0c'0d0e0f00}
                    * _Unsigned128{0x000f0e0d'0c0b0a09, 0x08070605'04030201};
            assert(product._Word[0] == 0x6dc18e55'16d48f48);
            assert(product._Word[1] == 0xdc1b6bce'43cd6c21);
            assert(product == 0xdc1b6bce'43cd6c21'6dc18e55'16d48f48__u128);
            assert(+product == product);
            assert(-product + product == 0);

            product <<= 11;
            assert(product._Word[0] == 0x0c72a8b6'a47a4000);
            assert(product._Word[1] == 0xdb5e721e'6b610b6e);
            assert(product == 0xdb5e721e'6b610b6e'0c72a8b6'a47a4000__u128);
            assert(+product == product);
            assert(-product + product == 0);

            product >>= 17;
            assert(product._Word[0] == 0x85b70639'545b523d);
            assert(product._Word[1] == 0x00006daf'390f35b0);
            assert(product == 0x00006daf'390f35b0'85b70639'545b523d__u128);
            assert(+product == product);
            assert(-product + product == 0);
        }
    }
    {
        _Unsigned128 q = _Unsigned128{13} / _Unsigned128{4};
        assert(q._Word[0] == 3);
        assert(q._Word[1] == 0);

        q = _Unsigned128{4} / _Unsigned128{13};
        assert(q._Word[0] == 0);
        assert(q._Word[1] == 0);

        q = _Unsigned128{0x01010101'01010101, 0x01010101'01010101} / _Unsigned128{13};
        assert(q._Word[0] == 0xc50013c5'0013c500);
        assert(q._Word[1] == 0x0013c500'13c50013);
        assert(q == 0x0013c500'13c50013'c50013c5'0013c500__u128);

        q = _Unsigned128{0x22222222'22222222, 0x22222222'22222222}
          / _Unsigned128{0x11111111'11111111, 0x11111111'11111111};
        assert(q._Word[0] == 2);
        assert(q._Word[1] == 0);

        q = (_Unsigned128{1} << 66) / (_Unsigned128{1} << 13);
        assert(q._Word[0] == 1ull << 53);
        assert(q._Word[1] == 0);
    }
    {
        auto tmp = ~_Unsigned128{};
        assert(tmp._Word[0] == ~0ull);
        assert(tmp._Word[1] == ~0ull);
        auto q = tmp / std::uint32_t{1};
        assert(q == tmp);
        q = tmp / std::uint64_t{1};
        assert(q == tmp);
        q = tmp / _Unsigned128{1};
        assert(q == tmp);

        q = tmp / _Unsigned128{1ull << 32};
        assert(q._Word[0] == ~0ull);
        assert(q._Word[1] == 0xffffffff);
    }
    {
        _Unsigned128 result{0x01010101'01010101, 0x01010101'01010101};
        _Unsigned128 tmp;
        for (tmp = 0xffu; static_cast<bool>(tmp); tmp <<= 8, result >>= 8) {
            auto q = ~_Unsigned128{} / tmp;
            assert(q == result);
            auto m    = ~_Unsigned128{} % tmp;
            auto p    = q * tmp;
            auto diff = ~_Unsigned128{} - p;
            assert(m == diff);
        }
        assert(tmp == 0);
    }
    {
        _Unsigned128 tmp;
        for (tmp = 1; static_cast<bool>(tmp); tmp <<= 1) {
            assert(tmp % tmp == 0);
        }
        assert(tmp == 0);
    }
    {
        const _Unsigned128 x{0x01020304'02030405, 0x03040506'04050607};
        const _Unsigned128 y{0x07060504'06050403, 0x05040302'04030101};
        assert(((x & y) == _Unsigned128{0x01020104'02010401, 0x01040102'04010001}));
        assert(((x & y) == 0x01040102'04010001'01020104'02010401__u128));
        auto tmp = x;
        tmp &= y;
        assert(tmp == (x & y));

        assert(((x | y) == _Unsigned128{0x07060704'06070407, 0x07040706'04070707}));
        assert(((x | y) == 0x07040706'04070707'07060704'06070407__u128));
        tmp = x;
        tmp |= y;
        assert(tmp == (x | y));

        assert(((x ^ y) == _Unsigned128{0x06040600'04060006, 0x06000604'00060706}));
        assert(((x ^ y) == 0x06000604'00060706'06040600'04060006__u128));
        tmp = x;
        tmp ^= y;
        assert(tmp == (x ^ y));
    }
    {
        _Unsigned128 x{};
        assert(++x == 1);
        assert(--x == 0);
        assert(x == std::numeric_limits<_Unsigned128>::min());
        --x;
        assert(x._Word[0] == ~0ull);
        assert(x._Word[1] == ~0ull);
        assert(x == std::numeric_limits<_Unsigned128>::max());
        --x;
        assert(x._Word[0] == (~0ull << 1));
        assert(x._Word[1] == ~0ull);
        x._Word[0] = 0;
        x._Word[1] = 1;
        --x;
        assert(x._Word[0] == ~0ull);
        assert(x._Word[1] == 0);
        ++x;
        assert(x._Word[0] == 0);
        assert(x._Word[1] == 1);
        ++x;
        assert(x._Word[0] == 1);
        assert(x._Word[1] == 1);
        x._Word[0] = ~0ull;
        x._Word[1] = static_cast<std::uint64_t>(std::numeric_limits<std::int64_t>::max());
        ++x;
        assert(x._Word[0] == 0);
        assert(x._Word[1] == static_cast<std::uint64_t>(std::numeric_limits<std::int64_t>::min()));
        --x;
        assert(x._Word[0] == ~0ull);
        assert(x._Word[1] == static_cast<std::uint64_t>(std::numeric_limits<std::int64_t>::max()));
    }
    {
        STATIC_ASSERT(noexcept(0__u128));
        STATIC_ASSERT(noexcept(42__u128));
        STATIC_ASSERT(noexcept(4'2__u128));
        STATIC_ASSERT(noexcept(052__u128));
        STATIC_ASSERT(noexcept(05'2__u128));
        STATIC_ASSERT(noexcept(0x2a__u128));
        STATIC_ASSERT(noexcept(0X2a__u128));
        STATIC_ASSERT(noexcept(0x2A__u128));
        STATIC_ASSERT(noexcept(0X2A__u128));
        STATIC_ASSERT(noexcept(0x2'A__u128));
        STATIC_ASSERT(noexcept(0b101010__u128));
        STATIC_ASSERT(noexcept(0b1'0101'0__u128));
        STATIC_ASSERT(noexcept(340282366920938463463374607431768211455__u128));
        STATIC_ASSERT(noexcept(0xffffffff'FFFFFFFF'ffffFFFF'FFFFffff__u128));

        STATIC_ASSERT(42__u128 == 42);
        STATIC_ASSERT(42__u128 == 42);
        STATIC_ASSERT(42__u128 == 052__u128);
        STATIC_ASSERT(42__u128 == 05'2__u128);
        STATIC_ASSERT(42__u128 == 0x2a__u128);
        STATIC_ASSERT(42__u128 == 0X2'a__u128);
        STATIC_ASSERT(42__u128 == 0b101010__u128);
        STATIC_ASSERT(42__u128 == 0b101'010__u128);
        STATIC_ASSERT(
            340'2823'6692'0938'4634'6337'4607'4317'6821'1455__u128 == 0xffffffff'FFFFFFFF'ffffFFFF'FFFFffff__u128);

        STATIC_ASSERT(
            340'282'366'920'938'463'463'374'607'431'768'211'455__u128 == std::numeric_limits<_Unsigned128>::max());
        STATIC_ASSERT(0xffffffff'ffffffff'ffffffff'ffffffff__u128 == std::numeric_limits<_Unsigned128>::max());
    }

    return true;
}

constexpr bool test_signed() {
#if _HAS_CXX20 && !defined(__EDG__) // TRANSITION, GH-395
    STATIC_ASSERT(std::regular<_Signed128>);
    STATIC_ASSERT(std::three_way_comparable<_Signed128, std::strong_ordering>);
#endif // _HAS_CXX20 && !defined(__EDG__)

    STATIC_ASSERT(std::numeric_limits<_Signed128>::min() == _Signed128{0, 1ull << 63});
    STATIC_ASSERT(std::numeric_limits<_Signed128>::max() == _Signed128{~0ull, ~0ull >> 1});
    STATIC_ASSERT(std::numeric_limits<_Signed128>::digits == 127);
    STATIC_ASSERT(!std::numeric_limits<_Signed128>::is_modulo);

    STATIC_ASSERT(SAME_AS<std::common_type_t<bool, _Signed128>, _Signed128>);
    STATIC_ASSERT(SAME_AS<std::common_type_t<char, _Signed128>, _Signed128>);
    STATIC_ASSERT(SAME_AS<std::common_type_t<signed char, _Signed128>, _Signed128>);
    STATIC_ASSERT(SAME_AS<std::common_type_t<unsigned char, _Signed128>, _Signed128>);
    STATIC_ASSERT(SAME_AS<std::common_type_t<wchar_t, _Signed128>, _Signed128>);
#ifdef __cpp_char8_t
    STATIC_ASSERT(SAME_AS<std::common_type_t<char8_t, _Signed128>, _Signed128>);
#endif // __cpp_char8_t
    STATIC_ASSERT(SAME_AS<std::common_type_t<char16_t, _Signed128>, _Signed128>);
    STATIC_ASSERT(SAME_AS<std::common_type_t<char32_t, _Signed128>, _Signed128>);
    STATIC_ASSERT(SAME_AS<std::common_type_t<short, _Signed128>, _Signed128>);
    STATIC_ASSERT(SAME_AS<std::common_type_t<unsigned short, _Signed128>, _Signed128>);
    STATIC_ASSERT(SAME_AS<std::common_type_t<int, _Signed128>, _Signed128>);
    STATIC_ASSERT(SAME_AS<std::common_type_t<unsigned int, _Signed128>, _Signed128>);
    STATIC_ASSERT(SAME_AS<std::common_type_t<long, _Signed128>, _Signed128>);
    STATIC_ASSERT(SAME_AS<std::common_type_t<unsigned long, _Signed128>, _Signed128>);
    STATIC_ASSERT(SAME_AS<std::common_type_t<long long, _Signed128>, _Signed128>);
    STATIC_ASSERT(SAME_AS<std::common_type_t<unsigned long long, _Signed128>, _Signed128>);

    STATIC_ASSERT(SAME_AS<std::common_type_t<_Signed128, bool>, _Signed128>);
    STATIC_ASSERT(SAME_AS<std::common_type_t<_Signed128, char>, _Signed128>);
    STATIC_ASSERT(SAME_AS<std::common_type_t<_Signed128, signed char>, _Signed128>);
    STATIC_ASSERT(SAME_AS<std::common_type_t<_Signed128, unsigned char>, _Signed128>);
    STATIC_ASSERT(SAME_AS<std::common_type_t<_Signed128, wchar_t>, _Signed128>);
#ifdef __cpp_char8_t
    STATIC_ASSERT(SAME_AS<std::common_type_t<_Signed128, char8_t>, _Signed128>);
#endif // __cpp_char8_t
    STATIC_ASSERT(SAME_AS<std::common_type_t<_Signed128, char16_t>, _Signed128>);
    STATIC_ASSERT(SAME_AS<std::common_type_t<_Signed128, char32_t>, _Signed128>);
    STATIC_ASSERT(SAME_AS<std::common_type_t<_Signed128, short>, _Signed128>);
    STATIC_ASSERT(SAME_AS<std::common_type_t<_Signed128, unsigned short>, _Signed128>);
    STATIC_ASSERT(SAME_AS<std::common_type_t<_Signed128, int>, _Signed128>);
    STATIC_ASSERT(SAME_AS<std::common_type_t<_Signed128, unsigned int>, _Signed128>);
    STATIC_ASSERT(SAME_AS<std::common_type_t<_Signed128, long>, _Signed128>);
    STATIC_ASSERT(SAME_AS<std::common_type_t<_Signed128, unsigned long>, _Signed128>);
    STATIC_ASSERT(SAME_AS<std::common_type_t<_Signed128, long long>, _Signed128>);
    STATIC_ASSERT(SAME_AS<std::common_type_t<_Signed128, unsigned long long>, _Signed128>);

#if _HAS_CXX20 && !defined(__EDG__) // TRANSITION, GH-395
    STATIC_ASSERT(std::_Integer_class<_Signed128>);
    STATIC_ASSERT(std::_Integer_like<_Signed128>);
    STATIC_ASSERT(std::_Signed_integer_like<_Signed128>);
    STATIC_ASSERT(SAME_AS<std::_Make_unsigned_like_t<_Signed128>, _Unsigned128>);
    STATIC_ASSERT(SAME_AS<std::_Make_signed_like_t<_Signed128>, _Signed128>);
#endif // _HAS_CXX20 && !defined(__EDG__)

    check_equal(_Signed128{});
    check_equal(_Signed128{42});
    check_equal(_Signed128{-42});
    check_equal(_Signed128{0x11111111'11111111, 0x22222222'22222222});
    check_equal(0x22222222'22222222'11111111'11111111__i128);

#if _HAS_CXX20
    check_order(_Signed128{42}, _Signed128{-42}, std::strong_ordering::greater);
    check_order(_Signed128{0x11111111'11111111, 0x22222222'22222222},
        _Signed128{0x01010101'01010101, 0x01010101'01010101}, std::strong_ordering::greater);
    check_order(0x22222222'22222222'11111111'11111111__i128, 0x01010101'01010101'01010101'01010101__i128,
        std::strong_ordering::greater);
    check_order(_Signed128{~0ull, ~0ull}, _Signed128{-1}, std::strong_ordering::equal);

    check_order(_Signed128{-2}, _Signed128{-1}, std::strong_ordering::less);
    check_order(_Signed128{-2}, _Signed128{1}, std::strong_ordering::less);
    check_order(_Signed128{2}, _Signed128{-1}, std::strong_ordering::greater);
    check_order(_Signed128{2}, _Signed128{1}, std::strong_ordering::greater);
#endif // _HAS_CXX20

    check_equal(_Signed128{0, 0});
#if _HAS_CXX20
    check_order(_Signed128{0, (1ull << 63)}, _Signed128{0, 0}, std::strong_ordering::less);
    check_order(_Signed128{0, (1ull << 63)}, _Signed128{0, (1ull << 63)}, std::strong_ordering::equal);
    check_order(_Signed128{0, 0}, _Signed128{1, 0}, std::strong_ordering::less);
    check_order(_Signed128{0, (1ull << 63)}, _Signed128{1, 0}, std::strong_ordering::less);
    check_order(_Signed128{0, (1ull << 63)}, _Signed128{1, (1ull << 63)}, std::strong_ordering::less);
    check_order(_Signed128{0, 0}, _Signed128{0, 1}, std::strong_ordering::less);
    check_order(_Signed128{0, (1ull << 63)}, _Signed128{0, 1}, std::strong_ordering::less);
    check_order(_Signed128{0, (1ull << 63)}, _Signed128{0, 1 | (1ull << 63)}, std::strong_ordering::less);
    check_order(_Signed128{0, 0}, _Signed128{1, 1}, std::strong_ordering::less);
    check_order(_Signed128{0, (1ull << 63)}, _Signed128{1, 1}, std::strong_ordering::less);
    check_order(_Signed128{0, (1ull << 63)}, _Signed128{1, 1 | (1ull << 63)}, std::strong_ordering::less);
#endif // _HAS_CXX20

    assert((_Signed128{-2} == _Unsigned128{0ull - 2, ~0ull}));
    assert((_Unsigned128{_Signed128{-2}} == _Unsigned128{0ull - 2, ~0ull}));
    assert((_Signed128{-2} == _Signed128{_Unsigned128{0ull - 2, ~0ull}}));

    {
        _Signed128 u{42};
        assert(u._Word[0] == 42);
        assert(u._Word[1] == 0);
        u += _Signed128{0};
        assert(u._Word[0] == 42);
        assert(u._Word[1] == 0);

        assert(static_cast<std::uint64_t>(u) == 42);
        assert(static_cast<std::uint32_t>(u) == 42);
        assert(static_cast<std::uint16_t>(u) == 42);
        assert(static_cast<std::uint8_t>(u) == 42);

        assert(static_cast<std::int64_t>(u) == 42);
        assert(static_cast<std::int32_t>(u) == 42);
        assert(static_cast<std::int16_t>(u) == 42);
        assert(static_cast<std::int8_t>(u) == 42);
    }
    {
        _Signed128 nu{-42};
        assert(nu._Word[0] == 0ull - 42);
        assert(nu._Word[1] == ~0ull);

        assert(static_cast<std::uint64_t>(nu) == 0ull - 42);
        assert(static_cast<std::uint32_t>(nu) == 0u - 42);
        assert(static_cast<std::uint16_t>(nu) == 65536 - 42);
        assert(static_cast<std::uint8_t>(nu) == 256 - 42);

        assert(static_cast<std::int64_t>(nu) == -42);
        assert(static_cast<std::int32_t>(nu) == -42);
        assert(static_cast<std::int16_t>(nu) == -42);
        assert(static_cast<std::int8_t>(nu) == -42);
    }

    {
        _Signed128 u{42};
        _Signed128 v{13};
        {
            _Signed128 sum = u + v;
            assert(sum._Word[0] == 55);
            assert(sum._Word[1] == 0);
            sum = v + u;
            assert(sum._Word[0] == 55);
            assert(sum._Word[1] == 0);
        }
        {
            _Signed128 diff = u - v;
            assert(diff._Word[0] == 29);
            assert(diff._Word[1] == 0);
            diff = v - u;
            assert(diff._Word[0] == 0ull - 29);
            assert(diff._Word[1] == ~0ull);
        }
        {
            u              = -u;
            _Signed128 sum = u + v;
            assert(sum._Word[0] == 0ull - 29);
            assert(sum._Word[1] == ~0ull);
            sum = v + u;
            assert(sum._Word[0] == 0ull - 29);
            assert(sum._Word[1] == ~0ull);
        }
        {
            _Signed128 diff = u - v;
            assert(diff._Word[0] == 0ull - 55);
            assert(diff._Word[1] == ~0ull);
            diff = v - u;
            assert(diff._Word[0] == 55);
            assert(diff._Word[1] == 0);
        }
        {
            u                  = -u;
            _Signed128 product = u * v;
            assert(product._Word[0] == 42 * 13);
            assert(product._Word[1] == 0);
            product = v * u;
            assert(product._Word[0] == 42 * 13);
            assert(product._Word[1] == 0);
        }
        {
            v                  = -v;
            _Signed128 product = u * v;
            assert(product._Word[0] == 0ull - (42 * 13));
            assert(product._Word[1] == ~0ull);
            product = v * u;
            assert(product._Word[0] == 0ull - (42 * 13));
            assert(product._Word[1] == ~0ull);
        }
        {
            u                  = -u;
            _Signed128 product = u * v;
            assert(product._Word[0] == 42 * 13);
            assert(product._Word[1] == 0);
            product = v * u;
            assert(product._Word[0] == 42 * 13);
            assert(product._Word[1] == 0);
        }
    }
    {
        auto product = _Signed128{0x01010101'01010101, 0x01010101'01010101} * 5;
        assert(product == 0x05050505'05050505'05050505'05050505__i128);
        assert(product._Word[0] == 0x05050505'05050505);
        assert(product._Word[1] == 0x05050505'05050505);

        product = 5 * _Signed128{0x01010101'01010101, 0x01010101'01010101};
        assert(product == 0x05050505'05050505'05050505'05050505__i128);
        assert(product._Word[0] == 0x05050505'05050505);
        assert(product._Word[1] == 0x05050505'05050505);
    }
    {
        auto product = _Signed128{0x01010101'01010101, 0x01010101'01010101};
        product *= product;
        assert(product == 0x100f0e0d'0c0b0a09'08070605'04030201__i128);
        assert(product._Word[0] == 0x08070605'04030201);
        assert(product._Word[1] == 0x100f0e0d'0c0b0a09);
        assert(+product == product);
        assert(-product + product == 0);
    }
    {
        auto product =
            _Signed128{0x01020304'05060708, 0x090a0b0c'0d0e0f00} * _Signed128{0x000f0e0d'0c0b0a09, 0x08070605'04030201};
        assert(product == -0x23e49431'bc3293de'923e71aa'e92b70b8__i128);
        assert(product._Word[0] == 0x6dc18e55'16d48f48);
        assert(product._Word[1] == 0xdc1b6bce'43cd6c21);
        assert(+product == product);
        assert(-product + product == 0);

        product <<= 11;
        assert(product == -0x24a18de1'949ef491'f38d5749'5b85c000__i128);
        assert(product._Word[0] == 0x0c72a8b6'a47a4000);
        assert(product._Word[1] == 0xdb5e721e'6b610b6e);
        assert(+product == product);
        assert(-product + product == 0);

        product >>= 17;
        assert(product == -0x00001250'c6f0ca4f'7a48f9c6'aba4adc3__i128);
        assert(product._Word[0] == 0x85b70639'545b523d);
        assert(product._Word[1] == 0xffffedaf'390f35b0);
        assert(+product == product);
        assert(-product + product == 0);
    }
    {
        _Signed128 q = _Signed128{13} / _Signed128{4};
        assert(q._Word[0] == 3);
        assert(q._Word[1] == 0);

        q = _Signed128{4} / _Signed128{13};
        assert(q._Word[0] == 0);
        assert(q._Word[1] == 0);

        q = _Signed128{13} / _Signed128{-4};
        assert(q._Word[0] == 0ull - 3);
        assert(q._Word[1] == ~0ull);

        q = _Signed128{-4} / _Signed128{13};
        assert(q._Word[0] == 0);
        assert(q._Word[1] == 0);

        q = _Signed128{-13} / _Signed128{4};
        assert(q._Word[0] == 0ull - 3);
        assert(q._Word[1] == ~0ull);

        q = _Signed128{4} / _Signed128{-13};
        assert(q._Word[0] == 0);
        assert(q._Word[1] == 0);

        q = _Signed128{-13} / _Signed128{-4};
        assert(q._Word[0] == 3);
        assert(q._Word[1] == 0);

        q = _Signed128{-4} / _Signed128{-13};
        assert(q._Word[0] == 0);
        assert(q._Word[1] == 0);

        q = _Signed128{0x01010101'01010101, 0x01010101'01010101} / _Signed128{13};
        assert(q == 0x0013c500'13c50013'c50013c5'0013c500__i128);
        assert(q._Word[0] == 0xc50013c5'0013c500);
        assert(q._Word[1] == 0x0013c500'13c50013);

        q = _Signed128{0x22222222'22222222, 0x22222222'22222222} / _Signed128{0x11111111'11111111, 0x11111111'11111111};
        assert(q._Word[0] == 2);
        assert(q._Word[1] == 0);

        q = (_Signed128{1} << 66) / (_Signed128{1} << 13);
        assert(q._Word[0] == 1ull << 53);
        assert(q._Word[1] == 0);
    }
    {
        auto tmp = ~_Signed128{};
        assert(tmp._Word[0] == ~0ull);
        assert(tmp._Word[1] == ~0ull);
        _Signed128 q = tmp / std::uint32_t{1};
        assert(q == tmp);
        q = tmp / std::uint64_t{1};
        assert(q == tmp);
        q = tmp / _Signed128{1};
        assert(q == tmp);

        q = tmp / _Signed128{1ull << 32};
        assert(q._Word[0] == 0);
        assert(q._Word[1] == 0);
    }
    {
        _Signed128 result{0x80808080'80808080, 0x00808080'80808080};
        for (_Signed128 tmp = 0xffu; tmp > 0; tmp <<= 8, result >>= 8) {
            auto q = std::numeric_limits<_Signed128>::max() / tmp;
            assert(q == result);
            auto m    = std::numeric_limits<_Signed128>::max() % tmp;
            auto p    = q * tmp;
            auto diff = std::numeric_limits<_Signed128>::max() - p;
            assert(m == diff);
        }
    }
    {
        _Signed128 result = _Signed128{0x7f7f7f7f'7f7f7f80, 0xff7f7f7f'7f7f7f7f};
        for (_Signed128 tmp = 0xffu; tmp > 0; tmp <<= 8, result = (result >> 8) + 1) {
            auto q = std::numeric_limits<_Signed128>::min() / tmp;
            assert(q == result);
            auto m    = std::numeric_limits<_Signed128>::min() % tmp;
            auto p    = q * tmp;
            auto diff = std::numeric_limits<_Signed128>::min() - p;
            assert(m == diff);
        }
    }
    {
        _Signed128 tmp;
        for (tmp = 1; static_cast<bool>(tmp); tmp <<= 1) {
            assert(tmp % tmp == 0);
        }
        assert(tmp == 0);
    }
    {
        _Signed128 r = _Signed128{13} % _Signed128{4};
        assert(r._Word[0] == 1);
        assert(r._Word[1] == 0);

        r = _Signed128{4} % _Signed128{13};
        assert(r._Word[0] == 4);
        assert(r._Word[1] == 0);

        r = _Signed128{13} % _Signed128{-4};
        assert(r._Word[0] == 1);
        assert(r._Word[1] == 0);

        r = _Signed128{-4} % _Signed128{13};
        assert(r._Word[0] == 0ull - 4);
        assert(r._Word[1] == ~0ull);

        r = _Signed128{-13} % _Signed128{4};
        assert(r._Word[0] == 0ull - 1);
        assert(r._Word[1] == ~0ull);

        r = _Signed128{4} % _Signed128{-13};
        assert(r._Word[0] == 4);
        assert(r._Word[1] == 0);

        r = _Signed128{-13} % _Signed128{-4};
        assert(r._Word[0] == 0ull - 1);
        assert(r._Word[1] == ~0ull);

        r = _Signed128{-4} % _Signed128{-13};
        assert(r._Word[0] == 0ull - 4);
        assert(r._Word[1] == ~0ull);
    }
    {
        const _Signed128 x{0x01020304'02030405, 0x03040506'04050607};
        const _Signed128 y{0x07060504'06050403, 0x05040302'04030101};
        assert(((x & y) == _Signed128{0x01020104'02010401, 0x01040102'04010001}));
        assert(((x & y) == 0x01040102'04010001'01020104'02010401__i128));
        auto tmp = x;
        tmp &= y;
        assert(tmp == (x & y));

        assert(((x | y) == _Signed128{0x07060704'06070407, 0x07040706'04070707}));
        assert(((x | y) == 0x07040706'04070707'07060704'06070407__i128));
        tmp = x;
        tmp |= y;
        assert(tmp == (x | y));

        assert(((x ^ y) == _Signed128{0x06040600'04060006, 0x06000604'00060706}));
        assert(((x ^ y) == 0x06000604'00060706'06040600'04060006__i128));
        tmp = x;
        tmp ^= y;
        assert(tmp == (x ^ y));
    }
    {
        _Signed128 x{};
        assert(++x == 1);
        assert(--x == 0);
        --x;
        assert(x._Word[0] == ~0ull);
        assert(x._Word[1] == ~0ull);
        --x;
        assert(x._Word[0] == (~0ull << 1));
        assert(x._Word[1] == ~0ull);
        x._Word[0] = 0;
        x._Word[1] = 1;
        --x;
        assert(x._Word[0] == ~0ull);
        assert(x._Word[1] == 0);
        ++x;
        assert(x._Word[0] == 0);
        assert(x._Word[1] == 1);
        ++x;
        assert(x._Word[0] == 1);
        assert(x._Word[1] == 1);
        x._Word[0] = ~0ull;
        x._Word[1] = static_cast<std::uint64_t>(std::numeric_limits<std::int64_t>::max());
        ++x;
        assert(x._Word[0] == 0);
        assert(x._Word[1] == static_cast<std::uint64_t>(std::numeric_limits<std::int64_t>::min()));
        assert(x == std::numeric_limits<_Signed128>::min());
        --x;
        assert(x._Word[0] == ~0ull);
        assert(x._Word[1] == static_cast<std::uint64_t>(std::numeric_limits<std::int64_t>::max()));
        assert(x == std::numeric_limits<_Signed128>::max());
    }
    {
        STATIC_ASSERT(noexcept(0__i128));
        STATIC_ASSERT(noexcept(42__i128));
        STATIC_ASSERT(noexcept(4'2__i128));
        STATIC_ASSERT(noexcept(052__i128));
        STATIC_ASSERT(noexcept(05'2__i128));
        STATIC_ASSERT(noexcept(0x2a__i128));
        STATIC_ASSERT(noexcept(0X2a__i128));
        STATIC_ASSERT(noexcept(0x2A__i128));
        STATIC_ASSERT(noexcept(0X2A__i128));
        STATIC_ASSERT(noexcept(0x2'A__i128));
        STATIC_ASSERT(noexcept(0b101010__i128));
        STATIC_ASSERT(noexcept(0b1'0101'0__i128));
        STATIC_ASSERT(noexcept(170141183460469231731687303715884105727__i128));
        STATIC_ASSERT(noexcept(0x7fffffff'FFFFFFFF'ffffFFFF'FFFFffff__i128));

        STATIC_ASSERT(42__i128 == 42);
        STATIC_ASSERT(42__i128 == 42);
        STATIC_ASSERT(42__i128 == 052__i128);
        STATIC_ASSERT(42__i128 == 05'2__i128);
        STATIC_ASSERT(42__i128 == 0x2a__i128);
        STATIC_ASSERT(42__i128 == 0X2'a__i128);
        STATIC_ASSERT(42__i128 == 0b101010__i128);
        STATIC_ASSERT(42__i128 == 0b101'010__i128);
        STATIC_ASSERT(
            170'1411'8346'0469'2317'3168'7303'7158'8410'5727__i128 == 0x7fffffff'FFFFFFFF'ffffFFFF'FFFFffff__i128);

        STATIC_ASSERT(
            170'141'183'460'469'231'731'687'303'715'884'105'727__i128 == std::numeric_limits<_Signed128>::max());
        STATIC_ASSERT(0x7fffffff'ffffffff'ffffffff'ffffffff__i128 == std::numeric_limits<_Signed128>::max());

        STATIC_ASSERT(
            -170'141'183'460'469'231'731'687'303'715'884'105'727__i128 - 1 == std::numeric_limits<_Signed128>::min());
        STATIC_ASSERT(-0x7fffffff'ffffffff'ffffffff'ffffffff__i128 - 1 == std::numeric_limits<_Signed128>::min());
    }

    return true;
}

template <class T>
T val() noexcept;

#if _HAS_CXX20 && !defined(__EDG__) // TRANSITION, GH-395
template <class T, class U>
concept CanConditional = requires {
    true ? val<T>() : val<U>();
};
#else // ^^^ has concepts / vvv has no concepts
template <class T, class U, class = void>
constexpr bool CanConditional = false;

template <class T, class U>
constexpr bool CanConditional<T, U, std::void_t<decltype(true ? val<T>() : val<U>())>> = true;
#endif // _HAS_CXX20 && !defined(__EDG__)

constexpr bool test_cross() {
    // Test the behavior of cross-type operations.

#define TEST(expr, result)                                                                 \
    do {                                                                                   \
        STATIC_ASSERT(SAME_AS<decltype((expr)), std::remove_const_t<decltype((result))>>); \
        assert((expr) == (result));                                                        \
    } while (0)

    //////// Mixed integer-class operands

    // With mixed operands, binary arithmetic operators convert the signed
    // operand to unsigned, producing an unsigned result.

    TEST(_Unsigned128{42} + _Signed128{-43}, _Unsigned128{-1});
    TEST(_Signed128{42} + _Unsigned128{-43}, _Unsigned128{-1});
    TEST(_Unsigned128{42} - _Signed128{-43}, _Unsigned128{42 + 43});
    TEST(_Signed128{42} - _Unsigned128{-43}, _Unsigned128{42 + 43});
    TEST(_Unsigned128{42} * _Signed128{-43}, _Unsigned128{42 * -43});
    TEST(_Signed128{42} * _Unsigned128{-43}, _Unsigned128{42 * -43});
    TEST(_Unsigned128{42} / _Signed128{-43}, _Unsigned128{0});
    TEST(_Signed128{42} / _Unsigned128{-43}, _Unsigned128{0});
    TEST(_Unsigned128{42} % _Signed128{-43}, _Unsigned128{42});
    TEST(_Signed128{42} % _Unsigned128{-43}, _Unsigned128{42});
    TEST(_Unsigned128{42} & _Signed128{43}, _Unsigned128{42});
    TEST(_Signed128{42} & _Unsigned128{43}, _Unsigned128{42});
    TEST(_Unsigned128{42} | _Signed128{43}, _Unsigned128{43});
    TEST(_Signed128{42} | _Unsigned128{43}, _Unsigned128{43});
    TEST(_Unsigned128{42} ^ _Signed128{43}, _Unsigned128{1});
    TEST(_Signed128{42} ^ _Unsigned128{43}, _Unsigned128{1});

    // Shifts yield a result of the left operand's type.

    TEST(_Unsigned128{1} << _Signed128{43}, _Unsigned128{1ull << 43});
    TEST(_Signed128{1} << _Unsigned128{43}, _Signed128{1ull << 43});
    TEST(_Unsigned128{-1} << _Signed128{43}, (_Unsigned128{0ull - (1ull << 43), ~0ull}));
    TEST(_Signed128{-1} << _Unsigned128{43}, (_Signed128{0ull - (1ull << 43), ~0ull}));

    TEST((_Unsigned128{0, 1}) >> _Signed128{43}, _Unsigned128{1ull << 21});
    TEST((_Signed128{0, 1}) >> _Unsigned128{43}, _Signed128{1ull << 21});
    TEST((_Unsigned128{0, ~0ull} >> _Signed128{43}), (_Unsigned128{~((1ull << 21) - 1), (1ull << 21) - 1}));
    TEST((_Signed128{0, ~0ull} >> _Unsigned128{43}), (_Signed128{~((1ull << 21) - 1), ~0ull}));

    // Integer-class types are explicitly convertible to integer-like (the union
    // of integer-class and integral) types. Integer-class types are implicitly
    // convertible only to wider types, or types of the same width and
    // signedness. Consequently, the conditional operator should reject integer-
    // class operands with the same width but differing signedness.
    STATIC_ASSERT(!CanConditional<_Unsigned128, _Signed128>);
    STATIC_ASSERT(!CanConditional<_Signed128, _Unsigned128>);

#if _HAS_CXX20 && !defined(__EDG__) // TRANSITION, GH-395
    // Conversions between integer-class types with the same width and differing
    // signedness are narrowing, so the three-way comparison operator should
    // reject mixed operands of such types.
    STATIC_ASSERT(!std::three_way_comparable_with<_Unsigned128, _Signed128>);
    STATIC_ASSERT(!std::three_way_comparable_with<_Signed128, _Unsigned128>);
#endif // _HAS_CXX20 && !defined(__EDG__)

    // Other comparison operators behave as they do for operands of mixed
    // integral types; when the operands have the same width, the signed operand
    // converts to unsigned and the comparison is made.

    TEST(_Unsigned128{42} == _Signed128{0}, false);
    TEST(_Signed128{42} == _Unsigned128{42}, true);
    TEST(_Unsigned128{42} != _Signed128{0}, true);
    TEST(_Signed128{42} != _Unsigned128{42}, false);

    TEST(_Unsigned128{42} < _Signed128{-43}, true);
    TEST(_Signed128{42} < _Unsigned128{-43}, true);
    TEST(_Unsigned128{42} > _Signed128{-43}, false);
    TEST(_Signed128{42} > _Unsigned128{-43}, false);
    TEST(_Unsigned128{42} <= _Signed128{-43}, true);
    TEST(_Signed128{42} <= _Unsigned128{-43}, true);
    TEST(_Unsigned128{42} >= _Signed128{-43}, false);
    TEST(_Signed128{42} >= _Unsigned128{-43}, false);

    // The logical binary operators behave just like they do for integral types;
    // the individual operands are converted to bool and the operation performed
    // normally.

    TEST(_Unsigned128{42} && _Signed128{0}, false);
    TEST(_Signed128{42} && _Unsigned128{0}, false);
    TEST(_Unsigned128{42} || _Signed128{0}, true);
    TEST(_Signed128{42} || _Unsigned128{0}, true);

    // Assignments convert the RHS to the type of the LHS, just as for integral
    // types.
    {
        _Unsigned128 u{};
        _Unsigned128 x{42};
        _Signed128 i{42};
        _Signed128 y{13};
        TEST(u = i, x);
        u = 13;
        TEST(i = u, y);

        x = 26;
        TEST(u += i, x);
        y = 39;
        TEST(i += u, y);

        x = -13;
        TEST(u -= i, x);
        y = 52;
        TEST(i -= u, y);

        x = -26;
        TEST(u *= _Signed128{2}, x);
        y = 104;
        TEST(i *= _Unsigned128{2}, y);

        x = _Unsigned128{0x55555555'5555554c, 0x55555555'55555555};
        TEST(u /= _Signed128{3}, x); // Yes, u is still unsigned =)
        y = 34;
        TEST(i /= _Unsigned128{3}, y);

        x = 4;
        TEST(u %= _Signed128{8}, x);
        y = 2;
        TEST(i %= _Unsigned128{8}, y);

        x = 16;
        TEST(u <<= _Signed128{2}, x);
        y = 8;
        TEST(i <<= _Unsigned128{2}, y);

        x = 8;
        TEST(u >>= _Signed128{1}, x);
        y = 4;
        TEST(i >>= _Unsigned128{1}, y);

        x = 9;
        TEST(u |= _Signed128{1}, x);
        y = 5;
        TEST(i |= _Unsigned128{1}, y);

        x = 9;
        TEST(u &= _Signed128{59}, x);
        y = 1;
        TEST(i &= _Unsigned128{59}, y);

        x = 12;
        TEST(u ^= _Signed128{5}, x);
        y = 4;
        TEST(i ^= _Unsigned128{5}, y);
    }

    //////// Mixed integer and integer-class operands

    // With mixed operands, binary arithmetic operators convert the signed
    // operand to unsigned, producing an unsigned result.

    TEST(_Unsigned128{42} + -43, _Unsigned128{-1});
    TEST(_Signed128{42} + -43, _Signed128{-1});
    TEST(42 + _Signed128{-43}, _Signed128{-1});
    TEST(42 + _Unsigned128{-43}, _Unsigned128{-1});
    TEST(_Unsigned128{42} - -43, _Unsigned128{42 + 43});
    TEST(_Signed128{42} - -43, _Signed128{42 + 43});
    TEST(42 - _Signed128{-43}, _Signed128{42 + 43});
    TEST(42 - _Unsigned128{-43}, _Unsigned128{42 + 43});
    TEST(_Unsigned128{42} * -43, _Unsigned128{42 * -43});
    TEST(_Signed128{42} * -43, _Signed128{42 * -43});
    TEST(42 * _Signed128{-43}, _Signed128{42 * -43});
    TEST(42 * _Unsigned128{-43}, _Unsigned128{42 * -43});
    TEST(_Unsigned128{42} / -43, _Unsigned128{0});
    TEST(_Signed128{42} / -43, _Signed128{0});
    TEST(42 / _Signed128{-43}, _Signed128{0});
    TEST(42 / _Unsigned128{-43}, _Unsigned128{0});
    TEST(_Unsigned128{42} % -43, _Unsigned128{42});
    TEST(_Signed128{42} % -43, _Signed128{42});
    TEST(42 % _Signed128{-43}, _Signed128{42});
    TEST(42 % _Unsigned128{-43}, _Unsigned128{42});
    TEST(_Unsigned128{42} & 43, _Unsigned128{42});
    TEST(_Signed128{42} & 43, _Signed128{42});
    TEST(42 & _Signed128{43}, _Signed128{42});
    TEST(42 & _Unsigned128{43}, _Unsigned128{42});
    TEST(_Unsigned128{42} | 43, _Unsigned128{43});
    TEST(_Signed128{42} | 43, _Signed128{43});
    TEST(42 | _Signed128{43}, _Signed128{43});
    TEST(42 | _Unsigned128{43}, _Unsigned128{43});
    TEST(_Unsigned128{42} ^ 43, _Unsigned128{1});
    TEST(_Signed128{42} ^ 43, _Signed128{1});
    TEST(42 ^ _Signed128{43}, _Signed128{1});
    TEST(42 ^ _Unsigned128{43}, _Unsigned128{1});

    // Shifts yield a result of the left operand's type.

    TEST(_Unsigned128{1} << 4, _Unsigned128{1ull << 4});
    TEST(_Signed128{1} << 4, _Signed128{1ull << 4});
    TEST(1 << _Signed128{4}, 16);
    TEST(1 << _Unsigned128{4}, 16);

    TEST((_Unsigned128{0, 1}) >> 3, _Unsigned128{1ull << 61});
    TEST((_Signed128{0, 1}) >> 3, _Signed128{1ull << 61});
    TEST(256 >> _Signed128{3}, 32);
    TEST(256 >> _Unsigned128{3}, 32);

    TEST(true ? _Unsigned128{42} : 13, _Unsigned128{42});
    TEST(true ? _Signed128{42} : 13, _Signed128{42});
    TEST(true ? 42 : _Signed128{13}, _Signed128{42});
    TEST(true ? 42 : _Unsigned128{13}, _Unsigned128{42});

#if _HAS_CXX20
    // (meow <=> 0) here is a hack to get prvalues
    TEST(4 <=> _Unsigned128{3}, (std::strong_ordering::greater <=> 0));
    TEST(4 <=> _Signed128{3}, (std::strong_ordering::greater <=> 0));
    TEST(_Signed128{4} <=> 3, (std::strong_ordering::greater <=> 0));
    TEST(_Unsigned128{4} <=> 3, (std::strong_ordering::greater <=> 0));
    TEST(3 <=> _Unsigned128{3}, (std::strong_ordering::equal <=> 0));
    TEST(3 <=> _Signed128{3}, (std::strong_ordering::equal <=> 0));
    TEST(_Signed128{3} <=> 3, (std::strong_ordering::equal <=> 0));
    TEST(_Unsigned128{3} <=> 3, (std::strong_ordering::equal <=> 0));
    TEST(-3 <=> _Unsigned128{3}, (std::strong_ordering::greater <=> 0));
    TEST(-3 <=> _Signed128{3}, (std::strong_ordering::less <=> 0));
    TEST(_Signed128{-3} <=> 3, (std::strong_ordering::less <=> 0));
    TEST(_Unsigned128{-3} <=> 3, (std::strong_ordering::greater <=> 0));
#endif // _HAS_CXX20

    // Other comparison operators behave as they do for operands of mixed
    // integral types; when the operands have the same width, the signed operand
    // converts to unsigned and the comparison is made.

    TEST(_Unsigned128{42} == 0, false);
    TEST(_Signed128{42} == 42, true);
    TEST(42 == _Signed128{0}, false);
    TEST(42 == _Unsigned128{42}, true);
    TEST(_Unsigned128{42} != 0, true);
    TEST(_Signed128{42} != 42, false);
    TEST(42 != _Signed128{0}, true);
    TEST(42 != _Unsigned128{42}, false);

    TEST(_Unsigned128{42} < -43, true);
    TEST(_Signed128{42} < -43, false);
    TEST(42 < _Signed128{-43}, false);
    TEST(42 < _Unsigned128{-43}, true);
    TEST(_Unsigned128{42} > -43, false);
    TEST(_Signed128{42} > -43, true);
    TEST(42 > _Signed128{-43}, true);
    TEST(42 > _Unsigned128{-43}, false);
    TEST(_Unsigned128{42} <= -43, true);
    TEST(_Signed128{42} <= -43, false);
    TEST(42 <= _Signed128{-43}, false);
    TEST(42 <= _Unsigned128{-43}, true);
    TEST(_Unsigned128{42} >= -43, false);
    TEST(_Signed128{42} >= -43, true);
    TEST(42 >= _Signed128{-43}, true);
    TEST(42 >= _Unsigned128{-43}, false);

    // The logical binary operators behave just like they do for integral types;
    // the individual operands are converted to bool and the operation performed
    // normally.

    TEST(_Unsigned128{42} && 0, false);
    TEST(_Signed128{42} && 0, false);
    TEST(42 && _Signed128{0}, false);
    TEST(42 && _Unsigned128{0}, false);
    TEST(_Unsigned128{42} || 0, true);
    TEST(_Signed128{42} || 0, true);
    TEST(42 || _Signed128{0}, true);
    TEST(42 || _Unsigned128{0}, true);

    // Assignments convert the RHS to the type of the LHS, just as for integral
    // types.
    {
        _Unsigned128 u;
        _Signed128 i;
        _Unsigned128 x = 42;
        _Signed128 y   = 13;
        TEST(u = 42, x);
        TEST(i = 13, y);

        x = 55;
        TEST(u += 13, x);
        y = 26;
        TEST(i += 13, y);

        x = -13;
        TEST(u -= 68, x);
        y = -6;
        TEST(i -= 32, y);

        x = -26;
        TEST(u *= 2, x);
        y = 12;
        TEST(i *= -2, y);

        x = _Unsigned128{0x55555555'5555554c, 0x55555555'55555555};
        TEST(u /= 3, x); // Yes, u is still unsigned =)
        y = 4;
        TEST(i /= 3, y);

        x = 4;
        TEST(u %= 8, x);
        y = 4;
        TEST(i %= 8, y);

        x = 16;
        TEST(u <<= 2, x);
        y = 16;
        TEST(i <<= 2, y);

        x = 8;
        TEST(u >>= 1, x);
        y = 8;
        TEST(i >>= 1, y);

        x = 9;
        TEST(u |= 9, x);
        y = 10;
        TEST(i |= 2, y);

        x = 9;
        TEST(u &= 59, x);
        y = 10;
        TEST(i &= 59, y);

        x = 12;
        TEST(u ^= 5, x);
        y = 15;
        TEST(i ^= 5, y);
    }
#undef TEST

    return true;
}

int main() {
    test_unsigned();
    STATIC_ASSERT(test_unsigned());
    test_signed();
    STATIC_ASSERT(test_signed());
    test_cross();
    STATIC_ASSERT(test_cross());
}
