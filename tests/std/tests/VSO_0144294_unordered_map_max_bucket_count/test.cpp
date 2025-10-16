// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <cassert>
#include <unordered_map>
#include <unordered_set>

template <typename T>
void check_max_buckets_is_reasonable(const T& val) {
    size_t max_buckets = val.max_bucket_count();
    assert(max_buckets > 1000000);
}

int main() {
    std::unordered_map<int, int> umap_test;
    std::unordered_set<int> uset_test;

    check_max_buckets_is_reasonable(umap_test);
    check_max_buckets_is_reasonable(uset_test);
}
