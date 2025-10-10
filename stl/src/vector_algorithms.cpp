// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifdef _M_CEE_PURE
#error _M_CEE_PURE should not be defined when compiling vector_algorithms.cpp.
#endif

#if defined(_M_IX86) || defined(_M_X64) // NB: includes _M_ARM64EC
#include <__msvc_minmax.hpp>
#include <cstdint>
#include <cstring>
#include <cwchar>
#include <xtr1common>

#ifndef _M_ARM64EC
#include <intrin.h>
#include <isa_availability.h>

extern "C" long __isa_enabled;

#ifndef _DEBUG
#pragma optimize("t", on) // Override /Os with /Ot for this TU
#endif // !defined(_DEBUG)
#endif // ^^^ !defined(_M_ARM64EC) ^^^

namespace {
#ifndef _M_ARM64EC
    bool _Use_avx2() noexcept {
        return __isa_enabled & (1 << __ISA_AVAILABLE_AVX2);
    }

    bool _Use_sse42() noexcept {
        return __isa_enabled & (1 << __ISA_AVAILABLE_SSE42);
    }

    struct [[nodiscard]] _Zeroupper_on_exit { // TRANSITION, DevCom-10331414
        _Zeroupper_on_exit() = default;

        _Zeroupper_on_exit(const _Zeroupper_on_exit&)            = delete;
        _Zeroupper_on_exit& operator=(const _Zeroupper_on_exit&) = delete;

        ~_Zeroupper_on_exit() {
            _mm256_zeroupper();
        }
    };

    __m256i _Avx2_tail_mask_32(const size_t _Count_in_bytes) noexcept {
        // _Count_in_bytes must be within [0, 32].
        static constexpr unsigned int _Tail_masks[16] = {
            ~0u, ~0u, ~0u, ~0u, ~0u, ~0u, ~0u, ~0u, 0, 0, 0, 0, 0, 0, 0, 0};
        return _mm256_loadu_si256(reinterpret_cast<const __m256i*>(
            reinterpret_cast<const unsigned char*>(_Tail_masks) + (32 - _Count_in_bytes)));
    }
#endif // ^^^ !defined(_M_ARM64EC) ^^^

    size_t _Byte_length(const void* const _First, const void* const _Last) noexcept {
        return static_cast<const unsigned char*>(_Last) - static_cast<const unsigned char*>(_First);
    }

    void _Rewind_bytes(void*& _Target, const size_t _Offset) noexcept {
        _Target = static_cast<unsigned char*>(_Target) - _Offset;
    }

    void _Rewind_bytes(const void*& _Target, const size_t _Offset) noexcept {
        _Target = static_cast<const unsigned char*>(_Target) - _Offset;
    }

    template <class _Integral>
    void _Advance_bytes(void*& _Target, const _Integral _Offset) noexcept {
        _Target = static_cast<unsigned char*>(_Target) + _Offset;
    }

    template <class _Integral>
    void _Advance_bytes(const void*& _Target, const _Integral _Offset) noexcept {
        _Target = static_cast<const unsigned char*>(_Target) + _Offset;
    }
} // unnamed namespace

extern "C" {

__declspec(noalias) void __cdecl __std_swap_ranges_trivially_swappable_noalias(
    void* _First1, void* const _Last1, void* _First2) noexcept {
#ifndef _M_ARM64EC
    constexpr size_t _Mask_32 = ~((static_cast<size_t>(1) << 5) - 1);
    if (_Byte_length(_First1, _Last1) >= 32 && _Use_avx2()) {
        const void* _Stop_at = _First1;
        _Advance_bytes(_Stop_at, _Byte_length(_First1, _Last1) & _Mask_32);
        do {
            const __m256i _Left  = _mm256_loadu_si256(static_cast<__m256i*>(_First1));
            const __m256i _Right = _mm256_loadu_si256(static_cast<__m256i*>(_First2));
            _mm256_storeu_si256(static_cast<__m256i*>(_First1), _Right);
            _mm256_storeu_si256(static_cast<__m256i*>(_First2), _Left);
            _Advance_bytes(_First1, 32);
            _Advance_bytes(_First2, 32);
        } while (_First1 != _Stop_at);

        _mm256_zeroupper(); // TRANSITION, DevCom-10331414
    }

    constexpr size_t _Mask_16 = ~((static_cast<size_t>(1) << 4) - 1);
    if (_Byte_length(_First1, _Last1) >= 16 && _Use_sse42()) {
        const void* _Stop_at = _First1;
        _Advance_bytes(_Stop_at, _Byte_length(_First1, _Last1) & _Mask_16);
        do {
            const __m128i _Left  = _mm_loadu_si128(static_cast<__m128i*>(_First1));
            const __m128i _Right = _mm_loadu_si128(static_cast<__m128i*>(_First2));
            _mm_storeu_si128(static_cast<__m128i*>(_First1), _Right);
            _mm_storeu_si128(static_cast<__m128i*>(_First2), _Left);
            _Advance_bytes(_First1, 16);
            _Advance_bytes(_First2, 16);
        } while (_First1 != _Stop_at);
    }

#if defined(_M_X64)
    constexpr size_t _Mask_8 = ~((static_cast<size_t>(1) << 3) - 1);
    if (_Byte_length(_First1, _Last1) >= 8) {
        const void* _Stop_at = _First1;
        _Advance_bytes(_Stop_at, _Byte_length(_First1, _Last1) & _Mask_8);
        do {
            unsigned long long _Left;
            unsigned long long _Right;
            memcpy(&_Left, _First1, 8);
            memcpy(&_Right, _First2, 8);
            memcpy(_First1, &_Right, 8);
            memcpy(_First2, &_Left, 8);
            _Advance_bytes(_First1, 8);
            _Advance_bytes(_First2, 8);
        } while (_First1 != _Stop_at);
    }
#elif defined(_M_IX86)
    constexpr size_t _Mask_4 = ~((static_cast<size_t>(1) << 2) - 1);
    if (_Byte_length(_First1, _Last1) >= 4) {
        const void* _Stop_at = _First1;
        _Advance_bytes(_Stop_at, _Byte_length(_First1, _Last1) & _Mask_4);
        do {
            unsigned long _Left;
            unsigned long _Right;
            memcpy(&_Left, _First1, 4);
            memcpy(&_Right, _First2, 4);
            memcpy(_First1, &_Right, 4);
            memcpy(_First2, &_Left, 4);
            _Advance_bytes(_First1, 4);
            _Advance_bytes(_First2, 4);
        } while (_First1 != _Stop_at);
    }
#else
#error Unsupported architecture
#endif
#endif // ^^^ !defined(_M_ARM64EC) ^^^

    auto _First1c = static_cast<unsigned char*>(_First1);
    auto _First2c = static_cast<unsigned char*>(_First2);
    for (; _First1c != _Last1; ++_First1c, ++_First2c) {
        const unsigned char _Ch = *_First1c;
        *_First1c               = *_First2c;
        *_First2c               = _Ch;
    }
}

// TRANSITION, ABI: __std_swap_ranges_trivially_swappable() is preserved for binary compatibility
void* __cdecl __std_swap_ranges_trivially_swappable(
    void* const _First1, void* const _Last1, void* const _First2) noexcept {
    __std_swap_ranges_trivially_swappable_noalias(_First1, _Last1, _First2);
    return static_cast<char*>(_First2) + (static_cast<char*>(_Last1) - static_cast<char*>(_First1));
}

} // extern "C"

namespace {
    namespace _Rotating {
        void _Swap_3_ranges(void* _First1, void* const _Last1, void* _First2, void* _First3) noexcept {
#ifndef _M_ARM64EC
            constexpr size_t _Mask_32 = ~((static_cast<size_t>(1) << 5) - 1);
            if (_Byte_length(_First1, _Last1) >= 32 && _Use_avx2()) {
                const void* _Stop_at = _First1;
                _Advance_bytes(_Stop_at, _Byte_length(_First1, _Last1) & _Mask_32);
                do {
                    const __m256i _Val1 = _mm256_loadu_si256(static_cast<__m256i*>(_First1));
                    const __m256i _Val2 = _mm256_loadu_si256(static_cast<__m256i*>(_First2));
                    const __m256i _Val3 = _mm256_loadu_si256(static_cast<__m256i*>(_First3));
                    _mm256_storeu_si256(static_cast<__m256i*>(_First1), _Val2);
                    _mm256_storeu_si256(static_cast<__m256i*>(_First2), _Val3);
                    _mm256_storeu_si256(static_cast<__m256i*>(_First3), _Val1);
                    _Advance_bytes(_First1, 32);
                    _Advance_bytes(_First2, 32);
                    _Advance_bytes(_First3, 32);
                } while (_First1 != _Stop_at);

                _mm256_zeroupper(); // TRANSITION, DevCom-10331414
            }

            constexpr size_t _Mask_16 = ~((static_cast<size_t>(1) << 4) - 1);
            if (_Byte_length(_First1, _Last1) >= 16 && _Use_sse42()) {
                const void* _Stop_at = _First1;
                _Advance_bytes(_Stop_at, _Byte_length(_First1, _Last1) & _Mask_16);
                do {
                    const __m128i _Val1 = _mm_loadu_si128(static_cast<__m128i*>(_First1));
                    const __m128i _Val2 = _mm_loadu_si128(static_cast<__m128i*>(_First2));
                    const __m128i _Val3 = _mm_loadu_si128(static_cast<__m128i*>(_First3));
                    _mm_storeu_si128(static_cast<__m128i*>(_First1), _Val2);
                    _mm_storeu_si128(static_cast<__m128i*>(_First2), _Val3);
                    _mm_storeu_si128(static_cast<__m128i*>(_First3), _Val1);
                    _Advance_bytes(_First1, 16);
                    _Advance_bytes(_First2, 16);
                    _Advance_bytes(_First3, 16);
                } while (_First1 != _Stop_at);
            }

#if defined(_M_X64)
            constexpr size_t _Mask_8 = ~((static_cast<size_t>(1) << 3) - 1);
            if (_Byte_length(_First1, _Last1) >= 8) {
                const void* _Stop_at = _First1;
                _Advance_bytes(_Stop_at, _Byte_length(_First1, _Last1) & _Mask_8);
                do {
                    unsigned long long _Val1;
                    unsigned long long _Val2;
                    unsigned long long _Val3;
                    memcpy(&_Val1, _First1, 8);
                    memcpy(&_Val2, _First2, 8);
                    memcpy(&_Val3, _First3, 8);
                    memcpy(_First1, &_Val2, 8);
                    memcpy(_First2, &_Val3, 8);
                    memcpy(_First3, &_Val1, 8);
                    _Advance_bytes(_First1, 8);
                    _Advance_bytes(_First2, 8);
                    _Advance_bytes(_First3, 8);
                } while (_First1 != _Stop_at);
            }
#elif defined(_M_IX86)
            constexpr size_t _Mask_4 = ~((static_cast<size_t>(1) << 2) - 1);
            if (_Byte_length(_First1, _Last1) >= 4) {
                const void* _Stop_at = _First1;
                _Advance_bytes(_Stop_at, _Byte_length(_First1, _Last1) & _Mask_4);
                do {
                    unsigned long _Val1;
                    unsigned long _Val2;
                    unsigned long _Val3;
                    memcpy(&_Val1, _First1, 4);
                    memcpy(&_Val2, _First2, 4);
                    memcpy(&_Val3, _First3, 4);
                    memcpy(_First1, &_Val2, 4);
                    memcpy(_First2, &_Val3, 4);
                    memcpy(_First3, &_Val1, 4);
                    _Advance_bytes(_First1, 4);
                    _Advance_bytes(_First2, 4);
                    _Advance_bytes(_First3, 4);
                } while (_First1 != _Stop_at);
            }
#else
#error Unsupported architecture
#endif
#endif // ^^^ !defined(_M_ARM64EC) ^^^

            auto _First1c = static_cast<unsigned char*>(_First1);
            auto _First2c = static_cast<unsigned char*>(_First2);
            auto _First3c = static_cast<unsigned char*>(_First3);
            for (; _First1c != _Last1; ++_First1c, ++_First2c, ++_First3c) {
                const unsigned char _Ch = *_First1c;
                *_First1c               = *_First2c;
                *_First2c               = *_First3c;
                *_First3c               = _Ch;
            }
        }

        constexpr size_t _Buf_size = 512;

        bool _Use_buffer(const size_t _Smaller, const size_t _Larger) noexcept {
            return _Smaller <= _Buf_size && (_Smaller <= 128 || _Larger >= _Smaller * 2);
        }
    } // namespace _Rotating
} // unnamed namespace

extern "C" {

__declspec(noalias) void __stdcall __std_rotate(void* _First, void* const _Mid, void* _Last) noexcept {
    unsigned char _Buf[_Rotating::_Buf_size];

    for (;;) {
        const size_t _Left  = _Byte_length(_First, _Mid);
        const size_t _Right = _Byte_length(_Mid, _Last);

        if (_Left <= _Right) {
            if (_Left == 0) {
                break;
            }

            if (_Rotating::_Use_buffer(_Left, _Right)) {
                memcpy(_Buf, _First, _Left);
                memmove(_First, _Mid, _Right);
                _Advance_bytes(_First, _Right);
                memcpy(_First, _Buf, _Left);
                break;
            }

            void* _Mid2 = _Last;
            _Rewind_bytes(_Mid2, _Left);
            if (_Left * 2 > _Right) {
                __std_swap_ranges_trivially_swappable_noalias(_Mid2, _Last, _First);
                _Last = _Mid2;
            } else {
                void* _Mid3 = _Mid2;
                _Rewind_bytes(_Mid3, _Left);
                _Rotating::_Swap_3_ranges(_Mid2, _Last, _First, _Mid3);
                _Last = _Mid3;
            }
        } else {
            if (_Right == 0) {
                break;
            }

            if (_Rotating::_Use_buffer(_Right, _Left)) {
                _Rewind_bytes(_Last, _Right);
                memcpy(_Buf, _Last, _Right);
                void* _Mid2 = _First;
                _Advance_bytes(_Mid2, _Right);
                memmove(_Mid2, _First, _Left);
                memcpy(_First, _Buf, _Right);
                break;
            }

            if (_Right * 2 > _Left) {
                __std_swap_ranges_trivially_swappable_noalias(_Mid, _Last, _First);
                _Advance_bytes(_First, _Right);
            } else {
                void* _Mid2 = _First;
                _Advance_bytes(_Mid2, _Right);
                _Rotating::_Swap_3_ranges(_Mid, _Last, _Mid2, _First);
                _Advance_bytes(_First, _Right * 2);
            }
        }
    }
}

} // extern "C"

namespace {
    namespace _Reversing {
#ifdef _M_ARM64EC
        using _Traits_1 = void;
        using _Traits_2 = void;
        using _Traits_4 = void;
        using _Traits_8 = void;
#else // ^^^ defined(_M_ARM64EC) / !defined(_M_ARM64EC) vvv
        struct _Traits_1 {
            static __m256i _Rev_avx(const __m256i _Val) noexcept {
                const __m256i _Reverse_char_lanes_avx = _mm256_set_epi8( //
                    0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, //
                    0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15);

                const __m256i _Perm = _mm256_permute4x64_epi64(_Val, _MM_SHUFFLE(1, 0, 3, 2));
                return _mm256_shuffle_epi8(_Perm, _Reverse_char_lanes_avx);
            }

            static __m128i _Rev_sse(const __m128i _Val) noexcept {
                const __m128i _Reverse_char_sse = _mm_set_epi8(0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15);
                return _mm_shuffle_epi8(_Val, _Reverse_char_sse);
            }
        };

        struct _Traits_2 {
            static __m256i _Rev_avx(const __m256i _Val) noexcept {
                const __m256i _Reverse_short_lanes_avx = _mm256_set_epi8( //
                    1, 0, 3, 2, 5, 4, 7, 6, 9, 8, 11, 10, 13, 12, 15, 14, //
                    1, 0, 3, 2, 5, 4, 7, 6, 9, 8, 11, 10, 13, 12, 15, 14);

                const __m256i _Perm = _mm256_permute4x64_epi64(_Val, _MM_SHUFFLE(1, 0, 3, 2));
                return _mm256_shuffle_epi8(_Perm, _Reverse_short_lanes_avx);
            }

            static __m128i _Rev_sse(const __m128i _Val) noexcept {
                const __m128i _Reverse_short_sse = _mm_set_epi8(1, 0, 3, 2, 5, 4, 7, 6, 9, 8, 11, 10, 13, 12, 15, 14);
                return _mm_shuffle_epi8(_Val, _Reverse_short_sse);
            }
        };

        struct _Traits_4 {
            static __m256i _Rev_avx(const __m256i _Val) noexcept {
                const __m256i _Shuf = _mm256_set_epi32(0, 1, 2, 3, 4, 5, 6, 7);
                return _mm256_permutevar8x32_epi32(_Val, _Shuf);
            }

            static __m128i _Rev_sse(const __m128i _Val) noexcept {
                return _mm_shuffle_epi32(_Val, _MM_SHUFFLE(0, 1, 2, 3));
            }
        };

        struct _Traits_8 {
            static __m256i _Rev_avx(const __m256i _Val) noexcept {
                return _mm256_permute4x64_epi64(_Val, _MM_SHUFFLE(0, 1, 2, 3));
            }

            static __m128i _Rev_sse(const __m128i _Val) noexcept {
                return _mm_shuffle_epi32(_Val, _MM_SHUFFLE(1, 0, 3, 2));
            }
        };
#endif // ^^^ !defined(_M_ARM64EC) ^^^

        template <class _BidIt>
        void _Reverse_tail(_BidIt _First, _BidIt _Last) noexcept {
            for (; _First != _Last && _First != --_Last; ++_First) {
                const auto _Temp = *_First;
                *_First          = *_Last;
                *_Last           = _Temp;
            }
        }

        template <class _BidIt, class _OutIt>
        void _Reverse_copy_tail(const _BidIt _First, _BidIt _Last, _OutIt _Dest) noexcept {
            while (_First != _Last) {
                *_Dest++ = *--_Last;
            }
        }

#ifndef _M_ARM64EC
        __m256i _Avx2_rev_tail_mask_32(const size_t _Count_in_bytes) noexcept {
            // _Count_in_bytes must be within [0, 32].
            static constexpr unsigned int _Tail_masks[16] = {
                0, 0, 0, 0, 0, 0, 0, 0, ~0u, ~0u, ~0u, ~0u, ~0u, ~0u, ~0u, ~0u};
            return _mm256_loadu_si256(reinterpret_cast<const __m256i*>(
                reinterpret_cast<const unsigned char*>(_Tail_masks) + _Count_in_bytes));
        }
#endif // ^^^ !defined(_M_ARM64EC) ^^^

        template <class _Traits, class _Ty>
        __declspec(noalias) void __cdecl _Reverse_impl(void* _First, void* _Last) noexcept {
#ifndef _M_ARM64EC
            if (const size_t _Length = _Byte_length(_First, _Last); _Length >= 64 && _Use_avx2()) {
                const void* _Stop_at = _First;
                _Advance_bytes(_Stop_at, (_Length >> 1) & ~size_t{0x1F});
                do {
                    _Advance_bytes(_Last, -32);
                    const __m256i _Left           = _mm256_loadu_si256(static_cast<__m256i*>(_First));
                    const __m256i _Right          = _mm256_loadu_si256(static_cast<__m256i*>(_Last));
                    const __m256i _Left_reversed  = _Traits::_Rev_avx(_Left);
                    const __m256i _Right_reversed = _Traits::_Rev_avx(_Right);
                    _mm256_storeu_si256(static_cast<__m256i*>(_First), _Right_reversed);
                    _mm256_storeu_si256(static_cast<__m256i*>(_Last), _Left_reversed);
                    _Advance_bytes(_First, 32);
                } while (_First != _Stop_at);

                _mm256_zeroupper(); // TRANSITION, DevCom-10331414
            }

            if (const size_t _Length = _Byte_length(_First, _Last); _Length >= 32 && _Use_sse42()) {
                const void* _Stop_at = _First;
                _Advance_bytes(_Stop_at, (_Length >> 1) & ~size_t{0xF});
                do {
                    _Advance_bytes(_Last, -16);
                    const __m128i _Left           = _mm_loadu_si128(static_cast<__m128i*>(_First));
                    const __m128i _Right          = _mm_loadu_si128(static_cast<__m128i*>(_Last));
                    const __m128i _Left_reversed  = _Traits::_Rev_sse(_Left);
                    const __m128i _Right_reversed = _Traits::_Rev_sse(_Right);
                    _mm_storeu_si128(static_cast<__m128i*>(_First), _Right_reversed);
                    _mm_storeu_si128(static_cast<__m128i*>(_Last), _Left_reversed);
                    _Advance_bytes(_First, 16);
                } while (_First != _Stop_at);
            }
#endif // ^^^ !defined(_M_ARM64EC) ^^^

            _Reverse_tail(static_cast<_Ty*>(_First), static_cast<_Ty*>(_Last));
        }

        template <class _Traits, class _Ty>
        __declspec(noalias) void __cdecl _Reverse_copy_impl(
            const void* _First, const void* _Last, void* _Dest) noexcept {
#ifndef _M_ARM64EC
            if (const size_t _Length = _Byte_length(_First, _Last); _Length >= 32 && _Use_avx2()) {
                const void* _Stop_at = _Dest;
                _Advance_bytes(_Stop_at, _Length & ~size_t{0x1F});
                do {
                    _Advance_bytes(_Last, -32);
                    const __m256i _Block          = _mm256_loadu_si256(static_cast<const __m256i*>(_Last));
                    const __m256i _Block_reversed = _Traits::_Rev_avx(_Block);
                    _mm256_storeu_si256(static_cast<__m256i*>(_Dest), _Block_reversed);
                    _Advance_bytes(_Dest, 32);
                } while (_Dest != _Stop_at);

                if (const size_t _Avx_tail = _Length & 0x1C; _Avx_tail != 0) {
                    _Advance_bytes(_Last, -32);
                    const __m256i _Mask           = _Avx2_tail_mask_32(_Avx_tail);
                    const __m256i _Rev_mask       = _Avx2_rev_tail_mask_32(_Avx_tail);
                    const __m256i _Block          = _mm256_maskload_epi32(static_cast<const int*>(_Last), _Rev_mask);
                    const __m256i _Block_reversed = _Traits::_Rev_avx(_Block);
                    _mm256_maskstore_epi32(static_cast<int*>(_Dest), _Mask, _Block_reversed);
                    if constexpr (sizeof(_Ty) < 4) {
                        _Advance_bytes(_Dest, _Avx_tail);
                        _Advance_bytes(_Last, 32 - _Avx_tail);
                    }
                }

                _mm256_zeroupper(); // TRANSITION, DevCom-10331414

                if constexpr (sizeof(_Ty) >= 4) {
                    return;
                }
            } else if (_Length >= 16 && _Use_sse42()) {
                const void* _Stop_at = _Dest;
                _Advance_bytes(_Stop_at, _Length & ~size_t{0xF});
                do {
                    _Advance_bytes(_Last, -16);
                    const __m128i _Block          = _mm_loadu_si128(static_cast<const __m128i*>(_Last));
                    const __m128i _Block_reversed = _Traits::_Rev_sse(_Block);
                    _mm_storeu_si128(static_cast<__m128i*>(_Dest), _Block_reversed);
                    _Advance_bytes(_Dest, 16);
                } while (_Dest != _Stop_at);
            }
#endif // ^^^ !defined(_M_ARM64EC) ^^^

            _Reverse_copy_tail(
                static_cast<const _Ty*>(_First), static_cast<const _Ty*>(_Last), static_cast<_Ty*>(_Dest));
        }
    } // namespace _Reversing
} // unnamed namespace

extern "C" {

__declspec(noalias) void __cdecl __std_reverse_trivially_swappable_1(void* _First, void* _Last) noexcept {
    _Reversing::_Reverse_impl<_Reversing::_Traits_1, uint8_t>(_First, _Last);
}

__declspec(noalias) void __cdecl __std_reverse_trivially_swappable_2(void* _First, void* _Last) noexcept {
    _Reversing::_Reverse_impl<_Reversing::_Traits_2, uint16_t>(_First, _Last);
}

__declspec(noalias) void __cdecl __std_reverse_trivially_swappable_4(void* _First, void* _Last) noexcept {
    _Reversing::_Reverse_impl<_Reversing::_Traits_4, uint32_t>(_First, _Last);
}

__declspec(noalias) void __cdecl __std_reverse_trivially_swappable_8(void* _First, void* _Last) noexcept {
    _Reversing::_Reverse_impl<_Reversing::_Traits_8, uint64_t>(_First, _Last);
}

__declspec(noalias) void __cdecl __std_reverse_copy_trivially_copyable_1(
    const void* _First, const void* _Last, void* _Dest) noexcept {
    _Reversing::_Reverse_copy_impl<_Reversing::_Traits_1, uint8_t>(_First, _Last, _Dest);
}

__declspec(noalias) void __cdecl __std_reverse_copy_trivially_copyable_2(
    const void* _First, const void* _Last, void* _Dest) noexcept {
    _Reversing::_Reverse_copy_impl<_Reversing::_Traits_2, uint16_t>(_First, _Last, _Dest);
}

__declspec(noalias) void __cdecl __std_reverse_copy_trivially_copyable_4(
    const void* _First, const void* _Last, void* _Dest) noexcept {
    _Reversing::_Reverse_copy_impl<_Reversing::_Traits_4, uint32_t>(_First, _Last, _Dest);
}

__declspec(noalias) void __cdecl __std_reverse_copy_trivially_copyable_8(
    const void* _First, const void* _Last, void* _Dest) noexcept {
    _Reversing::_Reverse_copy_impl<_Reversing::_Traits_8, uint64_t>(_First, _Last, _Dest);
}

} // extern "C"

namespace {
    namespace _Sorting {
        enum _Min_max_mode {
            _Mode_min  = 1 << 0,
            _Mode_max  = 1 << 1,
            _Mode_both = _Mode_min | _Mode_max,
        };

        template <class _Base>
        struct _Traits_scalar : _Base {
            static constexpr bool _Vectorized  = false;
            static constexpr size_t _Tail_mask = 0;
        };

#ifndef _M_ARM64EC
        struct _Traits_sse_base {
            using _Guard                       = char;
            static constexpr bool _Vectorized  = true;
            static constexpr size_t _Vec_size  = 16;
            static constexpr size_t _Vec_mask  = 0xF;
            static constexpr size_t _Tail_mask = 0;

            static __m128i _Zero() noexcept {
                return _mm_setzero_si128();
            }

            static __m128i _All_ones() noexcept {
                return _mm_set1_epi8(static_cast<char>(0xFF));
            }

            static __m128i _Blend(const __m128i _Px1, const __m128i _Px2, const __m128i _Msk) noexcept {
                return _mm_blendv_epi8(_Px1, _Px2, _Msk);
            }

            static unsigned long _Mask(const __m128i _Val) noexcept {
                return _mm_movemask_epi8(_Val);
            }

            static void _Exit_vectorized() noexcept {}
        };

        struct _Traits_avx_base {
            using _Guard                      = _Zeroupper_on_exit;
            static constexpr bool _Vectorized = true;
            static constexpr size_t _Vec_size = 32;
            static constexpr size_t _Vec_mask = 0x1F;

            static __m256i _Zero() noexcept {
                return _mm256_setzero_si256();
            }

            static __m256i _All_ones() noexcept {
                return _mm256_set1_epi8(static_cast<char>(0xFF));
            }

            static __m256i _Blend(const __m256i _Px1, const __m256i _Px2, const __m256i _Msk) noexcept {
                return _mm256_blendv_epi8(_Px1, _Px2, _Msk);
            }

            static unsigned long _Mask(const __m256i _Val) noexcept {
                return _mm256_movemask_epi8(_Val);
            }

            static void _Exit_vectorized() noexcept {
                _mm256_zeroupper();
            }
        };

        struct _Traits_avx_i_base : _Traits_avx_base {
            static constexpr size_t _Tail_mask = 0x1C;

            static __m256i _Blendval(const __m256i _Px1, const __m256i _Px2, const __m256i _Msk) noexcept {
                return _mm256_blendv_epi8(_Px1, _Px2, _Msk);
            }

            static __m256i _Load_mask(const void* const _Src, const __m256i _Mask) noexcept {
                return _mm256_maskload_epi32(reinterpret_cast<const int*>(_Src), _Mask);
            }
        };
#endif // ^^^ !defined(_M_ARM64EC) ^^^

        struct _Traits_1_base {
            static constexpr bool _Is_floating = false;

            using _Signed_t   = int8_t;
            using _Unsigned_t = uint8_t;

            static constexpr _Signed_t _Init_min_val = static_cast<_Signed_t>(0x7F);
            static constexpr _Signed_t _Init_max_val = static_cast<_Signed_t>(0x80);

            using _Minmax_i_t = _Min_max_1i;
            using _Minmax_u_t = _Min_max_1u;

#ifndef _M_ARM64EC
            static constexpr bool _Has_portion_max = true;
            static constexpr size_t _Portion_max   = 256;
#endif // ^^^ !defined(_M_ARM64EC) ^^^
        };

#ifndef _M_ARM64EC
        struct _Traits_1_sse : _Traits_1_base, _Traits_sse_base {
            static __m128i _Load(const void* const _Src) noexcept {
                return _mm_loadu_si128(reinterpret_cast<const __m128i*>(_Src));
            }

            static __m128i _Sign_correction(const __m128i _Val, const bool _Sign) noexcept {
                alignas(16) static constexpr _Unsigned_t _Sign_corrections[2][16] = {
                    {0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
                    {}};
                return _mm_sub_epi8(_Val, _mm_load_si128(reinterpret_cast<const __m128i*>(_Sign_corrections[_Sign])));
            }

            static __m128i _Inc(const __m128i _Idx) noexcept {
                return _mm_add_epi8(_Idx, _mm_set1_epi8(1));
            }

            template <class _Fn>
            static __m128i _H_func(const __m128i _Cur, const _Fn _Funct) noexcept {
                const __m128i _Shuf_b = _mm_set_epi8(14, 15, 12, 13, 10, 11, 8, 9, 6, 7, 4, 5, 2, 3, 0, 1);
                const __m128i _Shuf_w = _mm_set_epi8(13, 12, 15, 14, 9, 8, 11, 10, 5, 4, 7, 6, 1, 0, 3, 2);

                __m128i _H_min_val = _Cur;
                _H_min_val         = _Funct(_H_min_val, _mm_shuffle_epi32(_H_min_val, _MM_SHUFFLE(1, 0, 3, 2)));
                _H_min_val         = _Funct(_H_min_val, _mm_shuffle_epi32(_H_min_val, _MM_SHUFFLE(2, 3, 0, 1)));
                _H_min_val         = _Funct(_H_min_val, _mm_shuffle_epi8(_H_min_val, _Shuf_w));
                _H_min_val         = _Funct(_H_min_val, _mm_shuffle_epi8(_H_min_val, _Shuf_b));
                return _H_min_val;
            }

            static __m128i _H_min(const __m128i _Cur) noexcept {
                return _H_func(
                    _Cur, [](const __m128i _Val1, const __m128i _Val2) noexcept { return _mm_min_epi8(_Val1, _Val2); });
            }

            static __m128i _H_max(const __m128i _Cur) noexcept {
                return _H_func(
                    _Cur, [](const __m128i _Val1, const __m128i _Val2) noexcept { return _mm_max_epi8(_Val1, _Val2); });
            }

            static __m128i _H_min_u(const __m128i _Cur) noexcept {
                return _H_func(
                    _Cur, [](const __m128i _Val1, const __m128i _Val2) noexcept { return _mm_min_epu8(_Val1, _Val2); });
            }

            static __m128i _H_max_u(const __m128i _Cur) noexcept {
                return _H_func(
                    _Cur, [](const __m128i _Val1, const __m128i _Val2) noexcept { return _mm_max_epu8(_Val1, _Val2); });
            }

            static _Signed_t _Get_any(const __m128i _Cur) noexcept {
                return static_cast<_Signed_t>(_mm_cvtsi128_si32(_Cur));
            }

            static _Unsigned_t _Get_v_pos(const __m128i _Idx, const unsigned long _H_pos) noexcept {
                return static_cast<_Unsigned_t>(_mm_cvtsi128_si32(_mm_shuffle_epi8(_Idx, _mm_cvtsi32_si128(_H_pos))));
            }

            static __m128i _Cmp_eq(const __m128i _First, const __m128i _Second) noexcept {
                return _mm_cmpeq_epi8(_First, _Second);
            }

            static __m128i _Cmp_gt(const __m128i _First, const __m128i _Second) noexcept {
                return _mm_cmpgt_epi8(_First, _Second);
            }

            static __m128i _Cmp_eq_idx(const __m128i _First, const __m128i _Second) noexcept {
                return _mm_cmpeq_epi8(_First, _Second);
            }

            static __m128i _Min(const __m128i _First, const __m128i _Second, __m128i = _mm_undefined_si128()) noexcept {
                return _mm_min_epi8(_First, _Second);
            }

            static __m128i _Max(const __m128i _First, const __m128i _Second, __m128i = _mm_undefined_si128()) noexcept {
                return _mm_max_epi8(_First, _Second);
            }

            static __m128i _Min_u(const __m128i _First, const __m128i _Second) noexcept {
                return _mm_min_epu8(_First, _Second);
            }

            static __m128i _Max_u(const __m128i _First, const __m128i _Second) noexcept {
                return _mm_max_epu8(_First, _Second);
            }

            static __m128i _Mask_cast(const __m128i _Mask) noexcept {
                return _Mask;
            }
        };

        struct _Traits_1_avx : _Traits_1_base, _Traits_avx_i_base {
            static __m256i _Load(const void* const _Src) noexcept {
                return _mm256_loadu_si256(reinterpret_cast<const __m256i*>(_Src));
            }

            static __m256i _Sign_correction(const __m256i _Val, const bool _Sign) noexcept {
                alignas(32) static constexpr _Unsigned_t _Sign_corrections[2][32] = {
                    {0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
                        0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
                    {}};
                return _mm256_sub_epi8(
                    _Val, _mm256_load_si256(reinterpret_cast<const __m256i*>(_Sign_corrections[_Sign])));
            }

            static __m256i _Inc(const __m256i _Idx) noexcept {
                return _mm256_add_epi8(_Idx, _mm256_set1_epi8(1));
            }

            template <class _Fn>
            static __m256i _H_func(const __m256i _Cur, const _Fn _Funct) noexcept {
                const __m128i _Shuf_b = _mm_set_epi8(14, 15, 12, 13, 10, 11, 8, 9, 6, 7, 4, 5, 2, 3, 0, 1);
                const __m128i _Shuf_w = _mm_set_epi8(13, 12, 15, 14, 9, 8, 11, 10, 5, 4, 7, 6, 1, 0, 3, 2);

                __m256i _H_min_val = _Cur;
                _H_min_val         = _Funct(_H_min_val, _mm256_permute4x64_epi64(_H_min_val, _MM_SHUFFLE(1, 0, 3, 2)));
                _H_min_val         = _Funct(_H_min_val, _mm256_shuffle_epi32(_H_min_val, _MM_SHUFFLE(1, 0, 3, 2)));
                _H_min_val         = _Funct(_H_min_val, _mm256_shuffle_epi32(_H_min_val, _MM_SHUFFLE(2, 3, 0, 1)));
                _H_min_val = _Funct(_H_min_val, _mm256_shuffle_epi8(_H_min_val, _mm256_broadcastsi128_si256(_Shuf_w)));
                _H_min_val = _Funct(_H_min_val, _mm256_shuffle_epi8(_H_min_val, _mm256_broadcastsi128_si256(_Shuf_b)));
                return _H_min_val;
            }

            static __m256i _H_min(const __m256i _Cur) noexcept {
                return _H_func(_Cur,
                    [](const __m256i _Val1, const __m256i _Val2) noexcept { return _mm256_min_epi8(_Val1, _Val2); });
            }

            static __m256i _H_max(const __m256i _Cur) noexcept {
                return _H_func(_Cur,
                    [](const __m256i _Val1, const __m256i _Val2) noexcept { return _mm256_max_epi8(_Val1, _Val2); });
            }

            static __m256i _H_min_u(const __m256i _Cur) noexcept {
                return _H_func(_Cur,
                    [](const __m256i _Val1, const __m256i _Val2) noexcept { return _mm256_min_epu8(_Val1, _Val2); });
            }

            static __m256i _H_max_u(const __m256i _Cur) noexcept {
                return _H_func(_Cur,
                    [](const __m256i _Val1, const __m256i _Val2) noexcept { return _mm256_max_epu8(_Val1, _Val2); });
            }

            static _Signed_t _Get_any(const __m256i _Cur) noexcept {
                return static_cast<_Signed_t>(_mm256_cvtsi256_si32(_Cur));
            }

            static _Unsigned_t _Get_v_pos(const __m256i _Idx, const unsigned long _H_pos) noexcept {
                const uint32_t _Part = _mm256_cvtsi256_si32(
                    _mm256_permutevar8x32_epi32(_Idx, _mm256_castsi128_si256(_mm_cvtsi32_si128(_H_pos >> 2))));
                return static_cast<_Unsigned_t>(_Part >> ((_H_pos & 0x3) << 3));
            }

            static __m256i _Cmp_eq(const __m256i _First, const __m256i _Second) noexcept {
                return _mm256_cmpeq_epi8(_First, _Second);
            }

            static __m256i _Cmp_gt(const __m256i _First, const __m256i _Second) noexcept {
                return _mm256_cmpgt_epi8(_First, _Second);
            }

            static __m256i _Cmp_eq_idx(const __m256i _First, const __m256i _Second) noexcept {
                return _mm256_cmpeq_epi8(_First, _Second);
            }

            static __m256i _Min(
                const __m256i _First, const __m256i _Second, __m256i = _mm256_undefined_si256()) noexcept {
                return _mm256_min_epi8(_First, _Second);
            }

            static __m256i _Max(
                const __m256i _First, const __m256i _Second, __m256i = _mm256_undefined_si256()) noexcept {
                return _mm256_max_epi8(_First, _Second);
            }

            static __m256i _Min_u(const __m256i _First, const __m256i _Second) noexcept {
                return _mm256_min_epu8(_First, _Second);
            }

            static __m256i _Max_u(const __m256i _First, const __m256i _Second) noexcept {
                return _mm256_max_epu8(_First, _Second);
            }

            static __m256i _Mask_cast(const __m256i _Mask) noexcept {
                return _Mask;
            }
        };
#endif // ^^^ !defined(_M_ARM64EC) ^^^

        struct _Traits_2_base {
            static constexpr bool _Is_floating = false;

            using _Signed_t   = int16_t;
            using _Unsigned_t = uint16_t;

            static constexpr _Signed_t _Init_min_val = static_cast<_Signed_t>(0x7FFF);
            static constexpr _Signed_t _Init_max_val = static_cast<_Signed_t>(0x8000);

            using _Minmax_i_t = _Min_max_2i;
            using _Minmax_u_t = _Min_max_2u;

#ifndef _M_ARM64EC
            static constexpr bool _Has_portion_max = true;
            static constexpr size_t _Portion_max   = 65536;
#endif // ^^^ !defined(_M_ARM64EC) ^^^
        };

#ifndef _M_ARM64EC
        struct _Traits_2_sse : _Traits_2_base, _Traits_sse_base {
            static __m128i _Load(const void* const _Src) noexcept {
                return _mm_loadu_si128(reinterpret_cast<const __m128i*>(_Src));
            }

            static __m128i _Sign_correction(const __m128i _Val, const bool _Sign) noexcept {
                alignas(16) static constexpr _Unsigned_t _Sign_corrections[2][8] = {
                    0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, {}};
                return _mm_sub_epi16(_Val, _mm_load_si128(reinterpret_cast<const __m128i*>(_Sign_corrections[_Sign])));
            }

            static __m128i _Inc(const __m128i _Idx) noexcept {
                return _mm_add_epi16(_Idx, _mm_set1_epi16(1));
            }

            template <class _Fn>
            static __m128i _H_func(const __m128i _Cur, const _Fn _Funct) noexcept {
                const __m128i _Shuf_w = _mm_set_epi8(13, 12, 15, 14, 9, 8, 11, 10, 5, 4, 7, 6, 1, 0, 3, 2);

                __m128i _H_min_val = _Cur;
                _H_min_val         = _Funct(_H_min_val, _mm_shuffle_epi32(_H_min_val, _MM_SHUFFLE(1, 0, 3, 2)));
                _H_min_val         = _Funct(_H_min_val, _mm_shuffle_epi32(_H_min_val, _MM_SHUFFLE(2, 3, 0, 1)));
                _H_min_val         = _Funct(_H_min_val, _mm_shuffle_epi8(_H_min_val, _Shuf_w));
                return _H_min_val;
            }

            static __m128i _H_min(const __m128i _Cur) noexcept {
                return _H_func(_Cur,
                    [](const __m128i _Val1, const __m128i _Val2) noexcept { return _mm_min_epi16(_Val1, _Val2); });
            }

            static __m128i _H_max(const __m128i _Cur) noexcept {
                return _H_func(_Cur,
                    [](const __m128i _Val1, const __m128i _Val2) noexcept { return _mm_max_epi16(_Val1, _Val2); });
            }

            static __m128i _H_min_u(const __m128i _Cur) noexcept {
                return _H_func(_Cur,
                    [](const __m128i _Val1, const __m128i _Val2) noexcept { return _mm_min_epu16(_Val1, _Val2); });
            }

            static __m128i _H_max_u(const __m128i _Cur) noexcept {
                return _H_func(_Cur,
                    [](const __m128i _Val1, const __m128i _Val2) noexcept { return _mm_max_epu16(_Val1, _Val2); });
            }

            static _Signed_t _Get_any(const __m128i _Cur) noexcept {
                return static_cast<_Signed_t>(_mm_cvtsi128_si32(_Cur));
            }

            static _Unsigned_t _Get_v_pos(const __m128i _Idx, const unsigned long _H_pos) noexcept {
                static constexpr _Unsigned_t _Shuf[] = {0x0100, 0x0302, 0x0504, 0x0706, 0x0908, 0x0B0A, 0x0D0C, 0x0F0E};

                return static_cast<_Unsigned_t>(
                    _mm_cvtsi128_si32(_mm_shuffle_epi8(_Idx, _mm_cvtsi32_si128(_Shuf[_H_pos >> 1]))));
            }

            static __m128i _Cmp_eq(const __m128i _First, const __m128i _Second) noexcept {
                return _mm_cmpeq_epi16(_First, _Second);
            }

            static __m128i _Cmp_gt(const __m128i _First, const __m128i _Second) noexcept {
                return _mm_cmpgt_epi16(_First, _Second);
            }

            static __m128i _Cmp_eq_idx(const __m128i _First, const __m128i _Second) noexcept {
                return _mm_cmpeq_epi16(_First, _Second);
            }

            static __m128i _Min(const __m128i _First, const __m128i _Second, __m128i = _mm_undefined_si128()) noexcept {
                return _mm_min_epi16(_First, _Second);
            }

            static __m128i _Max(const __m128i _First, const __m128i _Second, __m128i = _mm_undefined_si128()) noexcept {
                return _mm_max_epi16(_First, _Second);
            }

            static __m128i _Min_u(const __m128i _First, const __m128i _Second) noexcept {
                return _mm_min_epu16(_First, _Second);
            }

            static __m128i _Max_u(const __m128i _First, const __m128i _Second) noexcept {
                return _mm_max_epu16(_First, _Second);
            }

            static __m128i _Mask_cast(const __m128i _Mask) noexcept {
                return _Mask;
            }
        };

        struct _Traits_2_avx : _Traits_2_base, _Traits_avx_i_base {
            static __m256i _Load(const void* const _Src) noexcept {
                return _mm256_loadu_si256(reinterpret_cast<const __m256i*>(_Src));
            }

            static __m256i _Sign_correction(const __m256i _Val, const bool _Sign) noexcept {
                alignas(32) static constexpr _Unsigned_t _Sign_corrections[2][16] = {0x8000, 0x8000, 0x8000, 0x8000,
                    0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, {}};
                return _mm256_sub_epi16(
                    _Val, _mm256_load_si256(reinterpret_cast<const __m256i*>(_Sign_corrections[_Sign])));
            }

            static __m256i _Inc(const __m256i _Idx) noexcept {
                return _mm256_add_epi16(_Idx, _mm256_set1_epi16(1));
            }

            template <class _Fn>
            static __m256i _H_func(const __m256i _Cur, const _Fn _Funct) noexcept {
                const __m128i _Shuf_w = _mm_set_epi8(13, 12, 15, 14, 9, 8, 11, 10, 5, 4, 7, 6, 1, 0, 3, 2);

                __m256i _H_min_val = _Cur;
                _H_min_val         = _Funct(_H_min_val, _mm256_permute4x64_epi64(_H_min_val, _MM_SHUFFLE(1, 0, 3, 2)));
                _H_min_val         = _Funct(_H_min_val, _mm256_shuffle_epi32(_H_min_val, _MM_SHUFFLE(1, 0, 3, 2)));
                _H_min_val         = _Funct(_H_min_val, _mm256_shuffle_epi32(_H_min_val, _MM_SHUFFLE(2, 3, 0, 1)));
                _H_min_val = _Funct(_H_min_val, _mm256_shuffle_epi8(_H_min_val, _mm256_broadcastsi128_si256(_Shuf_w)));
                return _H_min_val;
            }

            static __m256i _H_min(const __m256i _Cur) noexcept {
                return _H_func(_Cur,
                    [](const __m256i _Val1, const __m256i _Val2) noexcept { return _mm256_min_epi16(_Val1, _Val2); });
            }

            static __m256i _H_max(const __m256i _Cur) noexcept {
                return _H_func(_Cur,
                    [](const __m256i _Val1, const __m256i _Val2) noexcept { return _mm256_max_epi16(_Val1, _Val2); });
            }

            static __m256i _H_min_u(const __m256i _Cur) noexcept {
                return _H_func(_Cur,
                    [](const __m256i _Val1, const __m256i _Val2) noexcept { return _mm256_min_epu16(_Val1, _Val2); });
            }

            static __m256i _H_max_u(const __m256i _Cur) noexcept {
                return _H_func(_Cur,
                    [](const __m256i _Val1, const __m256i _Val2) noexcept { return _mm256_max_epu16(_Val1, _Val2); });
            }

            static _Signed_t _Get_any(const __m256i _Cur) noexcept {
                return static_cast<_Signed_t>(_mm256_cvtsi256_si32(_Cur));
            }

            static _Unsigned_t _Get_v_pos(const __m256i _Idx, const unsigned long _H_pos) noexcept {
                const uint32_t _Part = _mm256_cvtsi256_si32(
                    _mm256_permutevar8x32_epi32(_Idx, _mm256_castsi128_si256(_mm_cvtsi32_si128(_H_pos >> 2))));
                return static_cast<_Unsigned_t>(_Part >> ((_H_pos & 0x2) << 3));
            }

            static __m256i _Cmp_eq(const __m256i _First, const __m256i _Second) noexcept {
                return _mm256_cmpeq_epi16(_First, _Second);
            }

            static __m256i _Cmp_gt(const __m256i _First, const __m256i _Second) noexcept {
                return _mm256_cmpgt_epi16(_First, _Second);
            }

            static __m256i _Cmp_eq_idx(const __m256i _First, const __m256i _Second) noexcept {
                return _mm256_cmpeq_epi16(_First, _Second);
            }

            static __m256i _Min(
                const __m256i _First, const __m256i _Second, __m256i = _mm256_undefined_si256()) noexcept {
                return _mm256_min_epi16(_First, _Second);
            }

            static __m256i _Max(
                const __m256i _First, const __m256i _Second, __m256i = _mm256_undefined_si256()) noexcept {
                return _mm256_max_epi16(_First, _Second);
            }

            static __m256i _Min_u(const __m256i _First, const __m256i _Second) noexcept {
                return _mm256_min_epu16(_First, _Second);
            }

            static __m256i _Max_u(const __m256i _First, const __m256i _Second) noexcept {
                return _mm256_max_epu16(_First, _Second);
            }

            static __m256i _Mask_cast(const __m256i _Mask) noexcept {
                return _Mask;
            }
        };
#endif // ^^^ !defined(_M_ARM64EC) ^^^

        struct _Traits_4_base {
            static constexpr bool _Is_floating = false;

            using _Signed_t   = int32_t;
            using _Unsigned_t = uint32_t;

            using _Minmax_i_t = _Min_max_4i;
            using _Minmax_u_t = _Min_max_4u;

            static constexpr _Signed_t _Init_min_val = static_cast<_Signed_t>(0x7FFF'FFFFUL);
            static constexpr _Signed_t _Init_max_val = static_cast<_Signed_t>(0x8000'0000UL);

#ifndef _M_ARM64EC
#ifdef _M_IX86
            static constexpr bool _Has_portion_max = false;
#else // ^^^ 32-bit / 64-bit vvv
            static constexpr bool _Has_portion_max = true;
            static constexpr size_t _Portion_max   = 0x1'0000'0000ULL;
#endif // ^^^ 64-bit ^^^
#endif // ^^^ !defined(_M_ARM64EC) ^^^
        };

#ifndef _M_ARM64EC
        struct _Traits_4_sse : _Traits_4_base, _Traits_sse_base {
            static __m128i _Load(const void* const _Src) noexcept {
                return _mm_loadu_si128(reinterpret_cast<const __m128i*>(_Src));
            }

            static __m128i _Sign_correction(const __m128i _Val, const bool _Sign) noexcept {
                alignas(16) static constexpr _Unsigned_t _Sign_corrections[2][4] = {
                    0x8000'0000UL, 0x8000'0000UL, 0x8000'0000UL, 0x8000'0000UL, {}};
                return _mm_sub_epi32(_Val, _mm_load_si128(reinterpret_cast<const __m128i*>(_Sign_corrections[_Sign])));
            }

            static __m128i _Inc(const __m128i _Idx) noexcept {
                return _mm_add_epi32(_Idx, _mm_set1_epi32(1));
            }

            template <class _Fn>
            static __m128i _H_func(const __m128i _Cur, const _Fn _Funct) noexcept {
                __m128i _H_min_val = _Cur;
                _H_min_val         = _Funct(_H_min_val, _mm_shuffle_epi32(_H_min_val, _MM_SHUFFLE(1, 0, 3, 2)));
                _H_min_val         = _Funct(_H_min_val, _mm_shuffle_epi32(_H_min_val, _MM_SHUFFLE(2, 3, 0, 1)));
                return _H_min_val;
            }

            static __m128i _H_min(const __m128i _Cur) noexcept {
                return _H_func(_Cur,
                    [](const __m128i _Val1, const __m128i _Val2) noexcept { return _mm_min_epi32(_Val1, _Val2); });
            }

            static __m128i _H_max(const __m128i _Cur) noexcept {
                return _H_func(_Cur,
                    [](const __m128i _Val1, const __m128i _Val2) noexcept { return _mm_max_epi32(_Val1, _Val2); });
            }

            static __m128i _H_min_u(const __m128i _Cur) noexcept {
                return _H_func(_Cur,
                    [](const __m128i _Val1, const __m128i _Val2) noexcept { return _mm_min_epu32(_Val1, _Val2); });
            }

            static __m128i _H_max_u(const __m128i _Cur) noexcept {
                return _H_func(_Cur,
                    [](const __m128i _Val1, const __m128i _Val2) noexcept { return _mm_max_epu32(_Val1, _Val2); });
            }

            static _Signed_t _Get_any(const __m128i _Cur) noexcept {
                return static_cast<_Signed_t>(_mm_cvtsi128_si32(_Cur));
            }

            static _Unsigned_t _Get_v_pos(const __m128i _Idx, const unsigned long _H_pos) noexcept {
                _Unsigned_t _Array[4];
                _mm_storeu_si128(reinterpret_cast<__m128i*>(&_Array), _Idx);
                return _Array[_H_pos >> 2];
            }

            static __m128i _Cmp_eq(const __m128i _First, const __m128i _Second) noexcept {
                return _mm_cmpeq_epi32(_First, _Second);
            }

            static __m128i _Cmp_gt(const __m128i _First, const __m128i _Second) noexcept {
                return _mm_cmpgt_epi32(_First, _Second);
            }

            static __m128i _Cmp_eq_idx(const __m128i _First, const __m128i _Second) noexcept {
                return _mm_cmpeq_epi32(_First, _Second);
            }

            static __m128i _Min(const __m128i _First, const __m128i _Second, __m128i = _mm_undefined_si128()) noexcept {
                return _mm_min_epi32(_First, _Second);
            }

            static __m128i _Max(const __m128i _First, const __m128i _Second, __m128i = _mm_undefined_si128()) noexcept {
                return _mm_max_epi32(_First, _Second);
            }

            static __m128i _Min_u(const __m128i _First, const __m128i _Second) noexcept {
                return _mm_min_epu32(_First, _Second);
            }

            static __m128i _Max_u(const __m128i _First, const __m128i _Second) noexcept {
                return _mm_max_epu32(_First, _Second);
            }

            static __m128i _Mask_cast(const __m128i _Mask) noexcept {
                return _Mask;
            }
        };

        struct _Traits_4_avx : _Traits_4_base, _Traits_avx_i_base {
            static __m256i _Load(const void* const _Src) noexcept {
                return _mm256_loadu_si256(reinterpret_cast<const __m256i*>(_Src));
            }

            static __m256i _Sign_correction(const __m256i _Val, const bool _Sign) noexcept {
                alignas(32) static constexpr _Unsigned_t _Sign_corrections[2][8] = {0x8000'0000UL, 0x8000'0000UL,
                    0x8000'0000UL, 0x8000'0000UL, 0x8000'0000UL, 0x8000'0000UL, 0x8000'0000UL, 0x8000'0000UL, {}};
                return _mm256_sub_epi32(
                    _Val, _mm256_load_si256(reinterpret_cast<const __m256i*>(_Sign_corrections[_Sign])));
            }

            static __m256i _Inc(const __m256i _Idx) noexcept {
                return _mm256_add_epi32(_Idx, _mm256_set1_epi32(1));
            }

            template <class _Fn>
            static __m256i _H_func(const __m256i _Cur, const _Fn _Funct) noexcept {
                __m256i _H_min_val = _Cur;
                _H_min_val         = _Funct(_H_min_val, _mm256_permute4x64_epi64(_H_min_val, _MM_SHUFFLE(1, 0, 3, 2)));
                _H_min_val         = _Funct(_H_min_val, _mm256_shuffle_epi32(_H_min_val, _MM_SHUFFLE(1, 0, 3, 2)));
                _H_min_val         = _Funct(_H_min_val, _mm256_shuffle_epi32(_H_min_val, _MM_SHUFFLE(2, 3, 0, 1)));
                return _H_min_val;
            }

            static __m256i _H_min(const __m256i _Cur) noexcept {
                return _H_func(_Cur,
                    [](const __m256i _Val1, const __m256i _Val2) noexcept { return _mm256_min_epi32(_Val1, _Val2); });
            }

            static __m256i _H_max(const __m256i _Cur) noexcept {
                return _H_func(_Cur,
                    [](const __m256i _Val1, const __m256i _Val2) noexcept { return _mm256_max_epi32(_Val1, _Val2); });
            }

            static __m256i _H_min_u(const __m256i _Cur) noexcept {
                return _H_func(_Cur,
                    [](const __m256i _Val1, const __m256i _Val2) noexcept { return _mm256_min_epu32(_Val1, _Val2); });
            }

            static __m256i _H_max_u(const __m256i _Cur) noexcept {
                return _H_func(_Cur,
                    [](const __m256i _Val1, const __m256i _Val2) noexcept { return _mm256_max_epu32(_Val1, _Val2); });
            }

            static _Signed_t _Get_any(const __m256i _Cur) noexcept {
                return static_cast<_Signed_t>(_mm256_cvtsi256_si32(_Cur));
            }

            static _Unsigned_t _Get_v_pos(const __m256i _Idx, const unsigned long _H_pos) noexcept {
                return _mm256_cvtsi256_si32(
                    _mm256_permutevar8x32_epi32(_Idx, _mm256_castsi128_si256(_mm_cvtsi32_si128(_H_pos >> 2))));
            }

            static __m256i _Cmp_eq(const __m256i _First, const __m256i _Second) noexcept {
                return _mm256_cmpeq_epi32(_First, _Second);
            }

            static __m256i _Cmp_gt(const __m256i _First, const __m256i _Second) noexcept {
                return _mm256_cmpgt_epi32(_First, _Second);
            }

            static __m256i _Cmp_eq_idx(const __m256i _First, const __m256i _Second) noexcept {
                return _mm256_cmpeq_epi32(_First, _Second);
            }

            static __m256i _Min(
                const __m256i _First, const __m256i _Second, __m256i = _mm256_undefined_si256()) noexcept {
                return _mm256_min_epi32(_First, _Second);
            }

            static __m256i _Max(
                const __m256i _First, const __m256i _Second, __m256i = _mm256_undefined_si256()) noexcept {
                return _mm256_max_epi32(_First, _Second);
            }

            static __m256i _Min_u(const __m256i _First, const __m256i _Second) noexcept {
                return _mm256_min_epu32(_First, _Second);
            }

            static __m256i _Max_u(const __m256i _First, const __m256i _Second) noexcept {
                return _mm256_max_epu32(_First, _Second);
            }

            static __m256i _Mask_cast(const __m256i _Mask) noexcept {
                return _Mask;
            }
        };
#endif // ^^^ !defined(_M_ARM64EC) ^^^

        struct _Traits_8_base {
            static constexpr bool _Is_floating = false;

            using _Signed_t   = int64_t;
            using _Unsigned_t = uint64_t;

            static constexpr _Signed_t _Init_min_val = static_cast<_Signed_t>(0x7FFF'FFFF'FFFF'FFFFULL);
            static constexpr _Signed_t _Init_max_val = static_cast<_Signed_t>(0x8000'0000'0000'0000ULL);

            using _Minmax_i_t = _Min_max_8i;
            using _Minmax_u_t = _Min_max_8u;

#ifndef _M_ARM64EC
            static constexpr bool _Has_portion_max = false;
#endif // ^^^ !defined(_M_ARM64EC) ^^^
        };

#ifndef _M_ARM64EC
        struct _Traits_8_sse : _Traits_8_base, _Traits_sse_base {
            static __m128i _Load(const void* const _Src) noexcept {
                return _mm_loadu_si128(reinterpret_cast<const __m128i*>(_Src));
            }

            static __m128i _Sign_correction(const __m128i _Val, const bool _Sign) noexcept {
                alignas(16) static constexpr _Unsigned_t _Sign_corrections[2][2] = {
                    0x8000'0000'0000'0000ULL, 0x8000'0000'0000'0000ULL, {}};
                return _mm_sub_epi64(_Val, _mm_load_si128(reinterpret_cast<const __m128i*>(_Sign_corrections[_Sign])));
            }

            static __m128i _Inc(const __m128i _Idx) noexcept {
                return _mm_add_epi64(_Idx, _mm_set1_epi64x(1));
            }

            template <class _Fn>
            static __m128i _H_func(const __m128i _Cur, const _Fn _Funct) noexcept {
                _Signed_t _H_min_a       = _Get_any(_Cur);
                const _Signed_t _H_min_b = _Get_any(_mm_bsrli_si128(_Cur, 8));
                if (_Funct(_H_min_b, _H_min_a)) {
                    _H_min_a = _H_min_b;
                }
                return _mm_set1_epi64x(_H_min_a);
            }

            static __m128i _H_min(const __m128i _Cur) noexcept {
                return _H_func(_Cur, [](const _Signed_t _Lhs, const _Signed_t _Rhs) noexcept { return _Lhs < _Rhs; });
            }

            static __m128i _H_max(const __m128i _Cur) noexcept {
                return _H_func(_Cur, [](const _Signed_t _Lhs, const _Signed_t _Rhs) noexcept { return _Lhs > _Rhs; });
            }

            static __m128i _H_min_u(const __m128i _Cur) noexcept {
                return _H_func(
                    _Cur, [](const _Unsigned_t _Lhs, const _Unsigned_t _Rhs) noexcept { return _Lhs < _Rhs; });
            }

            static __m128i _H_max_u(const __m128i _Cur) noexcept {
                return _H_func(
                    _Cur, [](const _Unsigned_t _Lhs, const _Unsigned_t _Rhs) noexcept { return _Lhs > _Rhs; });
            }

            static _Signed_t _Get_any(const __m128i _Cur) noexcept {
                // With optimizations enabled, compiles into register movement, rather than an actual stack spill.
                // Works around the absence of _mm_cvtsi128_si64 on 32-bit.
                return static_cast<_Signed_t>(_Get_v_pos(_Cur, 0));
            }

            static _Unsigned_t _Get_v_pos(const __m128i _Idx, const unsigned long _H_pos) noexcept {
                _Unsigned_t _Array[2];
                _mm_storeu_si128(reinterpret_cast<__m128i*>(&_Array), _Idx);
                return _Array[_H_pos >> 3];
            }

            static __m128i _Cmp_eq(const __m128i _First, const __m128i _Second) noexcept {
                return _mm_cmpeq_epi64(_First, _Second);
            }

            static __m128i _Cmp_gt(const __m128i _First, const __m128i _Second) noexcept {
                return _mm_cmpgt_epi64(_First, _Second);
            }

            static __m128i _Cmp_eq_idx(const __m128i _First, const __m128i _Second) noexcept {
                return _mm_cmpeq_epi64(_First, _Second);
            }

            static __m128i _Min(const __m128i _First, const __m128i _Second, const __m128i _Mask) noexcept {
                return _mm_blendv_epi8(_First, _Second, _Mask);
            }

            static __m128i _Max(const __m128i _First, const __m128i _Second, const __m128i _Mask) noexcept {
                return _mm_blendv_epi8(_First, _Second, _Mask);
            }

            static __m128i _Min(const __m128i _First, const __m128i _Second) noexcept {
                return _mm_blendv_epi8(_First, _Second, _Cmp_gt(_First, _Second));
            }

            static __m128i _Max(const __m128i _First, const __m128i _Second) noexcept {
                return _mm_blendv_epi8(_First, _Second, _Cmp_gt(_Second, _First));
            }

            static __m128i _Mask_cast(const __m128i _Mask) noexcept {
                return _Mask;
            }
        };

        struct _Traits_8_avx : _Traits_8_base, _Traits_avx_i_base {
            static __m256i _Load(const void* const _Src) noexcept {
                return _mm256_loadu_si256(reinterpret_cast<const __m256i*>(_Src));
            }

            static __m256i _Sign_correction(const __m256i _Val, const bool _Sign) noexcept {
                alignas(32) static constexpr _Unsigned_t _Sign_corrections[2][4] = {0x8000'0000'0000'0000ULL,
                    0x8000'0000'0000'0000ULL, 0x8000'0000'0000'0000ULL, 0x8000'0000'0000'0000ULL, {}};
                return _mm256_sub_epi64(
                    _Val, _mm256_load_si256(reinterpret_cast<const __m256i*>(_Sign_corrections[_Sign])));
            }

            static __m256i _Inc(const __m256i _Idx) noexcept {
                return _mm256_add_epi64(_Idx, _mm256_set1_epi64x(1));
            }

            template <class _Fn>
            static __m256i _H_func(const __m256i _Cur, const _Fn _Funct) noexcept {
                alignas(32) _Signed_t _Array[4];
                _mm256_store_si256(reinterpret_cast<__m256i*>(_Array), _Cur);

                _Signed_t _H_min_v = _Array[0];

                if (_Funct(_Array[1], _H_min_v)) {
                    _H_min_v = _Array[1];
                }

                if (_Funct(_Array[2], _H_min_v)) {
                    _H_min_v = _Array[2];
                }

                if (_Funct(_Array[3], _H_min_v)) {
                    _H_min_v = _Array[3];
                }

                return _mm256_set1_epi64x(_H_min_v);
            }

            static __m256i _H_min(const __m256i _Cur) noexcept {
                return _H_func(_Cur, [](const _Signed_t _Lhs, const _Signed_t _Rhs) noexcept { return _Lhs < _Rhs; });
            }

            static __m256i _H_max(const __m256i _Cur) noexcept {
                return _H_func(_Cur, [](const _Signed_t _Lhs, const _Signed_t _Rhs) noexcept { return _Lhs > _Rhs; });
            }

            static __m256i _H_min_u(const __m256i _Cur) noexcept {
                return _H_func(
                    _Cur, [](const _Unsigned_t _Lhs, const _Unsigned_t _Rhs) noexcept { return _Lhs < _Rhs; });
            }

            static __m256i _H_max_u(const __m256i _Cur) noexcept {
                return _H_func(
                    _Cur, [](const _Unsigned_t _Lhs, const _Unsigned_t _Rhs) noexcept { return _Lhs > _Rhs; });
            }

            static _Signed_t _Get_any(const __m256i _Cur) noexcept {
                return _Traits_8_sse::_Get_any(_mm256_castsi256_si128(_Cur));
            }

            static _Unsigned_t _Get_v_pos(const __m256i _Idx, const unsigned long _H_pos) noexcept {
                _Unsigned_t _Array[4];
                _mm256_storeu_si256(reinterpret_cast<__m256i*>(&_Array), _Idx);
                return _Array[_H_pos >> 3];
            }

            static __m256i _Cmp_eq(const __m256i _First, const __m256i _Second) noexcept {
                return _mm256_cmpeq_epi64(_First, _Second);
            }

            static __m256i _Cmp_gt(const __m256i _First, const __m256i _Second) noexcept {
                return _mm256_cmpgt_epi64(_First, _Second);
            }

            static __m256i _Cmp_eq_idx(const __m256i _First, const __m256i _Second) noexcept {
                return _mm256_cmpeq_epi64(_First, _Second);
            }

            static __m256i _Min(const __m256i _First, const __m256i _Second, const __m256i _Mask) noexcept {
                return _mm256_blendv_epi8(_First, _Second, _Mask);
            }

            static __m256i _Max(const __m256i _First, const __m256i _Second, const __m256i _Mask) noexcept {
                return _mm256_blendv_epi8(_First, _Second, _Mask);
            }

            static __m256i _Min(const __m256i _First, const __m256i _Second) noexcept {
                return _mm256_blendv_epi8(_First, _Second, _Cmp_gt(_First, _Second));
            }

            static __m256i _Max(const __m256i _First, const __m256i _Second) noexcept {
                return _mm256_blendv_epi8(_First, _Second, _Cmp_gt(_Second, _First));
            }

            static __m256i _Mask_cast(const __m256i _Mask) noexcept {
                return _Mask;
            }
        };
#endif // ^^^ !defined(_M_ARM64EC) ^^^

        struct _Traits_f_base {
            static constexpr bool _Is_floating = true;

            using _Signed_t   = float;
            using _Unsigned_t = void;

            static constexpr _Signed_t _Init_min_val = __builtin_huge_valf();
            static constexpr _Signed_t _Init_max_val = -__builtin_huge_valf();

            using _Minmax_i_t = _Min_max_f;
            using _Minmax_u_t = void;

#ifndef _M_ARM64EC
#ifdef _M_IX86
            static constexpr bool _Has_portion_max = false;
#else // ^^^ 32-bit / 64-bit vvv
            static constexpr bool _Has_portion_max = true;
            static constexpr size_t _Portion_max   = 0x1'0000'0000ULL;
#endif // ^^^ 64-bit ^^^
#endif // ^^^ !defined(_M_ARM64EC) ^^^
        };

#ifndef _M_ARM64EC
        struct _Traits_f_sse : _Traits_f_base, _Traits_sse_base {
            static __m128 _Load(const void* const _Src) noexcept {
                return _mm_loadu_ps(reinterpret_cast<const float*>(_Src));
            }

            static __m128 _Sign_correction(const __m128 _Val, bool) noexcept {
                return _Val;
            }

            static __m128i _Inc(const __m128i _Idx) noexcept {
                return _mm_add_epi32(_Idx, _mm_set1_epi32(1));
            }

            template <class _Fn>
            static __m128 _H_func(const __m128 _Cur, const _Fn _Funct) noexcept {
                __m128 _H_min_val = _Cur;
                _H_min_val        = _Funct(_mm_shuffle_ps(_H_min_val, _H_min_val, _MM_SHUFFLE(2, 3, 0, 1)), _H_min_val);
                _H_min_val        = _Funct(_mm_shuffle_ps(_H_min_val, _H_min_val, _MM_SHUFFLE(1, 0, 3, 2)), _H_min_val);
                return _H_min_val;
            }

            static __m128 _H_min(const __m128 _Cur) noexcept {
                return _H_func(
                    _Cur, [](const __m128 _Val1, const __m128 _Val2) noexcept { return _mm_min_ps(_Val1, _Val2); });
            }

            static __m128 _H_max(const __m128 _Cur) noexcept {
                return _H_func(
                    _Cur, [](const __m128 _Val1, const __m128 _Val2) noexcept { return _mm_max_ps(_Val1, _Val2); });
            }

            static __m128i _H_min_u(const __m128i _Cur) noexcept {
                return _Traits_4_sse::_H_min_u(_Cur);
            }

            static __m128i _H_max_u(const __m128i _Cur) noexcept {
                return _Traits_4_sse::_H_max_u(_Cur);
            }

            static float _Get_any(const __m128 _Cur) noexcept {
                return _mm_cvtss_f32(_Cur);
            }

            static uint32_t _Get_v_pos(const __m128i _Idx, const unsigned long _H_pos) noexcept {
                return _Traits_4_sse::_Get_v_pos(_Idx, _H_pos);
            }

            static __m128 _Cmp_eq(const __m128 _First, const __m128 _Second) noexcept {
                return _mm_cmpeq_ps(_First, _Second);
            }

            static __m128 _Cmp_gt(const __m128 _First, const __m128 _Second) noexcept {
                return _mm_cmpgt_ps(_First, _Second);
            }

            static __m128i _Cmp_eq_idx(const __m128i _First, const __m128i _Second) noexcept {
                return _mm_cmpeq_epi32(_First, _Second);
            }

            static __m128 _Min(const __m128 _First, const __m128 _Second, __m128 = _mm_undefined_ps()) noexcept {
                return _mm_min_ps(_Second, _First);
            }

            static __m128 _Max(const __m128 _First, const __m128 _Second, __m128 = _mm_undefined_ps()) noexcept {
                return _mm_max_ps(_Second, _First);
            }

            static __m128i _Mask_cast(const __m128 _Mask) noexcept {
                return _mm_castps_si128(_Mask);
            }
        };

        struct _Traits_f_avx : _Traits_f_base, _Traits_avx_base {
            static constexpr size_t _Tail_mask = 0x1C;

            static __m256 _Blendval(const __m256 _Px1, const __m256 _Px2, const __m256i _Msk) noexcept {
                return _mm256_blendv_ps(_Px1, _Px2, _mm256_castsi256_ps(_Msk));
            }

            static __m256 _Load(const void* const _Src) noexcept {
                return _mm256_loadu_ps(reinterpret_cast<const float*>(_Src));
            }

            static __m256 _Load_mask(const void* const _Src, const __m256i _Mask) noexcept {
                return _mm256_maskload_ps(reinterpret_cast<const float*>(_Src), _Mask);
            }

            static __m256 _Sign_correction(const __m256 _Val, bool) noexcept {
                return _Val;
            }

            static __m256i _Inc(const __m256i _Idx) noexcept {
                return _mm256_add_epi32(_Idx, _mm256_set1_epi32(1));
            }

            template <class _Fn>
            static __m256 _H_func(const __m256 _Cur, const _Fn _Funct) noexcept {
                __m256 _H_min_val = _Cur;
                _H_min_val = _Funct(_mm256_shuffle_ps(_H_min_val, _H_min_val, _MM_SHUFFLE(2, 3, 0, 1)), _H_min_val);
                _H_min_val = _Funct(_mm256_shuffle_ps(_H_min_val, _H_min_val, _MM_SHUFFLE(1, 0, 3, 2)), _H_min_val);
                _H_min_val = _Funct(_mm256_permute2f128_ps(_H_min_val, _mm256_undefined_ps(), 0x01), _H_min_val);
                return _H_min_val;
            }

            static __m256 _H_min(const __m256 _Cur) noexcept {
                return _H_func(
                    _Cur, [](const __m256 _Val1, const __m256 _Val2) noexcept { return _mm256_min_ps(_Val1, _Val2); });
            }

            static __m256 _H_max(const __m256 _Cur) noexcept {
                return _H_func(
                    _Cur, [](const __m256 _Val1, const __m256 _Val2) noexcept { return _mm256_max_ps(_Val1, _Val2); });
            }

            static __m256i _H_min_u(const __m256i _Cur) noexcept {
                return _Traits_4_avx::_H_min_u(_Cur);
            }

            static __m256i _H_max_u(const __m256i _Cur) noexcept {
                return _Traits_4_avx::_H_max_u(_Cur);
            }

            static float _Get_any(const __m256 _Cur) noexcept {
                return _mm256_cvtss_f32(_Cur);
            }

            static uint32_t _Get_v_pos(const __m256i _Idx, const unsigned long _H_pos) noexcept {
                return _Traits_4_avx::_Get_v_pos(_Idx, _H_pos);
            }

            static __m256 _Cmp_eq(const __m256 _First, const __m256 _Second) noexcept {
                return _mm256_cmp_ps(_First, _Second, _CMP_EQ_OQ);
            }

            static __m256 _Cmp_gt(const __m256 _First, const __m256 _Second) noexcept {
                return _mm256_cmp_ps(_First, _Second, _CMP_GT_OQ);
            }

            static __m256i _Cmp_eq_idx(const __m256i _First, const __m256i _Second) noexcept {
                return _mm256_cmpeq_epi32(_First, _Second);
            }

            static __m256 _Min(const __m256 _First, const __m256 _Second, __m256 = _mm256_undefined_ps()) noexcept {
                return _mm256_min_ps(_Second, _First);
            }

            static __m256 _Max(const __m256 _First, const __m256 _Second, __m256 = _mm256_undefined_ps()) noexcept {
                return _mm256_max_ps(_Second, _First);
            }

            static __m256i _Mask_cast(const __m256 _Mask) noexcept {
                return _mm256_castps_si256(_Mask);
            }
        };
#endif // ^^^ !defined(_M_ARM64EC) ^^^

        struct _Traits_d_base {
            static constexpr bool _Is_floating = true;

            using _Signed_t   = double;
            using _Unsigned_t = void;

            static constexpr _Signed_t _Init_min_val = __builtin_huge_val();
            static constexpr _Signed_t _Init_max_val = -__builtin_huge_val();

            using _Minmax_i_t = _Min_max_d;
            using _Minmax_u_t = void;

#ifndef _M_ARM64EC
            static constexpr bool _Has_portion_max = false;
#endif // ^^^ !defined(_M_ARM64EC) ^^^
        };

#ifndef _M_ARM64EC
        struct _Traits_d_sse : _Traits_d_base, _Traits_sse_base {
            static __m128d _Load(const void* const _Src) noexcept {
                return _mm_loadu_pd(reinterpret_cast<const double*>(_Src));
            }

            static __m128d _Sign_correction(const __m128d _Val, bool) noexcept {
                return _Val;
            }

            static __m128i _Inc(const __m128i _Idx) noexcept {
                return _mm_add_epi64(_Idx, _mm_set1_epi64x(1));
            }

            template <class _Fn>
            static __m128d _H_func(const __m128d _Cur, const _Fn _Funct) noexcept {
                __m128d _H_min_val = _Cur;
                _H_min_val         = _Funct(_mm_shuffle_pd(_H_min_val, _H_min_val, 1), _H_min_val);
                return _H_min_val;
            }

            static __m128d _H_min(const __m128d _Cur) noexcept {
                return _H_func(
                    _Cur, [](const __m128d _Val1, const __m128d _Val2) noexcept { return _mm_min_pd(_Val1, _Val2); });
            }

            static __m128d _H_max(const __m128d _Cur) noexcept {
                return _H_func(
                    _Cur, [](const __m128d _Val1, const __m128d _Val2) noexcept { return _mm_max_pd(_Val1, _Val2); });
            }

            static __m128i _H_min_u(const __m128i _Cur) noexcept {
                return _Traits_8_sse::_H_min_u(_Cur);
            }

            static __m128i _H_max_u(const __m128i _Cur) noexcept {
                return _Traits_8_sse::_H_max_u(_Cur);
            }
            static double _Get_any(const __m128d _Cur) noexcept {
                return _mm_cvtsd_f64(_Cur);
            }

            static uint64_t _Get_v_pos(const __m128i _Idx, const unsigned long _H_pos) noexcept {
                return _Traits_8_sse::_Get_v_pos(_Idx, _H_pos);
            }

            static __m128d _Cmp_eq(const __m128d _First, const __m128d _Second) noexcept {
                return _mm_cmpeq_pd(_First, _Second);
            }

            static __m128d _Cmp_gt(const __m128d _First, const __m128d _Second) noexcept {
                return _mm_cmpgt_pd(_First, _Second);
            }

            static __m128i _Cmp_eq_idx(const __m128i _First, const __m128i _Second) noexcept {
                return _mm_cmpeq_epi64(_First, _Second);
            }

            static __m128d _Min(const __m128d _First, const __m128d _Second, __m128d = _mm_undefined_pd()) noexcept {
                return _mm_min_pd(_Second, _First);
            }

            static __m128d _Max(const __m128d _First, const __m128d _Second, __m128d = _mm_undefined_pd()) noexcept {
                return _mm_max_pd(_Second, _First);
            }

            static __m128i _Mask_cast(const __m128d _Mask) noexcept {
                return _mm_castpd_si128(_Mask);
            }
        };

        struct _Traits_d_avx : _Traits_d_base, _Traits_avx_base {
            static constexpr size_t _Tail_mask = 0x18;

            static __m256d _Blendval(const __m256d _Px1, const __m256d _Px2, const __m256i _Msk) noexcept {
                return _mm256_blendv_pd(_Px1, _Px2, _mm256_castsi256_pd(_Msk));
            }

            static __m256d _Load(const void* const _Src) noexcept {
                return _mm256_loadu_pd(reinterpret_cast<const double*>(_Src));
            }

            static __m256d _Load_mask(const void* const _Src, const __m256i _Mask) noexcept {
                return _mm256_maskload_pd(reinterpret_cast<const double*>(_Src), _Mask);
            }

            static __m256d _Sign_correction(const __m256d _Val, bool) noexcept {
                return _Val;
            }

            static __m256i _Inc(const __m256i _Idx) noexcept {
                return _mm256_add_epi64(_Idx, _mm256_set1_epi64x(1));
            }

            template <class _Fn>
            static __m256d _H_func(const __m256d _Cur, const _Fn _Funct) noexcept {
                __m256d _H_min_val = _Cur;
                _H_min_val         = _Funct(_mm256_shuffle_pd(_H_min_val, _H_min_val, 0b0101), _H_min_val);
                _H_min_val         = _Funct(_mm256_permute4x64_pd(_H_min_val, _MM_SHUFFLE(1, 0, 3, 2)), _H_min_val);
                return _H_min_val;
            }

            static __m256d _H_min(const __m256d _Cur) noexcept {
                return _H_func(_Cur,
                    [](const __m256d _Val1, const __m256d _Val2) noexcept { return _mm256_min_pd(_Val1, _Val2); });
            }

            static __m256d _H_max(const __m256d _Cur) noexcept {
                return _H_func(_Cur,
                    [](const __m256d _Val1, const __m256d _Val2) noexcept { return _mm256_max_pd(_Val1, _Val2); });
            }

            static __m256i _H_min_u(const __m256i _Cur) noexcept {
                return _Traits_8_avx::_H_min_u(_Cur);
            }

            static __m256i _H_max_u(const __m256i _Cur) noexcept {
                return _Traits_8_avx::_H_max_u(_Cur);
            }

            static double _Get_any(const __m256d _Cur) noexcept {
                return _mm256_cvtsd_f64(_Cur);
            }

            static uint64_t _Get_v_pos(const __m256i _Idx, const unsigned long _H_pos) noexcept {
                return _Traits_8_avx::_Get_v_pos(_Idx, _H_pos);
            }

            static __m256d _Cmp_eq(const __m256d _First, const __m256d _Second) noexcept {
                return _mm256_cmp_pd(_First, _Second, _CMP_EQ_OQ);
            }

            static __m256d _Cmp_gt(const __m256d _First, const __m256d _Second) noexcept {
                return _mm256_cmp_pd(_First, _Second, _CMP_GT_OQ);
            }

            static __m256i _Cmp_eq_idx(const __m256i _First, const __m256i _Second) noexcept {
                return _mm256_cmpeq_epi64(_First, _Second);
            }

            static __m256d _Min(const __m256d _First, const __m256d _Second, __m256d = _mm256_undefined_pd()) noexcept {
                return _mm256_min_pd(_Second, _First);
            }

            static __m256d _Max(const __m256d _First, const __m256d _Second, __m256d = _mm256_undefined_pd()) noexcept {
                return _mm256_max_pd(_Second, _First);
            }

            static __m256i _Mask_cast(const __m256d _Mask) noexcept {
                return _mm256_castpd_si256(_Mask);
            }
        };
#endif // ^^^ !defined(_M_ARM64EC) ^^^

        struct _Traits_1 {
            using _Scalar = _Traits_scalar<_Traits_1_base>;
#ifndef _M_ARM64EC
            using _Sse = _Traits_1_sse;
            using _Avx = _Traits_1_avx;
#endif // ^^^ !defined(_M_ARM64EC) ^^^
        };

        struct _Traits_2 {
            using _Scalar = _Traits_scalar<_Traits_2_base>;
#ifndef _M_ARM64EC
            using _Sse = _Traits_2_sse;
            using _Avx = _Traits_2_avx;
#endif // ^^^ !defined(_M_ARM64EC) ^^^
        };

        struct _Traits_4 {
            using _Scalar = _Traits_scalar<_Traits_4_base>;
#ifndef _M_ARM64EC
            using _Sse = _Traits_4_sse;
            using _Avx = _Traits_4_avx;
#endif // ^^^ !defined(_M_ARM64EC) ^^^
        };

        struct _Traits_8 {
            using _Scalar = _Traits_scalar<_Traits_8_base>;
#ifndef _M_ARM64EC
            using _Sse = _Traits_8_sse;
            using _Avx = _Traits_8_avx;
#endif // ^^^ !defined(_M_ARM64EC) ^^^
        };

        struct _Traits_f {
            using _Scalar = _Traits_scalar<_Traits_f_base>;
#ifndef _M_ARM64EC
            using _Sse = _Traits_f_sse;
            using _Avx = _Traits_f_avx;
#endif // ^^^ !defined(_M_ARM64EC) ^^^
        };

        struct _Traits_d {
            using _Scalar = _Traits_scalar<_Traits_d_base>;
#ifndef _M_ARM64EC
            using _Sse = _Traits_d_sse;
            using _Avx = _Traits_d_avx;
#endif // ^^^ !defined(_M_ARM64EC) ^^^
        };

        template <class _Ty>
        const void* _Min_tail(const void* const _First, const void* const _Last, const void* _Res, _Ty _Cur) noexcept {
            for (auto _Ptr = static_cast<const _Ty*>(_First); _Ptr != _Last; ++_Ptr) {
                if (*_Ptr < _Cur) {
                    _Res = _Ptr;
                    _Cur = *_Ptr;
                }
            }

            return _Res;
        }

        template <class _Ty>
        const void* _Max_tail(const void* const _First, const void* const _Last, const void* _Res, _Ty _Cur) noexcept {
            for (auto _Ptr = static_cast<const _Ty*>(_First); _Ptr != _Last; ++_Ptr) {
                if (_Cur < *_Ptr) {
                    _Res = _Ptr;
                    _Cur = *_Ptr;
                }
            }

            return _Res;
        }

        template <class _Ty>
        _Min_max_element_t _Both_tail(const void* const _First, const void* const _Last, _Min_max_element_t& _Res,
            _Ty _Cur_min, _Ty _Cur_max) noexcept {
            for (auto _Ptr = static_cast<const _Ty*>(_First); _Ptr != _Last; ++_Ptr) {
                if (*_Ptr < _Cur_min) {
                    _Res._Min = _Ptr;
                    _Cur_min  = *_Ptr;
                }
                // Not else!
                // * Needed for correctness if start with maximum, as we don't handle specially the first element.
                // * Promote branchless code generation.
                if (_Cur_max <= *_Ptr) {
                    _Res._Max = _Ptr;
                    _Cur_max  = *_Ptr;
                }
            }

            return _Res;
        }

        template <_Min_max_mode _Mode, class _Traits>
        auto _Minmax_element_impl(const void* _First, const void* const _Last, const bool _Sign) noexcept {
            _Min_max_element_t _Res = {_First, _First};
            auto _Cur_min_val       = _Traits::_Init_min_val;
            auto _Cur_max_val       = _Traits::_Init_max_val;

            if constexpr (_Traits::_Vectorized) {
#ifdef _M_ARM64EC
                static_assert(false, "No vectorization for _M_ARM64EC yet");
#else // ^^^ defined(_M_ARM64EC) / !defined(_M_ARM64EC) vvv
                auto _Base                = static_cast<const char*>(_First);
                size_t _Portion_byte_size = _Byte_length(_First, _Last) & ~_Traits::_Vec_mask;

                if constexpr (_Traits::_Has_portion_max) {
                    // vector of indices will wrap around at exactly this size
                    constexpr size_t _Max_portion_byte_size = _Traits::_Portion_max * _Traits::_Vec_size;
                    if (_Portion_byte_size > _Max_portion_byte_size) {
                        _Portion_byte_size = _Max_portion_byte_size;
                    }
                }

                const void* _Stop_at = _First;
                _Advance_bytes(_Stop_at, _Portion_byte_size);

                // Load values and if unsigned adjust them to be signed (for signed vector comparisons)
                auto _Cur_vals     = _Traits::_Sign_correction(_Traits::_Load(_First), _Sign);
                auto _Cur_vals_min = _Cur_vals; // vector of vertical minimum values
                auto _Cur_idx_min  = _Traits::_Zero(); // vector of vertical minimum indices
                auto _Cur_vals_max = _Cur_vals; // vector of vertical maximum values
                auto _Cur_idx_max  = _Traits::_Zero(); // vector of vertical maximum indices
                auto _Cur_idx      = _Traits::_Zero(); // current vector of indices

                const auto _Update_min_max = [&](const auto _Cur_vals, [[maybe_unused]] const auto _Blend_idx_0,
                                                 const auto _Blend_idx_1) noexcept {
                    if constexpr ((_Mode & _Mode_min) != 0) {
                        // Looking for the first occurrence of minimum, don't overwrite with newly found occurrences
                        const auto _Is_less = _Traits::_Cmp_gt(_Cur_vals_min, _Cur_vals); // _Cur_vals < _Cur_vals_min
                        // Remember their vertical indices
                        _Cur_idx_min  = _Blend_idx_1(_Cur_idx_min, _Cur_idx, _Traits::_Mask_cast(_Is_less));
                        _Cur_vals_min = _Traits::_Min(_Cur_vals_min, _Cur_vals, _Is_less); // Update the current minimum
                    }

                    if constexpr (_Mode == _Mode_max) {
                        // Looking for the first occurrence of maximum, don't overwrite with newly found occurrences
                        // _Cur_vals > _Cur_vals_max
                        const auto _Is_greater = _Traits::_Cmp_gt(_Cur_vals, _Cur_vals_max);
                        // Remember their vertical indices
                        _Cur_idx_max = _Blend_idx_1(_Cur_idx_max, _Cur_idx, _Traits::_Mask_cast(_Is_greater));
                        // Update the current maximum
                        _Cur_vals_max = _Traits::_Max(_Cur_vals_max, _Cur_vals, _Is_greater);
                    } else if constexpr (_Mode == _Mode_both) {
                        // Looking for the last occurrence of maximum, do overwrite with newly found occurrences
                        // !(_Cur_vals >= _Cur_vals_max)
                        const auto _Is_less = _Traits::_Cmp_gt(_Cur_vals_max, _Cur_vals);
                        // Remember their vertical indices
                        _Cur_idx_max  = _Blend_idx_0(_Cur_idx_max, _Cur_idx, _Traits::_Mask_cast(_Is_less));
                        _Cur_vals_max = _Traits::_Max(_Cur_vals, _Cur_vals_max, _Is_less); // Update the current maximum
                    }
                };

                const auto _Blend_idx_0 = [](const auto _Prev, const auto _Cur, const auto _Mask) noexcept {
                    return _Traits::_Blend(_Cur, _Prev, _Mask);
                };

                const auto _Blend_idx_1 = [](const auto _Prev, const auto _Cur, const auto _Mask) noexcept {
                    return _Traits::_Blend(_Prev, _Cur, _Mask);
                };

                for (;;) {
                    _Advance_bytes(_First, _Traits::_Vec_size);

                    // Increment vertical indices. Will stop at exactly wrap around, if not reach the end before
                    _Cur_idx = _Traits::_Inc(_Cur_idx);

                    if (_First != _Stop_at) {
                        // This is the main part, finding vertical minimum/maximum

                        // Load values and if unsigned adjust them to be signed (for signed vector comparisons)
                        _Cur_vals = _Traits::_Sign_correction(_Traits::_Load(_First), _Sign);

                        _Update_min_max(_Cur_vals, _Blend_idx_0, _Blend_idx_1);
                    } else {
                        if constexpr (_Traits::_Tail_mask != 0) {
                            const size_t _Remaining_byte_size = _Byte_length(_First, _Last);
                            bool _Last_portion;

                            if constexpr (_Traits::_Has_portion_max) {
                                _Last_portion = (_Remaining_byte_size & ~_Traits::_Vec_mask) == 0;
                            } else {
                                _Last_portion = true;
                            }

                            const size_t _Tail_byte_size = _Remaining_byte_size & _Traits::_Tail_mask;

                            if (_Last_portion && _Tail_byte_size != 0) {
                                const auto _Tail_mask = _Avx2_tail_mask_32(_Tail_byte_size);
                                const auto _Tail_vals =
                                    _Traits::_Sign_correction(_Traits::_Load_mask(_First, _Tail_mask), _Sign);
                                _Cur_vals = _Traits::_Blendval(_Cur_vals, _Tail_vals, _Tail_mask);

                                const auto _Blend_idx_0_mask = [_Tail_mask](const auto _Prev, const auto _Cur,
                                                                   const auto _Mask) noexcept {
                                    return _Traits::_Blend(_Prev, _Cur, _mm256_andnot_si256(_Mask, _Tail_mask));
                                };

                                const auto _Blend_idx_1_mask = [_Tail_mask](const auto _Prev, const auto _Cur,
                                                                   const auto _Mask) noexcept {
                                    return _Traits::_Blend(_Prev, _Cur, _mm256_and_si256(_Tail_mask, _Mask));
                                };

                                _Update_min_max(_Cur_vals, _Blend_idx_0_mask, _Blend_idx_1_mask);
                                _Advance_bytes(_First, _Tail_byte_size);
                            }
                        }

                        // Reached end or indices wrap around point.
                        // Compute horizontal min and/or max. Determine horizontal and vertical position of it.

                        if constexpr ((_Mode & _Mode_min) != 0) {
                            // Vector populated by the smallest element
                            const auto _H_min     = _Traits::_H_min(_Cur_vals_min);
                            const auto _H_min_val = _Traits::_Get_any(_H_min); // Get any element of it

                            if (_H_min_val < _Cur_min_val) { // Current horizontal min is less than the old
                                _Cur_min_val = _H_min_val; // update min
                                // Mask of all elems eq to min
                                const auto _Eq_mask = _Traits::_Cmp_eq(_H_min, _Cur_vals_min);
                                unsigned long _Mask = _Traits::_Mask(_Traits::_Mask_cast(_Eq_mask));
                                // Indices of minimum elements or the greatest index if none
                                const auto _Idx_min_val =
                                    _Traits::_Blend(_Traits::_All_ones(), _Cur_idx_min, _Traits::_Mask_cast(_Eq_mask));
                                auto _Idx_min = _Traits::_H_min_u(_Idx_min_val); // The smallest indices
                                // Select the smallest vertical indices from the smallest element mask
                                _Mask &= _Traits::_Mask(_Traits::_Cmp_eq_idx(_Idx_min, _Idx_min_val));
                                unsigned long _H_pos;

                                // Find the smallest horizontal index

                                // CodeQL [SM02313] _H_pos is always initialized: element exists, so _Mask != 0.
                                _BitScanForward(&_H_pos, _Mask);

                                // Extract its vertical index
                                const auto _V_pos = _Traits::_Get_v_pos(_Cur_idx_min, _H_pos);
                                // Finally, compute the pointer
                                _Res._Min = _Base + static_cast<size_t>(_V_pos) * _Traits::_Vec_size + _H_pos;
                            }
                        }

                        if constexpr ((_Mode & _Mode_max) != 0) {
                            // Vector populated by the largest element
                            const auto _H_max     = _Traits::_H_max(_Cur_vals_max);
                            const auto _H_max_val = _Traits::_Get_any(_H_max); // Get any element of it

                            if (_Mode == _Mode_both && _Cur_max_val <= _H_max_val
                                || _Mode == _Mode_max && _Cur_max_val < _H_max_val) {
                                // max_element: current horizontal max is greater than the old, update max
                                // minmax_element: current horizontal max is not less than the old, update max
                                _Cur_max_val = _H_max_val;

                                // Mask of all elems eq to max
                                const auto _Eq_mask = _Traits::_Cmp_eq(_H_max, _Cur_vals_max);
                                unsigned long _Mask = _Traits::_Mask(_Traits::_Mask_cast(_Eq_mask));

                                unsigned long _H_pos;
                                if constexpr (_Mode == _Mode_both) {
                                    // Looking for the last occurrence of maximum
                                    // Indices of maximum elements or zero if none
                                    const auto _Idx_max_val =
                                        _Traits::_Blend(_Traits::_Zero(), _Cur_idx_max, _Traits::_Mask_cast(_Eq_mask));
                                    const auto _Idx_max = _Traits::_H_max_u(_Idx_max_val); // The greatest indices
                                    // Select the greatest vertical indices from the largest element mask
                                    _Mask &= _Traits::_Mask(_Traits::_Cmp_eq_idx(_Idx_max, _Idx_max_val));

                                    // Find the largest horizontal index

                                    // CodeQL [SM02313] _H_pos is always initialized: element exists, so _Mask != 0.
                                    _BitScanReverse(&_H_pos, _Mask);

                                    _H_pos -= sizeof(_Cur_max_val) - 1; // Correct from highest val bit to lowest
                                } else {
                                    // Looking for the first occurrence of maximum
                                    // Indices of maximum elements or the greatest index if none
                                    const auto _Idx_max_val = _Traits::_Blend(
                                        _Traits::_All_ones(), _Cur_idx_max, _Traits::_Mask_cast(_Eq_mask));
                                    const auto _Idx_max = _Traits::_H_min_u(_Idx_max_val); // The smallest indices
                                    // Select the smallest vertical indices from the largest element mask
                                    _Mask &= _Traits::_Mask(_Traits::_Cmp_eq_idx(_Idx_max, _Idx_max_val));

                                    // Find the smallest horizontal index

                                    // CodeQL [SM02313] _H_pos is always initialized: element exists, so _Mask != 0.
                                    _BitScanForward(&_H_pos, _Mask);
                                }

                                // Extract its vertical index
                                const auto _V_pos = _Traits::_Get_v_pos(_Cur_idx_max, _H_pos);
                                // Finally, compute the pointer
                                _Res._Max = _Base + static_cast<size_t>(_V_pos) * _Traits::_Vec_size + _H_pos;
                            }
                        }
                        // Horizontal part done, results are saved, now need to see if there is another portion.

                        if constexpr (_Traits::_Has_portion_max) {
                            // Either the last portion or wrapping point reached, need to determine
                            _Portion_byte_size = _Byte_length(_First, _Last) & ~_Traits::_Vec_mask;
                            if (_Portion_byte_size == 0) {
                                break; // That was the last portion
                            }
                            // Start next portion to handle the wrapping indices. Assume _Cur_idx is zero
                            constexpr size_t _Max_portion_byte_size = _Traits::_Portion_max * _Traits::_Vec_size;
                            if (_Portion_byte_size > _Max_portion_byte_size) {
                                _Portion_byte_size = _Max_portion_byte_size;
                            }

                            _Advance_bytes(_Stop_at, _Portion_byte_size);
                            // Indices will be relative to the new base
                            _Base = static_cast<const char*>(_First);
                            // Load values and if unsigned adjust them to be signed (for signed vector comparisons)
                            _Cur_vals = _Traits::_Sign_correction(_Traits::_Load(_First), _Sign);

                            if constexpr ((_Mode & _Mode_min) != 0) {
                                _Cur_vals_min = _Cur_vals;
                                _Cur_idx_min  = _Traits::_Zero();
                            }

                            if constexpr ((_Mode & _Mode_max) != 0) {
                                _Cur_vals_max = _Cur_vals;
                                _Cur_idx_max  = _Traits::_Zero();
                            }
                        } else {
                            break; // No wrapping, so it was the only portion
                        }
                    }
                }

                _Traits::_Exit_vectorized(); // TRANSITION, DevCom-10331414
#endif // ^^^ !defined(_M_ARM64EC) ^^^
            }

            if constexpr (_Traits::_Is_floating) {
                if constexpr (_Mode == _Mode_min) {
                    return _Min_tail(_First, _Last, _Res._Min, _Cur_min_val);
                } else if constexpr (_Mode == _Mode_max) {
                    return _Max_tail(_First, _Last, _Res._Max, _Cur_max_val);
                } else {
                    return _Both_tail(_First, _Last, _Res, _Cur_min_val, _Cur_max_val);
                }
            } else {
                using _STy = _Traits::_Signed_t;
                using _UTy = _Traits::_Unsigned_t;

                constexpr _UTy _Correction = _UTy{1} << (sizeof(_UTy) * 8 - 1);

                if constexpr (_Mode == _Mode_min) {
                    if (_Sign) {
                        return _Min_tail(_First, _Last, _Res._Min, static_cast<_STy>(_Cur_min_val));
                    } else {
                        return _Min_tail(_First, _Last, _Res._Min, static_cast<_UTy>(_Cur_min_val + _Correction));
                    }
                } else if constexpr (_Mode == _Mode_max) {
                    if (_Sign) {
                        return _Max_tail(_First, _Last, _Res._Max, static_cast<_STy>(_Cur_max_val));
                    } else {
                        return _Max_tail(_First, _Last, _Res._Max, static_cast<_UTy>(_Cur_max_val + _Correction));
                    }
                } else {
                    if (_Sign) {
                        return _Both_tail(
                            _First, _Last, _Res, static_cast<_STy>(_Cur_min_val), static_cast<_STy>(_Cur_max_val));
                    } else {
                        return _Both_tail(_First, _Last, _Res, static_cast<_UTy>(_Cur_min_val + _Correction),
                            static_cast<_UTy>(_Cur_max_val + _Correction));
                    }
                }
            }
        }

        template <_Min_max_mode _Mode, class _Traits>
        auto __stdcall _Minmax_element_disp(
            const void* const _First, const void* const _Last, const bool _Sign) noexcept {
#ifndef _M_ARM64EC
            if (_Byte_length(_First, _Last) >= 32 && _Use_avx2()) {
                return _Minmax_element_impl<_Mode, typename _Traits::_Avx>(_First, _Last, _Sign);
            }

            if (_Byte_length(_First, _Last) >= 16 && _Use_sse42()) {
                return _Minmax_element_impl<_Mode, typename _Traits::_Sse>(_First, _Last, _Sign);
            }
#endif // ^^^ !defined(_M_ARM64EC) ^^^
            return _Minmax_element_impl<_Mode, typename _Traits::_Scalar>(_First, _Last, _Sign);
        }

        template <_Min_max_mode _Mode, class _Traits, bool _Sign>
        auto _Minmax_impl(const void* _First, const void* const _Last) noexcept {
            using _Ty = std::conditional_t<_Sign, typename _Traits::_Signed_t, typename _Traits::_Unsigned_t>;

            _Ty _Cur_min_val; // initialized in both of the branches below
            _Ty _Cur_max_val; // initialized in both of the branches below

            if constexpr (_Traits::_Vectorized) {
#ifdef _M_ARM64EC
                static_assert(false, "No vectorization for _M_ARM64EC yet");
#else // ^^^ defined(_M_ARM64EC) / !defined(_M_ARM64EC) vvv
                const size_t _Total_size_bytes = _Byte_length(_First, _Last);
                const size_t _Vec_byte_size    = _Total_size_bytes & ~_Traits::_Vec_mask;

                const void* _Stop_at = _First;
                _Advance_bytes(_Stop_at, _Vec_byte_size);

                auto _Cur_vals = _Traits::_Load(_First);

                // We don't have unsigned 64-bit stuff, so we'll use sign correction just for that case
                constexpr bool _Sign_correction = sizeof(_Ty) == 8 && !_Sign;

                if constexpr (_Sign_correction) {
                    _Cur_vals = _Traits::_Sign_correction(_Cur_vals, false);
                }

                auto _Cur_vals_min = _Cur_vals; // vector of vertical minimum values
                auto _Cur_vals_max = _Cur_vals; // vector of vertical maximum values

                const auto _Update_min_max = [&](const auto _Cur_vals) noexcept {
                    if constexpr ((_Mode & _Mode_min) != 0) {
                        if constexpr (_Sign || _Sign_correction) {
                            _Cur_vals_min = _Traits::_Min(_Cur_vals_min, _Cur_vals); // Update the current minimum
                        } else {
                            _Cur_vals_min = _Traits::_Min_u(_Cur_vals_min, _Cur_vals); // Update the current minimum
                        }
                    }

                    if constexpr ((_Mode & _Mode_max) != 0) {
                        if constexpr (_Sign || _Sign_correction) {
                            _Cur_vals_max = _Traits::_Max(_Cur_vals_max, _Cur_vals); // Update the current maximum
                        } else {
                            _Cur_vals_max = _Traits::_Max_u(_Cur_vals_max, _Cur_vals); // Update the current maximum
                        }
                    }
                };

                for (;;) {
                    _Advance_bytes(_First, _Traits::_Vec_size);

                    if (_First != _Stop_at) {
                        // This is the main part, finding vertical minimum/maximum

                        _Cur_vals = _Traits::_Load(_First);

                        if constexpr (_Sign_correction) {
                            _Cur_vals = _Traits::_Sign_correction(_Cur_vals, false);
                        }

                        _Update_min_max(_Cur_vals);
                    } else {
                        if constexpr (_Traits::_Tail_mask != 0) {
                            const size_t _Tail_byte_size = _Total_size_bytes & _Traits::_Tail_mask;
                            if (_Tail_byte_size != 0) {
                                const auto _Tail_mask = _Avx2_tail_mask_32(_Tail_byte_size);
                                auto _Tail_vals       = _Traits::_Load_mask(_First, _Tail_mask);

                                if constexpr (_Sign_correction) {
                                    _Tail_vals = _Traits::_Sign_correction(_Tail_vals, false);
                                }

                                _Tail_vals = _Traits::_Blendval(_Cur_vals, _Tail_vals, _Tail_mask);

                                _Update_min_max(_Tail_vals);

                                _Advance_bytes(_First, _Tail_byte_size);
                            }
                        }

                        // Reached end. Compute horizontal min and/or max.

                        if constexpr ((_Mode & _Mode_min) != 0) {
                            if constexpr (_Sign || _Sign_correction) {
                                // Vector populated by the smallest element
                                const auto _H_min = _Traits::_H_min(_Cur_vals_min);
                                _Cur_min_val      = _Traits::_Get_any(_H_min); // Get any element of it
                            } else {
                                // Vector populated by the smallest element
                                const auto _H_min = _Traits::_H_min_u(_Cur_vals_min);
                                _Cur_min_val      = _Traits::_Get_any(_H_min); // Get any element of it
                            }
                        }

                        if constexpr ((_Mode & _Mode_max) != 0) {
                            if constexpr (_Sign || _Sign_correction) {
                                // Vector populated by the largest element
                                const auto _H_max = _Traits::_H_max(_Cur_vals_max);
                                _Cur_max_val      = _Traits::_Get_any(_H_max); // Get any element of it
                            } else {
                                // Vector populated by the largest element
                                const auto _H_max = _Traits::_H_max_u(_Cur_vals_max);
                                _Cur_max_val      = _Traits::_Get_any(_H_max); // Get any element of it
                            }
                        }

                        if constexpr (_Sign_correction) {
                            constexpr _Ty _Correction = _Ty{1} << (sizeof(_Ty) * 8 - 1);

                            if constexpr ((_Mode & _Mode_min) != 0) {
                                _Cur_min_val += _Correction;
                            }

                            if constexpr ((_Mode & _Mode_max) != 0) {
                                _Cur_max_val += _Correction;
                            }
                        }

                        break;
                    }
                }

                _Traits::_Exit_vectorized(); // TRANSITION, DevCom-10331414
#endif // ^^^ !defined(_M_ARM64EC) ^^^
            } else {
                _Cur_min_val = *reinterpret_cast<const _Ty*>(_First);
                _Cur_max_val = *reinterpret_cast<const _Ty*>(_First);

                _Advance_bytes(_First, sizeof(_Ty));
            }

            for (auto _Ptr = static_cast<const _Ty*>(_First); _Ptr != _Last; ++_Ptr) {
                if constexpr ((_Mode & _Mode_min) != 0) {
                    if (*_Ptr < _Cur_min_val) {
                        _Cur_min_val = *_Ptr;
                    }
                }

                if constexpr ((_Mode & _Mode_max) != 0) {
                    if (_Cur_max_val < *_Ptr) {
                        _Cur_max_val = *_Ptr;
                    }
                }

                // _Mode_both could have been handled separately with 'else'.
                // We have _Cur_min_val / _Cur_max_val initialized by processing at least one element,
                // so the 'else' would be correct here.
                // But still separate 'if' statements promote branchless codegen.
            }

            if constexpr (_Mode == _Mode_min) {
                return _Cur_min_val;
            } else if constexpr (_Mode == _Mode_max) {
                return _Cur_max_val;
            } else {
                using _Rx = std::conditional_t<_Sign, typename _Traits::_Minmax_i_t, typename _Traits::_Minmax_u_t>;
                return _Rx{_Cur_min_val, _Cur_max_val};
            }
        }

#ifndef _M_ARM64EC
        // TRANSITION, DevCom-10767462
        template <_Min_max_mode _Mode, class _Traits, bool _Sign>
        auto _Minmax_impl_wrap(const void* const _First, const void* const _Last) noexcept {
            auto _Rx = _Minmax_impl<_Mode, _Traits, _Sign>(_First, _Last);
            _mm256_zeroupper();
            return _Rx;
        }
#endif // ^^^ !defined(_M_ARM64EC) ^^^

        template <_Min_max_mode _Mode, class _Traits, bool _Sign>
        auto __stdcall _Minmax_disp(const void* const _First, const void* const _Last) noexcept {
#ifndef _M_ARM64EC
            if (_Byte_length(_First, _Last) >= 32 && _Use_avx2()) {
                if constexpr (_Traits::_Avx::_Is_floating) {
                    return _Minmax_impl_wrap<_Mode, typename _Traits::_Avx, _Sign>(_First, _Last);
                } else {
                    return _Minmax_impl<_Mode, typename _Traits::_Avx, _Sign>(_First, _Last);
                }
            }

            if (_Byte_length(_First, _Last) >= 16 && _Use_sse42()) {
                return _Minmax_impl<_Mode, typename _Traits::_Sse, _Sign>(_First, _Last);
            }
#endif // ^^^ !defined(_M_ARM64EC) ^^^
            return _Minmax_impl<_Mode, typename _Traits::_Scalar, _Sign>(_First, _Last);
        }

        template <class _Traits, class _Ty>
        const void* _Is_sorted_until_impl(const void* _First, const void* const _Last, const bool _Greater) noexcept {
            const ptrdiff_t _Left_off  = 0 - static_cast<ptrdiff_t>(_Greater);
            const ptrdiff_t _Right_off = static_cast<ptrdiff_t>(_Greater) - 1;

            if constexpr (_Traits::_Vectorized) {
#ifdef _M_ARM64EC
                static_assert(false, "No vectorization for _M_ARM64EC yet");
#else // ^^^ defined(_M_ARM64EC) / !defined(_M_ARM64EC) vvv
                [[maybe_unused]] typename _Traits::_Guard _Guard; // TRANSITION, DevCom-10331414

                constexpr bool _Sign_cor = static_cast<_Ty>(-1) > _Ty{0};

                const size_t _Total_size_bytes = _Byte_length(_First, _Last);
                const size_t _Vec_byte_size    = _Total_size_bytes & ~_Traits::_Vec_mask;

                const void* _Stop_at = _First;
                _Advance_bytes(_Stop_at, _Vec_byte_size);

                do {
                    auto _Left  = _Traits::_Load(static_cast<const _Ty*>(_First) + _Left_off);
                    auto _Right = _Traits::_Load(static_cast<const _Ty*>(_First) + _Right_off);

                    if constexpr (_Sign_cor) {
                        _Left  = _Traits::_Sign_correction(_Left, false);
                        _Right = _Traits::_Sign_correction(_Right, false);
                    }

                    const auto _Is_less = _Traits::_Cmp_gt(_Right, _Left);
                    unsigned long _Mask = _Traits::_Mask(_Traits::_Mask_cast(_Is_less));

                    if (_Mask != 0) {
                        unsigned long _H_pos;

                        // CodeQL [SM02313] _H_pos is always initialized: we just tested `if (_Mask != 0)`.
                        _BitScanForward(&_H_pos, _Mask);
                        _Advance_bytes(_First, _H_pos);
                        return _First;
                    }

                    _Advance_bytes(_First, _Traits::_Vec_size);
                } while (_First != _Stop_at);

                if constexpr (_Traits::_Tail_mask != 0) {
                    const size_t _Tail_byte_size = _Total_size_bytes & _Traits::_Tail_mask;
                    if (_Tail_byte_size != 0) {
                        const auto _Tail_mask = _Avx2_tail_mask_32(_Tail_byte_size);

                        auto _Left  = _Traits::_Load_mask(static_cast<const _Ty*>(_First) + _Left_off, _Tail_mask);
                        auto _Right = _Traits::_Load_mask(static_cast<const _Ty*>(_First) + _Right_off, _Tail_mask);

                        if constexpr (_Sign_cor) {
                            _Left  = _Traits::_Sign_correction(_Left, false);
                            _Right = _Traits::_Sign_correction(_Right, false);
                        }

                        const auto _Is_less = _Traits::_Cmp_gt(_Right, _Left);
                        unsigned long _Mask =
                            _Traits::_Mask(_mm256_and_si256(_Traits::_Mask_cast(_Is_less), _Tail_mask));

                        if (_Mask != 0) {
                            unsigned long _H_pos;

                            // CodeQL [SM02313] _H_pos is always initialized: we just tested `if (_Mask != 0)`.
                            _BitScanForward(&_H_pos, _Mask);
                            _Advance_bytes(_First, _H_pos);
                            return _First;
                        }

                        _Advance_bytes(_First, _Tail_byte_size);
                    }
                }
#endif // ^^^ !defined(_M_ARM64EC) ^^^
            }

            if constexpr ((_Traits::_Tail_mask & sizeof(_Ty)) != sizeof(_Ty)) {
                for (const _Ty* _Ptr = static_cast<const _Ty*>(_First); _Ptr != _Last; ++_Ptr) {
                    if (_Ptr[_Left_off] < _Ptr[_Right_off]) {
                        return _Ptr;
                    }
                }
            }

            return _Last;
        }

        template <class _Traits, class _Ty>
        const void* __stdcall _Is_sorted_until_disp(
            const void* _First, const void* const _Last, const bool _Greater) noexcept {
            if (_First == _Last) {
                return _First;
            }

            _Advance_bytes(_First, sizeof(_Ty));

#ifndef _M_ARM64EC
            if (_Byte_length(_First, _Last) >= 32 && _Use_avx2()) {
                return _Is_sorted_until_impl<typename _Traits::_Avx, _Ty>(_First, _Last, _Greater);
            }

            if (_Byte_length(_First, _Last) >= 16 && _Use_sse42()) {
                return _Is_sorted_until_impl<typename _Traits::_Sse, _Ty>(_First, _Last, _Greater);
            }
#endif // ^^^ !defined(_M_ARM64EC) ^^^
            return _Is_sorted_until_impl<typename _Traits::_Scalar, _Ty>(_First, _Last, _Greater);
        }
    } // namespace _Sorting
} // unnamed namespace

extern "C" {

const void* __stdcall __std_min_element_1(
    const void* const _First, const void* const _Last, const bool _Signed) noexcept {
    return _Sorting::_Minmax_element_disp<_Sorting::_Mode_min, _Sorting::_Traits_1>(_First, _Last, _Signed);
}

const void* __stdcall __std_min_element_2(
    const void* const _First, const void* const _Last, const bool _Signed) noexcept {
    return _Sorting::_Minmax_element_disp<_Sorting::_Mode_min, _Sorting::_Traits_2>(_First, _Last, _Signed);
}

const void* __stdcall __std_min_element_4(
    const void* const _First, const void* const _Last, const bool _Signed) noexcept {
    return _Sorting::_Minmax_element_disp<_Sorting::_Mode_min, _Sorting::_Traits_4>(_First, _Last, _Signed);
}

const void* __stdcall __std_min_element_8(
    const void* const _First, const void* const _Last, const bool _Signed) noexcept {
    return _Sorting::_Minmax_element_disp<_Sorting::_Mode_min, _Sorting::_Traits_8>(_First, _Last, _Signed);
}

// TRANSITION, ABI: remove unused `bool`
const void* __stdcall __std_min_element_f(const void* const _First, const void* const _Last, bool) noexcept {
    return _Sorting::_Minmax_element_disp<_Sorting::_Mode_min, _Sorting::_Traits_f>(_First, _Last, false);
}

// TRANSITION, ABI: remove unused `bool`
const void* __stdcall __std_min_element_d(const void* const _First, const void* const _Last, bool) noexcept {
    return _Sorting::_Minmax_element_disp<_Sorting::_Mode_min, _Sorting::_Traits_d>(_First, _Last, false);
}

const void* __stdcall __std_max_element_1(
    const void* const _First, const void* const _Last, const bool _Signed) noexcept {
    return _Sorting::_Minmax_element_disp<_Sorting::_Mode_max, _Sorting::_Traits_1>(_First, _Last, _Signed);
}

const void* __stdcall __std_max_element_2(
    const void* const _First, const void* const _Last, const bool _Signed) noexcept {
    return _Sorting::_Minmax_element_disp<_Sorting::_Mode_max, _Sorting::_Traits_2>(_First, _Last, _Signed);
}

const void* __stdcall __std_max_element_4(
    const void* const _First, const void* const _Last, const bool _Signed) noexcept {
    return _Sorting::_Minmax_element_disp<_Sorting::_Mode_max, _Sorting::_Traits_4>(_First, _Last, _Signed);
}

const void* __stdcall __std_max_element_8(
    const void* const _First, const void* const _Last, const bool _Signed) noexcept {
    return _Sorting::_Minmax_element_disp<_Sorting::_Mode_max, _Sorting::_Traits_8>(_First, _Last, _Signed);
}

// TRANSITION, ABI: remove unused `bool`
const void* __stdcall __std_max_element_f(const void* const _First, const void* const _Last, bool) noexcept {
    return _Sorting::_Minmax_element_disp<_Sorting::_Mode_max, _Sorting::_Traits_f>(_First, _Last, false);
}

// TRANSITION, ABI: remove unused `bool`
const void* __stdcall __std_max_element_d(const void* const _First, const void* const _Last, bool) noexcept {
    return _Sorting::_Minmax_element_disp<_Sorting::_Mode_max, _Sorting::_Traits_d>(_First, _Last, false);
}

_Min_max_element_t __stdcall __std_minmax_element_1(
    const void* const _First, const void* const _Last, const bool _Signed) noexcept {
    return _Sorting::_Minmax_element_disp<_Sorting::_Mode_both, _Sorting::_Traits_1>(_First, _Last, _Signed);
}

_Min_max_element_t __stdcall __std_minmax_element_2(
    const void* const _First, const void* const _Last, const bool _Signed) noexcept {
    return _Sorting::_Minmax_element_disp<_Sorting::_Mode_both, _Sorting::_Traits_2>(_First, _Last, _Signed);
}

_Min_max_element_t __stdcall __std_minmax_element_4(
    const void* const _First, const void* const _Last, const bool _Signed) noexcept {
    return _Sorting::_Minmax_element_disp<_Sorting::_Mode_both, _Sorting::_Traits_4>(_First, _Last, _Signed);
}

_Min_max_element_t __stdcall __std_minmax_element_8(
    const void* const _First, const void* const _Last, const bool _Signed) noexcept {
    return _Sorting::_Minmax_element_disp<_Sorting::_Mode_both, _Sorting::_Traits_8>(_First, _Last, _Signed);
}

// TRANSITION, ABI: remove unused `bool`
_Min_max_element_t __stdcall __std_minmax_element_f(const void* const _First, const void* const _Last, bool) noexcept {
    return _Sorting::_Minmax_element_disp<_Sorting::_Mode_both, _Sorting::_Traits_f>(_First, _Last, false);
}

// TRANSITION, ABI: remove unused `bool`
_Min_max_element_t __stdcall __std_minmax_element_d(const void* const _First, const void* const _Last, bool) noexcept {
    return _Sorting::_Minmax_element_disp<_Sorting::_Mode_both, _Sorting::_Traits_d>(_First, _Last, false);
}

__declspec(noalias) int8_t __stdcall __std_min_1i(const void* const _First, const void* const _Last) noexcept {
    return _Sorting::_Minmax_disp<_Sorting::_Mode_min, _Sorting::_Traits_1, true>(_First, _Last);
}

__declspec(noalias) uint8_t __stdcall __std_min_1u(const void* const _First, const void* const _Last) noexcept {
    return _Sorting::_Minmax_disp<_Sorting::_Mode_min, _Sorting::_Traits_1, false>(_First, _Last);
}

__declspec(noalias) int16_t __stdcall __std_min_2i(const void* const _First, const void* const _Last) noexcept {
    return _Sorting::_Minmax_disp<_Sorting::_Mode_min, _Sorting::_Traits_2, true>(_First, _Last);
}

__declspec(noalias) uint16_t __stdcall __std_min_2u(const void* const _First, const void* const _Last) noexcept {
    return _Sorting::_Minmax_disp<_Sorting::_Mode_min, _Sorting::_Traits_2, false>(_First, _Last);
}

__declspec(noalias) int32_t __stdcall __std_min_4i(const void* const _First, const void* const _Last) noexcept {
    return _Sorting::_Minmax_disp<_Sorting::_Mode_min, _Sorting::_Traits_4, true>(_First, _Last);
}

__declspec(noalias) uint32_t __stdcall __std_min_4u(const void* const _First, const void* const _Last) noexcept {
    return _Sorting::_Minmax_disp<_Sorting::_Mode_min, _Sorting::_Traits_4, false>(_First, _Last);
}

__declspec(noalias) int64_t __stdcall __std_min_8i(const void* const _First, const void* const _Last) noexcept {
    return _Sorting::_Minmax_disp<_Sorting::_Mode_min, _Sorting::_Traits_8, true>(_First, _Last);
}

__declspec(noalias) uint64_t __stdcall __std_min_8u(const void* const _First, const void* const _Last) noexcept {
    return _Sorting::_Minmax_disp<_Sorting::_Mode_min, _Sorting::_Traits_8, false>(_First, _Last);
}

__declspec(noalias) float __stdcall __std_min_f(const void* const _First, const void* const _Last) noexcept {
    return _Sorting::_Minmax_disp<_Sorting::_Mode_min, _Sorting::_Traits_f, true>(_First, _Last);
}

__declspec(noalias) double __stdcall __std_min_d(const void* const _First, const void* const _Last) noexcept {
    return _Sorting::_Minmax_disp<_Sorting::_Mode_min, _Sorting::_Traits_d, true>(_First, _Last);
}

__declspec(noalias) int8_t __stdcall __std_max_1i(const void* const _First, const void* const _Last) noexcept {
    return _Sorting::_Minmax_disp<_Sorting::_Mode_max, _Sorting::_Traits_1, true>(_First, _Last);
}

__declspec(noalias) uint8_t __stdcall __std_max_1u(const void* const _First, const void* const _Last) noexcept {
    return _Sorting::_Minmax_disp<_Sorting::_Mode_max, _Sorting::_Traits_1, false>(_First, _Last);
}

__declspec(noalias) int16_t __stdcall __std_max_2i(const void* const _First, const void* const _Last) noexcept {
    return _Sorting::_Minmax_disp<_Sorting::_Mode_max, _Sorting::_Traits_2, true>(_First, _Last);
}

__declspec(noalias) uint16_t __stdcall __std_max_2u(const void* const _First, const void* const _Last) noexcept {
    return _Sorting::_Minmax_disp<_Sorting::_Mode_max, _Sorting::_Traits_2, false>(_First, _Last);
}

__declspec(noalias) int32_t __stdcall __std_max_4i(const void* const _First, const void* const _Last) noexcept {
    return _Sorting::_Minmax_disp<_Sorting::_Mode_max, _Sorting::_Traits_4, true>(_First, _Last);
}

__declspec(noalias) uint32_t __stdcall __std_max_4u(const void* const _First, const void* const _Last) noexcept {
    return _Sorting::_Minmax_disp<_Sorting::_Mode_max, _Sorting::_Traits_4, false>(_First, _Last);
}

__declspec(noalias) int64_t __stdcall __std_max_8i(const void* const _First, const void* const _Last) noexcept {
    return _Sorting::_Minmax_disp<_Sorting::_Mode_max, _Sorting::_Traits_8, true>(_First, _Last);
}

__declspec(noalias) uint64_t __stdcall __std_max_8u(const void* const _First, const void* const _Last) noexcept {
    return _Sorting::_Minmax_disp<_Sorting::_Mode_max, _Sorting::_Traits_8, false>(_First, _Last);
}

__declspec(noalias) float __stdcall __std_max_f(const void* const _First, const void* const _Last) noexcept {
    return _Sorting::_Minmax_disp<_Sorting::_Mode_max, _Sorting::_Traits_f, true>(_First, _Last);
}

__declspec(noalias) double __stdcall __std_max_d(const void* const _First, const void* const _Last) noexcept {
    return _Sorting::_Minmax_disp<_Sorting::_Mode_max, _Sorting::_Traits_d, true>(_First, _Last);
}

__declspec(noalias) _Min_max_1i __stdcall __std_minmax_1i(const void* const _First, const void* const _Last) noexcept {
    return _Sorting::_Minmax_disp<_Sorting::_Mode_both, _Sorting::_Traits_1, true>(_First, _Last);
}

__declspec(noalias) _Min_max_1u __stdcall __std_minmax_1u(const void* const _First, const void* const _Last) noexcept {
    return _Sorting::_Minmax_disp<_Sorting::_Mode_both, _Sorting::_Traits_1, false>(_First, _Last);
}

__declspec(noalias) _Min_max_2i __stdcall __std_minmax_2i(const void* const _First, const void* const _Last) noexcept {
    return _Sorting::_Minmax_disp<_Sorting::_Mode_both, _Sorting::_Traits_2, true>(_First, _Last);
}

__declspec(noalias) _Min_max_2u __stdcall __std_minmax_2u(const void* const _First, const void* const _Last) noexcept {
    return _Sorting::_Minmax_disp<_Sorting::_Mode_both, _Sorting::_Traits_2, false>(_First, _Last);
}

__declspec(noalias) _Min_max_4i __stdcall __std_minmax_4i(const void* const _First, const void* const _Last) noexcept {
    return _Sorting::_Minmax_disp<_Sorting::_Mode_both, _Sorting::_Traits_4, true>(_First, _Last);
}

__declspec(noalias) _Min_max_4u __stdcall __std_minmax_4u(const void* const _First, const void* const _Last) noexcept {
    return _Sorting::_Minmax_disp<_Sorting::_Mode_both, _Sorting::_Traits_4, false>(_First, _Last);
}

__declspec(noalias) _Min_max_8i __stdcall __std_minmax_8i(const void* const _First, const void* const _Last) noexcept {
    return _Sorting::_Minmax_disp<_Sorting::_Mode_both, _Sorting::_Traits_8, true>(_First, _Last);
}

__declspec(noalias) _Min_max_8u __stdcall __std_minmax_8u(const void* const _First, const void* const _Last) noexcept {
    return _Sorting::_Minmax_disp<_Sorting::_Mode_both, _Sorting::_Traits_8, false>(_First, _Last);
}

__declspec(noalias) _Min_max_f __stdcall __std_minmax_f(const void* const _First, const void* const _Last) noexcept {
    return _Sorting::_Minmax_disp<_Sorting::_Mode_both, _Sorting::_Traits_f, true>(_First, _Last);
}

__declspec(noalias) _Min_max_d __stdcall __std_minmax_d(const void* const _First, const void* const _Last) noexcept {
    return _Sorting::_Minmax_disp<_Sorting::_Mode_both, _Sorting::_Traits_d, true>(_First, _Last);
}

const void* __stdcall __std_is_sorted_until_1i(
    const void* const _First, const void* const _Last, const bool _Greater) noexcept {
    return _Sorting::_Is_sorted_until_disp<_Sorting::_Traits_1, int8_t>(_First, _Last, _Greater);
}

const void* __stdcall __std_is_sorted_until_1u(
    const void* const _First, const void* const _Last, const bool _Greater) noexcept {
    return _Sorting::_Is_sorted_until_disp<_Sorting::_Traits_1, uint8_t>(_First, _Last, _Greater);
}

const void* __stdcall __std_is_sorted_until_2i(
    const void* const _First, const void* const _Last, const bool _Greater) noexcept {
    return _Sorting::_Is_sorted_until_disp<_Sorting::_Traits_2, int16_t>(_First, _Last, _Greater);
}

const void* __stdcall __std_is_sorted_until_2u(
    const void* const _First, const void* const _Last, const bool _Greater) noexcept {
    return _Sorting::_Is_sorted_until_disp<_Sorting::_Traits_2, uint16_t>(_First, _Last, _Greater);
}

const void* __stdcall __std_is_sorted_until_4i(
    const void* const _First, const void* const _Last, const bool _Greater) noexcept {
    return _Sorting::_Is_sorted_until_disp<_Sorting::_Traits_4, int32_t>(_First, _Last, _Greater);
}

const void* __stdcall __std_is_sorted_until_4u(
    const void* const _First, const void* const _Last, const bool _Greater) noexcept {
    return _Sorting::_Is_sorted_until_disp<_Sorting::_Traits_4, uint32_t>(_First, _Last, _Greater);
}

const void* __stdcall __std_is_sorted_until_8i(
    const void* const _First, const void* const _Last, const bool _Greater) noexcept {
    return _Sorting::_Is_sorted_until_disp<_Sorting::_Traits_8, int64_t>(_First, _Last, _Greater);
}

const void* __stdcall __std_is_sorted_until_8u(
    const void* const _First, const void* const _Last, const bool _Greater) noexcept {
    return _Sorting::_Is_sorted_until_disp<_Sorting::_Traits_8, uint64_t>(_First, _Last, _Greater);
}

const void* __stdcall __std_is_sorted_until_f(
    const void* const _First, const void* const _Last, const bool _Greater) noexcept {
    return _Sorting::_Is_sorted_until_disp<_Sorting::_Traits_f, float>(_First, _Last, _Greater);
}

const void* __stdcall __std_is_sorted_until_d(
    const void* const _First, const void* const _Last, const bool _Greater) noexcept {
    return _Sorting::_Is_sorted_until_disp<_Sorting::_Traits_d, double>(_First, _Last, _Greater);
}

} // extern "C"

namespace {
    namespace _Finding {
#ifdef _M_ARM64EC
        using _Find_traits_1 = void;
        using _Find_traits_2 = void;
        using _Find_traits_4 = void;
        using _Find_traits_8 = void;
#else // ^^^ defined(_M_ARM64EC) / !defined(_M_ARM64EC) vvv
        struct _Find_traits_1 {
            static __m256i _Set_avx(const uint8_t _Val) noexcept {
                return _mm256_set1_epi8(_Val);
            }

            static __m128i _Set_sse(const uint8_t _Val) noexcept {
                return _mm_shuffle_epi8(_mm_cvtsi32_si128(_Val), _mm_setzero_si128());
            }

            static __m256i _Cmp_avx(const __m256i _Lhs, const __m256i _Rhs) noexcept {
                return _mm256_cmpeq_epi8(_Lhs, _Rhs);
            }

            static __m128i _Cmp_sse(const __m128i _Lhs, const __m128i _Rhs) noexcept {
                return _mm_cmpeq_epi8(_Lhs, _Rhs);
            }
        };

        struct _Find_traits_2 {
            static __m256i _Set_avx(const uint16_t _Val) noexcept {
                return _mm256_set1_epi16(_Val);
            }

            static __m128i _Set_sse(const uint16_t _Val) noexcept {
                return _mm_set1_epi16(_Val);
            }

            static __m256i _Cmp_avx(const __m256i _Lhs, const __m256i _Rhs) noexcept {
                return _mm256_cmpeq_epi16(_Lhs, _Rhs);
            }

            static __m128i _Cmp_sse(const __m128i _Lhs, const __m128i _Rhs) noexcept {
                return _mm_cmpeq_epi16(_Lhs, _Rhs);
            }
        };

        struct _Find_traits_4 {
            static __m256i _Set_avx(const uint32_t _Val) noexcept {
                return _mm256_set1_epi32(_Val);
            }

            static __m128i _Set_sse(const uint32_t _Val) noexcept {
                return _mm_set1_epi32(_Val);
            }

            static __m256i _Cmp_avx(const __m256i _Lhs, const __m256i _Rhs) noexcept {
                return _mm256_cmpeq_epi32(_Lhs, _Rhs);
            }

            static __m128i _Cmp_sse(const __m128i _Lhs, const __m128i _Rhs) noexcept {
                return _mm_cmpeq_epi32(_Lhs, _Rhs);
            }
        };

        struct _Find_traits_8 {
            static __m256i _Set_avx(const uint64_t _Val) noexcept {
                return _mm256_set1_epi64x(_Val);
            }

            static __m128i _Set_sse(const uint64_t _Val) noexcept {
                return _mm_set1_epi64x(_Val);
            }

            static __m256i _Cmp_avx(const __m256i _Lhs, const __m256i _Rhs) noexcept {
                return _mm256_cmpeq_epi64(_Lhs, _Rhs);
            }

            static __m128i _Cmp_sse(const __m128i _Lhs, const __m128i _Rhs) noexcept {
                return _mm_cmpeq_epi64(_Lhs, _Rhs);
            }
        };
#endif // ^^^ !defined(_M_ARM64EC) ^^^

        // TRANSITION, ABI: used only in functions preserved for binary compatibility
        template <class _Ty>
        const void* __stdcall _Find_unsized_impl(const void* const _First, const _Ty _Val) noexcept {
            auto _Ptr = static_cast<const _Ty*>(_First);
            while (*_Ptr != _Val) {
                ++_Ptr;
            }
            return _Ptr;
        }

        enum class _Predicate { _Equal, _Not_equal };

        // The below functions have exactly the same signature as the extern "C" functions, up to calling convention.
        // This makes sure the template specialization can be fused with the extern "C" function.
        // In optimized builds it avoids an extra call, as these functions are too large to inline.

        template <class _Traits, _Predicate _Pred, class _Ty>
        const void* __stdcall _Find_impl(const void* _First, const void* const _Last, const _Ty _Val) noexcept {
#ifndef _M_ARM64EC
            const size_t _Size_bytes = _Byte_length(_First, _Last);

            if (const size_t _Avx_size = _Size_bytes & ~size_t{0x1F}; _Avx_size != 0 && _Use_avx2()) {
                _Zeroupper_on_exit _Guard; // TRANSITION, DevCom-10331414

                const __m256i _Comparand = _Traits::_Set_avx(_Val);
                const void* _Stop_at     = _First;
                _Advance_bytes(_Stop_at, _Avx_size);

                do {
                    const __m256i _Data = _mm256_loadu_si256(static_cast<const __m256i*>(_First));
                    unsigned int _Bingo = _mm256_movemask_epi8(_Traits::_Cmp_avx(_Data, _Comparand));

                    if constexpr (_Pred == _Predicate::_Not_equal) {
                        _Bingo ^= 0xFFFF'FFFF;
                    }

                    if (_Bingo != 0) {
                        const unsigned long _Offset = _tzcnt_u32(_Bingo);
                        _Advance_bytes(_First, _Offset);
                        return _First;
                    }

                    _Advance_bytes(_First, 32);
                } while (_First != _Stop_at);

                if (const size_t _Avx_tail_size = _Size_bytes & 0x1C; _Avx_tail_size != 0) {
                    const __m256i _Tail_mask = _Avx2_tail_mask_32(_Avx_tail_size);
                    const __m256i _Data      = _mm256_maskload_epi32(static_cast<const int*>(_First), _Tail_mask);
                    const __m256i _Cmp       = _Traits::_Cmp_avx(_Data, _Comparand);
                    unsigned int _Bingo      = _mm256_movemask_epi8(_mm256_and_si256(_Cmp, _Tail_mask));

                    if constexpr (_Pred == _Predicate::_Not_equal) {
                        _Bingo ^= (1 << _Avx_tail_size) - 1;
                    }

                    if (_Bingo != 0) {
                        const unsigned long _Offset = _tzcnt_u32(_Bingo);
                        _Advance_bytes(_First, _Offset);
                        return _First;
                    }

                    _Advance_bytes(_First, _Avx_tail_size);
                }

                if constexpr (sizeof(_Ty) >= 4) {
                    return _First;
                }
            } else if (const size_t _Sse_size = _Size_bytes & ~size_t{0xF}; _Sse_size != 0 && _Use_sse42()) {
                const __m128i _Comparand = _Traits::_Set_sse(_Val);
                const void* _Stop_at     = _First;
                _Advance_bytes(_Stop_at, _Sse_size);

                do {
                    const __m128i _Data = _mm_loadu_si128(static_cast<const __m128i*>(_First));
                    unsigned int _Bingo = _mm_movemask_epi8(_Traits::_Cmp_sse(_Data, _Comparand));

                    if constexpr (_Pred == _Predicate::_Not_equal) {
                        _Bingo ^= 0xFFFF;
                    }

                    if (_Bingo != 0) {
                        unsigned long _Offset;
                        // CodeQL [SM02313] _Offset is always initialized: we just tested `if (_Bingo != 0)`.
                        _BitScanForward(&_Offset, _Bingo);
                        _Advance_bytes(_First, _Offset);
                        return _First;
                    }

                    _Advance_bytes(_First, 16);
                } while (_First != _Stop_at);
            }
#endif // ^^^ !defined(_M_ARM64EC) ^^^
            auto _Ptr = static_cast<const _Ty*>(_First);
            if constexpr (_Pred == _Predicate::_Not_equal) {
                while (_Ptr != _Last && *_Ptr == _Val) {
                    ++_Ptr;
                }
            } else {
                while (_Ptr != _Last && *_Ptr != _Val) {
                    ++_Ptr;
                }
            }
            return _Ptr;
        }

        template <class _Traits, _Predicate _Pred, class _Ty>
        const void* __stdcall _Find_last_impl(const void* _First, const void* _Last, const _Ty _Val) noexcept {
            const void* const _Real_last = _Last;
#ifndef _M_ARM64EC
            const size_t _Size_bytes = _Byte_length(_First, _Last);

            if (const size_t _Avx_size = _Size_bytes & ~size_t{0x1F}; _Avx_size != 0 && _Use_avx2()) {
                _Zeroupper_on_exit _Guard; // TRANSITION, DevCom-10331414

                const __m256i _Comparand = _Traits::_Set_avx(_Val);
                const void* _Stop_at     = _Last;
                _Rewind_bytes(_Stop_at, _Avx_size);

                do {
                    _Rewind_bytes(_Last, 32);
                    const __m256i _Data = _mm256_loadu_si256(static_cast<const __m256i*>(_Last));
                    unsigned int _Bingo = _mm256_movemask_epi8(_Traits::_Cmp_avx(_Data, _Comparand));

                    if constexpr (_Pred == _Predicate::_Not_equal) {
                        _Bingo ^= 0xFFFF'FFFF;
                    }

                    if (_Bingo != 0) {
                        const unsigned long _Offset = _lzcnt_u32(_Bingo);
                        _Advance_bytes(_Last, (31 - _Offset) - (sizeof(_Ty) - 1));
                        return _Last;
                    }
                } while (_Last != _Stop_at);

                if (const size_t _Avx_tail_size = _Size_bytes & 0x1C; _Avx_tail_size != 0) {
                    _Rewind_bytes(_Last, _Avx_tail_size);
                    const __m256i _Tail_mask = _Avx2_tail_mask_32(_Avx_tail_size);
                    const __m256i _Data      = _mm256_maskload_epi32(static_cast<const int*>(_Last), _Tail_mask);
                    const __m256i _Cmp       = _Traits::_Cmp_avx(_Data, _Comparand);
                    unsigned int _Bingo      = _mm256_movemask_epi8(_mm256_and_si256(_Cmp, _Tail_mask));

                    if constexpr (_Pred == _Predicate::_Not_equal) {
                        _Bingo ^= (1 << _Avx_tail_size) - 1;
                    }

                    if (_Bingo != 0) {
                        const unsigned long _Offset = _lzcnt_u32(_Bingo);
                        _Advance_bytes(_Last, (31 - _Offset) - (sizeof(_Ty) - 1));
                        return _Last;
                    }
                }

                if constexpr (sizeof(_Ty) >= 4) {
                    return _Real_last;
                }
            } else if (const size_t _Sse_size = _Size_bytes & ~size_t{0xF}; _Sse_size != 0 && _Use_sse42()) {
                const __m128i _Comparand = _Traits::_Set_sse(_Val);
                const void* _Stop_at     = _Last;
                _Rewind_bytes(_Stop_at, _Sse_size);

                do {
                    _Rewind_bytes(_Last, 16);
                    const __m128i _Data = _mm_loadu_si128(static_cast<const __m128i*>(_Last));
                    unsigned int _Bingo = _mm_movemask_epi8(_Traits::_Cmp_sse(_Data, _Comparand));

                    if constexpr (_Pred == _Predicate::_Not_equal) {
                        _Bingo ^= 0xFFFF;
                    }

                    if (_Bingo != 0) {
                        unsigned long _Offset;
                        // CodeQL [SM02313] _Offset is always initialized: we just tested `if (_Bingo != 0)`.
                        _BitScanReverse(&_Offset, _Bingo);
                        _Advance_bytes(_Last, _Offset - (sizeof(_Ty) - 1));
                        return _Last;
                    }
                } while (_Last != _Stop_at);
            }
#endif // ^^^ !defined(_M_ARM64EC) ^^^
            auto _Ptr = static_cast<const _Ty*>(_Last);
            for (;;) {
                if (_Ptr == _First) {
                    return _Real_last;
                }
                --_Ptr;
                if constexpr (_Pred == _Predicate::_Not_equal) {
                    if (*_Ptr != _Val) {
                        return _Ptr;
                    }
                } else {
                    if (*_Ptr == _Val) {
                        return _Ptr;
                    }
                }
            }
        }

        template <class _Traits, _Predicate _Pred, class _Ty>
        size_t __stdcall _Find_last_pos_impl(
            const void* const _First, const void* const _Last, const _Ty _Val) noexcept {
            const void* const _Result = _Find_last_impl<_Traits, _Pred>(_First, _Last, _Val);
            if (_Result == _Last) {
                return static_cast<size_t>(-1);
            } else {
                return _Byte_length(_First, _Result) / sizeof(_Ty);
            }
        }

        template <class _Traits, class _Ty>
        const void* __stdcall _Adjacent_find_impl(const void* _First, const void* const _Last) noexcept {
            if (_First == _Last) {
                return _Last;
            }

#ifndef _M_ARM64EC
            const size_t _Size_bytes = _Byte_length(_First, _Last) - sizeof(_Ty);

            if (const size_t _Avx_size = _Size_bytes & ~size_t{0x1F}; _Avx_size != 0 && _Use_avx2()) {
                _Zeroupper_on_exit _Guard; // TRANSITION, DevCom-10331414

                const void* _Stop_at = _First;
                _Advance_bytes(_Stop_at, _Avx_size);

                do {
                    const void* _Next = _First;
                    _Advance_bytes(_Next, sizeof(_Ty));

                    const __m256i _Data       = _mm256_loadu_si256(static_cast<const __m256i*>(_First));
                    const __m256i _Comparand  = _mm256_loadu_si256(static_cast<const __m256i*>(_Next));
                    const unsigned int _Bingo = _mm256_movemask_epi8(_Traits::_Cmp_avx(_Data, _Comparand));

                    if (_Bingo != 0) {
                        const unsigned long _Offset = _tzcnt_u32(_Bingo);
                        _Advance_bytes(_First, _Offset);
                        return _First;
                    }

                    _Advance_bytes(_First, 32);
                } while (_First != _Stop_at);

                if (const size_t _Avx_tail_size = _Size_bytes & 0x1C; _Avx_tail_size != 0) {
                    const void* _Next = _First;
                    _Advance_bytes(_Next, sizeof(_Ty));

                    const __m256i _Tail_mask  = _Avx2_tail_mask_32(_Avx_tail_size);
                    const __m256i _Data       = _mm256_maskload_epi32(static_cast<const int*>(_First), _Tail_mask);
                    const __m256i _Comparand  = _mm256_maskload_epi32(static_cast<const int*>(_Next), _Tail_mask);
                    const __m256i _Cmp        = _Traits::_Cmp_avx(_Data, _Comparand);
                    const unsigned int _Bingo = _mm256_movemask_epi8(_mm256_and_si256(_Cmp, _Tail_mask));

                    if (_Bingo != 0) {
                        const unsigned long _Offset = _tzcnt_u32(_Bingo);
                        _Advance_bytes(_First, _Offset);
                        return _First;
                    }

                    _Advance_bytes(_First, _Avx_tail_size);
                }

                if constexpr (sizeof(_Ty) >= 4) {
                    return _Last;
                }
            } else if (const size_t _Sse_size = _Size_bytes & ~size_t{0xF}; _Sse_size != 0 && _Use_sse42()) {
                const void* _Stop_at = _First;
                _Advance_bytes(_Stop_at, _Sse_size);

                do {
                    const void* _Next = _First;
                    _Advance_bytes(_Next, sizeof(_Ty));

                    const __m128i _Data       = _mm_loadu_si128(static_cast<const __m128i*>(_First));
                    const __m128i _Comparand  = _mm_loadu_si128(static_cast<const __m128i*>(_Next));
                    const unsigned int _Bingo = _mm_movemask_epi8(_Traits::_Cmp_sse(_Data, _Comparand));

                    if (_Bingo != 0) {
                        unsigned long _Offset;
                        // CodeQL [SM02313] _Offset is always initialized: we just tested `if (_Bingo != 0)`.
                        _BitScanForward(&_Offset, _Bingo);
                        _Advance_bytes(_First, _Offset);
                        return _First;
                    }

                    _Advance_bytes(_First, 16);
                } while (_First != _Stop_at);
            }
#endif // ^^^ !defined(_M_ARM64EC) ^^^

            auto _Ptr  = static_cast<const _Ty*>(_First);
            auto _Next = _Ptr + 1;
            for (; _Next != _Last; ++_Ptr, ++_Next) {
                if (*_Ptr == *_Next) {
                    return _Ptr;
                }
            }

            return _Last;
        }

        template <class _Traits, class _Ty>
        const void* __stdcall _Search_n_impl(
            const void* _First, const void* const _Last, const size_t _Count, const _Ty _Val) noexcept {
            if (_Count == 0) {
                return _First;
            } else if (_Count == 1) {
                return _Find_impl<_Traits, _Predicate::_Equal>(_First, _Last, _Val);
            }

            auto _Mid1 = static_cast<const _Ty*>(_First);
#ifndef _M_ARM64EC
            const size_t _Length = _Byte_length(_First, _Last);
            if (_Count <= 16 / sizeof(_Ty) && _Length >= 32 && _Use_avx2()) {
                _Zeroupper_on_exit _Guard; // TRANSITION, DevCom-10331414

                const int _Bytes_count = static_cast<int>(_Count * sizeof(_Ty));
                const int _Sh1         = sizeof(_Ty) != 1 ? 0 : (_Bytes_count < 4 ? _Bytes_count - 2 : 2);
                const int _Sh2         = sizeof(_Ty) >= 4 ? 0
                                       : _Bytes_count < 4 ? 0
                                                          : (_Bytes_count < 8 ? _Bytes_count - 4 : 4);
                const int _Sh3         = sizeof(_Ty) == 8 ? 0 : (_Bytes_count < 8 ? 0 : _Bytes_count - 8);

                const __m256i _Comparand = _Traits::_Set_avx(_Val);

                const void* _Stop_at = _First;
                _Advance_bytes(_Stop_at, _Length & ~size_t{0x1F});

                uint32_t _Carry = 0;
                do {
                    const __m256i _Data = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(_First));

                    const __m256i _Cmp   = _Traits::_Cmp_avx(_Comparand, _Data);
                    const uint32_t _Mask = _mm256_movemask_epi8(_Cmp);

                    uint64_t _MskX = uint64_t{_Carry} | (uint64_t{_Mask} << 32);

                    _MskX = (_MskX >> sizeof(_Ty)) & _MskX;

                    if constexpr (sizeof(_Ty) == 1) {
                        _MskX = __ull_rshift(_MskX, _Sh1) & _MskX;
                    }

                    if constexpr (sizeof(_Ty) < 4) {
                        _MskX = __ull_rshift(_MskX, _Sh2) & _MskX;
                    }

                    if constexpr (sizeof(_Ty) < 8) {
                        _MskX = __ull_rshift(_MskX, _Sh3) & _MskX;
                    }

                    if (_MskX != 0) {
#ifdef _M_IX86
                        const uint32_t _MskLow = static_cast<uint32_t>(_MskX);

                        const int _Shift = _MskLow != 0
                                             ? static_cast<int>(_tzcnt_u32(_MskLow)) - 32
                                             : static_cast<int>(_tzcnt_u32(static_cast<uint32_t>(_MskX >> 32)));

#elifdef _M_X64
                        const long long _Shift = static_cast<long long>(_tzcnt_u64(_MskX)) - 32;
#else
#error Unsupported architecture
#endif
                        _Advance_bytes(_First, _Shift);
                        return _First;
                    }

                    _Carry = _Mask;

                    _Advance_bytes(_First, 32);
                } while (_First != _Stop_at);

                if (const size_t _Tail = _Length & 0x1C; _Tail != 0) {
                    const __m256i _Tail_mask = _Avx2_tail_mask_32(_Tail);
                    const __m256i _Data      = _mm256_maskload_epi32(reinterpret_cast<const int*>(_First), _Tail_mask);

                    const __m256i _Cmp   = _Traits::_Cmp_avx(_Comparand, _Data);
                    const uint32_t _Mask = _mm256_movemask_epi8(_mm256_and_si256(_Cmp, _Tail_mask));

                    const uint64_t _Msk_with_carry = uint64_t{_Carry} | (uint64_t{_Mask} << 32);
                    uint64_t _MskX                 = _Msk_with_carry;

                    _MskX = (_MskX >> sizeof(_Ty)) & _MskX;

                    if constexpr (sizeof(_Ty) == 1) {
                        _MskX = __ull_rshift(_MskX, _Sh1) & _MskX;
                    }

                    if constexpr (sizeof(_Ty) < 4) {
                        _MskX = __ull_rshift(_MskX, _Sh2) & _MskX;
                    }

                    if constexpr (sizeof(_Ty) < 8) {
                        _MskX = __ull_rshift(_MskX, _Sh3) & _MskX;
                    }

                    if (_MskX != 0) {
#ifdef _M_IX86
                        const uint32_t _MskLow = static_cast<uint32_t>(_MskX);

                        const int _Shift = _MskLow != 0
                                             ? static_cast<int>(_tzcnt_u32(_MskLow)) - 32
                                             : static_cast<int>(_tzcnt_u32(static_cast<uint32_t>(_MskX >> 32)));

#elifdef _M_X64
                        const long long _Shift = static_cast<long long>(_tzcnt_u64(_MskX)) - 32;
#else
#error Unsupported architecture
#endif
                        _Advance_bytes(_First, _Shift);
                        return _First;
                    }

                    _Carry = static_cast<uint32_t>(__ull_rshift(_Msk_with_carry, static_cast<int>(_Tail)));

                    _Advance_bytes(_First, _Tail);
                }

                _Mid1 = static_cast<const _Ty*>(_First);
                _Rewind_bytes(_First, _lzcnt_u32(~_Carry));
            } else if constexpr (sizeof(_Ty) < 8) {
                if (_Count <= 8 / sizeof(_Ty) && _Length >= 16 && _Use_sse42()) {
                    const int _Bytes_count = static_cast<int>(_Count * sizeof(_Ty));
                    const int _Sh1         = sizeof(_Ty) != 1 ? 0 : (_Bytes_count < 4 ? _Bytes_count - 2 : 2);
                    const int _Sh2         = sizeof(_Ty) >= 4 ? 0
                                           : _Bytes_count < 4 ? 0
                                                              : (_Bytes_count < 8 ? _Bytes_count - 4 : 4);

                    const __m128i _Comparand = _Traits::_Set_sse(_Val);

                    const void* _Stop_at = _First;
                    _Advance_bytes(_Stop_at, _Length & ~size_t{0xF});

                    uint32_t _Carry = 0;
                    do {
                        const __m128i _Data = _mm_loadu_si128(reinterpret_cast<const __m128i*>(_First));

                        const __m128i _Cmp   = _Traits::_Cmp_sse(_Comparand, _Data);
                        const uint32_t _Mask = _mm_movemask_epi8(_Cmp);

                        uint32_t _MskX = _Carry | (_Mask << 16);

                        _MskX = (_MskX >> sizeof(_Ty)) & _MskX;

                        if constexpr (sizeof(_Ty) == 1) {
                            _MskX = (_MskX >> _Sh1) & _MskX;
                        }

                        if constexpr (sizeof(_Ty) < 4) {
                            _MskX = (_MskX >> _Sh2) & _MskX;
                        }

                        if (_MskX != 0) {
                            unsigned long _Pos;
                            // CodeQL [SM02313] _Pos is always initialized: _MskX != 0 was checked right above.
                            _BitScanForward(&_Pos, _MskX);
                            _Advance_bytes(_First, static_cast<ptrdiff_t>(_Pos) - 16);
                            return _First;
                        }

                        _Carry = _Mask;

                        _Advance_bytes(_First, 16);
                    } while (_First != _Stop_at);

                    _Mid1 = static_cast<const _Ty*>(_First);

                    unsigned long _Carry_pos;
                    // Here, _Carry can't be 0xFFFF, because that would have been a match. Therefore:
                    // CodeQL [SM02313] _Carry_pos is always initialized: `(_Carry ^ 0xFFFF) != 0` is always true.
                    _BitScanReverse(&_Carry_pos, _Carry ^ 0xFFFF);
                    _Rewind_bytes(_First, 15 - static_cast<ptrdiff_t>(_Carry_pos));
                }
            }
#endif // ^^^ !defined(_M_ARM64EC) ^^^
            auto _Match_start    = static_cast<const _Ty*>(_First);
            const auto _Last_ptr = static_cast<const _Ty*>(_Last);

            if (static_cast<size_t>(_Last_ptr - _Match_start) < _Count) {
                return _Last_ptr;
            }

            auto _Match_end = _Match_start + _Count;
            auto _Mid2      = _Match_end;
            for (;;) {
                // Invariants: _Match_end - _Match_start == _Count, [_Match_start, _Mid1) and [_Mid2, _Match_end) match
                // _Val:
                //
                // _Match_start  _Mid1    _Mid2    _Match_end
                // |=============|????????|========|??????????...

                --_Mid2;
                if (*_Mid2 == _Val) { // match;
                    if (_Mid1 == _Mid2) { // [_Mid1, _Mid2) is empty, so [_Match_start, _Match_end) all match
                        return _Match_start;
                    }
                } else { // mismatch; skip past it
                    _Match_start = _Mid2 + 1;

                    if (static_cast<size_t>(_Last_ptr - _Match_start) < _Count) { // not enough space left
                        return _Last_ptr;
                    }

                    _Mid1      = _Match_end;
                    _Match_end = _Match_start + _Count;
                    _Mid2      = _Match_end;
                }
            }
        }
    } // namespace _Finding
} // unnamed namespace

extern "C" {

// TRANSITION, ABI: preserved for binary compatibility
const void* __stdcall __std_find_trivial_unsized_1(const void* const _First, const uint8_t _Val) noexcept {
    // C23 7.27.5.2 "The memchr generic function"/2 says "The implementation shall behave as if
    // it reads the characters sequentially and stops as soon as a matching character is found."
    return memchr(_First, _Val, SIZE_MAX);
}

// TRANSITION, ABI: preserved for binary compatibility
const void* __stdcall __std_find_trivial_unsized_2(const void* const _First, const uint16_t _Val) noexcept {
    // C23 7.27.5.2 "The memchr generic function"/2 says "The implementation shall behave as if
    // it reads the characters sequentially and stops as soon as a matching character is found."
    // C23 7.32.4.6.9 "The wmemchr generic function"/2 lacks such wording,
    // so we don't use wmemchr(), avoiding issues with unreachable_sentinel_t.
    return _Finding::_Find_unsized_impl(_First, _Val);
}

// TRANSITION, ABI: preserved for binary compatibility
const void* __stdcall __std_find_trivial_unsized_4(const void* const _First, const uint32_t _Val) noexcept {
    return _Finding::_Find_unsized_impl(_First, _Val);
}

// TRANSITION, ABI: preserved for binary compatibility
const void* __stdcall __std_find_trivial_unsized_8(const void* const _First, const uint64_t _Val) noexcept {
    return _Finding::_Find_unsized_impl(_First, _Val);
}

const void* __stdcall __std_find_trivial_1(
    const void* const _First, const void* const _Last, const uint8_t _Val) noexcept {
#ifdef _M_ARM64EC
    return memchr(_First, _Val, _Byte_length(_First, _Last));
#else
    return _Finding::_Find_impl<_Finding::_Find_traits_1, _Finding::_Predicate::_Equal>(_First, _Last, _Val);
#endif
}

const void* __stdcall __std_find_trivial_2(
    const void* const _First, const void* const _Last, const uint16_t _Val) noexcept {
#ifdef _M_ARM64EC
    return wmemchr(static_cast<const wchar_t*>(_First), _Val, _Byte_length(_First, _Last) / sizeof(wchar_t));
#else
    return _Finding::_Find_impl<_Finding::_Find_traits_2, _Finding::_Predicate::_Equal>(_First, _Last, _Val);
#endif
}

const void* __stdcall __std_find_trivial_4(
    const void* const _First, const void* const _Last, const uint32_t _Val) noexcept {
    return _Finding::_Find_impl<_Finding::_Find_traits_4, _Finding::_Predicate::_Equal>(_First, _Last, _Val);
}

const void* __stdcall __std_find_trivial_8(
    const void* const _First, const void* const _Last, const uint64_t _Val) noexcept {
    return _Finding::_Find_impl<_Finding::_Find_traits_8, _Finding::_Predicate::_Equal>(_First, _Last, _Val);
}

const void* __stdcall __std_find_last_trivial_1(
    const void* const _First, const void* const _Last, const uint8_t _Val) noexcept {
    return _Finding::_Find_last_impl<_Finding::_Find_traits_1, _Finding::_Predicate::_Equal>(_First, _Last, _Val);
}

const void* __stdcall __std_find_last_trivial_2(
    const void* const _First, const void* const _Last, const uint16_t _Val) noexcept {
    return _Finding::_Find_last_impl<_Finding::_Find_traits_2, _Finding::_Predicate::_Equal>(_First, _Last, _Val);
}

const void* __stdcall __std_find_last_trivial_4(
    const void* const _First, const void* const _Last, const uint32_t _Val) noexcept {
    return _Finding::_Find_last_impl<_Finding::_Find_traits_4, _Finding::_Predicate::_Equal>(_First, _Last, _Val);
}

const void* __stdcall __std_find_last_trivial_8(
    const void* const _First, const void* const _Last, const uint64_t _Val) noexcept {
    return _Finding::_Find_last_impl<_Finding::_Find_traits_8, _Finding::_Predicate::_Equal>(_First, _Last, _Val);
}

const void* __stdcall __std_find_not_ch_1(
    const void* const _First, const void* const _Last, const uint8_t _Val) noexcept {
    return _Finding::_Find_impl<_Finding::_Find_traits_1, _Finding::_Predicate::_Not_equal>(_First, _Last, _Val);
}

const void* __stdcall __std_find_not_ch_2(
    const void* const _First, const void* const _Last, const uint16_t _Val) noexcept {
    return _Finding::_Find_impl<_Finding::_Find_traits_2, _Finding::_Predicate::_Not_equal>(_First, _Last, _Val);
}

const void* __stdcall __std_find_not_ch_4(
    const void* const _First, const void* const _Last, const uint32_t _Val) noexcept {
    return _Finding::_Find_impl<_Finding::_Find_traits_4, _Finding::_Predicate::_Not_equal>(_First, _Last, _Val);
}

const void* __stdcall __std_find_not_ch_8(
    const void* const _First, const void* const _Last, const uint64_t _Val) noexcept {
    return _Finding::_Find_impl<_Finding::_Find_traits_8, _Finding::_Predicate::_Not_equal>(_First, _Last, _Val);
}

__declspec(noalias) size_t __stdcall __std_find_last_not_ch_pos_1(
    const void* const _First, const void* const _Last, const uint8_t _Val) noexcept {
    return _Finding::_Find_last_pos_impl<_Finding::_Find_traits_1, _Finding::_Predicate::_Not_equal>(
        _First, _Last, _Val);
}

__declspec(noalias) size_t __stdcall __std_find_last_not_ch_pos_2(
    const void* const _First, const void* const _Last, const uint16_t _Val) noexcept {
    return _Finding::_Find_last_pos_impl<_Finding::_Find_traits_2, _Finding::_Predicate::_Not_equal>(
        _First, _Last, _Val);
}

__declspec(noalias) size_t __stdcall __std_find_last_not_ch_pos_4(
    const void* const _First, const void* const _Last, const uint32_t _Val) noexcept {
    return _Finding::_Find_last_pos_impl<_Finding::_Find_traits_4, _Finding::_Predicate::_Not_equal>(
        _First, _Last, _Val);
}

__declspec(noalias) size_t __stdcall __std_find_last_not_ch_pos_8(
    const void* const _First, const void* const _Last, const uint64_t _Val) noexcept {
    return _Finding::_Find_last_pos_impl<_Finding::_Find_traits_8, _Finding::_Predicate::_Not_equal>(
        _First, _Last, _Val);
}

const void* __stdcall __std_adjacent_find_1(const void* const _First, const void* const _Last) noexcept {
    return _Finding::_Adjacent_find_impl<_Finding::_Find_traits_1, uint8_t>(_First, _Last);
}

const void* __stdcall __std_adjacent_find_2(const void* const _First, const void* const _Last) noexcept {
    return _Finding::_Adjacent_find_impl<_Finding::_Find_traits_2, uint16_t>(_First, _Last);
}

const void* __stdcall __std_adjacent_find_4(const void* const _First, const void* const _Last) noexcept {
    return _Finding::_Adjacent_find_impl<_Finding::_Find_traits_4, uint32_t>(_First, _Last);
}

const void* __stdcall __std_adjacent_find_8(const void* const _First, const void* const _Last) noexcept {
    return _Finding::_Adjacent_find_impl<_Finding::_Find_traits_8, uint64_t>(_First, _Last);
}

const void* __stdcall __std_search_n_1(
    const void* const _First, const void* const _Last, const size_t _Count, const uint8_t _Value) noexcept {
    return _Finding::_Search_n_impl<_Finding::_Find_traits_1>(_First, _Last, _Count, _Value);
}

const void* __stdcall __std_search_n_2(
    const void* const _First, const void* const _Last, const size_t _Count, const uint16_t _Value) noexcept {
    return _Finding::_Search_n_impl<_Finding::_Find_traits_2>(_First, _Last, _Count, _Value);
}

const void* __stdcall __std_search_n_4(
    const void* const _First, const void* const _Last, const size_t _Count, const uint32_t _Value) noexcept {
    return _Finding::_Search_n_impl<_Finding::_Find_traits_4>(_First, _Last, _Count, _Value);
}

const void* __stdcall __std_search_n_8(
    const void* const _First, const void* const _Last, const size_t _Count, const uint64_t _Value) noexcept {
    return _Finding::_Search_n_impl<_Finding::_Find_traits_8>(_First, _Last, _Count, _Value);
}

} // extern "C"

namespace {
    namespace _Counting {
#ifdef _M_ARM64EC
        using _Count_traits_8 = void;
        using _Count_traits_4 = void;
        using _Count_traits_2 = void;
        using _Count_traits_1 = void;
#else // ^^^ defined(_M_ARM64EC) / !defined(_M_ARM64EC) vvv
        struct _Count_traits_8 : _Finding::_Find_traits_8 {
            static __m256i _Sub_avx(const __m256i _Lhs, const __m256i _Rhs) noexcept {
                return _mm256_sub_epi64(_Lhs, _Rhs);
            }

            static __m128i _Sub_sse(const __m128i _Lhs, const __m128i _Rhs) noexcept {
                return _mm_sub_epi64(_Lhs, _Rhs);
            }

            static size_t _Reduce_avx(const __m256i _Val) noexcept {
                const __m128i _Lo64 = _mm256_extracti128_si256(_Val, 0);
                const __m128i _Hi64 = _mm256_extracti128_si256(_Val, 1);
                const __m128i _Rx8  = _mm_add_epi64(_Lo64, _Hi64);
                return _Reduce_sse(_Rx8);
            }

            static size_t _Reduce_sse(const __m128i _Val) noexcept {
#ifdef _M_IX86
                return static_cast<uint32_t>(_mm_cvtsi128_si32(_Val))
                     + static_cast<uint32_t>(_mm_extract_epi32(_Val, 2));
#else // ^^^ defined(_M_IX86) / defined(_M_X64) vvv
                return _mm_cvtsi128_si64(_Val) + _mm_extract_epi64(_Val, 1);
#endif // ^^^ defined(_M_X64) ^^^
            }
        };

        struct _Count_traits_4 : _Finding::_Find_traits_4 {
            // For AVX2, we use hadd_epi32 three times to combine pairs of 32-bit counters into 32-bit results.
            // Therefore, _Max_count is 0x1FFF'FFFF, which is 0xFFFF'FFF8 when doubled three times; any more would
            // overflow.

            // For SSE4.2, we use hadd_epi32 twice. This would allow a larger limit,
            // but it's simpler to use the smaller limit for both codepaths.

            static constexpr size_t _Max_count = 0x1FFF'FFFF;

            static __m256i _Sub_avx(const __m256i _Lhs, const __m256i _Rhs) noexcept {
                return _mm256_sub_epi32(_Lhs, _Rhs);
            }

            static __m128i _Sub_sse(const __m128i _Lhs, const __m128i _Rhs) noexcept {
                return _mm_sub_epi32(_Lhs, _Rhs);
            }

            static size_t _Reduce_avx(const __m256i _Val) noexcept {
                constexpr auto _Shuf = _MM_SHUFFLE(3, 1, 2, 0); // Cross lane, to reduce further on low lane
                const __m256i _Rx4   = _mm256_hadd_epi32(_Val, _mm256_setzero_si256()); // (0+1),(2+3),0,0 per lane
                const __m256i _Rx5   = _mm256_permute4x64_epi64(_Rx4, _Shuf); // low lane  (0+1),(2+3),(4+5),(6+7)
                const __m256i _Rx6   = _mm256_hadd_epi32(_Rx5, _mm256_setzero_si256()); // (0+...+3),(4+...+7),0,0
                const __m256i _Rx7   = _mm256_hadd_epi32(_Rx6, _mm256_setzero_si256()); // (0+...+7),0,0,0
                return static_cast<uint32_t>(_mm_cvtsi128_si32(_mm256_castsi256_si128(_Rx7)));
            }

            static size_t _Reduce_sse(const __m128i _Val) noexcept {
                const __m128i _Rx4 = _mm_hadd_epi32(_Val, _mm_setzero_si128()); // (0+1),(2+3),0,0
                const __m128i _Rx5 = _mm_hadd_epi32(_Rx4, _mm_setzero_si128()); // (0+...+3),0,0,0
                return static_cast<uint32_t>(_mm_cvtsi128_si32(_Rx5));
            }
        };

        struct _Count_traits_2 : _Finding::_Find_traits_2 {
            // For both AVX2 and SSE4.2, we use hadd_epi16 once to combine pairs of 16-bit counters into 16-bit results.
            // Therefore, _Max_count is 0x7FFF, which is 0xFFFE when doubled; any more would overflow.

            static constexpr size_t _Max_count = 0x7FFF;

            static __m256i _Sub_avx(const __m256i _Lhs, const __m256i _Rhs) noexcept {
                return _mm256_sub_epi16(_Lhs, _Rhs);
            }

            static __m128i _Sub_sse(const __m128i _Lhs, const __m128i _Rhs) noexcept {
                return _mm_sub_epi16(_Lhs, _Rhs);
            }

            static size_t _Reduce_avx(const __m256i _Val) noexcept {
                const __m256i _Rx2 = _mm256_hadd_epi16(_Val, _mm256_setzero_si256());
                const __m256i _Rx3 = _mm256_unpacklo_epi16(_Rx2, _mm256_setzero_si256());
                return _Count_traits_4::_Reduce_avx(_Rx3);
            }

            static size_t _Reduce_sse(const __m128i _Val) noexcept {
                const __m128i _Rx2 = _mm_hadd_epi16(_Val, _mm_setzero_si128());
                const __m128i _Rx3 = _mm_unpacklo_epi16(_Rx2, _mm_setzero_si128());
                return _Count_traits_4::_Reduce_sse(_Rx3);
            }
        };

        struct _Count_traits_1 : _Finding::_Find_traits_1 {
            // For AVX2, _Max_portion_size below is _Max_count * 32 bytes, and we have 1-byte elements.
            // We're using packed 8-bit counters, and 32 of those fit in 256 bits.

            // For SSE4.2, _Max_portion_size below is _Max_count * 16 bytes, and we have 1-byte elements.
            // We're using packed 8-bit counters, and 16 of those fit in 128 bits.

            // For both codepaths, this is why _Max_count is the maximum unsigned 8-bit integer.
            // (The reduction steps aren't the limiting factor here.)

            static constexpr size_t _Max_count = 0xFF;

            static __m256i _Sub_avx(const __m256i _Lhs, const __m256i _Rhs) noexcept {
                return _mm256_sub_epi8(_Lhs, _Rhs);
            }

            static __m128i _Sub_sse(const __m128i _Lhs, const __m128i _Rhs) noexcept {
                return _mm_sub_epi8(_Lhs, _Rhs);
            }

            static size_t _Reduce_avx(const __m256i _Val) noexcept {
                const __m256i _Rx1 = _mm256_sad_epu8(_Val, _mm256_setzero_si256());
                return _Count_traits_8::_Reduce_avx(_Rx1);
            }

            static size_t _Reduce_sse(const __m128i _Val) noexcept {
                const __m128i _Rx1 = _mm_sad_epu8(_Val, _mm_setzero_si128());
                return _Count_traits_8::_Reduce_sse(_Rx1);
            }
        };
#endif // ^^^ !defined(_M_ARM64EC) ^^^

        template <class _Traits, class _Ty>
        __declspec(noalias) size_t __stdcall _Count_impl(
            const void* _First, const void* const _Last, const _Ty _Val) noexcept {
            size_t _Result = 0;

#ifndef _M_ARM64EC
            const size_t _Size_bytes = _Byte_length(_First, _Last);

            if (size_t _Avx_size = _Size_bytes & ~size_t{0x1F}; _Avx_size != 0 && _Use_avx2()) {
                const __m256i _Comparand = _Traits::_Set_avx(_Val);
                const void* _Stop_at     = _First;

                for (;;) {
                    if constexpr (sizeof(_Ty) >= sizeof(size_t)) {
                        _Advance_bytes(_Stop_at, _Avx_size);
                    } else {
                        constexpr size_t _Max_portion_size = _Traits::_Max_count * 32;
                        const size_t _Portion_size = _Avx_size < _Max_portion_size ? _Avx_size : _Max_portion_size;
                        _Advance_bytes(_Stop_at, _Portion_size);
                        _Avx_size -= _Portion_size;
                    }

                    __m256i _Count_vector = _mm256_setzero_si256();

                    do {
                        const __m256i _Data = _mm256_loadu_si256(static_cast<const __m256i*>(_First));
                        const __m256i _Mask = _Traits::_Cmp_avx(_Data, _Comparand);
                        _Count_vector       = _Traits::_Sub_avx(_Count_vector, _Mask);
                        _Advance_bytes(_First, 32);
                    } while (_First != _Stop_at);

                    _Result += _Traits::_Reduce_avx(_Count_vector);

                    if constexpr (sizeof(_Ty) >= sizeof(size_t)) {
                        break;
                    } else {
                        if (_Avx_size == 0) {
                            break;
                        }
                    }
                }

                if (const size_t _Avx_tail_size = _Size_bytes & 0x1C; _Avx_tail_size != 0) {
                    const __m256i _Tail_mask  = _Avx2_tail_mask_32(_Avx_tail_size);
                    const __m256i _Data       = _mm256_maskload_epi32(static_cast<const int*>(_First), _Tail_mask);
                    const __m256i _Mask       = _mm256_and_si256(_Traits::_Cmp_avx(_Data, _Comparand), _Tail_mask);
                    const unsigned int _Bingo = _mm256_movemask_epi8(_Mask);
                    const size_t _Tail_count  = __popcnt(_Bingo); // Assume available with SSE4.2
                    _Result += _Tail_count / sizeof(_Ty);
                    _Advance_bytes(_First, _Avx_tail_size);
                }

                _mm256_zeroupper(); // TRANSITION, DevCom-10331414

                if constexpr (sizeof(_Ty) >= 4) {
                    return _Result;
                }
            } else if (size_t _Sse_size = _Size_bytes & ~size_t{0xF}; _Sse_size != 0 && _Use_sse42()) {
                const __m128i _Comparand = _Traits::_Set_sse(_Val);
                const void* _Stop_at     = _First;

                for (;;) {
                    if constexpr (sizeof(_Ty) >= sizeof(size_t)) {
                        _Advance_bytes(_Stop_at, _Sse_size);
                    } else {
                        constexpr size_t _Max_portion_size = _Traits::_Max_count * 16;
                        const size_t _Portion_size = _Sse_size < _Max_portion_size ? _Sse_size : _Max_portion_size;
                        _Advance_bytes(_Stop_at, _Portion_size);
                        _Sse_size -= _Portion_size;
                    }

                    __m128i _Count_vector = _mm_setzero_si128();

                    do {
                        const __m128i _Data = _mm_loadu_si128(static_cast<const __m128i*>(_First));
                        const __m128i _Mask = _Traits::_Cmp_sse(_Data, _Comparand);
                        _Count_vector       = _Traits::_Sub_sse(_Count_vector, _Mask);
                        _Advance_bytes(_First, 16);
                    } while (_First != _Stop_at);

                    _Result += _Traits::_Reduce_sse(_Count_vector);

                    if constexpr (sizeof(_Ty) >= sizeof(size_t)) {
                        break;
                    } else {
                        if (_Sse_size == 0) {
                            break;
                        }
                    }
                }
            }
#endif // ^^^ !defined(_M_ARM64EC) ^^^

            for (auto _Ptr = static_cast<const _Ty*>(_First); _Ptr != _Last; ++_Ptr) {
                if (*_Ptr == _Val) {
                    ++_Result;
                }
            }
            return _Result;
        }
    } // namespace _Counting
} // unnamed namespace

extern "C" {

__declspec(noalias) size_t __stdcall __std_count_trivial_1(
    const void* const _First, const void* const _Last, const uint8_t _Val) noexcept {
    return _Counting::_Count_impl<_Counting::_Count_traits_1>(_First, _Last, _Val);
}

__declspec(noalias) size_t __stdcall __std_count_trivial_2(
    const void* const _First, const void* const _Last, const uint16_t _Val) noexcept {
    return _Counting::_Count_impl<_Counting::_Count_traits_2>(_First, _Last, _Val);
}

__declspec(noalias) size_t __stdcall __std_count_trivial_4(
    const void* const _First, const void* const _Last, const uint32_t _Val) noexcept {
    return _Counting::_Count_impl<_Counting::_Count_traits_4>(_First, _Last, _Val);
}

__declspec(noalias) size_t __stdcall __std_count_trivial_8(
    const void* const _First, const void* const _Last, const uint64_t _Val) noexcept {
    return _Counting::_Count_impl<_Counting::_Count_traits_8>(_First, _Last, _Val);
}

} // extern "C"

namespace {
    namespace _Find_meow_of {
        enum class _Predicate { _Any_of, _None_of };

#ifndef _M_ARM64EC
        namespace _Bitmap_details {
            __m256i _Bitmap_step(const __m256i _Bitmap, const __m256i _Data) noexcept {
                const __m256i _Data_high    = _mm256_srli_epi32(_Data, 5);
                const __m256i _Bitmap_parts = _mm256_permutevar8x32_epi32(_Bitmap, _Data_high);
                const __m256i _Data_low_inv = _mm256_andnot_si256(_Data, _mm256_set1_epi32(0x1F));
                const __m256i _Mask         = _mm256_sllv_epi32(_Bitmap_parts, _Data_low_inv);
                return _Mask;
            }

            template <class _Ty>
            __m256i _Load_avx_256_8(const _Ty* const _Src) noexcept {
                if constexpr (sizeof(_Ty) == 1) {
                    return _mm256_cvtepu8_epi32(_mm_loadu_si64(_Src));
                } else if constexpr (sizeof(_Ty) == 2) {
                    return _mm256_cvtepu16_epi32(_mm_loadu_si128(reinterpret_cast<const __m128i*>(_Src)));
                } else if constexpr (sizeof(_Ty) == 4) {
                    return _mm256_loadu_si256(reinterpret_cast<const __m256i*>(_Src));
                } else if constexpr (sizeof(_Ty) == 8) {
                    const __m256i _Low  = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(_Src));
                    const __m256i _High = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(_Src) + 1);
                    const __m256i _Pack = _mm256_packs_epi32(_Low, _High);
                    return _mm256_permute4x64_epi64(_Pack, _MM_SHUFFLE(3, 1, 2, 0));
                } else {
                    static_assert(false, "Unexpected size");
                }
            }

            template <class _Ty>
            __m256i _Load_avx_256_8_last(const _Ty* const _Src, const size_t _Count) noexcept {
                if constexpr (sizeof(_Ty) == 1) {
                    uint8_t _Buf[8];
                    memcpy(_Buf, _Src, _Count);
                    return _mm256_cvtepu8_epi32(_mm_loadu_si64(_Buf));
                } else if constexpr (sizeof(_Ty) == 2) {
                    uint8_t _Buf[16];
                    memcpy(_Buf, _Src, _Count * 2);
                    return _mm256_cvtepu16_epi32(_mm_loadu_si128(reinterpret_cast<const __m128i*>(_Buf)));
                } else if constexpr (sizeof(_Ty) == 4) {
                    return _mm256_maskload_epi32(reinterpret_cast<const int*>(_Src), _Avx2_tail_mask_32(_Count * 4));
                } else if constexpr (sizeof(_Ty) == 8) {
                    const __m256i _Mask_low  = _Avx2_tail_mask_32((_Count > 4 ? 4 : _Count) * 8);
                    const __m256i _Low       = _mm256_maskload_epi32(reinterpret_cast<const int*>(_Src) + 0, _Mask_low);
                    const __m256i _Mask_high = _Avx2_tail_mask_32((_Count > 4 ? _Count - 4 : 0) * 8);
                    const __m256i _High = _mm256_maskload_epi32(reinterpret_cast<const int*>(_Src) + 8, _Mask_high);
                    const __m256i _Pack = _mm256_packs_epi32(_Low, _High);
                    return _mm256_permute4x64_epi64(_Pack, _MM_SHUFFLE(3, 1, 2, 0));
                } else {
                    static_assert(false, "Unexpected size");
                }
            }

            template <class _Ty>
            __m256i _Mask_out_overflow(const __m256i _Mask, const __m256i _Data) noexcept {
                if constexpr (sizeof(_Ty) == 1) {
                    return _Mask;
                } else {
                    const __m256i _Data_high =
                        _mm256_and_si256(_Data, _mm256_set1_epi32(static_cast<int>(0xFFFF'FF00)));
                    const __m256i _Fit_mask = _mm256_cmpeq_epi32(_Data_high, _mm256_setzero_si256());
                    return _mm256_and_si256(_Mask, _Fit_mask);
                }
            }

            template <class _Ty>
            __m256i _Make_bitmap_small(const _Ty* _Needle_ptr, const size_t _Needle_length) noexcept {
                __m256i _Bitmap = _mm256_setzero_si256();

                const _Ty* const _Stop = _Needle_ptr + _Needle_length;

                for (; _Needle_ptr != _Stop; ++_Needle_ptr) {
                    const _Ty _Val            = *_Needle_ptr;
                    const __m128i _Count_low  = _mm_cvtsi32_si128(_Val & 0x3F);
                    const auto _Count_high_x8 = static_cast<uint32_t>((_Val >> 3) & 0x18);
                    const __m256i _One_1_high = _mm256_cvtepu8_epi64(_mm_cvtsi32_si128(1u << _Count_high_x8));
                    const __m256i _One_1      = _mm256_sll_epi64(_One_1_high, _Count_low);
                    _Bitmap                   = _mm256_or_si256(_Bitmap, _One_1);
                }

                return _Bitmap;
            }

            template <class _Ty>
            __m256i _Make_bitmap_large(const _Ty* _Needle_ptr, const size_t _Needle_length) noexcept {
                alignas(32) uint8_t _Table[256] = {};

                const _Ty* const _Stop = _Needle_ptr + _Needle_length;

                for (; _Needle_ptr != _Stop; ++_Needle_ptr) {
                    _Table[*_Needle_ptr] = 0xFF;
                }

                const auto _Table_as_avx = reinterpret_cast<const __m256i*>(_Table);

                return _mm256_setr_epi32( //
                    _mm256_movemask_epi8(_mm256_load_si256(_Table_as_avx + 0)),
                    _mm256_movemask_epi8(_mm256_load_si256(_Table_as_avx + 1)),
                    _mm256_movemask_epi8(_mm256_load_si256(_Table_as_avx + 2)),
                    _mm256_movemask_epi8(_mm256_load_si256(_Table_as_avx + 3)),
                    _mm256_movemask_epi8(_mm256_load_si256(_Table_as_avx + 4)),
                    _mm256_movemask_epi8(_mm256_load_si256(_Table_as_avx + 5)),
                    _mm256_movemask_epi8(_mm256_load_si256(_Table_as_avx + 6)),
                    _mm256_movemask_epi8(_mm256_load_si256(_Table_as_avx + 7)));
            }

            template <class _Ty>
            __m256i _Make_bitmap(const _Ty* const _Needle_ptr, const size_t _Needle_length) noexcept {
                if (_Needle_length <= 20) {
                    return _Make_bitmap_small(_Needle_ptr, _Needle_length);
                } else {
                    return _Make_bitmap_large(_Needle_ptr, _Needle_length);
                }
            }
        } // namespace _Bitmap_details
#endif // ^^^ !defined(_M_ARM64EC) ^^^

        namespace _Bitmap_impl {
#ifndef _M_ARM64EC
            template <class _Ty>
            bool _Use_bitmap_avx(const size_t _Count1, const size_t _Count2) noexcept {
                if constexpr (sizeof(_Ty) == 1) {
                    if (_Count2 <= 16) {
                        return _Count1 > 1000;
                    } else if (_Count2 <= 48) {
                        return _Count1 > 80;
                    } else if (_Count2 <= 240) {
                        return _Count1 > 40;
                    } else if (_Count2 <= 1000) {
                        return _Count1 > 32;
                    } else {
                        return _Count1 > 16;
                    }
                } else if constexpr (sizeof(_Ty) == 2) {
                    if (_Count2 <= 8) {
                        return _Count1 > 128;
                    } else if (_Count2 <= 48) {
                        return _Count1 > 32;
                    } else if (_Count2 <= 72) {
                        return _Count1 > 24;
                    } else if (_Count2 <= 144) {
                        return _Count1 > 16;
                    } else {
                        return _Count1 > 8;
                    }
                } else if constexpr (sizeof(_Ty) == 4) {
                    if (_Count2 <= 8) {
                        return _Count1 > 64;
                    } else if (_Count2 <= 24) {
                        return _Count1 > 40;
                    } else if (_Count2 <= 44) {
                        return _Count1 > 24;
                    } else if (_Count2 <= 112) {
                        return _Count1 > 16;
                    } else {
                        return _Count1 > 8;
                    }
                } else if constexpr (sizeof(_Ty) == 8) {
                    if (_Count2 <= 8) {
                        return _Count1 > 40;
                    } else if (_Count2 <= 12) {
                        return _Count1 > 20;
                    } else if (_Count2 <= 48) {
                        return _Count1 > 16;
                    } else if (_Count2 <= 64) {
                        return _Count1 > 12;
                    } else if (_Count2 <= 192) {
                        return _Count1 > 8;
                    } else {
                        return _Count1 > 4;
                    }
                } else {
                    static_assert(false, "unexpected size");
                }
            }

            template <class _Ty>
            bool _Use_bitmap_scalar(const size_t _Count1, const size_t _Count2) noexcept {
                if constexpr (sizeof(_Ty) == 1) {
                    if (_Count2 <= 32) {
                        return false;
                    } else if (_Count2 <= 48) {
                        return _Count1 > 416;
                    } else if (_Count2 <= 64) {
                        return _Count1 > 224;
                    } else if (_Count2 <= 80) {
                        return _Count1 > 128;
                    } else if (_Count2 <= 540) {
                        return _Count1 > 48;
                    } else {
                        return _Count1 > 32;
                    }
                } else if constexpr (sizeof(_Ty) == 2) {
                    if (_Count2 <= 8) {
                        return false;
                    } else if (_Count2 <= 80) {
                        return _Count1 > 16;
                    } else {
                        return _Count1 > 8;
                    }
                } else if constexpr (sizeof(_Ty) == 4) {
                    if (_Count2 <= 32) {
                        return false;
                    } else if (_Count2 <= 112) {
                        return _Count1 > 16;
                    } else {
                        return _Count1 > 8;
                    }
                } else if constexpr (sizeof(_Ty) == 8) {
                    if (_Count2 <= 16) {
                        return false;
                    } else if (_Count2 <= 32) {
                        return _Count1 > 16;
                    } else if (_Count2 <= 112) {
                        return _Count1 > 8;
                    } else {
                        return _Count1 > 4;
                    }
                } else {
                    static_assert(false, "unexpected size");
                }
            }

            enum class _Strategy { _No_bitmap, _Scalar_bitmap, _Vector_bitmap };

            template <class _Ty>
            _Strategy _Pick_strategy(const size_t _Count1, const size_t _Count2, const bool _Use_avx2_) noexcept {
                if (_Use_avx2_ && _Count1 > 48) {
                    return _Use_bitmap_avx<_Ty>(_Count1, _Count2) ? _Strategy::_Vector_bitmap : _Strategy::_No_bitmap;
                } else {
                    return _Use_bitmap_scalar<_Ty>(_Count1, _Count2) ? _Strategy::_Scalar_bitmap
                                                                     : _Strategy::_No_bitmap;
                }
            }

            template <class _Ty>
            bool _Can_fit_256_bits_sse(const _Ty* _Needle_ptr, const size_t _Needle_length) noexcept {
                if constexpr (sizeof(_Ty) == 1) {
                    return true;
                } else {
                    __m128i _Mask = _mm_undefined_si128();
                    if constexpr (sizeof(_Ty) == 2) {
                        _Mask = _mm_set1_epi16(static_cast<short>(0xFF00));
                    } else if constexpr (sizeof(_Ty) == 4) {
                        _Mask = _mm_set1_epi32(static_cast<int>(0xFFFF'FF00));
                    } else if constexpr (sizeof(_Ty) == 8) {
                        _Mask = _mm_set1_epi64x(static_cast<long long>(0xFFFF'FFFF'FFFF'FF00));
                    } else {
                        static_assert(false, "Unexpected size");
                    }

                    const size_t _Byte_size = _Needle_length * sizeof(_Ty);

                    constexpr size_t _Vec_size = sizeof(_Mask);
                    constexpr size_t _Vec_mask = _Vec_size - 1;
                    static_assert((_Vec_size & _Vec_mask) == 0);

                    const void* _Stop = _Needle_ptr;
                    _Advance_bytes(_Stop, _Byte_size & ~_Vec_mask);
                    for (; _Needle_ptr != _Stop; _Needle_ptr += _Vec_size / sizeof(_Ty)) {
                        const __m128i _Data = _mm_loadu_si128(reinterpret_cast<const __m128i*>(_Needle_ptr));
                        if (!_mm_testz_si128(_Mask, _Data)) {
                            return false;
                        }
                    }

                    _Advance_bytes(_Stop, _Byte_size & _Vec_mask);
                    for (; _Needle_ptr != _Stop; ++_Needle_ptr) {
                        if ((*_Needle_ptr & ~_Ty{0xFF}) != 0) {
                            return false;
                        }
                    }

                    return true;
                }
            }

            template <class _Ty, _Predicate _Pred>
            size_t _Impl_first_avx(const void* const _Haystack, const size_t _Haystack_length,
                const void* const _Needle, const size_t _Needle_length) noexcept {
                _Zeroupper_on_exit _Guard; // TRANSITION, DevCom-10331414

                const auto _Haystack_ptr = static_cast<const _Ty*>(_Haystack);
                const auto _Needle_ptr   = static_cast<const _Ty*>(_Needle);

                const __m256i _Bitmap = _Bitmap_details::_Make_bitmap(_Needle_ptr, _Needle_length);

                const size_t _Haystack_length_vec = _Haystack_length & ~size_t{7};
                for (size_t _Ix = 0; _Ix != _Haystack_length_vec; _Ix += 8) {
                    const __m256i _Data      = _Bitmap_details::_Load_avx_256_8(_Haystack_ptr + _Ix);
                    const __m256i _Mask_part = _Bitmap_details::_Bitmap_step(_Bitmap, _Data);
                    const __m256i _Mask_full = _Bitmap_details::_Mask_out_overflow<_Ty>(_Mask_part, _Data);
                    unsigned int _Bingo      = _mm256_movemask_ps(_mm256_castsi256_ps(_Mask_full));

                    if constexpr (_Pred == _Predicate::_None_of) {
                        _Bingo ^= 0xFF;
                    }

                    if (_Bingo != 0) {
                        return _Ix + _tzcnt_u32(_Bingo);
                    }
                }

                const size_t _Haystack_length_tail = _Haystack_length & 7;
                if (_Haystack_length_tail != 0) {
                    const unsigned int _Tail_bingo_mask = (1 << _Haystack_length_tail) - 1;
                    const auto _Last_ptr                = _Haystack_ptr + _Haystack_length_vec;
                    const __m256i _Data      = _Bitmap_details::_Load_avx_256_8_last(_Last_ptr, _Haystack_length_tail);
                    const __m256i _Mask_part = _Bitmap_details::_Bitmap_step(_Bitmap, _Data);
                    const __m256i _Mask_full = _Bitmap_details::_Mask_out_overflow<_Ty>(_Mask_part, _Data);
                    unsigned int _Bingo      = _mm256_movemask_ps(_mm256_castsi256_ps(_Mask_full)) & _Tail_bingo_mask;

                    if constexpr (_Pred == _Predicate::_None_of) {
                        _Bingo ^= _Tail_bingo_mask;
                    }

                    if (_Bingo != 0) {
                        return _Haystack_length_vec + _tzcnt_u32(_Bingo);
                    }
                }

                return static_cast<size_t>(-1);
            }

            template <class _Ty, _Predicate _Pred>
            size_t _Impl_last_avx(const void* const _Haystack, size_t _Haystack_length, const void* const _Needle,
                const size_t _Needle_length) noexcept {
                _Zeroupper_on_exit _Guard; // TRANSITION, DevCom-10331414

                const auto _Haystack_ptr = static_cast<const _Ty*>(_Haystack);
                const auto _Needle_ptr   = static_cast<const _Ty*>(_Needle);

                const __m256i _Bitmap = _Bitmap_details::_Make_bitmap(_Needle_ptr, _Needle_length);

                while (_Haystack_length >= 8) {
                    _Haystack_length -= 8;
                    const __m256i _Data      = _Bitmap_details::_Load_avx_256_8(_Haystack_ptr + _Haystack_length);
                    const __m256i _Mask_part = _Bitmap_details::_Bitmap_step(_Bitmap, _Data);
                    const __m256i _Mask_full = _Bitmap_details::_Mask_out_overflow<_Ty>(_Mask_part, _Data);
                    unsigned int _Bingo      = _mm256_movemask_ps(_mm256_castsi256_ps(_Mask_full));

                    if constexpr (_Pred == _Predicate::_None_of) {
                        _Bingo ^= 0xFF;
                    }

                    if (_Bingo != 0) {
                        return _Haystack_length + 31 - _lzcnt_u32(_Bingo);
                    }
                }

                const size_t _Haystack_length_tail = _Haystack_length & 7;
                if (_Haystack_length_tail != 0) {
                    const unsigned int _Tail_bingo_mask = (1 << _Haystack_length_tail) - 1;
                    const __m256i _Data = _Bitmap_details::_Load_avx_256_8_last(_Haystack_ptr, _Haystack_length_tail);
                    const __m256i _Mask_part = _Bitmap_details::_Bitmap_step(_Bitmap, _Data);
                    const __m256i _Mask_full = _Bitmap_details::_Mask_out_overflow<_Ty>(_Mask_part, _Data);
                    unsigned int _Bingo      = _mm256_movemask_ps(_mm256_castsi256_ps(_Mask_full)) & _Tail_bingo_mask;

                    if constexpr (_Pred == _Predicate::_None_of) {
                        _Bingo ^= _Tail_bingo_mask;
                    }

                    if (_Bingo != 0) {
                        return 31 - _lzcnt_u32(_Bingo);
                    }
                }

                return static_cast<size_t>(-1);
            }
#endif // ^^^ !defined(_M_ARM64EC) ^^^

            using _Scalar_table_t = bool[256];

            template <class _Ty>
            [[nodiscard]] bool _Build_scalar_table(
                const void* const _Needle, const size_t _Needle_length, _Scalar_table_t& _Table) noexcept {
                auto _Ptr       = static_cast<const _Ty*>(_Needle);
                const auto _End = _Ptr + _Needle_length;

                for (; _Ptr != _End; ++_Ptr) {
                    const _Ty _Val = *_Ptr;

                    if constexpr (sizeof(_Val) > 1) {
                        if (_Val >= 256) {
                            return false;
                        }
                    }

                    _Table[_Val] = true;
                }

                return true;
            }

#ifndef _M_ARM64EC
            template <class _Ty>
            void _Build_scalar_table_no_check(
                const void* const _Needle, const size_t _Needle_length, _Scalar_table_t& _Table) noexcept {
                auto _Ptr       = static_cast<const _Ty*>(_Needle);
                const auto _End = _Ptr + _Needle_length;

                for (; _Ptr != _End; ++_Ptr) {
                    _Table[*_Ptr] = true;
                }
            }
#endif // ^^^ !defined(_M_ARM64EC) ^^^

            template <class _Ty, _Predicate _Pred>
            size_t _Impl_first_scalar(
                const void* const _Haystack, const size_t _Haystack_length, const _Scalar_table_t& _Table) noexcept {
                const auto _Haystack_ptr = static_cast<const _Ty*>(_Haystack);

                for (size_t _Ix = 0; _Ix != _Haystack_length; ++_Ix) {
                    const _Ty _Val = _Haystack_ptr[_Ix];

                    if constexpr (sizeof(_Val) > 1) {
                        if (_Val >= 256) {
                            if constexpr (_Pred == _Predicate::_Any_of) {
                                continue;
                            } else {
                                return _Ix;
                            }
                        }
                    }

                    if constexpr (_Pred == _Predicate::_Any_of) {
                        if (_Table[_Val]) {
                            return _Ix;
                        }
                    } else {
                        if (!_Table[_Val]) {
                            return _Ix;
                        }
                    }
                }

                return static_cast<size_t>(-1);
            }

            template <class _Ty, _Predicate _Pred>
            size_t _Impl_last_scalar(
                const void* const _Haystack, size_t _Haystack_length, const _Scalar_table_t& _Table) noexcept {
                const auto _Haystack_ptr = static_cast<const _Ty*>(_Haystack);

                while (_Haystack_length != 0) {
                    --_Haystack_length;

                    const _Ty _Val = _Haystack_ptr[_Haystack_length];

                    if constexpr (sizeof(_Val) > 1) {
                        if (_Val >= 256) {
                            if constexpr (_Pred == _Predicate::_Any_of) {
                                continue;
                            } else {
                                return _Haystack_length;
                            }
                        }
                    }

                    if constexpr (_Pred == _Predicate::_Any_of) {
                        if (_Table[_Val]) {
                            return _Haystack_length;
                        }
                    } else {
                        if (!_Table[_Val]) {
                            return _Haystack_length;
                        }
                    }
                }

                return static_cast<size_t>(-1);
            }
        } // namespace _Bitmap_impl

        namespace _First_of {
            template <class _Ty, _Predicate _Pred>
            const void* _Fallback(const void* _First1, const void* const _Last1, const void* const _First2,
                const void* const _Last2) noexcept {
                auto _Ptr_haystack           = static_cast<const _Ty*>(_First1);
                const auto _Ptr_haystack_end = static_cast<const _Ty*>(_Last1);
                const auto _Ptr_needle       = static_cast<const _Ty*>(_First2);
                const auto _Ptr_needle_end   = static_cast<const _Ty*>(_Last2);

                for (; _Ptr_haystack != _Ptr_haystack_end; ++_Ptr_haystack) {
                    if constexpr (_Pred == _Predicate::_Any_of) {
                        for (auto _Ptr = _Ptr_needle; _Ptr != _Ptr_needle_end; ++_Ptr) {
                            if (*_Ptr_haystack == *_Ptr) {
                                return _Ptr_haystack;
                            }
                        }
                    } else {
                        bool _Match = false;
                        for (auto _Ptr = _Ptr_needle; _Ptr != _Ptr_needle_end; ++_Ptr) {
                            if (*_Ptr_haystack == *_Ptr) {
                                _Match = true;
                                break;
                            }
                        }

                        if (!_Match) {
                            return _Ptr_haystack;
                        }
                    }
                }

                return _Ptr_haystack;
            }

#ifndef _M_ARM64EC
            template <class _Ty, _Predicate _Pred>
            const void* _Impl_pcmpestri(const void* _First1, const size_t _Haystack_length, const void* const _First2,
                const size_t _Needle_length) noexcept {
                constexpr int _Op_base =
                    (_Pred == _Predicate::_Any_of ? _SIDD_POSITIVE_POLARITY : _SIDD_MASKED_NEGATIVE_POLARITY)
                    | (sizeof(_Ty) == 1 ? _SIDD_UBYTE_OPS : _SIDD_UWORD_OPS) | _SIDD_CMP_EQUAL_ANY;
                constexpr int _Op           = _Op_base | _SIDD_LEAST_SIGNIFICANT;
                constexpr int _Part_size_el = sizeof(_Ty) == 1 ? 16 : 8;

                const void* _Stop_at = _First1;
                _Advance_bytes(_Stop_at, _Haystack_length & ~size_t{0xF});

                if (_Needle_length <= 16) {
                    // Special handling of small needle
                    // The generic branch could also be modified to handle it, but with slightly worse performance

                    const int _Needle_length_el = static_cast<int>(_Needle_length / sizeof(_Ty));

                    alignas(16) uint8_t _Tmp2[16];
                    memcpy(_Tmp2, _First2, _Needle_length);
                    const __m128i _Data2 = _mm_load_si128(reinterpret_cast<const __m128i*>(_Tmp2));

                    while (_First1 != _Stop_at) {
                        const __m128i _Data1 = _mm_loadu_si128(static_cast<const __m128i*>(_First1));
                        if (_mm_cmpestrc(_Data2, _Needle_length_el, _Data1, _Part_size_el, _Op)) {
                            const int _Pos = _mm_cmpestri(_Data2, _Needle_length_el, _Data1, _Part_size_el, _Op);
                            _Advance_bytes(_First1, _Pos * sizeof(_Ty));
                            return _First1;
                        }

                        _Advance_bytes(_First1, 16);
                    }

                    if (const size_t _Last_part_size = _Haystack_length & 0xF; _Last_part_size != 0) {
                        const int _Last_part_size_el = static_cast<int>(_Last_part_size / sizeof(_Ty));

                        alignas(16) uint8_t _Tmp1[16];
                        memcpy(_Tmp1, _First1, _Last_part_size);
                        const __m128i _Data1 = _mm_load_si128(reinterpret_cast<const __m128i*>(_Tmp1));

                        if (_mm_cmpestrc(_Data2, _Needle_length_el, _Data1, _Last_part_size_el, _Op)) {
                            const int _Pos = _mm_cmpestri(_Data2, _Needle_length_el, _Data1, _Last_part_size_el, _Op);
                            _Advance_bytes(_First1, _Pos * sizeof(_Ty));
                            return _First1;
                        }

                        _Advance_bytes(_First1, _Last_part_size);
                    }

                    return _First1;
                } else {
                    const void* _Last_needle = _First2;
                    _Advance_bytes(_Last_needle, _Needle_length & ~size_t{0xF});

                    const int _Last_needle_length = static_cast<int>(_Needle_length & 0xF);

                    alignas(16) uint8_t _Tmp2[16];
                    memcpy(_Tmp2, _Last_needle, _Last_needle_length);
                    const __m128i _Last_needle_val   = _mm_load_si128(reinterpret_cast<const __m128i*>(_Tmp2));
                    const int _Last_needle_length_el = _Last_needle_length / sizeof(_Ty);

                    constexpr int _Not_found = 16; // arbitrary value greater than any found value
#pragma warning(push)
#pragma warning(disable : 4324) // structure was padded due to alignment specifier
                    const auto _Test_whole_needle = [=](const __m128i _Data1, const int _Size1,
                                                        const int _Found_pos_init) noexcept {
                        if constexpr (_Pred == _Predicate::_Any_of) {
                            int _Found_pos = _Found_pos_init;

                            const auto _Step = [&_Found_pos](const __m128i _Data2, const int _Size2,
                                                   const __m128i _Data1, const int _Size1) noexcept {
                                if (_mm_cmpestrc(_Data2, _Size2, _Data1, _Size1, _Op)) {
                                    const int _Pos = _mm_cmpestri(_Data2, _Size2, _Data1, _Size1, _Op);
                                    if (_Pos < _Found_pos) {
                                        _Found_pos = _Pos;
                                    }
                                }
                            };

                            const void* _Cur_needle = _First2;
                            do {
                                const __m128i _Data2 = _mm_loadu_si128(static_cast<const __m128i*>(_Cur_needle));
                                _Step(_Data2, _Part_size_el, _Data1, _Size1);
                                _Advance_bytes(_Cur_needle, 16);
                            } while (_Cur_needle != _Last_needle);

                            if (_Last_needle_length_el != 0) {
                                _Step(_Last_needle_val, _Last_needle_length_el, _Data1, _Size1);
                            }

                            return _Found_pos;
                        } else {
                            constexpr int _Op_mask = _Op_base | _SIDD_BIT_MASK;

                            const void* _Cur_needle = _First2;

                            const __m128i _Data2_first = _mm_loadu_si128(static_cast<const __m128i*>(_Cur_needle));

                            __m128i _Found = _mm_cmpestrm(_Data2_first, _Part_size_el, _Data1, _Size1, _Op_mask);
                            _Advance_bytes(_Cur_needle, 16);

                            while (_Cur_needle != _Last_needle) {
                                const __m128i _Data2 = _mm_loadu_si128(static_cast<const __m128i*>(_Cur_needle));
                                const __m128i _Found_part =
                                    _mm_cmpestrm(_Data2, _Part_size_el, _Data1, _Size1, _Op_mask);
                                _Found = _mm_and_si128(_Found, _Found_part);
                                _Advance_bytes(_Cur_needle, 16);
                            }

                            if (_Last_needle_length_el != 0) {
                                const __m128i _Found_part =
                                    _mm_cmpestrm(_Last_needle_val, _Last_needle_length_el, _Data1, _Size1, _Op_mask);
                                _Found = _mm_and_si128(_Found, _Found_part);
                            }

                            const unsigned int _Bingo = _mm_cvtsi128_si32(_Found);
                            int _Found_pos            = _Found_pos_init;

                            if (_Bingo != 0) {
                                unsigned long _Tmp;
                                // CodeQL [SM02313] _Tmp is always initialized: we just tested `if (_Bingo != 0)`.
                                _BitScanForward(&_Tmp, _Bingo);
                                if (_Found_pos > static_cast<int>(_Tmp)) {
                                    _Found_pos = static_cast<int>(_Tmp);
                                }
                            }

                            return _Found_pos;
                        }
                    };
#pragma warning(pop)

                    while (_First1 != _Stop_at) {
                        const int _Found_pos = _Test_whole_needle(
                            _mm_loadu_si128(static_cast<const __m128i*>(_First1)), _Part_size_el, _Not_found);

                        if (_Found_pos != _Not_found) {
                            _Advance_bytes(_First1, _Found_pos * sizeof(_Ty));
                            return _First1;
                        }

                        _Advance_bytes(_First1, 16);
                    }

                    if (const size_t _Last_part_size = _Haystack_length & 0xF; _Last_part_size != 0) {
                        const int _Last_part_size_el = static_cast<int>(_Last_part_size / sizeof(_Ty));

                        alignas(16) uint8_t _Tmp1[16];
                        memcpy(_Tmp1, _First1, _Last_part_size);
                        const __m128i _Data1 = _mm_load_si128(reinterpret_cast<const __m128i*>(_Tmp1));

                        const int _Found_pos = _Test_whole_needle(_Data1, _Last_part_size_el, _Last_part_size_el);

                        _Advance_bytes(_First1, _Found_pos * sizeof(_Ty));
                    }

                    return _First1;
                }
            }

            template <class _Ty>
            struct _Find_first_of_traits;

            template <>
            struct _Find_first_of_traits<uint32_t> {
                static __m256i _Cmp_avx(const __m256i _Lhs, const __m256i _Rhs) noexcept {
                    return _mm256_cmpeq_epi32(_Lhs, _Rhs);
                }

                template <size_t _Amount>
                static __m256i _Spread_avx(__m256i _Val, const size_t _Needle_length_el) noexcept {
                    if constexpr (_Amount == 0) {
                        return _mm256_undefined_si256();
                    } else if constexpr (_Amount == 1) {
                        return _mm256_broadcastd_epi32(_mm256_castsi256_si128(_Val));
                    } else if constexpr (_Amount == 2) {
                        return _mm256_broadcastq_epi64(_mm256_castsi256_si128(_Val));
                    } else if constexpr (_Amount == 4) {
                        if (_Needle_length_el < 4) {
                            _Val = _mm256_shuffle_epi32(_Val, _MM_SHUFFLE(0, 2, 1, 0));
                        }

                        return _mm256_permute4x64_epi64(_Val, _MM_SHUFFLE(1, 0, 1, 0));
                    } else if constexpr (_Amount == 8) {
                        if (_Needle_length_el < 8) {
                            const __m256i _Mask = _Avx2_tail_mask_32(_Needle_length_el * 4);
                            // zero unused elements in sequential permutation mask, so will be filled by 1st
                            const __m256i _Perm = _mm256_and_si256(_mm256_set_epi32(7, 6, 5, 4, 3, 2, 1, 0), _Mask);
                            _Val                = _mm256_permutevar8x32_epi32(_Val, _Perm);
                        }

                        return _Val;
                    } else {
                        static_assert(false, "Unexpected amount");
                    }
                }

                template <size_t _Amount>
                static __m256i _Shuffle_avx(const __m256i _Val) noexcept {
                    if constexpr (_Amount == 1) {
                        return _mm256_shuffle_epi32(_Val, _MM_SHUFFLE(2, 3, 0, 1));
                    } else if constexpr (_Amount == 2) {
                        return _mm256_shuffle_epi32(_Val, _MM_SHUFFLE(1, 0, 3, 2));
                    } else if constexpr (_Amount == 4) {
                        return _mm256_permute4x64_epi64(_Val, _MM_SHUFFLE(1, 0, 3, 2));
                    } else {
                        static_assert(false, "Unexpected amount");
                    }
                }
            };

            template <>
            struct _Find_first_of_traits<uint64_t> {
                static __m256i _Cmp_avx(const __m256i _Lhs, const __m256i _Rhs) noexcept {
                    return _mm256_cmpeq_epi64(_Lhs, _Rhs);
                }

                template <size_t _Amount>
                static __m256i _Spread_avx(const __m256i _Val, const size_t _Needle_length_el) noexcept {
                    if constexpr (_Amount == 0) {
                        return _mm256_undefined_si256();
                    } else if constexpr (_Amount == 1) {
                        return _mm256_broadcastq_epi64(_mm256_castsi256_si128(_Val));
                    } else if constexpr (_Amount == 2) {
                        return _mm256_permute4x64_epi64(_Val, _MM_SHUFFLE(1, 0, 1, 0));
                    } else if constexpr (_Amount == 4) {
                        if (_Needle_length_el < 4) {
                            return _mm256_permute4x64_epi64(_Val, _MM_SHUFFLE(0, 2, 1, 0));
                        }

                        return _Val;
                    } else {
                        static_assert(false, "Unexpected amount");
                    }
                }

                template <size_t _Amount>
                static __m256i _Shuffle_avx(const __m256i _Val) noexcept {
                    if constexpr (_Amount == 1) {
                        return _mm256_shuffle_epi32(_Val, _MM_SHUFFLE(1, 0, 3, 2));
                    } else if constexpr (_Amount == 2) {
                        return _mm256_permute4x64_epi64(_Val, _MM_SHUFFLE(1, 0, 3, 2));
                    } else {
                        static_assert(false, "Unexpected amount");
                    }
                }
            };

            template <class _Traits, size_t _Needle_length_el_magnitude>
            __m256i _Shuffle_step(const __m256i _Data1, const __m256i _Data2s0) noexcept {
                __m256i _Eq = _mm256_setzero_si256();
                if constexpr (_Needle_length_el_magnitude >= 1) {
                    _Eq = _Traits::_Cmp_avx(_Data1, _Data2s0);
                    if constexpr (_Needle_length_el_magnitude >= 2) {
                        const __m256i _Data2s1 = _Traits::template _Shuffle_avx<1>(_Data2s0);
                        _Eq                    = _mm256_or_si256(_Eq, _Traits::_Cmp_avx(_Data1, _Data2s1));
                        if constexpr (_Needle_length_el_magnitude >= 4) {
                            const __m256i _Data2s2 = _Traits::template _Shuffle_avx<2>(_Data2s0);
                            _Eq                    = _mm256_or_si256(_Eq, _Traits::_Cmp_avx(_Data1, _Data2s2));
                            const __m256i _Data2s3 = _Traits::template _Shuffle_avx<1>(_Data2s2);
                            _Eq                    = _mm256_or_si256(_Eq, _Traits::_Cmp_avx(_Data1, _Data2s3));
                            if constexpr (_Needle_length_el_magnitude >= 8) {
                                const __m256i _Data2s4 = _Traits::template _Shuffle_avx<4>(_Data2s0);
                                _Eq                    = _mm256_or_si256(_Eq, _Traits::_Cmp_avx(_Data1, _Data2s4));
                                const __m256i _Data2s5 = _Traits::template _Shuffle_avx<1>(_Data2s4);
                                _Eq                    = _mm256_or_si256(_Eq, _Traits::_Cmp_avx(_Data1, _Data2s5));
                                const __m256i _Data2s6 = _Traits::template _Shuffle_avx<2>(_Data2s4);
                                _Eq                    = _mm256_or_si256(_Eq, _Traits::_Cmp_avx(_Data1, _Data2s6));
                                const __m256i _Data2s7 = _Traits::template _Shuffle_avx<1>(_Data2s6);
                                _Eq                    = _mm256_or_si256(_Eq, _Traits::_Cmp_avx(_Data1, _Data2s7));
                            }
                        }
                    }
                }
                return _Eq;
            }

            template <class _Ty, bool _Large, size_t _Last2_length_el_magnitude>
            const void* _Shuffle_impl(const void* _First1, const size_t _Haystack_length, const void* const _First2,
                const void* const _Stop2, const size_t _Last2_length_el) noexcept {
                using _Traits               = _Find_first_of_traits<_Ty>;
                constexpr size_t _Length_el = 32 / sizeof(_Ty);

                const __m256i _Last2val = _mm256_maskload_epi32(
                    reinterpret_cast<const int*>(_Stop2), _Avx2_tail_mask_32(_Last2_length_el * sizeof(_Ty)));
                const __m256i _Last2s0 =
                    _Traits::template _Spread_avx<_Last2_length_el_magnitude>(_Last2val, _Last2_length_el);

                const void* _Stop1 = _First1;
                _Advance_bytes(_Stop1, _Haystack_length & ~size_t{0x1F});

                for (; _First1 != _Stop1; _Advance_bytes(_First1, 32)) {
                    const __m256i _Data1 = _mm256_loadu_si256(static_cast<const __m256i*>(_First1));
                    __m256i _Eq          = _Shuffle_step<_Traits, _Last2_length_el_magnitude>(_Data1, _Last2s0);

                    if constexpr (_Large) {
                        for (const void* _Ptr2 = _First2; _Ptr2 != _Stop2; _Advance_bytes(_Ptr2, 32)) {
                            const __m256i _Data2s0 = _mm256_loadu_si256(static_cast<const __m256i*>(_Ptr2));
                            _Eq = _mm256_or_si256(_Eq, _Shuffle_step<_Traits, _Length_el>(_Data1, _Data2s0));
                        }
                    }

                    if (const uint32_t _Bingo = _mm256_movemask_epi8(_Eq); _Bingo != 0) {
                        const unsigned long _Offset = _tzcnt_u32(_Bingo);
                        _Advance_bytes(_First1, _Offset);
                        return _First1;
                    }
                }

                if (const size_t _Haystack_tail_length = _Haystack_length & 0x1C; _Haystack_tail_length != 0) {
                    const __m256i _Tail_mask = _Avx2_tail_mask_32(_Haystack_tail_length);
                    const __m256i _Data1     = _mm256_maskload_epi32(static_cast<const int*>(_First1), _Tail_mask);
                    __m256i _Eq              = _Shuffle_step<_Traits, _Last2_length_el_magnitude>(_Data1, _Last2s0);

                    if constexpr (_Large) {
                        for (const void* _Ptr2 = _First2; _Ptr2 != _Stop2; _Advance_bytes(_Ptr2, 32)) {
                            const __m256i _Data2s0 = _mm256_loadu_si256(static_cast<const __m256i*>(_Ptr2));
                            _Eq = _mm256_or_si256(_Eq, _Shuffle_step<_Traits, _Length_el>(_Data1, _Data2s0));
                        }
                    }

                    if (const uint32_t _Bingo = _mm256_movemask_epi8(_mm256_and_si256(_Eq, _Tail_mask)); _Bingo != 0) {
                        const unsigned long _Offset = _tzcnt_u32(_Bingo);
                        _Advance_bytes(_First1, _Offset);
                        return _First1;
                    }

                    _Advance_bytes(_First1, _Haystack_tail_length);
                }

                return _First1;
            }

            template <class _Ty, bool _Large>
            const void* _Shuffle_impl_dispatch_magnitude(const void* const _First1, const size_t _Haystack_length,
                const void* const _First2, const void* const _Stop2, const size_t _Last2_length_el) noexcept {
                if (_Last2_length_el == 0) {
                    return _Shuffle_impl<_Ty, _Large, 0>(_First1, _Haystack_length, _First2, _Stop2, _Last2_length_el);
                } else if (_Last2_length_el == 1) {
                    return _Shuffle_impl<_Ty, _Large, 1>(_First1, _Haystack_length, _First2, _Stop2, _Last2_length_el);
                } else if (_Last2_length_el == 2) {
                    return _Shuffle_impl<_Ty, _Large, 2>(_First1, _Haystack_length, _First2, _Stop2, _Last2_length_el);
                } else if (_Last2_length_el <= 4) {
                    return _Shuffle_impl<_Ty, _Large, 4>(_First1, _Haystack_length, _First2, _Stop2, _Last2_length_el);
                } else if (_Last2_length_el <= 8) {
                    if constexpr (sizeof(_Ty) == 4) {
                        return _Shuffle_impl<_Ty, _Large, 8>(
                            _First1, _Haystack_length, _First2, _Stop2, _Last2_length_el);
                    }
                }

                _STL_UNREACHABLE;
            }

            template <class _Ty>
            const void* _Impl_4_8(const void* const _First1, const size_t _Haystack_length, const void* const _First2,
                const size_t _Needle_length) noexcept {
                _Zeroupper_on_exit _Guard; // TRANSITION, DevCom-10331414

                const size_t _Last_needle_length    = _Needle_length & 0x1F;
                const size_t _Last_needle_length_el = _Last_needle_length / sizeof(_Ty);

                if (const size_t _Needle_length_large = _Needle_length & ~size_t{0x1F}; _Needle_length_large != 0) {
                    const void* _Stop2 = _First2;
                    _Advance_bytes(_Stop2, _Needle_length_large);
                    return _Shuffle_impl_dispatch_magnitude<_Ty, true>(
                        _First1, _Haystack_length, _First2, _Stop2, _Last_needle_length_el);
                } else {
                    return _Shuffle_impl_dispatch_magnitude<_Ty, false>(
                        _First1, _Haystack_length, _First2, _First2, _Last_needle_length_el);
                }
            }
#endif // ^^^ !defined(_M_ARM64EC) ^^^

            template <class _Ty>
            const void* __stdcall _Dispatch_ptr(const void* const _First1, const void* const _Last1,
                const void* const _First2, const void* const _Last2) noexcept {
#ifndef _M_ARM64EC
                if constexpr (sizeof(_Ty) <= 2) {
                    if (_Use_sse42()) {
                        return _Impl_pcmpestri<_Ty, _Predicate::_Any_of>(
                            _First1, _Byte_length(_First1, _Last1), _First2, _Byte_length(_First2, _Last2));
                    }
                } else {
                    if (_Use_avx2()) {
                        return _Impl_4_8<_Ty>(
                            _First1, _Byte_length(_First1, _Last1), _First2, _Byte_length(_First2, _Last2));
                    }
                }
#endif // ^^^ !defined(_M_ARM64EC) ^^^

                return _Fallback<_Ty, _Predicate::_Any_of>(_First1, _Last1, _First2, _Last2);
            }

            template <class _Ty>
            size_t _Pos_from_ptr(
                const void* const _Result, const void* const _First1, const void* const _Last1) noexcept {
                if (_Result != _Last1) {
                    return _Byte_length(_First1, _Result) / sizeof(_Ty);
                } else {
                    return static_cast<size_t>(-1);
                }
            }

#ifndef _M_ARM64EC
            template <class _Ty, _Predicate _Pred>
            size_t _Dispatch_pos_sse_1_2(const void* const _First1, const size_t _Count1, const void* const _First2,
                const size_t _Count2) noexcept {
                using namespace _Bitmap_impl;

                const _Strategy _Strat = _Pick_strategy<_Ty>(_Count1, _Count2, _Use_avx2());

                if (_Strat == _Strategy::_Vector_bitmap) {
                    if (_Can_fit_256_bits_sse(static_cast<const _Ty*>(_First2), _Count2)) {
                        return _Impl_first_avx<_Ty, _Pred>(_First1, _Count1, _First2, _Count2);
                    }
                } else if (_Strat == _Strategy::_Scalar_bitmap) {
                    if (_Can_fit_256_bits_sse(static_cast<const _Ty*>(_First2), _Count2)) {
                        alignas(32) _Scalar_table_t _Table = {};
                        _Build_scalar_table_no_check<_Ty>(_First2, _Count2, _Table);
                        return _Impl_first_scalar<_Ty, _Pred>(_First1, _Count1, _Table);
                    }
                }

                const void* const _Last1   = static_cast<const _Ty*>(_First1) + _Count1;
                const size_t _Size_bytes_1 = _Count1 * sizeof(_Ty);
                const size_t _Size_bytes_2 = _Count2 * sizeof(_Ty);

                return _Pos_from_ptr<_Ty>(
                    _Impl_pcmpestri<_Ty, _Pred>(_First1, _Size_bytes_1, _First2, _Size_bytes_2), _First1, _Last1);
            }

            template <class _Ty>
            size_t _Dispatch_pos_avx_4_8(const void* const _First1, const size_t _Count1, const void* const _First2,
                const size_t _Count2) noexcept {
                using namespace _Bitmap_impl;

                const auto _Strat = _Pick_strategy<_Ty>(_Count1, _Count2, true);

                if (_Strat == _Strategy::_Vector_bitmap) {
                    if (_Can_fit_256_bits_sse(static_cast<const _Ty*>(_First2), _Count2)) {
                        return _Impl_first_avx<_Ty, _Predicate::_Any_of>(_First1, _Count1, _First2, _Count2);
                    }
                } else if (_Strat == _Strategy::_Scalar_bitmap) {
                    if (_Can_fit_256_bits_sse(static_cast<const _Ty*>(_First2), _Count2)) {
                        alignas(32) _Scalar_table_t _Table = {};
                        _Build_scalar_table_no_check<_Ty>(_First2, _Count2, _Table);
                        return _Impl_first_scalar<_Ty, _Predicate::_Any_of>(_First1, _Count1, _Table);
                    }
                }

                const void* const _Last1   = static_cast<const _Ty*>(_First1) + _Count1;
                const size_t _Size_bytes_1 = _Count1 * sizeof(_Ty);
                const size_t _Size_bytes_2 = _Count2 * sizeof(_Ty);

                return _Pos_from_ptr<_Ty>(
                    _Impl_4_8<_Ty>(_First1, _Size_bytes_1, _First2, _Size_bytes_2), _First1, _Last1);
            }
#endif // ^^^ !defined(_M_ARM64EC) ^^^

            template <class _Ty, _Predicate _Pred>
            size_t _Dispatch_pos_fallback(const void* const _First1, const size_t _Count1, const void* const _First2,
                const size_t _Count2) noexcept {
                using namespace _Bitmap_impl;

                _Scalar_table_t _Table = {};
                if (_Build_scalar_table<_Ty>(_First2, _Count2, _Table)) {
                    return _Impl_first_scalar<_Ty, _Pred>(_First1, _Count1, _Table);
                }

                const void* const _Last1 = static_cast<const _Ty*>(_First1) + _Count1;
                const void* const _Last2 = static_cast<const _Ty*>(_First2) + _Count2;

                return _Pos_from_ptr<_Ty>(_Fallback<_Ty, _Pred>(_First1, _Last1, _First2, _Last2), _First1, _Last1);
            }

            template <class _Ty, _Predicate _Pred>
            size_t __stdcall _Dispatch_pos(const void* const _First1, const size_t _Count1, const void* const _First2,
                const size_t _Count2) noexcept {
#ifndef _M_ARM64EC
                if constexpr (sizeof(_Ty) <= 2) {
                    if (_Use_sse42()) {
                        return _Dispatch_pos_sse_1_2<_Ty, _Pred>(_First1, _Count1, _First2, _Count2);
                    }
                } else {
                    if (_Use_avx2()) {
                        static_assert(_Pred == _Predicate::_Any_of);

                        return _Dispatch_pos_avx_4_8<_Ty>(_First1, _Count1, _First2, _Count2);
                    }
                }
#endif // ^^^ !defined(_M_ARM64EC) ^^^
                return _Dispatch_pos_fallback<_Ty, _Pred>(_First1, _Count1, _First2, _Count2);
            }
        } // namespace _First_of

        namespace _Last_of {
            template <class _Ty, _Predicate _Pred>
            size_t __stdcall _Fallback(const void* const _Haystack, const size_t _Haystack_length,
                const void* const _Needle, const size_t _Needle_length) noexcept {

                const auto _Ptr_haystack = static_cast<const _Ty*>(_Haystack);
                size_t _Pos              = _Haystack_length;
                const auto _Needle_end   = static_cast<const _Ty*>(_Needle) + _Needle_length;

                while (_Pos != 0) {
                    --_Pos;

                    if constexpr (_Pred == _Predicate::_Any_of) {
                        for (auto _Ptr = static_cast<const _Ty*>(_Needle); _Ptr != _Needle_end; ++_Ptr) {
                            if (_Ptr_haystack[_Pos] == *_Ptr) {
                                return _Pos;
                            }
                        }
                    } else {
                        bool _Match = false;
                        for (auto _Ptr = static_cast<const _Ty*>(_Needle); _Ptr != _Needle_end; ++_Ptr) {
                            if (_Ptr_haystack[_Pos] == *_Ptr) {
                                _Match = true;
                                break;
                            }
                        }

                        if (!_Match) {
                            return _Pos;
                        }
                    }
                }

                return static_cast<size_t>(-1);
            }

#ifndef _M_ARM64EC
            template <class _Ty, _Predicate _Pred>
            size_t _Impl(const void* const _Haystack, const size_t _Haystack_length, const void* const _Needle,
                const size_t _Needle_length) noexcept {
                const size_t _Haystack_length_bytes = _Haystack_length * sizeof(_Ty);

                constexpr int _Op_base =
                    (_Pred == _Predicate::_Any_of ? _SIDD_POSITIVE_POLARITY : _SIDD_MASKED_NEGATIVE_POLARITY)
                    | (sizeof(_Ty) == 1 ? _SIDD_UBYTE_OPS : _SIDD_UWORD_OPS) | _SIDD_CMP_EQUAL_ANY;
                constexpr int _Op           = _Op_base | _SIDD_MOST_SIGNIFICANT;
                constexpr int _Part_size_el = sizeof(_Ty) == 1 ? 16 : 8;

                const size_t _Last_part_size = _Haystack_length_bytes & 0xF;

                const void* _Stop_at = _Haystack;
                _Advance_bytes(_Stop_at, _Last_part_size);

                const void* _Cur = _Haystack;
                _Advance_bytes(_Cur, _Haystack_length_bytes);

                const size_t _Needle_length_bytes = _Needle_length * sizeof(_Ty);

                if (_Needle_length_bytes <= 16) {
                    // Special handling of small needle
                    // The generic branch could also be modified to handle it, but with slightly worse performance
                    const int _Needle_length_el = static_cast<int>(_Needle_length);

                    alignas(16) uint8_t _Tmp2[16];
                    memcpy(_Tmp2, _Needle, _Needle_length_bytes);
                    const __m128i _Data2 = _mm_load_si128(reinterpret_cast<const __m128i*>(_Tmp2));

                    while (_Cur != _Stop_at) {
                        _Rewind_bytes(_Cur, 16);
                        const __m128i _Data1 = _mm_loadu_si128(static_cast<const __m128i*>(_Cur));
                        if (_mm_cmpestrc(_Data2, _Needle_length_el, _Data1, _Part_size_el, _Op)) {
                            const int _Pos = _mm_cmpestri(_Data2, _Needle_length_el, _Data1, _Part_size_el, _Op);
                            return _Byte_length(_Haystack, _Cur) / sizeof(_Ty) + _Pos;
                        }
                    }

                    if (_Last_part_size != 0) {
                        const int _Last_part_size_el = static_cast<int>(_Last_part_size / sizeof(_Ty));
                        __m128i _Data1;

                        if (_Haystack_length_bytes >= 16) {
                            _Data1 = _mm_loadu_si128(reinterpret_cast<const __m128i*>(_Haystack));
                        } else {
                            alignas(16) uint8_t _Tmp1[16];
                            memcpy(_Tmp1, _Haystack, _Haystack_length_bytes);
                            _Data1 = _mm_load_si128(reinterpret_cast<const __m128i*>(_Tmp1));
                        }

                        if (_mm_cmpestrc(_Data2, _Needle_length_el, _Data1, _Last_part_size_el, _Op)) {
                            return _mm_cmpestri(_Data2, _Needle_length_el, _Data1, _Last_part_size_el, _Op);
                        }
                    }

                    return static_cast<size_t>(-1);
                } else {
                    const void* _Last_needle = _Needle;
                    _Advance_bytes(_Last_needle, _Needle_length_bytes & ~size_t{0xF});

                    const int _Last_needle_length = static_cast<int>(_Needle_length_bytes & 0xF);

                    alignas(16) uint8_t _Tmp2[16];
                    memcpy(_Tmp2, _Last_needle, _Last_needle_length);
                    const __m128i _Last_needle_val   = _mm_load_si128(reinterpret_cast<const __m128i*>(_Tmp2));
                    const int _Last_needle_length_el = _Last_needle_length / sizeof(_Ty);

                    // equal to npos when treated as size_t; also less than any found value
                    constexpr int _Not_found = -1;

#pragma warning(push)
#pragma warning(disable : 4324) // structure was padded due to alignment specifier
                    const auto _Test_whole_needle = [=](const __m128i _Data1, const int _Size1) noexcept {
                        if constexpr (_Pred == _Predicate::_Any_of) {
                            int _Found_pos = _Not_found;

                            const auto _Step = [&_Found_pos](const __m128i _Data2, const int _Size2,
                                                   const __m128i _Data1, const int _Size1) noexcept {
                                if (_mm_cmpestrc(_Data2, _Size2, _Data1, _Size1, _Op)) {
                                    const int _Pos = _mm_cmpestri(_Data2, _Size2, _Data1, _Size1, _Op);
                                    if (_Pos > _Found_pos) {
                                        _Found_pos = _Pos;
                                    }
                                }
                            };

                            const void* _Cur_needle = _Needle;
                            do {
                                const __m128i _Data2 = _mm_loadu_si128(static_cast<const __m128i*>(_Cur_needle));
                                _Step(_Data2, _Part_size_el, _Data1, _Size1);
                                _Advance_bytes(_Cur_needle, 16);
                            } while (_Cur_needle != _Last_needle);

                            if (_Last_needle_length_el != 0) {
                                _Step(_Last_needle_val, _Last_needle_length_el, _Data1, _Size1);
                            }

                            return _Found_pos;
                        } else {
                            constexpr int _Op_mask = _Op_base | _SIDD_BIT_MASK;

                            const void* _Cur_needle = _Needle;

                            const __m128i _Data2_first = _mm_loadu_si128(static_cast<const __m128i*>(_Cur_needle));

                            __m128i _Found = _mm_cmpestrm(_Data2_first, _Part_size_el, _Data1, _Size1, _Op_mask);

                            while (_Cur_needle != _Last_needle) {
                                const __m128i _Data2 = _mm_loadu_si128(static_cast<const __m128i*>(_Cur_needle));
                                const __m128i _Found_part =
                                    _mm_cmpestrm(_Data2, _Part_size_el, _Data1, _Size1, _Op_mask);
                                _Found = _mm_and_si128(_Found, _Found_part);
                                _Advance_bytes(_Cur_needle, 16);
                            }

                            if (_Last_needle_length_el != 0) {
                                const __m128i _Found_part =
                                    _mm_cmpestrm(_Last_needle_val, _Last_needle_length_el, _Data1, _Size1, _Op_mask);
                                _Found = _mm_and_si128(_Found, _Found_part);
                                _Advance_bytes(_Cur_needle, 16);
                            }

                            const unsigned int _Bingo = _mm_cvtsi128_si32(_Found);
                            int _Found_pos            = _Not_found;

                            if (_Bingo != 0) {
                                unsigned long _Tmp;
                                // CodeQL [SM02313] _Tmp is always initialized: we just tested `if (_Bingo != 0)`.
                                _BitScanReverse(&_Tmp, _Bingo);
                                _Found_pos = static_cast<int>(_Tmp);
                            }

                            return _Found_pos;
                        }
                    };
#pragma warning(pop)

                    while (_Cur != _Stop_at) {
                        _Rewind_bytes(_Cur, 16);
                        const int _Found_pos =
                            _Test_whole_needle(_mm_loadu_si128(static_cast<const __m128i*>(_Cur)), _Part_size_el);

                        if (_Found_pos != _Not_found) {
                            return _Byte_length(_Haystack, _Cur) / sizeof(_Ty) + _Found_pos;
                        }
                    }

                    if (_Last_part_size != 0) {
                        const int _Last_part_size_el = static_cast<int>(_Last_part_size / sizeof(_Ty));
                        __m128i _Data1;

                        if (_Haystack_length_bytes >= 16) {
                            _Data1 = _mm_loadu_si128(reinterpret_cast<const __m128i*>(_Haystack));
                        } else {
                            alignas(16) uint8_t _Tmp1[16];
                            memcpy(_Tmp1, _Haystack, _Haystack_length_bytes);
                            _Data1 = _mm_load_si128(reinterpret_cast<const __m128i*>(_Tmp1));
                        }

                        return _Test_whole_needle(_Data1, _Last_part_size_el);
                    }

                    return static_cast<size_t>(_Not_found);
                }
            }
#endif // ^^^ !defined(_M_ARM64EC) ^^^

            template <class _Ty, _Predicate _Pred>
            size_t __stdcall _Dispatch_pos(const void* const _First1, const size_t _Count1, const void* const _First2,
                const size_t _Count2) noexcept {
                using namespace _Bitmap_impl;

#ifndef _M_ARM64EC
                if (_Use_sse42()) {
                    const auto _Strat = _Pick_strategy<_Ty>(_Count1, _Count2, _Use_avx2());

                    if (_Strat == _Strategy::_Vector_bitmap) {
                        if (_Can_fit_256_bits_sse(static_cast<const _Ty*>(_First2), _Count2)) {
                            return _Impl_last_avx<_Ty, _Pred>(_First1, _Count1, _First2, _Count2);
                        }
                    } else if (_Strat == _Strategy::_Scalar_bitmap) {
                        if (_Can_fit_256_bits_sse(static_cast<const _Ty*>(_First2), _Count2)) {
                            alignas(32) _Scalar_table_t _Table = {};
                            _Build_scalar_table_no_check<_Ty>(_First2, _Count2, _Table);
                            return _Impl_last_scalar<_Ty, _Pred>(_First1, _Count1, _Table);
                        }
                    }

                    return _Impl<_Ty, _Pred>(_First1, _Count1, _First2, _Count2);
                } else
#endif // ^^^ !defined(_M_ARM64EC) ^^^
                {
                    alignas(32) _Scalar_table_t _Table = {};
                    if (_Build_scalar_table<_Ty>(_First2, _Count2, _Table)) {
                        return _Impl_last_scalar<_Ty, _Pred>(_First1, _Count1, _Table);
                    }

                    return _Fallback<_Ty, _Pred>(_First1, _Count1, _First2, _Count2);
                }
            }
        } // namespace _Last_of
    } // namespace _Find_meow_of
} // unnamed namespace

extern "C" {

const void* __stdcall __std_find_first_of_trivial_1(
    const void* const _First1, const void* const _Last1, const void* const _First2, const void* const _Last2) noexcept {
    return _Find_meow_of::_First_of::_Dispatch_ptr<uint8_t>(_First1, _Last1, _First2, _Last2);
}

const void* __stdcall __std_find_first_of_trivial_2(
    const void* const _First1, const void* const _Last1, const void* const _First2, const void* const _Last2) noexcept {
    return _Find_meow_of::_First_of::_Dispatch_ptr<uint16_t>(_First1, _Last1, _First2, _Last2);
}

const void* __stdcall __std_find_first_of_trivial_4(
    const void* const _First1, const void* const _Last1, const void* const _First2, const void* const _Last2) noexcept {
    return _Find_meow_of::_First_of::_Dispatch_ptr<uint32_t>(_First1, _Last1, _First2, _Last2);
}

const void* __stdcall __std_find_first_of_trivial_8(
    const void* const _First1, const void* const _Last1, const void* const _First2, const void* const _Last2) noexcept {
    return _Find_meow_of::_First_of::_Dispatch_ptr<uint64_t>(_First1, _Last1, _First2, _Last2);
}

__declspec(noalias) size_t __stdcall __std_find_first_of_trivial_pos_1(const void* const _Haystack,
    const size_t _Haystack_length, const void* const _Needle, const size_t _Needle_length) noexcept {
    return _Find_meow_of::_First_of::_Dispatch_pos<uint8_t, _Find_meow_of::_Predicate::_Any_of>(
        _Haystack, _Haystack_length, _Needle, _Needle_length);
}

__declspec(noalias) size_t __stdcall __std_find_first_of_trivial_pos_2(const void* const _Haystack,
    const size_t _Haystack_length, const void* const _Needle, const size_t _Needle_length) noexcept {
    return _Find_meow_of::_First_of::_Dispatch_pos<uint16_t, _Find_meow_of::_Predicate::_Any_of>(
        _Haystack, _Haystack_length, _Needle, _Needle_length);
}

__declspec(noalias) size_t __stdcall __std_find_first_of_trivial_pos_4(const void* const _Haystack,
    const size_t _Haystack_length, const void* const _Needle, const size_t _Needle_length) noexcept {
    return _Find_meow_of::_First_of::_Dispatch_pos<uint32_t, _Find_meow_of::_Predicate::_Any_of>(
        _Haystack, _Haystack_length, _Needle, _Needle_length);
}

__declspec(noalias) size_t __stdcall __std_find_first_of_trivial_pos_8(const void* const _Haystack,
    const size_t _Haystack_length, const void* const _Needle, const size_t _Needle_length) noexcept {
    return _Find_meow_of::_First_of::_Dispatch_pos<uint64_t, _Find_meow_of::_Predicate::_Any_of>(
        _Haystack, _Haystack_length, _Needle, _Needle_length);
}

__declspec(noalias) size_t __stdcall __std_find_last_of_trivial_pos_1(const void* const _Haystack,
    const size_t _Haystack_length, const void* const _Needle, const size_t _Needle_length) noexcept {
    return _Find_meow_of::_Last_of::_Dispatch_pos<uint8_t, _Find_meow_of::_Predicate::_Any_of>(
        _Haystack, _Haystack_length, _Needle, _Needle_length);
}

__declspec(noalias) size_t __stdcall __std_find_last_of_trivial_pos_2(const void* const _Haystack,
    const size_t _Haystack_length, const void* const _Needle, const size_t _Needle_length) noexcept {
    return _Find_meow_of::_Last_of::_Dispatch_pos<uint16_t, _Find_meow_of::_Predicate::_Any_of>(
        _Haystack, _Haystack_length, _Needle, _Needle_length);
}

__declspec(noalias) size_t __stdcall __std_find_first_not_of_trivial_pos_1(const void* const _Haystack,
    const size_t _Haystack_length, const void* const _Needle, const size_t _Needle_length) noexcept {
    return _Find_meow_of::_First_of::_Dispatch_pos<uint8_t, _Find_meow_of::_Predicate::_None_of>(
        _Haystack, _Haystack_length, _Needle, _Needle_length);
}

__declspec(noalias) size_t __stdcall __std_find_first_not_of_trivial_pos_2(const void* const _Haystack,
    const size_t _Haystack_length, const void* const _Needle, const size_t _Needle_length) noexcept {
    return _Find_meow_of::_First_of::_Dispatch_pos<uint16_t, _Find_meow_of::_Predicate::_None_of>(
        _Haystack, _Haystack_length, _Needle, _Needle_length);
}

__declspec(noalias) size_t __stdcall __std_find_last_not_of_trivial_pos_1(const void* const _Haystack,
    const size_t _Haystack_length, const void* const _Needle, const size_t _Needle_length) noexcept {
    return _Find_meow_of::_Last_of::_Dispatch_pos<uint8_t, _Find_meow_of::_Predicate::_None_of>(
        _Haystack, _Haystack_length, _Needle, _Needle_length);
}

__declspec(noalias) size_t __stdcall __std_find_last_not_of_trivial_pos_2(const void* const _Haystack,
    const size_t _Haystack_length, const void* const _Needle, const size_t _Needle_length) noexcept {
    return _Find_meow_of::_Last_of::_Dispatch_pos<uint16_t, _Find_meow_of::_Predicate::_None_of>(
        _Haystack, _Haystack_length, _Needle, _Needle_length);
}

} // extern "C"

namespace {
    namespace _Find_seq {
#ifdef _M_ARM64EC
        using _Find_seq_traits_avx_1 = void;
        using _Find_seq_traits_avx_2 = void;
        using _Find_seq_traits_avx_4 = void;
        using _Find_seq_traits_avx_8 = void;
        using _Find_seq_traits_sse_4 = void;
        using _Find_seq_traits_sse_8 = void;
#else // ^^^ defined(_M_ARM64EC) / !defined(_M_ARM64EC) vvv
        struct _Find_seq_traits_avx {
            using _Guard = _Zeroupper_on_exit;

            static constexpr size_t _Vec_size = 32;

            static __m256i _Mask(const size_t _Count_in_bytes) noexcept {
                return _Avx2_tail_mask_32(_Count_in_bytes);
            }

            static __m256i _Load(const void* const _Src) noexcept {
                return _mm256_loadu_si256(reinterpret_cast<const __m256i*>(_Src));
            }

            static __m256i _Xor(const __m256i _Val1, const __m256i _Val2) noexcept {
                return _mm256_xor_si256(_Val1, _Val2);
            }

            static bool _TestZ(const __m256i _Val1, const __m256i _Val2) noexcept {
                return _mm256_testz_si256(_Val1, _Val2);
            }

            static unsigned int _Bsf(const unsigned long _Mask) noexcept {
                return _tzcnt_u32(_Mask);
            }

            static unsigned int _Bsr(const unsigned long _Mask) noexcept {
                return 31 - _lzcnt_u32(_Mask);
            }
        };

        struct _Find_seq_traits_avx_1_2 : _Find_seq_traits_avx {
            static __m256i _Load_tail(
                const void* const _Src, const size_t _Size_bytes, __m256i = _mm256_undefined_si256()) noexcept {
                unsigned char _Tmp[32];
                memcpy(_Tmp, _Src, _Size_bytes);
                return _mm256_loadu_si256(reinterpret_cast<const __m256i*>(_Tmp));
            }
        };

        struct _Find_seq_traits_avx_4_8 : _Find_seq_traits_avx {
            static __m256i _Load_tail(const void* const _Src, size_t, const __m256i _Mask) noexcept {
                return _mm256_maskload_epi32(reinterpret_cast<const int*>(_Src), _Mask);
            }

            static __m256i _Load_tail(const void* const _Src, const size_t _Size_bytes) noexcept {
                const __m256i _Mask = _Avx2_tail_mask_32(_Size_bytes);
                return _mm256_maskload_epi32(reinterpret_cast<const int*>(_Src), _Mask);
            }
        };

        struct _Find_seq_traits_avx_1 : _Find_seq_traits_avx_1_2 {
            static __m256i _Broadcast(const __m256i _Data) noexcept {
                return _mm256_broadcastb_epi8(_mm256_castsi256_si128(_Data));
            }

            static unsigned long _Cmp(const __m256i _Lhs, const __m256i _Rhs) noexcept {
                return _mm256_movemask_epi8(_mm256_cmpeq_epi8(_Lhs, _Rhs));
            }
        };

        struct _Find_seq_traits_avx_2 : _Find_seq_traits_avx_1_2 {
            static __m256i _Broadcast(const __m256i _Data) noexcept {
                return _mm256_broadcastw_epi16(_mm256_castsi256_si128(_Data));
            }

            static unsigned long _Cmp(const __m256i _Lhs, const __m256i _Rhs) noexcept {
                return _mm256_movemask_epi8(_mm256_cmpeq_epi16(_Lhs, _Rhs)) & 0x55555555;
            }
        };

        struct _Find_seq_traits_avx_4 : _Find_seq_traits_avx_4_8 {
            static __m256i _Broadcast(const __m256i _Data) noexcept {
                return _mm256_broadcastd_epi32(_mm256_castsi256_si128(_Data));
            }

            static unsigned long _Cmp(const __m256i _Lhs, const __m256i _Rhs) noexcept {
                return _mm256_movemask_epi8(_mm256_cmpeq_epi32(_Lhs, _Rhs)) & 0x11111111;
            }
        };

        struct _Find_seq_traits_avx_8 : _Find_seq_traits_avx_4_8 {
            static __m256i _Broadcast(const __m256i _Data) noexcept {
                return _mm256_broadcastq_epi64(_mm256_castsi256_si128(_Data));
            }

            static unsigned long _Cmp(const __m256i _Lhs, const __m256i _Rhs) noexcept {
                return _mm256_movemask_epi8(_mm256_cmpeq_epi64(_Lhs, _Rhs)) & 0x01010101;
            }
        };

        struct _Find_seq_traits_sse_4_8 {
            using _Guard = char;

            static constexpr size_t _Vec_size = 16;

            static __m128i _Mask(const size_t _Count_in_bytes) noexcept {
                // _Count_in_bytes must be within [0, 16].
                static constexpr unsigned int _Tail_masks[8] = {~0u, ~0u, ~0u, ~0u, 0, 0, 0, 0};
                return _mm_loadu_si128(reinterpret_cast<const __m128i*>(
                    reinterpret_cast<const unsigned char*>(_Tail_masks) + (16 - _Count_in_bytes)));
            }

            static __m128i _Load(const void* const _Src) noexcept {
                return _mm_loadu_si128(reinterpret_cast<const __m128i*>(_Src));
            }

            static __m128i _Xor(const __m128i _Val1, const __m128i _Val2) noexcept {
                return _mm_xor_si128(_Val1, _Val2);
            }

            static bool _TestZ(const __m128i _Val1, const __m128i _Val2) noexcept {
                return _mm_testz_si128(_Val1, _Val2);
            }

            static __m128i _Load_tail(
                const void* const _Src, const size_t _Size_bytes, __m128i = _mm_undefined_si128()) noexcept {
                unsigned char _Tmp[16];
                memcpy(_Tmp, _Src, _Size_bytes);
                return _mm_loadu_si128(reinterpret_cast<const __m128i*>(_Tmp));
            }

            static unsigned int _Bsf(const unsigned long _Mask) noexcept {
                unsigned long _Index;
                // CodeQL [SM02313] _Index is always initialized: we checked _Mask != 0 on every call site
                _BitScanForward(&_Index, _Mask);
                return _Index;
            }

            static unsigned int _Bsr(const unsigned long _Mask) noexcept {
                unsigned long _Index;
                // CodeQL [SM02313] _Index is always initialized: we checked _Mask != 0 on every call site
                _BitScanReverse(&_Index, _Mask);
                return _Index;
            }
        };

        struct _Find_seq_traits_sse_4 : _Find_seq_traits_sse_4_8 {
            static __m128i _Broadcast(const __m128i _Data) noexcept {
                return _mm_shuffle_epi32(_Data, _MM_SHUFFLE(0, 0, 0, 0));
            }

            static unsigned long _Cmp(const __m128i _Lhs, const __m128i _Rhs) noexcept {
                return _mm_movemask_epi8(_mm_cmpeq_epi32(_Lhs, _Rhs)) & 0x1111;
            }
        };

        struct _Find_seq_traits_sse_8 : _Find_seq_traits_sse_4_8 {
            static __m128i _Broadcast(const __m128i _Data) noexcept {
                return _mm_shuffle_epi32(_Data, _MM_SHUFFLE(1, 0, 1, 0));
            }

            static unsigned long _Cmp(const __m128i _Lhs, const __m128i _Rhs) noexcept {
                return _mm_movemask_epi8(_mm_cmpeq_epi64(_Lhs, _Rhs)) & 0x0101;
            }
        };

        template <class _Traits, class _Ty>
        const void* _Search_cmpeq(const void* _First1, const void* const _Last1, const void* const _First2,
            const size_t _Size_bytes_2) noexcept {
            [[maybe_unused]] typename _Traits::_Guard _Guard; // TRANSITION, DevCom-10331414
            const size_t _Size_bytes_1 = _Byte_length(_First1, _Last1);
            constexpr size_t _Vec_size = _Traits::_Vec_size;
            constexpr size_t _Vec_mask = _Vec_size - 1;

            if (_Size_bytes_2 <= _Vec_size) {
                const auto _Mask2  = _Traits::_Mask(_Size_bytes_2);
                const auto _Data2  = _Traits::_Load_tail(_First2, _Size_bytes_2, _Mask2);
                const auto _Start2 = _Traits::_Broadcast(_Data2);

                const void* _Stop1 = _First1;
                _Advance_bytes(_Stop1, _Size_bytes_1 & ~_Vec_mask);
                do {
                    const auto _Data1    = _Traits::_Load(_First1);
                    unsigned long _Bingo = _Traits::_Cmp(_Data1, _Start2);

                    while (_Bingo != 0) {
                        const unsigned int _Pos = _Traits::_Bsf(_Bingo);

                        const void* _Match = _First1;
                        _Advance_bytes(_Match, _Pos);

                        decltype(_Traits::_Load(_Match)) _Cmp;
                        if (const size_t _Left_match = _Byte_length(_Match, _Last1); _Left_match >= _Vec_size) {
                            const auto _Match_val = _Traits::_Load(_Match);
                            _Cmp                  = _Traits::_Xor(_Data2, _Match_val);
                        } else if (_Left_match >= _Size_bytes_2) {
                            const auto _Match_val = _Traits::_Load_tail(_Match, _Left_match);
                            _Cmp                  = _Traits::_Xor(_Data2, _Match_val);
                        } else {
                            break;
                        }

                        if (_Traits::_TestZ(_Cmp, _Mask2)) {
                            return _Match;
                        }

                        _Bingo ^= 1 << _Pos;
                    }

                    _Advance_bytes(_First1, _Vec_size);

                } while (_First1 != _Stop1);

                if (const size_t _Left1 = _Byte_length(_First1, _Last1); _Left1 >= _Size_bytes_2) {
                    const auto _Data1    = _Traits::_Load_tail(_First1, _Left1);
                    unsigned long _Bingo = _Traits::_Cmp(_Data1, _Start2);

                    while (_Bingo != 0) {
                        const unsigned int _Pos = _Traits::_Bsf(_Bingo);

                        if (_Pos > _Left1 - _Size_bytes_2) {
                            break;
                        }

                        const void* _Match = _First1;
                        _Advance_bytes(_Match, _Pos);

                        const size_t _Left_match = _Byte_length(_Match, _Last1);
                        const auto _Match_val    = _Traits::_Load_tail(_Match, _Left_match);
                        const auto _Cmp          = _Traits::_Xor(_Data2, _Match_val);

                        if (_Traits::_TestZ(_Cmp, _Mask2)) {
                            return _Match;
                        }

                        _Bingo ^= 1 << _Pos;
                    }
                }

                return _Last1;
            } else { // _Size_bytes_2 is greater than _Vec_size bytes
                const auto _Data2  = _Traits::_Load(_First2);
                const auto _Start2 = _Traits::_Broadcast(_Data2);

                const size_t _Max_pos = _Size_bytes_1 - _Size_bytes_2;

                const void* _Stop1 = _First1;
                _Advance_bytes(_Stop1, _Max_pos);

                const void* _Tail2 = _First2;
                _Advance_bytes(_Tail2, _Vec_size);

                do {
                    const auto _Data1    = _Traits::_Load(_First1);
                    unsigned long _Bingo = _Traits::_Cmp(_Data1, _Start2);

                    while (_Bingo != 0) {
                        const unsigned int _Pos = _Traits::_Bsf(_Bingo);

                        const void* _Match = _First1;
                        _Advance_bytes(_Match, _Pos);

                        if (_Match > _Stop1) {
                            break; // Oops, doesn't fit
                        }

                        const auto _Match_val = _Traits::_Load(_Match);
                        const auto _Cmp       = _Traits::_Xor(_Data2, _Match_val);

                        if (_Traits::_TestZ(_Cmp, _Cmp)) {
                            const void* _Tail1 = _Match;
                            _Advance_bytes(_Tail1, _Vec_size);

                            if (memcmp(_Tail1, _Tail2, _Size_bytes_2 - _Vec_size) == 0) {
                                return _Match;
                            }
                        }

                        _Bingo ^= 1 << _Pos;
                    }

                    _Advance_bytes(_First1, _Vec_size);

                } while (_First1 <= _Stop1);

                return _Last1;
            }
        }

        template <class _Traits, class _Ty>
        const void* _Find_end_cmpeq(const void* const _First1, const void* const _Last1, const void* const _First2,
            const size_t _Size_bytes_2) noexcept {
            [[maybe_unused]] typename _Traits::_Guard _Guard; // TRANSITION, DevCom-10331414
            const size_t _Size_bytes_1 = _Byte_length(_First1, _Last1);
            constexpr size_t _Vec_size = _Traits::_Vec_size;
            constexpr size_t _Vec_mask = _Vec_size - 1;

            if (_Size_bytes_2 <= _Vec_size) {
                const unsigned int _Needle_fit_mask = (1 << (_Vec_size - _Size_bytes_2 + sizeof(_Ty))) - 1;

                const void* _Stop1 = _First1;
                _Advance_bytes(_Stop1, _Size_bytes_1 & _Vec_mask);

                const auto _Mask2  = _Traits::_Mask(_Size_bytes_2);
                const auto _Data2  = _Traits::_Load_tail(_First2, _Size_bytes_2, _Mask2);
                const auto _Start2 = _Traits::_Broadcast(_Data2);

                const void* _Mid1 = _Last1;
                _Rewind_bytes(_Mid1, _Vec_size);

#pragma warning(push)
#pragma warning(disable : 4324) // structure was padded due to alignment specifier
                const auto _Check_first = [=, &_Mid1](unsigned long _Match) noexcept {
                    while (_Match != 0) {
                        const unsigned int _Pos = _Traits::_Bsr(_Match);

                        const void* _Tmp1 = _Mid1;
                        _Advance_bytes(_Tmp1, _Pos);

                        const auto _Match_data = _Traits::_Load_tail(_Tmp1, _Byte_length(_Tmp1, _Last1));
                        const auto _Cmp_result = _Traits::_Xor(_Data2, _Match_data);

                        if (_Traits::_TestZ(_Cmp_result, _Mask2)) {
                            _Mid1 = _Tmp1;
                            return true;
                        }

                        _Match ^= 1 << _Pos;
                    }

                    return false;
                };

                const auto _Check = [=, &_Mid1](unsigned long _Match) noexcept {
                    while (_Match != 0) {
                        const unsigned int _Pos = _Traits::_Bsr(_Match);

                        const void* _Tmp1 = _Mid1;
                        _Advance_bytes(_Tmp1, _Pos);

                        const auto _Match_data = _Traits::_Load(_Tmp1);
                        const auto _Cmp_result = _Traits::_Xor(_Data2, _Match_data);

                        if (_Traits::_TestZ(_Cmp_result, _Mask2)) {
                            _Mid1 = _Tmp1;
                            return true;
                        }

                        _Match ^= 1 << _Pos;
                    }

                    return false;
                };
#pragma warning(pop)

                // The very last part, for any match needle should fit, otherwise false match
                const auto _Data1_last              = _Traits::_Load(_Mid1);
                const unsigned long _Match_last_val = _Traits::_Cmp(_Data1_last, _Start2);
                if (_Check_first(_Match_last_val & _Needle_fit_mask)) {
                    return _Mid1;
                }

                // The middle part, fit and unfit needle
                while (_Mid1 != _Stop1) {
                    _Rewind_bytes(_Mid1, _Vec_size);
                    const auto _Data1              = _Traits::_Load(_Mid1);
                    const unsigned long _Match_val = _Traits::_Cmp(_Data1, _Start2);
                    if (_Check(_Match_val)) {
                        return _Mid1;
                    }
                }

                // The first part, fit and unfit needle, mask out already processed positions
                if (const size_t _Tail_bytes_1 = _Size_bytes_1 & _Vec_mask; _Tail_bytes_1 != 0) {
                    _Mid1                          = _First1;
                    const auto _Data1              = _Traits::_Load(_Mid1);
                    const unsigned long _Match_val = _Traits::_Cmp(_Data1, _Start2);
                    if (_Match_val != 0 && _Check(_Match_val & ((1 << _Tail_bytes_1) - 1))) {
                        return _Mid1;
                    }
                }

                return _Last1;
            } else { // _Size_bytes_2 is greater than _Vec_size bytes
                const auto _Data2  = _Traits::_Load(_First2);
                const auto _Start2 = _Traits::_Broadcast(_Data2);

                const void* _Tail2 = _First2;
                _Advance_bytes(_Tail2, _Vec_size);

                const void* _Mid1 = _Last1;
                _Rewind_bytes(_Mid1, _Size_bytes_2);

                const size_t _Size_diff_bytes = _Size_bytes_1 - _Size_bytes_2;
                const void* _Stop1            = _First1;
                _Advance_bytes(_Stop1, _Size_diff_bytes & _Vec_mask);

#pragma warning(push)
#pragma warning(disable : 4324) // structure was padded due to alignment specifier
                const auto _Check = [=, &_Mid1](unsigned long _Match) noexcept {
                    while (_Match != 0) {
                        const unsigned int _Pos = _Traits::_Bsr(_Match);

                        const void* _Tmp1 = _Mid1;
                        _Advance_bytes(_Tmp1, _Pos);

                        const auto _Match_data = _Traits::_Load(_Tmp1);
                        const auto _Cmp_result = _Traits::_Xor(_Data2, _Match_data);

                        if (_Traits::_TestZ(_Cmp_result, _Cmp_result)) {
                            const void* _Tail1 = _Tmp1;
                            _Advance_bytes(_Tail1, _Vec_size);

                            if (memcmp(_Tail1, _Tail2, _Size_bytes_2 - _Vec_size) == 0) {
                                _Mid1 = _Tmp1;
                                return true;
                            }
                        }

                        _Match ^= 1 << _Pos;
                    }

                    return false;
                };
#pragma warning(pop)
                // The very last part, just compare, as true match must start with first symbol
                const auto _Data1_last = _Traits::_Load(_Mid1);
                const auto _Match_last = _Traits::_Xor(_Data2, _Data1_last);

                if (_Traits::_TestZ(_Match_last, _Match_last)) {
                    // Matched _Vec_size bytes, check the rest
                    const void* _Tail1 = _Mid1;
                    _Advance_bytes(_Tail1, _Vec_size);

                    if (memcmp(_Tail1, _Tail2, _Size_bytes_2 - _Vec_size) == 0) {
                        return _Mid1;
                    }
                }

                // The main part, match all characters
                while (_Mid1 != _Stop1) {
                    _Rewind_bytes(_Mid1, _Vec_size);

                    const auto _Data1              = _Traits::_Load(_Mid1);
                    const unsigned long _Match_val = _Traits::_Cmp(_Data1, _Start2);
                    if (_Check(_Match_val)) {
                        return _Mid1;
                    }
                }

                // The first part, mask out already processed positions
                if (const size_t _Tail_bytes_1 = _Size_diff_bytes & _Vec_mask; _Tail_bytes_1 != 0) {
                    _Mid1                          = _First1;
                    const auto _Data1              = _Traits::_Load(_Mid1);
                    const unsigned long _Match_val = _Traits::_Cmp(_Data1, _Start2);
                    if (_Match_val != 0 && _Check(_Match_val & ((1 << _Tail_bytes_1) - 1))) {
                        return _Mid1;
                    }
                }

                return _Last1;
            }
        }
#endif // ^^^ !defined(_M_ARM64EC) ^^^

        template <class _FindTraits, class _Traits_avx, class _Traits_sse, class _Ty>
        const void* __stdcall _Search_impl(
            const void* _First1, const void* const _Last1, const void* const _First2, const size_t _Count2) noexcept {
            if (_Count2 == 0) {
                return _First1;
            }

            if (_Count2 == 1) {
                return _Finding::_Find_impl<_FindTraits, _Finding::_Predicate::_Equal>(
                    _First1, _Last1, *static_cast<const _Ty*>(_First2));
            }

            const size_t _Size_bytes_1 = _Byte_length(_First1, _Last1);
            const size_t _Size_bytes_2 = _Count2 * sizeof(_Ty);

            if (_Size_bytes_1 < _Size_bytes_2) {
                return _Last1;
            }

#ifndef _M_ARM64EC
            // The AVX2 path for 8-bit elements is not necessarily more efficient than the SSE4.2 cmpestri path
            if constexpr (sizeof(_Ty) != 1) {
                if (_Use_avx2() && _Size_bytes_1 >= 32) {
                    return _Search_cmpeq<_Traits_avx, _Ty>(_First1, _Last1, _First2, _Size_bytes_2);
                }
            }

            if (_Use_sse42() && _Size_bytes_1 >= 16) {
                if constexpr (sizeof(_Ty) >= 4) {
                    return _Search_cmpeq<_Traits_sse, _Ty>(_First1, _Last1, _First2, _Size_bytes_2);
                } else {
                    constexpr int _Op =
                        (sizeof(_Ty) == 1 ? _SIDD_UBYTE_OPS : _SIDD_UWORD_OPS) | _SIDD_CMP_EQUAL_ORDERED;
                    constexpr int _Part_size_el = sizeof(_Ty) == 1 ? 16 : 8;

                    if (_Size_bytes_2 <= 16) {
                        const int _Size_el_2 = static_cast<int>(_Size_bytes_2 / sizeof(_Ty));

                        const int _Max_full_match_pos = _Part_size_el - _Size_el_2;

                        alignas(16) uint8_t _Tmp2[16];
                        memcpy(_Tmp2, _First2, _Size_bytes_2);
                        const __m128i _Data2 = _mm_load_si128(reinterpret_cast<const __m128i*>(_Tmp2));

                        const void* _Stop1 = _First1;
                        _Advance_bytes(_Stop1, _Size_bytes_1 - 16);

                        do {
                            const __m128i _Data1 = _mm_loadu_si128(static_cast<const __m128i*>(_First1));

                            if (!_mm_cmpestrc(_Data2, _Size_el_2, _Data1, _Part_size_el, _Op)) {
                                _Advance_bytes(_First1, 16); // No matches, next.
                            } else {
                                const int _Pos = _mm_cmpestri(_Data2, _Size_el_2, _Data1, _Part_size_el, _Op);
                                _Advance_bytes(_First1, _Pos * sizeof(_Ty));
                                if (_Pos <= _Max_full_match_pos) {
                                    // Full match. Return this match.
                                    return _First1;
                                }
                                // Partial match. Search again from the match start. Will return it if it is full.
                            }
                        } while (_First1 <= _Stop1);

                        const size_t _Size_bytes_1_tail = _Byte_length(_First1, _Last1);
                        if (_Size_bytes_1_tail != 0) {
                            const int _Size_el_1_tail = static_cast<int>(_Size_bytes_1_tail / sizeof(_Ty));

                            alignas(16) uint8_t _Tmp1[16];
                            memcpy(_Tmp1, _First1, _Size_bytes_1_tail);
                            const __m128i _Data1 = _mm_load_si128(reinterpret_cast<const __m128i*>(_Tmp1));

                            if (_mm_cmpestrc(_Data2, _Size_el_2, _Data1, _Size_el_1_tail, _Op)) {
                                const int _Pos = _mm_cmpestri(_Data2, _Size_el_2, _Data1, _Size_el_1_tail, _Op);
                                _Advance_bytes(_First1, _Pos * sizeof(_Ty));
                                // Full match because size is less than 16. Return this match.
                                return _First1;
                            }
                        }

                        return _Last1;
                    } else { // _Size_bytes_2 is greater than 16 bytes
                        const __m128i _Data2  = _mm_loadu_si128(reinterpret_cast<const __m128i*>(_First2));
                        const size_t _Max_pos = _Size_bytes_1 - _Size_bytes_2;

                        const void* _Stop1 = _First1;
                        _Advance_bytes(_Stop1, _Max_pos);

                        const void* _Tail2 = _First2;
                        _Advance_bytes(_Tail2, 16);

                        do {
                            const __m128i _Data1 = _mm_loadu_si128(static_cast<const __m128i*>(_First1));
                            if (!_mm_cmpestrc(_Data2, _Part_size_el, _Data1, _Part_size_el, _Op)) {
                                _Advance_bytes(_First1, 16); // No matches, next.
                            } else {
                                const int _Pos = _mm_cmpestri(_Data2, _Part_size_el, _Data1, _Part_size_el, _Op);

                                bool _Match_1st_16 = true;

                                if (_Pos != 0) {
                                    _Advance_bytes(_First1, _Pos * sizeof(_Ty));

                                    if (_First1 > _Stop1) {
                                        break; // Oops, doesn't fit
                                    }

                                    // Match not from the first byte, check 16 symbols
                                    const __m128i _Match1 = _mm_loadu_si128(static_cast<const __m128i*>(_First1));
                                    const __m128i _Cmp    = _mm_xor_si128(_Data2, _Match1);
                                    if (!_mm_testz_si128(_Cmp, _Cmp)) {
                                        _Match_1st_16 = false;
                                    }
                                }

                                if (_Match_1st_16) {
                                    const void* _Tail1 = _First1;
                                    _Advance_bytes(_Tail1, 16);

                                    if (memcmp(_Tail1, _Tail2, _Size_bytes_2 - 16) == 0) {
                                        return _First1;
                                    }
                                }

                                // Start from the next element
                                _Advance_bytes(_First1, sizeof(_Ty));
                            }
                        } while (_First1 <= _Stop1);

                        return _Last1;
                    }
                }
            }
#endif // ^^^ !defined(_M_ARM64EC) ^^^

            const size_t _Max_pos = _Size_bytes_1 - _Size_bytes_2 + sizeof(_Ty);

            auto _Ptr1         = static_cast<const _Ty*>(_First1);
            const auto _Ptr2   = static_cast<const _Ty*>(_First2);
            const void* _Stop1 = _Ptr1;
            _Advance_bytes(_Stop1, _Max_pos);

            for (; _Ptr1 != _Stop1; ++_Ptr1) {
                if (*_Ptr1 != *_Ptr2) {
                    continue;
                }

                bool _Equal = true;

                for (size_t _Idx = 1; _Idx != _Count2; ++_Idx) {
                    if (_Ptr1[_Idx] != _Ptr2[_Idx]) {
                        _Equal = false;
                        break;
                    }
                }

                if (_Equal) {
                    return _Ptr1;
                }
            }

            return _Last1;
        }

        template <class _FindTraits, class _Traits_avx, class _Traits_sse, class _Ty>
        const void* __stdcall _Find_end_impl(const void* const _First1, const void* const _Last1,
            const void* const _First2, const size_t _Count2) noexcept {
            if (_Count2 == 0) {
                return _Last1;
            }

            if (_Count2 == 1) {
                return _Finding::_Find_last_impl<_FindTraits, _Finding::_Predicate::_Equal>(
                    _First1, _Last1, *static_cast<const _Ty*>(_First2));
            }

            const size_t _Size_bytes_1 = _Byte_length(_First1, _Last1);
            const size_t _Size_bytes_2 = _Count2 * sizeof(_Ty);

            if (_Size_bytes_1 < _Size_bytes_2) {
                return _Last1;
            }

#ifndef _M_ARM64EC
            if (_Use_avx2() && _Size_bytes_1 >= 32) {
                return _Find_end_cmpeq<_Traits_avx, _Ty>(_First1, _Last1, _First2, _Size_bytes_2);
            }

            if (_Use_sse42() && _Size_bytes_1 >= 16) {
                if constexpr (sizeof(_Ty) >= 4) {
                    return _Find_end_cmpeq<_Traits_sse, _Ty>(_First1, _Last1, _First2, _Size_bytes_2);
                } else {
                    constexpr int _Op =
                        (sizeof(_Ty) == 1 ? _SIDD_UBYTE_OPS : _SIDD_UWORD_OPS) | _SIDD_CMP_EQUAL_ORDERED;
                    constexpr int _Part_size_el = sizeof(_Ty) == 1 ? 16 : 8;

                    static constexpr int8_t _Low_part_mask[] = {//
                        -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, //
                        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

                    if (_Size_bytes_2 <= 16) {
                        const int _Size_el_2                  = static_cast<int>(_Count2);
                        constexpr unsigned int _Whole_mask    = (1 << _Part_size_el) - 1;
                        const unsigned int _Needle_fit_mask   = (1 << (_Part_size_el - _Size_el_2 + 1)) - 1;
                        const unsigned int _Needle_unfit_mask = _Whole_mask ^ _Needle_fit_mask;

                        const void* _Stop1 = _First1;
                        _Advance_bytes(_Stop1, _Size_bytes_1 & 0xF);

                        alignas(16) uint8_t _Tmp2[16];
                        memcpy(_Tmp2, _First2, _Size_bytes_2);
                        const __m128i _Data2 = _mm_load_si128(reinterpret_cast<const __m128i*>(_Tmp2));

                        const void* _Mid1 = _Last1;
                        _Rewind_bytes(_Mid1, 16);

                        const auto _Check_fit = [&_Mid1, _Needle_fit_mask](const unsigned int _Match) noexcept {
                            const unsigned int _Fit_match = _Match & _Needle_fit_mask;
                            if (_Fit_match != 0) {
                                unsigned long _Match_last_pos;

                                // CodeQL [SM02313] Result is always initialized: we just tested that _Fit_match != 0.
                                _BitScanReverse(&_Match_last_pos, _Fit_match);

                                _Advance_bytes(_Mid1, _Match_last_pos * sizeof(_Ty));
                                return true;
                            }

                            return false;
                        };

#pragma warning(push)
#pragma warning(disable : 4324) // structure was padded due to alignment specifier
                        const auto _Check_unfit = [=, &_Mid1](const unsigned int _Match) noexcept {
                            unsigned long _Unfit_match = _Match & _Needle_unfit_mask;
                            while (_Unfit_match != 0) {
                                const void* _Tmp1 = _Mid1;
                                unsigned long _Match_last_pos;

                                // CodeQL [SM02313] Result is always initialized: we just tested that _Unfit_match != 0.
                                _BitScanReverse(&_Match_last_pos, _Unfit_match);

                                _Advance_bytes(_Tmp1, _Match_last_pos * sizeof(_Ty));

                                const __m128i _Match_data = _mm_loadu_si128(reinterpret_cast<const __m128i*>(_Tmp1));
                                const __m128i _Cmp_result = _mm_xor_si128(_Data2, _Match_data);
                                const __m128i _Data_mask  = _mm_loadu_si128(
                                    reinterpret_cast<const __m128i*>(_Low_part_mask + 16 - _Size_bytes_2));

                                if (_mm_testz_si128(_Cmp_result, _Data_mask)) {
                                    _Mid1 = _Tmp1;
                                    return true;
                                }

                                _Unfit_match ^= 1 << _Match_last_pos;
                            }

                            return false;
                        };
#pragma warning(pop)

                        // TRANSITION, DevCom-10689455, the code below could test with _mm_cmpestrc,
                        // if it has been fused with _mm_cmpestrm.

                        // The very last part, for any match needle should fit, otherwise false match
                        const __m128i _Data1_last = _mm_loadu_si128(reinterpret_cast<const __m128i*>(_Mid1));
                        const auto _Match_last    = _mm_cmpestrm(_Data2, _Size_el_2, _Data1_last, _Part_size_el, _Op);
                        const unsigned int _Match_last_val = _mm_cvtsi128_si32(_Match_last);
                        if (_Check_fit(_Match_last_val)) {
                            return _Mid1;
                        }

                        // The middle part, fit and unfit needle
                        while (_Mid1 != _Stop1) {
                            _Rewind_bytes(_Mid1, 16);
                            const __m128i _Data1 = _mm_loadu_si128(reinterpret_cast<const __m128i*>(_Mid1));
                            const auto _Match    = _mm_cmpestrm(_Data2, _Size_el_2, _Data1, _Part_size_el, _Op);
                            const unsigned int _Match_val = _mm_cvtsi128_si32(_Match);
                            if (_Match_val != 0 && (_Check_unfit(_Match_val) || _Check_fit(_Match_val))) {
                                return _Mid1;
                            }
                        }

                        // The first part, fit and unfit needle, mask out already processed positions
                        if (const size_t _Tail_bytes_1 = _Size_bytes_1 & 0xF; _Tail_bytes_1 != 0) {
                            _Mid1                   = _First1;
                            const __m128i _Data1    = _mm_loadu_si128(reinterpret_cast<const __m128i*>(_Mid1));
                            const auto _Match       = _mm_cmpestrm(_Data2, _Size_el_2, _Data1, _Part_size_el, _Op);
                            const size_t _Tail_el_1 = _Tail_bytes_1 / sizeof(_Ty);
                            const unsigned int _Match_val = _mm_cvtsi128_si32(_Match) & ((1 << _Tail_el_1) - 1);
                            if (_Match_val != 0 && (_Check_unfit(_Match_val) || _Check_fit(_Match_val))) {
                                return _Mid1;
                            }
                        }

                        return _Last1;
                    } else { // _Size_bytes_2 is greater than 16 bytes
                        const __m128i _Data2 = _mm_loadu_si128(reinterpret_cast<const __m128i*>(_First2));

                        const void* _Tail2 = _First2;
                        _Advance_bytes(_Tail2, 16);

                        const void* _Mid1 = _Last1;
                        _Rewind_bytes(_Mid1, _Size_bytes_2);

                        const size_t _Size_diff_bytes = _Size_bytes_1 - _Size_bytes_2;
                        const void* _Stop1            = _First1;
                        _Advance_bytes(_Stop1, _Size_diff_bytes & 0xF);

#pragma warning(push)
#pragma warning(disable : 4324) // structure was padded due to alignment specifier
                        const auto _Check = [=, &_Mid1](unsigned long _Match) noexcept {
                            while (_Match != 0) {
                                const void* _Tmp1 = _Mid1;
                                unsigned long _Match_last_pos;

                                // CodeQL [SM02313] Result is always initialized: we just tested that _Match != 0.
                                _BitScanReverse(&_Match_last_pos, _Match);

                                bool _Match_1st_16 = true;

                                if (_Match_last_pos != 0) {
                                    _Advance_bytes(_Tmp1, _Match_last_pos * sizeof(_Ty));

                                    const __m128i _Match_data =
                                        _mm_loadu_si128(reinterpret_cast<const __m128i*>(_Tmp1));
                                    const __m128i _Cmp_result = _mm_xor_si128(_Data2, _Match_data);

                                    if (!_mm_testz_si128(_Cmp_result, _Cmp_result)) {
                                        _Match_1st_16 = false;
                                    }
                                }

                                if (_Match_1st_16) {
                                    const void* _Tail1 = _Tmp1;
                                    _Advance_bytes(_Tail1, 16);

                                    if (memcmp(_Tail1, _Tail2, _Size_bytes_2 - 16) == 0) {
                                        _Mid1 = _Tmp1;
                                        return true;
                                    }
                                }

                                _Match ^= 1 << _Match_last_pos;
                            }

                            return false;
                        };
#pragma warning(pop)
                        // The very last part, just compare, as true match must start with first symbol
                        const __m128i _Data1_last = _mm_loadu_si128(reinterpret_cast<const __m128i*>(_Mid1));
                        const __m128i _Match_last = _mm_xor_si128(_Data2, _Data1_last);
                        if (_mm_testz_si128(_Match_last, _Match_last)) {
                            // Matched 16 bytes, check the rest
                            const void* _Tail1 = _Mid1;
                            _Advance_bytes(_Tail1, 16);

                            if (memcmp(_Tail1, _Tail2, _Size_bytes_2 - 16) == 0) {
                                return _Mid1;
                            }
                        }

                        // TRANSITION, DevCom-10689455, the code below could test with _mm_cmpestrc,
                        // if it has been fused with _mm_cmpestrm.

                        // The main part, match all characters
                        while (_Mid1 != _Stop1) {
                            _Rewind_bytes(_Mid1, 16);

                            const __m128i _Data1 = _mm_loadu_si128(reinterpret_cast<const __m128i*>(_Mid1));
                            const auto _Match    = _mm_cmpestrm(_Data2, _Part_size_el, _Data1, _Part_size_el, _Op);
                            const unsigned int _Match_val = _mm_cvtsi128_si32(_Match);
                            if (_Match_val != 0 && _Check(_Match_val)) {
                                return _Mid1;
                            }
                        }

                        // The first part, mask out already processed positions
                        if (const size_t _Tail_bytes_1 = _Size_diff_bytes & 0xF; _Tail_bytes_1 != 0) {
                            _Mid1                   = _First1;
                            const __m128i _Data1    = _mm_loadu_si128(reinterpret_cast<const __m128i*>(_Mid1));
                            const auto _Match       = _mm_cmpestrm(_Data2, _Part_size_el, _Data1, _Part_size_el, _Op);
                            const size_t _Tail_el_1 = _Tail_bytes_1 / sizeof(_Ty);
                            const unsigned int _Match_val = _mm_cvtsi128_si32(_Match) & ((1 << _Tail_el_1) - 1);
                            if (_Match_val != 0 && _Check(_Match_val)) {
                                return _Mid1;
                            }
                        }

                        return _Last1;
                    }
                }
            }
#endif // ^^^ !defined(_M_ARM64EC) ^^^
            auto _Ptr1       = static_cast<const _Ty*>(_Last1) - _Count2;
            const auto _Ptr2 = static_cast<const _Ty*>(_First2);

            for (;;) {
                if (*_Ptr1 == *_Ptr2) {
                    bool _Equal = true;

                    for (size_t _Idx = 1; _Idx != _Count2; ++_Idx) {
                        if (_Ptr1[_Idx] != _Ptr2[_Idx]) {
                            _Equal = false;
                            break;
                        }
                    }

                    if (_Equal) {
                        return _Ptr1;
                    }
                }

                if (_Ptr1 == _First1) {
                    return _Last1;
                }

                --_Ptr1;
            }
        }
    } // namespace _Find_seq
} // unnamed namespace

extern "C" {

const void* __stdcall __std_search_1(
    const void* const _First1, const void* const _Last1, const void* const _First2, const size_t _Count2) noexcept {
    return _Find_seq::_Search_impl<_Finding::_Find_traits_1, void, void, uint8_t>(_First1, _Last1, _First2, _Count2);
}

const void* __stdcall __std_search_2(
    const void* const _First1, const void* const _Last1, const void* const _First2, const size_t _Count2) noexcept {
    return _Find_seq::_Search_impl<_Finding::_Find_traits_2, _Find_seq::_Find_seq_traits_avx_2, void, uint16_t>(
        _First1, _Last1, _First2, _Count2);
}

const void* __stdcall __std_search_4(
    const void* const _First1, const void* const _Last1, const void* const _First2, const size_t _Count2) noexcept {
    return _Find_seq::_Search_impl<_Finding::_Find_traits_4, _Find_seq::_Find_seq_traits_avx_4,
        _Find_seq::_Find_seq_traits_sse_4, uint32_t>(_First1, _Last1, _First2, _Count2);
}

const void* __stdcall __std_search_8(
    const void* const _First1, const void* const _Last1, const void* const _First2, const size_t _Count2) noexcept {
    return _Find_seq::_Search_impl<_Finding::_Find_traits_8, _Find_seq::_Find_seq_traits_avx_8,
        _Find_seq::_Find_seq_traits_sse_8, uint64_t>(_First1, _Last1, _First2, _Count2);
}


const void* __stdcall __std_find_end_1(
    const void* const _First1, const void* const _Last1, const void* const _First2, const size_t _Count2) noexcept {
    return _Find_seq::_Find_end_impl<_Finding::_Find_traits_1, _Find_seq::_Find_seq_traits_avx_1, void, uint8_t>(
        _First1, _Last1, _First2, _Count2);
}

const void* __stdcall __std_find_end_2(
    const void* const _First1, const void* const _Last1, const void* const _First2, const size_t _Count2) noexcept {
    return _Find_seq::_Find_end_impl<_Finding::_Find_traits_2, _Find_seq::_Find_seq_traits_avx_2, void, uint16_t>(
        _First1, _Last1, _First2, _Count2);
}

const void* __stdcall __std_find_end_4(
    const void* const _First1, const void* const _Last1, const void* const _First2, const size_t _Count2) noexcept {
    return _Find_seq::_Find_end_impl<_Finding::_Find_traits_4, _Find_seq::_Find_seq_traits_avx_4,
        _Find_seq::_Find_seq_traits_sse_4, uint32_t>(_First1, _Last1, _First2, _Count2);
}

const void* __stdcall __std_find_end_8(
    const void* const _First1, const void* const _Last1, const void* const _First2, const size_t _Count2) noexcept {
    return _Find_seq::_Find_end_impl<_Finding::_Find_traits_8, _Find_seq::_Find_seq_traits_avx_8,
        _Find_seq::_Find_seq_traits_sse_8, uint64_t>(_First1, _Last1, _First2, _Count2);
}

} // extern "C"

namespace {
    namespace _Mismatching {
        template <class _Ty>
        __declspec(noalias) size_t __stdcall _Mismatch_impl(
            const void* const _First1, const void* const _First2, const size_t _Count) noexcept {
            size_t _Result = 0;
#ifndef _M_ARM64EC
            const auto _First1_ch = static_cast<const char*>(_First1);
            const auto _First2_ch = static_cast<const char*>(_First2);

            if (_Use_avx2()) {
                _Zeroupper_on_exit _Guard; // TRANSITION, DevCom-10331414

                const size_t _Count_bytes          = _Count * sizeof(_Ty);
                const size_t _Count_bytes_avx_full = _Count_bytes & ~size_t{0x1F};

                for (; _Result != _Count_bytes_avx_full; _Result += 0x20) {
                    const __m256i _Elem1 = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(_First1_ch + _Result));
                    const __m256i _Elem2 = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(_First2_ch + _Result));
                    const __m256i _Cmp   = _mm256_cmpeq_epi8(_Elem1, _Elem2);
                    const auto _Bingo    = ~static_cast<unsigned int>(_mm256_movemask_epi8(_Cmp));
                    if (_Bingo != 0) {
                        return (_Result + _tzcnt_u32(_Bingo)) / sizeof(_Ty);
                    }
                }

                const size_t _Count_tail = _Count_bytes & size_t{0x1C};

                if (_Count_tail != 0) {
                    const __m256i _Tail_mask = _Avx2_tail_mask_32(_Count_tail);
                    const __m256i _Elem1 =
                        _mm256_maskload_epi32(reinterpret_cast<const int*>(_First1_ch + _Result), _Tail_mask);
                    const __m256i _Elem2 =
                        _mm256_maskload_epi32(reinterpret_cast<const int*>(_First2_ch + _Result), _Tail_mask);

                    const __m256i _Cmp = _mm256_cmpeq_epi8(_Elem1, _Elem2);
                    const auto _Bingo  = ~static_cast<unsigned int>(_mm256_movemask_epi8(_Cmp));
                    if (_Bingo != 0) {
                        return (_Result + _tzcnt_u32(_Bingo)) / sizeof(_Ty);
                    }

                    _Result += _Count_tail;
                }

                _Result /= sizeof(_Ty);

                if constexpr (sizeof(_Ty) >= 4) {
                    return _Result;
                }
            } else if (_Use_sse42()) {
                const size_t _Count_bytes_sse = (_Count * sizeof(_Ty)) & ~size_t{0xF};

                for (; _Result != _Count_bytes_sse; _Result += 0x10) {
                    const __m128i _Elem1 = _mm_loadu_si128(reinterpret_cast<const __m128i*>(_First1_ch + _Result));
                    const __m128i _Elem2 = _mm_loadu_si128(reinterpret_cast<const __m128i*>(_First2_ch + _Result));
                    const __m128i _Cmp   = _mm_cmpeq_epi8(_Elem1, _Elem2);
                    const auto _Bingo    = static_cast<unsigned int>(_mm_movemask_epi8(_Cmp)) ^ 0xFFFF;
                    if (_Bingo != 0) {
                        unsigned long _Offset;
                        // CodeQL [SM02313] _Offset is always initialized: we just tested `if (_Bingo != 0)`.
                        _BitScanForward(&_Offset, _Bingo);
                        return (_Result + _Offset) / sizeof(_Ty);
                    }
                }

                _Result /= sizeof(_Ty);
            }
#endif // ^^^ !defined(_M_ARM64EC) ^^^
            const auto _First1_el = static_cast<const _Ty*>(_First1);
            const auto _First2_el = static_cast<const _Ty*>(_First2);

            for (; _Result != _Count; ++_Result) {
                if (_First1_el[_Result] != _First2_el[_Result]) {
                    break;
                }
            }

            return _Result;
        }
    } // namespace _Mismatching
} // unnamed namespace

extern "C" {

__declspec(noalias) size_t __stdcall __std_mismatch_1(
    const void* const _First1, const void* const _First2, const size_t _Count) noexcept {
    return _Mismatching::_Mismatch_impl<uint8_t>(_First1, _First2, _Count);
}

__declspec(noalias) size_t __stdcall __std_mismatch_2(
    const void* const _First1, const void* const _First2, const size_t _Count) noexcept {
    return _Mismatching::_Mismatch_impl<uint16_t>(_First1, _First2, _Count);
}

__declspec(noalias) size_t __stdcall __std_mismatch_4(
    const void* const _First1, const void* const _First2, const size_t _Count) noexcept {
    return _Mismatching::_Mismatch_impl<uint32_t>(_First1, _First2, _Count);
}

__declspec(noalias) size_t __stdcall __std_mismatch_8(
    const void* const _First1, const void* const _First2, const size_t _Count) noexcept {
    return _Mismatching::_Mismatch_impl<uint64_t>(_First1, _First2, _Count);
}

__declspec(noalias) void __stdcall __std_replace_4(
    void* _First, void* const _Last, const uint32_t _Old_val, const uint32_t _New_val) noexcept {
#ifndef _M_ARM64EC
    if (_Use_avx2()) {
        const __m256i _Comparand   = _mm256_broadcastd_epi32(_mm_cvtsi32_si128(_Old_val));
        const __m256i _Replacement = _mm256_broadcastd_epi32(_mm_cvtsi32_si128(_New_val));
        const size_t _Full_length  = _Byte_length(_First, _Last);

        void* _Stop_at = _First;
        _Advance_bytes(_Stop_at, _Full_length & ~size_t{0x1F});

        while (_First != _Stop_at) {
            const __m256i _Data = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(_First));
            const __m256i _Mask = _mm256_cmpeq_epi32(_Comparand, _Data);
            _mm256_maskstore_epi32(reinterpret_cast<int*>(_First), _Mask, _Replacement);

            _Advance_bytes(_First, 32);
        }

        if (const size_t _Tail_length = _Full_length & 0x1C; _Tail_length != 0) {
            const __m256i _Tail_mask = _Avx2_tail_mask_32(_Tail_length);
            const __m256i _Data      = _mm256_maskload_epi32(reinterpret_cast<const int*>(_First), _Tail_mask);
            const __m256i _Mask      = _mm256_and_si256(_mm256_cmpeq_epi32(_Comparand, _Data), _Tail_mask);
            _mm256_maskstore_epi32(reinterpret_cast<int*>(_First), _Mask, _Replacement);
        }

        _mm256_zeroupper(); // TRANSITION, DevCom-10331414
    } else
#endif // ^^^ !defined(_M_ARM64EC) ^^^
    {
        for (auto _Cur = reinterpret_cast<uint32_t*>(_First); _Cur != _Last; ++_Cur) {
            if (*_Cur == _Old_val) {
                *_Cur = _New_val;
            }
        }
    }
}

__declspec(noalias) void __stdcall __std_replace_8(
    void* _First, void* const _Last, const uint64_t _Old_val, const uint64_t _New_val) noexcept {
#ifndef _M_ARM64EC
    if (_Use_avx2()) {
#ifdef _WIN64
        const __m256i _Comparand   = _mm256_broadcastq_epi64(_mm_cvtsi64_si128(_Old_val));
        const __m256i _Replacement = _mm256_broadcastq_epi64(_mm_cvtsi64_si128(_New_val));
#else // ^^^ defined(_WIN64) / !defined(_WIN64), workaround, _mm_cvtsi64_si128 does not compile vvv
        const __m256i _Comparand   = _mm256_set1_epi64x(_Old_val);
        const __m256i _Replacement = _mm256_set1_epi64x(_New_val);
#endif // ^^^ !defined(_WIN64) ^^^
        const size_t _Full_length = _Byte_length(_First, _Last);

        void* _Stop_at = _First;
        _Advance_bytes(_Stop_at, _Full_length & ~size_t{0x1F});

        while (_First != _Stop_at) {
            const __m256i _Data = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(_First));
            const __m256i _Mask = _mm256_cmpeq_epi64(_Comparand, _Data);
            _mm256_maskstore_epi64(reinterpret_cast<long long*>(_First), _Mask, _Replacement);

            _Advance_bytes(_First, 32);
        }

        if (const size_t _Tail_length = _Full_length & 0x18; _Tail_length != 0) {
            const __m256i _Tail_mask = _Avx2_tail_mask_32(_Tail_length);
            const __m256i _Data      = _mm256_maskload_epi64(reinterpret_cast<const long long*>(_First), _Tail_mask);
            const __m256i _Mask      = _mm256_and_si256(_mm256_cmpeq_epi64(_Comparand, _Data), _Tail_mask);
            _mm256_maskstore_epi64(reinterpret_cast<long long*>(_First), _Mask, _Replacement);
        }

        _mm256_zeroupper(); // TRANSITION, DevCom-10331414
    } else
#endif // ^^^ !defined(_M_ARM64EC) ^^^
    {
        for (auto _Cur = reinterpret_cast<uint64_t*>(_First); _Cur != _Last; ++_Cur) {
            if (*_Cur == _Old_val) {
                *_Cur = _New_val;
            }
        }
    }
}

} // extern "C"

namespace {
    namespace _Removing {
        template <class _Ty>
        void* _Remove_fallback(
            const void* const _First, const void* const _Last, void* const _Out, const _Ty _Val) noexcept {
            const _Ty* _Src = reinterpret_cast<const _Ty*>(_First);
            _Ty* _Dest      = reinterpret_cast<_Ty*>(_Out);

            while (_Src != _Last) {
                if (*_Src != _Val) {
                    *_Dest = *_Src;
                    ++_Dest;
                }

                ++_Src;
            }

            return _Dest;
        }

        template <class _Ty>
        void* _Unique_fallback(const void* const _First, const void* const _Last, void* const _Dest) noexcept {
            _Ty* _Out       = reinterpret_cast<_Ty*>(_Dest);
            const _Ty* _Src = reinterpret_cast<const _Ty*>(_First);

            while (_Src != _Last) {
                if (*_Src != *_Out) {
                    ++_Out;
                    *_Out = *_Src;
                }

                ++_Src;
            }

            ++_Out;
            return _Out;
        }

#ifndef _M_ARM64EC
        template <size_t _Size_v, size_t _Size_h>
        struct _Tables {
            uint8_t _Shuf[_Size_v][_Size_h];
            uint8_t _Size[_Size_v];
        };

        template <size_t _Size_v, size_t _Size_h>
        constexpr auto _Make_tables(const uint32_t _Mul, const uint32_t _Ew) {
            _Tables<_Size_v, _Size_h> _Result;

            for (uint32_t _Vx = 0; _Vx != _Size_v; ++_Vx) {
                uint32_t _Nx = 0;

                // Make shuffle mask for pshufb / vpermd corresponding to _Vx bit value.
                // Every bit set corresponds to an element skipped.
                for (uint32_t _Hx = 0; _Hx != _Size_h / _Ew; ++_Hx) {
                    if ((_Vx & (1 << _Hx)) == 0) {
                        // Inner loop needed for cases where the shuffle mask operates on element parts rather than
                        // whole elements; for whole elements there would be one iteration.
                        for (uint32_t _Ex = 0; _Ex != _Ew; ++_Ex) {
                            _Result._Shuf[_Vx][_Nx * _Ew + _Ex] = static_cast<uint8_t>(_Hx * _Ew + _Ex);
                        }
                        ++_Nx;
                    }
                }

                // Size of elements that are not removed in bytes.
                _Result._Size[_Vx] = static_cast<uint8_t>(_Nx * _Mul);

                // Fill the remaining with arbitrary elements.
                // It is not possible to leave them untouched while keeping this optimization efficient.
                // This should not be a problem though, as they should be either overwritten by the next step,
                // or left in the removed range.
                for (; _Nx != _Size_h / _Ew; ++_Nx) {
                    // Inner loop needed for cases where the shuffle mask operates on element parts rather than whole
                    // elements; for whole elements there would be one iteration.
                    for (uint32_t _Ex = 0; _Ex != _Ew; ++_Ex) {
                        _Result._Shuf[_Vx][_Nx * _Ew + _Ex] = static_cast<uint8_t>(_Nx * _Ew + _Ex);
                    }
                }
            }

            return _Result;
        }

        constexpr auto _Tables_1_sse = _Make_tables<256, 8>(1, 1);
        constexpr auto _Tables_2_sse = _Make_tables<256, 16>(2, 2);
        constexpr auto _Tables_4_sse = _Make_tables<16, 16>(4, 4);
        constexpr auto _Tables_4_avx = _Make_tables<256, 8>(4, 1);
        constexpr auto _Tables_8_sse = _Make_tables<4, 16>(8, 8);
        constexpr auto _Tables_8_avx = _Make_tables<16, 8>(8, 2);

        struct _Sse_1 {
            static constexpr size_t _Elem_size = 1;
            static constexpr size_t _Step      = 8;

            static __m128i _Set(const uint8_t _Val) noexcept {
                return _mm_shuffle_epi8(_mm_cvtsi32_si128(_Val), _mm_setzero_si128());
            }

            static __m128i _Load(const void* const _Ptr) noexcept {
                return _mm_loadu_si64(_Ptr);
            }

            static uint32_t _Mask(const __m128i _First, const __m128i _Second) noexcept {
                return _mm_movemask_epi8(_mm_cmpeq_epi8(_First, _Second)) & 0xFF;
            }

            static void* _Store_masked(void* _Out, const __m128i _Src, const uint32_t _Bingo) noexcept {
                const __m128i _Shuf = _mm_loadu_si64(_Tables_1_sse._Shuf[_Bingo]);
                const __m128i _Dest = _mm_shuffle_epi8(_Src, _Shuf);
                _mm_storeu_si64(_Out, _Dest);
                _Advance_bytes(_Out, _Tables_1_sse._Size[_Bingo]);
                return _Out;
            }
        };

        struct _Sse_2 {
            static constexpr size_t _Elem_size = 2;
            static constexpr size_t _Step      = 16;

            static __m128i _Set(const uint16_t _Val) noexcept {
                return _mm_set1_epi16(_Val);
            }

            static __m128i _Load(const void* const _Ptr) noexcept {
                return _mm_loadu_si128(reinterpret_cast<const __m128i*>(_Ptr));
            }

            static uint32_t _Mask(const __m128i _First, const __m128i _Second) noexcept {
                const __m128i _Mask = _mm_cmpeq_epi16(_First, _Second);
                return _mm_movemask_epi8(_mm_packs_epi16(_Mask, _mm_setzero_si128()));
            }

            static void* _Store_masked(void* _Out, const __m128i _Src, const uint32_t _Bingo) noexcept {
                const __m128i _Shuf = _mm_loadu_si128(reinterpret_cast<const __m128i*>(_Tables_2_sse._Shuf[_Bingo]));
                const __m128i _Dest = _mm_shuffle_epi8(_Src, _Shuf);
                _mm_storeu_si128(reinterpret_cast<__m128i*>(_Out), _Dest);
                _Advance_bytes(_Out, _Tables_2_sse._Size[_Bingo]);
                return _Out;
            }
        };

        struct _Avx_4 {
            static constexpr size_t _Elem_size = 4;
            static constexpr size_t _Step      = 32;

            static __m256i _Set(const uint32_t _Val) noexcept {
                return _mm256_set1_epi32(_Val);
            }

            static __m256i _Load(const void* const _Ptr) noexcept {
                return _mm256_loadu_si256(reinterpret_cast<const __m256i*>(_Ptr));
            }

            static uint32_t _Mask(const __m256i _First, const __m256i _Second) noexcept {
                const __m256i _Mask = _mm256_cmpeq_epi32(_First, _Second);
                return _mm256_movemask_ps(_mm256_castsi256_ps(_Mask));
            }

            static void* _Store_masked(void* _Out, const __m256i _Src, const uint32_t _Bingo) noexcept {
                const __m256i _Shuf = _mm256_cvtepu8_epi32(_mm_loadu_si64(_Tables_4_avx._Shuf[_Bingo]));
                const __m256i _Dest = _mm256_permutevar8x32_epi32(_Src, _Shuf);
                _mm256_storeu_si256(reinterpret_cast<__m256i*>(_Out), _Dest);
                _Advance_bytes(_Out, _Tables_4_avx._Size[_Bingo]);
                return _Out;
            }
        };

        struct _Sse_4 {
            static constexpr size_t _Elem_size = 4;
            static constexpr size_t _Step      = 16;

            static __m128i _Set(const uint32_t _Val) noexcept {
                return _mm_set1_epi32(_Val);
            }

            static __m128i _Load(const void* const _Ptr) noexcept {
                return _mm_loadu_si128(reinterpret_cast<const __m128i*>(_Ptr));
            }

            static uint32_t _Mask(const __m128i _First, const __m128i _Second) noexcept {
                const __m128i _Mask = _mm_cmpeq_epi32(_First, _Second);
                return _mm_movemask_ps(_mm_castsi128_ps(_Mask));
            }

            static void* _Store_masked(void* _Out, const __m128i _Src, const uint32_t _Bingo) noexcept {
                const __m128i _Shuf = _mm_loadu_si128(reinterpret_cast<const __m128i*>(_Tables_4_sse._Shuf[_Bingo]));
                const __m128i _Dest = _mm_shuffle_epi8(_Src, _Shuf);
                _mm_storeu_si128(reinterpret_cast<__m128i*>(_Out), _Dest);
                _Advance_bytes(_Out, _Tables_4_sse._Size[_Bingo]);
                return _Out;
            }
        };

        struct _Avx_8 {
            static constexpr size_t _Elem_size = 8;
            static constexpr size_t _Step      = 32;

            static __m256i _Set(const uint64_t _Val) noexcept {
                return _mm256_set1_epi64x(_Val);
            }

            static __m256i _Load(const void* const _Ptr) noexcept {
                return _mm256_loadu_si256(reinterpret_cast<const __m256i*>(_Ptr));
            }

            static uint32_t _Mask(const __m256i _First, const __m256i _Second) noexcept {
                const __m256i _Mask = _mm256_cmpeq_epi64(_First, _Second);
                return _mm256_movemask_pd(_mm256_castsi256_pd(_Mask));
            }

            static void* _Store_masked(void* _Out, const __m256i _Src, const uint32_t _Bingo) noexcept {
                const __m256i _Shuf = _mm256_cvtepu8_epi32(_mm_loadu_si64(_Tables_8_avx._Shuf[_Bingo]));
                const __m256i _Dest = _mm256_permutevar8x32_epi32(_Src, _Shuf);
                _mm256_storeu_si256(reinterpret_cast<__m256i*>(_Out), _Dest);
                _Advance_bytes(_Out, _Tables_8_avx._Size[_Bingo]);
                return _Out;
            }
        };

        struct _Sse_8 {
            static constexpr size_t _Elem_size = 8;
            static constexpr size_t _Step      = 16;

            static __m128i _Set(const uint64_t _Val) noexcept {
                return _mm_set1_epi64x(_Val);
            }

            static __m128i _Load(const void* const _Ptr) noexcept {
                return _mm_loadu_si128(reinterpret_cast<const __m128i*>(_Ptr));
            }

            static uint32_t _Mask(const __m128i _First, const __m128i _Second) noexcept {
                const __m128i _Mask = _mm_cmpeq_epi64(_First, _Second);
                return _mm_movemask_pd(_mm_castsi128_pd(_Mask));
            }

            static void* _Store_masked(void* _Out, const __m128i _Src, const uint32_t _Bingo) noexcept {
                const __m128i _Shuf = _mm_loadu_si128(reinterpret_cast<const __m128i*>(_Tables_8_sse._Shuf[_Bingo]));
                const __m128i _Dest = _mm_shuffle_epi8(_Src, _Shuf);
                _mm_storeu_si128(reinterpret_cast<__m128i*>(_Out), _Dest);
                _Advance_bytes(_Out, _Tables_8_sse._Size[_Bingo]);
                return _Out;
            }
        };

        constexpr size_t _Copy_buffer_size = 512;

        template <class _Traits, class _Ty>
        void* _Remove_impl(void* _First, void* const _Stop, const _Ty _Val) noexcept {
            void* _Out        = _First;
            const auto _Match = _Traits::_Set(_Val);

            do {
                const auto _Src       = _Traits::_Load(_First);
                const uint32_t _Bingo = _Traits::_Mask(_Src, _Match);
                _Out                  = _Traits::_Store_masked(_Out, _Src, _Bingo);
                _Advance_bytes(_First, _Traits::_Step);
            } while (_First != _Stop);

            return _Out;
        }

        template <class _Traits, class _Ty>
        void* _Remove_copy_impl(const void* _First, const void* const _Stop, void* _Out, const _Ty _Val) noexcept {
            unsigned char _Buffer[_Copy_buffer_size];
            void* _Buffer_out        = _Buffer;
            void* const _Buffer_stop = _Buffer + _Copy_buffer_size - _Traits::_Step;

            const auto _Match = _Traits::_Set(_Val);

            do {
                const auto _Src       = _Traits::_Load(_First);
                const uint32_t _Bingo = _Traits::_Mask(_Src, _Match);
                _Buffer_out           = _Traits::_Store_masked(_Buffer_out, _Src, _Bingo);
                _Advance_bytes(_First, _Traits::_Step);

                if (_Buffer_out >= _Buffer_stop) {
                    const size_t _Fill = _Byte_length(_Buffer, _Buffer_out);
                    memcpy(_Out, _Buffer, _Fill);
                    _Advance_bytes(_Out, _Fill);
                    _Buffer_out = _Buffer;
                }
            } while (_First != _Stop);

            const size_t _Fill = _Byte_length(_Buffer, _Buffer_out);
            memcpy(_Out, _Buffer, _Fill);
            _Advance_bytes(_Out, _Fill);
            return _Out;
        }

        template <class _Traits>
        void* _Unique_impl(void* _First, void* const _Stop) noexcept {
            void* _Out = _First;

            do {
                const auto _Src = _Traits::_Load(_First);
                void* _First_d  = _First;
                _Rewind_bytes(_First_d, _Traits::_Elem_size);
                const auto _Match     = _Traits::_Load(_First_d);
                const uint32_t _Bingo = _Traits::_Mask(_Src, _Match);
                _Out                  = _Traits::_Store_masked(_Out, _Src, _Bingo);
                _Advance_bytes(_First, _Traits::_Step);
            } while (_First != _Stop);

            _Rewind_bytes(_Out, _Traits::_Elem_size);
            return _Out;
        }

        template <class _Traits>
        void* _Unique_copy_impl(const void* _First, const void* const _Stop, void* _Out) noexcept {
            unsigned char _Buffer[_Copy_buffer_size];
            void* _Buffer_out        = _Buffer;
            void* const _Buffer_stop = _Buffer + _Copy_buffer_size - _Traits::_Step;

            do {
                const auto _Src      = _Traits::_Load(_First);
                const void* _First_d = _First;
                _Rewind_bytes(_First_d, _Traits::_Elem_size);
                const auto _Match     = _Traits::_Load(_First_d);
                const uint32_t _Bingo = _Traits::_Mask(_Src, _Match);
                _Buffer_out           = _Traits::_Store_masked(_Buffer_out, _Src, _Bingo);
                _Advance_bytes(_First, _Traits::_Step);

                if (_Buffer_out >= _Buffer_stop) {
                    const size_t _Fill = _Byte_length(_Buffer, _Buffer_out);
                    memcpy(static_cast<unsigned char*>(_Out) + _Traits::_Elem_size, _Buffer, _Fill);
                    _Advance_bytes(_Out, _Fill);
                    _Buffer_out = _Buffer;
                }
            } while (_First != _Stop);

            const size_t _Fill = _Byte_length(_Buffer, _Buffer_out);
            memcpy(static_cast<unsigned char*>(_Out) + _Traits::_Elem_size, _Buffer, _Fill);
            _Advance_bytes(_Out, _Fill);
            return _Out;
        }
#endif // ^^^ !defined(_M_ARM64EC) ^^^
    } // namespace _Removing
} // unnamed namespace

extern "C" {

void* __stdcall __std_remove_1(void* _First, void* const _Last, const uint8_t _Val) noexcept {
    void* _Out = _First;

#ifndef _M_ARM64EC
    if (const size_t _Size_bytes = _Byte_length(_First, _Last); _Use_sse42() && _Size_bytes >= 8) {
        void* _Stop = _First;
        _Advance_bytes(_Stop, _Size_bytes & ~size_t{7});
        _Out   = _Removing::_Remove_impl<_Removing::_Sse_1>(_First, _Stop, _Val);
        _First = _Stop;
    }
#endif // ^^^ !defined(_M_ARM64EC) ^^^

    return _Removing::_Remove_fallback(_First, _Last, _Out, _Val);
}

void* __stdcall __std_remove_2(void* _First, void* const _Last, const uint16_t _Val) noexcept {
    void* _Out = _First;

#ifndef _M_ARM64EC
    if (const size_t _Size_bytes = _Byte_length(_First, _Last); _Use_sse42() && _Size_bytes >= 16) {
        void* _Stop = _First;
        _Advance_bytes(_Stop, _Size_bytes & ~size_t{0xF});
        _Out   = _Removing::_Remove_impl<_Removing::_Sse_2>(_First, _Stop, _Val);
        _First = _Stop;
    }
#endif // ^^^ !defined(_M_ARM64EC) ^^^

    return _Removing::_Remove_fallback(_First, _Last, _Out, _Val);
}

void* __stdcall __std_remove_4(void* _First, void* const _Last, const uint32_t _Val) noexcept {
    void* _Out = _First;

#ifndef _M_ARM64EC
    if (const size_t _Size_bytes = _Byte_length(_First, _Last); _Use_avx2() && _Size_bytes >= 32) {
        void* _Stop = _First;
        _Advance_bytes(_Stop, _Size_bytes & ~size_t{0x1F});
        _Out   = _Removing::_Remove_impl<_Removing::_Avx_4>(_First, _Stop, _Val);
        _First = _Stop;

        _mm256_zeroupper(); // TRANSITION, DevCom-10331414
    } else if (_Use_sse42() && _Size_bytes >= 16) {
        void* _Stop = _First;
        _Advance_bytes(_Stop, _Size_bytes & ~size_t{0xF});
        _Out   = _Removing::_Remove_impl<_Removing::_Sse_4>(_First, _Stop, _Val);
        _First = _Stop;
    }
#endif // ^^^ !defined(_M_ARM64EC) ^^^

    return _Removing::_Remove_fallback(_First, _Last, _Out, _Val);
}

void* __stdcall __std_remove_8(void* _First, void* const _Last, const uint64_t _Val) noexcept {
    void* _Out = _First;

#ifndef _M_ARM64EC
    if (const size_t _Size_bytes = _Byte_length(_First, _Last); _Use_avx2() && _Size_bytes >= 32) {
        void* _Stop = _First;
        _Advance_bytes(_Stop, _Size_bytes & ~size_t{0x1F});
        _Out   = _Removing::_Remove_impl<_Removing::_Avx_8>(_First, _Stop, _Val);
        _First = _Stop;

        _mm256_zeroupper(); // TRANSITION, DevCom-10331414
    } else if (_Use_sse42() && _Size_bytes >= 16) {
        void* _Stop = _First;
        _Advance_bytes(_Stop, _Size_bytes & ~size_t{0xF});
        _Out   = _Removing::_Remove_impl<_Removing::_Sse_8>(_First, _Stop, _Val);
        _First = _Stop;
    }
#endif // ^^^ !defined(_M_ARM64EC) ^^^

    return _Removing::_Remove_fallback(_First, _Last, _Out, _Val);
}

void* __stdcall __std_remove_copy_1(
    const void* _First, const void* const _Last, void* _Out, const uint8_t _Val) noexcept {
#ifndef _M_ARM64EC
    if (const size_t _Size_bytes = _Byte_length(_First, _Last); _Use_sse42() && _Size_bytes >= 8) {
        const void* _Stop = _First;
        _Advance_bytes(_Stop, _Size_bytes & ~size_t{7});
        _Out   = _Removing::_Remove_copy_impl<_Removing::_Sse_1>(_First, _Stop, _Out, _Val);
        _First = _Stop;
    }
#endif // ^^^ !defined(_M_ARM64EC) ^^^

    return _Removing::_Remove_fallback(_First, _Last, _Out, _Val);
}

void* __stdcall __std_remove_copy_2(
    const void* _First, const void* const _Last, void* _Out, const uint16_t _Val) noexcept {
#ifndef _M_ARM64EC
    if (const size_t _Size_bytes = _Byte_length(_First, _Last); _Use_sse42() && _Size_bytes >= 16) {
        const void* _Stop = _First;
        _Advance_bytes(_Stop, _Size_bytes & ~size_t{0xF});
        _Out   = _Removing::_Remove_copy_impl<_Removing::_Sse_2>(_First, _Stop, _Out, _Val);
        _First = _Stop;
    }
#endif // ^^^ !defined(_M_ARM64EC) ^^^

    return _Removing::_Remove_fallback(_First, _Last, _Out, _Val);
}

void* __stdcall __std_remove_copy_4(
    const void* _First, const void* const _Last, void* _Out, const uint32_t _Val) noexcept {
#ifndef _M_ARM64EC
    if (const size_t _Size_bytes = _Byte_length(_First, _Last); _Use_avx2() && _Size_bytes >= 32) {
        const void* _Stop = _First;
        _Advance_bytes(_Stop, _Size_bytes & ~size_t{0x1F});
        _Out   = _Removing::_Remove_copy_impl<_Removing::_Avx_4>(_First, _Stop, _Out, _Val);
        _First = _Stop;

        _mm256_zeroupper(); // TRANSITION, DevCom-10331414
    } else if (_Use_sse42() && _Size_bytes >= 16) {
        const void* _Stop = _First;
        _Advance_bytes(_Stop, _Size_bytes & ~size_t{0xF});
        _Out   = _Removing::_Remove_copy_impl<_Removing::_Sse_4>(_First, _Stop, _Out, _Val);
        _First = _Stop;
    }
#endif // ^^^ !defined(_M_ARM64EC) ^^^

    return _Removing::_Remove_fallback(_First, _Last, _Out, _Val);
}

void* __stdcall __std_remove_copy_8(
    const void* _First, const void* const _Last, void* _Out, const uint64_t _Val) noexcept {
#ifndef _M_ARM64EC
    if (const size_t _Size_bytes = _Byte_length(_First, _Last); _Use_avx2() && _Size_bytes >= 32) {
        const void* _Stop = _First;
        _Advance_bytes(_Stop, _Size_bytes & ~size_t{0x1F});
        _Out   = _Removing::_Remove_copy_impl<_Removing::_Avx_8>(_First, _Stop, _Out, _Val);
        _First = _Stop;

        _mm256_zeroupper(); // TRANSITION, DevCom-10331414
    } else if (_Use_sse42() && _Size_bytes >= 16) {
        const void* _Stop = _First;
        _Advance_bytes(_Stop, _Size_bytes & ~size_t{0xF});
        _Out   = _Removing::_Remove_copy_impl<_Removing::_Sse_8>(_First, _Stop, _Out, _Val);
        _First = _Stop;
    }
#endif // ^^^ !defined(_M_ARM64EC) ^^^

    return _Removing::_Remove_fallback(_First, _Last, _Out, _Val);
}

void* __stdcall __std_unique_1(void* _First, void* const _Last) noexcept {
    _First = const_cast<void*>(__std_adjacent_find_1(_First, _Last));

    if (_First == _Last) {
        return _First;
    }

    void* _Dest = _First;
    _Advance_bytes(_First, 1);

#ifndef _M_ARM64EC
    if (const size_t _Size_bytes = _Byte_length(_First, _Last); _Use_sse42() && _Size_bytes >= 8) {
        void* _Stop = _First;
        _Advance_bytes(_Stop, _Size_bytes & ~size_t{7});
        _Dest  = _Removing::_Unique_impl<_Removing::_Sse_1>(_First, _Stop);
        _First = _Stop;
    }
#endif // ^^^ !defined(_M_ARM64EC) ^^^

    return _Removing::_Unique_fallback<uint8_t>(_First, _Last, _Dest);
}

void* __stdcall __std_unique_2(void* _First, void* const _Last) noexcept {
    _First = const_cast<void*>(__std_adjacent_find_2(_First, _Last));

    if (_First == _Last) {
        return _First;
    }

    void* _Dest = _First;
    _Advance_bytes(_First, 2);

#ifndef _M_ARM64EC
    if (const size_t _Size_bytes = _Byte_length(_First, _Last); _Use_sse42() && _Size_bytes >= 16) {
        void* _Stop = _First;
        _Advance_bytes(_Stop, _Size_bytes & ~size_t{0xF});
        _Dest  = _Removing::_Unique_impl<_Removing::_Sse_2>(_First, _Stop);
        _First = _Stop;
    }
#endif // ^^^ !defined(_M_ARM64EC) ^^^

    return _Removing::_Unique_fallback<uint16_t>(_First, _Last, _Dest);
}

void* __stdcall __std_unique_4(void* _First, void* const _Last) noexcept {
    _First = const_cast<void*>(__std_adjacent_find_4(_First, _Last));

    if (_First == _Last) {
        return _First;
    }

    void* _Dest = _First;
    _Advance_bytes(_First, 4);

#ifndef _M_ARM64EC
    if (const size_t _Size_bytes = _Byte_length(_First, _Last); _Use_avx2() && _Size_bytes >= 32) {
        void* _Stop = _First;
        _Advance_bytes(_Stop, _Size_bytes & ~size_t{0x1F});
        _Dest  = _Removing::_Unique_impl<_Removing::_Avx_4>(_First, _Stop);
        _First = _Stop;

        _mm256_zeroupper(); // TRANSITION, DevCom-10331414
    } else if (_Use_sse42() && _Size_bytes >= 16) {
        void* _Stop = _First;
        _Advance_bytes(_Stop, _Size_bytes & ~size_t{0xF});
        _Dest  = _Removing::_Unique_impl<_Removing::_Sse_4>(_First, _Stop);
        _First = _Stop;
    }
#endif // ^^^ !defined(_M_ARM64EC) ^^^

    return _Removing::_Unique_fallback<uint32_t>(_First, _Last, _Dest);
}

void* __stdcall __std_unique_8(void* _First, void* const _Last) noexcept {
    _First = const_cast<void*>(__std_adjacent_find_8(_First, _Last));

    if (_First == _Last) {
        return _First;
    }

    void* _Dest = _First;
    _Advance_bytes(_First, 8);

#ifndef _M_ARM64EC
    if (const size_t _Size_bytes = _Byte_length(_First, _Last); _Use_avx2() && _Size_bytes >= 32) {
        void* _Stop = _First;
        _Advance_bytes(_Stop, _Size_bytes & ~size_t{0x1F});
        _Dest  = _Removing::_Unique_impl<_Removing::_Avx_8>(_First, _Stop);
        _First = _Stop;

        _mm256_zeroupper(); // TRANSITION, DevCom-10331414
    } else if (_Use_sse42() && _Size_bytes >= 16) {
        void* _Stop = _First;
        _Advance_bytes(_Stop, _Size_bytes & ~size_t{0xF});
        _Dest  = _Removing::_Unique_impl<_Removing::_Sse_8>(_First, _Stop);
        _First = _Stop;
    }
#endif // ^^^ !defined(_M_ARM64EC) ^^^

    return _Removing::_Unique_fallback<uint64_t>(_First, _Last, _Dest);
}

void* __stdcall __std_unique_copy_1(const void* _First, const void* const _Last, void* _Dest) noexcept {
    if (_First == _Last) {
        return _Dest;
    }

    memcpy(_Dest, _First, 1);
    _Advance_bytes(_First, 1);

#ifndef _M_ARM64EC
    if (const size_t _Size_bytes = _Byte_length(_First, _Last); _Use_sse42() && _Size_bytes >= 8) {
        const void* _Stop = _First;
        _Advance_bytes(_Stop, _Size_bytes & ~size_t{7});
        _Dest  = _Removing::_Unique_copy_impl<_Removing::_Sse_1>(_First, _Stop, _Dest);
        _First = _Stop;
    }
#endif // ^^^ !defined(_M_ARM64EC) ^^^

    return _Removing::_Unique_fallback<uint8_t>(_First, _Last, _Dest);
}

void* __stdcall __std_unique_copy_2(const void* _First, const void* const _Last, void* _Dest) noexcept {
    if (_First == _Last) {
        return _Dest;
    }

    memcpy(_Dest, _First, 2);
    _Advance_bytes(_First, 2);

#ifndef _M_ARM64EC
    if (const size_t _Size_bytes = _Byte_length(_First, _Last); _Use_sse42() && _Size_bytes >= 16) {
        const void* _Stop = _First;
        _Advance_bytes(_Stop, _Size_bytes & ~size_t{0xF});
        _Dest  = _Removing::_Unique_copy_impl<_Removing::_Sse_2>(_First, _Stop, _Dest);
        _First = _Stop;
    }
#endif // ^^^ !defined(_M_ARM64EC) ^^^

    return _Removing::_Unique_fallback<uint16_t>(_First, _Last, _Dest);
}

void* __stdcall __std_unique_copy_4(const void* _First, const void* const _Last, void* _Dest) noexcept {
    if (_First == _Last) {
        return _Dest;
    }

    memcpy(_Dest, _First, 4);
    _Advance_bytes(_First, 4);

#ifndef _M_ARM64EC
    if (const size_t _Size_bytes = _Byte_length(_First, _Last); _Use_avx2() && _Size_bytes >= 32) {
        const void* _Stop = _First;
        _Advance_bytes(_Stop, _Size_bytes & ~size_t{0x1F});
        _Dest  = _Removing::_Unique_copy_impl<_Removing::_Avx_4>(_First, _Stop, _Dest);
        _First = _Stop;

        _mm256_zeroupper(); // TRANSITION, DevCom-10331414
    } else if (_Use_sse42() && _Size_bytes >= 16) {
        const void* _Stop = _First;
        _Advance_bytes(_Stop, _Size_bytes & ~size_t{0xF});
        _Dest  = _Removing::_Unique_copy_impl<_Removing::_Sse_4>(_First, _Stop, _Dest);
        _First = _Stop;
    }
#endif // ^^^ !defined(_M_ARM64EC) ^^^

    return _Removing::_Unique_fallback<uint32_t>(_First, _Last, _Dest);
}

void* __stdcall __std_unique_copy_8(const void* _First, const void* const _Last, void* _Dest) noexcept {
    if (_First == _Last) {
        return _Dest;
    }

    memcpy(_Dest, _First, 8);
    _Advance_bytes(_First, 8);

#ifndef _M_ARM64EC
    if (const size_t _Size_bytes = _Byte_length(_First, _Last); _Use_avx2() && _Size_bytes >= 32) {
        const void* _Stop = _First;
        _Advance_bytes(_Stop, _Size_bytes & ~size_t{0x1F});
        _Dest  = _Removing::_Unique_copy_impl<_Removing::_Avx_8>(_First, _Stop, _Dest);
        _First = _Stop;

        _mm256_zeroupper(); // TRANSITION, DevCom-10331414
    } else if (_Use_sse42() && _Size_bytes >= 16) {
        const void* _Stop = _First;
        _Advance_bytes(_Stop, _Size_bytes & ~size_t{0xF});
        _Dest  = _Removing::_Unique_copy_impl<_Removing::_Sse_8>(_First, _Stop, _Dest);
        _First = _Stop;
    }
#endif // ^^^ !defined(_M_ARM64EC) ^^^

    return _Removing::_Unique_fallback<uint64_t>(_First, _Last, _Dest);
}

} // extern "C"

namespace {
    namespace _Bitset_to_string {
#ifdef _M_ARM64EC
        using _Traits_1_avx = void;
        using _Traits_1_sse = void;
        using _Traits_2_avx = void;
        using _Traits_2_sse = void;
#else // ^^^ defined(_M_ARM64EC) / !defined(_M_ARM64EC) vvv
        struct _Traits_avx {
            static void _Out(void* const _Dest, const __m256i _Elems) noexcept {
                _mm256_storeu_si256(static_cast<__m256i*>(_Dest), _Elems);
            }

            static void _Exit_vectorized() noexcept {
                _mm256_zeroupper();
            }
        };

        struct _Traits_sse {
            static void _Out(void* const _Dest, const __m128i _Elems) noexcept {
                _mm_storeu_si128(static_cast<__m128i*>(_Dest), _Elems);
            }

            static void _Exit_vectorized() noexcept {}
        };

        struct _Traits_1_avx : _Traits_avx {
            using _Value_type = uint32_t;

            static __m256i _Set(const char _Val) noexcept {
                return _mm256_broadcastb_epi8(_mm_cvtsi32_si128(_Val));
            }

            static __m256i __forceinline _Step(const uint32_t _Val, const __m256i _Px0, const __m256i _Px1) noexcept {
                const __m128i _Vx0 = _mm_cvtsi32_si128(_Val);
                const __m128i _Vx1 =
                    _mm_shuffle_epi8(_Vx0, _mm_set_epi32(0x00000000, 0x01010101, 0x02020202, 0x03030303));
                const __m256i _Vx2 = _mm256_castsi128_si256(_Vx1);
                const __m256i _Vx3 = _mm256_permutevar8x32_epi32(_Vx2, _mm256_set_epi32(3, 3, 2, 2, 1, 1, 0, 0));
                const __m256i _Msk = _mm256_and_si256(_Vx3, _mm256_set1_epi64x(0x0102040810204080));
                const __m256i _Ex0 = _mm256_cmpeq_epi8(_Msk, _mm256_setzero_si256());
                const __m256i _Ex1 = _mm256_blendv_epi8(_Px1, _Px0, _Ex0);
                return _Ex1;
            }
        };

        struct _Traits_1_sse : _Traits_sse {
            using _Value_type = uint16_t;

            static __m128i _Set(const char _Val) noexcept {
                return _mm_shuffle_epi8(_mm_cvtsi32_si128(_Val), _mm_setzero_si128());
            }

            static __m128i __forceinline _Step(const uint16_t _Val, const __m128i _Px0, const __m128i _Px1) noexcept {
                const __m128i _Vx0 = _mm_cvtsi32_si128(_Val);
                const __m128i _Vx1 =
                    _mm_shuffle_epi8(_Vx0, _mm_set_epi32(0x00000000, 0x00000000, 0x01010101, 0x01010101));
                const __m128i _Msk = _mm_and_si128(_Vx1, _mm_set1_epi64x(0x0102040810204080));
                const __m128i _Ex0 = _mm_cmpeq_epi8(_Msk, _mm_setzero_si128());
                const __m128i _Ex1 = _mm_blendv_epi8(_Px1, _Px0, _Ex0);
                return _Ex1;
            }
        };

        struct _Traits_2_avx : _Traits_avx {
            using _Value_type = uint16_t;

            static __m256i _Set(const wchar_t _Val) noexcept {
                return _mm256_broadcastw_epi16(_mm_cvtsi32_si128(_Val));
            }

            static __m256i __forceinline _Step(const uint16_t _Val, const __m256i _Px0, const __m256i _Px1) noexcept {
                const __m128i _Vx0 = _mm_cvtsi32_si128(_Val);
                const __m128i _Vx1 =
                    _mm_shuffle_epi8(_Vx0, _mm_set_epi32(0x00000000, 0x00000000, 0x01010101, 0x01010101));
                const __m256i _Vx2 = _mm256_castsi128_si256(_Vx1);
                const __m256i _Vx3 = _mm256_permute4x64_epi64(_Vx2, _MM_SHUFFLE(1, 1, 0, 0));
                const __m256i _Msk = _mm256_and_si256(_Vx3,
                    _mm256_set_epi64x(0x0001000200040008, 0x0010002000400080, 0x0001000200040008, 0x0010002000400080));
                const __m256i _Ex0 = _mm256_cmpeq_epi16(_Msk, _mm256_setzero_si256());
                const __m256i _Ex1 = _mm256_blendv_epi8(_Px1, _Px0, _Ex0);
                return _Ex1;
            }
        };

        struct _Traits_2_sse : _Traits_sse {
            using _Value_type = uint8_t;

            static __m128i _Set(const wchar_t _Val) noexcept {
                return _mm_set1_epi16(_Val);
            }

            static __m128i __forceinline _Step(const uint8_t _Val, const __m128i _Px0, const __m128i _Px1) noexcept {
                const __m128i _Vx  = _mm_set1_epi16(_Val);
                const __m128i _Msk = _mm_and_si128(_Vx, _mm_set_epi64x(0x0001000200040008, 0x0010002000400080));
                const __m128i _Ex0 = _mm_cmpeq_epi16(_Msk, _mm_setzero_si128());
                const __m128i _Ex1 = _mm_blendv_epi8(_Px1, _Px0, _Ex0);
                return _Ex1;
            }
        };

        template <class _Traits, class _Elem>
        void __stdcall _Impl(
            _Elem* const _Dest, const void* _Src, size_t _Size_bits, const _Elem _Elem0, const _Elem _Elem1) noexcept {
            constexpr size_t _Step_size_bits = sizeof(typename _Traits::_Value_type) * 8;

            const auto _Px0 = _Traits::_Set(_Elem0);
            const auto _Px1 = _Traits::_Set(_Elem1);
            if (_Size_bits >= _Step_size_bits) {
                _Elem* _Pos = _Dest + _Size_bits;
                _Size_bits &= _Step_size_bits - 1;
                _Elem* const _Stop_at = _Dest + _Size_bits;
                do {
                    typename _Traits::_Value_type _Val;
                    memcpy(&_Val, _Src, sizeof(_Val));
                    const auto _Elems = _Traits::_Step(_Val, _Px0, _Px1);
                    _Pos -= _Step_size_bits;
                    _Traits::_Out(_Pos, _Elems);
                    _Advance_bytes(_Src, sizeof(_Val));
                } while (_Pos != _Stop_at);
            }

            if (_Size_bits > 0) {
                typename _Traits::_Value_type _Val;
                memcpy(&_Val, _Src, sizeof(_Val));
                const auto _Elems = _Traits::_Step(_Val, _Px0, _Px1);
                _Elem _Tmp[_Step_size_bits];
                _Traits::_Out(_Tmp, _Elems);
                const _Elem* const _Tmpd = _Tmp + (_Step_size_bits - _Size_bits);
                memcpy(_Dest, _Tmpd, _Size_bits * sizeof(_Elem));
            }

            _Traits::_Exit_vectorized(); // TRANSITION, DevCom-10331414
        }
#endif // ^^^ !defined(_M_ARM64EC) ^^^

        template <class _Avx_traits, class _Sse_traits, class _Elem>
        void __stdcall _Dispatch(_Elem* const _Dest, const void* const _Src, const size_t _Size_bits,
            const _Elem _Elem0, const _Elem _Elem1) noexcept {
#ifndef _M_ARM64EC
            if (_Use_avx2() && _Size_bits >= 256) {
                _Impl<_Avx_traits>(_Dest, _Src, _Size_bits, _Elem0, _Elem1);
            } else if (_Use_sse42()) {
                _Impl<_Sse_traits>(_Dest, _Src, _Size_bits, _Elem0, _Elem1);
            } else
#endif // ^^^ !defined(_M_ARM64EC) ^^^
            {
                const auto _Arr = reinterpret_cast<const uint8_t*>(_Src);
                for (size_t _Ix = 0; _Ix < _Size_bits; ++_Ix) {
                    _Dest[_Size_bits - 1 - _Ix] = ((_Arr[_Ix >> 3] >> (_Ix & 7)) & 1) != 0 ? _Elem1 : _Elem0;
                }
            }
        }
    } // namespace _Bitset_to_string
} // unnamed namespace

extern "C" {

__declspec(noalias) void __stdcall __std_bitset_to_string_1(
    char* const _Dest, const void* const _Src, const size_t _Size_bits, const char _Elem0, const char _Elem1) noexcept {
    using namespace _Bitset_to_string;
    _Dispatch<_Traits_1_avx, _Traits_1_sse>(_Dest, _Src, _Size_bits, _Elem0, _Elem1);
}

__declspec(noalias) void __stdcall __std_bitset_to_string_2(wchar_t* const _Dest, const void* const _Src,
    const size_t _Size_bits, const wchar_t _Elem0, const wchar_t _Elem1) noexcept {
    using namespace _Bitset_to_string;
    _Dispatch<_Traits_2_avx, _Traits_2_sse>(_Dest, _Src, _Size_bits, _Elem0, _Elem1);
}

} // extern "C"

namespace {
    namespace _Bitset_from_string {
#ifdef _M_ARM64EC
        using _Traits_1_avx = void;
        using _Traits_1_sse = void;
        using _Traits_2_avx = void;
        using _Traits_2_sse = void;
#else // ^^^ defined(_M_ARM64EC) / !defined(_M_ARM64EC) vvv
        struct _Traits_avx {
            using _Guard = _Zeroupper_on_exit;
            using _Vec   = __m256i;

            static __m256i _Load(const void* _Src) noexcept {
                return _mm256_loadu_si256(reinterpret_cast<const __m256i*>(_Src));
            }

            static void _Store(void* _Dest, const __m256i _Val) noexcept {
                _mm256_storeu_si256(reinterpret_cast<__m256i*>(_Dest), _Val);
            }

            static bool _Check(const __m256i _Val, const __m256i _Ex1, const __m256i _Dx0) noexcept {
                return _mm256_testc_si256(_Ex1, _mm256_xor_si256(_Val, _Dx0));
            }
        };

        struct _Traits_sse {
            using _Guard = char;
            using _Vec   = __m128i;

            static __m128i _Load(const void* _Src) noexcept {
                return _mm_loadu_si128(reinterpret_cast<const __m128i*>(_Src));
            }

            static void _Store(void* _Dest, const __m128i _Val) noexcept {
                _mm_storeu_si128(reinterpret_cast<__m128i*>(_Dest), _Val);
            }

            static bool _Check(const __m128i _Val, const __m128i _Ex1, const __m128i _Dx0) noexcept {
                return _mm_testc_si128(_Ex1, _mm_xor_si128(_Val, _Dx0));
            }
        };

        struct _Traits_1_avx : _Traits_avx {
            static __m256i _Set(const char _Val) noexcept {
                return _mm256_set1_epi8(_Val);
            }

            static uint32_t _To_bits(const __m256i _Ex1) noexcept {
                const __m256i _Shuf = _mm256_set_epi8( //
                    0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, //
                    0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15);

                const __m256i _Ex2 = _mm256_shuffle_epi8(_Ex1, _Shuf);
                return _rotl(static_cast<uint32_t>(_mm256_movemask_epi8(_Ex2)), 16);
            }

            static __m256i _Cmp(const __m256i _Val, const __m256i _Dx1) noexcept {
                return _mm256_cmpeq_epi8(_Val, _Dx1);
            }
        };

        struct _Traits_1_sse : _Traits_sse {
            static __m128i _Set(const char _Val) noexcept {
                return _mm_shuffle_epi8(_mm_cvtsi32_si128(_Val), _mm_setzero_si128());
            }

            static uint16_t _To_bits(const __m128i _Ex1) noexcept {
                const __m128i _Shuf = _mm_set_epi8(0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15);
                const __m128i _Ex2  = _mm_shuffle_epi8(_Ex1, _Shuf);
                return static_cast<uint16_t>(_mm_movemask_epi8(_Ex2));
            }

            static __m128i _Cmp(const __m128i _Val, const __m128i _Dx1) noexcept {
                return _mm_cmpeq_epi8(_Val, _Dx1);
            }
        };

        struct _Traits_2_avx : _Traits_avx {
            static __m256i _Set(const wchar_t _Val) noexcept {
                return _mm256_set1_epi16(_Val);
            }

            static uint16_t _To_bits(const __m256i _Ex1) noexcept {
                const __m256i _Shuf = _mm256_set_epi8( //
                    +0, +2, +4, +6, +8, 10, 12, 14, -1, -1, -1, -1, -1, -1, -1, -1, //
                    -1, -1, -1, -1, -1, -1, -1, -1, +0, +2, +4, +6, +8, 10, 12, 14);

                const __m256i _Ex2 = _mm256_shuffle_epi8(_Ex1, _Shuf);
                return static_cast<uint16_t>(_rotl(static_cast<uint32_t>(_mm256_movemask_epi8(_Ex2)), 8));
            }

            static __m256i _Cmp(const __m256i _Val, const __m256i _Dx1) noexcept {
                return _mm256_cmpeq_epi16(_Val, _Dx1);
            }
        };

        struct _Traits_2_sse : _Traits_sse {
            static __m128i _Set(const wchar_t _Val) noexcept {
                return _mm_set1_epi16(_Val);
            }

            static uint8_t _To_bits(const __m128i _Ex1) noexcept {
                const __m128i _Shuf = _mm_set_epi8(-1, -1, -1, -1, -1, -1, -1, -1, 0, 2, 4, 6, 8, 10, 12, 14);
                const __m128i _Ex2  = _mm_shuffle_epi8(_Ex1, _Shuf);
                return static_cast<uint8_t>(_mm_movemask_epi8(_Ex2));
            }

            static __m128i _Cmp(const __m128i _Val, const __m128i _Dx1) noexcept {
                return _mm_cmpeq_epi16(_Val, _Dx1);
            }
        };

        template <class _Traits, class _Elem, class _OutFn>
        bool _Loop(const _Elem* const _Src, const _Elem* _Src_end, const typename _Traits::_Vec _Dx0,
            const typename _Traits::_Vec _Dx1, _OutFn _Out) noexcept {
            for (;;) {
                typename _Traits::_Vec _Val;
                constexpr size_t _Per_vec = sizeof(_Val) / sizeof(_Elem);

                if (const size_t _Left = _Src_end - _Src; _Left >= _Per_vec) {
                    _Src_end -= _Per_vec;
                    _Val = _Traits::_Load(_Src_end);
                } else if (_Left == 0) {
                    return true;
                } else {
                    _Src_end = _Src;
                    _Elem _Tmp[_Per_vec];
                    _Traits::_Store(_Tmp, _Dx0);
                    _Elem* const _Tmpd = _Tmp + (_Per_vec - _Left);
                    memcpy(_Tmpd, _Src_end, _Left * sizeof(_Elem));
                    _Val = _Traits::_Load(_Tmp);
                }

                const auto _Ex1 = _Traits::_Cmp(_Val, _Dx1);

                if (!_Traits::_Check(_Val, _Ex1, _Dx0)) {
                    return false;
                }

                _Out(_Ex1);
            }
        }

        template <class _Traits, class _Elem>
        bool _Impl(void* _Dest, const _Elem* const _Src, const size_t _Size_bytes, const size_t _Size_bits,
            const size_t _Size_chars, const _Elem _Elem0, const _Elem _Elem1) noexcept {
            [[maybe_unused]] typename _Traits::_Guard _Guard; // TRANSITION, DevCom-10331414
            const auto _Dx0 = _Traits::_Set(_Elem0);
            const auto _Dx1 = _Traits::_Set(_Elem1);

            void* _Dest_end = _Dest;
            _Advance_bytes(_Dest_end, _Size_bytes);

            auto _Out = [&_Dest](const _Traits::_Vec _Ex1) {
                const auto _Val = _Traits::_To_bits(_Ex1);
                memcpy(_Dest, &_Val, sizeof(_Val));
                _Advance_bytes(_Dest, sizeof(_Val));
            };

            const size_t _Size_convert = (_Size_chars <= _Size_bits) ? _Size_chars : _Size_bits;

            // Convert characters to bits
            if (!_Loop<_Traits>(_Src, _Src + _Size_convert, _Dx0, _Dx1, _Out)) {
                return false;
            }

            // Verify remaining characters, if any
            if (_Size_convert != _Size_chars
                && !_Loop<_Traits>(_Src + _Size_convert, _Src + _Size_chars, _Dx0, _Dx1, [](_Traits::_Vec) {})) {
                return false;
            }

            // Trim tail (may be padding tail, or too short string, or both)
            if (_Dest != _Dest_end) {
                memset(_Dest, 0, _Byte_length(_Dest, _Dest_end));
            }

            return true;
        }
#endif // ^^^ !defined(_M_ARM64EC) ^^^

        template <class _Elem>
        bool _Fallback(void* const _Dest, const _Elem* const _Src, const size_t _Size_bytes, const size_t _Size_bits,
            const size_t _Size_chars, const _Elem _Elem0, const _Elem _Elem1) noexcept {
            const auto _Dest_bytes = static_cast<uint8_t*>(_Dest);
            size_t _Size_convert   = _Size_chars;

            if (_Size_chars > _Size_bits) {
                _Size_convert = _Size_bits;

                for (size_t _Ix = _Size_bits; _Ix < _Size_chars; ++_Ix) {
                    if (const _Elem _Cur = _Src[_Ix]; _Cur != _Elem0 && _Cur != _Elem1) {
                        return false;
                    }
                }
            }

            memset(_Dest, 0, _Size_bytes);

            for (size_t _Ix = 0; _Ix != _Size_convert; ++_Ix) {
                const _Elem _Cur = _Src[_Size_convert - _Ix - 1];

                if (_Cur != _Elem0 && _Cur != _Elem1) {
                    return false;
                }

                _Dest_bytes[_Ix >> 3] |= static_cast<uint8_t>(_Cur == _Elem1) << (_Ix & 0x7);
            }

            return true;
        }

        template <class _Avx, class _Sse, class _Elem>
        bool _Dispatch(void* _Dest, const _Elem* _Src, size_t _Size_bytes, size_t _Size_bits, size_t _Size_chars,
            _Elem _Elem0, _Elem _Elem1) noexcept {
#ifndef _M_ARM64EC
            if (_Use_avx2() && _Size_bits >= 256) {
                return _Impl<_Avx>(_Dest, _Src, _Size_bytes, _Size_bits, _Size_chars, _Elem0, _Elem1);
            } else if (_Use_sse42()) {
                return _Impl<_Sse>(_Dest, _Src, _Size_bytes, _Size_bits, _Size_chars, _Elem0, _Elem1);
            } else
#endif // ^^^ !defined(_M_ARM64EC) ^^^
            {
                return _Fallback(_Dest, _Src, _Size_bytes, _Size_bits, _Size_chars, _Elem0, _Elem1);
            }
        }
    } // namespace _Bitset_from_string
} // unnamed namespace

extern "C" {

__declspec(noalias) bool __stdcall __std_bitset_from_string_1(void* const _Dest, const char* const _Src,
    const size_t _Size_bytes, const size_t _Size_bits, const size_t _Size_chars, const char _Elem0,
    const char _Elem1) noexcept {
    using namespace _Bitset_from_string;

    return _Dispatch<_Traits_1_avx, _Traits_1_sse>(_Dest, _Src, _Size_bytes, _Size_bits, _Size_chars, _Elem0, _Elem1);
}

__declspec(noalias) bool __stdcall __std_bitset_from_string_2(void* const _Dest, const wchar_t* const _Src,
    const size_t _Size_bytes, const size_t _Size_bits, const size_t _Size_chars, const wchar_t _Elem0,
    const wchar_t _Elem1) noexcept {
    using namespace _Bitset_from_string;

    return _Dispatch<_Traits_2_avx, _Traits_2_sse>(_Dest, _Src, _Size_bytes, _Size_bits, _Size_chars, _Elem0, _Elem1);
}

} // extern "C"
#endif // defined(_M_IX86) || defined(_M_X64)
