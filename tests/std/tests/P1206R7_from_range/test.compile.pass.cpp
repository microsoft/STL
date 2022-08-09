// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <ranges>
#include <type_traits>

using namespace std;

int main() {} // COMPILE-ONLY

static_assert(same_as<decltype(from_range), const from_range_t>);
static_assert(semiregular<from_range_t>);

void helper(auto);

template <class T>
concept can_implicitly_construct = requires {
    helper<T>({});
};

static_assert(!can_implicitly_construct<from_range_t>);
