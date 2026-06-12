// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <algorithm>
#include <cstdint>

using namespace std;

#define STATIC_ASSERT(...) static_assert(__VA_ARGS__, #__VA_ARGS__)

#if _HAS_CXX20

#ifdef __clang__
constexpr bool vectorized_trivial_comparison = true;
#else
constexpr bool vectorized_trivial_comparison = false;
#endif

struct DefaultComparison {
    int32_t i;

    bool operator==(const DefaultComparison&) const noexcept = default;
};

struct DefaultComparisonOddSize {
    int32_t i;
    int16_t j;

    bool operator==(const DefaultComparisonOddSize&) const noexcept = default;
};

struct DefaultComparisonLargeSize {
    int32_t i;
    int32_t j;

    bool operator==(const DefaultComparisonLargeSize&) const noexcept = default;
};

struct DefaultComparisonTooLargeSize {
    int64_t i;
    int64_t j;

    bool operator==(const DefaultComparisonTooLargeSize&) const noexcept = default;
};

#endif // _HAS_CXX20

STATIC_ASSERT(
    _Vector_alg_in_find_is_safe<int*, int>); // ints are trivially equality comparable and 4 bytes, so should be allowed
STATIC_ASSERT(_Vector_alg_in_find_is_safe<long*,
    long>); // long is an integral type with a supported element size (4 or 8 bytes), so should be allowed

STATIC_ASSERT(_Vector_alg_in_find_is_safe<int const*, int>); // "Top level const should not change the answer"
STATIC_ASSERT(!_Vector_alg_in_find_is_safe<float*, float>); // "Don't allow floating point"
STATIC_ASSERT(!_Vector_alg_in_find_is_safe<int volatile*, int>); // "Don't allow volatile"

#if _HAS_CXX20
STATIC_ASSERT(_Vector_alg_in_find_is_safe<DefaultComparison*, DefaultComparison> == vectorized_trivial_comparison);
STATIC_ASSERT(_Vector_alg_in_find_is_safe<DefaultComparisonLargeSize*, DefaultComparisonLargeSize>
              == vectorized_trivial_comparison);
STATIC_ASSERT(!_Vector_alg_in_find_is_safe<DefaultComparisonOddSize*, DefaultComparisonOddSize>);
STATIC_ASSERT(!_Vector_alg_in_find_is_safe<DefaultComparisonTooLargeSize*, DefaultComparisonTooLargeSize>);
#endif // _HAS_CXX20
