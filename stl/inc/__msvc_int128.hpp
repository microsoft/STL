// __msvc_int128 internal header (core)

// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#pragma once
#ifndef __MSVC_INT128_HPP
#define __MSVC_INT128_HPP

#include <yvals_core.h>
#if _STL_COMPILER_PREPROCESSOR
#ifdef __cpp_lib_concepts
#include <bit>
#include <compare>
#include <concepts>
#include <cstdint>
#include <intrin.h> // TODO: move things we need to intrin0

#if 1 // FIXME: comment out _STL_INTERNAL_CHECKs and remove to make this core
#include <yvals.h>
#endif

#pragma pack(push, _CRT_PACKING)
#pragma warning(push, _STL_WARNING_LEVEL)
#pragma warning(disable : _STL_DISABLED_WARNINGS)
_STL_DISABLE_CLANG_WARNINGS
#pragma push_macro("new")
#undef new

_STD_BEGIN

struct _Int128;

struct alignas(16) _Uint128 {
    uint64_t _Word[2];

    using _Signed_type   = _Int128;
    using _Unsigned_type = _Uint128;

    constexpr void _Left_shift(const unsigned char _Count) noexcept {
        if (_Count >= 64) {
            _Word[1] = _Word[0] << (_Count % 64);
            _Word[0] = 0;
            return;
        }

        if (_Count != 0) {
#if defined(_M_X64) && !defined(_M_ARM64EC)
            if (!_STD is_constant_evaluated()) {
                _Word[1] = __shiftleft128(_Word[0], _Word[1], _Count);
            } else
#endif // defined(_M_X64) && !defined(_M_ARM64EC)
            {
                _Word[1] = (_Word[1] << _Count) | (_Word[0] >> (64 - _Count));
            }

            _Word[0] <<= _Count;
        }
    }

    constexpr void _Right_shift(const unsigned char _Count) noexcept {
        if (_Count >= 64) {
            _Word[0] = _Word[1] >> (_Count % 64);
            _Word[1] = 0;
            return;
        }

        if (_Count != 0) {
#if defined(_M_X64) && !defined(_M_ARM64EC)
            if (!_STD is_constant_evaluated()) {
                _Word[0] = __shiftright128(_Word[0], _Word[1], _Count);
            } else
#endif // defined(_M_X64) && !defined(_M_ARM64EC)
            {
                _Word[0] = (_Word[0] >> _Count) | (_Word[1] << (64 - _Count));
            }

            _Word[1] >>= _Count;
        }
    }

    static constexpr unsigned char _AddCarry64(
        unsigned char _Carry, uint64_t _Left, uint64_t _Right, uint64_t& _Result) noexcept {
        _STL_INTERNAL_CHECK(_Carry < 2);
#if defined(_M_X64) && !defined(_M_ARM64EC)
        if (!_STD is_constant_evaluated()) {
            return _addcarry_u64(_Carry, _Left, _Right, &_Result);
        }
#endif // defined(_M_X64) && !defined(_M_ARM64EC)

        const uint64_t _Sum = _Left + _Right + _Carry;
        _Result             = _Sum;
        return _Carry ? _Sum <= _Left : _Sum < _Left;
    }

    static constexpr unsigned char _SubBorrow64(
        unsigned char _Carry, uint64_t _Left, uint64_t _Right, uint64_t& _Result) noexcept {
#if defined(_M_X64) && !defined(_M_ARM64EC)
        if (!_STD is_constant_evaluated()) {
            return _subborrow_u64(_Carry, _Left, _Right, &_Result);
        }
#endif // defined(_M_X64) && !defined(_M_ARM64EC)
        const auto _Difference = _Left - _Right - _Carry;
        _Result                = _Difference;
        return _Carry ? _Difference >= _Left : _Difference > _Left;
    }

    template <size_t __m, size_t __n>
    static constexpr void _Knuth_4_3_1_M(
        const uint32_t (&__u)[__m], const uint32_t (&__v)[__n], uint32_t (&__w)[__n + __m]) noexcept {
        _STL_INTERNAL_STATIC_ASSERT(__m <= numeric_limits<int>::max());
        _STL_INTERNAL_STATIC_ASSERT(__n <= numeric_limits<int>::max());

        for (auto& _Elem : __w) {
            _Elem = 0;
        }

        for (int __j = 0; __j < static_cast<int>(__n); ++__j) {
            // stash Knuth's `k` in the lower 32 bits of __t
            uint64_t __t = 0;
            for (int __i = 0; __i < static_cast<int>(__m); ++__i) {
                __t += static_cast<uint64_t>(__u[__i]) * __v[__j] + __w[__i + __j];
                __w[__i + __j] = static_cast<uint32_t>(__t);
                __t >>= 32;
            }
            __w[__j + __m] = static_cast<uint32_t>(__t);
        }
    }

    _NODISCARD static constexpr uint64_t _UMul128(
        const uint64_t _Left, const uint64_t _Right, uint64_t& _High_result) noexcept {
#if defined(_M_X64) && !defined(_M_ARM64EC)
        if (!_STD is_constant_evaluated()) {
            return _umul128(_Left, _Right, &_High_result);
        }
#endif // defined(_M_X64) && !defined(_M_ARM64EC)

        const uint32_t __u[2] = {
            static_cast<uint32_t>(_Left),
            static_cast<uint32_t>(_Left >> 32),
        };
        const uint32_t __v[2] = {
            static_cast<uint32_t>(_Right),
            static_cast<uint32_t>(_Right >> 32),
        };
        uint32_t __w[4];

        // multiply 2-digit numbers with 4-digit result in base 2^32
        _Knuth_4_3_1_M(__u, __v, __w);

        _High_result = (static_cast<uint64_t>(__w[3]) << 32) | __w[2];
        return (static_cast<uint64_t>(__w[1]) << 32) | __w[0];
    }

