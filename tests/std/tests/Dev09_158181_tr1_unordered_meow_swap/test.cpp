// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <cassert>
#include <cmath>
#include <cstdlib>
#include <limits>
#include <stdexcept>
#include <unordered_set>
#include <utility>

using namespace std;

constexpr auto size_max = numeric_limits<size_t>::max();

#define STATIC_ASSERT(...) static_assert(__VA_ARGS__, #__VA_ARGS__)

void assert_is_pow2(const size_t value) {
    assert(value != 0 && ((value & (value - 1)) == 0));
}

void assert_close(const float f, const float g) {
    assert(abs(f - g) < 0.01f);
}

template <typename X, typename Fx>
void assert_throws(Fx fn) noexcept {
    try {
        fn();
        abort();
    } catch (const X&) {
    }
}

// Test DDB-158181. unordered_set.swap() was O(N), throwing, and invalidating iterators, which was bad.
void test_ddb_158181() {
    unordered_set<int> x;
    x.insert(11);
    x.insert(21);
    x.insert(31);

    unordered_set<int> y;
    y.insert(42);
    y.insert(52);
    y.insert(62);

    const auto a = x.cbegin();
    const auto b = y.cbegin();

    const int a1 = *a;
    const int b1 = *b;

    x.swap(y);

    STATIC_ASSERT(noexcept(x.swap(y)));

    const int a2 = *a;
    const int b2 = *b;

    assert(a1 == a2 && b1 == b2);
}

// Also test swapping/assigning max_load_factor(), broken in VS 2015 RTM.
void test_max_load_factor() {
    unordered_set<int> x{10, 11, 12, 13, 14};
    unordered_set<int> y{20, 21, 22, 23, 24};
    unordered_set<int> z{30, 31, 32, 33, 34};

    x.max_load_factor(1.1f);
    y.max_load_factor(2.2f);
    z.max_load_factor(3.3f);

    assert_close(x.max_load_factor(), 1.1f);
    assert_close(y.max_load_factor(), 2.2f);
    assert_close(z.max_load_factor(), 3.3f);

    x.swap(y);

    assert_close(x.max_load_factor(), 2.2f);
    assert_close(y.max_load_factor(), 1.1f);

    x = y;

    assert_close(x.max_load_factor(), 1.1f);
    assert_close(y.max_load_factor(), 1.1f);

    x = move(z);

    assert_close(x.max_load_factor(), 3.3f);
}

void test_floor_log2() {
    assert(_Floor_of_log_2(0) == 0);
    size_t expected = 0;
    for (size_t test = 2; test != 0; test <<= 1) {
        assert(_Floor_of_log_2(test - 1) == expected);
        ++expected;
        assert(_Floor_of_log_2(test) == expected);
        assert(_Floor_of_log_2(test + 1) == expected);
    }
}

void test_LWG_2156() {
    unordered_set<int> x;

    x.max_load_factor(5.0f);
    for (int i = 0; i < 100; ++i) {
        x.emplace(i);
        assert_is_pow2(x.bucket_count()); // MSVC-specific invariant
    }

    const auto afterEmplaceBuckets = x.bucket_count();
    x.max_load_factor(1.0f); // break usual container attempt to maintain bucket_count() >= size() / max_load_factor()
    assert(!(x.load_factor() <= x.max_load_factor())); // assert invariant broken
    assert(afterEmplaceBuckets == x.bucket_count()); // max_load_factor must not rehash the container
    x.rehash(0); // container will have to choose more buckets to restore bucket_count() >= size() / max_load_factor()
    const auto afterRehashBuckets = x.bucket_count();
    assert_is_pow2(afterRehashBuckets);
    assert(afterRehashBuckets >= x.size()); // check postcondition directly
    assert(x.load_factor() <= x.max_load_factor());
    x.erase(0); // keeping the same number of elements must not cause a rehash
    x.emplace(0);
    assert(x.bucket_count() == afterRehashBuckets);

    assert_throws<length_error>([&] { x.rehash(size_max); });
    assert_throws<length_error>([&] { x.rehash(size_max / 2); });

#ifndef _WIN64
    // make it so rehash can't meet its postcondition, even when not asking for more buckets
    // skip on 64 bit because creating reasonable container sizes to trigger this problem there
    // consumes too much RAM.
    x.max_load_factor(FLT_EPSILON);
    assert_throws<length_error>([&] { x.rehash(0); });
#endif // !_WIN64
}

void test_bad_rehash_size() {
    unordered_set<size_t> x;
    x.max_load_factor(1.0f);
    const auto initialBuckets = x.bucket_count();
    for (size_t i = 0; i < initialBuckets; ++i) {
        x.emplace(i);
    }

    assert(x.bucket_count() == initialBuckets); // the above shouldn't have rehashed
    assert(x.load_factor() <= x.max_load_factor());
    x.max_load_factor(1.0f / 64.0f); // force 64 buckets per element, so that multiply by 8 on emplace insufficient
    x.emplace(initialBuckets); // trigger rehash
    assert(x.bucket_count() > initialBuckets);
    assert(x.load_factor() <= x.max_load_factor());
}

int main() {
    test_ddb_158181();
    test_max_load_factor();
    test_floor_log2();
    test_LWG_2156();
    test_bad_rehash_size();
}
