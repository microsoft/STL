// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef _M_CEE // TRANSITION, VSO-1659496
#include <cstddef>
#include <functional>
#include <memory>
#include <ranges>
#include <type_traits>
#include <utility>

using namespace std;

template <class T>
struct holder {
    T t;
};

struct incomplete;

constexpr holder<incomplete>* placeholder             = nullptr;
constexpr holder<incomplete>* const* placeholder_addr = &placeholder;

template <class T>
struct validating_iterator_provider {
    struct iterator;
};

template <class I>
concept validating_iterator_specialization = requires {
    typename I::reference;
} && same_as<I, typename validating_iterator_provider<remove_reference_t<typename I::reference>>::iterator>;

template <class T>
struct validating_iterator_provider<T>::iterator {
    using value_type        = remove_cv_t<T>;
    using pointer           = T*;
    using reference         = T&;
    using difference_type   = ptrdiff_t;
    using iterator_category = random_access_iterator_tag;
    using iterator_concept  = contiguous_iterator_tag;

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

    template <validating_iterator_specialization OtherIter>
    friend constexpr decltype(auto) operator==(iterator i, OtherIter j) noexcept
        requires requires { pointer{} == typename OtherIter::pointer{} ? placeholder_addr : nullptr; }
    {
        return i.operator->() == j.operator->() ? placeholder_addr : nullptr;
    }
    template <validating_iterator_specialization OtherIter>
    friend constexpr decltype(auto) operator!=(iterator i, OtherIter j) noexcept
        requires requires { pointer{} != typename OtherIter::pointer{} ? placeholder_addr : nullptr; }
    {
        return i.operator->() != j.operator->() ? placeholder_addr : nullptr;
    }

    template <validating_iterator_specialization OtherIter>
    friend constexpr decltype(auto) operator<(iterator i, OtherIter j) noexcept
        requires requires { pointer{} < typename OtherIter::pointer{} ? placeholder_addr : nullptr; }
    {
        return i.operator->() < j.operator->() ? placeholder_addr : nullptr;
    }
    template <validating_iterator_specialization OtherIter>
    friend constexpr decltype(auto) operator>(iterator i, OtherIter j) noexcept
        requires requires { pointer{} > typename OtherIter::pointer{} ? placeholder_addr : nullptr; }
    {
        return i.operator->() > j.operator->() ? placeholder_addr : nullptr;
    }
    template <validating_iterator_specialization OtherIter>
    friend constexpr decltype(auto) operator<=(iterator i, OtherIter j)
        requires requires { pointer{} <= typename OtherIter::pointer{} ? placeholder_addr : nullptr; }
    {
        return i.operator->() <= j.operator->() ? placeholder_addr : nullptr;
    }
    template <validating_iterator_specialization OtherIter>
    friend constexpr decltype(auto) operator>=(iterator i, OtherIter j) noexcept
        requires requires { pointer{} >= typename OtherIter::pointer{} ? placeholder_addr : nullptr; }
    {
        return i.operator->() >= j.operator->() ? placeholder_addr : nullptr;
    }

    template <validating_iterator_specialization OtherIter>
        requires (!is_same_v<OtherIter, iterator>) && convertible_to<T*, typename OtherIter::pointer>
    constexpr operator OtherIter() const noexcept {
        return {ptr_};
    }

    T* ptr_;
};

struct validating_view_for_verification {
    constexpr holder<incomplete>** begin() const noexcept {
        return begin_;
    }

    constexpr holder<incomplete>** end() const noexcept {
        return end_;
    }

    holder<incomplete>** begin_;
    holder<incomplete>** end_;
};

template <>
inline constexpr bool ranges::enable_view<validating_view_for_verification> = true;

template <>
inline constexpr bool ranges::enable_borrowed_range<validating_view_for_verification> = true;

template <class Tag>
struct tagged_ints_view {
    constexpr int* begin() const noexcept {
        return begin_;
    }

    constexpr int* end() const noexcept {
        return end_;
    }

    int* begin_;
    int* end_;
};

template <class Tag>
constexpr bool ranges::enable_view<tagged_ints_view<Tag>> = true;

