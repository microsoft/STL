// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <cassert>
#include <compare>
#include <concepts>
#include <optional>
#include <ranges>
#include <vector>

using namespace std;

static_assert(three_way_comparable<nullopt_t, strong_ordering>);
static_assert(is_eq(nullopt <=> nullopt));
static_assert(nullopt == nullopt);
static_assert(!(nullopt != nullopt));
static_assert(!(nullopt < nullopt));
static_assert(nullopt <= nullopt);
static_assert(!(nullopt > nullopt));
static_assert(nullopt >= nullopt);

void test_ranges_find() {
    vector<optional<int>> v = {1, nullopt, 3};
    auto it                 = ranges::find(v, nullopt);
    assert(it != v.end());
    assert(!it->has_value());
}

int main() {
    test_ranges_find();
}
