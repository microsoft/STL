// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

// This must be as small as possible, because its contents are
// injected into the msvcprt.lib and msvcprtd.lib import libraries.
// Do not include or define anything else here.
// In particular, basic_string must not be included here.

#ifdef _M_CEE_PURE
#error _M_CEE_PURE should not be defined when compiling vector_algorithms.cpp.
#endif

#if (defined(_M_IX86) || defined(_M_X64)) && !defined(_M_ARM64EC)

#include <emmintrin.h>
#include <immintrin.h>
#include <intrin0.h>
#include <isa_availability.h>
#include <stdint.h>

extern "C" long __isa_enabled;

#ifndef _DEBUG
#pragma optimize("t", on) // Override /Os with /Ot for this TU
#endif // !_DEBUG

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
#endif // _M_IX86
    }

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

    void _Advance_bytes(void*& _Target, ptrdiff_t _Offset) noexcept {
        _Target = static_cast<unsigned char*>(_Target) + _Offset;
    }

    void _Advance_bytes(const void*& _Target, ptrdiff_t _Offset) noexcept {
        _Target = static_cast<const unsigned char*>(_Target) + _Offset;
    }
} // unnamed namespace

extern "C" {
__declspec(noalias) void __cdecl __std_swap_ranges_trivially_swappable_noalias(
    void* _First1, void* _Last1, void* _First2) noexcept {
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
            const unsigned long _Left = *static_cast<unsigned long*>(_First1);
            const unsigned long _Right = *static_cast<unsigned long*>(_First2);
            *static_cast<unsigned long*>(_First1) = _Right;
            *static_cast<unsigned long*>(_First2) = _Left;
            _Advance_bytes(_First1, 4);
            _Advance_bytes(_First2, 4);
        } while (_First1 != _Stop_at);
    }
#else
#error Unsupported architecture
#endif

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
    if (_Byte_length(_First, _Last) >= 64 && _Use_avx2()) {
        const __m256i _Reverse_char_lanes_avx = _mm256_set_epi8( //
            0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, //
            0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15);
        const void* _Stop_at                  = _First;
        _Advance_bytes(_Stop_at, _Byte_length(_First, _Last) >> 6 << 5);
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
    }

    if (_Byte_length(_First, _Last) >= 32 && _Use_sse42()) {
        const __m128i _Reverse_char_sse = _mm_set_epi8(0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15);
        const void* _Stop_at            = _First;
        _Advance_bytes(_Stop_at, _Byte_length(_First, _Last) >> 5 << 4);
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

    _Reverse_tail(static_cast<unsigned char*>(_First), static_cast<unsigned char*>(_Last));
}

