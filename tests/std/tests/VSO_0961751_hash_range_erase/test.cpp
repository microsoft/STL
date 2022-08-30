// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#define _STL_INTERNAL_CHECK_EXHAUSTIVE
#include <cassert>
#include <cstdlib>
#include <functional>
#include <iterator>
#include <limits>
#include <memory>
#include <stdexcept>
#include <type_traits>
#include <unordered_set>
#include <utility>
#include <vector>

#include <test_death.hpp>

using namespace std;

constexpr auto size_max = (numeric_limits<size_t>::max)();

static size_t liveness_alive_objects;

template <class T>
struct liveness_allocator {
    using value_type = T;

    liveness_allocator()                                     = default;
    liveness_allocator(const liveness_allocator&)            = default;
    liveness_allocator& operator=(const liveness_allocator&) = default;
    template <class U, enable_if_t<!is_same_v<T, U>, int> = 0>
    liveness_allocator(const liveness_allocator<U>&) {}

    T* allocate(const size_t n) {
        liveness_alive_objects += n;
        return allocator<T>{}.allocate(n);
    }

    void deallocate(T* const p, const size_t n) {
        liveness_alive_objects -= n;
        allocator<T>{}.deallocate(p, n);
    }

    template <class B>
    friend bool operator==(const liveness_allocator&, const liveness_allocator<B>&) noexcept {
        return true;
    }
    template <class B>
    friend bool operator!=(const liveness_allocator&, const liveness_allocator<B>&) noexcept {
        return false;
    }
};

struct erase_fixture {
    struct liveness_empty_asserter {
        ~liveness_empty_asserter() {
            assert(liveness_alive_objects == 0);
        }
    } instance_empty; // destroyed after theHash

    using container = unordered_set<int, hash<int>, equal_to<>, liveness_allocator<int>>;
    using iterator  = container::iterator;
    container theHash;

    struct post_asserter {
        container& theHash;

        ~post_asserter() {
            // tests that erase actually deallocated
            assert(liveness_alive_objects
                   == (_ITERATOR_DEBUG_LEVEL != 0) * 2 + 1 + theHash.size() + theHash.bucket_count() * 2);
            theHash._Stl_internal_check_container_invariants();
        }
    } instance_nonempty; // destroyed before theHash

    void assert_iterators_consistent(const vector<iterator>& iterators) {
        assert(theHash.size() == iterators.size() - 1);
        auto i = theHash.begin();
        assert(i == iterators[0]);
        for (size_t idx = 1; idx < iterators.size(); ++idx) {
            ++i;
            assert(i == iterators[idx]);
        }
    }

    vector<iterator> get_iterators() {
        vector<iterator> iterators;
        auto i = theHash.cbegin();
        for (size_t idx = 0; idx < theHash.size(); ++idx) {
            iterators.push_back(i);
            ++i;
        }

        iterators.push_back(i);
        return iterators;
    }

    erase_fixture() : instance_empty(), theHash{0, 1, 2, 3, 4, 5, 6, 7, 8, 9}, instance_nonempty{theHash} {}

    void test_case_erase_one() {
        auto iterators = get_iterators();
        theHash.erase(next(theHash.begin(), static_cast<ptrdiff_t>(theHash.size() - 1)));
        iterators[iterators.size() - 2] = iterators[iterators.size() - 1];
        iterators.pop_back();
        assert_iterators_consistent(iterators);
    }

    void test_case_range_erase_nothing() {
        auto iterators = get_iterators();
        theHash.erase(theHash.begin(), theHash.begin());
        assert_iterators_consistent(iterators);
    }

    void test_case_range_erase_one() {
        auto iterators = get_iterators();
        theHash.erase(next(theHash.begin(), static_cast<ptrdiff_t>(theHash.size() - 1)), theHash.end());
        iterators.erase(iterators.end() - 2);
        assert_iterators_consistent(iterators);
    }

    void test_case_range_erase_some() {
        auto iterators = get_iterators();
        theHash.erase(next(theHash.begin()), next(theHash.begin(), static_cast<ptrdiff_t>(theHash.size() - 1)));
        iterators.erase(iterators.begin() + 1, iterators.begin() + static_cast<ptrdiff_t>(iterators.size() - 2));
        assert_iterators_consistent(iterators);
    }

    void test_case_range_erase_all() {
        auto iterators = get_iterators();
        theHash.erase(theHash.begin(), theHash.end());
        iterators.erase(iterators.begin(), iterators.end() - 1);
        assert_iterators_consistent(iterators);
    }
};

