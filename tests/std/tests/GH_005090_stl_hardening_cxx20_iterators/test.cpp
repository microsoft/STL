// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

// env.lst defines _MSVC_STL_HARDENING to 1.

#include <cstddef>
#include <istream>
#include <iterator>
#include <utility>
#include <variant>

#include <test_death.hpp>

using namespace std;

// helper types for common_iterator

struct int_writer {
    using difference_type = ptrdiff_t;

    int n_{};

    int& operator*() { // non-const
        return n_;
    }

    int_writer& operator++() {
        return *this;
    }
    int_writer operator++(int) {
        return *this;
    }

    friend bool operator==(const int_writer&, default_sentinel_t) {
        return false;
    }
};

class lowered_iterator {
public:
    using value_type      = int;
    using difference_type = ptrdiff_t;

    lowered_iterator() = default;
    explicit lowered_iterator(int* p) : ptr_{p} {}

    int& operator*() const {
        return *ptr_;
    }

    int* operator->() const {
        return ptr_;
    }

    lowered_iterator& operator++() {
        ++ptr_;
        return *this;
    }

    lowered_iterator operator++(int) {
        auto old = *this;
        ++*this;
        return old;
    }

    friend bool operator==(const lowered_iterator&, const lowered_iterator&) = default;

    friend bool operator==(const lowered_iterator& i, int* const se) {
        return i.ptr_ == se;
    }

private:
    int* ptr_ = nullptr;
};

class exceptional_iterator {
public:
    using value_type      = int;
    using difference_type = ptrdiff_t;

    exceptional_iterator() = default;
    explicit exceptional_iterator(int* p) : ptr_{p} {}
    exceptional_iterator(const exceptional_iterator& other) : ptr_{other.ptr_} {
        check_throwing();
    }

    exceptional_iterator& operator=(const exceptional_iterator& other) {
        ptr_ = other.ptr_;
        check_throwing();
        return *this;
    }

    operator lowered_iterator() const {
        return lowered_iterator{ptr_};
    }

    int& operator*() const {
        return *ptr_;
    }

    int& operator[](const ptrdiff_t n) const {
        return ptr_[n];
    }

    int* operator->() const {
        return ptr_;
    }

    exceptional_iterator& operator++() {
        ++ptr_;
        return *this;
    }

    exceptional_iterator operator++(int) {
        auto old = *this;
        ++*this;
        return old;
    }

    exceptional_iterator& operator--() {
        --ptr_;
        return *this;
    }

    exceptional_iterator operator--(int) {
        auto old = *this;
        --*this;
        return old;
    }

    exceptional_iterator& operator+=(const ptrdiff_t n) {
        ptr_ += n;
        return *this;
    }

    exceptional_iterator& operator-=(const ptrdiff_t n) {
        ptr_ -= n;
        return *this;
    }

    friend bool operator==(const exceptional_iterator& i, const exceptional_iterator& j) {
        return i.ptr_ == j.ptr_;
    }

    friend strong_ordering operator<=>(const exceptional_iterator& i, const exceptional_iterator& j) {
        return i.ptr_ <=> j.ptr_;
    }

    friend exceptional_iterator operator+(const exceptional_iterator& i, const ptrdiff_t n) {
        return exceptional_iterator{i.ptr_ + n};
    }
    friend exceptional_iterator operator+(const ptrdiff_t n, const exceptional_iterator& i) {
        return exceptional_iterator{i.ptr_ + n};
    }

    friend exceptional_iterator operator-(const exceptional_iterator& i, const ptrdiff_t n) {
        return exceptional_iterator{i.ptr_ - n};
    }
    friend ptrdiff_t operator-(const exceptional_iterator& i, const exceptional_iterator& j) {
        return i.ptr_ - j.ptr_;
    }

    friend bool operator==(const exceptional_iterator& i, int* const se) {
        return i.ptr_ == se;
    }

    friend ptrdiff_t operator-(const exceptional_iterator& i, int* const se) {
        return i.ptr_ - se;
    }
    friend ptrdiff_t operator-(int* const se, const exceptional_iterator& i) {
        return se - i.ptr_;
    }

    static void reset_throwing() noexcept {
        throwing_ = false;
    }

private:
    static void check_throwing() {
        if (throwing_) {
            throw bad_variant_access{};
        }
        throwing_ = true;
    }

    int* ptr_ = nullptr;

    static inline bool throwing_ = false;
};

common_iterator<exceptional_iterator, int*> make_valueless_common_iterator() {
    using CI = common_iterator<exceptional_iterator, int*>;
    exceptional_iterator::reset_throwing();

    CI ret{static_cast<int*>(nullptr)};
    CI src{exceptional_iterator{}};
    try {
        ret = move(src);
    } catch (...) {
    }
    exceptional_iterator::reset_throwing();
    return ret;
}