__declspec(noalias) void __cdecl __std_reverse_trivially_swappable_2(void* _First, void* _Last) noexcept {
    if (_Byte_length(_First, _Last) >= 64 && _Use_avx2()) {
        const __m256i _Reverse_short_lanes_avx = _mm256_set_epi8( //
            1, 0, 3, 2, 5, 4, 7, 6, 9, 8, 11, 10, 13, 12, 15, 14, //
            1, 0, 3, 2, 5, 4, 7, 6, 9, 8, 11, 10, 13, 12, 15, 14);
        const void* _Stop_at                   = _First;
        _Advance_bytes(_Stop_at, _Byte_length(_First, _Last) >> 6 << 5);
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
    }

    if (_Byte_length(_First, _Last) >= 32 && _Use_sse42()) {
        const __m128i _Reverse_short_sse = _mm_set_epi8(1, 0, 3, 2, 5, 4, 7, 6, 9, 8, 11, 10, 13, 12, 15, 14);
        const void* _Stop_at             = _First;
        _Advance_bytes(_Stop_at, _Byte_length(_First, _Last) >> 5 << 4);
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

    _Reverse_tail(static_cast<unsigned short*>(_First), static_cast<unsigned short*>(_Last));
}

__declspec(noalias) void __cdecl __std_reverse_trivially_swappable_4(void* _First, void* _Last) noexcept {
    if (_Byte_length(_First, _Last) >= 64 && _Use_avx2()) {
        const void* _Stop_at = _First;
        _Advance_bytes(_Stop_at, _Byte_length(_First, _Last) >> 6 << 5);
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
    }

    if (_Byte_length(_First, _Last) >= 32 && _Use_sse2()) {
        const void* _Stop_at = _First;
        _Advance_bytes(_Stop_at, _Byte_length(_First, _Last) >> 5 << 4);
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

    _Reverse_tail(static_cast<unsigned long*>(_First), static_cast<unsigned long*>(_Last));
}

__declspec(noalias) void __cdecl __std_reverse_trivially_swappable_8(void* _First, void* _Last) noexcept {
    if (_Byte_length(_First, _Last) >= 64 && _Use_avx2()) {
        const void* _Stop_at = _First;
        _Advance_bytes(_Stop_at, _Byte_length(_First, _Last) >> 6 << 5);
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
    }

    if (_Byte_length(_First, _Last) >= 32 && _Use_sse2()) {
        const void* _Stop_at = _First;
        _Advance_bytes(_Stop_at, _Byte_length(_First, _Last) >> 5 << 4);
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

    _Reverse_tail(static_cast<unsigned long long*>(_First), static_cast<unsigned long long*>(_Last));
}

__declspec(noalias) void __cdecl __std_reverse_copy_trivially_copyable_1(
    const void* _First, const void* _Last, void* _Dest) noexcept {
    if (_Byte_length(_First, _Last) >= 32 && _Use_avx2()) {
        const __m256i _Reverse_char_lanes_avx = _mm256_set_epi8( //
            0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, //
            0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15);
        const void* _Stop_at                  = _Dest;
        _Advance_bytes(_Stop_at, _Byte_length(_First, _Last) >> 5 << 5);
        do {
            _Advance_bytes(_Last, -32);
            const __m256i _Block          = _mm256_loadu_si256(static_cast<const __m256i*>(_Last));
            const __m256i _Block_permuted = _mm256_permute4x64_epi64(_Block, _MM_SHUFFLE(1, 0, 3, 2));
            const __m256i _Block_reversed = _mm256_shuffle_epi8(_Block_permuted, _Reverse_char_lanes_avx);
            _mm256_storeu_si256(static_cast<__m256i*>(_Dest), _Block_reversed);
            _Advance_bytes(_Dest, 32);
        } while (_Dest != _Stop_at);
    }

    if (_Byte_length(_First, _Last) >= 16 && _Use_sse42()) {
        const __m128i _Reverse_char_sse = _mm_set_epi8(0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15);
        const void* _Stop_at            = _Dest;
        _Advance_bytes(_Stop_at, _Byte_length(_First, _Last) >> 4 << 4);
        do {
            _Advance_bytes(_Last, -16);
            const __m128i _Block          = _mm_loadu_si128(static_cast<const __m128i*>(_Last));
            const __m128i _Block_reversed = _mm_shuffle_epi8(_Block, _Reverse_char_sse); // SSSE3
            _mm_storeu_si128(static_cast<__m128i*>(_Dest), _Block_reversed);
            _Advance_bytes(_Dest, 16);
        } while (_Dest != _Stop_at);
    }

    _Reverse_copy_tail(static_cast<const unsigned char*>(_First), static_cast<const unsigned char*>(_Last),
        static_cast<unsigned char*>(_Dest));
}

__declspec(noalias) void __cdecl __std_reverse_copy_trivially_copyable_2(
    const void* _First, const void* _Last, void* _Dest) noexcept {
    if (_Byte_length(_First, _Last) >= 32 && _Use_avx2()) {
        const __m256i _Reverse_short_lanes_avx = _mm256_set_epi8( //
            1, 0, 3, 2, 5, 4, 7, 6, 9, 8, 11, 10, 13, 12, 15, 14, //
            1, 0, 3, 2, 5, 4, 7, 6, 9, 8, 11, 10, 13, 12, 15, 14);
        const void* _Stop_at                   = _Dest;
        _Advance_bytes(_Stop_at, _Byte_length(_First, _Last) >> 5 << 5);
        do {
            _Advance_bytes(_Last, -32);
            const __m256i _Block          = _mm256_loadu_si256(static_cast<const __m256i*>(_Last));
            const __m256i _Block_permuted = _mm256_permute4x64_epi64(_Block, _MM_SHUFFLE(1, 0, 3, 2));
            const __m256i _Block_reversed = _mm256_shuffle_epi8(_Block_permuted, _Reverse_short_lanes_avx);
            _mm256_storeu_si256(static_cast<__m256i*>(_Dest), _Block_reversed);
            _Advance_bytes(_Dest, 32);
        } while (_Dest != _Stop_at);
    }

    if (_Byte_length(_First, _Last) >= 16 && _Use_sse42()) {
        const __m128i _Reverse_short_sse = _mm_set_epi8(1, 0, 3, 2, 5, 4, 7, 6, 9, 8, 11, 10, 13, 12, 15, 14);
        const void* _Stop_at             = _Dest;
        _Advance_bytes(_Stop_at, _Byte_length(_First, _Last) >> 4 << 4);
        do {
            _Advance_bytes(_Last, -16);
            const __m128i _Block          = _mm_loadu_si128(static_cast<const __m128i*>(_Last));
            const __m128i _Block_reversed = _mm_shuffle_epi8(_Block, _Reverse_short_sse); // SSSE3
            _mm_storeu_si128(static_cast<__m128i*>(_Dest), _Block_reversed);
            _Advance_bytes(_Dest, 16);
        } while (_Dest != _Stop_at);
    }

    _Reverse_copy_tail(static_cast<const unsigned short*>(_First), static_cast<const unsigned short*>(_Last),
        static_cast<unsigned short*>(_Dest));
}

__declspec(noalias) void __cdecl __std_reverse_copy_trivially_copyable_4(
    const void* _First, const void* _Last, void* _Dest) noexcept {
    if (_Byte_length(_First, _Last) >= 32 && _Use_avx2()) {
        const void* _Stop_at = _Dest;
        _Advance_bytes(_Stop_at, _Byte_length(_First, _Last) >> 5 << 5);
        const __m256i _Shuf = _mm256_set_epi32(0, 1, 2, 3, 4, 5, 6, 7);
        do {
            _Advance_bytes(_Last, -32);
            const __m256i _Block          = _mm256_loadu_si256(static_cast<const __m256i*>(_Last));
            const __m256i _Block_reversed = _mm256_permutevar8x32_epi32(_Block, _Shuf);
            _mm256_storeu_si256(static_cast<__m256i*>(_Dest), _Block_reversed);
            _Advance_bytes(_Dest, 32);
        } while (_Dest != _Stop_at);
    }

    if (_Byte_length(_First, _Last) >= 16 && _Use_sse2()) {
        const void* _Stop_at = _Dest;
        _Advance_bytes(_Stop_at, _Byte_length(_First, _Last) >> 4 << 4);
        do {
            _Advance_bytes(_Last, -16);
            const __m128i _Block          = _mm_loadu_si128(static_cast<const __m128i*>(_Last));
            const __m128i _Block_reversed = _mm_shuffle_epi32(_Block, _MM_SHUFFLE(0, 1, 2, 3));
            _mm_storeu_si128(static_cast<__m128i*>(_Dest), _Block_reversed);
            _Advance_bytes(_Dest, 16);
        } while (_Dest != _Stop_at);
    }

    _Reverse_copy_tail(static_cast<const unsigned long*>(_First), static_cast<const unsigned long*>(_Last),
        static_cast<unsigned long*>(_Dest));
}

__declspec(noalias) void __cdecl __std_reverse_copy_trivially_copyable_8(
    const void* _First, const void* _Last, void* _Dest) noexcept {
    if (_Byte_length(_First, _Last) >= 32 && _Use_avx2()) {
        const void* _Stop_at = _Dest;
        _Advance_bytes(_Stop_at, _Byte_length(_First, _Last) >> 5 << 5);
        do {
            _Advance_bytes(_Last, -32);
            const __m256i _Block          = _mm256_loadu_si256(static_cast<const __m256i*>(_Last));
            const __m256i _Block_reversed = _mm256_permute4x64_epi64(_Block, _MM_SHUFFLE(0, 1, 2, 3));
            _mm256_storeu_si256(static_cast<__m256i*>(_Dest), _Block_reversed);
            _Advance_bytes(_Dest, 32);
        } while (_Dest != _Stop_at);
    }

    if (_Byte_length(_First, _Last) >= 16 && _Use_sse2()) {
        const void* _Stop_at = _Dest;
        _Advance_bytes(_Stop_at, _Byte_length(_First, _Last) >> 4 << 4);
        do {
            _Advance_bytes(_Last, -16);
            const __m128i _Block          = _mm_loadu_si128(static_cast<const __m128i*>(_Last));
            const __m128i _Block_reversed = _mm_shuffle_epi32(_Block, _MM_SHUFFLE(1, 0, 3, 2));
            _mm_storeu_si128(static_cast<__m128i*>(_Dest), _Block_reversed);
            _Advance_bytes(_Dest, 16);
        } while (_Dest != _Stop_at);
    }

    _Reverse_copy_tail(static_cast<const unsigned long long*>(_First), static_cast<const unsigned long long*>(_Last),
        static_cast<unsigned long long*>(_Dest));
}

} // extern "C"

namespace {
    template <class _Ty>
    const void* _Find_trivial_unsized_fallback(const void* _First, _Ty _Val) {
        auto _Ptr = static_cast<const _Ty*>(_First);
        while (*_Ptr != _Val) {
            ++_Ptr;
        }
        return _Ptr;
    }

    template <class _Ty>
    const void* _Find_trivial_tail(const void* _First, const void* _Last, _Ty _Val) {
        auto _Ptr = static_cast<const _Ty*>(_First);
        while (_Ptr != _Last && *_Ptr != _Val) {
            ++_Ptr;
        }
        return _Ptr;
    }

    template <class _Ty>
    __declspec(noalias) size_t _Count_trivial_tail(const void* _First, const void* _Last, size_t _Current, _Ty _Val) {
        auto _Ptr = static_cast<const _Ty*>(_First);
        for (; _Ptr != _Last; ++_Ptr) {
            if (*_Ptr == _Val) {
                ++_Current;
            }
        }
        return _Current;
    }

    struct _Find_traits_1 {
        static constexpr size_t _Shift = 0;

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
    };

    struct _Find_traits_2 {
        static constexpr size_t _Shift = 1;

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
    };

    struct _Find_traits_4 {
        static constexpr size_t _Shift = 2;

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
    };

    struct _Find_traits_8 {
        static constexpr size_t _Shift = 3;

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
    };

    // The below functions have exactly the same signature as the extern "C" functions, up to calling convention.
    // This makes sure the template specialization is fused with the extern "C" function.
    // In optimized builds it avoids an extra call, as this function is too large to inline.

    template <class _Traits, class _Ty>
    const void* __stdcall __std_find_trivial_unsized(const void* _First, const _Ty _Val) noexcept {
        if (_Use_avx2()) {
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

            if ((_Bingo &= _Mask) != 0) {
                unsigned long _Offset = _tzcnt_u32(_Bingo);
                _Advance_bytes(_First, _Offset);
                return _First;
            }

            for (;;) {
                _Data  = _mm256_load_si256(static_cast<const __m256i*>(_First));
                _Bingo = static_cast<unsigned int>(_mm256_movemask_epi8(_Traits::_Cmp_avx(_Data, _Comparand)));

                if (_Bingo != 0) {
                    unsigned long _Offset = _tzcnt_u32(_Bingo);
                    _Advance_bytes(_First, _Offset);
                    return _First;
                }

                _Advance_bytes(_First, 32);
            }
        }

        if (_Traits::_Sse_available()) {
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

            if ((_Bingo &= _Mask) != 0) {
                unsigned long _Offset;
                _BitScanForward(&_Offset, _Bingo);
                _Advance_bytes(_First, _Offset);
                return _First;
            }

            for (;;) {
                _Data  = _mm_load_si128(static_cast<const __m128i*>(_First));
                _Bingo = static_cast<unsigned int>(_mm_movemask_epi8(_Traits::_Cmp_sse(_Data, _Comparand)));

                if (_Bingo != 0) {
                    unsigned long _Offset;
                    _BitScanForward(&_Offset, _Bingo);
                    _Advance_bytes(_First, _Offset);
                    return _First;
                }

                _Advance_bytes(_First, 16);
            }
        }

        return _Find_trivial_unsized_fallback(_First, _Val);
    }


    template <class _Traits, class _Ty>
    const void* __stdcall __std_find_trivial(const void* _First, const void* _Last, _Ty _Val) noexcept {
        size_t _Size_bytes = _Byte_length(_First, _Last);

        const size_t _Avx_size = _Size_bytes & ~size_t{0x1F};
        if (_Avx_size != 0 && _Use_avx2()) {
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
                    _BitScanForward(&_Offset, _Bingo);
                    _Advance_bytes(_First, _Offset);
                    return _First;
                }

                _Advance_bytes(_First, 16);
            } while (_First != _Stop_at);
        }

        return _Find_trivial_tail(_First, _Last, _Val);
    }

    template <class _Traits, class _Ty>
    __declspec(noalias) size_t
        __stdcall __std_count_trivial(const void* _First, const void* const _Last, const _Ty _Val) noexcept {
        size_t _Size_bytes = _Byte_length(_First, _Last);
        size_t _Result     = 0;

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

        return _Count_trivial_tail(_First, _Last, _Result >> _Traits::_Shift, _Val);
    }
} // unnamed namespace