void test_case_erase_one_bad() {
    erase_fixture f;
    auto i = f.theHash.begin();
    f.theHash.erase(i);
    (void) *i;
}

void test_case_range_erase_one_bad() {
    erase_fixture f;
    auto i = f.theHash.begin();
    f.theHash.erase(i, next(i));
    (void) *i;
}

void test_case_range_erase_some_bad_first() {
    erase_fixture f;
    auto first = f.theHash.begin();
    ++first;
    f.theHash.erase(first, f.theHash.end());
    (void) *first;
}

void test_case_range_erase_some_bad_middle() {
    erase_fixture f;
    auto middle = f.theHash.begin();
    ++middle;
    ++middle;
    f.theHash.erase(next(f.theHash.begin()), next(f.theHash.begin(), static_cast<ptrdiff_t>(f.theHash.size() - 1)));
    (void) *middle;
}

void test_case_range_erase_some_bad_last() {
    erase_fixture f;
    auto last = next(f.theHash.end(), static_cast<ptrdiff_t>(f.theHash.size() - 2));
    f.theHash.erase(next(f.theHash.begin()), next(last));
    (void) *last;
}

void test_case_range_erase_all_bad_first() {
    erase_fixture f;
    auto first = f.theHash.begin();
    f.theHash.erase(f.theHash.begin(), f.theHash.end());
    (void) *first;
}

void test_case_range_erase_all_bad_middle() {
    erase_fixture f;
    auto middle = f.theHash.begin();
    ++middle;
    ++middle;
    f.theHash.erase(f.theHash.begin(), f.theHash.end());
    (void) *middle;
}

void test_case_range_erase_all_bad_last() {
    erase_fixture f;
    auto last = next(f.theHash.end(), static_cast<ptrdiff_t>(f.theHash.size() - 1));
    f.theHash.erase(f.theHash.begin(), f.theHash.end());
    (void) *last;
}

struct elem {
    size_t memoized_hash;
    size_t key;

    friend bool operator==(const elem& lhs, const elem& rhs) noexcept {
        return lhs.key == rhs.key;
    }
};

static size_t allowed_hash_calls;
struct throwing_hash {
    size_t operator()(const elem& e) const {
        if (allowed_hash_calls == 0) {
            throw runtime_error("example exception");
        }

        --allowed_hash_calls;
        return e.memoized_hash;
    }
};

void fill_throwing_elems(unordered_set<elem, throwing_hash>& theHash) {
    const size_t oldAllowed = exchange(allowed_hash_calls, size_max);
    allowed_hash_calls      = size_max;
    theHash.max_load_factor(2.0f);
    for (size_t idx = 0; idx < 50; ++idx) {
        theHash.insert({0, idx});
    }
    for (size_t idx = 50; idx < 100; ++idx) {
        theHash.insert({size_max & (~size_t{15}), idx});
    }
    for (size_t idx = 100; idx < 150; ++idx) {
        theHash.insert({size_max, idx});
    }
    // this loop will terminate because the max load factor is 2
    const auto buckets = theHash.bucket_count();
    for (size_t idx = 150; idx < buckets; ++idx) {
        theHash.insert({idx, idx});
    }
    allowed_hash_calls = oldAllowed;
}

void test_case_throwing_hash_not_called_in_clear() {
    unordered_set<elem, throwing_hash> theHash;
    fill_throwing_elems(theHash);
    allowed_hash_calls = 0; // ban calling the hash function
    theHash.clear(); // should choose the 'bulk' strategy always
    theHash.clear(); // should choose the 'no-op' strategy always
    allowed_hash_calls = size_max;
    theHash.insert({0, 0});
    allowed_hash_calls = 0;
    theHash.clear(); // would want to choose the 'elementwise' strategy, but can't do so because the hash is throwing
}

void test_case_consistent_after_throwing_hash() {
    unordered_set<elem, throwing_hash> theHash;
    fill_throwing_elems(theHash);
    allowed_hash_calls = size_max;
    size_t total_hash_calls;
    { // determine how many hash calls are necessary
        auto theCopy       = theHash;
        allowed_hash_calls = size_max;
        theCopy.erase(theCopy.begin(), theCopy.end());
        total_hash_calls = size_max - allowed_hash_calls;
    }

    // throw for each number of calls less than that and make sure the container is okay
    for (size_t callsNow = 0; callsNow < total_hash_calls; ++callsNow) {
        allowed_hash_calls = size_max;
        auto theCopy       = theHash;
        try {
            allowed_hash_calls = callsNow;
            theCopy.erase(theCopy.begin(), theCopy.end());
            abort();
        } catch (const runtime_error&) {
            allowed_hash_calls = size_max;
            theCopy._Stl_internal_check_container_invariants();
        }
    }

    allowed_hash_calls = total_hash_calls;
    theHash.erase(theHash.begin(), theHash.end());
}

