// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef _M_CEE // TRANSITION, VSO-1659496
#include <algorithm>
#include <cstddef>
#if _HAS_CXX17
#include <execution>
#endif // _HAS_CXX17
#include <memory>
#include <new>
#include <numeric>
#include <type_traits>
#include <utility>

template <class Tag>
struct tagged_truth {
    template <class T>
    constexpr bool operator()(T&&) const noexcept {
        return true;
    }
};

template <class Tag>
struct tagged_equal {
    template <class T, class U>
    constexpr auto operator()(T&& t, U&& u) const -> decltype(std::forward<T>(t) == std::forward<U>(u)) {
        return std::forward<T>(t) == std::forward<U>(u);
    }
};

template <class Tag>
struct tagged_less {
    template <class T, class U>
    constexpr auto operator()(T&& t, U&& u) const -> decltype(std::forward<T>(t) < std::forward<U>(u)) {
        return std::forward<T>(t) < std::forward<U>(u);
    }
};

template <class Tag>
struct tagged_zero_equality {
    template <class T>
    constexpr auto operator()(T&& t) const //
        -> decltype(std::forward<T>(t) == std::remove_cv_t<std::remove_reference_t<T>>{}) {
        return std::forward<T>(t) == std::remove_cv_t<std::remove_reference_t<T>>{};
    }
};

#if _HAS_CXX20
template <class Tag>
struct tagged_compare_three_way {
    template <class T, class U>
    constexpr auto operator()(T&& t, U&& u) const -> decltype(std::forward<T>(t) <=> std::forward<U>(u)) {
        return std::forward<T>(t) <=> std::forward<U>(u);
    }
};
#endif // _HAS_CXX20

template <class Tag>
struct tagged_identity {
    template <class T>
    constexpr T&& operator()(T&& t) const noexcept {
        return std::forward<T>(t);
    }
};

template <class Tag>
struct tagged_urng {
    using result_type = unsigned int;

    static constexpr result_type max() noexcept {
        return static_cast<result_type>(-1);
    }

    static constexpr result_type min() noexcept {
        return 0;
    }

    result_type operator()() noexcept {
        return value_++;
    }

    result_type value_{};
};

template <class T>
struct tagged_nontrivial {
    tagged_nontrivial() noexcept {}
    tagged_nontrivial(const tagged_nontrivial&) noexcept {}
    tagged_nontrivial(tagged_nontrivial&&) noexcept {}

    tagged_nontrivial& operator=(const tagged_nontrivial&) noexcept {
        return *this;
    }
    tagged_nontrivial& operator=(tagged_nontrivial&&) noexcept {
        return *this;
    }

    ~tagged_nontrivial() noexcept {}
};

template <class Tag>
struct tagged_left_selector {
    template <class T>
    constexpr T operator()(T lhs, T) const noexcept {
        return lhs;
    }
};

template <class T>
struct holder {
    T t;
};

struct incomplete;

using simple_truth         = tagged_truth<void>;
using simple_identity      = tagged_identity<void>;
using simple_left_selector = tagged_left_selector<void>;
using simple_zero_equality = tagged_zero_equality<void>;
using simple_urng          = tagged_urng<void>;

using validator                = holder<incomplete>*;
using validating_truth         = tagged_truth<holder<incomplete>>;
using validating_equal         = tagged_equal<holder<incomplete>>;
using validating_less          = tagged_less<holder<incomplete>>;
using validating_identity      = tagged_identity<holder<incomplete>>;
using validating_left_selector = tagged_left_selector<holder<incomplete>>;
using validating_zero_equality = tagged_zero_equality<holder<incomplete>>;
using validating_urng          = tagged_urng<holder<incomplete>>;
using validating_nontrivial    = tagged_nontrivial<holder<incomplete>>;

#if _HAS_CXX20
using validating_compare_three_way = tagged_compare_three_way<holder<incomplete>>;
#endif // _HAS_CXX20

template <class T>
struct value_generator {
    T operator()() const {
        return T{};
    }
};

