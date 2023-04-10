// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <algorithm>
#include <cassert>
#include <cstdlib>
#include <functional>
#include <iterator>
#include <list>
#include <memory>
#include <stdexcept>
#include <type_traits>
#include <vector>

#include <test_death.hpp>

using namespace std;

struct non_noexcept_less_int {
    bool operator()(int a, int b) /* neither const nor noexcept */ {
        return a < b;
    }
};

template <class Cmp>
void test_case_ownership_of_element_transferred() {
    list<int> list1{1, 3, 5};
    list<int> list2{2, 4, 6};
    auto transfer = list2.begin();
    list1.merge(list2, Cmp{});
    list1.erase(transfer); // OK, transfer is now a member of list1
    static constexpr int expected[] = {1, 3, 4, 5, 6};
    assert(equal(begin(expected), end(expected), list1.begin(), list1.end()));
    assert(list2.empty());
}

template <class Cmp>
void test_case_ownership_of_element_transferred_bad() {
    list<int> list1{1, 3, 5};
    list<int> list2{2, 4, 6};
    auto transfer = list2.begin();
    list1.merge(list2, Cmp{});
    list2.erase(transfer); // Iterator incompatible, it became a member of list1
}

template <class Cmp>
void test_case_ownership_of_end_not_transferred() {
    list<int> list1{1, 3, 5};
    list<int> list2{2, 4, 6};
    auto noTransfer = list2.end();
    list1.merge(list2, Cmp{});
    list2.push_front(10);
    list2.erase(--noTransfer); // OK, end is not transferred
    static constexpr int expected[] = {1, 2, 3, 4, 5, 6};
    assert(equal(begin(expected), end(expected), list1.begin(), list1.end()));
    assert(list2.empty());
}

template <class Cmp>
void test_case_empty_both() {
    list<int> list1;
    list<int> list2;
    auto noTransfer = list2.end();
    list1.merge(list2, Cmp{});
    assert(list2.end() == noTransfer); // OK, stayed a member of list2
}

template <class Cmp>
void test_case_empty_both_bad() {
    list<int> list1;
    list<int> list2;
    auto noTransfer = list2.end();
    list1.merge(list2, Cmp{});
    (void) (list1.end() == noTransfer); // Iterator incompatible, still a member of list2
}

template <class Cmp>
void test_case_empty_source() {
    list<int> list1{1, 2, 3};
    list<int> list2;
    auto noTransfer = list2.end();
    list1.merge(list2, Cmp{});
    assert(list2.end() == noTransfer); // OK, stayed a member of list2
}

template <class Cmp>
void test_case_empty_source_bad() {
    list<int> list1{1, 2, 3};
    list<int> list2;
    auto noTransfer = list2.end();
    list1.merge(list2, Cmp{});
    (void) (list1.end() == noTransfer); // Iterator incompatible, still a member of list2
}

template <class Cmp>
void test_case_empty_target() {
    list<int> list1;
    list<int> list2{1, 2, 3};
    auto noTransfer = list2.end();
    auto transfer   = list2.begin();
    list1.merge(list2, Cmp{});
    assert(list2.end() == noTransfer); // Ownership not transferred
    assert(list1.begin() == transfer); // Ownership transferred to list1
}

template <class Cmp>
void test_case_empty_target_bad() {
    list<int> list1;
    list<int> list2{1, 2, 3};
    auto i = list2.begin();
    list1.merge(list2, Cmp{});
    (void) (list2.end() == i); // Iterator incompatible, was transferred to list1
}

struct throw_cmp_less_int {
    size_t remainingCompares;
    explicit throw_cmp_less_int(size_t remainingCompares_) : remainingCompares(remainingCompares_) {}
    throw_cmp_less_int(const throw_cmp_less_int&)            = delete;
    throw_cmp_less_int& operator=(const throw_cmp_less_int&) = delete;

