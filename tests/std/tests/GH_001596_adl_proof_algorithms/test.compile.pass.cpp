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

template <class T>
struct holder {
    T t;
};

#if _HAS_CXX23 && defined(__cpp_lib_concepts) // TRANSITION, GH-395
template <class Tag>
struct tagged_left_selector {
    template <class T>
    constexpr T operator()(T lhs, T) const noexcept {
        return lhs;
    }
};
#endif // _HAS_CXX23 && defined(__cpp_lib_concepts)

struct incomplete;

using simple_truth = tagged_truth<void>;

using validator        = holder<incomplete>*;
using validating_truth = tagged_truth<holder<incomplete>>;
using validating_equal = tagged_equal<holder<incomplete>>;

#if _HAS_CXX23 && defined(__cpp_lib_concepts) // TRANSITION, GH-395
using simple_left_selector = tagged_left_selector<void>;

using validating_left_selector = tagged_left_selector<holder<incomplete>>;
#endif // _HAS_CXX23 && defined(__cpp_lib_concepts)

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

    (void) std::remove(varr, varr, validator{});

    (void) std::remove_if(varr, varr, simple_truth{});
    (void) std::remove_if(iarr, iarr, validating_truth{});
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

#if _HAS_CXX23 && defined(__cpp_lib_concepts) // TRANSITION, GH-395
void test_ranges_non_projected_algorithms() {
    using namespace std::ranges;

    int iarr[1]{};
    validator varr[1]{};

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
}
#endif // _HAS_CXX23 && defined(__cpp_lib_concepts)
#endif // _M_CEE
