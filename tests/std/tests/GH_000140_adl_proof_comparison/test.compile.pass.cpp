// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef _M_CEE // TRANSITION, VSO-1659496
#include <cstddef>
#include <functional>
#include <iterator>
#include <memory>
#include <type_traits>
#include <utility>

#if _HAS_CXX17
#include <optional>
#endif // _HAS_CXX17

#if _HAS_CXX20 && defined(__cpp_lib_concepts) // TRANSITION, GH-395
#include <ranges>
#endif // _HAS_CXX20 && defined(__cpp_lib_concepts)

#if _HAS_CXX23 && defined(__cpp_lib_concepts) // TRANSITION, GH-395
#include <expected>
#endif // _HAS_CXX23 && defined(__cpp_lib_concepts)

using namespace std;

template <class T>
struct tagged_bool_like {
    bool val_;
    constexpr operator bool() const noexcept {
        return val_;
    }
};

template <class T>
struct holder {
    T t;
};

struct incomplete;

using validating_bool_like = tagged_bool_like<holder<incomplete>>;

constexpr holder<incomplete>* placeholder             = nullptr;
constexpr holder<incomplete>* const* placeholder_addr = std::addressof(placeholder);

struct test_comparable {
    friend constexpr validating_bool_like operator==(const test_comparable&, const test_comparable&) noexcept {
        return {true};
    }
    friend constexpr validating_bool_like operator!=(const test_comparable&, const test_comparable&) noexcept {
        return {false};
    }
    friend constexpr validating_bool_like operator<(const test_comparable&, const test_comparable&) noexcept {
        return {false};
    }
    friend constexpr validating_bool_like operator>(const test_comparable&, const test_comparable&) noexcept {
        return {false};
    }
    friend constexpr validating_bool_like operator<=(const test_comparable&, const test_comparable&) noexcept {
        return {true};
    }
    friend constexpr validating_bool_like operator>=(const test_comparable&, const test_comparable&) noexcept {
        return {true};
    }
};

template <class T>
struct validating_iterator_provider {
    struct iterator;
};

template <class, class = void>
constexpr bool is_validating_iterator = false;
template <class I>
constexpr bool is_validating_iterator<I, void_t<typename I::reference>> =
    is_same_v<I, typename validating_iterator_provider<remove_reference_t<typename I::reference>>::iterator>;

template <class T>
struct validating_iterator_provider<T>::iterator {
    using value_type        = remove_cv_t<T>;
    using pointer           = T*;
    using reference         = T&;
    using difference_type   = ptrdiff_t;
    using iterator_category = random_access_iterator_tag;
#if _HAS_CXX20 && defined(__cpp_lib_concepts) // TRANSITION, GH-395
    using iterator_concept = contiguous_iterator_tag;
#endif // _HAS_CXX20 && defined(__cpp_lib_concepts)

    constexpr T& operator*() const noexcept {
        return *ptr_;
    }

    constexpr T* operator->() const noexcept {
        return ptr_;
    }

    constexpr T& operator[](const ptrdiff_t n) const noexcept {
        return ptr_[n];
    }

    constexpr iterator& operator++() noexcept {
        ++ptr_;
        return *this;
    }
    constexpr iterator operator++(int) noexcept {
        auto old = *this;
        ++*this;
        return old;
    }

    constexpr iterator& operator--() noexcept {
        --ptr_;
        return *this;
    }
    constexpr iterator operator--(int) noexcept {
        auto old = *this;
        --*this;
        return old;
    }

    constexpr iterator& operator+=(const ptrdiff_t n) noexcept {
        ptr_ += n;
        return *this;
    }

    constexpr iterator& operator-=(const ptrdiff_t n) noexcept {
        ptr_ -= n;
        return *this;
    }

    friend constexpr iterator operator+(const iterator i, const ptrdiff_t n) noexcept {
        return {i.ptr_ + n};
    }
    friend constexpr iterator operator+(const ptrdiff_t n, const iterator i) noexcept {
        return {i.ptr_ + n};
    }