    static constexpr void _Knuth_4_3_1_D_impl(uint32_t __u[/* __n + __m + 1 */], const uint32_t __v[/* __n */],
        uint32_t __q[/* __m + 1 */], const size_t __n, const size_t __m) noexcept {
        _STL_INTERNAL_CHECK(__v[__n - 1] >> 31 != 0); // Arguments are already normalized
        _STL_INTERNAL_CHECK(__m <= numeric_limits<int>::max());
        _STL_INTERNAL_CHECK(__n <= numeric_limits<int>::max());

        for (auto __j = static_cast<int>(__m); __j >= 0; --__j) {
            const auto _Two_digits = (static_cast<uint64_t>(__u[__j + __n]) << 32) | __u[__j + __n - 1];
            auto __qhat            = _Two_digits / __v[__n - 1];
            auto __rhat            = _Two_digits % __v[__n - 1];

            while ((__qhat >> 32) != 0
                   || static_cast<uint32_t>(__qhat) * static_cast<uint64_t>(__v[__n - 2])
                          > ((__rhat << 32) | __u[__j + __n - 2])) {
                --__qhat;
                __rhat += __v[__n - 1];
                if ((__rhat >> 32) != 0) {
                    break;
                }
            }

            int64_t __k = 0;
            int64_t __t;
            for (int __i = 0; __i < static_cast<int>(__n); ++__i) {
                const auto _Prod = static_cast<uint32_t>(__qhat) * static_cast<uint64_t>(__v[__i]);
                __t              = __u[__i + __j] - __k - static_cast<uint32_t>(_Prod);
                __u[__i + __j]   = static_cast<uint32_t>(__t);
                __k              = static_cast<int64_t>((_Prod >> 32)) - (__t >> 32);
            }
            __t            = __u[__j + __n] - __k;
            __u[__j + __n] = static_cast<uint32_t>(__t);

            __q[__j] = static_cast<uint32_t>(__qhat);
            if (__t < 0) {
                --__q[__j];
                __k = 0;
                for (int __i = 0; __i < static_cast<int>(__n); ++__i) {
                    __t            = __u[__i + __j] + __k + __v[__i];
                    __u[__i + __j] = static_cast<uint32_t>(__t);
                    __k            = __t >> 32;
                }
                __u[__j + __n] += static_cast<int32_t>(__k);
            }
        }

        // quotient is in __q, normalized remainder is in __u
    }

    template <size_t __m_plus_n_plus_one, size_t __n>
    static constexpr void _Knuth_4_3_1_D(uint32_t (&__u)[__m_plus_n_plus_one], const uint32_t (&__v)[__n],
        uint32_t (&__q)[__m_plus_n_plus_one - __n]) noexcept {
        _Knuth_4_3_1_D_impl(__u, __v, __q, __n, __m_plus_n_plus_one - (__n + 1));
    }

    _NODISCARD static constexpr uint64_t _UDiv128(
        uint64_t _High, uint64_t _Low, uint64_t _Div, uint64_t& _Remainder) noexcept {
        _STL_INTERNAL_CHECK(_High < _Div);

#if !defined(__clang__) && defined(_M_X64) && !defined(_M_ARM64EC)
        if (!_STD is_constant_evaluated()) {
            return _udiv128(_High, _Low, _Div, &_Remainder);
        }
#endif // !defined(__clang__) && defined(_M_X64) && !defined(_M_ARM64EC)

        const auto __d = _STD countl_zero(static_cast<uint32_t>(_Div >> 32));
        if (__d >= 32) { // _Div < 2^32
            auto _Rem    = (_High << 32) | (_Low >> 32);
            auto _Result = _Rem / static_cast<uint32_t>(_Div);
            _Rem         = (_Rem % static_cast<uint32_t>(_Div) << 32) | static_cast<uint32_t>(_Low);
            _Result      = _Result << 32 | _Rem / static_cast<uint32_t>(_Div);
            _Remainder   = _Rem % static_cast<uint32_t>(_Div);
            return _Result;
        }

        uint32_t __u[5] = {
            static_cast<uint32_t>(_Low << __d),
            static_cast<uint32_t>(_Low >> (32 - __d)),
            static_cast<uint32_t>(_High << __d),
            static_cast<uint32_t>(_High >> (32 - __d)),
            0,
        };
        if (__d != 0) {
            __u[2] |= static_cast<uint32_t>(_Low >> (64 - __d));
            __u[4] = static_cast<uint32_t>(_High >> (64 - __d));
        }
        uint32_t __v[2] = {
            static_cast<uint32_t>(_Div << __d),
            static_cast<uint32_t>(_Div >> (32 - __d)),
        };
        uint32_t __q[3];

        _Knuth_4_3_1_D(__u, __v, __q);
        _STL_INTERNAL_CHECK(__u[4] == 0);
        _STL_INTERNAL_CHECK(__u[3] == 0);
        _STL_INTERNAL_CHECK(__u[2] == 0);
        _Remainder = (static_cast<uint64_t>(__u[1]) << (32 - __d)) | (__u[0] >> __d);

        _STL_INTERNAL_CHECK(__q[2] == 0);
        return static_cast<uint64_t>(__q[1]) << 32 | __q[0];
    }

    constexpr _Uint128() noexcept : _Word{} {}

    constexpr _Uint128(const integral auto _Val) noexcept : _Word{static_cast<uint64_t>(_Val)} {
        if constexpr (signed_integral<decltype(_Val)>) {
            if (_Val < 0) {
                _Word[1] = ~0ull;
            }
        }
    }

    explicit constexpr _Uint128(const uint64_t _Low, const uint64_t _High) noexcept : _Word{_Low, _High} {}

    template <integral _Ty>
    _NODISCARD explicit constexpr operator _Ty() const noexcept {
        return static_cast<_Ty>(_Word[0]);
    }

    _NODISCARD explicit constexpr operator bool() const noexcept {
        return (_Word[0] | _Word[1]) != 0;
    }

    _NODISCARD constexpr friend bool operator==(const _Uint128& _Left, const _Uint128& _Right) noexcept {
        return _Left._Word[0] == _Right._Word[0] && _Left._Word[1] == _Right._Word[1];
    }

