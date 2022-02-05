// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <__msvc_int128.hpp>
#include <cassert>
#include <compare>
#include <concepts>
#include <cstdint>
#include <iterator>
#include <limits>
#include <type_traits>

using std::_Int128;
using std::_Uint128;

constexpr void check_equal(const auto& x) {
    assert(x == x);
    assert(!(x != x));
    assert(!(x < x));
    assert(!(x > x));
    assert(x <= x);
    assert(x >= x);
    assert(x <=> x == 0);
}

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

constexpr bool test_unsigned() {
    static_assert(std::regular<_Uint128>);
    static_assert(std::three_way_comparable<_Uint128, std::strong_ordering>);

    static_assert(std::numeric_limits<_Uint128>::min() == 0);
    static_assert(std::numeric_limits<_Uint128>::max() == ~_Uint128{});
    static_assert(std::numeric_limits<_Uint128>::digits == 128);
    static_assert(std::numeric_limits<_Uint128>::is_modulo);

    static_assert(std::same_as<std::common_type_t<bool, _Uint128>, _Uint128>);
    static_assert(std::same_as<std::common_type_t<char, _Uint128>, _Uint128>);
    static_assert(std::same_as<std::common_type_t<signed char, _Uint128>, _Uint128>);
    static_assert(std::same_as<std::common_type_t<unsigned char, _Uint128>, _Uint128>);
    static_assert(std::same_as<std::common_type_t<wchar_t, _Uint128>, _Uint128>);
#ifdef __cpp_char8_t
    static_assert(std::same_as<std::common_type_t<char8_t, _Uint128>, _Uint128>);
#endif // __cpp_char8_t
    static_assert(std::same_as<std::common_type_t<char16_t, _Uint128>, _Uint128>);
    static_assert(std::same_as<std::common_type_t<char32_t, _Uint128>, _Uint128>);
    static_assert(std::same_as<std::common_type_t<short, _Uint128>, _Uint128>);
    static_assert(std::same_as<std::common_type_t<unsigned short, _Uint128>, _Uint128>);
    static_assert(std::same_as<std::common_type_t<int, _Uint128>, _Uint128>);
    static_assert(std::same_as<std::common_type_t<unsigned int, _Uint128>, _Uint128>);
    static_assert(std::same_as<std::common_type_t<long, _Uint128>, _Uint128>);
    static_assert(std::same_as<std::common_type_t<unsigned long, _Uint128>, _Uint128>);
    static_assert(std::same_as<std::common_type_t<long long, _Uint128>, _Uint128>);
    static_assert(std::same_as<std::common_type_t<unsigned long long, _Uint128>, _Uint128>);

    static_assert(std::same_as<std::common_type_t<_Uint128, bool>, _Uint128>);
    static_assert(std::same_as<std::common_type_t<_Uint128, char>, _Uint128>);
    static_assert(std::same_as<std::common_type_t<_Uint128, signed char>, _Uint128>);
    static_assert(std::same_as<std::common_type_t<_Uint128, unsigned char>, _Uint128>);
    static_assert(std::same_as<std::common_type_t<_Uint128, wchar_t>, _Uint128>);
#ifdef __cpp_char8_t
    static_assert(std::same_as<std::common_type_t<_Uint128, char8_t>, _Uint128>);
#endif // __cpp_char8_t
    static_assert(std::same_as<std::common_type_t<_Uint128, char16_t>, _Uint128>);
    static_assert(std::same_as<std::common_type_t<_Uint128, char32_t>, _Uint128>);
    static_assert(std::same_as<std::common_type_t<_Uint128, short>, _Uint128>);
    static_assert(std::same_as<std::common_type_t<_Uint128, unsigned short>, _Uint128>);
    static_assert(std::same_as<std::common_type_t<_Uint128, int>, _Uint128>);
    static_assert(std::same_as<std::common_type_t<_Uint128, unsigned int>, _Uint128>);
    static_assert(std::same_as<std::common_type_t<_Uint128, long>, _Uint128>);
    static_assert(std::same_as<std::common_type_t<_Uint128, unsigned long>, _Uint128>);
    static_assert(std::same_as<std::common_type_t<_Uint128, long long>, _Uint128>);
    static_assert(std::same_as<std::common_type_t<_Uint128, unsigned long long>, _Uint128>);

    static_assert(std::_Integer_class<_Uint128>);
    static_assert(std::_Integer_like<_Uint128>);
    static_assert(!std::_Signed_integer_like<_Uint128>);
    static_assert(std::same_as<std::_Make_unsigned_like_t<_Uint128>, _Uint128>);
    static_assert(std::same_as<std::_Make_signed_like_t<_Uint128>, _Int128>);

    check_equal(_Uint128{});
    check_equal(_Uint128{42});
    check_equal(_Uint128{-42});
    check_equal(_Uint128{0x11111111'11111111, 0x22222222'22222222});

    check_order(_Uint128{42}, _Uint128{-42}, std::strong_ordering::less);
    check_order(_Uint128{0, 42}, _Uint128{0, 52}, std::strong_ordering::less); // ordered only by MSW
    check_order(_Uint128{42}, _Uint128{52}, std::strong_ordering::less); // ordered only by LSW
    check_order(_Uint128{0x11111111'11111111, 0x22222222'22222222}, _Uint128{0x01010101'01010101, 0x01010101'01010101},
        std::strong_ordering::greater);

    {
        _Uint128 u{42};
        assert(u._Word[0] == 42);
        assert(u._Word[1] == 0);
        u += _Uint128{0};
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

        _Uint128 nu{-42};
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

        _Uint128 sum = u + nu;
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

        _Uint128 product = u * u;
        assert(product._Word[0] == 42 * 42);
        assert(product._Word[1] == 0);

        product = nu * nu;
        assert(product._Word[0] == 42 * 42);
        assert(product._Word[1] == 0);

        product = _Uint128{0x01010101'01010101, 0x01010101'01010101} * 5;
        assert(product._Word[0] == 0x05050505'05050505);
        assert(product._Word[1] == 0x05050505'05050505);

        product = 5 * _Uint128{0x01010101'01010101, 0x01010101'01010101};
        assert(product._Word[0] == 0x05050505'05050505);
        assert(product._Word[1] == 0x05050505'05050505);

        product = _Uint128{0x01010101'01010101, 0x01010101'01010101};
        product *= product;
        assert(product._Word[0] == 0x08070605'04030201);
        assert(product._Word[1] == 0x100f0e0d'0c0b0a09);
        assert(+product == product);
        assert(-product + product == 0);

        product =
            _Uint128{0x01020304'05060708, 0x090a0b0c'0d0e0f00} * _Uint128{0x000f0e0d'0c0b0a09, 0x08070605'04030201};
        assert(product._Word[0] == 0x6dc18e55'16d48f48);
        assert(product._Word[1] == 0xdc1b6bce'43cd6c21);
        assert(+product == product);
        assert(-product + product == 0);

        product <<= 11;
        assert(product._Word[0] == 0x0c72a8b6'a47a4000);
        assert(product._Word[1] == 0xdb5e721e'6b610b6e);
        assert(+product == product);
        assert(-product + product == 0);

        product >>= 17;
        assert(product._Word[0] == 0x85b70639'545b523d);
        assert(product._Word[1] == 0x00006daf'390f35b0);
        assert(+product == product);
        assert(-product + product == 0);
    }

    {
        _Uint128 q = _Uint128{13} / _Uint128{4};
        assert(q._Word[0] == 3);
        assert(q._Word[1] == 0);

        q = _Uint128{4} / _Uint128{13};
        assert(q._Word[0] == 0);
        assert(q._Word[1] == 0);

        q = _Uint128{0x01010101'01010101, 0x01010101'01010101} / _Uint128{13};
        assert(q._Word[0] == 0xc50013c5'0013c500);
        assert(q._Word[1] == 0x0013c500'13c50013);

        q = _Uint128{0x22222222'22222222, 0x22222222'22222222} / _Uint128{0x11111111'11111111, 0x11111111'11111111};
        assert(q._Word[0] == 2);
        assert(q._Word[1] == 0);

        q = (_Uint128{1} << 66) / (_Uint128{1} << 13);
        assert(q._Word[0] == 1ull << 53);
        assert(q._Word[1] == 0);

        auto tmp = ~_Uint128{};
        assert(tmp._Word[0] == ~0ull);
        assert(tmp._Word[1] == ~0ull);
        q = tmp / std::uint32_t{1};
        assert(q == tmp);
        q = tmp / std::uint64_t{1};
        assert(q == tmp);
        q = tmp / _Uint128{1};
        assert(q == tmp);

        q = tmp / _Uint128{1ull << 32};
        assert(q._Word[0] == ~0ull);
        assert(q._Word[1] == 0xffffffff);

        _Uint128 result{0x01010101'01010101, 0x01010101'01010101};
        for (tmp = 0xffu; static_cast<bool>(tmp); tmp <<= 8, result >>= 8) {
            q = ~_Uint128{} / tmp;
            assert(q == result);
            auto m    = ~_Uint128{} % tmp;
            auto p    = q * tmp;
            auto diff = ~_Uint128{} - p;
            assert(m == diff);
        }
        assert(tmp == 0);

        for (tmp = 1; static_cast<bool>(tmp); tmp <<= 1) {
            assert(tmp % tmp == 0);
        }
        assert(tmp == 0);
    }

    {
        const _Uint128 x{0x01020304'02030405, 0x03040506'04050607};
        const _Uint128 y{0x07060504'06050403, 0x05040302'04030101};
        assert(((x & y) == _Uint128{0x01020104'02010401, 0x01040102'04010001}));
        auto tmp = x;
        tmp &= y;
        assert(tmp == (x & y));

        assert(((x | y) == _Uint128{0x07060704'06070407, 0x07040706'04070707}));
        tmp = x;
        tmp |= y;
        assert(tmp == (x | y));

        assert(((x ^ y) == _Uint128{0x06040600'04060006, 0x06000604'00060706}));
        tmp = x;
        tmp ^= y;
        assert(tmp == (x ^ y));
    }

    {
        _Uint128 x{};
        assert(++x == 1);
        assert(--x == 0);
        assert(x == std::numeric_limits<_Uint128>::min());
        --x;
        assert(x._Word[0] == ~0ull);
        assert(x._Word[1] == ~0ull);
        assert(x == std::numeric_limits<_Uint128>::max());
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

    return true;
}

constexpr bool test_signed() {
    static_assert(std::regular<_Int128>);
    static_assert(std::three_way_comparable<_Int128, std::strong_ordering>);

    static_assert(std::numeric_limits<_Int128>::min() == _Int128{0, 1ull << 63});
    static_assert(std::numeric_limits<_Int128>::max() == _Int128{~0ull, ~0ull >> 1});
    static_assert(std::numeric_limits<_Int128>::digits == 127);
    static_assert(!std::numeric_limits<_Int128>::is_modulo);

    static_assert(std::same_as<std::common_type_t<bool, _Int128>, _Int128>);
    static_assert(std::same_as<std::common_type_t<char, _Int128>, _Int128>);
    static_assert(std::same_as<std::common_type_t<signed char, _Int128>, _Int128>);
    static_assert(std::same_as<std::common_type_t<unsigned char, _Int128>, _Int128>);
    static_assert(std::same_as<std::common_type_t<wchar_t, _Int128>, _Int128>);
#ifdef __cpp_char8_t
    static_assert(std::same_as<std::common_type_t<char8_t, _Int128>, _Int128>);
#endif // __cpp_char8_t
    static_assert(std::same_as<std::common_type_t<char16_t, _Int128>, _Int128>);
    static_assert(std::same_as<std::common_type_t<char32_t, _Int128>, _Int128>);
    static_assert(std::same_as<std::common_type_t<short, _Int128>, _Int128>);
    static_assert(std::same_as<std::common_type_t<unsigned short, _Int128>, _Int128>);
    static_assert(std::same_as<std::common_type_t<int, _Int128>, _Int128>);
    static_assert(std::same_as<std::common_type_t<unsigned int, _Int128>, _Int128>);
    static_assert(std::same_as<std::common_type_t<long, _Int128>, _Int128>);
    static_assert(std::same_as<std::common_type_t<unsigned long, _Int128>, _Int128>);
    static_assert(std::same_as<std::common_type_t<long long, _Int128>, _Int128>);
    static_assert(std::same_as<std::common_type_t<unsigned long long, _Int128>, _Int128>);

    static_assert(std::same_as<std::common_type_t<_Int128, bool>, _Int128>);
    static_assert(std::same_as<std::common_type_t<_Int128, char>, _Int128>);
    static_assert(std::same_as<std::common_type_t<_Int128, signed char>, _Int128>);
    static_assert(std::same_as<std::common_type_t<_Int128, unsigned char>, _Int128>);
    static_assert(std::same_as<std::common_type_t<_Int128, wchar_t>, _Int128>);
#ifdef __cpp_char8_t
    static_assert(std::same_as<std::common_type_t<_Int128, char8_t>, _Int128>);
#endif // __cpp_char8_t
    static_assert(std::same_as<std::common_type_t<_Int128, char16_t>, _Int128>);
    static_assert(std::same_as<std::common_type_t<_Int128, char32_t>, _Int128>);
    static_assert(std::same_as<std::common_type_t<_Int128, short>, _Int128>);
    static_assert(std::same_as<std::common_type_t<_Int128, unsigned short>, _Int128>);
    static_assert(std::same_as<std::common_type_t<_Int128, int>, _Int128>);
    static_assert(std::same_as<std::common_type_t<_Int128, unsigned int>, _Int128>);
    static_assert(std::same_as<std::common_type_t<_Int128, long>, _Int128>);
    static_assert(std::same_as<std::common_type_t<_Int128, unsigned long>, _Int128>);
    static_assert(std::same_as<std::common_type_t<_Int128, long long>, _Int128>);
    static_assert(std::same_as<std::common_type_t<_Int128, unsigned long long>, _Int128>);

    static_assert(std::_Integer_class<_Int128>);
    static_assert(std::_Integer_like<_Int128>);
    static_assert(std::_Signed_integer_like<_Int128>);
    static_assert(std::same_as<std::_Make_unsigned_like_t<_Int128>, _Uint128>);
    static_assert(std::same_as<std::_Make_signed_like_t<_Int128>, _Int128>);

    check_equal(_Int128{});
    check_equal(_Int128{42});
    check_equal(_Int128{-42});
    check_equal(_Int128{0x11111111'11111111, 0x22222222'22222222});

    check_order(_Int128{42}, _Int128{-42}, std::strong_ordering::greater);
    check_order(_Int128{0x11111111'11111111, 0x22222222'22222222}, _Int128{0x01010101'01010101, 0x01010101'01010101},
        std::strong_ordering::greater);
    check_order(_Int128{~0ull, ~0ull}, _Int128{-1}, std::strong_ordering::equal);

    check_order(_Int128{-2}, _Int128{-1}, std::strong_ordering::less);
    check_order(_Int128{-2}, _Int128{1}, std::strong_ordering::less);
    check_order(_Int128{2}, _Int128{-1}, std::strong_ordering::greater);
    check_order(_Int128{2}, _Int128{1}, std::strong_ordering::greater);

    check_equal(_Int128{0, 0});
    check_order(_Int128{0, (1ull << 63)}, _Int128{0, 0}, std::strong_ordering::less);
    check_order(_Int128{0, (1ull << 63)}, _Int128{0, (1ull << 63)}, std::strong_ordering::equal);
    check_order(_Int128{0, 0}, _Int128{1, 0}, std::strong_ordering::less);
    check_order(_Int128{0, (1ull << 63)}, _Int128{1, 0}, std::strong_ordering::less);
    check_order(_Int128{0, (1ull << 63)}, _Int128{1, (1ull << 63)}, std::strong_ordering::less);
    check_order(_Int128{0, 0}, _Int128{0, 1}, std::strong_ordering::less);
    check_order(_Int128{0, (1ull << 63)}, _Int128{0, 1}, std::strong_ordering::less);
    check_order(_Int128{0, (1ull << 63)}, _Int128{0, 1 | (1ull << 63)}, std::strong_ordering::less);
    check_order(_Int128{0, 0}, _Int128{1, 1}, std::strong_ordering::less);
    check_order(_Int128{0, (1ull << 63)}, _Int128{1, 1}, std::strong_ordering::less);
    check_order(_Int128{0, (1ull << 63)}, _Int128{1, 1 | (1ull << 63)}, std::strong_ordering::less);

    assert((_Int128{-2} == _Uint128{0ull - 2, ~0ull}));
    assert((_Uint128{_Int128{-2}} == _Uint128{0ull - 2, ~0ull}));
    assert((_Int128{-2} == _Int128{_Uint128{0ull - 2, ~0ull}}));

    {
        _Int128 u{42};
        assert(u._Word[0] == 42);
        assert(u._Word[1] == 0);
        u += _Int128{0};
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

        _Int128 nu{-42};
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
        _Int128 u{42};
        _Int128 v{13};
        _Int128 sum = u + v;
        assert(sum._Word[0] == 55);
        assert(sum._Word[1] == 0);
        sum = v + u;
        assert(sum._Word[0] == 55);
        assert(sum._Word[1] == 0);
        _Int128 diff = u - v;
        assert(diff._Word[0] == 29);
        assert(diff._Word[1] == 0);
        diff = v - u;
        assert(diff._Word[0] == 0ull - 29);
        assert(diff._Word[1] == ~0ull);

        u   = -u;
        sum = u + v;
        assert(diff._Word[0] == 0ull - 29);
        assert(diff._Word[1] == ~0ull);
        sum = v + u;
        assert(diff._Word[0] == 0ull - 29);
        assert(diff._Word[1] == ~0ull);
        diff = u - v;
        assert(diff._Word[0] == 0ull - 55);
        assert(diff._Word[1] == ~0ull);
        diff = v - u;
        assert(diff._Word[0] == 55);
        assert(diff._Word[1] == 0);

        u               = -u;
        _Int128 product = u * v;
        assert(product._Word[0] == 42 * 13);
        assert(product._Word[1] == 0);
        product = v * u;
        assert(product._Word[0] == 42 * 13);
        assert(product._Word[1] == 0);

        v       = -v;
        product = u * v;
        assert(product._Word[0] == 0ull - (42 * 13));
        assert(product._Word[1] == ~0ull);
        product = v * u;
        assert(product._Word[0] == 0ull - (42 * 13));
        assert(product._Word[1] == ~0ull);

        u       = -u;
        product = u * v;
        assert(product._Word[0] == 42 * 13);
        assert(product._Word[1] == 0);
        product = v * u;
        assert(product._Word[0] == 42 * 13);
        assert(product._Word[1] == 0);

        product = _Int128{0x01010101'01010101, 0x01010101'01010101} * 5;
        assert(product._Word[0] == 0x05050505'05050505);
        assert(product._Word[1] == 0x05050505'05050505);

        product = 5 * _Int128{0x01010101'01010101, 0x01010101'01010101};
        assert(product._Word[0] == 0x05050505'05050505);
        assert(product._Word[1] == 0x05050505'05050505);

        product = _Int128{0x01010101'01010101, 0x01010101'01010101};
        product *= product;
        assert(product._Word[0] == 0x08070605'04030201);
        assert(product._Word[1] == 0x100f0e0d'0c0b0a09);
        assert(+product == product);
        assert(-product + product == 0);

        product = _Int128{0x01020304'05060708, 0x090a0b0c'0d0e0f00} * _Int128{0x000f0e0d'0c0b0a09, 0x08070605'04030201};
        assert(product._Word[0] == 0x6dc18e55'16d48f48);
        assert(product._Word[1] == 0xdc1b6bce'43cd6c21);
        assert(+product == product);
        assert(-product + product == 0);

        product <<= 11;
        assert(product._Word[0] == 0x0c72a8b6'a47a4000);
        assert(product._Word[1] == 0xdb5e721e'6b610b6e);
        assert(+product == product);
        assert(-product + product == 0);

        product >>= 17;
        assert(product._Word[0] == 0x85b70639'545b523d);
        assert(product._Word[1] == 0xffffedaf'390f35b0);
        assert(+product == product);
        assert(-product + product == 0);
    }

    {
        _Int128 q = _Int128{13} / _Int128{4};
        assert(q._Word[0] == 3);
        assert(q._Word[1] == 0);

        q = _Int128{4} / _Int128{13};
        assert(q._Word[0] == 0);
        assert(q._Word[1] == 0);

        q = _Int128{13} / _Int128{-4};
        assert(q._Word[0] == 0ull - 3);
        assert(q._Word[1] == ~0ull);

        q = _Int128{-4} / _Int128{13};
        assert(q._Word[0] == 0);
        assert(q._Word[1] == 0);

        q = _Int128{-13} / _Int128{4};
        assert(q._Word[0] == 0ull - 3);
        assert(q._Word[1] == ~0ull);

        q = _Int128{4} / _Int128{-13};
        assert(q._Word[0] == 0);
        assert(q._Word[1] == 0);

        q = _Int128{-13} / _Int128{-4};
        assert(q._Word[0] == 3);
        assert(q._Word[1] == 0);

        q = _Int128{-4} / _Int128{-13};
        assert(q._Word[0] == 0);
        assert(q._Word[1] == 0);

        q = _Int128{0x01010101'01010101, 0x01010101'01010101} / _Int128{13};
        assert(q._Word[0] == 0xc50013c5'0013c500);
        assert(q._Word[1] == 0x0013c500'13c50013);

        q = _Int128{0x22222222'22222222, 0x22222222'22222222} / _Int128{0x11111111'11111111, 0x11111111'11111111};
        assert(q._Word[0] == 2);
        assert(q._Word[1] == 0);

        q = (_Int128{1} << 66) / (_Int128{1} << 13);
        assert(q._Word[0] == 1ull << 53);
        assert(q._Word[1] == 0);

        auto tmp = ~_Int128{};
        assert(tmp._Word[0] == ~0ull);
        assert(tmp._Word[1] == ~0ull);
        q = tmp / std::uint32_t{1};
        assert(q == tmp);
        q = tmp / std::uint64_t{1};
        assert(q == tmp);
        q = tmp / _Int128{1};
        assert(q == tmp);

        q = tmp / _Int128{1ull << 32};
        assert(q._Word[0] == 0);
        assert(q._Word[1] == 0);

        _Int128 result{0x80808080'80808080, 0x00808080'80808080};
        for (tmp = 0xffu; tmp > 0; tmp <<= 8, result >>= 8) {
            q = std::numeric_limits<_Int128>::max() / tmp;
            assert(q == result);
            auto m    = std::numeric_limits<_Int128>::max() % tmp;
            auto p    = q * tmp;
            auto diff = std::numeric_limits<_Int128>::max() - p;
            assert(m == diff);
        }

        result = _Int128{0x7f7f7f7f'7f7f7f80, 0xff7f7f7f'7f7f7f7f};
        for (tmp = 0xffu; tmp > 0; tmp <<= 8, result = (result >> 8) + 1) {
            q = std::numeric_limits<_Int128>::min() / tmp;
            assert(q == result);
            auto m    = std::numeric_limits<_Int128>::min() % tmp;
            auto p    = q * tmp;
            auto diff = std::numeric_limits<_Int128>::min() - p;
            assert(m == diff);
        }

        for (tmp = 1; static_cast<bool>(tmp); tmp <<= 1) {
            assert(tmp % tmp == 0);
        }
        assert(tmp == 0);

        _Int128 r = _Int128{13} % _Int128{4};
        assert(r._Word[0] == 1);
        assert(r._Word[1] == 0);

        r = _Int128{4} % _Int128{13};
        assert(r._Word[0] == 4);
        assert(r._Word[1] == 0);

        r = _Int128{13} % _Int128{-4};
        assert(r._Word[0] == 1);
        assert(r._Word[1] == 0);

        r = _Int128{-4} % _Int128{13};
        assert(r._Word[0] == 0ull - 4);
        assert(r._Word[1] == ~0ull);

        r = _Int128{-13} % _Int128{4};
        assert(r._Word[0] == 0ull - 1);
        assert(r._Word[1] == ~0ull);

        r = _Int128{4} % _Int128{-13};
        assert(r._Word[0] == 4);
        assert(r._Word[1] == 0);

        r = _Int128{-13} % _Int128{-4};
        assert(r._Word[0] == 0ull - 1);
        assert(r._Word[1] == ~0ull);

        r = _Int128{-4} % _Int128{-13};
        assert(r._Word[0] == 0ull - 4);
        assert(r._Word[1] == ~0ull);
    }

    {
        const _Int128 x{0x01020304'02030405, 0x03040506'04050607};
        const _Int128 y{0x07060504'06050403, 0x05040302'04030101};
        assert(((x & y) == _Int128{0x01020104'02010401, 0x01040102'04010001}));
        auto tmp = x;
        tmp &= y;
        assert(tmp == (x & y));

        assert(((x | y) == _Int128{0x07060704'06070407, 0x07040706'04070707}));
        tmp = x;
        tmp |= y;
        assert(tmp == (x | y));

        assert(((x ^ y) == _Int128{0x06040600'04060006, 0x06000604'00060706}));
        tmp = x;
        tmp ^= y;
        assert(tmp == (x ^ y));
    }

    {
        _Int128 x{};
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
        assert(x == std::numeric_limits<_Int128>::min());
        --x;
        assert(x._Word[0] == ~0ull);
        assert(x._Word[1] == static_cast<std::uint64_t>(std::numeric_limits<std::int64_t>::max()));
        assert(x == std::numeric_limits<_Int128>::max());
    }

    return true;
}

template <class T>
T val() noexcept;

template <class T, class U>
concept CanConditional = requires {
    true ? val<T>() : val<U>();
};

constexpr bool test_cross() {
#define TEST(expr, result)                                                                  \
    do {                                                                                    \
        static_assert(std::same_as<decltype(expr), std::remove_const_t<decltype(result)>>); \
        assert((expr) == (result));                                                         \
    } while (0)

    TEST(_Uint128{42} + _Int128{-43}, _Uint128{-1});
    TEST(_Int128{42} + _Uint128{-43}, _Uint128{-1});
    TEST(_Uint128{42} + _Int128{-43}, _Uint128{-1});
    TEST(_Int128{42} + _Uint128{-43}, _Uint128{-1});
    TEST(_Uint128{42} - _Int128{-43}, _Uint128{42 + 43});
    TEST(_Int128{42} - _Uint128{-43}, _Uint128{42 + 43});
    TEST(_Uint128{42} * _Int128{-43}, _Uint128{42 * -43});
    TEST(_Int128{42} * _Uint128{-43}, _Uint128{42 * -43});
    TEST(_Uint128{42} / _Int128{-43}, _Uint128{0});
    TEST(_Int128{42} / _Uint128{-43}, _Uint128{0});
    TEST(_Uint128{42} % _Int128{-43}, _Uint128{42});
    TEST(_Int128{42} % _Uint128{-43}, _Uint128{42});
    TEST(_Uint128{42} & _Int128{43}, _Uint128{42});
    TEST(_Int128{42} & _Uint128{43}, _Uint128{42});
    TEST(_Uint128{42} | _Int128{43}, _Uint128{43});
    TEST(_Int128{42} | _Uint128{43}, _Uint128{43});
    TEST(_Uint128{42} ^ _Int128{43}, _Uint128{1});
    TEST(_Int128{42} ^ _Uint128{43}, _Uint128{1});

    TEST(_Uint128{1} << _Int128{43}, _Uint128{1ull << 43});
    TEST(_Int128{1} << _Uint128{43}, _Int128{1ull << 43});
    TEST(_Uint128{-1} << _Int128{43}, _Uint128(0ull - (1ull << 43), ~0ull));
    TEST(_Int128{-1} << _Uint128{43}, _Int128(0ull - (1ull << 43), ~0ull));

    TEST(_Uint128(0, 1) >> _Int128{43}, _Uint128(1ull << 21));
    TEST(_Int128(0, 1) >> _Uint128{43}, _Int128(1ull << 21));
    TEST(_Uint128(0, ~0ull) >> _Int128{43}, _Uint128(~((1ull << 21) - 1), (1ull << 21) - 1));
    TEST(_Int128(0, ~0ull) >> _Uint128{43}, _Int128(~((1ull << 21) - 1), ~0ull));

    static_assert(!CanConditional<_Uint128, _Int128>);
    static_assert(!CanConditional<_Int128, _Uint128>);

    TEST(_Uint128{42} && _Int128{0}, false);
    TEST(_Int128{42} && _Uint128{0}, false);
    TEST(_Uint128{42} || _Int128{0}, true);
    TEST(_Int128{42} || _Uint128{0}, true);

    TEST(_Uint128{42} == _Int128{0}, false);
    TEST(_Int128{42} == _Uint128{42}, true);
    TEST(_Uint128{42} != _Int128{0}, true);
    TEST(_Int128{42} != _Uint128{42}, false);

    static_assert(!std::three_way_comparable_with<_Uint128, _Int128>);
    static_assert(!std::three_way_comparable_with<_Int128, _Uint128>);

    TEST(_Uint128{42} < _Int128{-43}, true);
    TEST(_Int128{42} < _Uint128{-43}, true);
    TEST(_Uint128{42} > _Int128{-43}, false);
    TEST(_Int128{42} > _Uint128{-43}, false);
    TEST(_Uint128{42} <= _Int128{-43}, true);
    TEST(_Int128{42} <= _Uint128{-43}, true);
    TEST(_Uint128{42} >= _Int128{-43}, false);
    TEST(_Int128{42} >= _Uint128{-43}, false);

#undef TEST

    return true;
}

int main() {
    test_unsigned();
    static_assert(test_unsigned());
    test_signed();
    static_assert(test_signed());
    test_cross();
    // FIXME static_assert(test_cross());
}
