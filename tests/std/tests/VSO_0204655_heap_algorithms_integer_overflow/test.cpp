// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <algorithm>
#include <cassert>
#include <cstdint>
#include <iterator>
#include <limits>
#include <numeric>
#include <random>

using namespace std;

template <typename T, typename U>
void assert_equal(const T& lhs, const U& rhs) {
    assert(lhs == rhs);
}

typedef short test_difference_type;
template <typename T>
struct small_diff_iterator {
    typedef test_difference_type difference_type;
    typedef T value_type;
    typedef T* pointer;
    typedef T& reference;
    typedef random_access_iterator_tag iterator_category;

    pointer base_ptr;

    small_diff_iterator() : base_ptr() {}

    explicit small_diff_iterator(pointer base_ptr) : base_ptr(base_ptr) {}

    reference operator*() const {
        return *base_ptr;
    }

    pointer operator->() const {
        return base_ptr;
    }

    small_diff_iterator& operator++() {
        ++base_ptr;
        return *this;
    }

    void operator++(int) = delete; // avoid postincrement

    small_diff_iterator& operator--() {
        --base_ptr;
        return *this;
    }

    void operator--(int) = delete; // avoid postdecrement

    bool operator==(const small_diff_iterator& other) const {
        return base_ptr == other.base_ptr;
    }

    bool operator!=(const small_diff_iterator& other) const {
        return base_ptr != other.base_ptr;
    }

    bool operator<(const small_diff_iterator& other) const {
        return base_ptr < other.base_ptr;
    }

    bool operator<=(const small_diff_iterator& other) const {
        return base_ptr <= other.base_ptr;
    }

    bool operator>(const small_diff_iterator& other) const {
        return base_ptr > other.base_ptr;
    }

    bool operator>=(const small_diff_iterator& other) const {
        return base_ptr >= other.base_ptr;
    }

    small_diff_iterator& operator+=(difference_type n) {
        base_ptr += n;
        return *this;
    }

    small_diff_iterator& operator-=(difference_type n) {
        base_ptr -= n;
        return *this;
    }

    reference operator[](difference_type n) const = delete; // avoid subscript, N4849 [algorithms.requirements]/8
};
template <typename T>
small_diff_iterator<T> operator+(small_diff_iterator<T> lhs, test_difference_type rhs) {
    return small_diff_iterator<T>{lhs.base_ptr + rhs};
}
template <typename T>
small_diff_iterator<T> operator+(test_difference_type lhs, small_diff_iterator<T> rhs) {
    return small_diff_iterator<T>{lhs + rhs.base_ptr};
}
template <typename T>
small_diff_iterator<T> operator-(small_diff_iterator<T> lhs, test_difference_type rhs) {
    return small_diff_iterator<T>{lhs.base_ptr - rhs};
}
template <typename T>
test_difference_type operator-(small_diff_iterator<T> lhs, small_diff_iterator<T> rhs) {
    return static_cast<test_difference_type>(lhs.base_ptr - rhs.base_ptr);
}

template <typename T>
struct heap_test_fixture {
    static const auto max_size = static_cast<size_t>(numeric_limits<test_difference_type>::max());

    static T buff[max_size];

    static void fill_monotonic(T val) {
        iota(begin(buff), end(buff), val);
    }

    static void assert_sorted() {
        assert(is_sorted(begin(buff), end(buff)));
    }

    static void assert_heap() {
        assert(is_heap(begin(buff), end(buff)));
    }

    static void make_buff_heap() {
        make_heap(small_diff_iterator<T>{buff}, small_diff_iterator<T>{buff + max_size});
    }

    static void sort_buff_heap() {
        sort_heap(small_diff_iterator<T>{buff}, small_diff_iterator<T>{buff + max_size});
    }
};

template <typename T>
T heap_test_fixture<T>::buff[heap_test_fixture<T>::max_size];