namespace std {
    template <>
    struct hash<elem> {
        size_t operator()(const elem& e) const noexcept {
            return e.memoized_hash;
        }
    };
} // namespace std

struct erase_bucket_consistency_fixture {
    struct liveness_empty_asserter {
        ~liveness_empty_asserter() {
            assert(liveness_alive_objects == 0);
        }
    } instance_empty; // destroyed after theHash

    using container = unordered_set<elem, hash<elem>, equal_to<>, liveness_allocator<elem>>;
    container theHash;

    struct post_asserter {
        container& theHash;

        ~post_asserter() {
            // tests that erase actually deallocated
            assert(liveness_alive_objects
                   == (_ITERATOR_DEBUG_LEVEL != 0) * 2 + 1 + theHash.size() + theHash.bucket_count() * 2);
            assert(distance(theHash.begin(), theHash.end()) == static_cast<ptrdiff_t>(theHash.size()));
            theHash._Stl_internal_check_container_invariants();
        }
    } instance_nonempty; // destroyed before theHash

    size_t zeroBucket;
    size_t midBucket;
    size_t maxBucket;

    erase_bucket_consistency_fixture() : instance_empty(), theHash(), instance_nonempty{theHash} {
        // create 3 buckets of 5 elements each; assuming that these are different buckets is a nonstandard assumption
        theHash.reserve(30);
        for (size_t idx = 0; idx < 5; ++idx) {
            theHash.insert({0, idx});
        }
        for (size_t idx = 5; idx < 10; ++idx) {
            theHash.insert({size_max & (~size_t{15}), idx});
        }
        for (size_t idx = 10; idx < 15; ++idx) {
            theHash.insert({size_max, idx});
        }

        zeroBucket = theHash.bucket({0, 0});
        midBucket  = theHash.bucket({size_max & (~size_t{15}), 10});
        maxBucket  = theHash.bucket({size_max, 20});
    }

    void assert_equal(size_t expected, size_t actual) {
        assert(expected == actual);
    }

    void assert_buckets(size_t zero, size_t mid, size_t maxB) {
        assert_equal(zero, theHash.bucket_size(zeroBucket));
        assert_equal(mid, theHash.bucket_size(midBucket));
        assert_equal(maxB, theHash.bucket_size(maxBucket));
    }

    void test_case_intra_first_bucket() {
        theHash.erase(next(theHash.begin()), next(theHash.begin(), 4));
        assert_buckets(2, 5, 5);
    }

    void test_case_prefix_first_bucket() {
        theHash.erase(theHash.begin(), next(theHash.begin(), 4));
        assert_buckets(1, 5, 5);
    }

    void test_case_first_first_bucket() {
        theHash.erase(theHash.begin(), next(theHash.begin()));
        assert_buckets(4, 5, 5);
    }

    void test_case_last_first_bucket() {
        const auto i = next(theHash.begin(), 4);
        theHash.erase(i, next(i));
        assert_buckets(4, 5, 5);
    }

    void test_case_suffix_first_bucket() {
        theHash.erase(next(theHash.begin(), 2), next(theHash.begin(), 5));
        assert_buckets(2, 5, 5);
    }

    void test_case_whole_first_bucket() {
        theHash.erase(theHash.begin(), next(theHash.begin(), 5));
        assert_buckets(0, 5, 5);
    }

    void test_case_suffix_first_first_second_bucket() {
        theHash.erase(next(theHash.begin()), next(theHash.begin(), 6));
        assert_buckets(1, 4, 5);
    }

    void test_case_whole_first_first_second_bucket() {
        theHash.erase(theHash.begin(), next(theHash.begin(), 6));
        assert_buckets(0, 4, 5);
    }

    void test_case_suffix_first_prefix_second_bucket() {
        theHash.erase(next(theHash.begin()), next(theHash.begin(), 7));
        assert_buckets(1, 3, 5);
    }

    void test_case_whole_first_prefix_second_bucket() {
        theHash.erase(theHash.begin(), next(theHash.begin(), 7));
        assert_buckets(0, 3, 5);
    }

    void test_case_suffix_first_whole_second_bucket() {
        theHash.erase(next(theHash.begin()), next(theHash.begin(), 10));
        assert_buckets(1, 0, 5);
    }

    void test_case_whole_first_whole_second_bucket() {
        theHash.erase(theHash.begin(), next(theHash.begin(), 10));
        assert_buckets(0, 0, 5);
    }