template <class Tag>
constexpr bool ranges::enable_borrowed_range<tagged_ints_view<Tag>> = true;

struct validating_view_to_join_for_verification {
    constexpr const tagged_ints_view<holder<incomplete>>* begin() const noexcept {
        return begin_;
    }

    constexpr const tagged_ints_view<holder<incomplete>>* end() const noexcept {
        return end_;
    }

    const tagged_ints_view<holder<incomplete>>* begin_;
    const tagged_ints_view<holder<incomplete>>* end_;
};

template <>
inline constexpr bool ranges::enable_view<validating_view_to_join_for_verification> = true;

template <>
inline constexpr bool ranges::enable_borrowed_range<validating_view_to_join_for_verification> = true;

void test_views_verification() {
    constexpr auto simple_truth = [](const auto&) { return true; };

    holder<incomplete>* varr[1]{};
    const validating_view_for_verification validating_view_val{varr, varr + 1};

    for (auto&& _ [[maybe_unused]] : validating_view_val | views::filter(simple_truth)) {
    }

    for (auto&& _ [[maybe_unused]] : validating_view_val | views::transform(simple_truth)) {
    }

    int iarr[1]{};
    const tagged_ints_view<holder<incomplete>> inner_arr[1]{{iarr, iarr + 1}};
    const validating_view_to_join_for_verification view_to_join{inner_arr, inner_arr + 1};

    for (auto&& _ [[maybe_unused]] : view_to_join | views::join) {
    }

    for (auto&& _ [[maybe_unused]] : view_to_join | views::join | views::reverse) {
    }

#if _HAS_CXX23
    const tagged_ints_view<void> simple_pattern{iarr, iarr + 1};

    for (auto&& _ [[maybe_unused]] : view_to_join | views::join_with(simple_pattern)) {
    }
#endif // _HAS_CXX23
}

struct validating_common_view {
    using iterator = validating_iterator_provider<int>::iterator;

    constexpr iterator begin() const noexcept {
        return {begin_};
    }

    constexpr iterator end() const noexcept {
        return {end_};
    }

    int* begin_;
    int* end_;
};

template <>
inline constexpr bool ranges::enable_view<validating_common_view> = true;

template <>
inline constexpr bool ranges::enable_borrowed_range<validating_common_view> = true;

template <class I>
struct noncommon_sentinel_for {
    using pointer = iterator_traits<I>::pointer;

    pointer p_end_;

    friend constexpr holder<incomplete>* const* operator==(const I i, const noncommon_sentinel_for s) noexcept {
        return to_address(i) == s.p_end_ ? placeholder_addr : nullptr;
    }
    friend constexpr holder<incomplete>* const* operator==(const noncommon_sentinel_for s, const I i) noexcept {
        return to_address(i) == s.p_end_ ? placeholder_addr : nullptr;
    }
    friend constexpr holder<incomplete>* const* operator!=(const I i, const noncommon_sentinel_for s) noexcept {
        return to_address(i) == s.p_end_ ? nullptr : placeholder_addr;
    }
    friend constexpr holder<incomplete>* const* operator!=(const noncommon_sentinel_for s, const I i) noexcept {
        return to_address(i) == s.p_end_ ? nullptr : placeholder_addr;
    }
};

struct validating_noncommon_view {
    using iterator = validating_iterator_provider<int>::iterator;

    constexpr iterator begin() const noexcept {
        return {begin_};
    }

    constexpr noncommon_sentinel_for<iterator> end() const noexcept {
        return {end_};
    }

    int* begin_;
    int* end_;
};

template <>
inline constexpr bool ranges::enable_view<validating_noncommon_view> = true;

template <>
inline constexpr bool ranges::enable_borrowed_range<validating_noncommon_view> = true;


struct validating_noncommon_to_join_view {
    using iterator = const validating_noncommon_view*;

    constexpr iterator begin() const noexcept {
        return begin_;
    }

    constexpr noncommon_sentinel_for<iterator> end() const noexcept {
        return {end_};
    }