    _NODISCARD constexpr friend strong_ordering operator<=>(const _Uint128& _Left, const _Uint128& _Right) noexcept {
        strong_ordering _Ord = _Left._Word[1] <=> _Right._Word[1];
        if (_Ord == strong_ordering::equal) {
            _Ord = _Left._Word[0] <=> _Right._Word[0];
        }
        return _Ord;
    }

    _NODISCARD friend constexpr _Uint128 operator<<(const _Uint128& _Left, const _Uint128& _Right) noexcept {
        auto _Tmp{_Left};
        _Tmp._Left_shift(static_cast<unsigned char>(_Right._Word[0]));
        return _Tmp;
    }

    constexpr _Uint128& operator<<=(const _Uint128& _Count) noexcept {
        _Left_shift(static_cast<unsigned char>(_Count._Word[0]));
        return *this;
    }
    template <integral _Ty>
    friend constexpr _Ty& operator<<=(_Ty& _Left, const _Uint128& _Right) noexcept {
        _Left = static_cast<_Ty>(_Uint128{_Left} << _Right);
        return _Left;
    }

    _NODISCARD friend constexpr _Uint128 operator>>(const _Uint128& _Left, const _Uint128& _Right) noexcept {
        auto _Tmp{_Left};
        _Tmp._Right_shift(static_cast<unsigned char>(_Right));
        return _Tmp;
    }

    constexpr _Uint128& operator>>=(const _Uint128& _Count) noexcept {
        _Right_shift(static_cast<unsigned char>(_Count._Word[0]));
        return *this;
    }
    template <integral _Ty>
    friend constexpr _Ty& operator>>=(_Ty& _Left, const _Uint128& _Right) noexcept {
        _Left = static_cast<_Ty>(_Uint128{_Left} >> _Right);
        return _Left;
    }

    constexpr _Uint128& operator++() noexcept {
        if (++_Word[0] == 0) {
            ++_Word[1];
        }
        return *this;
    }
    constexpr _Uint128 operator++(int) noexcept {
        auto _Tmp = *this;
        ++*this;
        return _Tmp;
    }

    constexpr _Uint128& operator--() noexcept {
        if (_Word[0]-- == 0) {
            --_Word[1];
        }
        return *this;
    }
    constexpr _Uint128 operator--(int) noexcept {
        auto _Tmp = *this;
        --*this;
        return _Tmp;
    }

    constexpr _Uint128 operator+() const noexcept {
        return *this;
    }

    constexpr _Uint128 operator-() const noexcept {
        return _Uint128{} - *this;
    }

    constexpr _Uint128 operator~() const noexcept {
        return _Uint128{~_Word[0], ~_Word[1]};
    }

    _NODISCARD friend constexpr _Uint128 operator+(const _Uint128& _Left, const _Uint128& _Right) noexcept {
        _Uint128 _Result;
        const auto _Carry = _AddCarry64(0, _Left._Word[0], _Right._Word[0], _Result._Word[0]);
        _AddCarry64(_Carry, _Left._Word[1], _Right._Word[1], _Result._Word[1]);
        return _Result;
    }

    constexpr _Uint128& operator+=(const _Uint128& _That) noexcept {
        const auto _Carry = _AddCarry64(0, _Word[0], _That._Word[0], _Word[0]);
        _AddCarry64(_Carry, _Word[1], _That._Word[1], _Word[1]);
        return *this;
    }
    template <integral _Ty>
    friend constexpr _Ty& operator+=(_Ty& _Left, const _Uint128& _Right) noexcept {
        _Left = static_cast<_Ty>(_Uint128{_Left} + _Right);
        return _Left;
    }

    _NODISCARD friend constexpr _Uint128 operator-(const _Uint128& _Left, const _Uint128& _Right) noexcept {
        _Uint128 _Result;
        const auto _Borrow = _SubBorrow64(0, _Left._Word[0], _Right._Word[0], _Result._Word[0]);
        _SubBorrow64(_Borrow, _Left._Word[1], _Right._Word[1], _Result._Word[1]);
        return _Result;
    }

    constexpr _Uint128& operator-=(const _Uint128& _That) noexcept {
        const auto _Borrow = _SubBorrow64(0, _Word[0], _That._Word[0], _Word[0]);
        _SubBorrow64(_Borrow, _Word[1], _That._Word[1], _Word[1]);
        return *this;
    }
    template <integral _Ty>
    friend constexpr _Ty& operator-=(_Ty& _Left, const _Uint128& _Right) noexcept {
        _Left = static_cast<_Ty>(_Uint128{_Left} - _Right);
        return _Left;
    }

    _NODISCARD friend constexpr _Uint128 operator*(const _Uint128& _Left, const _Uint128& _Right) noexcept {
        _Uint128 _Result;
        _Result._Word[0] = _UMul128(_Left._Word[0], _Right._Word[0], _Result._Word[1]);
        _Result._Word[1] += _Left._Word[0] * _Right._Word[1];
        _Result._Word[1] += _Left._Word[1] * _Right._Word[0];
        return _Result;
    }

    constexpr _Uint128& operator*=(const _Uint128& _That) noexcept {
        *this = *this * _That;
        return *this;
    }
    template <integral _Ty>
    friend constexpr _Ty& operator*=(_Ty& _Left, const _Uint128& _Right) noexcept {
        _Left = static_cast<_Ty>(_Uint128{_Left} * _Right);
        return _Left;
    }

    _NODISCARD friend constexpr _Uint128 operator/(const _Uint128& _Num, const uint32_t _Den) noexcept {
        _Uint128 _Result;
        _Result._Word[1] = _Num._Word[1] / _Den;
        uint64_t _Rem    = (_Num._Word[1] % _Den) << 32 | (_Num._Word[0] >> 32);
        _Result._Word[0] = (_Rem / _Den) << 32;
        _Rem             = (_Rem % _Den) << 32 | static_cast<uint32_t>(_Num._Word[0]);
        _Result._Word[0] |= static_cast<uint32_t>(_Rem / _Den);
        return _Result;
    }


