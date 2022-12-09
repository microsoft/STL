// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <algorithm>
#include <cassert>
#include <cstdlib>
#include <forward_list>
#include <functional>
#include <iterator>
#include <stdexcept>
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
    forward_list<int> f1{1, 3, 5};
    forward_list<int> f2{2, 4, 6};
    auto i = f2.begin();
    f1.merge(f2, Cmp{});
    f1.erase_after(i); // OK, i is now a member of f1
    static const int expected[] = {1, 2, 4, 5, 6};
    assert(equal(begin(expected), end(expected), f1.begin(), f1.end()));
    assert(f2.empty());
}

template <class Cmp>
void test_case_ownership_of_element_transferred_bad() {
    forward_list<int> f1{1, 3, 5};
    forward_list<int> f2{2, 4, 6};
    auto i = f2.begin();
    f1.merge(f2, Cmp{});
    f2.erase_after(i); // Iterator incompatible, it became a member of f1
}

template <class Cmp>
void test_case_ownership_of_before_begin_not_transferred() {
    forward_list<int> f1{1, 3, 5};
    forward_list<int> f2{2, 4, 6};
    auto i = f2.before_begin();
    f1.merge(f2, Cmp{});
    f2.push_front(10);
    f2.erase_after(i); // OK, before_begin is not transferred
    static const int expected[] = {1, 2, 3, 4, 5, 6};
    assert(equal(begin(expected), end(expected), f1.begin(), f1.end()));
    assert(f2.empty());
}

template <class Cmp>
void test_case_empty_both() {
    forward_list<int> f1;
    forward_list<int> f2;
    auto i = f2.before_begin();
    f1.merge(f2, Cmp{});
    assert(f2.before_begin() == i); // OK, stayed a member of f2
}

template <class Cmp>
void test_case_empty_both_bad() {
    forward_list<int> f1;
    forward_list<int> f2;
    auto i = f2.before_begin();
    f1.merge(f2, Cmp{});
    (void) (f1.before_begin() == i); // Iterator incompatible, still a member of f2
}

template <class Cmp>
void test_case_empty_source() {
    forward_list<int> f1{1, 2, 3};
    forward_list<int> f2;
    auto i = f2.before_begin();
    f1.merge(f2, Cmp{});
    assert(f2.before_begin() == i); // OK, stayed a member of f2
}

template <class Cmp>
void test_case_empty_source_bad() {
    forward_list<int> f1{1, 2, 3};
    forward_list<int> f2;
    auto i = f2.before_begin();
    f1.merge(f2, Cmp{});
    (void) (f1.before_begin() == i); // Iterator incompatible, still a member of f2
}

template <class Cmp>
void test_case_empty_target() {
    forward_list<int> f1;
    forward_list<int> f2{1, 2, 3};
    auto bi = f2.before_begin();
    auto i  = f2.begin();
    f1.merge(f2, Cmp{});
    assert(f2.before_begin() == bi); // Ownership not transferred
    assert(f1.begin() == i); // Ownership transferred to f1
}

template <class Cmp>
void test_case_empty_target_bad() {
    forward_list<int> f1;
    forward_list<int> f2{1, 2, 3};
    auto i = f2.begin();
    f1.merge(f2, Cmp{});
    (void) (f2.before_begin() == i); // Iterator incompatible, was transferred to f1
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

    forward_list<int> f1;
    forward_list<int> f2;
    vector<forward_list<int>::const_iterator> iterators;

    void assert_iterators_consistent() const {
        size_t total = 0;
        for (auto i = f1.begin(), e = f1.end(); i != e; ++i) {
            ++total;
            assert(iterators[static_cast<size_t>(*i)] == i); // also checks ownership transfer
        }

        for (auto i = f2.begin(), e = f2.end(); i != e; ++i) {
            ++total;
            assert(iterators[static_cast<size_t>(*i)] == i); // also checks ownership transfer
        }

        assert(total == startTotal);
    }

    throwing_merge_fixture() : f1{0, 1, 2, 3, 8, 10, 15, 16}, f2{4, 5, 6, 7, 9, 11, 12, 13, 14, 17} {
        iterators.reserve(startTotal);
        auto f1It = f1.cbegin();
        auto f2It = f2.cbegin();
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
    forward_list<int> startedEmpty;
    {
        throw_cmp_less_int cmp{throwing_merge_fixture::ensureOrderedF1Size};
        startedEmpty.merge(f.f1, ref(cmp));
    }

    {
        throw_cmp_less_int cmp{throwing_merge_fixture::ensureOrderedF1Size};
        startedEmpty.merge(forward_list<int>{}, ref(cmp));
    }

    {
        throw_cmp_less_int cmp{throwing_merge_fixture::maxTotalComparisons};
        startedEmpty.merge(f.f2, ref(cmp));
    }
}

void test_case_partial_merge_throw() {
    for (size_t idx = 0; idx <= throwing_merge_fixture::maxTotalComparisons; ++idx) {
        throwing_merge_fixture f;
        try {
            throw_cmp_less_int cmp{idx};
            f.f2.merge(f.f1, ref(cmp));
            f.assert_iterators_consistent();
            return;
        } catch (const runtime_error&) {
            f.assert_iterators_consistent();
        }
    }

    abort();
}

void test_case_front_bad() {
    forward_list<int> f;
    (void) f.front();
}

void test_case_cfront_bad() {
    const forward_list<int> f;
    (void) f.front();
}

void test_case_pop_front_bad() {
    forward_list<int> f;
    f.pop_front();
}

int main(int argc, char* argv[]) {
    std_testing::death_test_executive exec([] {
        test_case_ownership_of_element_transferred<non_noexcept_less_int>();
        test_case_ownership_of_element_transferred<less<>>();
        test_case_ownership_of_before_begin_not_transferred<non_noexcept_less_int>();
        test_case_ownership_of_before_begin_not_transferred<less<>>();
        test_case_empty_both<non_noexcept_less_int>();
        test_case_empty_both<less<>>();
        test_case_empty_source<non_noexcept_less_int>();
        test_case_empty_source<less<>>();
        test_case_empty_target<non_noexcept_less_int>();
        test_case_empty_target<less<>>();
        test_case_comparison_count();
        test_case_partial_merge_throw();
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
        test_case_front_bad,
        test_case_cfront_bad,
    });
#endif // _ITERATOR_DEBUG_LEVEL == 2

#if _CONTAINER_DEBUG_LEVEL > 0
    exec.add_death_tests({
        test_case_front_bad,
        test_case_cfront_bad,
        test_case_pop_front_bad,
    });
#endif // _CONTAINER_DEBUG_LEVEL > 0

    return exec.run(argc, argv);
}
