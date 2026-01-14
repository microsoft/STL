// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <cassert>
#include <optional>

struct LargePayload {
    int data[4];
};

void test_activation_unpoisoning() {
    std::optional<LargePayload> opt;
    opt.emplace(LargePayload{1, 2, 3, 4});
    assert(opt->data[0] == 1);
}

void test_assignment_unpoisoning() {
    std::optional<LargePayload> opt;
    LargePayload val{5, 6, 7, 8};
    opt = val;
    assert(opt->data[0] == 5);
}

void test_poison_on_empty_access() {
    std::optional<LargePayload> opt;
    volatile int x = opt->data[0];
    (void) x;
}

void test_repoison_after_reset() {
    std::optional<LargePayload> opt;
    opt.emplace(LargePayload{1, 1, 1, 1});
    opt.reset();

    volatile int x = opt->data[0];
    (void) x;
}

int main() {
    test_activation_unpoisoning();
    test_assignment_unpoisoning();
    test_poison_on_empty_access();
    test_repoison_after_reset();

    return 0;
}
