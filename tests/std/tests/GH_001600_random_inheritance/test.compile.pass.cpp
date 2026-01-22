// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <cstddef>
#include <random>
#include <type_traits>
#include <vector>

#define STATIC_ASSERT(...) static_assert(__VA_ARGS__, #__VA_ARGS__)

using namespace std;

// GH-1600 "<random>: piecewise_linear_distribution<T>::param_type should not inherit from
// discrete_distribution<size_t>::param_type"
// Note that inheritance between distribution types should also be removed.
template <class F>
void test_gh_1600_inheritance() {
    STATIC_ASSERT(!is_base_of_v<discrete_distribution<size_t>, piecewise_constant_distribution<F>>);
    STATIC_ASSERT(!is_base_of_v<discrete_distribution<size_t>::param_type,
        typename piecewise_constant_distribution<F>::param_type>);

    STATIC_ASSERT(!is_base_of_v<discrete_distribution<size_t>, piecewise_linear_distribution<F>>);
    STATIC_ASSERT(!is_base_of_v<discrete_distribution<size_t>::param_type,
        typename piecewise_linear_distribution<F>::param_type>);
}

// Until 2026-01, piecewise_{constant,linear}_distribution<FP> were both derived from discrete_distribution<size_t>.
//   Same for their param_type.
// Now the discrete_distribution<size_t> objects are changed from base class subobjects to leading member subobjects.
//   Same for corresponding param_type subobjects.
// In vNext, we should probably remove unused members, so the sizes will be reduced.
template <class F>
void test_gh_1600_abi() {
    // piecewise_{constant,linear}_distribution and their param_type don't introduce padding bytes under MSVC ABI.
    // So it's OK to just add the sizes.

    // The sizes will be probably reduced in vNext.

    STATIC_ASSERT(
        sizeof(piecewise_constant_distribution<F>)
        == sizeof(discrete_distribution<size_t>) + sizeof(typename piecewise_constant_distribution<F>::param_type));
    STATIC_ASSERT(sizeof(typename piecewise_constant_distribution<F>::param_type)
                  == sizeof(discrete_distribution<size_t>::param_type) + sizeof(vector<F>));

    STATIC_ASSERT(
        sizeof(piecewise_linear_distribution<F>)
        == sizeof(discrete_distribution<size_t>) + sizeof(typename piecewise_linear_distribution<F>::param_type));
    STATIC_ASSERT(sizeof(typename piecewise_linear_distribution<F>::param_type)
                  == sizeof(discrete_distribution<size_t>::param_type) + sizeof(vector<F>));

    // The alignments are likely to be unchanged in vNext.

    STATIC_ASSERT(alignof(piecewise_constant_distribution<F>) == alignof(void*));
    STATIC_ASSERT(alignof(typename piecewise_constant_distribution<F>::param_type) == alignof(void*));

    STATIC_ASSERT(alignof(piecewise_linear_distribution<F>) == alignof(void*));
    STATIC_ASSERT(alignof(typename piecewise_linear_distribution<F>::param_type) == alignof(void*));
}

void test() {
    test_gh_1600_inheritance<float>();
    test_gh_1600_inheritance<double>();
    test_gh_1600_inheritance<long double>();

    test_gh_1600_abi<float>();
    test_gh_1600_abi<double>();
    test_gh_1600_abi<long double>();
}