extern "C" {

const void* __stdcall __std_find_trivial_unsized_1(const void* const _First, const uint8_t _Val) noexcept {
    return __std_find_trivial_unsized<_Find_traits_1>(_First, _Val);
}

const void* __stdcall __std_find_trivial_unsized_2(const void* const _First, const uint16_t _Val) noexcept {
    return __std_find_trivial_unsized<_Find_traits_2>(_First, _Val);
}

const void* __stdcall __std_find_trivial_unsized_4(const void* const _First, const uint32_t _Val) noexcept {
    return __std_find_trivial_unsized<_Find_traits_4>(_First, _Val);
}

const void* __stdcall __std_find_trivial_unsized_8(const void* const _First, const uint64_t _Val) noexcept {
    return __std_find_trivial_unsized<_Find_traits_8>(_First, _Val);
}

const void* __stdcall __std_find_trivial_1(
    const void* const _First, const void* const _Last, const uint8_t _Val) noexcept {
    return __std_find_trivial<_Find_traits_1>(_First, _Last, _Val);
}

const void* __stdcall __std_find_trivial_2(
    const void* const _First, const void* const _Last, const uint16_t _Val) noexcept {
    return __std_find_trivial<_Find_traits_2>(_First, _Last, _Val);
}

const void* __stdcall __std_find_trivial_4(
    const void* const _First, const void* const _Last, const uint32_t _Val) noexcept {
    return __std_find_trivial<_Find_traits_4>(_First, _Last, _Val);
}

const void* __stdcall __std_find_trivial_8(
    const void* const _First, const void* const _Last, const uint64_t _Val) noexcept {
    return __std_find_trivial<_Find_traits_8>(_First, _Last, _Val);
}

__declspec(noalias) size_t
    __stdcall __std_count_trivial_1(const void* const _First, const void* const _Last, const uint8_t _Val) noexcept {
    return __std_count_trivial<_Find_traits_1>(_First, _Last, _Val);
}

__declspec(noalias) size_t
    __stdcall __std_count_trivial_2(const void* const _First, const void* const _Last, const uint16_t _Val) noexcept {
    return __std_count_trivial<_Find_traits_2>(_First, _Last, _Val);
}

__declspec(noalias) size_t
    __stdcall __std_count_trivial_4(const void* const _First, const void* const _Last, const uint32_t _Val) noexcept {
    return __std_count_trivial<_Find_traits_4>(_First, _Last, _Val);
}

__declspec(noalias) size_t
    __stdcall __std_count_trivial_8(const void* const _First, const void* const _Last, const uint64_t _Val) noexcept {
    return __std_count_trivial<_Find_traits_8>(_First, _Last, _Val);
}

} // extern "C"

#endif // (defined(_M_IX86) || defined(_M_X64)) && !defined(_M_ARM64EC)