    _NODISCARD friend constexpr _Uint128 operator/(const _Uint128& _Num, const uint64_t _Den) noexcept {
        _Uint128 _Result;
        _Result._Word[1] = _Num._Word[1] / _Den;
        uint64_t _Rem    = _Num._Word[1] % _Den;
        _Result._Word[0] = _UDiv128(_Rem, _Num._Word[0], _Den, _Rem);
        return _Result;
    }

    _NODISCARD friend constexpr _Uint128 operator/(_Uint128 _Num, _Uint128 _Den) noexcept {
        // establish _Den < _Num and _Num._Word[1] > 0
        if (_Den._Word[1] > _Num._Word[1]) {
            return _Uint128{};
        }
        if (_Den._Word[1] == _Num._Word[1]) {
            return _Uint128{_Num._Word[1] == 0 ? _Num._Word[0] / _Den._Word[0] : _Num._Word[0] >= _Den._Word[0]};
        }

        // establish _Den has more than 1 non-zero "digit"
        if (_Den._Word[1] == 0) {
            if (_Den._Word[0] < (1ull << 32)) {
                return operator/(_Num, static_cast<uint32_t>(_Den._Word[0]));
            } else {
                return operator/(_Num, _Den._Word[0]);
            }
        }

#if defined(_M_X64) && !defined(_M_ARM64EC)
        // Knuth 4.3.1D, 2-digit by 2-digit divide in base 2^64
        _STL_INTERNAL_CHECK(_Den._Word[1] != 0);
        _STL_INTERNAL_CHECK(_Num._Word[1] > _Den._Word[1]);
        // Normalize by shifting both left until _Den's high bit is set (So _Den's high digit is >= b / 2)
        const auto __d = _STD countl_zero(_Den._Word[1]);
        _Den <<= __d;
        auto _High_digit = __d == 0 ? 0 : _Num._Word[1] >> (64 - __d); // This creates a third digit for _Num
        _Num <<= __d;

        _Uint128 __qhat;
        __qhat._Word[1] = _High_digit >= _Den._Word[1];
        uint64_t __rhat;
        __qhat._Word[0] = _UDiv128(_High_digit >= _Den._Word[1] ? _High_digit - _Den._Word[1] : _High_digit,
            _Num._Word[1], _Den._Word[1], __rhat);

        for (;;) {
            if (__qhat._Word[1] > 0) {
                --__qhat;
            } else {
                _Uint128 _Prod;
                _Prod._Word[0] = _UMul128(__qhat._Word[0], _Den._Word[0], _Prod._Word[1]);
                if (_Prod <= _Uint128{_Num._Word[0], __rhat}) {
                    break;
                }
                --__qhat._Word[0];
            }

            const auto _Sum = __rhat + _Den._Word[1];
            if (__rhat > _Sum) {
                break;
            }
            __rhat = _Sum;
        }
        _STL_INTERNAL_CHECK(__qhat._Word[1] == 0);

        // [_High_digit | _Num] -= __qhat * _Den [Since __qhat < b, this is 3-digit - 1-digit * 2-digit]
        uint64_t _Prod0_hi;
        uint64_t _Prod_lo = _UMul128(__qhat._Word[0], _Den._Word[0], _Prod0_hi);
        auto _Borrow      = _SubBorrow64(0, _Num._Word[0], _Prod_lo, _Num._Word[0]);
        uint64_t _Prod1_hi;
        _Prod_lo = _UMul128(__qhat._Word[0], _Den._Word[1], _Prod1_hi);
        _Prod1_hi += _AddCarry64(0, _Prod_lo, _Prod0_hi, _Prod_lo);
        _Borrow = _SubBorrow64(_Borrow, _Num._Word[1], _Prod_lo, _Num._Word[1]);
        _Borrow = _SubBorrow64(_Borrow, _High_digit, _Prod1_hi, _High_digit);
        if (_Borrow) {
            --__qhat._Word[0];
        }
        return __qhat;
#else // ^^^ 128-bit intrinsics / no such intrinsics vvv
        const auto __d             = _STD countl_zero(_Den._Word[1]);
        const bool _Three_word_den = __d >= 32;
        __d &= 31;
        uint32_t __u[5]{
            static_cast<uint32_t>(_Num._Word[0] << __d),
            static_cast<uint32_t>(_Num._Word[0] >> (32 - __d)),
            static_cast<uint32_t>(_Num._Word[1] << __d),
            static_cast<uint32_t>(_Num._Word[1] >> (32 - __d)),
            0,
        };
        uint32_t __v[4] = {
            static_cast<uint32_t>(_Den._Word[0] << __d),
            static_cast<uint32_t>(_Den._Word[0] >> (32 - __d)),
            static_cast<uint32_t>(_Den._Word[1] << __d),
            static_cast<uint32_t>(_Den._Word[1] >> (32 - __d)),
        };
        if (__d != 0) {
            __u[2] |= _Num._Word[0] >> (64 - __d);
            __v[2] |= _Den._Word[0] >> (64 - __d);
        }

        uint32_t __q[2];
        if (_Three_word_den) {
            // 4-digit by 3-digit base 2^32 division
            _Knuth_4_3_1_D(__u, reinterpret_cast<uint(&)[3]>(__v), __q);
        } else {
            // 4-digit by 4-digit base 2^32 division
            _Knuth_4_3_1_D(__u, __v, reinterpret_cast<uint32_t(&)[1]>(__q));
            __q[1] = 0;
        }

        return _Uint128{static_cast<uint64_t>(__q[1]) << 32 | __q[0]};
#endif // defined(_M_X64) && !defined(_M_ARM64EC)
    }

