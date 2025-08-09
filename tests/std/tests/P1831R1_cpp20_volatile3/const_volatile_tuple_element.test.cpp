// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <tuple>
#include <type_traits>

int main() {
    static_assert(std::is_same_v<std::tuple_element_t<0, const volatile std::tuple<int>>, const volatile int>);
}