void test_algorithms() {
    int iarr[1]{};
    validator varr[1]{};

    (void) std::all_of(varr, varr, simple_truth{});
    (void) std::all_of(iarr, iarr, validating_truth{});

    (void) std::any_of(varr, varr, simple_truth{});
    (void) std::any_of(iarr, iarr, validating_truth{});

    (void) std::none_of(varr, varr, simple_truth{});
    (void) std::none_of(iarr, iarr, validating_truth{});

    (void) std::for_each(varr, varr, simple_truth{});
    (void) std::for_each(varr, varr, validating_truth{});
    (void) std::for_each(iarr, iarr, validating_truth{});

#if _HAS_CXX17
    (void) std::for_each_n(varr, 0, simple_truth{});
    (void) std::for_each_n(varr, 0, validating_truth{});
    (void) std::for_each_n(iarr, 0, validating_truth{});
#endif // _HAS_CXX17

    (void) std::find(varr, varr, validator{});

    (void) std::find_if(varr, varr, simple_truth{});
    (void) std::find_if(iarr, iarr, validating_truth{});

    (void) std::find_if_not(varr, varr, simple_truth{});
    (void) std::find_if_not(iarr, iarr, validating_truth{});

    (void) std::find_end(varr, varr, varr, varr);
    (void) std::find_end(varr, varr, varr, varr, validating_equal{});
    (void) std::find_end(iarr, iarr, iarr, iarr, validating_equal{});

    (void) std::find_first_of(varr, varr, varr, varr);
    (void) std::find_first_of(varr, varr, varr, varr, validating_equal{});
    (void) std::find_first_of(iarr, iarr, iarr, iarr, validating_equal{});

    (void) std::adjacent_find(varr, varr);
    (void) std::adjacent_find(iarr, iarr, validating_equal{});

    (void) std::count(varr, varr, validator{});

    (void) std::count_if(varr, varr, simple_truth{});
    (void) std::count_if(iarr, iarr, validating_truth{});

    (void) std::mismatch(varr, varr, varr);
    (void) std::mismatch(varr, varr, varr, varr);
    (void) std::mismatch(iarr, iarr, iarr, validating_equal{});
    (void) std::mismatch(iarr, iarr, iarr, iarr, validating_equal{});

    (void) std::equal(varr, varr, varr);
    (void) std::equal(varr, varr, varr, varr);
    (void) std::equal(iarr, iarr, iarr, validating_equal{});
    (void) std::equal(iarr, iarr, iarr, iarr, validating_equal{});

    (void) std::is_permutation(varr, varr, varr);
    (void) std::is_permutation(varr, varr, varr, validating_equal{});
    (void) std::is_permutation(varr, varr, varr, varr);
    (void) std::is_permutation(varr, varr, varr, varr, validating_equal{});
    (void) std::is_permutation(iarr, iarr, iarr, validating_equal{});
    (void) std::is_permutation(iarr, iarr, iarr, iarr, validating_equal{});

    (void) std::search(varr, varr, varr, varr);
    (void) std::search(iarr, iarr, iarr, iarr, validating_equal{});

    (void) std::search_n(varr, varr, 0, validator{});
    (void) std::search_n(iarr, iarr, 0, 0, validating_equal{});

    int iarr2[1]{};
    validator varr2[1]{};

    (void) std::copy(varr, varr + 1, varr2);

    (void) std::copy_n(varr, 1, varr2);

    (void) std::copy_if(varr, varr + 1, varr2, simple_truth{});
    (void) std::copy_if(iarr, iarr + 1, iarr2, validating_truth{});

    (void) std::copy_backward(varr, varr + 1, varr2 + 1);

    (void) std::move(varr, varr + 1, varr2);

    (void) std::move_backward(varr, varr + 1, varr2 + 1);

    // (void) std::swap_ranges(varr, varr, varr2); // requires Cpp17ValueSwappable

    // std::iter_swap(varr, varr2); // requires Cpp17ValueSwappable

    (void) std::transform(varr, varr, varr2, simple_identity{});
    (void) std::transform(varr, varr, varr, varr2, simple_left_selector{});
    (void) std::transform(iarr, iarr, iarr2, validating_identity{});
    (void) std::transform(iarr, iarr, iarr, iarr2, validating_left_selector{});

    std::replace(varr, varr, validator{}, validator{});

    std::replace_if(varr, varr, simple_truth{}, validator{});
    std::replace_if(iarr, iarr, validating_truth{}, 0);

    (void) std::replace_copy(varr, varr, varr2, validator{}, validator{});

    (void) std::replace_copy_if(varr, varr, varr2, simple_truth{}, validator{});
    (void) std::replace_copy_if(iarr, iarr, iarr2, validating_truth{}, 0);

    std::fill(varr, varr, validator{});

    (void) std::fill_n(varr, 0, validator{});

    std::generate(varr, varr, value_generator<validator>{});

    (void) std::generate_n(varr, 0, value_generator<validator>{});

    (void) std::remove(varr, varr, validator{});

    (void) std::remove_if(varr, varr, simple_truth{});
    (void) std::remove_if(iarr, iarr, validating_truth{});

    (void) std::remove_copy(varr, varr, varr2, validator{});

    (void) std::remove_copy_if(varr, varr, varr2, simple_truth{});
    (void) std::remove_copy_if(iarr, iarr, iarr2, validating_truth{});

    (void) std::unique(varr, varr);
    (void) std::unique(iarr, iarr, validating_equal{});

    (void) std::unique_copy(varr, varr, varr2);
    (void) std::unique_copy(iarr, iarr, iarr2, validating_equal{});

    // (void) std::reverse(varr, varr); // requires Cpp17ValueSwappable

    (void) std::reverse_copy(varr, varr, varr2);

    // (void) std::rotate(varr, varr, varr); // requires Cpp17ValueSwappable

    (void) std::rotate_copy(varr, varr, varr, varr2);

#if _HAS_CXX17
    (void) std::sample(varr, varr, varr2, 0, simple_urng{});
    (void) std::sample(iarr, iarr, iarr2, 0, validating_urng{});
#endif // _HAS_CXX17

    // std::shuffle(varr, varr, simple_urng{}); // requires Cpp17ValueSwappable
    std::shuffle(iarr, iarr, validating_urng{});

    // std::random_shuffle (removed in C++17) also requires Cpp17ValueSwappable

#if _HAS_CXX20
    (void) std::shift_left(varr, varr, 0);

    // (void) std::shift_right(varr, varr, 0); // requires Cpp17ValueSwappable
#endif // _HAS_CXX20

    // std::sort(varr, varr); // requires Cpp17ValueSwappable
    std::sort(iarr, iarr, validating_less{});

    // std::stable_sort(varr, varr); // requires Cpp17ValueSwappable
    std::stable_sort(iarr, iarr, validating_less{});

    std::partial_sort(varr, varr, varr); // requires Cpp17ValueSwappable
    std::partial_sort(iarr, iarr, iarr, validating_less{});

    void* vparr[1]{};

    (void) std::partial_sort_copy(varr, varr, varr2, varr2); // requires Cpp17ValueSwappable
    (void) std::partial_sort_copy(varr, varr, vparr, vparr);
    (void) std::partial_sort_copy(iarr, iarr, iarr2, iarr2, validating_less{});

    (void) std::is_sorted(varr, varr);
    (void) std::is_sorted(iarr, iarr, validating_less{});

    (void) std::is_sorted_until(varr, varr);
    (void) std::is_sorted_until(iarr, iarr, validating_less{});

    // std::nth_element(varr, varr, varr); // requires Cpp17ValueSwappable
    std::nth_element(iarr, iarr, iarr, validating_less{});

    (void) std::lower_bound(varr, varr, validator{});
    (void) std::lower_bound(iarr, iarr, 0, validating_less{});

    (void) std::upper_bound(varr, varr, validator{});
    (void) std::upper_bound(iarr, iarr, 0, validating_less{});

    (void) std::equal_range(varr, varr, validator{});
    (void) std::equal_range(iarr, iarr, 0, validating_less{});

    (void) std::binary_search(varr, varr, validator{});
    (void) std::binary_search(iarr, iarr, 0, validating_less{});

    (void) std::is_partitioned(varr, varr, simple_zero_equality{});
    (void) std::is_partitioned(iarr, iarr, validating_zero_equality{});

    // (void) std::partition(varr, varr, simple_zero_equality{}); // requires Cpp17ValueSwappable
    (void) std::partition(iarr, iarr, validating_zero_equality{});

    // (void) std::stable_partition(varr, varr, simple_zero_equality{}); // requires Cpp17ValueSwappable
    (void) std::stable_partition(iarr, iarr, validating_zero_equality{});

    int iarr3[1]{};
    validator varr3[1]{};

    (void) std::partition_copy(varr, varr, varr2, varr3, simple_zero_equality{});
    (void) std::partition_copy(iarr, iarr, iarr2, iarr3, validating_zero_equality{});

    (void) std::partition_point(varr, varr, simple_zero_equality{});
    (void) std::partition_point(iarr, iarr, validating_zero_equality{});

    (void) std::merge(varr, varr, varr2, varr2, varr3);
    (void) std::merge(iarr, iarr, iarr2, iarr2, iarr3, validating_less{});

    // std::inplace_merge(varr, varr, varr); // requires Cpp17ValueSwappable
    std::inplace_merge(iarr, iarr, iarr, validating_less{});

    (void) std::includes(varr, varr, varr, varr);
    (void) std::includes(iarr, iarr, iarr, iarr, validating_less{});

    (void) std::set_union(varr, varr, varr, varr, varr3);
    (void) std::set_union(iarr, iarr, iarr, iarr, iarr3, validating_less{});

    (void) std::set_intersection(varr, varr, varr, varr, varr3);
    (void) std::set_intersection(iarr, iarr, iarr, iarr, iarr3, validating_less{});

    (void) std::set_difference(varr, varr, varr, varr, varr3);
    (void) std::set_difference(iarr, iarr, iarr, iarr, iarr3, validating_less{});

    (void) std::set_symmetric_difference(varr, varr, varr, varr, varr3);
    (void) std::set_symmetric_difference(iarr, iarr, iarr, iarr, iarr3, validating_less{});

    std::push_heap(varr3, varr3 + 1); // requires Cpp17ValueSwappable
    std::push_heap(iarr3, iarr3 + 1, validating_less{});

    std::pop_heap(varr3, varr3 + 1); // requires Cpp17ValueSwappable
    std::pop_heap(iarr3, iarr3 + 1, validating_less{});

    std::make_heap(varr3, varr3 + 1); // requires Cpp17ValueSwappable
    std::make_heap(iarr3, iarr3 + 1, validating_less{});

    std::sort_heap(varr3, varr3 + 1); // requires Cpp17ValueSwappable
    std::sort_heap(iarr3, iarr3 + 1, validating_less{});

    (void) std::is_heap(varr3, varr3 + 1);
    (void) std::is_heap(iarr3, iarr3 + 1, validating_less{});

    (void) std::is_heap_until(varr3, varr3 + 1);
    (void) std::is_heap_until(iarr3, iarr3 + 1, validating_less{});

    (void) std::min(+varr, +varr);
    (void) std::min(+iarr, +iarr, validating_less{});
    (void) std::min({+varr, +varr});
    (void) std::min({+iarr, +iarr}, validating_less{});

    (void) std::max(+varr, +varr);
    (void) std::max(+iarr, +iarr, validating_less{});
    (void) std::max({+varr, +varr});
    (void) std::max({+iarr, +iarr}, validating_less{});

    (void) std::minmax(+varr, +varr);
    (void) std::minmax(+iarr, +iarr, validating_less{});
    (void) std::minmax({+varr, +varr});
    (void) std::minmax({+iarr, +iarr}, validating_less{});

    (void) std::min_element(varr, varr + 1);
    (void) std::min_element(iarr, iarr + 1, validating_less{});

    (void) std::max_element(varr, varr + 1);
    (void) std::max_element(iarr, iarr + 1, validating_less{});

    (void) std::minmax_element(varr, varr + 1);
    (void) std::minmax_element(iarr, iarr + 1, validating_less{});

#if _HAS_CXX17
    (void) std::clamp(+varr, +varr, +varr);
    (void) std::clamp(+iarr, +iarr, +iarr, validating_less{});
#endif // _HAS_CXX17

    (void) std::lexicographical_compare(varr, varr, varr, varr);
    (void) std::lexicographical_compare(iarr, iarr, iarr, iarr, validating_less{});

#if _HAS_CXX20
    (void) std::lexicographical_compare_three_way(varr, varr, varr, varr);
    (void) std::lexicographical_compare_three_way(iarr, iarr, iarr, iarr, validating_compare_three_way{});
#endif // _HAS_CXX20

    // (void) std::next_permutation(varr, varr); // requires Cpp17ValueSwappable
    (void) std::next_permutation(iarr, iarr, validating_less{});

    // (void) std::prev_permutation(varr, varr); // requires Cpp17ValueSwappable
    (void) std::prev_permutation(iarr, iarr, validating_less{});

    (void) std::accumulate(varr, varr, validator{}, simple_left_selector{});
    (void) std::accumulate(iarr, iarr, 0, validating_left_selector{});

#if _HAS_CXX17
    (void) std::reduce(varr, varr, validator{}, simple_left_selector{});
    (void) std::reduce(iarr, iarr, 0, validating_left_selector{});
#endif // _HAS_CXX17

    (void) std::inner_product(varr, varr, varr, validator{}, simple_left_selector{}, simple_left_selector{});
    (void) std::inner_product(iarr, iarr, iarr, 0, validating_left_selector{}, validating_left_selector{});

#if _HAS_CXX17
    (void) std::transform_reduce(varr, varr, varr, validator{}, simple_left_selector{}, simple_left_selector{});
    (void) std::transform_reduce(iarr, iarr, iarr, 0, validating_left_selector{}, validating_left_selector{});
    (void) std::transform_reduce(varr, varr, validator{}, simple_left_selector{}, simple_identity{});
    (void) std::transform_reduce(iarr, iarr, 0, validating_left_selector{}, simple_identity{});
#endif // _HAS_CXX17

    (void) std::partial_sum(varr, varr, varr2, simple_left_selector{});
    (void) std::partial_sum(iarr, iarr, iarr2, validating_left_selector{});

#if _HAS_CXX17
    (void) std::exclusive_scan(varr, varr, varr2, validator{}, simple_left_selector{});
    (void) std::exclusive_scan(iarr, iarr, iarr2, 0, validating_left_selector{});

    (void) std::inclusive_scan(varr, varr, varr2, simple_left_selector{});
    (void) std::inclusive_scan(iarr, iarr, iarr2, validating_left_selector{});
    (void) std::inclusive_scan(varr, varr, varr2, simple_left_selector{}, validator{});
    (void) std::inclusive_scan(iarr, iarr, iarr2, validating_left_selector{}, 0);

    (void) std::transform_exclusive_scan(varr, varr, varr2, validator{}, simple_left_selector{}, simple_identity{});
    (void) std::transform_exclusive_scan(iarr, iarr, iarr2, 0, validating_left_selector{}, validating_identity{});

    (void) std::transform_inclusive_scan(varr, varr, varr2, simple_left_selector{}, simple_identity{});
    (void) std::transform_inclusive_scan(iarr, iarr, iarr2, validating_left_selector{}, validating_identity{});
    (void) std::transform_inclusive_scan(varr, varr, varr2, simple_left_selector{}, simple_identity{}, validator{});
    (void) std::transform_inclusive_scan(iarr, iarr, iarr2, validating_left_selector{}, validating_identity{}, 0);
#endif // _HAS_CXX17

    (void) std::adjacent_difference(varr, varr, varr2, simple_left_selector{});
    (void) std::adjacent_difference(iarr, iarr, iarr2, validating_left_selector{});

    validator* pvarr[1]{};
    std::iota(pvarr, pvarr, +varr);

#if _HAS_CXX20
    (void) std::midpoint(+varr, +varr);
#endif // _HAS_CXX20

    validating_nontrivial narr[1]{};
    validating_nontrivial narr2[1]{};

#if _HAS_CXX17
    std::uninitialized_default_construct(varr, varr);
    std::uninitialized_default_construct(narr, narr);

    (void) std::uninitialized_default_construct_n(varr, 0);
    (void) std::uninitialized_default_construct_n(narr, 0);

    std::uninitialized_value_construct(varr, varr);
    std::uninitialized_value_construct(narr, narr);

    (void) std::uninitialized_value_construct_n(varr, 0);
    (void) std::uninitialized_value_construct_n(narr, 0);
#endif // _HAS_CXX17

    (void) std::uninitialized_copy(varr, varr, varr2);
    (void) std::uninitialized_copy(narr, narr, narr2);

    (void) std::uninitialized_copy_n(varr, 0, varr2);
    (void) std::uninitialized_copy_n(narr, 0, narr2);

#if _HAS_CXX17
    (void) std::uninitialized_move(varr, varr, varr2);
    (void) std::uninitialized_move(narr, narr, narr2);

    (void) std::uninitialized_move_n(varr, 0, varr2);
    (void) std::uninitialized_move_n(narr, 0, narr2);
#endif // _HAS_CXX17

    std::uninitialized_fill(varr, varr, validator{});
    std::uninitialized_fill(narr, narr, validating_nontrivial{});

    (void) std::uninitialized_fill_n(varr, 0, validator{});
    (void) std::uninitialized_fill_n(narr, 0, validating_nontrivial{});

#if _HAS_CXX20
    {
        validator vx{};
        validating_nontrivial nx{};

        std::construct_at(&vx);
        std::construct_at(std::addressof(nx));
    }
#endif // _HAS_CXX20

#if _HAS_CXX17
    alignas(validator) alignas(
        validating_nontrivial) unsigned char buffer[std::max(sizeof(validator), sizeof(validating_nontrivial))]{};

    std::destroy_at(new (buffer) validator{});
    std::destroy_at(new (buffer) validating_nontrivial{});

#if _HAS_CXX20
    {
        alignas(validator[1]) unsigned char buf[sizeof(validator[1])];
        ::new (static_cast<void*>(buf)) validator[1]{};
        const auto p_arr = std::launder(reinterpret_cast<validator(*)[1]>(buf));

        std::destroy(p_arr, p_arr);

        (void) std::destroy_n(p_arr, 0);

        std::destroy_at(p_arr);
    }
    {
        alignas(validating_nontrivial[1]) unsigned char buf[sizeof(validating_nontrivial[1])];
        ::new (static_cast<void*>(buf)) validating_nontrivial[1]{};
        const auto p_arr = std::launder(reinterpret_cast<validating_nontrivial(*)[1]>(buf));

        std::destroy(p_arr, p_arr);

        (void) std::destroy_n(p_arr, 0);

        std::destroy_at(p_arr);
    }
#endif // _HAS_CXX20

    std::destroy(varr, varr);
    std::destroy(narr, narr);

    (void) std::destroy_n(varr, 0);
    (void) std::destroy_n(narr, 0);
#endif // _HAS_CXX17
}

