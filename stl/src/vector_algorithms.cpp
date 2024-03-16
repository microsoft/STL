// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifdef _M_CEE_PURE
#error _M_CEE_PURE should not be defined when compiling vector_algorithms.cpp.
#endif

#if defined(_M_IX86) || defined(_M_X64) // NB: includes _M_ARM64EC
#include <__msvc_minmax.hpp>
#include <cstdint>
#include <cstring>
#include <xtr1common>
#ifndef _M_ARM64EC
#include <intrin.h>
#include <isa_availability.h>

extern "C" long __isa_enabled;

#ifndef _DEBUG
#pragma optimize("t", on) // Override /Os with /Ot for this TU
#endif // !defined(_DEBUG)

namespace {
    bool _Use_avx2() noexcept {
        return __isa_enabled & (1 << __ISA_AVAILABLE_AVX2);
    }

    bool _Use_sse42() noexcept {
        return __isa_enabled & (1 << __ISA_AVAILABLE_SSE42);
    }

    bool _Use_sse2() noexcept {
#ifdef _M_IX86
        return __isa_enabled & (1 << __ISA_AVAILABLE_SSE2);
#else
        return true;
#endif
    }

    struct [[nodiscard]] _Zeroupper_on_exit { // TRANSITION, DevCom-10331414
        _Zeroupper_on_exit() = default;

        _Zeroupper_on_exit(const _Zeroupper_on_exit&)            = delete;
        _Zeroupper_on_exit& operator=(const _Zeroupper_on_exit&) = delete;

        ~_Zeroupper_on_exit() {
            _mm256_zeroupper();
        }
    };
} // namespace
#endif // !defined(_M_ARM64EC)

namespace {
    template <class _BidIt>
    void _Reverse_tail(_BidIt _First, _BidIt _Last) noexcept {
        for (; _First != _Last && _First != --_Last; ++_First) {
            const auto _Temp = *_First;
            *_First          = *_Last;
            *_Last           = _Temp;
        }
    }

    template <class _BidIt, class _OutIt>
    void _Reverse_copy_tail(_BidIt _First, _BidIt _Last, _OutIt _Dest) noexcept {
        while (_First != _Last) {
            *_Dest++ = *--_Last;
        }
    }

    size_t _Byte_length(const void* _First, const void* _Last) noexcept {
        return static_cast<const unsigned char*>(_Last) - static_cast<const unsigned char*>(_First);
    }

    void _Rewind_bytes(void*& _Target, size_t _Offset) noexcept {
        _Target = static_cast<unsigned char*>(_Target) - _Offset;
    }

    void _Rewind_bytes(const void*& _Target, size_t _Offset) noexcept {
        _Target = static_cast<const unsigned char*>(_Target) - _Offset;
    }

    template <class _Integral>
    void _Advance_bytes(void*& _Target, _Integral _Offset) noexcept {
        _Target = static_cast<unsigned char*>(_Target) + _Offset;
    }

    template <class _Integral>
    void _Advance_bytes(const void*& _Target, _Integral _Offset) noexcept {
        _Target = static_cast<const unsigned char*>(_Target) + _Offset;
    }
} // unnamed namespace

