// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <algorithm>
#include <cstdint>
#include <iterator>

using namespace std;

#define STATIC_ASSERT(...) static_assert(__VA_ARGS__, #__VA_ARGS__)

#if _HAS_CXX20

struct ThreeBytes {
    int8_t i;
    int8_t j;
    int8_t k;
    bool operator==(const ThreeBytes&) const noexcept = default;
};

struct FourBytes {
    int32_t i;
    bool operator==(const FourBytes&) const noexcept = default;
};

struct PaddedToEight {
    int32_t i;
    int16_t j;
    bool operator==(const PaddedToEight&) const noexcept = default;
};

struct EightBytes {
    int32_t i;
    int32_t j;
    bool operator==(const EightBytes&) const noexcept = default;
};

struct SixteenBytes {
    int64_t i;
    int64_t j;
    bool operator==(const SixteenBytes&) const noexcept = default;
};

struct NonTrivial {
    int32_t i;
    bool operator==(const NonTrivial& other) const noexcept {
        return i == other.i;
    }
};

STATIC_ASSERT(sizeof(ThreeBytes) == 3);
STATIC_ASSERT(sizeof(FourBytes) == 4);
STATIC_ASSERT(sizeof(PaddedToEight) == 8);
STATIC_ASSERT(sizeof(EightBytes) == 8);
STATIC_ASSERT(sizeof(SixteenBytes) == 16);
STATIC_ASSERT(sizeof(NonTrivial) == 4);

#ifdef __clang__
STATIC_ASSERT(__is_trivially_equality_comparable(ThreeBytes));
STATIC_ASSERT(__is_trivially_equality_comparable(FourBytes));
STATIC_ASSERT(!__is_trivially_equality_comparable(PaddedToEight)); // padding is not allowed
STATIC_ASSERT(__is_trivially_equality_comparable(EightBytes));
STATIC_ASSERT(__is_trivially_equality_comparable(SixteenBytes));
STATIC_ASSERT(!__is_trivially_equality_comparable(NonTrivial)); // exactly what it says on the tin
constexpr bool vectorized_trivial_comparison = true;
#else // ^^^ defined(__clang__) / !defined(__clang__) vvv
constexpr bool vectorized_trivial_comparison = false;
#endif // ^^^ !defined(__clang__) ^^^

#endif // _HAS_CXX20

STATIC_ASSERT(_Vector_alg_in_find_is_safe<int8_t*, int8_t>);
STATIC_ASSERT(_Vector_alg_in_find_is_safe<int16_t*, int16_t>);
STATIC_ASSERT(_Vector_alg_in_find_is_safe<int32_t*, int32_t>);
STATIC_ASSERT(_Vector_alg_in_find_is_safe<int64_t*, int64_t>);

STATIC_ASSERT(_Vector_alg_in_find_is_safe<const int*, int>); // Allow const
STATIC_ASSERT(!_Vector_alg_in_find_is_safe<float*, float>); // Don't allow floating-point
STATIC_ASSERT(!_Vector_alg_in_find_is_safe<volatile int*, int>); // Don't allow volatile

#if _HAS_CXX20
STATIC_ASSERT(!_Vector_alg_in_find_is_safe<ThreeBytes*, ThreeBytes>); // not a power of two
STATIC_ASSERT(_Vector_alg_in_find_is_safe<FourBytes*, FourBytes> == vectorized_trivial_comparison);
STATIC_ASSERT(!_Vector_alg_in_find_is_safe<PaddedToEight*, PaddedToEight>); // padding is not allowed
STATIC_ASSERT(_Vector_alg_in_find_is_safe<EightBytes*, EightBytes> == vectorized_trivial_comparison);
STATIC_ASSERT(!_Vector_alg_in_find_is_safe<SixteenBytes*, SixteenBytes>); // too large
STATIC_ASSERT(!_Vector_alg_in_find_is_safe<NonTrivial*, NonTrivial>); // exactly what it says on the tin

template <class T>
void test_find() {
    T arr[3]{};
    (void) find(begin(arr), end(arr), T{});
    (void) ranges::find(arr, T{});
}

void test() {
    test_find<ThreeBytes>();
    test_find<FourBytes>();
    test_find<PaddedToEight>();
    test_find<EightBytes>();
    test_find<SixteenBytes>();
    test_find<NonTrivial>();
}
#endif // _HAS_CXX20
