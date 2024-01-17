// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef _M_CEE // TRANSITION, VSO-1659496
#include <algorithm>
#include <cstddef>
#if _HAS_CXX17
#include <execution>
#endif // _HAS_CXX17
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
using simple_urng          = tagged_urng<void>;

using validator                = holder<incomplete>*;
using validating_truth         = tagged_truth<holder<incomplete>>;
using validating_equal         = tagged_equal<holder<incomplete>>;
using validating_less          = tagged_less<holder<incomplete>>;
using validating_identity      = tagged_identity<holder<incomplete>>;
using validating_left_selector = tagged_left_selector<holder<incomplete>>;
using validating_urng          = tagged_urng<holder<incomplete>>;

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
    // std::shuffle(iarr, iarr, validating_urng{}); // requires Cpp17ValueSwappable

    // std::random_shuffle (removed in C++17) also requires Cpp17ValueSwappable

#if _HAS_CXX20
    (void) std::shift_left(varr, varr, 0);

    // (void) std::shift_right(varr, varr, 0); // requires Cpp17ValueSwappable
#endif // _HAS_CXX20

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

#if _HAS_CXX20 && defined(__cpp_lib_concepts) // TRANSITION, GH-395
    (void) std::lexicographical_compare_three_way(varr, varr, varr, varr);
    (void) std::lexicographical_compare_three_way(iarr, iarr, iarr, iarr, validating_compare_three_way{});
#endif // _HAS_CXX20 && defined(__cpp_lib_concepts)
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

    (void) std::min_element(ExecutionPolicy, varr, varr + 1);
    (void) std::min_element(ExecutionPolicy, iarr, iarr + 1, validating_less{});

    (void) std::max_element(ExecutionPolicy, varr, varr + 1);
    (void) std::max_element(ExecutionPolicy, iarr, iarr + 1, validating_less{});

    (void) std::minmax_element(ExecutionPolicy, varr, varr + 1);
    (void) std::minmax_element(ExecutionPolicy, iarr, iarr + 1, validating_less{});

    (void) std::lexicographical_compare(ExecutionPolicy, varr, varr, varr, varr);
    (void) std::lexicographical_compare(ExecutionPolicy, iarr, iarr, iarr, iarr, validating_less{});
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

#if _HAS_CXX20 && defined(__cpp_lib_concepts) // TRANSITION, GH-395
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
#endif // _HAS_CXX23
}
#endif // _HAS_CXX20 && defined(__cpp_lib_concepts)
#endif // _M_CEE
