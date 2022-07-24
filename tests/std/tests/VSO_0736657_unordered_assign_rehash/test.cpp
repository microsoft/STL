// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <cassert>
#include <unordered_set>

using namespace std;

void assert_set_is_iota(const unordered_set<int>& theSet, const int maxValue) {
    assert(theSet.size() == static_cast<size_t>(maxValue));
    for (int value = 0; value < maxValue; ++value) {
        assert(theSet.count(value) == 1);
    }
}

int main() {
    unordered_set<int> source;
    const auto initialBuckets = source.bucket_count();
    // insert enough elements to force a rehash
    int maxValue = 0;
    for (; source.bucket_count() == initialBuckets; ++maxValue) {
        source.emplace(maxValue);
    }

    unordered_set<int> target;
    assert(target.bucket_count() < source.bucket_count());
    target = source; // Test that this assignment actually rehashes
    // The following is not technically guaranteed but is true of our implementation today:
    assert(target.bucket_count() == source.bucket_count());

    assert_set_is_iota(source, maxValue);
    assert_set_is_iota(target, maxValue);
}
