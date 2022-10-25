// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <cassert>
#include <memory>
#include <unordered_set>

using namespace std;

// This test tests for a condition where, during a rehash, the container would invalidate a reference to the key to
// insert.

struct hasher {
    size_t operator()(const unique_ptr<int>&) const noexcept {
        return 0;
    }
};

struct eq {
    bool operator()(const unique_ptr<int>& p, const unique_ptr<int>& q) const noexcept {
        if (p == nullptr) {
            return q == nullptr;
        }

        if (q == nullptr) {
            return false;
        }
        return *p == *q;
    }
};

int main() {
    int i = 0;
    unordered_set<unique_ptr<int>, hasher, eq> uut;
    uut.max_load_factor(0.5f);
    const auto originalBucketCount = uut.bucket_count();
    // insert nullptr because that's the moved from state; the bug made this element get returned:
    uut.insert(unique_ptr<int>{});
    // insert enough elements to be on the edge of a rehash:
    while ((static_cast<float>(uut.size() + 1) / static_cast<float>(uut.bucket_count())) <= 0.5f) {
        uut.insert(make_unique<int>(i++));
    }

    assert(originalBucketCount == uut.bucket_count());

    // try to insert the next element and make sure what we get back is actually that element:
    const auto inserted = uut.insert(make_unique<int>(i));
    assert(inserted.second);
    assert(**inserted.first == i);

    // make sure the rehash actually happened:
    assert(originalBucketCount != uut.bucket_count());
}