    constexpr _Uint128& operator/=(const uint32_t _That) noexcept {
        *this = *this / _That;
        return *this;
    }
    constexpr _Uint128& operator/=(const uint64_t _That) noexcept {
        *this = *this / _That;
        return *this;
    }
    constexpr _Uint128& operator/=(const _Uint128& _That) noexcept {
        *this = *this / _That;
        return *this;
    }
    template <integral _Ty>
    friend constexpr _Ty& operator/=(_Ty& _Left, const _Uint128& _Right) noexcept {
        _Left = static_cast<_Ty>(_Uint128{_Left} / _Right);
        return _Left;
    }

    _NODISCARD friend constexpr _Uint128 operator%(const _Uint128& _Num, const uint32_t _Den) noexcept {
        uint64_t _Rem = _Num._Word[1];
        _Rem          = (_Rem % _Den) << 32 | (_Num._Word[0] >> 32);
        _Rem          = (_Rem % _Den) << 32 | static_cast<uint32_t>(_Num._Word[0]);
        return _Uint128{_Rem % _Den};
    }

    _NODISCARD friend constexpr _Uint128 operator%(const _Uint128& _Num, const uint64_t _Den) noexcept {
        uint64_t _Rem;
        (void) _UDiv128(_Num._Word[1] % _Den, _Num._Word[0], _Den, _Rem);
        return _Uint128{_Rem};
    }

    _NODISCARD friend constexpr _Uint128 operator%(_Uint128 _Num, _Uint128 _Den) noexcept {
        // establish _Den < _Num and _Num._Word[1] > 0
        if (_Den._Word[1] > _Num._Word[1]) {
            return _Num;
        }
        if (_Den._Word[1] == _Num._Word[1]) {
            if (_Den._Word[0] < _Num._Word[0]) {
                return _Uint128{_Num._Word[0] - _Den._Word[0]};
            }

            return _Num;
        }

        // establish _Den has more than 1 non-zero "digit"
        if (_Den._Word[1] == 0) {
            if ((_Den._Word[0] >> 32) == 0) {
                return operator%(_Num, static_cast<uint32_t>(_Den._Word[0]));
            } else {
                return operator%(_Num, _Den._Word[0]);
            }
        }

#if defined(_M_X64) && !defined(_M_ARM64EC)
        // Knuth 4.3.1D, 2-digit by 2-digit divide in base 2^64
        _STL_INTERNAL_CHECK(_Den._Word[1] != 0);
        _STL_INTERNAL_CHECK(_Num._Word[1] > _Den._Word[1]);
        // Normalize by shifting both left until _Den's high bit is set (So _Den's high digit is >= b / 2)
        const auto __d = _STD countl_zero(_Den._Word[1]);
        _Den <<= __d;
        auto _High_digit = __d == 0 ? 0 : _Num._Word[1] >> (64 - __d); // This creates a third digit for _Num
        _Num <<= __d;

        uint64_t __qhat_high = _High_digit >= _Den._Word[1];
        uint64_t __rhat;
        uint64_t __qhat = _UDiv128(_High_digit >= _Den._Word[1] ? _High_digit - _Den._Word[1] : _High_digit,
            _Num._Word[1], _Den._Word[1], __rhat);

        for (;;) {
            if (__qhat_high > 0) {
                if (__qhat-- == 0) {
                    --__qhat_high;
                }
            } else {
                _Uint128 _Prod;
                _Prod._Word[0] = _UMul128(__qhat, _Den._Word[0], _Prod._Word[1]);
                if (_Prod <= _Uint128{_Num._Word[0], __rhat}) {
                    break;
                }
                --__qhat;
            }

            const auto _Sum = __rhat + _Den._Word[1];
            if (__rhat > _Sum) {
                break;
            }
            __rhat = _Sum;
            // The addition didn't overflow, so `__rhat < b` holds
        }
        _STL_INTERNAL_CHECK(__qhat_high == 0);

        // [_High_digit | _Num] -= __qhat * _Den [3-digit - 1-digit * 2-digit]
        uint64_t _Prod0_hi;
        uint64_t _Prod_lo = _UMul128(__qhat, _Den._Word[0], _Prod0_hi);
        auto _Borrow      = _SubBorrow64(0, _Num._Word[0], _Prod_lo, _Num._Word[0]);
        uint64_t _Prod1_hi;
        _Prod_lo = _UMul128(__qhat, _Den._Word[1], _Prod1_hi);
        _Prod1_hi += _AddCarry64(0, _Prod_lo, _Prod0_hi, _Prod_lo);
        _Borrow = _SubBorrow64(_Borrow, _Num._Word[1], _Prod_lo, _Num._Word[1]);
        _Borrow = _SubBorrow64(_Borrow, _High_digit, _Prod1_hi, _High_digit);
        if (_Borrow) {
            auto _Carry = _AddCarry64(0, _Num._Word[0], _Den._Word[0], _Num._Word[0]);
            (void) _AddCarry64(_Carry, _Num._Word[1], _Den._Word[1], _Num._Word[1]);
        }
#else // ^^^ 128-bit intrinsics / no such intrinsics vvv
        const auto __d             = _STD countl_zero(_Den._Word[1]);
        const bool _Three_word_den = __d >= 32;
        __d &= 31;
        uint32_t __u[5]{
            static_cast<uint32_t>(_Num._Word[0] << __d),
            static_cast<uint32_t>(_Num._Word[0] >> (32 - __d)),
            static_cast<uint32_t>(_Num._Word[1] << __d),
            static_cast<uint32_t>(_Num._Word[1] >> (32 - __d)),
            0,
        };
        if (__d != 0) {
            __u[3] |= _Num._Word[0] >> (64 - __d);
        }

        uint32_t _Quotient[2];
        if (_Three_word_den) {
            // 4-digit by 3-digit base 2^32 division
            uint32_t __v[3] = {
                static_cast<uint32_t>(_Den._Word[0] << __d),
                static_cast<uint32_t>(_Den._Word[0] >> (32 - __d)),
                static_cast<uint32_t>(_Den._Word[1] << __d),
            };
            if (__d != 0) {
                __v[2] |= _Den._Word[0] >> (64 - __d);
            }

            _Knuth_4_3_1_D(__u, __v, _Quotient);
            _STL_INTERNAL_CHECK(__u[3] == 0);
        } else {
            // 4-digit by 4-digit base 2^32 division
            uint32_t __v[4] = {
                static_cast<uint32_t>(_Den._Word[0] << __d),
                static_cast<uint32_t>(_Den._Word[0] >> (32 - __d)),
                static_cast<uint32_t>(_Den._Word[1] << __d),
                static_cast<uint32_t>(_Den._Word[1] >> (32 - __d)),
            };
            if (__d != 0) {
                __v[2] |= _Den._Word[0] >> (64 - __d);
            }
            auto& __q = reinterpret_cast<uint32_t(&)[1]>(_Quotient);

            _Knuth_4_3_1_D(__u, __v, __q);
        }
        _STL_INTERNAL_CHECK(__u[4] == 0);

        _Num._Word[0] = static_cast<uint64_t>(__u[1]) << 32 | __u[0];
        _Num._Word[1] = static_cast<uint64_t>(__u[3]) << 32 | __u[2];
#endif // defined(_M_X64) && !defined(_M_ARM64EC)
        _Num >>= __d;
        return _Num;
    }