    friend constexpr ptrdiff_t operator-(const iterator i, const iterator j) noexcept {
        return i.ptr_ - j.ptr_;
    }
    friend constexpr iterator operator-(const iterator i, const ptrdiff_t n) noexcept {
        return iterator{i.ptr_ - n};
    }

    template <class OtherIter, enable_if_t<is_validating_iterator<OtherIter>, int> = 0>
    friend constexpr auto operator==(iterator i, OtherIter j) noexcept
        -> decltype(pointer{} == typename OtherIter::pointer{} ? placeholder_addr : nullptr) {
        return i.operator->() == j.operator->() ? placeholder_addr : nullptr;
    }
    template <class OtherIter, enable_if_t<is_validating_iterator<OtherIter>, int> = 0>
    friend constexpr auto operator!=(iterator i, OtherIter j) noexcept
        -> decltype(pointer{} != typename OtherIter::pointer{} ? placeholder_addr : nullptr) {
        return i.operator->() != j.operator->() ? placeholder_addr : nullptr;
    }

    template <class OtherIter, enable_if_t<is_validating_iterator<OtherIter>, int> = 0>
    friend constexpr auto operator<(iterator i, OtherIter j) noexcept
        -> decltype(pointer{} < typename OtherIter::pointer{} ? placeholder_addr : nullptr) {
        return i.operator->() < j.operator->() ? placeholder_addr : nullptr;
    }
    template <class OtherIter, enable_if_t<is_validating_iterator<OtherIter>, int> = 0>
    friend constexpr auto operator>(iterator i, OtherIter j) noexcept
        -> decltype(pointer{} > typename OtherIter::pointer{} ? placeholder_addr : nullptr) {
        return i.operator->() > j.operator->() ? placeholder_addr : nullptr;
    }
    template <class OtherIter, enable_if_t<is_validating_iterator<OtherIter>, int> = 0>
    friend constexpr auto operator<=(iterator i, OtherIter j) noexcept
        -> decltype(pointer{} <= typename OtherIter::pointer{} ? placeholder_addr : nullptr) {
        return i.operator->() <= j.operator->() ? placeholder_addr : nullptr;
    }
    template <class OtherIter, enable_if_t<is_validating_iterator<OtherIter>, int> = 0>
    friend constexpr auto operator>=(iterator i, OtherIter j) noexcept
        -> decltype(pointer{} >= typename OtherIter::pointer{} ? placeholder_addr : nullptr) {
        return i.operator->() >= j.operator->() ? placeholder_addr : nullptr;
    }

    template <class OtherIter, enable_if_t<!is_same_v<OtherIter, iterator> //
                                               && is_validating_iterator<OtherIter> //
                                               && is_convertible_v<T*, typename OtherIter::pointer>,
                                   int> = 0>
    constexpr operator OtherIter() const noexcept {
        return {ptr_};
    }

    T* ptr_;
};

void test_adl_proof_legacy_comparison_functors() {
    (void) equal_to<test_comparable>{}(test_comparable{}, test_comparable{});
    (void) not_equal_to<test_comparable>{}(test_comparable{}, test_comparable{});
    (void) less<test_comparable>{}(test_comparable{}, test_comparable{});
    (void) greater<test_comparable>{}(test_comparable{}, test_comparable{});
    (void) less_equal<test_comparable>{}(test_comparable{}, test_comparable{});
    (void) greater_equal<test_comparable>{}(test_comparable{}, test_comparable{});
}

void test_adl_proof_reverse_iterator_comparison() {
    using validating_iter = validating_iterator_provider<holder<incomplete>*>::iterator;
    holder<incomplete>* p{};

    reverse_iterator<validating_iter> rit{validating_iter{&p + 1}};
    (void) rit.operator->();
    (void) rit[0];

    reverse_iterator<validating_iterator_provider<int>::iterator> i{};
    reverse_iterator<validating_iterator_provider<const int>::iterator> j{};

    (void) (i == j);
    (void) (i != j);
    (void) (i < j);
    (void) (i > j);
    (void) (i <= j);
    (void) (i >= j);

    (void) (j == i);
    (void) (j != i);
    (void) (j < i);
    (void) (j > i);
    (void) (j <= i);
    (void) (j >= i);
}