struct simple_chars_heap_test : heap_test_fixture<char> {
    // This test is kept around mostly because it's easiest to debug what's going on inside the heap functions
    static void test() {
        const char small_value      = 42;
        const char big_value        = 100;
        const size_t division_point = 32; // constant doesn't matter, only following condition does
        static_assert(sizeof(test_difference_type) * CHAR_BIT < division_point, "");
        fill(begin(buff), end(buff) - division_point, small_value);
        fill(end(buff) - division_point, end(buff), big_value);
        // Before integer overflow bugs were fixed in push_heap / pop_heap, the following
        // line triggered integer overflows resulting in infinite loops.
        make_buff_heap();
        assert_heap();
        sort_buff_heap();
        assert_sorted();
    }
};

struct monotonic_integers_heap_test : heap_test_fixture<int> {
    static void test() {
        fill_monotonic(100);
        make_buff_heap();
        assert_heap();
        sort_buff_heap();
        assert_sorted();
    }
};

struct random_order_integers_heap_test : heap_test_fixture<int> {
    static void test() {
        fill_monotonic(1729);
        mt19937 r_gen(0);
        shuffle(begin(buff), end(buff), r_gen);
        assert(!is_sorted(begin(buff), end(buff)));
        make_buff_heap();
        assert_heap();

        buff[max_size - 1] = 1'000'000;
        push_heap(small_diff_iterator<int>{buff}, small_diff_iterator<int>{buff + max_size});
        assert_equal(buff[0], 1'000'000);
        assert(buff[max_size - 1] != 1'000'000);
        pop_heap(small_diff_iterator<int>{buff}, small_diff_iterator<int>{buff + max_size});
        assert_equal(buff[max_size - 1], 1'000'000);
        assert_equal(buff[0], static_cast<int>(1729 + max_size - 1));
        sort_heap(small_diff_iterator<int>{buff}, small_diff_iterator<int>{buff + max_size - 1});
        assert_sorted();
    }
};

void test_pop_heap_maintains_heap_at_each_step_test() {
    const int start_value  = 314159;
    const size_t buff_size = 1000;
    int buff[buff_size];

    iota(begin(buff), end(buff), start_value);
    make_heap(begin(buff), end(buff));

    for (int* logical_end = end(buff); logical_end != begin(buff); --logical_end) {
        const int expected_value = start_value + static_cast<int>(logical_end - begin(buff)) - 1;
        assert_equal(buff[0], expected_value);
        assert(is_heap(begin(buff), logical_end));
        pop_heap(begin(buff), logical_end);
        assert_equal(logical_end[-1], expected_value);
    }
}

// Also smoke test partial_sort which calls the heap functions' internal bits
// without the heap functions' normal precondition checking.
void test_partial_sort() {
    mt19937 r_gen(0);
    int all_numbers[100];
    iota(begin(all_numbers), end(all_numbers), 0);
    int buff[100];
    copy(begin(all_numbers), end(all_numbers), buff);

    // Test the degenerate case. Note that the standard doesn't guarantee this is
    // a no-op but we want to avoid swapping elements in our implementation when we can.
    {
        shuffle(begin(buff), end(buff), r_gen);
        int expected_first[100];
        copy(begin(buff), end(buff), expected_first);
        partial_sort(begin(buff), begin(buff), end(buff));
        assert(equal(begin(buff), end(buff), begin(expected_first), end(expected_first)));
    }

    for (size_t division = 1; division <= size(buff); ++division) {
        shuffle(begin(buff), end(buff), r_gen);
        const auto first = begin(buff);
        const auto mid   = first + division;
        const auto last  = end(buff);
        partial_sort(first, mid, last);
        assert(is_sorted(first, mid));
        assert(is_permutation(first, last, begin(all_numbers), end(all_numbers)));
        assert_equal(static_cast<size_t>(buff[division - 1]), division - 1);
    }
}

int main() {
    simple_chars_heap_test::test();
    monotonic_integers_heap_test::test();
    random_order_integers_heap_test::test();
    test_pop_heap_maintains_heap_at_each_step_test();
    test_partial_sort();
}