#if _HAS_CXX17
template <auto& ExecutionPolicy>
void test_per_execution_policy() {
    int iarr[1]{};
    validator varr[1]{};

    (void) std::all_of(ExecutionPolicy, varr, varr, simple_truth{});
    (void) std::all_of(ExecutionPolicy, iarr, iarr, validating_truth{});

    (void) std::any_of(ExecutionPolicy, varr, varr, simple_truth{});
    (void) std::any_of(ExecutionPolicy, iarr, iarr, validating_truth{});

    (void) std::none_of(ExecutionPolicy, varr, varr, simple_truth{});
    (void) std::none_of(ExecutionPolicy, iarr, iarr, validating_truth{});

    std::for_each(ExecutionPolicy, varr, varr, simple_truth{});
    std::for_each(ExecutionPolicy, varr, varr, validating_truth{});
    std::for_each(ExecutionPolicy, iarr, iarr, validating_truth{});

    (void) std::for_each_n(ExecutionPolicy, varr, 0, simple_truth{});
    (void) std::for_each_n(ExecutionPolicy, varr, 0, validating_truth{});
    (void) std::for_each_n(ExecutionPolicy, iarr, 0, validating_truth{});

    (void) std::find(ExecutionPolicy, varr, varr, validator{});

    (void) std::find_if(ExecutionPolicy, varr, varr, simple_truth{});
    (void) std::find_if(ExecutionPolicy, iarr, iarr, validating_truth{});

    (void) std::find_if_not(ExecutionPolicy, varr, varr, simple_truth{});
    (void) std::find_if_not(ExecutionPolicy, iarr, iarr, validating_truth{});

    (void) std::find_end(ExecutionPolicy, varr, varr, varr, varr);
    (void) std::find_end(ExecutionPolicy, varr, varr, varr, varr, validating_equal{});
    (void) std::find_end(ExecutionPolicy, iarr, iarr, iarr, iarr, validating_equal{});

    (void) std::find_first_of(ExecutionPolicy, varr, varr, varr, varr);
    (void) std::find_first_of(ExecutionPolicy, varr, varr, varr, varr, validating_equal{});
    (void) std::find_first_of(ExecutionPolicy, iarr, iarr, iarr, iarr, validating_equal{});

    (void) std::adjacent_find(ExecutionPolicy, varr, varr);
    (void) std::adjacent_find(ExecutionPolicy, iarr, iarr, validating_equal{});

    (void) std::count(ExecutionPolicy, varr, varr, validator{});

    (void) std::count_if(ExecutionPolicy, varr, varr, simple_truth{});
    (void) std::count_if(ExecutionPolicy, iarr, iarr, validating_truth{});

    (void) std::mismatch(ExecutionPolicy, varr, varr, varr);
    (void) std::mismatch(ExecutionPolicy, varr, varr, varr, varr);
    (void) std::mismatch(ExecutionPolicy, iarr, iarr, iarr, validating_equal{});
    (void) std::mismatch(ExecutionPolicy, iarr, iarr, iarr, iarr, validating_equal{});

    (void) std::equal(ExecutionPolicy, varr, varr, varr);
    (void) std::equal(ExecutionPolicy, varr, varr, varr, varr);
    (void) std::equal(ExecutionPolicy, iarr, iarr, iarr, validating_equal{});
    (void) std::equal(ExecutionPolicy, iarr, iarr, iarr, iarr, validating_equal{});

    (void) std::search(ExecutionPolicy, varr, varr, varr, varr);
    (void) std::search(ExecutionPolicy, iarr, iarr, iarr, iarr, validating_equal{});

    (void) std::search_n(ExecutionPolicy, varr, varr, 0, validator{});
    (void) std::search_n(ExecutionPolicy, iarr, iarr, 0, 0, validating_equal{});

    int iarr2[1]{};
    validator varr2[1]{};

    (void) std::copy(ExecutionPolicy, varr, varr + 1, varr2);

    (void) std::copy_n(ExecutionPolicy, varr, 1, varr2);

    (void) std::copy_if(ExecutionPolicy, varr, varr + 1, varr2, simple_truth{});
    (void) std::copy_if(ExecutionPolicy, iarr, iarr + 1, iarr2, validating_truth{});

    (void) std::move(ExecutionPolicy, varr, varr + 1, varr2);

    // (void) std::swap_ranges(ExecutionPolicy, varr, varr, varr2);  // requires Cpp17ValueSwappable

    (void) std::transform(ExecutionPolicy, varr, varr, varr2, simple_identity{});
    (void) std::transform(ExecutionPolicy, varr, varr, varr, varr2, simple_left_selector{});
    (void) std::transform(ExecutionPolicy, iarr, iarr, iarr2, validating_identity{});
    (void) std::transform(ExecutionPolicy, iarr, iarr, iarr, iarr2, validating_left_selector{});

    std::replace(ExecutionPolicy, varr, varr, validator{}, validator{});

    std::replace_if(ExecutionPolicy, varr, varr, simple_truth{}, validator{});
    std::replace_if(ExecutionPolicy, iarr, iarr, validating_truth{}, 0);

    (void) std::replace_copy(ExecutionPolicy, varr, varr, varr2, validator{}, validator{});

    (void) std::replace_copy_if(ExecutionPolicy, varr, varr, varr2, simple_truth{}, validator{});
    (void) std::replace_copy_if(ExecutionPolicy, iarr, iarr, iarr2, validating_truth{}, 0);

    std::fill(ExecutionPolicy, varr, varr, validator{});

    (void) std::fill_n(ExecutionPolicy, varr, 0, validator{});

    std::generate(ExecutionPolicy, varr, varr, value_generator<validator>{});

    (void) std::generate_n(ExecutionPolicy, varr, 0, value_generator<validator>{});

    (void) std::remove(ExecutionPolicy, varr, varr, validator{});

    (void) std::remove_if(ExecutionPolicy, varr, varr, simple_truth{});
    (void) std::remove_if(ExecutionPolicy, iarr, iarr, validating_truth{});

    (void) std::remove_copy(ExecutionPolicy, varr, varr, varr2, validator{});

    (void) std::remove_copy_if(ExecutionPolicy, varr, varr, varr2, simple_truth{});
    (void) std::remove_copy_if(ExecutionPolicy, iarr, iarr, iarr2, validating_truth{});

    (void) std::unique(ExecutionPolicy, varr, varr);
    (void) std::unique(ExecutionPolicy, iarr, iarr, validating_equal{});

    (void) std::unique_copy(ExecutionPolicy, varr, varr, varr2);
    (void) std::unique_copy(ExecutionPolicy, iarr, iarr, iarr2, validating_equal{});

    // std::reverse(ExecutionPolicy, varr, varr); // requires Cpp17ValueSwappable

    (void) std::reverse_copy(ExecutionPolicy, varr, varr, varr2);

    // (void) std::rotate(ExecutionPolicy, varr, varr, varr); // requires Cpp17ValueSwappable

    (void) std::rotate_copy(ExecutionPolicy, varr, varr, varr, varr2);

#if _HAS_CXX20
    (void) std::shift_left(ExecutionPolicy, varr, varr, 0);

    // (void) std::shift_right(ExecutionPolicy, varr, varr, 0); // requires Cpp17ValueSwappable
#endif // _HAS_CXX20

    // std::sort(ExecutionPolicy, varr, varr); // requires Cpp17ValueSwappable
    std::sort(ExecutionPolicy, iarr, iarr, validating_less{});

    // std::stable_sort(ExecutionPolicy, varr, varr); // requires Cpp17ValueSwappable
    std::stable_sort(ExecutionPolicy, iarr, iarr, validating_less{});

    std::partial_sort(ExecutionPolicy, varr, varr, varr); // requires Cpp17ValueSwappable
    std::partial_sort(ExecutionPolicy, iarr, iarr, iarr, validating_less{});

    void* vparr[1]{};

    (void) std::partial_sort_copy(ExecutionPolicy, varr, varr, varr2, varr2); // requires Cpp17ValueSwappable
    (void) std::partial_sort_copy(ExecutionPolicy, varr, varr, vparr, vparr);
    (void) std::partial_sort_copy(ExecutionPolicy, iarr, iarr, iarr2, iarr2, validating_less{});

    (void) std::is_sorted(ExecutionPolicy, varr, varr);
    (void) std::is_sorted(ExecutionPolicy, iarr, iarr, validating_less{});

    (void) std::is_sorted_until(ExecutionPolicy, varr, varr);
    (void) std::is_sorted_until(ExecutionPolicy, iarr, iarr, validating_less{});

    // std::nth_element(ExecutionPolicy, varr, varr, varr); // requires Cpp17ValueSwappable
    std::nth_element(ExecutionPolicy, iarr, iarr, iarr, validating_less{});

    (void) std::is_partitioned(ExecutionPolicy, varr, varr, simple_zero_equality{});
    (void) std::is_partitioned(ExecutionPolicy, iarr, iarr, validating_zero_equality{});

    // (void) std::partition(ExecutionPolicy, varr, varr, simple_zero_equality{}); // requires Cpp17ValueSwappable
    (void) std::partition(ExecutionPolicy, iarr, iarr, validating_zero_equality{});

    // (void) std::stable_partition(
    //     ExecutionPolicy, varr, varr, simple_zero_equality{}); // requires Cpp17ValueSwappable
    (void) std::stable_partition(ExecutionPolicy, iarr, iarr, validating_zero_equality{});

    int iarr3[2]{};
    validator varr3[2]{};

    (void) std::partition_copy(ExecutionPolicy, varr, varr, varr2, varr3, simple_zero_equality{});
    (void) std::partition_copy(ExecutionPolicy, iarr, iarr, iarr2, iarr3, validating_zero_equality{});

    (void) std::merge(ExecutionPolicy, varr, varr, varr2, varr2, varr3);
    (void) std::merge(ExecutionPolicy, iarr, iarr, iarr2, iarr2, iarr3, validating_less{});

    // std::inplace_merge(ExecutionPolicy, varr, varr, varr); // requires Cpp17ValueSwappable
    std::inplace_merge(ExecutionPolicy, iarr, iarr, iarr, validating_less{});

    (void) std::includes(ExecutionPolicy, varr, varr, varr, varr);
    (void) std::includes(ExecutionPolicy, iarr, iarr, iarr, iarr, validating_less{});

    (void) std::set_union(ExecutionPolicy, varr, varr, varr, varr, varr3);
    (void) std::set_union(ExecutionPolicy, iarr, iarr, iarr, iarr, iarr3, validating_less{});

    (void) std::set_intersection(ExecutionPolicy, varr, varr, varr, varr, varr3);
    (void) std::set_intersection(ExecutionPolicy, iarr, iarr, iarr, iarr, iarr3, validating_less{});

    (void) std::set_difference(ExecutionPolicy, varr, varr, varr, varr, varr3);
    (void) std::set_difference(ExecutionPolicy, iarr, iarr, iarr, iarr, iarr3, validating_less{});

    (void) std::set_symmetric_difference(ExecutionPolicy, varr, varr, varr, varr, varr3);
    (void) std::set_symmetric_difference(ExecutionPolicy, iarr, iarr, iarr, iarr, iarr3, validating_less{});

    (void) std::is_heap(ExecutionPolicy, varr3, varr3 + 1);
    (void) std::is_heap(ExecutionPolicy, iarr3, iarr3 + 1, validating_less{});

    (void) std::is_heap_until(ExecutionPolicy, varr3, varr3 + 1);
    (void) std::is_heap_until(ExecutionPolicy, iarr3, iarr3 + 1, validating_less{});

    (void) std::min_element(ExecutionPolicy, varr, varr + 1);
    (void) std::min_element(ExecutionPolicy, iarr, iarr + 1, validating_less{});

    (void) std::max_element(ExecutionPolicy, varr, varr + 1);
    (void) std::max_element(ExecutionPolicy, iarr, iarr + 1, validating_less{});

    (void) std::minmax_element(ExecutionPolicy, varr, varr + 1);
    (void) std::minmax_element(ExecutionPolicy, iarr, iarr + 1, validating_less{});

    (void) std::lexicographical_compare(ExecutionPolicy, varr, varr, varr, varr);
    (void) std::lexicographical_compare(ExecutionPolicy, iarr, iarr, iarr, iarr, validating_less{});

    (void) std::reduce(ExecutionPolicy, varr, varr, validator{}, simple_left_selector{});
    (void) std::reduce(ExecutionPolicy, iarr, iarr, 0, simple_left_selector{});

    (void) std::transform_reduce(
        ExecutionPolicy, varr, varr, varr, validator{}, simple_left_selector{}, simple_left_selector{});
    (void) std::transform_reduce(
        ExecutionPolicy, iarr, iarr, iarr, 0, validating_left_selector{}, validating_left_selector{});
    (void) std::transform_reduce(ExecutionPolicy, varr, varr, validator{}, simple_left_selector{}, simple_identity{});
    (void) std::transform_reduce(ExecutionPolicy, iarr, iarr, 0, validating_left_selector{}, simple_identity{});

    (void) std::exclusive_scan(ExecutionPolicy, varr, varr, varr2, validator{}, simple_left_selector{});
    (void) std::exclusive_scan(ExecutionPolicy, iarr, iarr, iarr2, 0, validating_left_selector{});

    (void) std::inclusive_scan(ExecutionPolicy, varr, varr, varr2, simple_left_selector{});
    (void) std::inclusive_scan(ExecutionPolicy, iarr, iarr, iarr2, validating_left_selector{});
    (void) std::inclusive_scan(ExecutionPolicy, varr, varr, varr2, simple_left_selector{}, validator{});
    (void) std::inclusive_scan(ExecutionPolicy, iarr, iarr, iarr2, validating_left_selector{}, 0);

    (void) std::transform_exclusive_scan(
        ExecutionPolicy, varr, varr, varr2, validator{}, simple_left_selector{}, simple_identity{});
    (void) std::transform_exclusive_scan(
        ExecutionPolicy, iarr, iarr, iarr2, 0, validating_left_selector{}, validating_identity{});

    (void) std::transform_inclusive_scan(ExecutionPolicy, varr, varr, varr2, simple_left_selector{}, simple_identity{});
    (void) std::transform_inclusive_scan(
        ExecutionPolicy, iarr, iarr, iarr2, validating_left_selector{}, validating_identity{});
    (void) std::transform_inclusive_scan(
        ExecutionPolicy, varr, varr, varr2, simple_left_selector{}, simple_identity{}, validator{});
    (void) std::transform_inclusive_scan(
        ExecutionPolicy, iarr, iarr, iarr2, validating_left_selector{}, validating_identity{}, 0);

    (void) std::adjacent_difference(ExecutionPolicy, varr, varr, varr2, simple_left_selector{});
    (void) std::adjacent_difference(ExecutionPolicy, iarr, iarr, iarr2, validating_left_selector{});

    validating_nontrivial narr[1]{};
    validating_nontrivial narr2[1]{};

    std::uninitialized_default_construct(ExecutionPolicy, varr, varr);
    std::uninitialized_default_construct(ExecutionPolicy, narr, narr);

    (void) std::uninitialized_default_construct_n(ExecutionPolicy, varr, 0);
    (void) std::uninitialized_default_construct_n(ExecutionPolicy, narr, 0);

    std::uninitialized_value_construct(ExecutionPolicy, varr, varr);
    std::uninitialized_value_construct(ExecutionPolicy, narr, narr);

    (void) std::uninitialized_value_construct_n(ExecutionPolicy, varr, 0);
    (void) std::uninitialized_value_construct_n(ExecutionPolicy, narr, 0);

    (void) std::uninitialized_copy(ExecutionPolicy, varr, varr, varr2);
    (void) std::uninitialized_copy(ExecutionPolicy, narr, narr, narr2);

    (void) std::uninitialized_copy_n(ExecutionPolicy, varr, 0, varr2);
    (void) std::uninitialized_copy_n(ExecutionPolicy, narr, 0, narr2);

    (void) std::uninitialized_move(ExecutionPolicy, varr, varr, varr2);
    (void) std::uninitialized_move(ExecutionPolicy, narr, narr, narr2);

    (void) std::uninitialized_move_n(ExecutionPolicy, varr, 0, varr2);
    (void) std::uninitialized_move_n(ExecutionPolicy, narr, 0, narr2);

    std::uninitialized_fill(ExecutionPolicy, varr, varr, validator{});
    std::uninitialized_fill(ExecutionPolicy, narr, narr, validating_nontrivial{});

    (void) std::uninitialized_fill_n(ExecutionPolicy, varr, 0, validator{});
    (void) std::uninitialized_fill_n(ExecutionPolicy, narr, 0, validating_nontrivial{});

    std::destroy(ExecutionPolicy, varr, varr);
    std::destroy(ExecutionPolicy, narr, narr);

    (void) std::destroy_n(ExecutionPolicy, varr, 0);
    (void) std::destroy_n(ExecutionPolicy, narr, 0);

#if _HAS_CXX20
    validator varr_2d[1][1]{};
    validating_nontrivial narr_2d[1][1]{};

    std::destroy(ExecutionPolicy, varr_2d, varr_2d);
    std::destroy(ExecutionPolicy, narr_2d, narr_2d);

    (void) std::destroy_n(ExecutionPolicy, varr_2d, 0);
    (void) std::destroy_n(ExecutionPolicy, narr_2d, 0);
#endif // _HAS_CXX20
}

