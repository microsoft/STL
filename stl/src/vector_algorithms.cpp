// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

// This must be as small as possible, because its contents are
// injected into the msvcprt.lib and msvcprtd.lib import libraries.
// Do not include or define anything else here.
// In particular, basic_string must not be included here.

#ifdef _M_CEE_PURE
#error _M_CEE_PURE should not be defined when compiling vector_algorithms.cpp.
#endif

#if defined(_M_IX86) || defined(_M_X64)

#if defined(_M_ARM64EC)
#include <intrin.h>
#else // defined(_M_ARM64EC)
#include <emmintrin.h>
#include <immintrin.h>
#include <intrin0.h>
#endif // defined(_M_ARM64EC)
#include <isa_availability.h>
#include <stdint.h>
#include <string.h>
#include <wchar.h>

extern "C" long __isa_enabled;

#ifdef _M_X64
#define VECTORCALL __vectorcall
#else
#define VECTORCALL
#endif

#pragma optimize("t", on) // Override /Os with /Ot for this TU

static bool _Use_avx2() {
    return __isa_enabled & (1 << __ISA_AVAILABLE_AVX2);
}

static bool _Use_sse42() {
    return __isa_enabled & (1 << __ISA_AVAILABLE_SSE42);
}

static bool _Use_sse2() {
#ifdef _M_IX86
    return __isa_enabled & (1 << __ISA_AVAILABLE_SSE2);
#else
    return true;
#endif // _M_IX86
}

template <class _BidIt>
static void _Reverse_tail(_BidIt _First, _BidIt _Last) noexcept {
    for (; _First != _Last && _First != --_Last; ++_First) {
        const auto _Temp = *_First;
        *_First          = *_Last;
        *_Last           = _Temp;
    }
}

template <class _BidIt, class _OutIt>
static void _Reverse_copy_tail(_BidIt _First, _BidIt _Last, _OutIt _Dest) noexcept {
    while (_First != _Last) {
        *_Dest++ = *--_Last;
    }
}

static size_t _Byte_length(const void* _First, const void* _Last) noexcept {
    return static_cast<const unsigned char*>(_Last) - static_cast<const unsigned char*>(_First);
}

static void _Advance_bytes(void*& _Target, ptrdiff_t _Offset) noexcept {
    _Target = static_cast<unsigned char*>(_Target) + _Offset;
}

static void _Advance_bytes(const void*& _Target, ptrdiff_t _Offset) noexcept {
    _Target = static_cast<const unsigned char*>(_Target) + _Offset;
}

