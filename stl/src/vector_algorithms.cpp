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
#include <stdlib.h>

extern "C" long __isa_enabled;

#pragma optimize("t", on) // Override /Os with /Ot for this TU

static bool _Use_sse42() {
    return __isa_enabled & (1 << __ISA_AVAILABLE_SSE42);
}

struct _Min_max_t {
    const void* _Min;
    const void* _Max;
};

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
    if (_Byte_length(_First1, _Last1) >= 32 && _bittest(&__isa_enabled, __ISA_AVAILABLE_AVX2)) {
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
    if (_Byte_length(_First1, _Last1) >= 16
#ifdef _M_IX86
        && _bittest(&__isa_enabled, __ISA_AVAILABLE_SSE2)
#endif // _M_IX86
    ) {
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
    if (_Byte_length(_First, _Last) >= 64 && _bittest(&__isa_enabled, __ISA_AVAILABLE_AVX2)) {
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

    if (_Byte_length(_First, _Last) >= 32 && _bittest(&__isa_enabled, __ISA_AVAILABLE_SSE42)) {
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
    if (_Byte_length(_First, _Last) >= 64 && _bittest(&__isa_enabled, __ISA_AVAILABLE_AVX2)) {
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

    if (_Byte_length(_First, _Last) >= 32 && _bittest(&__isa_enabled, __ISA_AVAILABLE_SSE42)) {
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
    if (_Byte_length(_First, _Last) >= 64 && _bittest(&__isa_enabled, __ISA_AVAILABLE_AVX2)) {
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

    if (_Byte_length(_First, _Last) >= 32
#ifdef _M_IX86
        && _bittest(&__isa_enabled, __ISA_AVAILABLE_SSE2)
#endif // _M_IX86
    ) {
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
    if (_Byte_length(_First, _Last) >= 64 && _bittest(&__isa_enabled, __ISA_AVAILABLE_AVX2)) {
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

    if (_Byte_length(_First, _Last) >= 32
#ifdef _M_IX86
        && _bittest(&__isa_enabled, __ISA_AVAILABLE_SSE2)
#endif // _M_IX86
    ) {
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
    if (_Byte_length(_First, _Last) >= 32 && _bittest(&__isa_enabled, __ISA_AVAILABLE_AVX2)) {
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

    if (_Byte_length(_First, _Last) >= 16 && _bittest(&__isa_enabled, __ISA_AVAILABLE_SSE42)) {
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
    if (_Byte_length(_First, _Last) >= 32 && _bittest(&__isa_enabled, __ISA_AVAILABLE_AVX2)) {
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

    if (_Byte_length(_First, _Last) >= 16 && _bittest(&__isa_enabled, __ISA_AVAILABLE_SSE42)) {
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
    if (_Byte_length(_First, _Last) >= 32 && _bittest(&__isa_enabled, __ISA_AVAILABLE_AVX2)) {
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

    if (_Byte_length(_First, _Last) >= 16
#ifdef _M_IX86
        && _bittest(&__isa_enabled, __ISA_AVAILABLE_SSE2)
#endif // _M_IX86
    ) {
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
    if (_Byte_length(_First, _Last) >= 32 && _bittest(&__isa_enabled, __ISA_AVAILABLE_AVX2)) {
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

    if (_Byte_length(_First, _Last) >= 16
#ifdef _M_IX86
        && _bittest(&__isa_enabled, __ISA_AVAILABLE_SSE2)
#endif // _M_IX86
    ) {
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
_Min_max_t _Both_tail(
    const void* const _First, const void* const _Last, _Min_max_t& _Res, _Ty _Cur_min, _Ty _Cur_max) noexcept {
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

enum class _Min_max_mode {
    _Min_only,
    _Max_only,
    _Both,
};

template <_Min_max_mode _Mode, class _STy, class _UTy>
auto _Minmax_tail(
    const void* _First, const void* _Last, _Min_max_t& _Res, bool _Sign, _UTy _Cur_min, _UTy _Cur_max) noexcept {
    constexpr _UTy _Cor = (_UTy{1} << (sizeof(_UTy) * CHAR_BIT - 1));

    if constexpr (_Mode == _Min_max_mode::_Min_only) {
        if (_Sign) {
            return _Min_tail(_First, _Last, _Res._Min, static_cast<_STy>(_Cur_min));
        } else {
            return _Min_tail(_First, _Last, _Res._Min, static_cast<_UTy>(_Cur_min + _Cor));
        }
    } else if constexpr (_Mode == _Min_max_mode::_Max_only) {
        if (_Sign) {
            return _Max_tail(_First, _Last, _Res._Max, static_cast<_STy>(_Cur_max));
        } else {
            return _Max_tail(_First, _Last, _Res._Max, static_cast<_UTy>(_Cur_max + _Cor));
        }
    } else {
        if (_Sign) {
            return _Both_tail(_First, _Last, _Res, static_cast<_STy>(_Cur_min), static_cast<_STy>(_Cur_max));
        } else {
            return _Both_tail(
                _First, _Last, _Res, static_cast<_UTy>(_Cur_min + _Cor), static_cast<_UTy>(_Cur_max + _Cor));
        }
    }
}

struct _Minmax_traits_1 {
    using _Signed_t   = int8_t;
    using _Unsigned_t = uint8_t;

    static constexpr bool _Has_portion_max = true;
    static constexpr size_t _Portion_max   = 256;

    static constexpr _Signed_t _Init_min_val = static_cast<_Signed_t>(0x7F);
    static constexpr _Signed_t _Init_max_val = static_cast<_Signed_t>(0x80);

    static __m128i _Sign_cor(const __m128i _Val, const bool _Sign) noexcept {
        alignas(16) static constexpr _Unsigned_t _Sign_cors[2][16] = {
            {0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80}, {}};
        return _mm_sub_epi8(_Val, _mm_loadu_si128(reinterpret_cast<const __m128i*>(_Sign_cors[_Sign])));
    }

    static __m128i _Inc(__m128i _Idx) noexcept {
        return _mm_add_epi8(_Idx, _mm_set1_epi8(1));
    }

    template <class _Fn>
    static __m128i _H_func(const __m128i _Cur, _Fn _Funct) noexcept {
        const __m128i _Shuf_bytes = _mm_set_epi8(14, 15, 12, 13, 10, 11, 8, 9, 6, 7, 4, 5, 2, 3, 0, 1);
        const __m128i _Shuf_words = _mm_set_epi8(13, 12, 15, 14, 9, 8, 11, 10, 5, 4, 7, 6, 1, 0, 3, 2);

        __m128i _H_min = _Cur;
        _H_min         = _Funct(_H_min, _mm_shuffle_epi32(_H_min, _MM_SHUFFLE(1, 0, 3, 2)));
        _H_min         = _Funct(_H_min, _mm_shuffle_epi32(_H_min, _MM_SHUFFLE(2, 3, 0, 1)));
        _H_min         = _Funct(_H_min, _mm_shuffle_epi8(_H_min, _Shuf_words));
        _H_min         = _Funct(_H_min, _mm_shuffle_epi8(_H_min, _Shuf_bytes));
        return _H_min;
    }

    static __m128i _H_min(const __m128i _Cur) noexcept {
        return _H_func(_Cur, [](__m128i _First, __m128i _Second) { return _mm_min_epi8(_First, _Second); });
    }

    static __m128i _H_max(const __m128i _Cur) noexcept {
        return _H_func(_Cur, [](__m128i _First, __m128i _Second) { return _mm_max_epi8(_First, _Second); });
    }

    static __m128i _H_min_u(const __m128i _Cur) noexcept {
        return _H_func(_Cur, [](__m128i _First, __m128i _Second) { return _mm_min_epu8(_First, _Second); });
    }

    static __m128i _H_max_u(const __m128i _Cur) noexcept {
        return _H_func(_Cur, [](__m128i _First, __m128i _Second) { return _mm_max_epu8(_First, _Second); });
    }

    static _Signed_t _Get_any(const __m128i _Cur) noexcept {
        return static_cast<_Signed_t>(_mm_cvtsi128_si32(_Cur));
    }

    static _Unsigned_t _Get_v_pos(const __m128i _Idx, const unsigned long _H_pos) noexcept {
        return static_cast<uint8_t>(_mm_cvtsi128_si32(_mm_shuffle_epi8(_Idx, _mm_cvtsi32_si128(_H_pos))));
    }

    static __m128i _Cmp_eq(const __m128i _First, const __m128i _Second) noexcept {
        return _mm_cmpeq_epi8(_First, _Second);
    }

    static __m128i _Cmp_gt(const __m128i _First, const __m128i _Second) noexcept {
        return _mm_cmpgt_epi8(_First, _Second);
    }

    static __m128i _Cmp_lt(const __m128i _First, const __m128i _Second) noexcept {
        return _mm_cmplt_epi8(_First, _Second);
    }

    static __m128i _Min(const __m128i _First, const __m128i _Second, __m128i) noexcept {
        return _mm_min_epi8(_First, _Second);
    }

    static __m128i _Max(const __m128i _First, __m128i _Second, __m128i) noexcept {
        return _mm_max_epi8(_First, _Second);
    }
};

struct _Minmax_traits_2 {
    using _Signed_t   = int16_t;
    using _Unsigned_t = uint16_t;

    static constexpr bool _Has_portion_max = true;
    static constexpr size_t _Portion_max   = 65536;

    static constexpr _Signed_t _Init_min_val = static_cast<_Signed_t>(0x7FFF);
    static constexpr _Signed_t _Init_max_val = static_cast<_Signed_t>(0x8000);

    static __m128i _Sign_cor(const __m128i _Val, const bool _Sign) noexcept {
        alignas(16) static constexpr _Unsigned_t _Sign_cors[2][8] = {
            0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, {}};
        return _mm_sub_epi16(_Val, _mm_loadu_si128(reinterpret_cast<const __m128i*>(_Sign_cors[_Sign])));
    }

    static __m128i _Inc(__m128i _Idx) noexcept {
        return _mm_add_epi16(_Idx, _mm_set1_epi16(1));
    }

    template <class _Fn>
    static __m128i _H_func(const __m128i _Cur, _Fn _Funct) noexcept {
        const __m128i _Shuf_words = _mm_set_epi8(13, 12, 15, 14, 9, 8, 11, 10, 5, 4, 7, 6, 1, 0, 3, 2);

        __m128i _H_min = _Cur;
        _H_min         = _Funct(_H_min, _mm_shuffle_epi32(_H_min, _MM_SHUFFLE(1, 0, 3, 2)));
        _H_min         = _Funct(_H_min, _mm_shuffle_epi32(_H_min, _MM_SHUFFLE(2, 3, 0, 1)));
        _H_min         = _Funct(_H_min, _mm_shuffle_epi8(_H_min, _Shuf_words));
        return _H_min;
    }

    static __m128i _H_min(const __m128i _Cur) noexcept {
        return _H_func(_Cur, [](__m128i _First, __m128i _Second) { return _mm_min_epi16(_First, _Second); });
    }

    static __m128i _H_max(const __m128i _Cur) noexcept {
        return _H_func(_Cur, [](__m128i _First, __m128i _Second) { return _mm_max_epi16(_First, _Second); });
    }

    static __m128i _H_min_u(const __m128i _Cur) noexcept {
        return _H_func(_Cur, [](__m128i _First, __m128i _Second) { return _mm_min_epu16(_First, _Second); });
    }

    static __m128i _H_max_u(const __m128i _Cur) noexcept {
        return _H_func(_Cur, [](__m128i _First, __m128i _Second) { return _mm_max_epu16(_First, _Second); });
    }

    static _Signed_t _Get_any(const __m128i _Cur) noexcept {
        return static_cast<_Signed_t>(_mm_cvtsi128_si32(_Cur));
    }

    static _Unsigned_t _Get_v_pos(const __m128i _Idx, const unsigned long _H_pos) noexcept {
        _Unsigned_t _Array[8];
        _mm_storeu_si128(reinterpret_cast<__m128i*>(&_Array), _Idx);
        return _Array[_H_pos >> 1];
    }

    static __m128i _Cmp_eq(const __m128i _First, const __m128i _Second) noexcept {
        return _mm_cmpeq_epi16(_First, _Second);
    }

    static __m128i _Cmp_gt(const __m128i _First, const __m128i _Second) noexcept {
        return _mm_cmpgt_epi16(_First, _Second);
    }

    static __m128i _Cmp_lt(const __m128i _First, const __m128i _Second) noexcept {
        return _mm_cmplt_epi16(_First, _Second);
    }

    static __m128i _Min(const __m128i _First, const __m128i _Second, const __m128i) noexcept {
        return _mm_min_epi16(_First, _Second);
    }

    static __m128i _Max(const __m128i _First, const __m128i _Second, const __m128i) noexcept {
        return _mm_max_epi16(_First, _Second);
    }
};


struct _Minmax_traits_4 {
    using _Signed_t   = int32_t;
    using _Unsigned_t = uint32_t;

#ifdef _M_IX86
    static constexpr bool _Has_portion_max = false;
#else // ^^^ 32-bit ^^^ / vvv 64-bit vvv
    static constexpr bool _Has_portion_max = true;
    static constexpr size_t _Portion_max   = 0x1'0000'0000ULL;
#endif // ^^^ 64-bit ^^^

    static constexpr _Signed_t _Init_min_val = static_cast<_Signed_t>(0x7FFF'FFFFUL);
    static constexpr _Signed_t _Init_max_val = static_cast<_Signed_t>(0x8000'0000UL);

    static __m128i _Sign_cor(const __m128i _Val, const bool _Sign) noexcept {
        alignas(16) static constexpr _Unsigned_t _Sign_cors[2][4] = {
            0x8000'0000UL, 0x8000'0000UL, 0x8000'0000UL, 0x8000'0000UL, {}};
        return _mm_sub_epi32(_Val, _mm_loadu_si128(reinterpret_cast<const __m128i*>(_Sign_cors[_Sign])));
    }

    static __m128i _Inc(__m128i _Idx) noexcept {
        return _mm_add_epi32(_Idx, _mm_set1_epi32(1));
    }

    template <class _Fn>
    static __m128i _H_func(const __m128i _Cur, _Fn _Funct) noexcept {
        const __m128i _Shuf_words = _mm_set_epi8(13, 12, 15, 14, 9, 8, 11, 10, 5, 4, 7, 6, 1, 0, 3, 2);

        __m128i _H_min = _Cur;
        _H_min         = _Funct(_H_min, _mm_shuffle_epi32(_H_min, _MM_SHUFFLE(1, 0, 3, 2)));
        _H_min         = _Funct(_H_min, _mm_shuffle_epi32(_H_min, _MM_SHUFFLE(2, 3, 0, 1)));
        return _H_min;
    }

    static __m128i _H_min(const __m128i _Cur) noexcept {
        return _H_func(_Cur, [](__m128i _First, __m128i _Second) { return _mm_min_epi32(_First, _Second); });
    }

    static __m128i _H_max(const __m128i _Cur) noexcept {
        return _H_func(_Cur, [](__m128i _First, __m128i _Second) { return _mm_max_epi32(_First, _Second); });
    }

    static __m128i _H_min_u(const __m128i _Cur) noexcept {
        return _H_func(_Cur, [](__m128i _First, __m128i _Second) { return _mm_min_epu32(_First, _Second); });
    }

    static __m128i _H_max_u(const __m128i _Cur) noexcept {
        return _H_func(_Cur, [](__m128i _First, __m128i _Second) { return _mm_max_epu32(_First, _Second); });
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

    static __m128i _Cmp_lt(const __m128i _First, const __m128i _Second) noexcept {
        return _mm_cmplt_epi32(_First, _Second);
    }

    static __m128i _Min(const __m128i _First, const __m128i _Second, __m128i) noexcept {
        return _mm_min_epi32(_First, _Second);
    }

    static __m128i _Max(const __m128i _First, const __m128i _Second, __m128i) noexcept {
        return _mm_max_epi32(_First, _Second);
    }
};

struct _Minmax_traits_8 {
    using _Signed_t   = int64_t;
    using _Unsigned_t = uint64_t;

    static constexpr bool _Has_portion_max = false;

    static constexpr _Signed_t _Init_min_val = static_cast<_Signed_t>(0x7FFF'FFFF'FFFF'FFFFULL);
    static constexpr _Signed_t _Init_max_val = static_cast<_Signed_t>(0x8000'0000'0000'0000ULL);

    static __m128i _Sign_cor(__m128i _Val, const bool _Sign) {
        alignas(16) static constexpr _Unsigned_t _Sign_cors[2][2] = {
            0x8000'0000'0000'0000ULL, 0x8000'0000'0000'0000ULL, {}};
        return _mm_sub_epi64(_Val, _mm_loadu_si128(reinterpret_cast<const __m128i*>(_Sign_cors[_Sign])));
    }

    static __m128i _Inc(__m128i _Idx) noexcept {
        return _mm_add_epi64(_Idx, _mm_set1_epi64x(1));
    }

    static __m128i _H_min(const __m128i _Cur) noexcept {
        _Signed_t _H_min_a = _Get_any(_Cur);
        _Signed_t _H_min_b = _Get_any(_mm_bsrli_si128(_Cur, 8));
        if (_H_min_b < _H_min_a) {
            _H_min_a = _H_min_b;
        }
        return _mm_set1_epi64x(_H_min_a);
    }

    static __m128i _H_max(const __m128i _Cur) noexcept {
        _Signed_t _H_max_a = _Get_any(_Cur);
        _Signed_t _H_max_b = _Get_any(_mm_bsrli_si128(_Cur, 8));
        if (_H_max_b > _H_max_a) {
            _H_max_a = _H_max_b;
        }
        return _mm_set1_epi64x(_H_max_a);
    }

    static __m128i _H_min_u(const __m128i _Cur) noexcept {
        _Unsigned_t _H_min_a = _Get_any(_Cur);
        _Unsigned_t _H_min_b = _Get_any(_mm_bsrli_si128(_Cur, 8));
        if (_H_min_b < _H_min_a) {
            _H_min_a = _H_min_b;
        }
        return _mm_set1_epi64x(_H_min_a);
    }


    static __m128i _H_max_u(const __m128i _Cur) noexcept {
        _Unsigned_t _H_max_a = _Get_any(_Cur);
        _Unsigned_t _H_max_b = _Get_any(_mm_bsrli_si128(_Cur, 8));
        if (_H_max_b > _H_max_a) {
            _H_max_a = _H_max_b;
        }
        return _mm_set1_epi64x(_H_max_a);
    }


    static _Signed_t _Get_any(const __m128i _Cur) noexcept {
#ifdef _M_IX86
        return static_cast<_Signed_t>(_mm_extract_epi32(_Cur, 1)) | static_cast<_Signed_t>(_mm_cvtsi128_si32(_Cur));
#else
        return static_cast<_Signed_t>(_mm_cvtsi128_si64(_Cur));
#endif
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

    static __m128i _Cmp_lt(const __m128i _First, const __m128i _Second) noexcept {
        __m128i _Gt = _mm_cmpgt_epi64(_Second, _First); // less or equal
        __m128i _Eq = _mm_cmpeq_epi64(_First, _Second);
        return _mm_andnot_si128(_Eq, _Gt);
    }

    static __m128i _Min(const __m128i _First, const __m128i _Second, const __m128i _Mask) noexcept {
        return _mm_blendv_epi8(_First, _Second, _Mask);
    }

    static __m128i _Max(const __m128i _First, const __m128i _Second, const __m128i _Mask) noexcept {
        return _mm_blendv_epi8(_First, _Second, _Mask);
    }
};

// Exactly the same signature with __std_min_element_N / __std_max_element_N / __std_minmax_element_N,
// up to calling convention. This makes sure the template specialization is fused with the export function.
// In optimized build it avoids extra call, as this function is too large to inline.
template <_Min_max_mode _Mode, class _Traits>
auto __stdcall _Minmax_element(const void* _First, const void* const _Last, const bool _Sign) noexcept {
    _Min_max_t _Res   = {_First, _First};
    auto _Base        = static_cast<const char*>(_First);
    auto _Cur_min_val = _Traits::_Init_min_val;
    auto _Cur_max_val = _Traits::_Init_max_val;

    if (_Byte_length(_First, _Last) >= 16 && _Use_sse42()) {
        const void* _Stop_at = _First;

        size_t _Sse_size     = _Byte_length(_First, _Last) & ~size_t{0xF};
        size_t _Portion_size = _Sse_size;

        if constexpr (_Traits::_Has_portion_max) {
            // vector of indices will wrap around at this size
            constexpr size_t _Max_portion_size = _Traits::_Portion_max * 16;
            if (_Portion_size > _Max_portion_size) {
                _Portion_size = _Max_portion_size;
            }
        }

        _Sse_size -= _Portion_size;

        _Advance_bytes(_Stop_at, _Portion_size);

        // Load values and if unsigned adjust them to be signed (for signed vector comparisons)
        __m128i _Cur_vals     = _Traits::_Sign_cor(_mm_loadu_si128(reinterpret_cast<const __m128i*>(_First)), _Sign);
        __m128i _Cur_vals_min = _Cur_vals; // vector of vertical minimum values
        __m128i _Cur_idx_min  = _mm_setzero_si128(); // vector of vertical minimum indices
        __m128i _Cur_vals_max = _Cur_vals; // vector of vertical maximum values
        __m128i _Cur_idx_max  = _mm_setzero_si128(); // vector of vertical maximum indices
        __m128i _Cur_idx      = _mm_setzero_si128(); // current vector of indices

        for (;;) {
            _Advance_bytes(_First, 16);

            // Increment vertical indices. Will stop at exactly wrap around, if not reach the end before
            _Cur_idx = _Traits::_Inc(_Cur_idx);

            if (_First == _Stop_at) {
                // Reached end or indices wrap around point. Compute horizontal min and/or max

                if constexpr (_Mode != _Min_max_mode::_Max_only) {
                    // Vector populated by the smallest element
                    const __m128i _H_min = _Traits::_H_min(_Cur_vals_min);
                    // Get any element of it
                    const auto _H_min_val = _Traits::_Get_any(_H_min);

                    if (_H_min_val < _Cur_min_val) {
                        // Current horizontal min is less than the old, update min
                        _Cur_min_val = _H_min_val;
                        // Mask of all elements equal to minimum
                        const __m128i _Eq_mask   = _Traits::_Cmp_eq(_H_min, _Cur_vals_min);
                        int _Mask                = _mm_movemask_epi8(_Eq_mask);
                        const __m128i _Minus_one = _mm_cmpeq_epi8(_mm_setzero_si128(), _mm_setzero_si128());
                        // Indices of minimum elements or the greatest index if none
                        const __m128i _Idx_min_val = _mm_blendv_epi8(_Minus_one, _Cur_idx_min, _Eq_mask);
                        // The smallest indexes
                        __m128i _Idx_min = _Traits::_H_min_u(_Idx_min_val);
                        // Select the smallest vertical indexes from the smallest element mask
                        _Mask &= _mm_movemask_epi8(_Traits::_Cmp_eq(_Idx_min, _Idx_min_val));
                        // Find the smallest horizontal index
                        unsigned long _H_pos;
                        _BitScanForward(&_H_pos, _Mask);
                        // Extract its vertical index
                        const auto _V_pos = _Traits::_Get_v_pos(_Cur_idx_min, _H_pos);
                        // Finally, compute the pointer
                        _Res._Min = _Base + (_V_pos * 16 + _H_pos);
                    }
                }

                if constexpr (_Mode != _Min_max_mode::_Min_only) {
                    // Vector populated by the largest element
                    const __m128i _H_max = _Traits::_H_max(_Cur_vals_max);
                    // Get any element of it
                    const auto _H_max_val = _Traits::_Get_any(_H_max);

                    if (_Mode == _Min_max_mode::_Both ? _Cur_max_val <= _H_max_val : _Cur_max_val < _H_max_val) {
                        // max_element: current horizontal max is greater than the old, update max
                        // minmax_element: current horizontal max is not less than the old, update max
                        _Cur_max_val = _H_max_val;
                        // Mask of all elements equal to maximum
                        const __m128i _Eq_mask = _Traits::_Cmp_eq(_H_max, _Cur_vals_max);
                        // Mask of all elements equal to maximum
                        int _Mask = _mm_movemask_epi8(_Eq_mask);

                        if constexpr (_Mode == _Min_max_mode::_Both) {
                            // Indices of minimum elements or zero none
                            const __m128i _Idx_max_val = _mm_blendv_epi8(_mm_setzero_si128(), _Cur_idx_max, _Eq_mask);
                            // The greatest indexes
                            const __m128i _Idx_max = _Traits::_H_max_u(_Idx_max_val);
                            // Select the greatest vertical indexes from the largest element mask
                            _Mask &= _mm_movemask_epi8(_Traits::_Cmp_eq(_Idx_max, _Idx_max_val));
                        } else {
                            const __m128i _Minus_one = _mm_cmpeq_epi8(_mm_setzero_si128(), _mm_setzero_si128());
                            // Indices of minimum elements or the greatest index if none
                            const __m128i _Idx_max_val = _mm_blendv_epi8(_Minus_one, _Cur_idx_max, _Eq_mask);
                            // The smallest indexes
                            const __m128i _Idx_max = _Traits::_H_min_u(_Idx_max_val);
                            // Select the smallest vertical indexes from the largest element mask
                            _Mask &= _mm_movemask_epi8(_Traits::_Cmp_eq(_Idx_max, _Idx_max_val));
                        }

                        unsigned long _H_pos;
                        if constexpr (_Mode == _Min_max_mode::_Both) {
                            // Find the largest horizontal index
                            _BitScanReverse(&_H_pos, _Mask);
                        } else {
                            // Find the smallest horizontal index
                            _BitScanForward(&_H_pos, _Mask);
                        }
                        // Extract its vertical index
                        const auto _V_pos = _Traits::_Get_v_pos(_Cur_idx_max, _H_pos);
                        // Finally, compute the pointer
                        _Res._Max = _Base + (_V_pos * 16 + _H_pos);
                    }
                }

                if constexpr (_Traits::_Has_portion_max) {
                    // Last portion or wrapping, need to determine

                    _Portion_size = _Sse_size;
                    if (_Portion_size == 0) {
                        break; // Last portion
                    }

                    // Handle the wrapping indexes. Assume _Cur_idx is zero

                    // This portion size
                    constexpr size_t _Max_portion_size = _Traits::_Portion_max * 16;
                    if (_Portion_size > _Max_portion_size) {
                        _Portion_size = _Max_portion_size;
                    }

                    _Advance_bytes(_Stop_at, _Portion_size);
                    // Size remaining after this
                    _Sse_size -= _Portion_size;

                    // Indices will be relative to the new base
                    _Base = static_cast<const char*>(_First);

                    // Load values and if unsigned adjust them to be signed (for signed vector comparisons)
                    _Cur_vals = _Traits::_Sign_cor(_mm_loadu_si128(reinterpret_cast<const __m128i*>(_First)), _Sign);

                    if constexpr (_Mode != _Min_max_mode::_Max_only) {
                        _Cur_vals_min = _Cur_vals;
                        _Cur_idx_min  = _mm_setzero_si128();
                    }
                    if constexpr (_Mode != _Min_max_mode::_Min_only) {
                        _Cur_vals_max = _Cur_vals;
                        _Cur_idx_max  = _mm_setzero_si128();
                    }

                    continue;
                } else {
                    break; // No wrapping, so it was the only portion
                }
            }
            // This is the main part, finding vertical minimum/maximum

            // Load values and if unsigned adjust them to be signed (for signed vector comparisons)
            __m128i _Cur_vals = _Traits::_Sign_cor(_mm_loadu_si128(reinterpret_cast<const __m128i*>(_First)), _Sign);

            if constexpr (_Mode != _Min_max_mode::_Max_only) {
                // Mask for the values less than the current minimum
                const __m128i _Is_less = _Traits::_Cmp_lt(_Cur_vals, _Cur_vals_min);
                // Remember their vertical indices
                _Cur_idx_min = _mm_blendv_epi8(_Cur_idx_min, _Cur_idx, _Is_less);
                // Update the current minimum
                _Cur_vals_min = _Traits::_Min(_Cur_vals_min, _Cur_vals, _Is_less);
            }

            if constexpr (_Mode == _Min_max_mode::_Max_only) {
                // Mask for the values greater or equal to the current maximum
                const __m128i _Is_greater = _Traits::_Cmp_gt(_Cur_vals, _Cur_vals_max);
                // Remember their vertical indices
                _Cur_idx_max = _mm_blendv_epi8(_Cur_idx_max, _Cur_idx, _Is_greater);
                // Update the current maximum
                _Cur_vals_max = _Traits::_Max(_Cur_vals_max, _Cur_vals, _Is_greater);
            } else if constexpr (_Mode == _Min_max_mode::_Both) {
                // *Inverse* mask for the values greater or equal to the current maximum
                const __m128i _Is_greater_or_equal_inv = _Traits::_Cmp_gt(_Cur_vals_max, _Cur_vals);
                // Remember their vertical indices
                _Cur_idx_max = _mm_blendv_epi8(_Cur_idx, _Cur_idx_max, _Is_greater_or_equal_inv);
                // Update the current maximum
                _Cur_vals_max = _Traits::_Max(_Cur_vals, _Cur_vals_max, _Is_greater_or_equal_inv);
            }
        }
    }

    return _Minmax_tail<_Mode, _Traits::_Signed_t, _Traits::_Unsigned_t>(
        _First, _Last, _Res, _Sign, _Cur_min_val, _Cur_max_val);
}

extern "C" {

const void* __stdcall __std_min_element_1(
    const void* const _First, const void* const _Last, const bool _Signed) noexcept {
    return _Minmax_element<_Min_max_mode::_Min_only, _Minmax_traits_1>(_First, _Last, _Signed);
}

const void* __stdcall __std_min_element_2(
    const void* const _First, const void* const _Last, const bool _Signed) noexcept {
    return _Minmax_element<_Min_max_mode::_Min_only, _Minmax_traits_2>(_First, _Last, _Signed);
}

const void* __stdcall __std_min_element_4(
    const void* const _First, const void* const _Last, const bool _Signed) noexcept {
    return _Minmax_element<_Min_max_mode::_Min_only, _Minmax_traits_4>(_First, _Last, _Signed);
}

const void* __stdcall __std_min_element_8(
    const void* const _First, const void* const _Last, const bool _Signed) noexcept {
    return _Minmax_element<_Min_max_mode::_Min_only, _Minmax_traits_8>(_First, _Last, _Signed);
}

const void* __stdcall __std_max_element_1(
    const void* const _First, const void* const _Last, const bool _Signed) noexcept {
    return _Minmax_element<_Min_max_mode::_Max_only, _Minmax_traits_1>(_First, _Last, _Signed);
}

const void* __stdcall __std_max_element_2(
    const void* const _First, const void* const _Last, const bool _Signed) noexcept {
    return _Minmax_element<_Min_max_mode::_Max_only, _Minmax_traits_2>(_First, _Last, _Signed);
}

const void* __stdcall __std_max_element_4(
    const void* const _First, const void* const _Last, const bool _Signed) noexcept {
    return _Minmax_element<_Min_max_mode::_Max_only, _Minmax_traits_4>(_First, _Last, _Signed);
}

const void* __stdcall __std_max_element_8(
    const void* const _First, const void* const _Last, const bool _Signed) noexcept {
    return _Minmax_element<_Min_max_mode::_Max_only, _Minmax_traits_8>(_First, _Last, _Signed);
}

_Min_max_t __stdcall __std_minmax_element_1(
    const void* const _First, const void* const _Last, const bool _Signed) noexcept {
    return _Minmax_element<_Min_max_mode::_Both, _Minmax_traits_1>(_First, _Last, _Signed);
}

_Min_max_t __stdcall __std_minmax_element_2(
    const void* const _First, const void* const _Last, const bool _Signed) noexcept {
    return _Minmax_element<_Min_max_mode::_Both, _Minmax_traits_2>(_First, _Last, _Signed);
}

_Min_max_t __stdcall __std_minmax_element_4(
    const void* const _First, const void* const _Last, const bool _Signed) noexcept {
    return _Minmax_element<_Min_max_mode::_Both, _Minmax_traits_4>(_First, _Last, _Signed);
}

_Min_max_t __stdcall __std_minmax_element_8(
    const void* const _First, const void* const _Last, const bool _Signed) noexcept {
    return _Minmax_element<_Min_max_mode::_Both, _Minmax_traits_8>(_First, _Last, _Signed);
}

} // extern "C"

#endif // defined(_M_IX86) || defined(_M_X64)