    const validating_noncommon_view* begin_;
    const validating_noncommon_view* end_;
};

template <>
inline constexpr bool ranges::enable_view<validating_noncommon_to_join_view> = true;

template <>
inline constexpr bool ranges::enable_borrowed_range<validating_noncommon_to_join_view> = true;

struct negation_validating_iterator {
    struct boolean_testable_result {
        constexpr operator bool() const noexcept {
            return val_;
        }

        constexpr holder<incomplete>* const* operator!() const noexcept {
            return val_ ? placeholder_addr : nullptr;
        }
        bool val_;
    };

    using value_type        = int;
    using reference         = int&;
    using pointer           = int*;
    using difference_type   = ptrdiff_t;
    using iterator_category = random_access_iterator_tag;
    using iterator_concept  = contiguous_iterator_tag;

    constexpr int& operator*() const noexcept {
        return *ptr_;
    }

    constexpr int* operator->() const noexcept {
        return ptr_;
    }

    constexpr negation_validating_iterator& operator++() noexcept {
        ++ptr_;
        return *this;
    }
    constexpr negation_validating_iterator operator++(int) noexcept {
        auto old = *this;
        ++ptr_;
        return old;
    }

    constexpr negation_validating_iterator& operator--() noexcept {
        --ptr_;
        return *this;
    }
    constexpr negation_validating_iterator operator--(int) noexcept {
        auto old = *this;
        --ptr_;
        return old;
    }

    constexpr negation_validating_iterator& operator+=(const ptrdiff_t n) noexcept {
        ptr_ += n;
        return *this;
    }

    constexpr negation_validating_iterator& operator-=(const ptrdiff_t n) noexcept {
        ptr_ -= n;
        return *this;
    }

    constexpr int& operator[](const ptrdiff_t n) const noexcept {
        return ptr_[n];
    }

    friend constexpr negation_validating_iterator operator+(
        const negation_validating_iterator i, const ptrdiff_t n) noexcept {
        return {i.ptr_ + n};
    }
    friend constexpr negation_validating_iterator operator+(
        const ptrdiff_t n, const negation_validating_iterator i) noexcept {
        return {i.ptr_ + n};
    }
    friend constexpr negation_validating_iterator operator-(
        const negation_validating_iterator i, const ptrdiff_t n) noexcept {
        return {i.ptr_ - n};
    }
    friend constexpr ptrdiff_t operator-(
        const negation_validating_iterator i, const negation_validating_iterator j) noexcept {
        return i.ptr_ - j.ptr_;
    }

    friend constexpr boolean_testable_result operator==(
        const negation_validating_iterator i, const negation_validating_iterator j) noexcept {
        return {i.ptr_ == j.ptr_};
    }

    friend constexpr boolean_testable_result operator!=(
        const negation_validating_iterator i, const negation_validating_iterator j) noexcept {
        return {i.ptr_ != j.ptr_};
    }

    friend constexpr boolean_testable_result operator<(
        const negation_validating_iterator i, const negation_validating_iterator j) noexcept {
        return {i.ptr_ < j.ptr_};
    }

    friend constexpr boolean_testable_result operator>(
        const negation_validating_iterator i, const negation_validating_iterator j) noexcept {
        return {i.ptr_ > j.ptr_};
    }

    friend constexpr boolean_testable_result operator<=(
        const negation_validating_iterator i, const negation_validating_iterator j) noexcept {
        return {i.ptr_ <= j.ptr_};
    }

    friend constexpr boolean_testable_result operator>=(
        const negation_validating_iterator i, const negation_validating_iterator j) noexcept {
        return {i.ptr_ >= j.ptr_};
    }

    int* ptr_;
};

struct validating_negation_view {
    constexpr negation_validating_iterator begin() const noexcept {
        return {begin_};
    }

    constexpr negation_validating_iterator end() const noexcept {
        return {end_};
    }

    int* begin_;
    int* end_;
};

template <>
inline constexpr bool ranges::enable_view<validating_negation_view> = true;

template <>
inline constexpr bool ranges::enable_borrowed_range<validating_negation_view> = true;