extern "C" {
__declspec(noalias) void __cdecl __std_swap_ranges_trivially_swappable_noalias(
    void* _First1, void* _Last1, void* _First2) noexcept {
#if !defined(_M_ARM64EC)
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
#endif // !defined(_M_ARM64EC)

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
#if !defined(_M_ARM64EC)
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
#endif // !defined(_M_ARM64EC)

    if (_Byte_length(_First, _Last) >= 32 && _Use_sse2()) {
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
#if !defined(_M_ARM64EC)
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
#endif // !defined(_M_ARM64EC)

    if (_Byte_length(_First, _Last) >= 32 && _Use_sse2()) {
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
#if !defined(_M_ARM64EC)
    if (_Byte_length(_First, _Last) >= 64 && _Use_avx2()) {
        const void* _Stop_at = _First;
        _Advance_bytes(_Stop_at, _Byte_length(_First, _Last) >> 6 << 5);
        do {
            _Advance_bytes(_Last, -32);
            const __m256i _Left           = _mm256_loadu_si256(static_cast<__m256i*>(_First));
            const __m256i _Right          = _mm256_loadu_si256(static_cast<__m256i*>(_Last));
            const __m256i _Left_perm      = _mm256_permute4x64_epi64(_Left, _MM_SHUFFLE(1, 0, 3, 2));
            const __m256i _Right_perm     = _mm256_permute4x64_epi64(_Right, _MM_SHUFFLE(1, 0, 3, 2));
            const __m256i _Left_reversed  = _mm256_shuffle_epi32(_Left_perm, _MM_SHUFFLE(0, 1, 2, 3));
            const __m256i _Right_reversed = _mm256_shuffle_epi32(_Right_perm, _MM_SHUFFLE(0, 1, 2, 3));
            _mm256_storeu_si256(static_cast<__m256i*>(_First), _Right_reversed);
            _mm256_storeu_si256(static_cast<__m256i*>(_Last), _Left_reversed);
            _Advance_bytes(_First, 32);
        } while (_First != _Stop_at);
    }
#endif // !defined(_M_ARM64EC)

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
#if !defined(_M_ARM64EC)
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
#endif // !defined(_M_ARM64EC)

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
#if !defined(_M_ARM64EC)
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
#endif // !defined(_M_ARM64EC)

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
#if !defined(_M_ARM64EC)
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
#endif // !defined(_M_ARM64EC)

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
#if !defined(_M_ARM64EC)
    if (_Byte_length(_First, _Last) >= 32 && _Use_avx2()) {
        const void* _Stop_at = _Dest;
        _Advance_bytes(_Stop_at, _Byte_length(_First, _Last) >> 5 << 5);
        do {
            _Advance_bytes(_Last, -32);
            const __m256i _Block          = _mm256_loadu_si256(static_cast<const __m256i*>(_Last));
            const __m256i _Block_permuted = _mm256_permute4x64_epi64(_Block, _MM_SHUFFLE(1, 0, 3, 2));
            const __m256i _Block_reversed = _mm256_shuffle_epi32(_Block_permuted, _MM_SHUFFLE(0, 1, 2, 3));
            _mm256_storeu_si256(static_cast<__m256i*>(_Dest), _Block_reversed);
            _Advance_bytes(_Dest, 32);
        } while (_Dest != _Stop_at);
    }
#endif // !defined(_M_ARM64EC)

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
#if !defined(_M_ARM64EC)
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
#endif // !defined(_M_ARM64EC)

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

template <class _Ty>
static const void* _Find_trivial_unsized_fallback(const void* _First, _Ty _Val) {
    auto _Ptr = static_cast<const _Ty*>(_First);
    for (;;) {
        if (*_Ptr == _Val) {
            return _Ptr;
        }
        ++_Ptr;
    }
}

template <class _Callback>
static const void* VECTORCALL _Find_trivial_unsized_avx(
    const void* _First, __m256i _Comparand, _Callback _Get_mask) noexcept {
    // We read by vector-sized pieces, and we align pointers to vector-sized boundary.
    // From start/end partial pieces we mask out matches that don't belong to the range.
    // This makes sure we never cross page boundary, thus we read 'as if' sequentially.
    // Also, vector instructions favor aligned accesses.
    constexpr size_t _Vector_pad_mask = 0x1F;
    constexpr unsigned _Full_mask     = 0xFFFF'FFFF;

    const intptr_t _Pad_start = reinterpret_cast<intptr_t>(_First) & _Vector_pad_mask;
    unsigned _Mask            = (_Full_mask << _Pad_start);
    _Advance_bytes(_First, -_Pad_start);

    for (;;) {
        unsigned _Bingo = static_cast<unsigned>(_Get_mask(_First, _Comparand));

        if ((_Bingo &= _Mask) != 0) {
            unsigned long _Offset = _tzcnt_u32(_Bingo);
            _Advance_bytes(_First, _Offset);
            return _First;
        }

        _Advance_bytes(_First, 32);
        _Mask = _Full_mask;
    };
}

template <class _Callback>
static const void* VECTORCALL _Find_trivial_unsized_sse(
    const void* _First, __m128i _Comparand, _Callback _Get_mask) noexcept {
    // We read by vector-sized pieces, and we align pointers to vector-sized boundary.
    // From start/end partial pieces we mask out matches that don't belong to the range.
    // This makes sure we never cross page boundary, thus we read 'as if' sequentially.
    // Also, vector instructions favor aligned accesses.
    constexpr size_t _Vector_pad_mask = 0xF;
    constexpr unsigned _Full_mask     = 0xFFFF;

    const intptr_t _Pad_start = reinterpret_cast<intptr_t>(_First) & _Vector_pad_mask;
    unsigned _Mask            = (_Full_mask << _Pad_start);
    _Advance_bytes(_First, -_Pad_start);

    for (;;) {
        unsigned _Bingo = static_cast<unsigned>(_Get_mask(_First, _Comparand));
        unsigned long _Offset;

        if (_BitScanForward(&_Offset, _Bingo &= _Mask)) {
            _Advance_bytes(_First, _Offset);
            return _First;
        }

        _Advance_bytes(_First, 16);
        _Mask = _Full_mask;
    };
}

extern "C" {

#if 0

const void* __stdcall __std_find_trivial_unsized_1(const void* _First, uint8_t _Val) noexcept {
    if (_Use_avx2()) {
        const __m256i _Comparand = _mm256_set1_epi8(_Val);
        return _Find_trivial_unsized_avx(_First, _Comparand, [](const void* _Current, __m256i _Comparand) {
            __m256i _Data = _mm256_load_si256(static_cast<const __m256i*>(_Current));
            return _mm256_movemask_epi8(_mm256_cmpeq_epi8(_Data, _Comparand));
        });
    }
    if (_Use_sse2()) {
        const __m128i _Comparand = _mm_set1_epi8(_Val);
        return _Find_trivial_unsized_sse(_First, _Comparand, [](const void* _Current, __m128i _Comparand) {
            __m128i _Data = _mm_load_si128(static_cast<const __m128i*>(_Current));
            return _mm_movemask_epi8(_mm_cmpeq_epi8(_Data, _Comparand));
        });
    }
    return _Find_trivial_unsized_fallback(_First, _Val);
}

const void* __stdcall __std_find_trivial_unsized_2(const void* _First, uint16_t _Val) noexcept {
    if (_Use_avx2()) {
        const __m256i _Comparand = _mm256_set1_epi16(_Val);
        return _Find_trivial_unsized_avx(_First, _Comparand, [](const void* _Current, __m256i _Comparand) {
            __m256i _Data = _mm256_load_si256(static_cast<const __m256i*>(_Current));
            return _mm256_movemask_epi8(_mm256_cmpeq_epi16(_Data, _Comparand));
        });
    }
    if (_Use_sse2()) {
        const __m128i _Comparand = _mm_set1_epi16(_Val);
        return _Find_trivial_unsized_sse(_First, _Comparand, [](const void* _Current, __m128i _Comparand) {
            __m128i _Data = _mm_load_si128(static_cast<const __m128i*>(_Current));
            return _mm_movemask_epi8(_mm_cmpeq_epi16(_Data, _Comparand));
        });
    }
    return _Find_trivial_unsized_fallback(_First, _Val);
}

const void* __stdcall __std_find_trivial_unsized_4(const void* _First, uint32_t _Val) noexcept {
    if (_Use_avx2()) {
        const __m256i _Comparand = _mm256_set1_epi32(_Val);
        return _Find_trivial_unsized_avx(_First, _Comparand, [](const void* _Current, __m256i _Comparand) {
            __m256i _Data = _mm256_load_si256(static_cast<const __m256i*>(_Current));
            return _mm256_movemask_epi8(_mm256_cmpeq_epi32(_Data, _Comparand));
        });
    }
    if (_Use_sse2()) {
        const __m128i _Comparand = _mm_set1_epi32(_Val);
        return _Find_trivial_unsized_sse(_First, _Comparand, [](const void* _Current, __m128i _Comparand) {
            __m128i _Data = _mm_load_si128(static_cast<const __m128i*>(_Current));
            return _mm_movemask_epi8(_mm_cmpeq_epi32(_Data, _Comparand));
        });
    }
    return _Find_trivial_unsized_fallback(_First, _Val);
}

const void* __stdcall __std_find_trivial_unsized_8(const void* _First, uint64_t _Val) noexcept {
    if (_Use_avx2()) {
        const __m256i _Comparand = _mm256_set1_epi64x(_Val);
        return _Find_trivial_unsized_avx(_First, _Comparand, [](const void* _Current, __m256i _Comparand) {
            __m256i _Data = _mm256_load_si256(static_cast<const __m256i*>(_Current));
            return _mm256_movemask_epi8(_mm256_cmpeq_epi64(_Data, _Comparand));
        });
    }
    if (_Use_sse42()) {
        const __m128i _Comparand = _mm_set1_epi64x(_Val);
        return _Find_trivial_unsized_sse(_First, _Comparand, [](const void* _Current, __m128i _Comparand) {
            __m128i _Data = _mm_load_si128(static_cast<const __m128i*>(_Current));
            return _mm_movemask_epi8(_mm_cmpeq_epi64(_Data, _Comparand)); // SSE 4.1
        });
    }
    return _Find_trivial_unsized_fallback(_First, _Val);
}

const void* __stdcall __std_find_trivial_1(const void* _First, size_t _Size, uint8_t _Val) noexcept {
    size_t _Avx_size = _Size & ~size_t{0x1F};
    if (_Avx_size != 0 && _Use_avx2()) {
        const __m256i _Comparand = _mm256_set1_epi8(_Val);
        const void* _Stop_at     = _First;
        _Advance_bytes(_Stop_at, _Avx_size);
        do {
            const __m256i _Data = _mm256_loadu_si256(static_cast<const __m256i*>(_First));
            int _Bingo          = _mm256_movemask_epi8(_mm256_cmpeq_epi8(_Data, _Comparand));
            if (_Bingo != 0) {
                const unsigned long _Offset = _tzcnt_u32(_Bingo);
                _Advance_bytes(_First, _Offset);
                return _First;
            }
            _Advance_bytes(_First, 32);

        } while (_First != _Stop_at);
        _Size &= 0x1F;
    }

    size_t _Sse_size = _Size & ~size_t{0xF};
    if (_Sse_size != 0 && _Use_sse2()) {
        const __m128i _Comparand = _mm_set1_epi8(_Val);
        const void* _Stop_at     = _First;
        _Advance_bytes(_Stop_at, _Sse_size);
        do {
            const __m128i _Data = _mm_loadu_si128(static_cast<const __m128i*>(_First));
            int _Bingo          = _mm_movemask_epi8(_mm_cmpeq_epi8(_Data, _Comparand));
            unsigned long _Offset;
            if (_BitScanForward(&_Offset, _Bingo)) {
                _Advance_bytes(_First, _Offset);
                return _First;
            }
            _Advance_bytes(_First, 16);

        } while (_First != _Stop_at);
        _Size &= 0xF;
    }

    // clang-format off
     auto _Ptr = static_cast<const uint8_t*>(_First);
     switch (_Size) {
     case 15: if (*_Ptr == _Val) { return _Ptr; } ++_Ptr; [[fallthrough]];
     case 14: if (*_Ptr == _Val) { return _Ptr; } ++_Ptr; [[fallthrough]];
     case 13: if (*_Ptr == _Val) { return _Ptr; } ++_Ptr; [[fallthrough]];
     case 12: if (*_Ptr == _Val) { return _Ptr; } ++_Ptr; [[fallthrough]];
     case 11: if (*_Ptr == _Val) { return _Ptr; } ++_Ptr; [[fallthrough]];
     case 10: if (*_Ptr == _Val) { return _Ptr; } ++_Ptr; [[fallthrough]];
     case 9:  if (*_Ptr == _Val) { return _Ptr; } ++_Ptr; [[fallthrough]];
     case 8:  if (*_Ptr == _Val) { return _Ptr; } ++_Ptr; [[fallthrough]];
     case 7:  if (*_Ptr == _Val) { return _Ptr; } ++_Ptr; [[fallthrough]];
     case 6:  if (*_Ptr == _Val) { return _Ptr; } ++_Ptr; [[fallthrough]];
     case 5:  if (*_Ptr == _Val) { return _Ptr; } ++_Ptr; [[fallthrough]];
     case 4:  if (*_Ptr == _Val) { return _Ptr; } ++_Ptr; [[fallthrough]];
     case 3:  if (*_Ptr == _Val) { return _Ptr; } ++_Ptr; [[fallthrough]];
     case 2:  if (*_Ptr == _Val) { return _Ptr; } ++_Ptr; [[fallthrough]];
     case 1:  if (*_Ptr == _Val) { return _Ptr; } ++_Ptr; [[fallthrough]];
     case 0:  return _Ptr;
     default: __assume(false);
    }
    // clang-format on
}

const void* __stdcall __std_find_trivial_2(const void* _First, size_t _Size, uint16_t _Val) noexcept {
    size_t Size_bytes = _Size * 2;

    size_t _Avx_size = Size_bytes & ~size_t{0x1F};
    if (_Avx_size != 0 && _Use_avx2()) {
        const __m256i _Comparand = _mm256_set1_epi16(_Val);
        const void* _Stop_at     = _First;
        _Advance_bytes(_Stop_at, _Avx_size);
        do {
            const __m256i _Data = _mm256_loadu_si256(static_cast<const __m256i*>(_First));
            int _Bingo          = _mm256_movemask_epi8(_mm256_cmpeq_epi16(_Data, _Comparand));
            if (_Bingo != 0) {
                const unsigned long _Offset = _tzcnt_u32(_Bingo);
                _Advance_bytes(_First, _Offset);
                return _First;
            }
            _Advance_bytes(_First, 32);

        } while (_First != _Stop_at);
        Size_bytes &= 0x1F;
    }

    size_t _Sse_size = Size_bytes & ~size_t{0xF};
    if (_Sse_size != 0 && _Use_sse2()) {
        const __m128i _Comparand = _mm_set1_epi16(_Val);
        const void* _Stop_at     = _First;
        _Advance_bytes(_Stop_at, _Sse_size);
        do {
            const __m128i _Data = _mm_loadu_si128(static_cast<const __m128i*>(_First));
            int _Bingo          = _mm_movemask_epi8(_mm_cmpeq_epi16(_Data, _Comparand));
            unsigned long _Offset;
            if (_BitScanForward(&_Offset, _Bingo)) {
                _Advance_bytes(_First, _Offset);
                return _First;
            }
            _Advance_bytes(_First, 16);

        } while (_First != _Stop_at);
        Size_bytes &= 0xF;
    }

    // clang-format off
     auto _Ptr = static_cast<const uint16_t*>(_First);
     switch (Size_bytes >> 1) {
     case 7:  if (*_Ptr == _Val) { return _Ptr; } ++_Ptr; [[fallthrough]];
     case 6:  if (*_Ptr == _Val) { return _Ptr; } ++_Ptr; [[fallthrough]];
     case 5:  if (*_Ptr == _Val) { return _Ptr; } ++_Ptr; [[fallthrough]];
     case 4:  if (*_Ptr == _Val) { return _Ptr; } ++_Ptr; [[fallthrough]];
     case 3:  if (*_Ptr == _Val) { return _Ptr; } ++_Ptr; [[fallthrough]];
     case 2:  if (*_Ptr == _Val) { return _Ptr; } ++_Ptr; [[fallthrough]];
     case 1:  if (*_Ptr == _Val) { return _Ptr; } ++_Ptr; [[fallthrough]];
     case 0:  return _Ptr;
     default: __assume(false);
    }
    // clang-format on
}

const void* __stdcall __std_find_trivial_4(const void* _First, size_t _Size, uint32_t _Val) noexcept {
    size_t Size_bytes = _Size * 4;

    size_t _Avx_size = Size_bytes & ~size_t{0x1F};
    if (_Avx_size != 0 && _Use_avx2()) {
        const __m256i _Comparand = _mm256_set1_epi32(_Val);
        const void* _Stop_at     = _First;
        _Advance_bytes(_Stop_at, _Avx_size);
        do {
            const __m256i _Data = _mm256_loadu_si256(static_cast<const __m256i*>(_First));
            int _Bingo          = _mm256_movemask_epi8(_mm256_cmpeq_epi32(_Data, _Comparand));
            if (_Bingo != 0) {
                const unsigned long _Offset = _tzcnt_u32(_Bingo);
                _Advance_bytes(_First, _Offset);
                return _First;
            }
            _Advance_bytes(_First, 32);

        } while (_First != _Stop_at);
        Size_bytes &= 0x1F;
    }

    size_t _Sse_size = Size_bytes & ~size_t{0xF};
    if (_Sse_size != 0 && _Use_sse2()) {
        const __m128i _Comparand = _mm_set1_epi32(_Val);
        const void* _Stop_at     = _First;
        _Advance_bytes(_Stop_at, _Sse_size);
        do {
            const __m128i _Data = _mm_loadu_si128(static_cast<const __m128i*>(_First));
            int _Bingo          = _mm_movemask_epi8(_mm_cmpeq_epi32(_Data, _Comparand));
            unsigned long _Offset;
            if (_BitScanForward(&_Offset, _Bingo)) {
                _Advance_bytes(_First, _Offset);
                return _First;
            }
            _Advance_bytes(_First, 16);

        } while (_First != _Stop_at);
        Size_bytes &= 0xF;
    }

    // clang-format off
     auto _Ptr = static_cast<const uint32_t*>(_First);
     switch (Size_bytes >> 2) {
     case 3:  if (*_Ptr == _Val) { return _Ptr; } ++_Ptr; [[fallthrough]];
     case 2:  if (*_Ptr == _Val) { return _Ptr; } ++_Ptr; [[fallthrough]];
     case 1:  if (*_Ptr == _Val) { return _Ptr; } ++_Ptr; [[fallthrough]];
     case 0:  return _Ptr;
     default: __assume(false);
    }
    // clang-format on
}

const void* __stdcall __std_find_trivial_8(const void* _First, size_t _Size, uint64_t _Val) noexcept {
    size_t Size_bytes = _Size * 8;

    size_t _Avx_size = Size_bytes & ~size_t{0x1F};
    if (_Avx_size != 0 && _Use_avx2()) {
        const __m256i _Comparand = _mm256_set1_epi64x(_Val);
        const void* _Stop_at     = _First;
        _Advance_bytes(_Stop_at, _Avx_size);
        do {
            const __m256i _Data = _mm256_loadu_si256(static_cast<const __m256i*>(_First));
            int _Bingo          = _mm256_movemask_epi8(_mm256_cmpeq_epi64(_Data, _Comparand));
            if (_Bingo != 0) {
                const unsigned long _Offset = _tzcnt_u32(_Bingo);
                _Advance_bytes(_First, _Offset);
                return _First;
            }
            _Advance_bytes(_First, 32);

        } while (_First != _Stop_at);
        Size_bytes &= 0x1F;
    }

    size_t _Sse_size = Size_bytes & ~size_t{0xF};
    if (_Sse_size != 0 && _Use_sse42()) {
        const __m128i _Comparand = _mm_set1_epi64x(_Val);
        const void* _Stop_at     = _First;
        _Advance_bytes(_Stop_at, _Sse_size);
        do {
            const __m128i _Data = _mm_loadu_si128(static_cast<const __m128i*>(_First));
            int _Bingo          = _mm_movemask_epi8(_mm_cmpeq_epi64(_Data, _Comparand));
            unsigned long _Offset;
            if (_BitScanForward(&_Offset, _Bingo)) {
                _Advance_bytes(_First, _Offset);
                return _First;
            }
            _Advance_bytes(_First, 16);

        } while (_First != _Stop_at);
        Size_bytes &= 0xF;
    }

    auto _Ptr = static_cast<const uint64_t*>(_First);
    if (Size_bytes != 0) {
        if (*_Ptr == _Val) {
            return _Ptr;
        }
        ++_Ptr;
    }
    return _Ptr;
}

#endif

} // extern "C"

template <class _Vector, class _Callback>
static const void* VECTORCALL __std_find_trivial(
    const void* _First, size_t _Size, _Vector _Comparand, _Callback _Get_mask) noexcept {
    // We read by vector-sized pieces, and we align pointers to vector-sized boundary.
    // From start/end partial pieces we mask out matches that don't belong to the range.
    // This makes sure we never cross page boundary, thus we read 'as if' sequentially.
    // Also, vector instructions favor aligned accesses.
    intptr_t _Pad_stop                = 0;
    constexpr size_t _Vector_size     = sizeof(_Vector);
    constexpr size_t _Vector_pad_mask = _Vector_size - 1;
    constexpr unsigned _Full_mask     = static_cast<unsigned>((1ULL << _Vector_size) - 1);
    const void* _Stop_at              = nullptr;
    unsigned _Stop_mask               = 0;

    if (_Size != SIZE_MAX) {
        _Stop_at = _First;
        _Advance_bytes(_Stop_at, _Size);
        _Pad_stop = reinterpret_cast<intptr_t>(_Stop_at) & _Vector_pad_mask;
        if (_Pad_stop == 0) {
            _Pad_stop = _Vector_size;
        }
        _Advance_bytes(_Stop_at, -_Pad_stop);
        _Stop_mask = _Full_mask >> (_Vector_size - _Pad_stop);
    }

    const intptr_t _Pad_start = reinterpret_cast<intptr_t>(_First) & _Vector_pad_mask;
    unsigned _Mask            = (_Full_mask << _Pad_start);
    _Advance_bytes(_First, -_Pad_start);

    for (;;) {
        if (_First == _Stop_at) {
            _Mask = _Stop_mask;
        }

        unsigned _Bingo = static_cast<unsigned>(_Get_mask(_First, _Comparand));

        if ((_Bingo &= _Mask) != 0) {
            unsigned long _Offset;
            if constexpr (_Vector_size == 32) {
                _Offset = _tzcnt_u32(_Bingo);
            } else {
                _BitScanForward(&_Offset, _Bingo);
            }
            _Advance_bytes(_First, _Offset);
            return _First;
        }

        if (_First == _Stop_at) {
            _Advance_bytes(_First, _Pad_stop);
            return _First;
        }

        _Advance_bytes(_First, _Vector_size);
        _Mask = _Full_mask;
    };
}

template <class _Ty>
static const void* __std_find_trivial_fallback(const void* _First, size_t _Size, _Ty _Val) {
    auto _Ptr  = static_cast<const _Ty*>(_First);
    auto _Last = _Ptr + _Size;
    for (; _Ptr != _Last; ++_Ptr) {
        if (*_Ptr == _Val) {
            break;
        }
    }
    return _Ptr;
}

extern "C" {

const void* __stdcall __std_find_trivial_1(const void* _First, size_t _Size, uint8_t _Val) noexcept {
    if (_Size >= 32 && _Use_avx2()) {
        const __m256i _Comparand = _mm256_set1_epi8(_Val);

        return __std_find_trivial(_First, _Size, _Comparand, [](const void* _Current, __m256i _Comparand) {
            __m256i _Data = _mm256_load_si256(static_cast<const __m256i*>(_Current));
            return _mm256_movemask_epi8(_mm256_cmpeq_epi8(_Data, _Comparand));
        });
    }

    if (_Size >= 16 && _Use_sse2()) {
        const __m128i _Comparand = _mm_set1_epi8(_Val);

        return __std_find_trivial(_First, _Size, _Comparand, [](const void* _Current, __m128i _Comparand) {
            __m128i _Data = _mm_load_si128(static_cast<const __m128i*>(_Current));
            return _mm_movemask_epi8(_mm_cmpeq_epi8(_Data, _Comparand));
        });
    }

    return __std_find_trivial_fallback(_First, _Size, _Val);
}

const void* __stdcall __std_find_trivial_2(const void* _First, size_t _Size, uint16_t _Val) noexcept {
    size_t _Bytes_size = _Size * 2;

    if (_Bytes_size >= 32 && _Use_avx2()) {
        const __m256i _Comparand = _mm256_set1_epi16(_Val);

        return __std_find_trivial(_First, _Bytes_size, _Comparand, [](const void* _Current, __m256i _Comparand) {
            __m256i _Data = _mm256_load_si256(static_cast<const __m256i*>(_Current));
            return _mm256_movemask_epi8(_mm256_cmpeq_epi16(_Data, _Comparand));
        });
    }

    if (_Bytes_size >= 16 && _Use_sse2()) {
        const __m128i _Comparand = _mm_set1_epi16(_Val);

        return __std_find_trivial(_First, _Bytes_size, _Comparand, [](const void* _Current, __m128i _Comparand) {
            __m128i _Data = _mm_load_si128(static_cast<const __m128i*>(_Current));
            return _mm_movemask_epi8(_mm_cmpeq_epi16(_Data, _Comparand));
        });
    }

    return __std_find_trivial_fallback(_First, _Size, _Val);
}

const void* __stdcall __std_find_trivial_4(const void* _First, size_t _Size, uint32_t _Val) noexcept {
    size_t _Bytes_size = _Size * 4;

    if (_Bytes_size >= 32 && _Use_avx2()) {
        const __m256i _Comparand = _mm256_set1_epi32(_Val);

        return __std_find_trivial(_First, _Bytes_size, _Comparand, [](const void* _Current, __m256i _Comparand) {
            __m256i _Data = _mm256_load_si256(static_cast<const __m256i*>(_Current));
            return _mm256_movemask_epi8(_mm256_cmpeq_epi32(_Data, _Comparand));
        });
    }

    if (_Bytes_size >= 16 && _Use_sse2()) {
        const __m128i _Comparand = _mm_set1_epi32(_Val);

        return __std_find_trivial(_First, _Bytes_size, _Comparand, [](const void* _Current, __m128i _Comparand) {
            __m128i _Data = _mm_load_si128(static_cast<const __m128i*>(_Current));
            return _mm_movemask_epi8(_mm_cmpeq_epi32(_Data, _Comparand));
        });
    }

    return __std_find_trivial_fallback(_First, _Size, _Val);
}

const void* __stdcall __std_find_trivial_8(const void* _First, size_t _Size, uint64_t _Val) noexcept {
    size_t _Bytes_size = _Size * 8;

    if (_Bytes_size >= 32 && _Use_avx2()) {
        const __m256i _Comparand = _mm256_set1_epi64x(_Val);

        return __std_find_trivial(_First, _Bytes_size, _Comparand, [](const void* _Current, __m256i _Comparand) {
            __m256i _Data = _mm256_load_si256(static_cast<const __m256i*>(_Current));
            return _mm256_movemask_epi8(_mm256_cmpeq_epi64(_Data, _Comparand));
        });
    }

    if (_Bytes_size >= 16 && _Use_sse42()) {
        const __m128i _Comparand = _mm_set1_epi64x(_Val);

        return __std_find_trivial(_First, _Bytes_size, _Comparand, [](const void* _Current, __m128i _Comparand) {
            __m128i _Data = _mm_load_si128(static_cast<const __m128i*>(_Current));
            return _mm_movemask_epi8(_mm_cmpeq_epi64(_Data, _Comparand)); // SSE4.1
        });
    }

    return __std_find_trivial_fallback(_First, _Size, _Val);
}

const void* __stdcall __std_find_trivial_unsized_1(const void* _First, uint8_t _Val) noexcept {
    return __std_find_trivial_1(_First, SIZE_MAX, _Val);
}

const void* __stdcall __std_find_trivial_unsized_2(const void* _First, uint16_t _Val) noexcept {
    return __std_find_trivial_2(_First, SIZE_MAX, _Val);
}

const void* __stdcall __std_find_trivial_unsized_4(const void* _First, uint32_t _Val) noexcept {
    return __std_find_trivial_4(_First, SIZE_MAX, _Val);
}

const void* __stdcall __std_find_trivial_unsized_8(const void* _First, uint64_t _Val) noexcept {
    return __std_find_trivial_8(_First, SIZE_MAX, _Val);
}

} // extern "C"

#endif // defined(_M_IX86) || defined(_M_X64)
