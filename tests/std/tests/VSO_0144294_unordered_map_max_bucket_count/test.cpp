// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#define _SILENCE_STDEXT_HASH_DEPRECATION_WARNINGS
#include <cassert>
#include <hash_map>
#include <hash_set>
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
    stdext::hash_map<int, int> hashmap_test;
    stdext::hash_set<int> hashset_test;

    check_max_buckets_is_reasonable(umap_test);
    check_max_buckets_is_reasonable(uset_test);
    check_max_buckets_is_reasonable(hashmap_test);
    check_max_buckets_is_reasonable(hashset_test);
}