void test_parallel_algorithms() {
    test_per_execution_policy<std::execution::seq>();
    test_per_execution_policy<std::execution::par>();
    test_per_execution_policy<std::execution::par_unseq>();
#if _HAS_CXX20
    test_per_execution_policy<std::execution::unseq>();
#endif // _HAS_CXX20
}
#endif // _HAS_CXX17

#if _HAS_CXX20
void test_ranges_non_projected_algorithms() {
    using namespace std::ranges;

    int iarr[1]{};
    validator varr[1]{};

#if _HAS_CXX23
    (void) fold_left(varr, varr, validator{}, simple_left_selector{});
    (void) fold_left(varr, validator{}, simple_left_selector{});
    (void) fold_left(iarr, iarr, 0, validating_left_selector{});
    (void) fold_left(iarr, 0, validating_left_selector{});

    (void) fold_left_first(varr, varr + 1, simple_left_selector{});
    (void) fold_left_first(varr, simple_left_selector{});
    (void) fold_left_first(iarr, iarr + 1, validating_left_selector{});
    (void) fold_left_first(iarr, validating_left_selector{});

    (void) fold_right(varr, varr, validator{}, simple_left_selector{});
    (void) fold_right(varr, validator{}, simple_left_selector{});
    (void) fold_right(iarr, iarr, 0, validating_left_selector{});
    (void) fold_right(iarr, 0, validating_left_selector{});

    (void) fold_right_last(varr, varr + 1, simple_left_selector{});
    (void) fold_right_last(varr, simple_left_selector{});
    (void) fold_right_last(iarr, iarr + 1, validating_left_selector{});
    (void) fold_right_last(iarr, validating_left_selector{});

    (void) fold_left_with_iter(varr, varr, validator{}, simple_left_selector{});
    (void) fold_left_with_iter(varr, validator{}, simple_left_selector{});
    (void) fold_left_with_iter(iarr, iarr, 0, validating_left_selector{});
    (void) fold_left_with_iter(iarr, 0, validating_left_selector{});

    (void) fold_left_first_with_iter(varr, varr, simple_left_selector{});
    (void) fold_left_first_with_iter(varr, simple_left_selector{});
    (void) fold_left_first_with_iter(iarr, iarr, validating_left_selector{});
    (void) fold_left_first_with_iter(iarr, validating_left_selector{});
#endif // _HAS_CXX23

    int iarr2[1]{};
    validator varr2[1]{};

    (void) copy(varr, varr + 1, varr2);
    (void) copy(varr, varr2);

    (void) copy_n(varr, 1, varr2);

    (void) copy_backward(varr, varr + 1, varr2 + 1);
    (void) copy_backward(varr, varr2 + 1);

    (void) move(varr, varr + 1, varr2);
    (void) move(varr, varr2);

    (void) move_backward(varr, varr + 1, varr2 + 1);
    (void) move_backward(varr, varr2 + 1);

    (void) swap_ranges(varr, varr, varr2, varr2);
    (void) swap_ranges(varr, varr2);

    (void) fill(varr, varr, validator{});
    (void) fill(varr, validator{});

    (void) fill_n(varr, 0, validator{});

    (void) generate(varr, varr, value_generator<validator>{});
    (void) generate(varr, value_generator<validator>{});

    (void) generate_n(varr, 0, value_generator<validator>{});

    (void) reverse(varr, varr);
    (void) reverse(varr);

    (void) reverse_copy(varr, varr, varr2);
    (void) reverse_copy(varr, varr2);

    (void) rotate(varr, varr, varr);
    (void) rotate(varr, varr);

    (void) rotate_copy(varr, varr, varr, varr2);
    (void) rotate_copy(varr, varr, varr2);

    (void) sample(varr, varr, varr2, 0, simple_urng{});
    (void) sample(varr, varr2, 0, simple_urng{});
    (void) sample(iarr, iarr, iarr2, 0, validating_urng{});
    (void) sample(iarr, iarr2, 0, validating_urng{});

    (void) shuffle(varr, varr, simple_urng{});
    (void) shuffle(varr, simple_urng{});
    (void) shuffle(iarr, iarr, validating_urng{});
    (void) shuffle(iarr, validating_urng{});

#if _HAS_CXX23
    (void) shift_left(varr, varr, 0);
    (void) shift_left(varr, 0);

    (void) shift_right(varr, varr, 0);
    (void) shift_right(varr, 0);

    validator* pvarr[1]{};
    (void) iota(pvarr, pvarr, +varr);
    (void) iota(pvarr, +varr);
#endif // _HAS_CXX23

    validating_nontrivial narr[1]{};
    validating_nontrivial narr2[1]{};

    (void) uninitialized_default_construct(varr, varr);
    (void) uninitialized_default_construct(varr);
    (void) uninitialized_default_construct(narr, narr);
    (void) uninitialized_default_construct(narr);

    (void) uninitialized_default_construct_n(varr, 0);
    (void) uninitialized_default_construct_n(narr, 0);

    (void) uninitialized_value_construct(varr, varr);
    (void) uninitialized_value_construct(varr);
    (void) uninitialized_value_construct(narr, narr);
    (void) uninitialized_value_construct(narr);

    (void) uninitialized_value_construct_n(varr, 0);
    (void) uninitialized_value_construct_n(narr, 0);

    (void) uninitialized_copy(varr, varr, varr2, varr2);
    (void) uninitialized_copy(narr, narr, narr2, narr2);
    (void) uninitialized_copy(varr, varr2);
    (void) uninitialized_copy(narr, narr2);

    (void) uninitialized_copy_n(varr, 0, varr2, varr2);
    (void) uninitialized_copy_n(narr, 0, narr2, narr2);

    (void) uninitialized_move(varr, varr, varr2, varr2);
    (void) uninitialized_move(narr, narr, narr2, narr2);
    (void) uninitialized_move(varr, varr2);
    (void) uninitialized_move(narr, narr2);

    (void) uninitialized_move_n(varr, 0, varr2, varr2);
    (void) uninitialized_move_n(narr, 0, narr2, narr2);

    (void) uninitialized_fill(varr, varr, validator{});
    (void) uninitialized_fill(varr, validator{});
    (void) uninitialized_fill(narr, narr, validating_nontrivial{});
    (void) uninitialized_fill(narr, validating_nontrivial{});

    (void) uninitialized_fill_n(varr, 0, validator{});
    (void) uninitialized_fill_n(narr, 0, validating_nontrivial{});

    {
        alignas(validator) unsigned char buf[sizeof(validator)];
        const auto pv = construct_at(reinterpret_cast<validator*>(buf));
        destroy_at(pv);
    }
    {
        alignas(validating_nontrivial) unsigned char buf[sizeof(validating_nontrivial)];
        const auto pn = construct_at(reinterpret_cast<validating_nontrivial*>(buf));
        destroy_at(pn);
    }
    {
        alignas(validator[1]) unsigned char buf[sizeof(validator[1])];
        ::new (static_cast<void*>(buf)) validator[1]{};
        destroy_at(std::launder(reinterpret_cast<validator(*)[1]>(buf)));
    }
    {
        alignas(validating_nontrivial[1]) unsigned char buf[sizeof(validating_nontrivial[1])];
        ::new (static_cast<void*>(buf)) validating_nontrivial[1]{};
        destroy_at(std::launder(reinterpret_cast<validating_nontrivial(*)[1]>(buf)));
    }

    {
        alignas(validator[1]) unsigned char buf[sizeof(validator[1])];
        ::new (buf) validator[1];
        auto& arr = *std::launder(reinterpret_cast<validator(*)[1]>(buf));
        destroy(arr, arr);
        destroy(arr);
    }
    {
        alignas(validating_nontrivial[1]) unsigned char buf[sizeof(validating_nontrivial[1])];
        ::new (buf) validating_nontrivial[1];
        auto& arr = *std::launder(reinterpret_cast<validating_nontrivial(*)[1]>(buf));
        destroy(arr, arr);
        destroy(arr);
    }

    (void) destroy_n(varr, 0);
    (void) destroy_n(narr, 0);
}
#endif // _HAS_CXX20
#endif // ^^^ no workaround ^^^