    bool operator()(int a, int b) /* neither const nor noexcept */ {
        if (remainingCompares == 0) {
            throw runtime_error("either the algorithm is wrong or the user is evil (or both)");
        }

        const bool result = a < b;
        --remainingCompares;
#if _ITERATOR_DEBUG_LEVEL == 2
        if (result) {
            // _Debug_lt_pred will test the other direction
            ++remainingCompares;
        }
#endif // _ITERATOR_DEBUG_LEVEL == 2
        return result;
    }
};

struct throwing_merge_fixture {
    static constexpr size_t startF1Size         = 8;
    static constexpr size_t startF2Size         = 10;
    static constexpr size_t startTotal          = startF1Size + startF2Size;
    static constexpr size_t maxMergeComparisons = startTotal - 1;
#if _ITERATOR_DEBUG_LEVEL == 0
    static constexpr size_t ensureOrderedF1Size = 0;
    static constexpr size_t ensureOrderedF2Size = 0;
#else // ^^^ _ITERATOR_DEBUG_LEVEL == 0 / _ITERATOR_DEBUG_LEVEL != 0 vvv
    static constexpr size_t ensureOrderedF1Size = startF1Size - 1;
    static constexpr size_t ensureOrderedF2Size = startF2Size - 1;
#endif // _ITERATOR_DEBUG_LEVEL == 0
    static constexpr size_t maxTotalComparisons = maxMergeComparisons + ensureOrderedF1Size + ensureOrderedF2Size;

    list<int> list1;
    list<int> list2;
    vector<list<int>::const_iterator> iterators;

    void assert_iterators_consistent() const {
        size_t total = 0;
        for (auto i = list1.begin(), e = list1.end(); i != e; ++i) {
            ++total;
            assert(iterators[static_cast<size_t>(*i)] == i); // also checks ownership transfer
        }

        for (auto i = list2.begin(), e = list2.end(); i != e; ++i) {
            ++total;
            assert(iterators[static_cast<size_t>(*i)] == i); // also checks ownership transfer
        }

        assert(total == startTotal);
    }

    throwing_merge_fixture() : list1{0, 1, 2, 3, 8, 10, 15, 16}, list2{4, 5, 6, 7, 9, 11, 12, 13, 14, 17} {
        iterators.reserve(startTotal);
        auto f1It = list1.cbegin();
        auto f2It = list2.cbegin();
        iterators.push_back(f1It);
        iterators.push_back(++f1It);
        iterators.push_back(++f1It);
        iterators.push_back(++f1It);
        iterators.push_back(f2It); // 4
        iterators.push_back(++f2It);
        iterators.push_back(++f2It);
        iterators.push_back(++f2It);
        iterators.push_back(++f1It); // 8
        iterators.push_back(++f2It); // 9
        iterators.push_back(++f1It); // 10
        iterators.push_back(++f2It); // 11
        iterators.push_back(++f2It);
        iterators.push_back(++f2It);
        iterators.push_back(++f2It);
        iterators.push_back(++f1It); // 15
        iterators.push_back(++f1It);
        iterators.push_back(++f2It); // 17
    }
};

void test_case_comparison_count() {
    throwing_merge_fixture f;
    list<int> startedEmpty;
    {
        throw_cmp_less_int cmp{throwing_merge_fixture::ensureOrderedF1Size};
        startedEmpty.merge(f.list1, ref(cmp));
    }

    {
        throw_cmp_less_int cmp{throwing_merge_fixture::ensureOrderedF1Size};
        startedEmpty.merge(list<int>{}, ref(cmp));
    }

    {
        throw_cmp_less_int cmp{throwing_merge_fixture::maxTotalComparisons};
        startedEmpty.merge(f.list2, ref(cmp));
    }
}