template <class V>
void test_adl_proof_random_access_views_comparison(V&& v) {
    auto i = v.begin();
    auto j = v.end();

    (void) (i == j);
    (void) (i != j);
    (void) (i < j);
    (void) (i > j);
    (void) (i <= j);
    (void) (i >= j);
}

template <class V>
void test_adl_proof_basic_views_comparison(V&& v) {
    auto it = v.begin();
    auto se = v.end();

    (void) (it == se);
    (void) (it != se);
}

void test_adl_proof_views_comparison() {
    constexpr auto simple_truth = [](const auto&) { return true; };

    int iarr[1]{};
    const validating_common_view validating_common_view_val{iarr, iarr + 1};
    const validating_noncommon_view validating_noncommon_view_val{iarr, iarr + 1};
    const validating_negation_view validating_negation_view_val{iarr, iarr + 1};
    const validating_noncommon_to_join_view validating_view_to_join{
        &validating_noncommon_view_val, &validating_noncommon_view_val + 1};

    test_adl_proof_basic_views_comparison(validating_common_view_val | views::filter(simple_truth));
    test_adl_proof_basic_views_comparison(validating_negation_view_val | views::filter(simple_truth));
    test_adl_proof_basic_views_comparison(validating_noncommon_view_val | views::filter(simple_truth));

    test_adl_proof_basic_views_comparison(validating_view_to_join | views::join);

    test_adl_proof_basic_views_comparison(validating_common_view_val | views::split(validating_common_view_val));
    test_adl_proof_basic_views_comparison(validating_negation_view_val | views::split(validating_negation_view_val));
    test_adl_proof_basic_views_comparison(validating_noncommon_view_val | views::split(validating_noncommon_view_val));

    test_adl_proof_basic_views_comparison(validating_common_view_val | views::lazy_split(validating_common_view_val));
    test_adl_proof_basic_views_comparison(
        validating_negation_view_val | views::lazy_split(validating_negation_view_val));
    test_adl_proof_basic_views_comparison(
        validating_noncommon_view_val | views::lazy_split(validating_noncommon_view_val));

#if _HAS_CXX23
    test_adl_proof_basic_views_comparison(validating_view_to_join | views::join_with(validating_noncommon_view_val));

    test_adl_proof_random_access_views_comparison(validating_common_view_val | views::enumerate);
    test_adl_proof_random_access_views_comparison(validating_negation_view_val | views::enumerate);
    test_adl_proof_basic_views_comparison(validating_noncommon_view_val | views::enumerate);

    test_adl_proof_random_access_views_comparison(validating_common_view_val | views::chunk(1));
    test_adl_proof_random_access_views_comparison(validating_negation_view_val | views::chunk(1));
    test_adl_proof_basic_views_comparison(validating_noncommon_view_val | views::chunk(1));

    test_adl_proof_random_access_views_comparison(validating_common_view_val | views::slide(1));
    test_adl_proof_random_access_views_comparison(validating_negation_view_val | views::slide(1));
    test_adl_proof_basic_views_comparison(validating_noncommon_view_val | views::slide(1));

    test_adl_proof_basic_views_comparison(validating_common_view_val | views::chunk_by(ranges::equal_to{}));
    test_adl_proof_basic_views_comparison(validating_negation_view_val | views::chunk_by(ranges::equal_to{}));
    test_adl_proof_basic_views_comparison(validating_noncommon_view_val | views::chunk_by(ranges::equal_to{}));

    test_adl_proof_random_access_views_comparison(validating_common_view_val | views::stride(1));
    test_adl_proof_random_access_views_comparison(validating_negation_view_val | views::stride(1));
    test_adl_proof_basic_views_comparison(validating_noncommon_view_val | views::stride(1));

    test_adl_proof_random_access_views_comparison(validating_common_view_val | views::adjacent<1>);
    test_adl_proof_random_access_views_comparison(validating_negation_view_val | views::adjacent<1>);
    test_adl_proof_basic_views_comparison(validating_noncommon_view_val | views::adjacent<1>);
#endif // _HAS_CXX23
}
#endif // ^^^ no workaround ^^^