extern "C" {
__declspec(noalias) void __cdecl __std_swap_ranges_trivially_swappable_noalias(
    void* _First1, void* _Last1, void* _First2) noexcept {
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
    if (_Byte_length(_First1, _Last1) >= 16 && _Use_sse2()) {
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

#if defined(_M_X64) // NOTE: UNALIGNED MEMORY ACCESSES
    constexpr size_t _Mask_8 = ~((static_cast<size_t>(1) << 3) - 1);
    if (_Byte_length(_First1, _Last1) >= 8) {
        const void* _Stop_at = _First1;
        _Advance_bytes(_Stop_at, _Byte_length(_First1, _Last1) & _Mask_8);
        do {
            const unsigned long long _Left             = *static_cast<unsigned long long*>(_First1);
            const unsigned long long _Right            = *static_cast<unsigned long long*>(_First2);
            *static_cast<unsigned long long*>(_First1) = _Right;
            *static_cast<unsigned long long*>(_First2) = _Left;
            _Advance_bytes(_First1, 8);
            _Advance_bytes(_First2, 8);
        } while (_First1 != _Stop_at);
    }
#elif defined(_M_IX86) // NOTE: UNALIGNED MEMORY ACCESSES
    constexpr size_t _Mask_4 = ~((static_cast<size_t>(1) << 2) - 1);
    if (_Byte_length(_First1, _Last1) >= 4) {
        const void* _Stop_at = _First1;
        _Advance_bytes(_Stop_at, _Byte_length(_First1, _Last1) & _Mask_4);
        do {
            const unsigned long _Left             = *static_cast<unsigned long*>(_First1);
            const unsigned long _Right            = *static_cast<unsigned long*>(_First2);
            *static_cast<unsigned long*>(_First1) = _Right;
            *static_cast<unsigned long*>(_First2) = _Left;
            _Advance_bytes(_First1, 4);
            _Advance_bytes(_First2, 4);
        } while (_First1 != _Stop_at);
    }
#else
#error Unsupported architecture
#endif
#endif // !_M_ARM64EC

    auto _First1c = static_cast<unsigned char*>(_First1);
    auto _Last1c  = static_cast<unsigned char*>(_Last1);
    auto _First2c = static_cast<unsigned char*>(_First2);
    for (; _First1c != _Last1c; ++_First1c, ++_First2c) {
        unsigned char _Ch = *_First1c;
        *_First1c         = *_First2c;
        *_First2c         = _Ch;
    }
}

// TRANSITION, ABI: __std_swap_ranges_trivially_swappable() is preserved for binary compatibility
void* __cdecl __std_swap_ranges_trivially_swappable(void* _First1, void* _Last1, void* _First2) noexcept {
    __std_swap_ranges_trivially_swappable_noalias(_First1, _Last1, _First2);
    return static_cast<char*>(_First2) + (static_cast<char*>(_Last1) - static_cast<char*>(_First1));
}

__declspec(noalias) void __cdecl __std_reverse_trivially_swappable_1(void* _First, void* _Last) noexcept {
#ifndef _M_ARM64EC
    if (_Byte_length(_First, _Last) >= 64 && _Use_avx2()) {
        const __m256i _Reverse_char_lanes_avx = _mm256_set_epi8( //
            0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, //
            0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15);
        const void* _Stop_at                  = _First;
        _Advance_bytes(_Stop_at, (_Byte_length(_First, _Last) >> 1) & ~size_t{0x1F});
        do {
            _Advance_bytes(_Last, -32);
            // vpermq to load left and right, and transpose the lanes
            const __m256i _Left       = _mm256_loadu_si256(static_cast<__m256i*>(_First));
            const __m256i _Right      = _mm256_loadu_si256(static_cast<__m256i*>(_Last));
            const __m256i _Left_perm  = _mm256_permute4x64_epi64(_Left, _MM_SHUFFLE(1, 0, 3, 2));
            const __m256i _Right_perm = _mm256_permute4x64_epi64(_Right, _MM_SHUFFLE(1, 0, 3, 2));
            // transpose all the chars in the lanes
            const __m256i _Left_reversed  = _mm256_shuffle_epi8(_Left_perm, _Reverse_char_lanes_avx);
            const __m256i _Right_reversed = _mm256_shuffle_epi8(_Right_perm, _Reverse_char_lanes_avx);
            _mm256_storeu_si256(static_cast<__m256i*>(_First), _Right_reversed);
            _mm256_storeu_si256(static_cast<__m256i*>(_Last), _Left_reversed);
            _Advance_bytes(_First, 32);
        } while (_First != _Stop_at);

        _mm256_zeroupper(); // TRANSITION, DevCom-10331414
    }

    if (_Byte_length(_First, _Last) >= 32 && _Use_sse42()) {
        const __m128i _Reverse_char_sse = _mm_set_epi8(0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15);
        const void* _Stop_at            = _First;
        _Advance_bytes(_Stop_at, (_Byte_length(_First, _Last) >> 1) & ~size_t{0xF});
        do {
            _Advance_bytes(_Last, -16);
            const __m128i _Left           = _mm_loadu_si128(static_cast<__m128i*>(_First));
            const __m128i _Right          = _mm_loadu_si128(static_cast<__m128i*>(_Last));
            const __m128i _Left_reversed  = _mm_shuffle_epi8(_Left, _Reverse_char_sse); // SSSE3
            const __m128i _Right_reversed = _mm_shuffle_epi8(_Right, _Reverse_char_sse);
            _mm_storeu_si128(static_cast<__m128i*>(_First), _Right_reversed);
            _mm_storeu_si128(static_cast<__m128i*>(_Last), _Left_reversed);
            _Advance_bytes(_First, 16);
        } while (_First != _Stop_at);
    }
#endif // !_M_ARM64EC

    _Reverse_tail(static_cast<unsigned char*>(_First), static_cast<unsigned char*>(_Last));
}

__declspec(noalias) void __cdecl __std_reverse_trivially_swappable_2(void* _First, void* _Last) noexcept {
#ifndef _M_ARM64EC
    if (_Byte_length(_First, _Last) >= 64 && _Use_avx2()) {
        const __m256i _Reverse_short_lanes_avx = _mm256_set_epi8( //
            1, 0, 3, 2, 5, 4, 7, 6, 9, 8, 11, 10, 13, 12, 15, 14, //
            1, 0, 3, 2, 5, 4, 7, 6, 9, 8, 11, 10, 13, 12, 15, 14);
        const void* _Stop_at                   = _First;
        _Advance_bytes(_Stop_at, (_Byte_length(_First, _Last) >> 1) & ~size_t{0x1F});
        do {
            _Advance_bytes(_Last, -32);
            const __m256i _Left           = _mm256_loadu_si256(static_cast<__m256i*>(_First));
            const __m256i _Right          = _mm256_loadu_si256(static_cast<__m256i*>(_Last));
            const __m256i _Left_perm      = _mm256_permute4x64_epi64(_Left, _MM_SHUFFLE(1, 0, 3, 2));
            const __m256i _Right_perm     = _mm256_permute4x64_epi64(_Right, _MM_SHUFFLE(1, 0, 3, 2));
            const __m256i _Left_reversed  = _mm256_shuffle_epi8(_Left_perm, _Reverse_short_lanes_avx);
            const __m256i _Right_reversed = _mm256_shuffle_epi8(_Right_perm, _Reverse_short_lanes_avx);
            _mm256_storeu_si256(static_cast<__m256i*>(_First), _Right_reversed);
            _mm256_storeu_si256(static_cast<__m256i*>(_Last), _Left_reversed);
            _Advance_bytes(_First, 32);
        } while (_First != _Stop_at);

        _mm256_zeroupper(); // TRANSITION, DevCom-10331414
    }

    if (_Byte_length(_First, _Last) >= 32 && _Use_sse42()) {
        const __m128i _Reverse_short_sse = _mm_set_epi8(1, 0, 3, 2, 5, 4, 7, 6, 9, 8, 11, 10, 13, 12, 15, 14);
        const void* _Stop_at             = _First;
        _Advance_bytes(_Stop_at, (_Byte_length(_First, _Last) >> 1) & ~size_t{0xF});
        do {
            _Advance_bytes(_Last, -16);
            const __m128i _Left           = _mm_loadu_si128(static_cast<__m128i*>(_First));
            const __m128i _Right          = _mm_loadu_si128(static_cast<__m128i*>(_Last));
            const __m128i _Left_reversed  = _mm_shuffle_epi8(_Left, _Reverse_short_sse); // SSSE3
            const __m128i _Right_reversed = _mm_shuffle_epi8(_Right, _Reverse_short_sse);
            _mm_storeu_si128(static_cast<__m128i*>(_First), _Right_reversed);
            _mm_storeu_si128(static_cast<__m128i*>(_Last), _Left_reversed);
            _Advance_bytes(_First, 16);
        } while (_First != _Stop_at);
    }
#endif // !_M_ARM64EC

    _Reverse_tail(static_cast<unsigned short*>(_First), static_cast<unsigned short*>(_Last));
}

__declspec(noalias) void __cdecl __std_reverse_trivially_swappable_4(void* _First, void* _Last) noexcept {
#ifndef _M_ARM64EC
    if (_Byte_length(_First, _Last) >= 64 && _Use_avx2()) {
        const void* _Stop_at = _First;
        _Advance_bytes(_Stop_at, (_Byte_length(_First, _Last) >> 1) & ~size_t{0x1F});
        const __m256i _Shuf = _mm256_set_epi32(0, 1, 2, 3, 4, 5, 6, 7);
        do {
            _Advance_bytes(_Last, -32);
            const __m256i _Left           = _mm256_loadu_si256(static_cast<__m256i*>(_First));
            const __m256i _Right          = _mm256_loadu_si256(static_cast<__m256i*>(_Last));
            const __m256i _Left_reversed  = _mm256_permutevar8x32_epi32(_Left, _Shuf);
            const __m256i _Right_reversed = _mm256_permutevar8x32_epi32(_Right, _Shuf);
            _mm256_storeu_si256(static_cast<__m256i*>(_First), _Right_reversed);
            _mm256_storeu_si256(static_cast<__m256i*>(_Last), _Left_reversed);
            _Advance_bytes(_First, 32);
        } while (_First != _Stop_at);

        _mm256_zeroupper(); // TRANSITION, DevCom-10331414
    }

    if (_Byte_length(_First, _Last) >= 32 && _Use_sse2()) {
        const void* _Stop_at = _First;
        _Advance_bytes(_Stop_at, (_Byte_length(_First, _Last) >> 1) & ~size_t{0xF});
        do {
            _Advance_bytes(_Last, -16);
            const __m128i _Left           = _mm_loadu_si128(static_cast<__m128i*>(_First));
            const __m128i _Right          = _mm_loadu_si128(static_cast<__m128i*>(_Last));
            const __m128i _Left_reversed  = _mm_shuffle_epi32(_Left, _MM_SHUFFLE(0, 1, 2, 3));
            const __m128i _Right_reversed = _mm_shuffle_epi32(_Right, _MM_SHUFFLE(0, 1, 2, 3));
            _mm_storeu_si128(static_cast<__m128i*>(_First), _Right_reversed);
            _mm_storeu_si128(static_cast<__m128i*>(_Last), _Left_reversed);
            _Advance_bytes(_First, 16);
        } while (_First != _Stop_at);
    }
#endif // !_M_ARM64EC

    _Reverse_tail(static_cast<unsigned long*>(_First), static_cast<unsigned long*>(_Last));
}

__declspec(noalias) void __cdecl __std_reverse_trivially_swappable_8(void* _First, void* _Last) noexcept {
#ifndef _M_ARM64EC
    if (_Byte_length(_First, _Last) >= 64 && _Use_avx2()) {
        const void* _Stop_at = _First;
        _Advance_bytes(_Stop_at, (_Byte_length(_First, _Last) >> 1) & ~size_t{0x1F});
        do {
            _Advance_bytes(_Last, -32);
            const __m256i _Left           = _mm256_loadu_si256(static_cast<__m256i*>(_First));
            const __m256i _Right          = _mm256_loadu_si256(static_cast<__m256i*>(_Last));
            const __m256i _Left_reversed  = _mm256_permute4x64_epi64(_Left, _MM_SHUFFLE(0, 1, 2, 3));
            const __m256i _Right_reversed = _mm256_permute4x64_epi64(_Right, _MM_SHUFFLE(0, 1, 2, 3));
            _mm256_storeu_si256(static_cast<__m256i*>(_First), _Right_reversed);
            _mm256_storeu_si256(static_cast<__m256i*>(_Last), _Left_reversed);
            _Advance_bytes(_First, 32);
        } while (_First != _Stop_at);

        _mm256_zeroupper(); // TRANSITION, DevCom-10331414
    }

    if (_Byte_length(_First, _Last) >= 32 && _Use_sse2()) {
        const void* _Stop_at = _First;
        _Advance_bytes(_Stop_at, (_Byte_length(_First, _Last) >> 1) & ~size_t{0xF});
        do {
            _Advance_bytes(_Last, -16);
            const __m128i _Left           = _mm_loadu_si128(static_cast<__m128i*>(_First));
            const __m128i _Right          = _mm_loadu_si128(static_cast<__m128i*>(_Last));
            const __m128i _Left_reversed  = _mm_shuffle_epi32(_Left, _MM_SHUFFLE(1, 0, 3, 2));
            const __m128i _Right_reversed = _mm_shuffle_epi32(_Right, _MM_SHUFFLE(1, 0, 3, 2));
            _mm_storeu_si128(static_cast<__m128i*>(_First), _Right_reversed);
            _mm_storeu_si128(static_cast<__m128i*>(_Last), _Left_reversed);
            _Advance_bytes(_First, 16);
        } while (_First != _Stop_at);
    }
#endif // !_M_ARM64EC

    _Reverse_tail(static_cast<unsigned long long*>(_First), static_cast<unsigned long long*>(_Last));
}

__declspec(noalias) void __cdecl __std_reverse_copy_trivially_copyable_1(
    const void* _First, const void* _Last, void* _Dest) noexcept {
#ifndef _M_ARM64EC
    if (_Byte_length(_First, _Last) >= 32 && _Use_avx2()) {
        const __m256i _Reverse_char_lanes_avx = _mm256_set_epi8( //
            0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, //
            0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15);
        const void* _Stop_at                  = _Dest;
        _Advance_bytes(_Stop_at, _Byte_length(_First, _Last) & ~size_t{0x1F});
        do {
            _Advance_bytes(_Last, -32);
            const __m256i _Block          = _mm256_loadu_si256(static_cast<const __m256i*>(_Last));
            const __m256i _Block_permuted = _mm256_permute4x64_epi64(_Block, _MM_SHUFFLE(1, 0, 3, 2));
            const __m256i _Block_reversed = _mm256_shuffle_epi8(_Block_permuted, _Reverse_char_lanes_avx);
            _mm256_storeu_si256(static_cast<__m256i*>(_Dest), _Block_reversed);
            _Advance_bytes(_Dest, 32);
        } while (_Dest != _Stop_at);

        _mm256_zeroupper(); // TRANSITION, DevCom-10331414
    }

    if (_Byte_length(_First, _Last) >= 16 && _Use_sse42()) {
        const __m128i _Reverse_char_sse = _mm_set_epi8(0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15);
        const void* _Stop_at            = _Dest;
        _Advance_bytes(_Stop_at, _Byte_length(_First, _Last) & ~size_t{0xF});
        do {
            _Advance_bytes(_Last, -16);
            const __m128i _Block          = _mm_loadu_si128(static_cast<const __m128i*>(_Last));
            const __m128i _Block_reversed = _mm_shuffle_epi8(_Block, _Reverse_char_sse); // SSSE3
            _mm_storeu_si128(static_cast<__m128i*>(_Dest), _Block_reversed);
            _Advance_bytes(_Dest, 16);
        } while (_Dest != _Stop_at);
    }
#endif // !_M_ARM64EC

    _Reverse_copy_tail(static_cast<const unsigned char*>(_First), static_cast<const unsigned char*>(_Last),
        static_cast<unsigned char*>(_Dest));
}

__declspec(noalias) void __cdecl __std_reverse_copy_trivially_copyable_2(
    const void* _First, const void* _Last, void* _Dest) noexcept {
#ifndef _M_ARM64EC
    if (_Byte_length(_First, _Last) >= 32 && _Use_avx2()) {
        const __m256i _Reverse_short_lanes_avx = _mm256_set_epi8( //
            1, 0, 3, 2, 5, 4, 7, 6, 9, 8, 11, 10, 13, 12, 15, 14, //
            1, 0, 3, 2, 5, 4, 7, 6, 9, 8, 11, 10, 13, 12, 15, 14);
        const void* _Stop_at                   = _Dest;
        _Advance_bytes(_Stop_at, _Byte_length(_First, _Last) & ~size_t{0x1F});
        do {
            _Advance_bytes(_Last, -32);
            const __m256i _Block          = _mm256_loadu_si256(static_cast<const __m256i*>(_Last));
            const __m256i _Block_permuted = _mm256_permute4x64_epi64(_Block, _MM_SHUFFLE(1, 0, 3, 2));
            const __m256i _Block_reversed = _mm256_shuffle_epi8(_Block_permuted, _Reverse_short_lanes_avx);
            _mm256_storeu_si256(static_cast<__m256i*>(_Dest), _Block_reversed);
            _Advance_bytes(_Dest, 32);
        } while (_Dest != _Stop_at);

        _mm256_zeroupper(); // TRANSITION, DevCom-10331414
    }

    if (_Byte_length(_First, _Last) >= 16 && _Use_sse42()) {
        const __m128i _Reverse_short_sse = _mm_set_epi8(1, 0, 3, 2, 5, 4, 7, 6, 9, 8, 11, 10, 13, 12, 15, 14);
        const void* _Stop_at             = _Dest;
        _Advance_bytes(_Stop_at, _Byte_length(_First, _Last) & ~size_t{0xF});
        do {
            _Advance_bytes(_Last, -16);
            const __m128i _Block          = _mm_loadu_si128(static_cast<const __m128i*>(_Last));
            const __m128i _Block_reversed = _mm_shuffle_epi8(_Block, _Reverse_short_sse); // SSSE3
            _mm_storeu_si128(static_cast<__m128i*>(_Dest), _Block_reversed);
            _Advance_bytes(_Dest, 16);
        } while (_Dest != _Stop_at);
    }
#endif // !_M_ARM64EC

    _Reverse_copy_tail(static_cast<const unsigned short*>(_First), static_cast<const unsigned short*>(_Last),
        static_cast<unsigned short*>(_Dest));
}

__declspec(noalias) void __cdecl __std_reverse_copy_trivially_copyable_4(
    const void* _First, const void* _Last, void* _Dest) noexcept {
#ifndef _M_ARM64EC
    if (_Byte_length(_First, _Last) >= 32 && _Use_avx2()) {
        const void* _Stop_at = _Dest;
        _Advance_bytes(_Stop_at, _Byte_length(_First, _Last) & ~size_t{0x1F});
        const __m256i _Shuf = _mm256_set_epi32(0, 1, 2, 3, 4, 5, 6, 7);
        do {
            _Advance_bytes(_Last, -32);
            const __m256i _Block          = _mm256_loadu_si256(static_cast<const __m256i*>(_Last));
            const __m256i _Block_reversed = _mm256_permutevar8x32_epi32(_Block, _Shuf);
            _mm256_storeu_si256(static_cast<__m256i*>(_Dest), _Block_reversed);
            _Advance_bytes(_Dest, 32);
        } while (_Dest != _Stop_at);

        _mm256_zeroupper(); // TRANSITION, DevCom-10331414
    }

    if (_Byte_length(_First, _Last) >= 16 && _Use_sse2()) {
        const void* _Stop_at = _Dest;
        _Advance_bytes(_Stop_at, _Byte_length(_First, _Last) & ~size_t{0xF});
        do {
            _Advance_bytes(_Last, -16);
            const __m128i _Block          = _mm_loadu_si128(static_cast<const __m128i*>(_Last));
            const __m128i _Block_reversed = _mm_shuffle_epi32(_Block, _MM_SHUFFLE(0, 1, 2, 3));
            _mm_storeu_si128(static_cast<__m128i*>(_Dest), _Block_reversed);
            _Advance_bytes(_Dest, 16);
        } while (_Dest != _Stop_at);
    }
#endif // !_M_ARM64EC

    _Reverse_copy_tail(static_cast<const unsigned long*>(_First), static_cast<const unsigned long*>(_Last),
        static_cast<unsigned long*>(_Dest));
}

__declspec(noalias) void __cdecl __std_reverse_copy_trivially_copyable_8(
    const void* _First, const void* _Last, void* _Dest) noexcept {
#ifndef _M_ARM64EC
    if (_Byte_length(_First, _Last) >= 32 && _Use_avx2()) {
        const void* _Stop_at = _Dest;
        _Advance_bytes(_Stop_at, _Byte_length(_First, _Last) & ~size_t{0x1F});
        do {
            _Advance_bytes(_Last, -32);
            const __m256i _Block          = _mm256_loadu_si256(static_cast<const __m256i*>(_Last));
            const __m256i _Block_reversed = _mm256_permute4x64_epi64(_Block, _MM_SHUFFLE(0, 1, 2, 3));
            _mm256_storeu_si256(static_cast<__m256i*>(_Dest), _Block_reversed);
            _Advance_bytes(_Dest, 32);
        } while (_Dest != _Stop_at);

        _mm256_zeroupper(); // TRANSITION, DevCom-10331414
    }

    if (_Byte_length(_First, _Last) >= 16 && _Use_sse2()) {
        const void* _Stop_at = _Dest;
        _Advance_bytes(_Stop_at, _Byte_length(_First, _Last) & ~size_t{0xF});
        do {
            _Advance_bytes(_Last, -16);
            const __m128i _Block          = _mm_loadu_si128(static_cast<const __m128i*>(_Last));
            const __m128i _Block_reversed = _mm_shuffle_epi32(_Block, _MM_SHUFFLE(1, 0, 3, 2));
            _mm_storeu_si128(static_cast<__m128i*>(_Dest), _Block_reversed);
            _Advance_bytes(_Dest, 16);
        } while (_Dest != _Stop_at);
    }
#endif // !_M_ARM64EC

    _Reverse_copy_tail(static_cast<const unsigned long long*>(_First), static_cast<const unsigned long long*>(_Last),
        static_cast<unsigned long long*>(_Dest));
}

} // extern "C"

namespace {
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

    enum _Min_max_mode {
        _Mode_min  = 1 << 0,
        _Mode_max  = 1 << 1,
        _Mode_both = _Mode_min | _Mode_max,
    };

    struct _Minmax_traits_1 {
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

        static __m128i _Load(const void* _Src) noexcept {
            return _mm_loadu_si128(reinterpret_cast<const __m128i*>(_Src));
        }

        static __m128i _Sign_correction(const __m128i _Val, const bool _Sign) noexcept {
            alignas(16) static constexpr _Unsigned_t _Sign_corrections[2][16] = {
                {0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80}, {}};
            return _mm_sub_epi8(_Val, _mm_load_si128(reinterpret_cast<const __m128i*>(_Sign_corrections[_Sign])));
        }

        static __m128i _Inc(__m128i _Idx) noexcept {
            return _mm_add_epi8(_Idx, _mm_set1_epi8(1));
        }

        template <class _Fn>
        static __m128i _H_func(const __m128i _Cur, _Fn _Funct) noexcept {
            const __m128i _Shuf_bytes = _mm_set_epi8(14, 15, 12, 13, 10, 11, 8, 9, 6, 7, 4, 5, 2, 3, 0, 1);
            const __m128i _Shuf_words = _mm_set_epi8(13, 12, 15, 14, 9, 8, 11, 10, 5, 4, 7, 6, 1, 0, 3, 2);

            __m128i _H_min_val = _Cur;
            _H_min_val         = _Funct(_H_min_val, _mm_shuffle_epi32(_H_min_val, _MM_SHUFFLE(1, 0, 3, 2)));
            _H_min_val         = _Funct(_H_min_val, _mm_shuffle_epi32(_H_min_val, _MM_SHUFFLE(2, 3, 0, 1)));
            _H_min_val         = _Funct(_H_min_val, _mm_shuffle_epi8(_H_min_val, _Shuf_words)); // SSSE3
            _H_min_val         = _Funct(_H_min_val, _mm_shuffle_epi8(_H_min_val, _Shuf_bytes)); // SSSE3
            return _H_min_val;
        }

        static __m128i _H_min(const __m128i _Cur) noexcept {
            return _H_func(_Cur, [](__m128i _Val1, __m128i _Val2) { return _mm_min_epi8(_Val1, _Val2); }); // SSE4.1
        }

        static __m128i _H_max(const __m128i _Cur) noexcept {
            return _H_func(_Cur, [](__m128i _Val1, __m128i _Val2) { return _mm_max_epi8(_Val1, _Val2); }); // SSE4.1
        }

        static __m128i _H_min_u(const __m128i _Cur) noexcept {
            return _H_func(_Cur, [](__m128i _Val1, __m128i _Val2) { return _mm_min_epu8(_Val1, _Val2); });
        }

        static __m128i _H_max_u(const __m128i _Cur) noexcept {
            return _H_func(_Cur, [](__m128i _Val1, __m128i _Val2) { return _mm_max_epu8(_Val1, _Val2); });
        }

        static _Signed_t _Get_any(const __m128i _Cur) noexcept {
            return static_cast<_Signed_t>(_mm_cvtsi128_si32(_Cur));
        }

        static _Unsigned_t _Get_v_pos(const __m128i _Idx, const unsigned long _H_pos) noexcept {
            // _mm_shuffle_epi8 is SSSE3:
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
            return _mm_min_epi8(_First, _Second); // SSE4.1
        }

        static __m128i _Max(const __m128i _First, const __m128i _Second, __m128i = _mm_undefined_si128()) noexcept {
            return _mm_max_epi8(_First, _Second); // SSE4.1
        }

        static __m128i _Min_u(const __m128i _First, const __m128i _Second) noexcept {
            return _mm_min_epu8(_First, _Second);
        }

        static __m128i _Max_u(const __m128i _First, const __m128i _Second) noexcept {
            return _mm_max_epu8(_First, _Second);
        }

        static __m128i _Mask_cast(__m128i _Mask) noexcept {
            return _Mask;
        }
#endif // !_M_ARM64EC
    };

    struct _Minmax_traits_2 {
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

        static __m128i _Load(const void* _Src) noexcept {
            return _mm_loadu_si128(reinterpret_cast<const __m128i*>(_Src));
        }

        static __m128i _Sign_correction(const __m128i _Val, const bool _Sign) noexcept {
            alignas(16) static constexpr _Unsigned_t _Sign_corrections[2][8] = {
                0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, {}};
            return _mm_sub_epi16(_Val, _mm_load_si128(reinterpret_cast<const __m128i*>(_Sign_corrections[_Sign])));
        }

        static __m128i _Inc(__m128i _Idx) noexcept {
            return _mm_add_epi16(_Idx, _mm_set1_epi16(1));
        }

        template <class _Fn>
        static __m128i _H_func(const __m128i _Cur, _Fn _Funct) noexcept {
            const __m128i _Shuf_words = _mm_set_epi8(13, 12, 15, 14, 9, 8, 11, 10, 5, 4, 7, 6, 1, 0, 3, 2);

            __m128i _H_min_val = _Cur;
            _H_min_val         = _Funct(_H_min_val, _mm_shuffle_epi32(_H_min_val, _MM_SHUFFLE(1, 0, 3, 2)));
            _H_min_val         = _Funct(_H_min_val, _mm_shuffle_epi32(_H_min_val, _MM_SHUFFLE(2, 3, 0, 1)));
            _H_min_val         = _Funct(_H_min_val, _mm_shuffle_epi8(_H_min_val, _Shuf_words)); // SSSE3
            return _H_min_val;
        }

        static __m128i _H_min(const __m128i _Cur) noexcept {
            return _H_func(_Cur, [](__m128i _Val1, __m128i _Val2) { return _mm_min_epi16(_Val1, _Val2); });
        }

        static __m128i _H_max(const __m128i _Cur) noexcept {
            return _H_func(_Cur, [](__m128i _Val1, __m128i _Val2) { return _mm_max_epi16(_Val1, _Val2); });
        }

        static __m128i _H_min_u(const __m128i _Cur) noexcept {
            return _H_func(_Cur, [](__m128i _Val1, __m128i _Val2) { return _mm_min_epu16(_Val1, _Val2); }); // SSE4.1
        }

        static __m128i _H_max_u(const __m128i _Cur) noexcept {
            return _H_func(_Cur, [](__m128i _Val1, __m128i _Val2) { return _mm_max_epu16(_Val1, _Val2); }); // SSE4.1
        }

        static _Signed_t _Get_any(const __m128i _Cur) noexcept {
            return static_cast<_Signed_t>(_mm_cvtsi128_si32(_Cur));
        }

        static _Unsigned_t _Get_v_pos(const __m128i _Idx, const unsigned long _H_pos) noexcept {
            static constexpr _Unsigned_t _Shuf[] = {0x0100, 0x0302, 0x0504, 0x0706, 0x0908, 0x0B0A, 0x0D0C, 0x0F0E};

            // _mm_shuffle_epi8 is SSSE3:
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
            return _mm_min_epu16(_First, _Second); // SSE4.1
        }

        static __m128i _Max_u(const __m128i _First, const __m128i _Second) noexcept {
            return _mm_max_epu16(_First, _Second); // SSE4.1
        }

        static __m128i _Mask_cast(__m128i _Mask) noexcept {
            return _Mask;
        }
#endif // !_M_ARM64EC
    };

    struct _Minmax_traits_4 {
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

        static __m128i _Load(const void* _Src) noexcept {
            return _mm_loadu_si128(reinterpret_cast<const __m128i*>(_Src));
        }

        static __m128i _Sign_correction(const __m128i _Val, const bool _Sign) noexcept {
            alignas(16) static constexpr _Unsigned_t _Sign_corrections[2][4] = {
                0x8000'0000UL, 0x8000'0000UL, 0x8000'0000UL, 0x8000'0000UL, {}};
            return _mm_sub_epi32(_Val, _mm_load_si128(reinterpret_cast<const __m128i*>(_Sign_corrections[_Sign])));
        }

        static __m128i _Inc(__m128i _Idx) noexcept {
            return _mm_add_epi32(_Idx, _mm_set1_epi32(1));
        }

        template <class _Fn>
        static __m128i _H_func(const __m128i _Cur, _Fn _Funct) noexcept {
            __m128i _H_min_val = _Cur;
            _H_min_val         = _Funct(_H_min_val, _mm_shuffle_epi32(_H_min_val, _MM_SHUFFLE(1, 0, 3, 2)));
            _H_min_val         = _Funct(_H_min_val, _mm_shuffle_epi32(_H_min_val, _MM_SHUFFLE(2, 3, 0, 1)));
            return _H_min_val;
        }

        static __m128i _H_min(const __m128i _Cur) noexcept {
            return _H_func(_Cur, [](__m128i _Val1, __m128i _Val2) { return _mm_min_epi32(_Val1, _Val2); }); // SSE4.1
        }

        static __m128i _H_max(const __m128i _Cur) noexcept {
            return _H_func(_Cur, [](__m128i _Val1, __m128i _Val2) { return _mm_max_epi32(_Val1, _Val2); }); // SSE4.1
        }

        static __m128i _H_min_u(const __m128i _Cur) noexcept {
            return _H_func(_Cur, [](__m128i _Val1, __m128i _Val2) { return _mm_min_epu32(_Val1, _Val2); }); // SSE4.1
        }

        static __m128i _H_max_u(const __m128i _Cur) noexcept {
            return _H_func(_Cur, [](__m128i _Val1, __m128i _Val2) { return _mm_max_epu32(_Val1, _Val2); }); // SSE4.1
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
            return _mm_min_epi32(_First, _Second); // SSE4.1
        }

        static __m128i _Max(const __m128i _First, const __m128i _Second, __m128i = _mm_undefined_si128()) noexcept {
            return _mm_max_epi32(_First, _Second); // SSE4.1
        }

        static __m128i _Min_u(const __m128i _First, const __m128i _Second) noexcept {
            return _mm_min_epu32(_First, _Second); // SSE4.1
        }

        static __m128i _Max_u(const __m128i _First, const __m128i _Second) noexcept {
            return _mm_max_epu32(_First, _Second); // SSE4.1
        }

        static __m128i _Mask_cast(__m128i _Mask) noexcept {
            return _Mask;
        }
#endif // !_M_ARM64EC
    };

    struct _Minmax_traits_8 {
        static constexpr bool _Is_floating = false;

        using _Signed_t   = int64_t;
        using _Unsigned_t = uint64_t;

        static constexpr _Signed_t _Init_min_val = static_cast<_Signed_t>(0x7FFF'FFFF'FFFF'FFFFULL);
        static constexpr _Signed_t _Init_max_val = static_cast<_Signed_t>(0x8000'0000'0000'0000ULL);

        using _Minmax_i_t = _Min_max_8i;
        using _Minmax_u_t = _Min_max_8u;

#ifndef _M_ARM64EC
        static constexpr bool _Has_portion_max = false;

        static __m128i _Load(const void* _Src) noexcept {
            return _mm_loadu_si128(reinterpret_cast<const __m128i*>(_Src));
        }

        static __m128i _Sign_correction(const __m128i _Val, const bool _Sign) noexcept {
            alignas(16) static constexpr _Unsigned_t _Sign_corrections[2][2] = {
                0x8000'0000'0000'0000ULL, 0x8000'0000'0000'0000ULL, {}};
            return _mm_sub_epi64(_Val, _mm_load_si128(reinterpret_cast<const __m128i*>(_Sign_corrections[_Sign])));
        }

        static __m128i _Inc(__m128i _Idx) noexcept {
            return _mm_add_epi64(_Idx, _mm_set1_epi64x(1));
        }

        template <class _Fn>
        static __m128i _H_func(const __m128i _Cur, _Fn _Funct) noexcept {
            _Signed_t _H_min_a = _Get_any(_Cur);
            _Signed_t _H_min_b = _Get_any(_mm_bsrli_si128(_Cur, 8));
            if (_Funct(_H_min_b, _H_min_a)) {
                _H_min_a = _H_min_b;
            }
            return _mm_set1_epi64x(_H_min_a);
        }

        static __m128i _H_min(const __m128i _Cur) noexcept {
            return _H_func(_Cur, [](_Signed_t _Lhs, _Signed_t _Rhs) { return _Lhs < _Rhs; });
        }

        static __m128i _H_max(const __m128i _Cur) noexcept {
            return _H_func(_Cur, [](_Signed_t _Lhs, _Signed_t _Rhs) { return _Lhs > _Rhs; });
        }

        static __m128i _H_min_u(const __m128i _Cur) noexcept {
            return _H_func(_Cur, [](_Unsigned_t _Lhs, _Unsigned_t _Rhs) { return _Lhs < _Rhs; });
        }

        static __m128i _H_max_u(const __m128i _Cur) noexcept {
            return _H_func(_Cur, [](_Unsigned_t _Lhs, _Unsigned_t _Rhs) { return _Lhs > _Rhs; });
        }

        static _Signed_t _Get_any(const __m128i _Cur) noexcept {
#ifdef _M_IX86
            return static_cast<_Signed_t>(
                (static_cast<_Unsigned_t>(static_cast<uint32_t>(_mm_extract_epi32(_Cur, 1))) << 32) // SSE4.1
                | static_cast<_Unsigned_t>(static_cast<uint32_t>(_mm_cvtsi128_si32(_Cur))));
#else // ^^^ x86 / x64 vvv
            return static_cast<_Signed_t>(_mm_cvtsi128_si64(_Cur));
#endif // ^^^ x64 ^^^
        }

        static _Unsigned_t _Get_v_pos(const __m128i _Idx, const unsigned long _H_pos) noexcept {
            _Unsigned_t _Array[2];
            _mm_storeu_si128(reinterpret_cast<__m128i*>(&_Array), _Idx);
            return _Array[_H_pos >> 3];
        }

        static __m128i _Cmp_eq(const __m128i _First, const __m128i _Second) noexcept {
            return _mm_cmpeq_epi64(_First, _Second); // SSE4.1
        }

        static __m128i _Cmp_gt(const __m128i _First, const __m128i _Second) noexcept {
            return _mm_cmpgt_epi64(_First, _Second); // SSE4.2
        }

        static __m128i _Cmp_eq_idx(const __m128i _First, const __m128i _Second) noexcept {
            return _mm_cmpeq_epi64(_First, _Second); // SSE4.1
        }

        static __m128i _Min(const __m128i _First, const __m128i _Second, const __m128i _Mask) noexcept {
            return _mm_blendv_epi8(_First, _Second, _Mask); // SSE4.1
        }

        static __m128i _Max(const __m128i _First, const __m128i _Second, const __m128i _Mask) noexcept {
            return _mm_blendv_epi8(_First, _Second, _Mask); // SSE4.1
        }

        static __m128i _Min(const __m128i _First, const __m128i _Second) noexcept {
            return _mm_blendv_epi8(_First, _Second, _Cmp_gt(_First, _Second)); // _Cmp_gt is SSE4.2
        }

        static __m128i _Max(const __m128i _First, const __m128i _Second) noexcept {
            return _mm_blendv_epi8(_First, _Second, _Cmp_gt(_Second, _First)); // _Cmp_gt is SSE4.2
        }

        static __m128i _Mask_cast(__m128i _Mask) noexcept {
            return _Mask;
        }
#endif // !_M_ARM64EC
    };

    struct _Minmax_traits_f {
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

        static __m128 _Load(const void* _Src) noexcept {
            return _mm_loadu_ps(reinterpret_cast<const float*>(_Src));
        }

        static __m128 _Sign_correction(const __m128 _Val, bool) noexcept {
            return _Val;
        }

        static __m128i _Inc(__m128i _Idx) noexcept {
            return _mm_add_epi32(_Idx, _mm_set1_epi32(1));
        }

        template <class _Fn>
        static __m128 _H_func(const __m128 _Cur, _Fn _Funct) noexcept {
            __m128 _H_min_val = _Cur;
            _H_min_val        = _Funct(_H_min_val, _mm_shuffle_ps(_H_min_val, _H_min_val, _MM_SHUFFLE(1, 0, 3, 2)));
            _H_min_val        = _Funct(_H_min_val, _mm_shuffle_ps(_H_min_val, _H_min_val, _MM_SHUFFLE(2, 3, 0, 1)));
            return _H_min_val;
        }

        template <class _Fn>
        static __m128i _H_func_u(const __m128i _Cur, _Fn _Funct) noexcept {
            __m128i _H_min_val = _Cur;
            _H_min_val         = _Funct(_H_min_val, _mm_shuffle_epi32(_H_min_val, _MM_SHUFFLE(1, 0, 3, 2)));
            _H_min_val         = _Funct(_H_min_val, _mm_shuffle_epi32(_H_min_val, _MM_SHUFFLE(2, 3, 0, 1)));
            return _H_min_val;
        }

        static __m128 _H_min(const __m128 _Cur) noexcept {
            return _H_func(_Cur, [](__m128 _Val1, __m128 _Val2) { return _mm_min_ps(_Val1, _Val2); });
        }

        static __m128 _H_max(const __m128 _Cur) noexcept {
            return _H_func(_Cur, [](__m128 _Val1, __m128 _Val2) { return _mm_max_ps(_Val1, _Val2); });
        }

        static __m128i _H_min_u(const __m128i _Cur) noexcept {
            return _H_func_u(_Cur, [](__m128i _Val1, __m128i _Val2) { return _mm_min_epu32(_Val1, _Val2); }); // SSE4.1
        }

        static __m128i _H_max_u(const __m128i _Cur) noexcept {
            return _H_func_u(_Cur, [](__m128i _Val1, __m128i _Val2) { return _mm_max_epu32(_Val1, _Val2); }); // SSE4.1
        }

        static float _Get_any(const __m128 _Cur) noexcept {
            return _mm_cvtss_f32(_Cur);
        }

        static uint32_t _Get_v_pos(const __m128i _Idx, const unsigned long _H_pos) noexcept {
            uint32_t _Array[4];
            _mm_storeu_si128(reinterpret_cast<__m128i*>(&_Array), _Idx);
            return _Array[_H_pos >> 2];
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
            return _mm_min_ps(_First, _Second);
        }

        static __m128 _Max(const __m128 _First, const __m128 _Second, __m128 = _mm_undefined_ps()) noexcept {
            return _mm_max_ps(_First, _Second);
        }

        static __m128i _Mask_cast(__m128 _Mask) noexcept {
            return _mm_castps_si128(_Mask);
        }
#endif // !_M_ARM64EC
    };

    struct _Minmax_traits_d {
        static constexpr bool _Is_floating = true;

        using _Signed_t   = double;
        using _Unsigned_t = void;

        static constexpr _Signed_t _Init_min_val = __builtin_huge_val();
        static constexpr _Signed_t _Init_max_val = -__builtin_huge_val();

        using _Minmax_i_t = _Min_max_d;
        using _Minmax_u_t = void;

#ifndef _M_ARM64EC
        static constexpr bool _Has_portion_max = false;

        static __m128d _Load(const void* _Src) noexcept {
            return _mm_loadu_pd(reinterpret_cast<const double*>(_Src));
        }

        static __m128d _Sign_correction(const __m128d _Val, bool) noexcept {
            return _Val;
        }

        static __m128i _Inc(__m128i _Idx) noexcept {
            return _mm_add_epi64(_Idx, _mm_set1_epi64x(1));
        }

        template <class _Fn>
        static __m128d _H_func(const __m128d _Cur, _Fn _Funct) noexcept {
            __m128d _H_min_val = _Cur;
            _H_min_val         = _Funct(_H_min_val, _mm_shuffle_pd(_H_min_val, _H_min_val, 1));
            return _H_min_val;
        }

        template <class _Fn>
        static __m128i _H_func_u(const __m128i _Cur, _Fn _Funct) noexcept {
            uint64_t _H_min_a = _Get_any_u(_Cur);
            uint64_t _H_min_b = _Get_any_u(_mm_bsrli_si128(_Cur, 8));
            if (_Funct(_H_min_b, _H_min_a)) {
                _H_min_a = _H_min_b;
            }
            return _mm_set1_epi64x(_H_min_a);
        }

        static __m128d _H_min(const __m128d _Cur) noexcept {
            return _H_func(_Cur, [](__m128d _Val1, __m128d _Val2) { return _mm_min_pd(_Val1, _Val2); });
        }

        static __m128d _H_max(const __m128d _Cur) noexcept {
            return _H_func(_Cur, [](__m128d _Val1, __m128d _Val2) { return _mm_max_pd(_Val1, _Val2); });
        }

        static __m128i _H_min_u(const __m128i _Cur) noexcept {
            return _H_func_u(_Cur, [](uint64_t _Lhs, uint64_t _Rhs) { return _Lhs < _Rhs; });
        }

        static __m128i _H_max_u(const __m128i _Cur) noexcept {
            return _H_func_u(_Cur, [](uint64_t _Lhs, uint64_t _Rhs) { return _Lhs > _Rhs; });
        }
        static double _Get_any(const __m128d _Cur) noexcept {
            return _mm_cvtsd_f64(_Cur);
        }

        static uint64_t _Get_any_u(const __m128i _Cur) noexcept {
#ifdef _M_IX86
            return (static_cast<uint64_t>(static_cast<uint32_t>(_mm_extract_epi32(_Cur, 1))) << 32) // SSE4.1
                 | static_cast<uint64_t>(static_cast<uint32_t>(_mm_cvtsi128_si32(_Cur)));
#else // ^^^ x86 / x64 vvv
            return static_cast<uint64_t>(_mm_cvtsi128_si64(_Cur));
#endif // ^^^ x64 ^^^
        }

        static uint64_t _Get_v_pos(const __m128i _Idx, const unsigned long _H_pos) noexcept {
            uint64_t _Array[2];
            _mm_storeu_si128(reinterpret_cast<__m128i*>(&_Array), _Idx);
            return _Array[_H_pos >> 3];
        }

        static __m128d _Cmp_eq(const __m128d _First, const __m128d _Second) noexcept {
            return _mm_cmpeq_pd(_First, _Second);
        }

        static __m128d _Cmp_gt(const __m128d _First, const __m128d _Second) noexcept {
            return _mm_cmpgt_pd(_First, _Second);
        }

        static __m128i _Cmp_eq_idx(const __m128i _First, const __m128i _Second) noexcept {
            return _mm_cmpeq_epi64(_First, _Second); // SSE4.1
        }

        static __m128d _Min(const __m128d _First, const __m128d _Second, __m128d = _mm_undefined_pd()) noexcept {
            return _mm_min_pd(_First, _Second);
        }

        static __m128d _Max(const __m128d _First, const __m128d _Second, __m128d = _mm_undefined_pd()) noexcept {
            return _mm_max_pd(_First, _Second);
        }

        static __m128i _Mask_cast(__m128d _Mask) noexcept {
            return _mm_castpd_si128(_Mask);
        }
#endif // !_M_ARM64EC
    };

    // _Minmax_element has exactly the same signature as the extern "C" functions
    // (__std_min_element_N, __std_max_element_N, __std_minmax_element_N), up to calling convention.
    // This makes sure the template specialization is fused with the extern "C" function.
    // In optimized builds it avoids an extra call, as this function is too large to inline.
    template <_Min_max_mode _Mode, class _Traits>
    auto __stdcall _Minmax_element(const void* _First, const void* const _Last, const bool _Sign) noexcept {
        _Min_max_element_t _Res = {_First, _First};
        auto _Cur_min_val       = _Traits::_Init_min_val;
        auto _Cur_max_val       = _Traits::_Init_max_val;

#ifndef _M_ARM64EC
        auto _Base = static_cast<const char*>(_First);

        if (_Byte_length(_First, _Last) >= 16 && _Use_sse42()) {
            size_t _Portion_byte_size = _Byte_length(_First, _Last) & ~size_t{0xF};

            if constexpr (_Traits::_Has_portion_max) {
                // vector of indices will wrap around at exactly this size
                constexpr size_t _Max_portion_byte_size = _Traits::_Portion_max * 16;
                if (_Portion_byte_size > _Max_portion_byte_size) {
                    _Portion_byte_size = _Max_portion_byte_size;
                }
            }

            const void* _Stop_at = _First;
            _Advance_bytes(_Stop_at, _Portion_byte_size);

            // Load values and if unsigned adjust them to be signed (for signed vector comparisons)
            auto _Cur_vals     = _Traits::_Sign_correction(_Traits::_Load(_First), _Sign);
            auto _Cur_vals_min = _Cur_vals; // vector of vertical minimum values
            auto _Cur_idx_min  = _mm_setzero_si128(); // vector of vertical minimum indices
            auto _Cur_vals_max = _Cur_vals; // vector of vertical maximum values
            auto _Cur_idx_max  = _mm_setzero_si128(); // vector of vertical maximum indices
            auto _Cur_idx      = _mm_setzero_si128(); // current vector of indices

            for (;;) {
                _Advance_bytes(_First, 16);

                // Increment vertical indices. Will stop at exactly wrap around, if not reach the end before
                _Cur_idx = _Traits::_Inc(_Cur_idx);

                if (_First != _Stop_at) {
                    // This is the main part, finding vertical minimum/maximum

                    // Load values and if unsigned adjust them to be signed (for signed vector comparisons)
                    _Cur_vals = _Traits::_Sign_correction(_Traits::_Load(_First), _Sign);

                    if constexpr ((_Mode & _Mode_min) != 0) {
                        // Looking for the first occurrence of minimum, don't overwrite with newly found occurrences
                        const auto _Is_less = _Traits::_Cmp_gt(_Cur_vals_min, _Cur_vals); // _Cur_vals < _Cur_vals_min
                        _Cur_idx_min        = _mm_blendv_epi8(
                            _Cur_idx_min, _Cur_idx, _Traits::_Mask_cast(_Is_less)); // Remember their vertical indices
                        _Cur_vals_min = _Traits::_Min(_Cur_vals_min, _Cur_vals, _Is_less); // Update the current minimum
                    }

                    if constexpr (_Mode == _Mode_max) {
                        // Looking for the first occurrence of maximum, don't overwrite with newly found occurrences
                        const auto _Is_greater =
                            _Traits::_Cmp_gt(_Cur_vals, _Cur_vals_max); // _Cur_vals > _Cur_vals_max
                        _Cur_idx_max = _mm_blendv_epi8(_Cur_idx_max, _Cur_idx,
                            _Traits::_Mask_cast(_Is_greater)); // Remember their vertical indices
                        _Cur_vals_max =
                            _Traits::_Max(_Cur_vals_max, _Cur_vals, _Is_greater); // Update the current maximum
                    } else if constexpr (_Mode == _Mode_both) {
                        // Looking for the last occurrence of maximum, do overwrite with newly found occurrences
                        const auto _Is_less =
                            _Traits::_Cmp_gt(_Cur_vals_max, _Cur_vals); // !(_Cur_vals >= _Cur_vals_max)
                        _Cur_idx_max  = _mm_blendv_epi8(_Cur_idx, _Cur_idx_max,
                             _Traits::_Mask_cast(_Is_less)); // Remember their vertical indices
                        _Cur_vals_max = _Traits::_Max(_Cur_vals, _Cur_vals_max, _Is_less); // Update the current maximum
                    }
                } else {
                    // Reached end or indices wrap around point.
                    // Compute horizontal min and/or max. Determine horizontal and vertical position of it.

                    if constexpr ((_Mode & _Mode_min) != 0) {
                        const auto _H_min = _Traits::_H_min(_Cur_vals_min); // Vector populated by the smallest element
                        const auto _H_min_val = _Traits::_Get_any(_H_min); // Get any element of it

                        if (_H_min_val < _Cur_min_val) { // Current horizontal min is less than the old
                            _Cur_min_val = _H_min_val; // update min
                            const auto _Eq_mask =
                                _Traits::_Cmp_eq(_H_min, _Cur_vals_min); // Mask of all elems eq to min
                            int _Mask = _mm_movemask_epi8(_Traits::_Mask_cast(_Eq_mask));
                            // Indices of minimum elements or the greatest index if none
                            const auto _All_max = _mm_set1_epi8(static_cast<char>(0xFF));
                            const auto _Idx_min_val =
                                _mm_blendv_epi8(_All_max, _Cur_idx_min, _Traits::_Mask_cast(_Eq_mask));
                            auto _Idx_min = _Traits::_H_min_u(_Idx_min_val); // The smallest indices
                            // Select the smallest vertical indices from the smallest element mask
                            _Mask &= _mm_movemask_epi8(_Traits::_Cmp_eq_idx(_Idx_min, _Idx_min_val));
                            unsigned long _H_pos;

                            // Find the smallest horizontal index
                            _BitScanForward(&_H_pos, _Mask); // lgtm [cpp/conditionallyuninitializedvariable]

                            const auto _V_pos = _Traits::_Get_v_pos(_Cur_idx_min, _H_pos); // Extract its vertical index
                            _Res._Min =
                                _Base + static_cast<size_t>(_V_pos) * 16 + _H_pos; // Finally, compute the pointer
                        }
                    }

                    if constexpr ((_Mode & _Mode_max) != 0) {
                        const auto _H_max = _Traits::_H_max(_Cur_vals_max); // Vector populated by the largest element
                        const auto _H_max_val = _Traits::_Get_any(_H_max); // Get any element of it

                        if (_Mode == _Mode_both && _Cur_max_val <= _H_max_val
                            || _Mode == _Mode_max && _Cur_max_val < _H_max_val) {
                            // max_element: current horizontal max is greater than the old, update max
                            // minmax_element: current horizontal max is not less than the old, update max
                            _Cur_max_val = _H_max_val;
                            const auto _Eq_mask =
                                _Traits::_Cmp_eq(_H_max, _Cur_vals_max); // Mask of all elems eq to max
                            int _Mask = _mm_movemask_epi8(_Traits::_Mask_cast(_Eq_mask));

                            unsigned long _H_pos;
                            if constexpr (_Mode == _Mode_both) {
                                // Looking for the last occurrence of maximum
                                // Indices of maximum elements or zero if none
                                const auto _Idx_max_val =
                                    _mm_blendv_epi8(_mm_setzero_si128(), _Cur_idx_max, _Traits::_Mask_cast(_Eq_mask));
                                const auto _Idx_max = _Traits::_H_max_u(_Idx_max_val); // The greatest indices
                                // Select the greatest vertical indices from the largest element mask
                                _Mask &= _mm_movemask_epi8(_Traits::_Cmp_eq_idx(_Idx_max, _Idx_max_val));

                                // Find the largest horizontal index
                                _BitScanReverse(&_H_pos, _Mask); // lgtm [cpp/conditionallyuninitializedvariable]

                                _H_pos -= sizeof(_Cur_max_val) - 1; // Correct from highest val bit to lowest
                            } else {
                                // Looking for the first occurrence of maximum
                                // Indices of maximum elements or the greatest index if none
                                const auto _All_max = _mm_set1_epi8(static_cast<char>(0xFF));
                                const auto _Idx_max_val =
                                    _mm_blendv_epi8(_All_max, _Cur_idx_max, _Traits::_Mask_cast(_Eq_mask));
                                const auto _Idx_max = _Traits::_H_min_u(_Idx_max_val); // The smallest indices
                                // Select the smallest vertical indices from the largest element mask
                                _Mask &= _mm_movemask_epi8(_Traits::_Cmp_eq_idx(_Idx_max, _Idx_max_val));

                                // Find the smallest horizontal index
                                _BitScanForward(&_H_pos, _Mask); // lgtm [cpp/conditionallyuninitializedvariable]
                            }

                            const auto _V_pos = _Traits::_Get_v_pos(_Cur_idx_max, _H_pos); // Extract its vertical index
                            _Res._Max =
                                _Base + static_cast<size_t>(_V_pos) * 16 + _H_pos; // Finally, compute the pointer
                        }
                    }
                    // Horizontal part done, results are saved, now need to see if there is another portion to process

                    if constexpr (_Traits::_Has_portion_max) {
                        // Either the last portion or wrapping point reached, need to determine
                        _Portion_byte_size = _Byte_length(_First, _Last) & ~size_t{0xF};
                        if (_Portion_byte_size == 0) {
                            break; // That was the last portion
                        }
                        // Start next portion to handle the wrapping indices. Assume _Cur_idx is zero
                        constexpr size_t _Max_portion_byte_size = _Traits::_Portion_max * 16;
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
                            _Cur_idx_min  = _mm_setzero_si128();
                        }

                        if constexpr ((_Mode & _Mode_max) != 0) {
                            _Cur_vals_max = _Cur_vals;
                            _Cur_idx_max  = _mm_setzero_si128();
                        }
                    } else {
                        break; // No wrapping, so it was the only portion
                    }
                }
            }
        }
#endif // !_M_ARM64EC

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

    // _Minmax has exactly the same signature as the extern "C" functions
    // (__std_min_Nn, __std_max_Nn, __std_minmax_Nn), up to calling convention.
    // This makes sure the template specialization is fused with the extern "C" function.
    // In optimized builds it avoids an extra call, as this function is too large to inline.
    template <_Min_max_mode _Mode, class _Traits, bool _Sign>
    auto __stdcall _Minmax(const void* _First, const void* const _Last) noexcept {
        using _Ty = std::conditional_t<_Sign, typename _Traits::_Signed_t, typename _Traits::_Unsigned_t>;

        _Ty _Cur_min_val; // initialized in both of the branches below
        _Ty _Cur_max_val; // initialized in both of the branches below

#ifndef _M_ARM64EC
        // We don't have unsigned 64-bit stuff, so we'll use sign correction just for that case
        constexpr bool _Sign_correction = sizeof(_Ty) == 8 && !_Sign;

        if (_Byte_length(_First, _Last) >= 16 && _Use_sse42()) {
            const size_t _Sse_byte_size = _Byte_length(_First, _Last) & ~size_t{0xF};

            const void* _Stop_at = _First;
            _Advance_bytes(_Stop_at, _Sse_byte_size);

            auto _Cur_vals = _Traits::_Load(_First);

            if constexpr (_Sign_correction) {
                _Cur_vals = _Traits::_Sign_correction(_Cur_vals, false);
            }

            auto _Cur_vals_min = _Cur_vals; // vector of vertical minimum values
            auto _Cur_vals_max = _Cur_vals; // vector of vertical maximum values

            for (;;) {
                _Advance_bytes(_First, 16);

                if (_First != _Stop_at) {
                    // This is the main part, finding vertical minimum/maximum

                    _Cur_vals = _Traits::_Load(_First);

                    if constexpr (_Sign_correction) {
                        _Cur_vals = _Traits::_Sign_correction(_Cur_vals, false);
                    }

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
                } else {
                    // Reached end. Compute horizontal min and/or max.

                    if constexpr ((_Mode & _Mode_min) != 0) {
                        if constexpr (_Sign || _Sign_correction) {
                            const auto _H_min =
                                _Traits::_H_min(_Cur_vals_min); // Vector populated by the smallest element
                            _Cur_min_val = _Traits::_Get_any(_H_min); // Get any element of it
                        } else {
                            const auto _H_min =
                                _Traits::_H_min_u(_Cur_vals_min); // Vector populated by the smallest element
                            _Cur_min_val = _Traits::_Get_any(_H_min); // Get any element of it
                        }
                    }

                    if constexpr ((_Mode & _Mode_max) != 0) {
                        if constexpr (_Sign || _Sign_correction) {
                            const auto _H_max =
                                _Traits::_H_max(_Cur_vals_max); // Vector populated by the largest element
                            _Cur_max_val = _Traits::_Get_any(_H_max); // Get any element of it
                        } else {
                            const auto _H_max =
                                _Traits::_H_max_u(_Cur_vals_max); // Vector populated by the largest element
                            _Cur_max_val = _Traits::_Get_any(_H_max); // Get any element of it
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
        } else
#endif // !_M_ARM64EC
        {
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

} // unnamed namespace

extern "C" {

const void* __stdcall __std_min_element_1(
    const void* const _First, const void* const _Last, const bool _Signed) noexcept {
    return _Minmax_element<_Mode_min, _Minmax_traits_1>(_First, _Last, _Signed);
}

const void* __stdcall __std_min_element_2(
    const void* const _First, const void* const _Last, const bool _Signed) noexcept {
    return _Minmax_element<_Mode_min, _Minmax_traits_2>(_First, _Last, _Signed);
}

const void* __stdcall __std_min_element_4(
    const void* const _First, const void* const _Last, const bool _Signed) noexcept {
    return _Minmax_element<_Mode_min, _Minmax_traits_4>(_First, _Last, _Signed);
}

const void* __stdcall __std_min_element_8(
    const void* const _First, const void* const _Last, const bool _Signed) noexcept {
    return _Minmax_element<_Mode_min, _Minmax_traits_8>(_First, _Last, _Signed);
}

const void* __stdcall __std_min_element_f( // _Minmax_element's "signature" comment explains `bool _Unused`
    const void* const _First, const void* const _Last, const bool _Unused) noexcept {
    return _Minmax_element<_Mode_min, _Minmax_traits_f>(_First, _Last, _Unused);
}

const void* __stdcall __std_min_element_d( // _Minmax_element's "signature" comment explains `bool _Unused`
    const void* const _First, const void* const _Last, const bool _Unused) noexcept {
    return _Minmax_element<_Mode_min, _Minmax_traits_d>(_First, _Last, _Unused);
}

const void* __stdcall __std_max_element_1(
    const void* const _First, const void* const _Last, const bool _Signed) noexcept {
    return _Minmax_element<_Mode_max, _Minmax_traits_1>(_First, _Last, _Signed);
}

const void* __stdcall __std_max_element_2(
    const void* const _First, const void* const _Last, const bool _Signed) noexcept {
    return _Minmax_element<_Mode_max, _Minmax_traits_2>(_First, _Last, _Signed);
}

const void* __stdcall __std_max_element_4(
    const void* const _First, const void* const _Last, const bool _Signed) noexcept {
    return _Minmax_element<_Mode_max, _Minmax_traits_4>(_First, _Last, _Signed);
}

const void* __stdcall __std_max_element_8(
    const void* const _First, const void* const _Last, const bool _Signed) noexcept {
    return _Minmax_element<_Mode_max, _Minmax_traits_8>(_First, _Last, _Signed);
}

const void* __stdcall __std_max_element_f( // _Minmax_element's "signature" comment explains `bool _Unused`
    const void* const _First, const void* const _Last, const bool _Unused) noexcept {
    return _Minmax_element<_Mode_max, _Minmax_traits_f>(_First, _Last, _Unused);
}

const void* __stdcall __std_max_element_d( // _Minmax_element's "signature" comment explains `bool _Unused`
    const void* const _First, const void* const _Last, const bool _Unused) noexcept {
    return _Minmax_element<_Mode_max, _Minmax_traits_d>(_First, _Last, _Unused);
}

_Min_max_element_t __stdcall __std_minmax_element_1(
    const void* const _First, const void* const _Last, const bool _Signed) noexcept {
    return _Minmax_element<_Mode_both, _Minmax_traits_1>(_First, _Last, _Signed);
}

_Min_max_element_t __stdcall __std_minmax_element_2(
    const void* const _First, const void* const _Last, const bool _Signed) noexcept {
    return _Minmax_element<_Mode_both, _Minmax_traits_2>(_First, _Last, _Signed);
}

_Min_max_element_t __stdcall __std_minmax_element_4(
    const void* const _First, const void* const _Last, const bool _Signed) noexcept {
    return _Minmax_element<_Mode_both, _Minmax_traits_4>(_First, _Last, _Signed);
}

_Min_max_element_t __stdcall __std_minmax_element_8(
    const void* const _First, const void* const _Last, const bool _Signed) noexcept {
    return _Minmax_element<_Mode_both, _Minmax_traits_8>(_First, _Last, _Signed);
}

_Min_max_element_t __stdcall __std_minmax_element_f( // _Minmax_element's "signature" comment explains `bool _Unused`
    const void* const _First, const void* const _Last, const bool _Unused) noexcept {
    return _Minmax_element<_Mode_both, _Minmax_traits_f>(_First, _Last, _Unused);
}

_Min_max_element_t __stdcall __std_minmax_element_d( // _Minmax_element's "signature" comment explains `bool _Unused`
    const void* const _First, const void* const _Last, const bool _Unused) noexcept {
    return _Minmax_element<_Mode_both, _Minmax_traits_d>(_First, _Last, _Unused);
}

__declspec(noalias) int8_t __stdcall __std_min_1i(const void* const _First, const void* const _Last) noexcept {
    return _Minmax<_Mode_min, _Minmax_traits_1, true>(_First, _Last);
}

__declspec(noalias) uint8_t __stdcall __std_min_1u(const void* const _First, const void* const _Last) noexcept {
    return _Minmax<_Mode_min, _Minmax_traits_1, false>(_First, _Last);
}

__declspec(noalias) int16_t __stdcall __std_min_2i(const void* const _First, const void* const _Last) noexcept {
    return _Minmax<_Mode_min, _Minmax_traits_2, true>(_First, _Last);
}

__declspec(noalias) uint16_t __stdcall __std_min_2u(const void* const _First, const void* const _Last) noexcept {
    return _Minmax<_Mode_min, _Minmax_traits_2, false>(_First, _Last);
}

__declspec(noalias) int32_t __stdcall __std_min_4i(const void* const _First, const void* const _Last) noexcept {
    return _Minmax<_Mode_min, _Minmax_traits_4, true>(_First, _Last);
}

__declspec(noalias) uint32_t __stdcall __std_min_4u(const void* const _First, const void* const _Last) noexcept {
    return _Minmax<_Mode_min, _Minmax_traits_4, false>(_First, _Last);
}

__declspec(noalias) int64_t __stdcall __std_min_8i(const void* const _First, const void* const _Last) noexcept {
    return _Minmax<_Mode_min, _Minmax_traits_8, true>(_First, _Last);
}

__declspec(noalias) uint64_t __stdcall __std_min_8u(const void* const _First, const void* const _Last) noexcept {
    return _Minmax<_Mode_min, _Minmax_traits_8, false>(_First, _Last);
}

__declspec(noalias) float __stdcall __std_min_f(const void* const _First, const void* const _Last) noexcept {
    return _Minmax<_Mode_min, _Minmax_traits_f, true>(_First, _Last);
}

__declspec(noalias) double __stdcall __std_min_d(const void* const _First, const void* const _Last) noexcept {
    return _Minmax<_Mode_min, _Minmax_traits_d, true>(_First, _Last);
}

__declspec(noalias) int8_t __stdcall __std_max_1i(const void* const _First, const void* const _Last) noexcept {
    return _Minmax<_Mode_max, _Minmax_traits_1, true>(_First, _Last);
}

__declspec(noalias) uint8_t __stdcall __std_max_1u(const void* const _First, const void* const _Last) noexcept {
    return _Minmax<_Mode_max, _Minmax_traits_1, false>(_First, _Last);
}

__declspec(noalias) int16_t __stdcall __std_max_2i(const void* const _First, const void* const _Last) noexcept {
    return _Minmax<_Mode_max, _Minmax_traits_2, true>(_First, _Last);
}

__declspec(noalias) uint16_t __stdcall __std_max_2u(const void* const _First, const void* const _Last) noexcept {
    return _Minmax<_Mode_max, _Minmax_traits_2, false>(_First, _Last);
}

__declspec(noalias) int32_t __stdcall __std_max_4i(const void* const _First, const void* const _Last) noexcept {
    return _Minmax<_Mode_max, _Minmax_traits_4, true>(_First, _Last);
}

__declspec(noalias) uint32_t __stdcall __std_max_4u(const void* const _First, const void* const _Last) noexcept {
    return _Minmax<_Mode_max, _Minmax_traits_4, false>(_First, _Last);
}

__declspec(noalias) int64_t __stdcall __std_max_8i(const void* const _First, const void* const _Last) noexcept {
    return _Minmax<_Mode_max, _Minmax_traits_8, true>(_First, _Last);
}

__declspec(noalias) uint64_t __stdcall __std_max_8u(const void* const _First, const void* const _Last) noexcept {
    return _Minmax<_Mode_max, _Minmax_traits_8, false>(_First, _Last);
}

__declspec(noalias) float __stdcall __std_max_f(const void* const _First, const void* const _Last) noexcept {
    return _Minmax<_Mode_max, _Minmax_traits_f, true>(_First, _Last);
}

__declspec(noalias) double __stdcall __std_max_d(const void* const _First, const void* const _Last) noexcept {
    return _Minmax<_Mode_max, _Minmax_traits_d, true>(_First, _Last);
}

__declspec(noalias) _Min_max_1i __stdcall __std_minmax_1i(const void* const _First, const void* const _Last) noexcept {
    return _Minmax<_Mode_both, _Minmax_traits_1, true>(_First, _Last);
}

__declspec(noalias) _Min_max_1u __stdcall __std_minmax_1u(const void* const _First, const void* const _Last) noexcept {
    return _Minmax<_Mode_both, _Minmax_traits_1, false>(_First, _Last);
}

__declspec(noalias) _Min_max_2i __stdcall __std_minmax_2i(const void* const _First, const void* const _Last) noexcept {
    return _Minmax<_Mode_both, _Minmax_traits_2, true>(_First, _Last);
}

__declspec(noalias) _Min_max_2u __stdcall __std_minmax_2u(const void* const _First, const void* const _Last) noexcept {
    return _Minmax<_Mode_both, _Minmax_traits_2, false>(_First, _Last);
}

__declspec(noalias) _Min_max_4i __stdcall __std_minmax_4i(const void* const _First, const void* const _Last) noexcept {
    return _Minmax<_Mode_both, _Minmax_traits_4, true>(_First, _Last);
}

__declspec(noalias) _Min_max_4u __stdcall __std_minmax_4u(const void* const _First, const void* const _Last) noexcept {
    return _Minmax<_Mode_both, _Minmax_traits_4, false>(_First, _Last);
}

__declspec(noalias) _Min_max_8i __stdcall __std_minmax_8i(const void* const _First, const void* const _Last) noexcept {
    return _Minmax<_Mode_both, _Minmax_traits_8, true>(_First, _Last);
}

__declspec(noalias) _Min_max_8u __stdcall __std_minmax_8u(const void* const _First, const void* const _Last) noexcept {
    return _Minmax<_Mode_both, _Minmax_traits_8, false>(_First, _Last);
}

__declspec(noalias) _Min_max_f __stdcall __std_minmax_f(const void* const _First, const void* const _Last) noexcept {
    return _Minmax<_Mode_both, _Minmax_traits_f, true>(_First, _Last);
}

__declspec(noalias) _Min_max_d __stdcall __std_minmax_d(const void* const _First, const void* const _Last) noexcept {
    return _Minmax<_Mode_both, _Minmax_traits_d, true>(_First, _Last);
}

} // extern "C"

namespace {
    struct _Find_traits_1 {
        static constexpr size_t _Shift = 0;

#ifndef _M_ARM64EC
        static __m256i _Set_avx(const uint8_t _Val) noexcept {
            return _mm256_set1_epi8(_Val);
        }

        static __m128i _Set_sse(const uint8_t _Val) noexcept {
            return _mm_set1_epi8(_Val);
        }

        static __m256i _Cmp_avx(const __m256i _Lhs, const __m256i _Rhs) noexcept {
            return _mm256_cmpeq_epi8(_Lhs, _Rhs);
        }

        static __m128i _Cmp_sse(const __m128i _Lhs, const __m128i _Rhs) noexcept {
            return _mm_cmpeq_epi8(_Lhs, _Rhs);
        }

        static bool _Sse_available() noexcept {
            return _Use_sse2();
        }
#endif // !_M_ARM64EC
    };

    struct _Find_traits_2 {
        static constexpr size_t _Shift = 1;

#ifndef _M_ARM64EC
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

        static bool _Sse_available() noexcept {
            return _Use_sse2();
        }
#endif // !_M_ARM64EC
    };

    struct _Find_traits_4 {
        static constexpr size_t _Shift = 2;

#ifndef _M_ARM64EC
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

        static bool _Sse_available() noexcept {
            return _Use_sse2();
        }
#endif // !_M_ARM64EC
    };

    struct _Find_traits_8 {
        static constexpr size_t _Shift = 3;

#ifndef _M_ARM64EC
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
            return _mm_cmpeq_epi64(_Lhs, _Rhs); // SSE4.1
        }

        static bool _Sse_available() noexcept {
            return _Use_sse42(); // for pcmpeqq on _Cmp_sse
        }
#endif // !_M_ARM64EC
    };

    // The below functions have exactly the same signature as the extern "C" functions, up to calling convention.
    // This makes sure the template specialization is fused with the extern "C" function.
    // In optimized builds it avoids an extra call, as this function is too large to inline.

    template <class _Traits, class _Ty>
    const void* __stdcall __std_find_trivial_unsized_impl(const void* _First, const _Ty _Val) noexcept {
#ifndef _M_ARM64EC
        if ((reinterpret_cast<uintptr_t>(_First) & (sizeof(_Ty) - 1)) != 0) {
            // _First isn't aligned to sizeof(_Ty), so we need to use the scalar fallback below.
            // This can happen with 8-byte elements on x86's 4-aligned stack. It can also happen with packed structs.
        } else if (_Use_avx2()) {
            _Zeroupper_on_exit _Guard; // TRANSITION, DevCom-10331414

            // We read by vector-sized pieces, and we align pointers to vector-sized boundary.
            // From start partial piece we mask out matches that don't belong to the range.
            // This makes sure we never cross page boundary, thus we read 'as if' sequentially.
            constexpr size_t _Vector_pad_mask = 0x1F;
            constexpr unsigned int _Full_mask = 0xFFFF'FFFF;

            const __m256i _Comparand  = _Traits::_Set_avx(_Val);
            const intptr_t _Pad_start = reinterpret_cast<intptr_t>(_First) & _Vector_pad_mask;
            const unsigned int _Mask  = _Full_mask << _Pad_start;
            _Advance_bytes(_First, -_Pad_start);

            __m256i _Data       = _mm256_load_si256(static_cast<const __m256i*>(_First));
            unsigned int _Bingo = static_cast<unsigned int>(_mm256_movemask_epi8(_Traits::_Cmp_avx(_Data, _Comparand)));

            _Bingo &= _Mask;

            for (;;) {
                if (_Bingo != 0) {
                    unsigned long _Offset = _tzcnt_u32(_Bingo);
                    _Advance_bytes(_First, _Offset);
                    return _First;
                }

                _Advance_bytes(_First, 32);

                _Data  = _mm256_load_si256(static_cast<const __m256i*>(_First));
                _Bingo = static_cast<unsigned int>(_mm256_movemask_epi8(_Traits::_Cmp_avx(_Data, _Comparand)));
            }
        } else if (_Traits::_Sse_available()) {
            // We read by vector-sized pieces, and we align pointers to vector-sized boundary.
            // From start partial piece we mask out matches that don't belong to the range.
            // This makes sure we never cross page boundary, thus we read 'as if' sequentially.
            constexpr size_t _Vector_pad_mask = 0xF;
            constexpr unsigned int _Full_mask = 0xFFFF;

            const __m128i _Comparand  = _Traits::_Set_sse(_Val);
            const intptr_t _Pad_start = reinterpret_cast<intptr_t>(_First) & _Vector_pad_mask;
            const unsigned int _Mask  = _Full_mask << _Pad_start;
            _Advance_bytes(_First, -_Pad_start);

            __m128i _Data       = _mm_load_si128(static_cast<const __m128i*>(_First));
            unsigned int _Bingo = static_cast<unsigned int>(_mm_movemask_epi8(_Traits::_Cmp_sse(_Data, _Comparand)));

            _Bingo &= _Mask;

            for (;;) {
                if (_Bingo != 0) {
                    unsigned long _Offset;
                    _BitScanForward(&_Offset, _Bingo); // lgtm [cpp/conditionallyuninitializedvariable]
                    _Advance_bytes(_First, _Offset);
                    return _First;
                }

                _Advance_bytes(_First, 16);

                _Data  = _mm_load_si128(static_cast<const __m128i*>(_First));
                _Bingo = static_cast<unsigned int>(_mm_movemask_epi8(_Traits::_Cmp_sse(_Data, _Comparand)));
            }
        }
#endif // !_M_ARM64EC
        auto _Ptr = static_cast<const _Ty*>(_First);
        while (*_Ptr != _Val) {
            ++_Ptr;
        }
        return _Ptr;
    }

    template <class _Traits, class _Ty>
    const void* __stdcall __std_find_trivial_impl(const void* _First, const void* _Last, _Ty _Val) noexcept {
#ifndef _M_ARM64EC
        size_t _Size_bytes = _Byte_length(_First, _Last);

        const size_t _Avx_size = _Size_bytes & ~size_t{0x1F};
        if (_Avx_size != 0 && _Use_avx2()) {
            _Zeroupper_on_exit _Guard; // TRANSITION, DevCom-10331414

            const __m256i _Comparand = _Traits::_Set_avx(_Val);
            const void* _Stop_at     = _First;
            _Advance_bytes(_Stop_at, _Avx_size);
            do {
                const __m256i _Data = _mm256_loadu_si256(static_cast<const __m256i*>(_First));
                const int _Bingo    = _mm256_movemask_epi8(_Traits::_Cmp_avx(_Data, _Comparand));

                if (_Bingo != 0) {
                    const unsigned long _Offset = _tzcnt_u32(_Bingo);
                    _Advance_bytes(_First, _Offset);
                    return _First;
                }

                _Advance_bytes(_First, 32);
            } while (_First != _Stop_at);
            _Size_bytes &= 0x1F;
        }

        const size_t _Sse_size = _Size_bytes & ~size_t{0xF};
        if (_Sse_size != 0 && _Traits::_Sse_available()) {
            const __m128i _Comparand = _Traits::_Set_sse(_Val);
            const void* _Stop_at     = _First;
            _Advance_bytes(_Stop_at, _Sse_size);
            do {
                const __m128i _Data = _mm_loadu_si128(static_cast<const __m128i*>(_First));
                const int _Bingo    = _mm_movemask_epi8(_Traits::_Cmp_sse(_Data, _Comparand));

                if (_Bingo != 0) {
                    unsigned long _Offset;
                    _BitScanForward(&_Offset, _Bingo); // lgtm [cpp/conditionallyuninitializedvariable]
                    _Advance_bytes(_First, _Offset);
                    return _First;
                }

                _Advance_bytes(_First, 16);
            } while (_First != _Stop_at);
        }
#endif // !_M_ARM64EC
        auto _Ptr = static_cast<const _Ty*>(_First);
        while (_Ptr != _Last && *_Ptr != _Val) {
            ++_Ptr;
        }
        return _Ptr;
    }

    template <class _Traits, class _Ty>
    const void* __stdcall __std_find_last_trivial_impl(const void* _First, const void* _Last, _Ty _Val) noexcept {
        const void* const _Real_last = _Last;
#ifndef _M_ARM64EC
        size_t _Size_bytes = _Byte_length(_First, _Last);

        const size_t _Avx_size = _Size_bytes & ~size_t{0x1F};
        if (_Avx_size != 0 && _Use_avx2()) {
            _Zeroupper_on_exit _Guard; // TRANSITION, DevCom-10331414

            const __m256i _Comparand = _Traits::_Set_avx(_Val);
            const void* _Stop_at     = _Last;
            _Rewind_bytes(_Stop_at, _Avx_size);
            do {
                _Rewind_bytes(_Last, 32);
                const __m256i _Data = _mm256_loadu_si256(static_cast<const __m256i*>(_Last));
                const int _Bingo    = _mm256_movemask_epi8(_Traits::_Cmp_avx(_Data, _Comparand));

                if (_Bingo != 0) {
                    const unsigned long _Offset = _lzcnt_u32(_Bingo);
                    _Advance_bytes(_Last, (31 - _Offset) - (sizeof(_Ty) - 1));
                    return _Last;
                }
            } while (_Last != _Stop_at);
            _Size_bytes &= 0x1F;
        }

        const size_t _Sse_size = _Size_bytes & ~size_t{0xF};
        if (_Sse_size != 0 && _Traits::_Sse_available()) {
            const __m128i _Comparand = _Traits::_Set_sse(_Val);
            const void* _Stop_at     = _Last;
            _Rewind_bytes(_Stop_at, _Sse_size);
            do {
                _Rewind_bytes(_Last, 16);
                const __m128i _Data = _mm_loadu_si128(static_cast<const __m128i*>(_Last));
                const int _Bingo    = _mm_movemask_epi8(_Traits::_Cmp_sse(_Data, _Comparand));

                if (_Bingo != 0) {
                    unsigned long _Offset;
                    _BitScanReverse(&_Offset, _Bingo); // lgtm [cpp/conditionallyuninitializedvariable]
                    _Advance_bytes(_Last, _Offset - (sizeof(_Ty) - 1));
                    return _Last;
                }
            } while (_Last != _Stop_at);
        }
#endif // !_M_ARM64EC
        auto _Ptr = static_cast<const _Ty*>(_Last);
        for (;;) {
            if (_Ptr == _First) {
                return _Real_last;
            }
            --_Ptr;
            if (*_Ptr == _Val) {
                return _Ptr;
            }
        }
    }

    template <class _Traits, class _Ty>
    __declspec(noalias) size_t
        __stdcall __std_count_trivial_impl(const void* _First, const void* const _Last, const _Ty _Val) noexcept {
        size_t _Result = 0;

#ifndef _M_ARM64EC
        size_t _Size_bytes = _Byte_length(_First, _Last);

        const size_t _Avx_size = _Size_bytes & ~size_t{0x1F};
        if (_Avx_size != 0 && _Use_avx2()) {
            const __m256i _Comparand = _Traits::_Set_avx(_Val);
            const void* _Stop_at     = _First;
            _Advance_bytes(_Stop_at, _Avx_size);
            do {
                const __m256i _Data = _mm256_loadu_si256(static_cast<const __m256i*>(_First));
                const int _Bingo    = _mm256_movemask_epi8(_Traits::_Cmp_avx(_Data, _Comparand));
                _Result += __popcnt(_Bingo); // Assume available with SSE4.2
                _Advance_bytes(_First, 32);
            } while (_First != _Stop_at);
            _Size_bytes &= 0x1F;

            _mm256_zeroupper(); // TRANSITION, DevCom-10331414
        }

        const size_t _Sse_size = _Size_bytes & ~size_t{0xF};
        if (_Sse_size != 0 && _Use_sse42()) {
            const __m128i _Comparand = _Traits::_Set_sse(_Val);
            const void* _Stop_at     = _First;
            _Advance_bytes(_Stop_at, _Sse_size);
            do {
                const __m128i _Data = _mm_loadu_si128(static_cast<const __m128i*>(_First));
                const int _Bingo    = _mm_movemask_epi8(_Traits::_Cmp_sse(_Data, _Comparand));
                _Result += __popcnt(_Bingo); // Assume available with SSE4.2
                _Advance_bytes(_First, 16);
            } while (_First != _Stop_at);
        }
#endif // !_M_ARM64EC
        _Result >>= _Traits::_Shift;
        auto _Ptr = static_cast<const _Ty*>(_First);
        for (; _Ptr != _Last; ++_Ptr) {
            if (*_Ptr == _Val) {
                ++_Result;
            }
        }
        return _Result;
    }
} // unnamed namespace

extern "C" {

const void* __stdcall __std_find_trivial_unsized_1(const void* const _First, const uint8_t _Val) noexcept {
    return __std_find_trivial_unsized_impl<_Find_traits_1>(_First, _Val);
}

const void* __stdcall __std_find_trivial_unsized_2(const void* const _First, const uint16_t _Val) noexcept {
    return __std_find_trivial_unsized_impl<_Find_traits_2>(_First, _Val);
}

const void* __stdcall __std_find_trivial_unsized_4(const void* const _First, const uint32_t _Val) noexcept {
    return __std_find_trivial_unsized_impl<_Find_traits_4>(_First, _Val);
}

const void* __stdcall __std_find_trivial_unsized_8(const void* const _First, const uint64_t _Val) noexcept {
    return __std_find_trivial_unsized_impl<_Find_traits_8>(_First, _Val);
}

const void* __stdcall __std_find_trivial_1(
    const void* const _First, const void* const _Last, const uint8_t _Val) noexcept {
    return __std_find_trivial_impl<_Find_traits_1>(_First, _Last, _Val);
}

const void* __stdcall __std_find_trivial_2(
    const void* const _First, const void* const _Last, const uint16_t _Val) noexcept {
    return __std_find_trivial_impl<_Find_traits_2>(_First, _Last, _Val);
}

const void* __stdcall __std_find_trivial_4(
    const void* const _First, const void* const _Last, const uint32_t _Val) noexcept {
    return __std_find_trivial_impl<_Find_traits_4>(_First, _Last, _Val);
}

const void* __stdcall __std_find_trivial_8(
    const void* const _First, const void* const _Last, const uint64_t _Val) noexcept {
    return __std_find_trivial_impl<_Find_traits_8>(_First, _Last, _Val);
}

const void* __stdcall __std_find_last_trivial_1(
    const void* const _First, const void* const _Last, const uint8_t _Val) noexcept {
    return __std_find_last_trivial_impl<_Find_traits_1>(_First, _Last, _Val);
}

const void* __stdcall __std_find_last_trivial_2(
    const void* const _First, const void* const _Last, const uint16_t _Val) noexcept {
    return __std_find_last_trivial_impl<_Find_traits_2>(_First, _Last, _Val);
}

const void* __stdcall __std_find_last_trivial_4(
    const void* const _First, const void* const _Last, const uint32_t _Val) noexcept {
    return __std_find_last_trivial_impl<_Find_traits_4>(_First, _Last, _Val);
}

const void* __stdcall __std_find_last_trivial_8(
    const void* const _First, const void* const _Last, const uint64_t _Val) noexcept {
    return __std_find_last_trivial_impl<_Find_traits_8>(_First, _Last, _Val);
}

__declspec(noalias) size_t
    __stdcall __std_count_trivial_1(const void* const _First, const void* const _Last, const uint8_t _Val) noexcept {
    return __std_count_trivial_impl<_Find_traits_1>(_First, _Last, _Val);
}

__declspec(noalias) size_t
    __stdcall __std_count_trivial_2(const void* const _First, const void* const _Last, const uint16_t _Val) noexcept {
    return __std_count_trivial_impl<_Find_traits_2>(_First, _Last, _Val);
}

__declspec(noalias) size_t
    __stdcall __std_count_trivial_4(const void* const _First, const void* const _Last, const uint32_t _Val) noexcept {
    return __std_count_trivial_impl<_Find_traits_4>(_First, _Last, _Val);
}

__declspec(noalias) size_t
    __stdcall __std_count_trivial_8(const void* const _First, const void* const _Last, const uint64_t _Val) noexcept {
    return __std_count_trivial_impl<_Find_traits_8>(_First, _Last, _Val);
}

} // extern "C"

#ifndef _M_ARM64EC
namespace {
    __m256i __forceinline _Bitset_to_string_1_step_avx(const uint32_t _Val, const __m256i _Px0, const __m256i _Px1) {
        const __m128i _Vx0 = _mm_cvtsi32_si128(_Val);
        const __m128i _Vx1 = _mm_shuffle_epi8(_Vx0, _mm_set_epi32(0x00000000, 0x01010101, 0x02020202, 0x03030303));
        const __m256i _Vx2 = _mm256_castsi128_si256(_Vx1);
        const __m256i _Vx3 = _mm256_permutevar8x32_epi32(_Vx2, _mm256_set_epi32(3, 3, 2, 2, 1, 1, 0, 0));
        const __m256i _Msk = _mm256_and_si256(_Vx3, _mm256_set1_epi64x(0x0102040810204080));
        const __m256i _Ex0 = _mm256_cmpeq_epi8(_Msk, _mm256_setzero_si256());
        const __m256i _Ex1 = _mm256_blendv_epi8(_Px1, _Px0, _Ex0);
        return _Ex1;
    }

    __m128i __forceinline _Bitset_to_string_1_step(const uint16_t _Val, const __m128i _Px0, const __m128i _Px1) {
        const __m128i _Vx0 = _mm_cvtsi32_si128(_Val);
        const __m128i _Vx1 = _mm_unpacklo_epi8(_Vx0, _Vx0);
        const __m128i _Vx2 = _mm_unpacklo_epi8(_Vx1, _Vx1);
        const __m128i _Vx3 = _mm_shuffle_epi32(_Vx2, _MM_SHUFFLE(0, 0, 1, 1));
        const __m128i _Msk = _mm_and_si128(_Vx3, _mm_set1_epi64x(0x0102040810204080));
        const __m128i _Ex0 = _mm_cmpeq_epi8(_Msk, _mm_setzero_si128());
        const __m128i _Ex1 = _mm_xor_si128(_mm_and_si128(_Ex0, _Px0), _Px1);
        return _Ex1;
    }

    __m256i __forceinline _Bitset_to_string_2_step_avx(const uint16_t _Val, const __m256i _Px0, const __m256i _Px1) {
        const __m128i _Vx0 = _mm_cvtsi32_si128(_Val);
        const __m128i _Vx1 = _mm_shuffle_epi8(_Vx0, _mm_set_epi32(0x00000000, 0x00000000, 0x01010101, 0x01010101));
        const __m256i _Vx2 = _mm256_castsi128_si256(_Vx1);
        const __m256i _Vx3 = _mm256_permute4x64_epi64(_Vx2, _MM_SHUFFLE(1, 1, 0, 0));
        const __m256i _Msk = _mm256_and_si256(
            _Vx3, _mm256_set_epi64x(0x0001000200040008, 0x0010002000400080, 0x0001000200040008, 0x0010002000400080));
        const __m256i _Ex0 = _mm256_cmpeq_epi16(_Msk, _mm256_setzero_si256());
        const __m256i _Ex1 = _mm256_blendv_epi8(_Px1, _Px0, _Ex0);
        return _Ex1;
    }

    __m128i __forceinline _Bitset_to_string_2_step(const uint8_t _Val, const __m128i _Px0, const __m128i _Px1) {
        const __m128i _Vx  = _mm_set1_epi16(_Val);
        const __m128i _Msk = _mm_and_si128(_Vx, _mm_set_epi64x(0x0001000200040008, 0x0010002000400080));
        const __m128i _Ex0 = _mm_cmpeq_epi16(_Msk, _mm_setzero_si128());
        const __m128i _Ex1 = _mm_xor_si128(_mm_and_si128(_Ex0, _Px0), _Px1);
        return _Ex1;
    }
} // unnamed namespace
#endif // !defined(_M_ARM64EC)

extern "C" {

__declspec(noalias) void __stdcall __std_bitset_to_string_1(
    char* const _Dest, const void* _Src, size_t _Size_bits, const char _Elem0, const char _Elem1) noexcept {
#ifndef _M_ARM64EC
    if (_Use_avx2() && _Size_bits >= 256) {
        const __m256i _Px0 = _mm256_broadcastb_epi8(_mm_cvtsi32_si128(_Elem0));
        const __m256i _Px1 = _mm256_broadcastb_epi8(_mm_cvtsi32_si128(_Elem1));
        if (_Size_bits >= 32) {
            char* _Pos = _Dest + _Size_bits;
            _Size_bits &= 0x1F;
            char* const _Stop_at = _Dest + _Size_bits;
            do {
                uint32_t _Val;
                memcpy(&_Val, _Src, 4);
                const __m256i _Elems = _Bitset_to_string_1_step_avx(_Val, _Px0, _Px1);
                _Pos -= 32;
                _mm256_storeu_si256(reinterpret_cast<__m256i*>(_Pos), _Elems);
                _Advance_bytes(_Src, 4);
            } while (_Pos != _Stop_at);
        }

        if (_Size_bits > 0) {
            __assume(_Size_bits < 32);
            uint32_t _Val = 0;
            memcpy(&_Val, _Src, (_Size_bits + 7) / 8);
            const __m256i _Elems = _Bitset_to_string_1_step_avx(_Val, _Px0, _Px1);
            char _Tmp[32];
            _mm256_storeu_si256(reinterpret_cast<__m256i*>(_Tmp), _Elems);
            const char* const _Tmpd = _Tmp + (32 - _Size_bits);
            memcpy(_Dest, _Tmpd, _Size_bits);
        }

        _mm256_zeroupper(); // TRANSITION, DevCom-10331414
        return;
    }

    if (_Use_sse2()) {
        const __m128i _Px0 = _mm_set1_epi8(_Elem0 ^ _Elem1);
        const __m128i _Px1 = _mm_set1_epi8(_Elem1);
        if (_Size_bits >= 16) {
            char* _Pos = _Dest + _Size_bits;
            _Size_bits &= 0xF;
            char* const _Stop_at = _Dest + _Size_bits;
            do {
                uint16_t _Val;
                memcpy(&_Val, _Src, 2);
                const __m128i _Elems = _Bitset_to_string_1_step(_Val, _Px0, _Px1);
                _Pos -= 16;
                _mm_storeu_si128(reinterpret_cast<__m128i*>(_Pos), _Elems);
                _Advance_bytes(_Src, 2);
            } while (_Pos != _Stop_at);
        }

        if (_Size_bits > 0) {
            __assume(_Size_bits < 16);
            uint16_t _Val;
            if (_Size_bits > 8) {
                memcpy(&_Val, _Src, 2);
            } else {
                _Val = *reinterpret_cast<const uint8_t*>(_Src);
            }
            const __m128i _Elems = _Bitset_to_string_1_step(_Val, _Px0, _Px1);
            char _Tmp[16];
            _mm_storeu_si128(reinterpret_cast<__m128i*>(_Tmp), _Elems);
            const char* const _Tmpd = _Tmp + (16 - _Size_bits);
            for (size_t _Ix = 0; _Ix < _Size_bits; ++_Ix) {
                _Dest[_Ix] = _Tmpd[_Ix];
            }
        }

        return;
    }
#endif // !defined(_M_ARM64EC)
    const auto _Arr = reinterpret_cast<const uint8_t*>(_Src);
    for (size_t _Ix = 0; _Ix < _Size_bits; ++_Ix) {
        _Dest[_Size_bits - 1 - _Ix] = ((_Arr[_Ix >> 3] >> (_Ix & 7)) & 1) != 0 ? _Elem1 : _Elem0;
    }
}

__declspec(noalias) void __stdcall __std_bitset_to_string_2(
    wchar_t* const _Dest, const void* _Src, size_t _Size_bits, const wchar_t _Elem0, const wchar_t _Elem1) noexcept {
#ifndef _M_ARM64EC
    if (_Use_avx2() && _Size_bits >= 256) {
        const __m256i _Px0 = _mm256_broadcastw_epi16(_mm_cvtsi32_si128(_Elem0));
        const __m256i _Px1 = _mm256_broadcastw_epi16(_mm_cvtsi32_si128(_Elem1));

        if (_Size_bits >= 16) {
            wchar_t* _Pos = _Dest + _Size_bits;
            _Size_bits &= 0xF;
            wchar_t* const _Stop_at = _Dest + _Size_bits;
            do {
                uint16_t _Val;
                memcpy(&_Val, _Src, 2);
                const __m256i _Elems = _Bitset_to_string_2_step_avx(_Val, _Px0, _Px1);
                _Pos -= 16;
                _mm256_storeu_si256(reinterpret_cast<__m256i*>(_Pos), _Elems);
                _Advance_bytes(_Src, 2);
            } while (_Pos != _Stop_at);
        }

        if (_Size_bits > 0) {
            __assume(_Size_bits < 16);
            uint16_t _Val;
            if (_Size_bits > 8) {
                memcpy(&_Val, _Src, 2);
            } else {
                _Val = *reinterpret_cast<const uint8_t*>(_Src);
            }
            const __m256i _Elems = _Bitset_to_string_2_step_avx(_Val, _Px0, _Px1);
            wchar_t _Tmp[16];
            _mm256_storeu_si256(reinterpret_cast<__m256i*>(_Tmp), _Elems);
            const wchar_t* const _Tmpd = _Tmp + (16 - _Size_bits);
            memcpy(_Dest, _Tmpd, _Size_bits * 2);
        }

        _mm256_zeroupper(); // TRANSITION, DevCom-10331414
        return;
    }

    if (_Use_sse2()) {
        const __m128i _Px0 = _mm_set1_epi16(_Elem0 ^ _Elem1);
        const __m128i _Px1 = _mm_set1_epi16(_Elem1);
        if (_Size_bits >= 8) {
            wchar_t* _Pos = _Dest + _Size_bits;
            _Size_bits &= 0x7;
            wchar_t* const _Stop_at = _Dest + _Size_bits;
            do {
                const uint8_t _Val   = *reinterpret_cast<const uint8_t*>(_Src);
                const __m128i _Elems = _Bitset_to_string_2_step(_Val, _Px0, _Px1);
                _Pos -= 8;
                _mm_storeu_si128(reinterpret_cast<__m128i*>(_Pos), _Elems);
                _Advance_bytes(_Src, 1);
            } while (_Pos != _Stop_at);
        }

        if (_Size_bits > 0) {
            __assume(_Size_bits < 8);
            const uint8_t _Val   = *reinterpret_cast<const uint8_t*>(_Src);
            const __m128i _Elems = _Bitset_to_string_2_step(_Val, _Px0, _Px1);
            wchar_t _Tmp[8];
            _mm_storeu_si128(reinterpret_cast<__m128i*>(_Tmp), _Elems);
            const wchar_t* const _Tmpd = _Tmp + (8 - _Size_bits);
            for (size_t _Ix = 0; _Ix < _Size_bits; ++_Ix) {
                _Dest[_Ix] = _Tmpd[_Ix];
            }
        }

        return;
    }
#endif // !defined(_M_ARM64EC)
    const auto _Arr = reinterpret_cast<const uint8_t*>(_Src);
    for (size_t _Ix = 0; _Ix < _Size_bits; ++_Ix) {
        _Dest[_Size_bits - 1 - _Ix] = ((_Arr[_Ix >> 3] >> (_Ix & 7)) & 1) != 0 ? _Elem1 : _Elem0;
    }
}

} // extern "C"
#endif // defined(_M_IX86) || defined(_M_X64)