void test_case_partial_merge_throw() {
    for (size_t idx = 0; idx <= throwing_merge_fixture::maxTotalComparisons; ++idx) {
        throwing_merge_fixture f;
        try {
            throw_cmp_less_int cmp{idx};
            f.list2.merge(f.list1, ref(cmp));
            f.assert_iterators_consistent();
            return;
        } catch (const runtime_error&) {
            f.assert_iterators_consistent();
        }
    }

    abort();
}

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
    };
    liveness_empty_asserter instance_empty; // destroyed after theList

    list<int, liveness_allocator<int>> theList;

    struct post_asserter {
        list<int, liveness_allocator<int>>& theList;

        ~post_asserter() {
            // tests that erase actually deallocated
            assert(liveness_alive_objects == (_ITERATOR_DEBUG_LEVEL != 0) + 1 + theList.size());
            // tests that the whole container is iterable
            assert(is_sorted(theList.begin(), theList.end()));
        }
    };
    post_asserter instance_nonempty; // destroyed before theList

    void assert_iterators_consistent(const vector<list<int>::const_iterator>& iterators) {
        assert(theList.size() == iterators.size() - 1);
        auto i = theList.begin();
        assert(i == iterators[0]);
        for (size_t idx = 1; idx < iterators.size(); ++idx) {
            ++i;
            assert(i == iterators[idx]);
        }
    }

    vector<list<int>::const_iterator> get_iterators() {
        vector<list<int>::const_iterator> iterators;
        auto i = theList.cbegin();
        for (size_t idx = 0; idx < theList.size(); ++idx) {
            iterators.push_back(i);
            ++i;
        }

        iterators.push_back(i);
        return iterators;
    }

    erase_fixture() : instance_empty(), theList{0, 1, 2, 3, 4, 5, 6, 7, 8, 9}, instance_nonempty{theList} {}

    void test_case_erase_one() {
        auto iterators = get_iterators();
        theList.erase(prev(theList.end()));
        iterators[iterators.size() - 2] = iterators[iterators.size() - 1];
        iterators.pop_back();
        assert_iterators_consistent(iterators);
    }

    void test_case_range_erase_nothing() {
        auto iterators = get_iterators();
        theList.erase(theList.begin(), theList.begin());
        assert_iterators_consistent(iterators);
    }

    void test_case_range_erase_one() {
        auto iterators = get_iterators();
        theList.erase(prev(theList.end()), theList.end());
        iterators.erase(iterators.end() - 2);
        assert_iterators_consistent(iterators);
    }

    void test_case_range_erase_some() {
        auto iterators = get_iterators();
        theList.erase(next(theList.begin()), prev(theList.end()));
        iterators.erase(iterators.begin() + 1, iterators.begin() + static_cast<ptrdiff_t>(iterators.size() - 2));
        assert_iterators_consistent(iterators);
    }

    void test_case_range_erase_all() {
        auto iterators = get_iterators();
        theList.erase(theList.begin(), theList.end());
        iterators.erase(iterators.begin(), iterators.end() - 1);
        assert_iterators_consistent(iterators);
    }
};

void test_case_erase_one_bad() {
    erase_fixture f;
    auto i = f.theList.begin();
    f.theList.erase(i);
    (void) *i;
}

void test_case_range_erase_one_bad() {
    erase_fixture f;
    auto i = f.theList.end();
    --i;
    f.theList.erase(i, f.theList.end());
    (void) *i;
}

void test_case_range_erase_some_bad_first() {
    erase_fixture f;
    auto first = f.theList.begin();
    ++first;
    f.theList.erase(first, f.theList.end());
    (void) *first;
}

void test_case_range_erase_some_bad_middle() {
    erase_fixture f;
    auto middle = f.theList.begin();
    ++middle;
    ++middle;
    f.theList.erase(next(f.theList.begin()), prev(f.theList.end()));
    (void) *middle;
}

void test_case_range_erase_some_bad_last() {
    erase_fixture f;
    auto last = f.theList.end();
    --last;
    --last;
    f.theList.erase(next(f.theList.begin()), prev(f.theList.end()));
    (void) *last;
}

void test_case_range_erase_all_bad_first() {
    erase_fixture f;
    auto first = f.theList.begin();
    f.theList.erase(f.theList.begin(), f.theList.end());
    (void) *first;
}

void test_case_range_erase_all_bad_middle() {
    erase_fixture f;
    auto middle = f.theList.begin();
    ++middle;
    ++middle;
    f.theList.erase(f.theList.begin(), f.theList.end());
    (void) *middle;
}

