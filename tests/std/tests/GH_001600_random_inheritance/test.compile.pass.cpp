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
    using Con = piecewise_constant_distribution<F>;
    using Lin = piecewise_linear_distribution<F>;
    using Dis = discrete_distribution<size_t>;

    using ConParam = typename Con::param_type;
    using LinParam = typename Lin::param_type;
    using DisParam = Dis::param_type;

    STATIC_ASSERT(!is_base_of_v<Dis, Con>);
    STATIC_ASSERT(!is_base_of_v<DisParam, ConParam>);

    STATIC_ASSERT(!is_base_of_v<Dis, Lin>);
    STATIC_ASSERT(!is_base_of_v<DisParam, LinParam>);
}

// Until 2026-01, piecewise_constant_distribution<FP> and piecewise_linear_distribution<FP>
//   were both derived from discrete_distribution<size_t>. Same for their param_type structs.
// Now the discrete_distribution<size_t> objects are changed from base class subobjects to leading member subobjects.
//   Same for piecewise_constant_distribution::param_type.
//   For piecewise_linear_distribution::param_type, the original discrete_distribution<size_t>::param_type subobject is
//   decomposed into vectors to avoid imposing invariants.
// In vNext, we should probably remove unused members, so the sizes will be reduced.
template <class F>
void test_gh_1600_abi() {
    // piecewise_constant_distribution, piecewise_linear_distribution, and their param_type structs
    // don't introduce padding bytes under MSVC ABI. So it's OK to just add the sizes.

    // The sizes will be probably reduced in vNext.

    using Con = piecewise_constant_distribution<F>;
    using Lin = piecewise_linear_distribution<F>;
    using Dis = discrete_distribution<size_t>;

    using ConParam = typename Con::param_type;
    using LinParam = typename Lin::param_type;
    using DisParam = Dis::param_type;

    STATIC_ASSERT(sizeof(Con) == sizeof(Dis) + sizeof(ConParam));
    STATIC_ASSERT(sizeof(ConParam) == sizeof(DisParam) + sizeof(vector<F>));

    STATIC_ASSERT(sizeof(Lin) == sizeof(Dis) + sizeof(LinParam));
    STATIC_ASSERT(sizeof(LinParam) == sizeof(DisParam) + sizeof(vector<F>));

    // The alignments are likely to be unchanged in vNext.

    STATIC_ASSERT(alignof(Con) == alignof(void*));
    STATIC_ASSERT(alignof(ConParam) == alignof(void*));

    STATIC_ASSERT(alignof(Lin) == alignof(void*));
    STATIC_ASSERT(alignof(LinParam) == alignof(void*));
}

void test() {
    test_gh_1600_inheritance<float>();
    test_gh_1600_inheritance<double>();
    test_gh_1600_inheritance<long double>();

    test_gh_1600_abi<float>();
    test_gh_1600_abi<double>();
    test_gh_1600_abi<long double>();
}