    constexpr _Uint128& operator%=(const uint32_t _Den) noexcept {
        *this = *this % _Den;
        return *this;
    }
    constexpr _Uint128& operator%=(const uint64_t _Den) noexcept {
        *this = *this % _Den;
        return *this;
    }
    constexpr _Uint128& operator%=(const _Uint128& _Den) noexcept {
        *this = *this % _Den;
        return *this;
    }
    template <integral _Ty>
    friend constexpr _Ty& operator%=(_Ty& _Left, const _Uint128& _Right) noexcept {
        _Left = static_cast<_Ty>(_Uint128{_Left} % _Right);
        return _Left;
    }

    _NODISCARD friend constexpr _Uint128 operator&(const _Uint128& _Left, const _Uint128& _Right) noexcept {
        return _Uint128{_Left._Word[0] & _Right._Word[0], _Left._Word[1] & _Right._Word[1]};
    }

    constexpr _Uint128& operator&=(const _Uint128& _That) noexcept {
        _Word[0] &= _That._Word[0];
        _Word[1] &= _That._Word[1];
        return *this;
    }
    template <integral _Ty>
    friend constexpr _Ty& operator&=(_Ty& _Left, const _Uint128& _Right) noexcept {
        _Left = static_cast<_Ty>(_Uint128{_Left} & _Right);
        return _Left;
    }

    _NODISCARD friend constexpr _Uint128 operator^(const _Uint128& _Left, const _Uint128& _Right) noexcept {
        return _Uint128{_Left._Word[0] ^ _Right._Word[0], _Left._Word[1] ^ _Right._Word[1]};
    }

    constexpr _Uint128& operator^=(const _Uint128& _That) noexcept {
        _Word[0] ^= _That._Word[0];
        _Word[1] ^= _That._Word[1];
        return *this;
    }
    template <integral _Ty>
    friend constexpr _Ty& operator^=(_Ty& _Left, const _Uint128& _Right) noexcept {
        _Left = static_cast<_Ty>(_Uint128{_Left} ^ _Right);
        return _Left;
    }

    _NODISCARD friend constexpr _Uint128 operator|(const _Uint128& _Left, const _Uint128& _Right) noexcept {
        return _Uint128{_Left._Word[0] | _Right._Word[0], _Left._Word[1] | _Right._Word[1]};
    }

    constexpr _Uint128& operator|=(const _Uint128& _That) noexcept {
        _Word[0] |= _That._Word[0];
        _Word[1] |= _That._Word[1];
        return *this;
    }
    template <integral _Ty>
    friend constexpr _Ty& operator|=(_Ty& _Left, const _Uint128& _Right) noexcept {
        _Left = static_cast<_Ty>(_Uint128{_Left} | _Right);
        return _Left;
    }
};

template <class>
class numeric_limits;
template <>
class numeric_limits<_Uint128> : public _Num_int_base {
public:
    _NODISCARD static constexpr _Uint128(min)() noexcept {
        return 0;
    }

    _NODISCARD static constexpr _Uint128(max)() noexcept {
        return _Uint128{~0ull, ~0ull};
    }

    _NODISCARD static constexpr _Uint128 lowest() noexcept {
        return (min) ();
    }

    _NODISCARD static constexpr _Uint128 epsilon() noexcept {
        return 0;
    }

    _NODISCARD static constexpr _Uint128 round_error() noexcept {
        return 0;
    }

    _NODISCARD static constexpr _Uint128 denorm_min() noexcept {
        return 0;
    }

    _NODISCARD static constexpr _Uint128 infinity() noexcept {
        return 0;
    }

    _NODISCARD static constexpr _Uint128 quiet_NaN() noexcept {
        return 0;
    }

    _NODISCARD static constexpr _Uint128 signaling_NaN() noexcept {
        return 0;
    }

    static constexpr bool is_modulo = true;
    static constexpr int digits     = 128;
    static constexpr int digits10   = 38;
};

template <class...>
struct common_type;
template <integral _Ty>
struct common_type<_Ty, _Uint128> {
    using type = _Uint128;
};
template <integral _Ty>
struct common_type<_Uint128, _Ty> {
    using type = _Uint128;
};

struct _Int128 : _Uint128 {
    using _Signed_type   = _Int128;
    using _Unsigned_type = _Uint128;

    constexpr void _Right_shift(const unsigned char _Count) noexcept {
        if (_Count >= 64) {
            _Word[0] = static_cast<uint64_t>(static_cast<int64_t>(_Word[1]) >> (_Count % 64));
            _Word[1] = (_Word[1] & (1ull << 63)) == 0 ? 0 : ~0ull;
            return;
        }

        if (_Count != 0) {
#if defined(_M_X64) && !defined(_M_ARM64EC)
            if (!_STD is_constant_evaluated()) {
                _Word[0] = __shiftright128(_Word[0], _Word[1], _Count);
            } else
#endif // defined(_M_X64) && !defined(_M_ARM64EC)
            {
                _Word[0] = (_Word[0] >> _Count) | (_Word[1] << (64 - _Count));
            }

            _Word[1] = static_cast<uint64_t>(static_cast<int64_t>(_Word[1]) >> _Count);
        }
    }