void test_case_range_erase_all_bad_last() {
    erase_fixture f;
    auto last = f.theList.end();
    --last;
    f.theList.erase(f.theList.begin(), f.theList.end());
    (void) *last;
}

void test_case_empty_front_bad() {
    list<int> li;
    (void) li.front();
}

void test_case_empty_cfront_bad() {
    const list<int> li;
    (void) li.front();
}

void test_case_empty_back_bad() {
    list<int> li;
    (void) li.back();
}

void test_case_empty_cback_bad() {
    const list<int> li;
    (void) li.back();
}

void test_case_pop_front_bad() {
    list<int> li;
    li.pop_front();
}

void test_case_pop_back_bad() {
    list<int> li;
    li.pop_back();
}

struct splice_fixture {
    list<int> source{0, 1, 2, 3, 4, 5};
    list<int> target{6, 7};
    vector<list<int>::const_iterator> iterators;

    splice_fixture() {
        {
            const auto sEnd = source.cend();
            for (auto it = source.cbegin(); it != sEnd; ++it) {
                iterators.push_back(it);
            }
        }
        {
            const auto tEnd = target.cend();
            for (auto it = target.cbegin(); it != tEnd; ++it) {
                iterators.push_back(it);
            }
        }
    }

    ~splice_fixture() {
        auto expected = iterators.cbegin();
        {
            const auto sEnd = source.cend();
            for (auto it = source.cbegin(); it != sEnd; ++it) {
                (void) *it; // asserts that the iterator is not invalidated
                assert(*expected == it);
                ++expected;
            }
        }

        {
            const auto tEnd = target.cend();
            for (auto it = target.cbegin(); it != tEnd; ++it) {
                (void) *it; // asserts that the iterator is not invalidated
                assert(*expected == it);
                ++expected;
            }
        }

        assert(expected == iterators.cend());
    }
};

void test_case_splices() {
    for (int splices = 0; splices < 6; ++splices) {
        splice_fixture f;
        f.target.splice(f.target.begin(), f.source, prev(f.source.end(), splices), f.source.end());
    }
}


int main(int argc, char* argv[]) {
    std_testing::death_test_executive exec([] {
        test_case_ownership_of_element_transferred<non_noexcept_less_int>();
        test_case_ownership_of_element_transferred<less<>>();
        test_case_ownership_of_end_not_transferred<non_noexcept_less_int>();
        test_case_ownership_of_end_not_transferred<less<>>();
        test_case_empty_both<non_noexcept_less_int>();
        test_case_empty_both<less<>>();
        test_case_empty_source<non_noexcept_less_int>();
        test_case_empty_source<less<>>();
        test_case_empty_target<non_noexcept_less_int>();
        test_case_empty_target<less<>>();
        test_case_comparison_count();
        test_case_partial_merge_throw();
        erase_fixture{}.test_case_erase_one();
        erase_fixture{}.test_case_range_erase_nothing();
        erase_fixture{}.test_case_range_erase_one();
        erase_fixture{}.test_case_range_erase_some();
        erase_fixture{}.test_case_range_erase_all();
        test_case_splices();
    });

#if _ITERATOR_DEBUG_LEVEL == 2
    exec.add_death_tests({
        test_case_ownership_of_element_transferred_bad<non_noexcept_less_int>,
        test_case_ownership_of_element_transferred_bad<less<>>,
        test_case_empty_both_bad<non_noexcept_less_int>,
        test_case_empty_both_bad<less<>>,
        test_case_empty_source_bad<non_noexcept_less_int>,
        test_case_empty_source_bad<less<>>,
        test_case_empty_target_bad<non_noexcept_less_int>,
        test_case_empty_target_bad<less<>>,
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

#if _CONTAINER_DEBUG_LEVEL > 0
    exec.add_death_tests({
        test_case_empty_front_bad,
        test_case_empty_cfront_bad,
        test_case_empty_back_bad,
        test_case_empty_cback_bad,
        test_case_pop_front_bad,
        test_case_pop_back_bad,
    });
#endif // _CONTAINER_DEBUG_LEVEL > 0

    return exec.run(argc, argv);
}