void test_adl_proof_move_iterator_comparison() {
    move_iterator<validating_iterator_provider<int>::iterator> i{};
    move_iterator<validating_iterator_provider<const int>::iterator> j{};

    (void) (i == j);
    (void) (i != j);
    (void) (i < j);
    (void) (i > j);
    (void) (i <= j);
    (void) (i >= j);

    (void) (j == i);
    (void) (j != i);
    (void) (j < i);
    (void) (j > i);
    (void) (j <= i);
    (void) (j >= i);

#if _HAS_CXX20 && defined(__cpp_lib_concepts) // TRANSITION, GH-395
    move_sentinel<validating_iterator_provider<int>::iterator> s{};

    (void) (i == s);
    (void) (i != s);
#endif // _HAS_CXX20 && defined(__cpp_lib_concepts)
}

#if _HAS_CXX17
void test_adl_proof_optional_comparison() {
    test_comparable v{};
    optional<test_comparable> op{v};

    (void) (op == op);
    (void) (op != op);
    (void) (op < op);
    (void) (op > op);
    (void) (op <= op);
    (void) (op >= op);

    (void) (op == v);
    (void) (op != v);
    (void) (op < v);
    (void) (op > v);
    (void) (op <= v);
    (void) (op >= v);

    (void) (v == op);
    (void) (v != op);
    (void) (v < op);
    (void) (v > op);
    (void) (v <= op);
    (void) (v >= op);
}
#endif // _HAS_CXX17

#if _HAS_CXX23 && defined(__cpp_lib_concepts) // TRANSITION, GH-395
void test_adl_proof_expected_comparison() {
    expected<test_comparable, test_comparable> ex;
    (void) (ex == ex);
    (void) (ex != ex);

    expected<void, test_comparable> vex;
    (void) (vex == vex);
    (void) (vex != vex);

    unexpected<test_comparable> unex{test_comparable{}};
    (void) (unex == unex);
    (void) (unex != unex);
}

void test_adl_proof_basic_const_iterator_comparison() {
    validating_iterator_provider<int>::iterator i{};
    validating_iterator_provider<const int>::iterator j{};

    basic_const_iterator<validating_iterator_provider<int>::iterator> ci{};
    basic_const_iterator<validating_iterator_provider<const int>::iterator> cj{};

    (void) (ci == ci);
    (void) (ci != ci);
    (void) (ci < ci);
    (void) (ci > ci);
    (void) (ci <= ci);
    (void) (ci >= ci);

    (void) (ci == cj);
    (void) (ci != cj);

    (void) (cj == ci);
    (void) (cj != ci);
    (void) (cj < ci);
    (void) (cj > ci);
    (void) (cj <= ci);
    (void) (cj >= ci);

    (void) (ci == i);
    (void) (ci != i);
    (void) (ci < i);
    (void) (ci > i);
    (void) (ci <= i);
    (void) (ci >= i);

    (void) (ci == j);
    (void) (ci != j);

    (void) (cj == i);
    (void) (cj != i);
    (void) (cj < i);
    (void) (cj > i);
    (void) (cj <= i);
    (void) (cj >= i);

    (void) (i == ci);
    (void) (i != ci);
    (void) (i < ci);
    (void) (i > ci);
    (void) (i <= ci);
    (void) (i >= ci);

    (void) (i == cj);
    (void) (i != cj);

    (void) (j == ci);
    (void) (j != ci);
    (void) (j < ci);
    (void) (j > ci);
    (void) (j <= ci);
    (void) (j >= ci);
}
#endif // _HAS_CXX23 && defined(__cpp_lib_concepts)

#endif // _M_CEE