    void test_case_suffix_first_first_third_bucket() {
        theHash.erase(next(theHash.begin()), next(theHash.begin(), 11));
        assert_buckets(1, 0, 4);
    }

    void test_case_whole_first_first_third_bucket() {
        theHash.erase(theHash.begin(), next(theHash.begin(), 11));
        assert_buckets(0, 0, 4);
    }

    void test_case_suffix_first_prefix_third_bucket() {
        theHash.erase(next(theHash.begin()), next(theHash.begin(), 12));
        assert_buckets(1, 0, 3);
    }

    void test_case_whole_first_prefix_third_bucket() {
        theHash.erase(theHash.begin(), next(theHash.begin(), 12));
        assert_buckets(0, 0, 3);
    }

    void test_case_all() {
        theHash.erase(theHash.begin(), theHash.end());
        assert_buckets(0, 0, 0);
    }

    void test_case_none() {
        theHash.erase(theHash.begin(), theHash.begin());
        assert_buckets(5, 5, 5);
    }
};

static size_t counted_hash_calls;
struct counting_hash {
    size_t operator()(const elem& e) const noexcept {
        ++counted_hash_calls;
        return e.memoized_hash;
    }
};

void test_case_elementwise_clear_called() {
    // this test case tests an implementation detail
    size_t calls;
    {
        unordered_set<elem, counting_hash> theHash;
        theHash.max_load_factor(0.1f); // 10 buckets per element
        for (size_t idx = 1; idx < size_max / 2; idx <<= 1) {
            theHash.insert({idx, idx});
        }

        // 10 buckets per element should choose the 'elementwise' strategy meaning the hash
        // function should be called
        counted_hash_calls = 0;
        theHash.clear();
        assert(counted_hash_calls != 0);
        theHash.insert({0, 0});
        calls = counted_hash_calls;
    } // destroying the container shouldn't hash anything
    assert(counted_hash_calls == calls);
    counted_hash_calls = 0;
}

int main(int argc, char* argv[]) {
    std_testing::death_test_executive exec([] {
        erase_fixture{}.test_case_erase_one();
        erase_fixture{}.test_case_range_erase_nothing();
        erase_fixture{}.test_case_range_erase_one();
        erase_fixture{}.test_case_range_erase_some();
        erase_fixture{}.test_case_range_erase_all();
        test_case_throwing_hash_not_called_in_clear();
        test_case_consistent_after_throwing_hash();
        erase_bucket_consistency_fixture{}.test_case_intra_first_bucket();
        erase_bucket_consistency_fixture{}.test_case_prefix_first_bucket();
        erase_bucket_consistency_fixture{}.test_case_first_first_bucket();
        erase_bucket_consistency_fixture{}.test_case_last_first_bucket();
        erase_bucket_consistency_fixture{}.test_case_suffix_first_bucket();
        erase_bucket_consistency_fixture{}.test_case_whole_first_bucket();
        erase_bucket_consistency_fixture{}.test_case_suffix_first_first_second_bucket();
        erase_bucket_consistency_fixture{}.test_case_whole_first_first_second_bucket();
        erase_bucket_consistency_fixture{}.test_case_suffix_first_prefix_second_bucket();
        erase_bucket_consistency_fixture{}.test_case_whole_first_prefix_second_bucket();
        erase_bucket_consistency_fixture{}.test_case_suffix_first_whole_second_bucket();
        erase_bucket_consistency_fixture{}.test_case_whole_first_whole_second_bucket();
        erase_bucket_consistency_fixture{}.test_case_suffix_first_first_third_bucket();
        erase_bucket_consistency_fixture{}.test_case_whole_first_first_third_bucket();
        erase_bucket_consistency_fixture{}.test_case_suffix_first_prefix_third_bucket();
        erase_bucket_consistency_fixture{}.test_case_whole_first_prefix_third_bucket();
        erase_bucket_consistency_fixture{}.test_case_all();
        erase_bucket_consistency_fixture{}.test_case_none();
        test_case_elementwise_clear_called();
    });

#if _ITERATOR_DEBUG_LEVEL == 2
    exec.add_death_tests({
        test_case_erase_one_bad,
        test_case_range_erase_one_bad,
        test_case_range_erase_some_bad_first,
        test_case_range_erase_some_bad_middle,
        test_case_range_erase_some_bad_last,
        test_case_range_erase_all_bad_first,
        test_case_range_erase_all_bad_middle,
        test_case_range_erase_all_bad_last,
    });
#endif // _ITERATOR_DEBUG_LEVEL == 2

    return exec.run(argc, argv);
}
