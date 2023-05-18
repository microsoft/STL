// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <cassert>
#include <unordered_set>

using namespace std;

int main() {
    unordered_set<int> a;
    a.max_load_factor(24.0f / 64.25f);
    a.reserve(24);
    assert(a.bucket_count() > 64);
    return 0;
}
