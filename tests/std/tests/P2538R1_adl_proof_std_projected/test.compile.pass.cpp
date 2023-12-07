// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef _M_CEE // TRANSITION, VSO-1659496
#include <algorithm>
#include <cassert>
#include <functional>
#include <iterator>
#include <ranges>
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
struct tagged_identity {
    template <class T>
    constexpr T&& operator()(T&& t) const noexcept {
        return std::forward<T>(t);
    }
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

using simple_truth    = tagged_truth<void>;
using simple_identity = tagged_identity<void>;

using validator           = holder<incomplete>*;
using validating_truth    = tagged_truth<holder<incomplete>>;
using validating_equal    = tagged_equal<holder<incomplete>>;
using validating_less     = tagged_less<holder<incomplete>>;
using validating_identity = tagged_identity<holder<incomplete>>;

void test_ranges_algorithms() {
    using namespace std::ranges;

    int iarr[1]{};
    validator varr[1]{};

    (void) all_of(varr, varr, simple_truth{});
    (void) all_of(varr, simple_truth{});
    (void) all_of(iarr, iarr, validating_truth{});
    (void) all_of(iarr, validating_truth{});

    (void) any_of(varr, varr, simple_truth{});
    (void) any_of(varr, simple_truth{});
    (void) any_of(iarr, iarr, validating_truth{});
    (void) any_of(iarr, validating_truth{});

    (void) none_of(varr, varr, simple_truth{});
    (void) none_of(varr, simple_truth{});
    (void) none_of(iarr, iarr, validating_truth{});
    (void) none_of(iarr, validating_truth{});

#if _HAS_CXX23
    (void) contains(varr, varr, validator{});
    (void) contains(varr, validator{});
    (void) contains(iarr, iarr, 0, validating_identity{});
    (void) contains(iarr, 0, validating_identity{});

    (void) contains_subrange(varr, varr, varr, varr);
    (void) contains_subrange(varr, varr);
    (void) contains_subrange(iarr, iarr, iarr, iarr, validating_equal{});
    // (void) contains_subrange(iarr, iarr, validating_equal{}); // needs to check ADL-found operator*
    (void) contains_subrange(iarr, iarr, {}, validating_identity{});
#endif // _HAS_CXX23

    (void) for_each(varr, varr, simple_truth{});
    (void) for_each(varr, simple_truth{});
    (void) for_each(varr, varr, validating_truth{});
    (void) for_each(varr, validating_truth{});
    (void) for_each(iarr, iarr, validating_truth{});
    (void) for_each(iarr, validating_truth{});

    (void) for_each_n(varr, 0, simple_truth{});
    (void) for_each_n(varr, 0, validating_truth{});
    (void) for_each_n(iarr, 0, validating_truth{});

    (void) find(varr, varr, validator{});
    (void) find(varr, validator{});

    (void) find_if(varr, varr, simple_truth{});
    (void) find_if(varr, simple_truth{});
    (void) find_if(iarr, iarr, validating_truth{});
    (void) find_if(iarr, validating_truth{});

    (void) find_if_not(varr, varr, simple_truth{});
    (void) find_if_not(varr, simple_truth{});
    (void) find_if_not(iarr, iarr, validating_truth{});
    (void) find_if_not(iarr, validating_truth{});

#if _HAS_CXX23
    (void) find_last(varr, varr, validator{});
    (void) find_last(varr, validator{});

    (void) find_last_if(varr, varr, simple_truth{});
    (void) find_last_if(varr, simple_truth{});
    (void) find_last_if(iarr, iarr, validating_truth{});
    (void) find_last_if(iarr, validating_truth{});

    (void) find_last_if_not(varr, varr, simple_truth{});
    (void) find_last_if_not(varr, simple_truth{});
    (void) find_last_if_not(iarr, iarr, validating_truth{});
    (void) find_last_if_not(iarr, validating_truth{});
#endif // _HAS_CXX23

    (void) find_end(varr, varr, varr, varr);
    (void) find_end(varr, varr);
    (void) find_end(varr, varr, varr, varr, validating_equal{});
    (void) find_end(varr, varr, validating_equal{});
    (void) find_end(iarr, iarr, iarr, iarr, validating_equal{});
    (void) find_end(iarr, iarr, validating_equal{});

    (void) find_first_of(varr, varr, varr, varr);
    (void) find_first_of(varr, varr);
    (void) find_first_of(varr, varr, varr, varr, validating_equal{});
    (void) find_first_of(varr, varr, validating_equal{});
    (void) find_first_of(iarr, iarr, iarr, iarr, validating_equal{});
    (void) find_first_of(iarr, iarr, validating_equal{});

    (void) adjacent_find(varr, varr);
    (void) adjacent_find(varr);
    (void) adjacent_find(iarr, iarr, validating_equal{});
    (void) adjacent_find(iarr, iarr, {}, validating_identity{});
    // (void) adjacent_find(iarr, validating_equal{}); // needs to check ADL-found swap
    (void) adjacent_find(iarr, {}, validating_identity{});

    (void) count(varr, varr, validator{});
    (void) count(varr, validator{});
    (void) count(iarr, iarr, 0, validating_identity{});
    (void) count(iarr, 0, validating_identity{});

    (void) count_if(varr, varr, simple_truth{});
    (void) count_if(varr, simple_truth{});
    (void) count_if(iarr, iarr, validating_truth{});
    (void) count_if(iarr, validating_truth{});

    (void) mismatch(varr, varr, varr, varr);
    (void) mismatch(varr, varr);
    (void) mismatch(iarr, iarr, iarr, iarr, validating_equal{});
    (void) mismatch(iarr, iarr, validating_equal{});

    (void) equal(varr, varr, varr, varr);
    (void) equal(varr, varr);
    (void) equal(iarr, iarr, iarr, iarr, validating_equal{});
    (void) equal(iarr, iarr, validating_equal{});

    (void) is_permutation(varr, varr, varr, varr);
    (void) is_permutation(varr, varr);
    (void) is_permutation(varr, varr, varr, varr, validating_equal{});
    (void) is_permutation(varr, varr, validating_equal{});
    (void) is_permutation(iarr, iarr, iarr, iarr, validating_equal{});
    (void) is_permutation(iarr, iarr, validating_equal{});
    (void) is_permutation(iarr, iarr, iarr, iarr, {}, validating_identity{});
    (void) is_permutation(iarr, iarr, {}, validating_identity{});

    (void) search(varr, varr, varr, varr);
    (void) search(varr, varr);
    (void) search(iarr, iarr, iarr, iarr, validating_equal{});
    (void) search(iarr, iarr, validating_equal{});

    (void) search_n(varr, varr, 0, validator{});
    (void) search_n(varr, 0, validator{});
    (void) search_n(iarr, iarr, 0, 0, validating_equal{});
    (void) search_n(iarr, 0, 0, validating_equal{});

#if _HAS_CXX23
    (void) starts_with(varr, varr, varr, varr);
    (void) starts_with(varr, varr);
    (void) starts_with(iarr, iarr, iarr, iarr, validating_equal{});
    (void) starts_with(iarr, iarr, validating_equal{});

    (void) ends_with(varr, varr, varr, varr);
    (void) ends_with(varr, varr);
    (void) ends_with(iarr, iarr, iarr, iarr, validating_equal{});
    (void) ends_with(iarr, iarr, validating_equal{});
#endif // _HAS_CXX23

    (void) min(+varr, +varr);
    (void) min({+varr, +varr});
    (void) min(varr);
    (void) min(+iarr, +iarr, validating_less{});
    (void) min({+iarr, +iarr}, {}, validating_identity{});
    (void) min(iarr, validating_less{});

    (void) max(+varr, +varr);
    (void) max({+varr, +varr});
    (void) max(varr);
    (void) max(+iarr, +iarr, validating_less{});
    (void) max({+iarr, +iarr}, {}, validating_identity{});
    (void) max(iarr, validating_less{});

    (void) minmax(+varr, +varr);
    (void) minmax({+varr, +varr});
    (void) minmax(varr);
    (void) minmax(+iarr, +iarr, validating_less{});
    (void) minmax({+iarr, +iarr}, {}, validating_identity{});
    (void) minmax(iarr, validating_less{});

    (void) min_element(varr, varr + 1);
    (void) min_element(varr);
    (void) min_element(iarr, iarr + 1, validating_less{});
    (void) min_element(iarr, {}, validating_identity{});

    (void) max_element(varr, varr + 1);
    (void) max_element(varr);
    (void) max_element(iarr, iarr + 1, validating_less{});
    (void) max_element(iarr, {}, validating_identity{});

    (void) minmax_element(varr, varr + 1);
    (void) minmax_element(varr);
    (void) minmax_element(iarr, iarr + 1, validating_less{});
    (void) minmax_element(iarr, {}, validating_identity{});

    (void) clamp(+varr, +varr, +varr);
    (void) clamp(+iarr, +iarr, +iarr, validating_less{});

    (void) lexicographical_compare(varr, varr, varr, varr);
    (void) lexicographical_compare(varr, varr);
    (void) lexicographical_compare(iarr, iarr, iarr, iarr, validating_less{});
    (void) lexicographical_compare(iarr, iarr, validating_less{});
}

// Separated test for ranges::count and equality
static_assert(std::equality_comparable<validator>);
static_assert(std::indirectly_comparable<validator*, validator*, std::equal_to<>>);
static_assert(std::sortable<validator*>);

constexpr bool test_ranges_count() {
    using namespace std::ranges;

    validator a[10]{};
    assert(count(a, a + 10, nullptr) == 10);
    assert(count(a, nullptr) == 10);
    return true;
}
static_assert(test_ranges_count());
#endif // _M_CEE