    using _Uint128::_Uint128;
    constexpr explicit _Int128(const _Uint128& _That) noexcept : _Uint128{_That} {}

    _NODISCARD constexpr friend strong_ordering operator<=>(const _Int128& _Left, const _Int128& _Right) noexcept {
        strong_ordering _Ord = static_cast<int64_t>(_Left._Word[1]) <=> static_cast<int64_t>(_Right._Word[1]);
        if (_Ord == strong_ordering::equal) {
            _Ord = _Left._Word[0] <=> _Right._Word[0];
        }
        return _Ord;
    }

    _NODISCARD friend constexpr _Int128 operator<<(const _Int128& _Left, const _Int128& _Right) noexcept {
        auto _Tmp{_Left};
        _Tmp._Left_shift(static_cast<unsigned char>(_Right._Word[0]));
        return _Tmp;
    }

    constexpr _Int128& operator<<=(const _Uint128& _Count) noexcept {
        _Left_shift(static_cast<unsigned char>(_Count._Word[0]));
        return *this;
    }
    template <integral _Ty>
    friend constexpr _Ty& operator<<=(_Ty& _Left, const _Int128& _Right) noexcept {
        _Left = static_cast<_Ty>(_Int128{_Left} << _Right);
        return _Left;
    }

    _NODISCARD friend constexpr _Int128 operator>>(const _Int128& _Left, const _Int128& _Right) noexcept {
        auto _Tmp{_Left};
        _Tmp._Right_shift(static_cast<unsigned char>(_Right._Word[0]));
        return _Tmp;
    }

    constexpr _Int128& operator>>=(const _Uint128& _Count) noexcept {
        _Right_shift(static_cast<unsigned char>(_Count._Word[0]));
        return *this;
    }
    template <integral _Ty>
    friend constexpr _Ty& operator>>=(_Ty& _Left, const _Int128& _Right) noexcept {
        _Left = static_cast<_Ty>(_Int128{_Left} >> _Right);
        return _Left;
    }

    constexpr _Int128& operator++() noexcept {
        _Uint128::operator++();
        return *this;
    }
    constexpr _Int128 operator++(int) noexcept {
        auto _Tmp = *this;
        ++*this;
        return _Tmp;
    }

    constexpr _Int128& operator--() noexcept {
        _Uint128::operator--();
        return *this;
    }
    constexpr _Int128 operator--(int) noexcept {
        auto _Tmp = *this;
        --*this;
        return _Tmp;
    }

    constexpr _Int128 operator+() const noexcept {
        return *this;
    }

    constexpr _Int128 operator-() const noexcept {
        return _Int128{_Uint128::operator-()};
    }

    constexpr _Int128 operator~() const noexcept {
        return _Int128{~_Word[0], ~_Word[1]};
    }

    _NODISCARD friend constexpr _Int128 operator+(const _Int128& _Left, const _Int128& _Right) noexcept {
        return _Int128{static_cast<const _Uint128&>(_Left) + static_cast<const _Uint128&>(_Right)};
    }
    constexpr _Int128& operator+=(const _Int128& _That) noexcept {
        _Uint128::operator+=(static_cast<const _Uint128&>(_That));
        return *this;
    }
    template <integral _Ty>
    friend constexpr _Ty& operator+=(_Ty& _Left, const _Int128& _Right) noexcept {
        _Left = static_cast<_Ty>(_Int128{_Left} + _Right);
        return _Left;
    }

    _NODISCARD friend constexpr _Int128 operator-(const _Int128& _Left, const _Int128& _Right) noexcept {
        return _Int128{static_cast<const _Uint128&>(_Left) - static_cast<const _Uint128&>(_Right)};
    }

    constexpr _Int128& operator-=(const _Int128& _That) noexcept {
        _Uint128::operator-=(static_cast<const _Uint128&>(_That));
        return *this;
    }
    template <integral _Ty>
    friend constexpr _Ty& operator-=(_Ty& _Left, const _Int128& _Right) noexcept {
        _Left = static_cast<_Ty>(_Int128{_Left} - _Right);
        return _Left;
    }

    _NODISCARD friend constexpr _Int128 operator*(_Int128 _Left, _Int128 _Right) noexcept {
        bool _Negative = false;
        if ((_Left._Word[1] & (1ull << 63)) != 0) {
            _Left     = -_Left;
            _Negative = true;
        }
        if ((_Right._Word[1] & (1ull << 63)) != 0) {
            _Right    = -_Right;
            _Negative = !_Negative;
        }

        _Uint128 _Result = static_cast<_Uint128&>(_Left) * static_cast<_Uint128&>(_Right);
        if (_Negative) {
            _Result = -_Result;
        }
        return _Int128{_Result};
    }

    constexpr _Int128& operator*=(const _Int128& _That) noexcept {
        *this = *this * _That;
        return *this;
    }
    template <integral _Ty>
    friend constexpr _Ty& operator*=(_Ty& _Left, const _Int128& _Right) noexcept {
        _Left = static_cast<_Ty>(_Int128{_Left} * _Right);
        return _Left;
    }

