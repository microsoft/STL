// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <functional>
#include <type_traits>

using namespace std;

void test_LWG_3146() {
    int i = 0;
    reference_wrapper<int> ri(i);
    reference_wrapper<reference_wrapper<int>> rri(ri);

    auto reference       = ref(rri);
    auto const_reference = cref(rri);

    static_assert(
        is_same_v<decltype(reference), reference_wrapper<reference_wrapper<int>>>, "LWG-3146 is not implemented");
    static_assert(is_same_v<decltype(const_reference), reference_wrapper<const reference_wrapper<int>>>,
        "LWG-3146 is not implemented");
}
