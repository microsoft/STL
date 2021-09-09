// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <memory>
#include <utility>

using namespace std;

struct MoveOnlyType {
    MoveOnlyType()               = default;
    MoveOnlyType(MoveOnlyType&&) = default;
};

void test() {
    std::allocator<MoveOnlyType> alloc;
    [[maybe_unused]] auto p = pair<MoveOnlyType&&, MoveOnlyType&&>{MoveOnlyType{}, MoveOnlyType{}};
    [[maybe_unused]] auto t = uses_allocator_construction_args<pair<MoveOnlyType&&, MoveOnlyType&&>>(alloc, move(p));
}

int main() {} // COMPILE-ONLY