    _NODISCARD friend constexpr _Int128 operator/(_Int128 _Num, _Int128 _Den) noexcept {
        bool _Negative = false;
        if ((_Num._Word[1] & (1ull << 63)) != 0) {
            _Num      = -_Num;
            _Negative = true;
        }
        if ((_Den._Word[1] & (1ull << 63)) != 0) {
            _Den      = -_Den;
            _Negative = !_Negative;
        }

        _Uint128 _Result = static_cast<_Uint128&>(_Num) / static_cast<_Uint128&>(_Den);
        if (_Negative) {
            _Result = -_Result;
        }
        return _Int128{_Result};
    }
    _NODISCARD friend constexpr _Int128 operator/(_Int128 _Num, const uint64_t _Den) noexcept {
        bool _Negative = false;
        if ((_Num._Word[1] & (1ull << 63)) != 0) {
            _Num      = -_Num;
            _Negative = true;
        }

        _Uint128 _Result = static_cast<_Uint128&>(_Num) / _Den;
        if (_Negative) {
            _Result = -_Result;
        }
        return _Int128{_Result};
    }
    _NODISCARD friend constexpr _Int128 operator/(_Int128 _Num, const uint32_t _Den) noexcept {
        bool _Negative = false;
        if ((_Num._Word[1] & (1ull << 63)) != 0) {
            _Num      = -_Num;
            _Negative = true;
        }

        _Uint128 _Result = static_cast<_Uint128&>(_Num) / _Den;
        if (_Negative) {
            _Result = -_Result;
        }
        return _Int128{_Result};
    }

    constexpr _Int128& operator/=(const _Int128& _That) noexcept {
        *this = *this / _That;
        return *this;
    }
    template <integral _Ty>
    friend constexpr _Ty& operator/=(_Ty& _Left, const _Int128& _Right) noexcept {
        _Left = static_cast<_Ty>(_Int128{_Left} / _Right);
        return _Left;
    }

    _NODISCARD friend constexpr _Int128 operator%(_Int128 _Left, _Int128 _Right) noexcept {
        bool _Negative = false;
        if ((_Left._Word[1] & (1ull << 63)) != 0) {
            _Left     = -_Left;
            _Negative = true;
        }
        if ((_Right._Word[1] & (1ull << 63)) != 0) {
            _Right = -_Right;
        }

        _Uint128 _Result = static_cast<_Uint128&>(_Left) % static_cast<_Uint128&>(_Right);
        if (_Negative) {
            _Result = -_Result;
        }
        return _Int128{_Result};
    }

    constexpr _Int128& operator%=(const _Int128& _That) noexcept {
        *this = *this % _That;
        return *this;
    }
    template <integral _Ty>
    friend constexpr _Ty& operator%=(_Ty& _Left, const _Int128& _Right) noexcept {
        _Left = static_cast<_Ty>(_Int128{_Left} % _Right);
        return _Left;
    }

    _NODISCARD friend constexpr _Int128 operator&(const _Int128& _Left, const _Int128& _Right) noexcept {
        return _Int128{_Left._Word[0] & _Right._Word[0], _Left._Word[1] & _Right._Word[1]};
    }

    constexpr _Int128& operator&=(const _Int128& _That) noexcept {
        _Word[0] &= _That._Word[0];
        _Word[1] &= _That._Word[1];
        return *this;
    }
    template <integral _Ty>
    friend constexpr _Ty& operator&=(_Ty& _Left, const _Int128& _Right) noexcept {
        _Left = static_cast<_Ty>(_Int128{_Left} & _Right);
        return _Left;
    }

    _NODISCARD friend constexpr _Int128 operator^(const _Int128& _Left, const _Int128& _Right) noexcept {
        return _Int128{_Left._Word[0] ^ _Right._Word[0], _Left._Word[1] ^ _Right._Word[1]};
    }

    constexpr _Int128& operator^=(const _Int128& _That) noexcept {
        _Word[0] ^= _That._Word[0];
        _Word[1] ^= _That._Word[1];
        return *this;
    }
    template <integral _Ty>
    friend constexpr _Ty& operator^=(_Ty& _Left, const _Int128& _Right) noexcept {
        _Left = static_cast<_Ty>(_Int128{_Left} ^ _Right);
        return _Left;
    }

    _NODISCARD friend constexpr _Int128 operator|(const _Int128& _Left, const _Int128& _Right) noexcept {
        return _Int128{_Left._Word[0] | _Right._Word[0], _Left._Word[1] | _Right._Word[1]};
    }

    constexpr _Int128& operator|=(const _Int128& _That) noexcept {
        _Word[0] |= _That._Word[0];
        _Word[1] |= _That._Word[1];
        return *this;
    }
    template <integral _Ty>
    friend constexpr _Ty& operator|=(_Ty& _Left, const _Int128& _Right) noexcept {
        _Left = static_cast<_Ty>(_Int128{_Left} | _Right);
        return _Left;
    }
};

template <class>
class numeric_limits;
template <>
class numeric_limits<_Int128> : public _Num_int_base {
public:
    _NODISCARD static constexpr _Int128(min)() noexcept {
        return _Int128{0ull, 1ull << 63};
    }

    _NODISCARD static constexpr _Int128(max)() noexcept {
        return _Int128{~0ull, ~0ull >> 1};
    }

    _NODISCARD static constexpr _Int128 lowest() noexcept {
        return (min) ();
    }

    _NODISCARD static constexpr _Int128 epsilon() noexcept {
        return 0;
    }

    _NODISCARD static constexpr _Int128 round_error() noexcept {
        return 0;
    }

    _NODISCARD static constexpr _Int128 denorm_min() noexcept {
        return 0;
    }

    _NODISCARD static constexpr _Int128 infinity() noexcept {
        return 0;
    }

    _NODISCARD static constexpr _Int128 quiet_NaN() noexcept {
        return 0;
    }

    _NODISCARD static constexpr _Int128 signaling_NaN() noexcept {
        return 0;
    }

    static constexpr int digits   = 128;
    static constexpr int digits10 = 38;
};

template <integral _Ty>
struct common_type<_Ty, _Int128> {
    using type = _Int128;
};
template <integral _Ty>
struct common_type<_Int128, _Ty> {
    using type = _Int128;
};

_STD_END

#pragma pop_macro("new")
_STL_RESTORE_CLANG_WARNINGS
#pragma warning(pop)
#pragma pack(pop)
#endif // __cpp_lib_concepts
#endif // _STL_COMPILER_PREPROCESSOR
#endif // __MSVC_INT128_HPP