// <iterator>
void test_counted_iterator_deref() {
    int arr[1]{};
    auto it = counted_iterator<int*>{arr, 1} + 1;
    (void) *it;
}

void test_counted_iterator_deref_const() {
    int arr[1]{};
    const auto it = counted_iterator<int*>{arr, 1} + 1;
    (void) *it;
}

void test_counted_iterator_subscript() {
    int arr[1]{};
    counted_iterator<int*> it{arr, 1};
    (void) it[1];
}

void test_counted_iterator_iter_move() {
    int arr[1]{};
    auto it = counted_iterator<int*>{arr, 1} + 1;
    (void) ranges::iter_move(it);
}

void test_counted_iterator_iter_swap() {
    int arr[1]{};
    counted_iterator<int*> it{arr, 1};
    auto jt = it + 1;
    (void) ranges::iter_swap(it, jt);
}

void test_counted_iterator_construction() {
    int arr[1]{};
    [[maybe_unused]] counted_iterator<int*> it{arr, -1};
}

void test_counted_iterator_pre_inc() {
    int arr[1]{};
    auto it = counted_iterator<int*>{arr, 1} + 1;
    ++it;
}

void test_counted_iterator_post_inc_input() {
    counted_iterator<istream_iterator<int>> it{istream_iterator<int>{}, 0};
    it++;
}

void test_counted_iterator_post_inc_forward() {
    int arr[1]{};
    auto it = counted_iterator<int*>{arr, 1} + 1;
    it++;
}

void test_counted_iterator_advance() {
    int arr[1]{};
    counted_iterator<int*> it{arr, 1};
    it += 2;
}

void test_counted_iterator_retreat() {
    int arr[1]{};
    counted_iterator<int*> it{arr, 1};
    it -= -2;
}

void test_common_iterator_construct() {
    auto it = make_valueless_common_iterator();
    [[maybe_unused]] common_iterator<lowered_iterator, int*> jt{it};
}

void test_common_iterator_assign() {
    auto it = make_valueless_common_iterator();
    common_iterator<lowered_iterator, int*> jt{static_cast<int*>(nullptr)};
    jt = it;
}

void test_common_iterator_deref() {
    common_iterator<int_writer, default_sentinel_t> it{default_sentinel};
    (void) *it;
}

void test_common_iterator_deref_const() {
    const common_iterator<counted_iterator<int*>, default_sentinel_t> it{default_sentinel};
    (void) *it;
}

void test_common_iterator_arrow() {
    struct S {
        int n;
    };
    common_iterator<counted_iterator<S*>, default_sentinel_t> it{default_sentinel};
    (void) it->n;
}

void test_common_iterator_pre_inc() {
    common_iterator<counted_iterator<int*>, default_sentinel_t> it{default_sentinel};
    (void) ++it;
}

void test_common_iterator_post_inc() {
    common_iterator<counted_iterator<int*>, default_sentinel_t> it{default_sentinel};
    (void) it++;
}

void test_common_iterator_equality() {
    auto it = make_valueless_common_iterator();
    common_iterator<exceptional_iterator, int*> jt{static_cast<int*>(nullptr)};
    (void) (it == jt);
}

void test_common_iterator_difference() {
    auto it = make_valueless_common_iterator();
    common_iterator<exceptional_iterator, int*> jt{static_cast<int*>(nullptr)};
    (void) (it - jt);
}

void test_common_iterator_iter_move() {
    common_iterator<counted_iterator<int*>, default_sentinel_t> it{default_sentinel};
    (void) ranges::iter_move(it);
}

void test_common_iterator_iter_swap() {
    using CI = counted_iterator<int*>;

    int arr[1]{};
    common_iterator<CI, default_sentinel_t> it{default_sentinel};
    common_iterator<CI, default_sentinel_t> jt{CI{arr, 1}};
    (void) ranges::iter_swap(it, jt);
}

int main(int argc, char* argv[]) {
    std_testing::death_test_executive exec;

    exec.add_death_tests({
        test_counted_iterator_deref,
        test_counted_iterator_deref_const,
        test_counted_iterator_subscript,
        test_counted_iterator_iter_move,
        test_counted_iterator_iter_swap,
        test_counted_iterator_construction,
        test_counted_iterator_pre_inc,
        test_counted_iterator_post_inc_input,
        test_counted_iterator_post_inc_forward,
        test_counted_iterator_advance,
        test_counted_iterator_retreat,
        test_common_iterator_construct,
        test_common_iterator_assign,
        test_common_iterator_deref,
        test_common_iterator_deref_const,
        test_common_iterator_arrow,
        test_common_iterator_pre_inc,
        test_common_iterator_post_inc,
        test_common_iterator_equality,
        test_common_iterator_difference,
        test_common_iterator_iter_move,
        test_common_iterator_iter_swap,
    });

    return exec.run(argc, argv);
}
