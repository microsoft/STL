// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

// intentionally test narrowing conversion from int64_t to int32_t
#pragma warning(disable : 4244)

#include <cassert>
#include <cstdint>
#include <execution>
#include <numeric>
#include <type_traits>

using namespace std;
using namespace std::execution;

struct implicitly_convertible_to_i32_only {
    int32_t n;

    template <class = void>
    explicit operator int32_t() const = delete;

    operator int64_t() const noexcept {
        return n;
    }
};

static_assert(!is_constructible_v<int32_t, implicitly_convertible_to_i32_only>);
static_assert(is_convertible_v<implicitly_convertible_to_i32_only, int32_t>);

struct implicitly_validating_converter {
    implicitly_convertible_to_i32_only operator()(int n) const noexcept {
        return {n};
    }
};

struct explicitly_convertible_to_i32_only {
    int32_t n;

    explicit operator int32_t() const noexcept {
        return n;
    }
};

static_assert(is_constructible_v<int32_t, explicitly_convertible_to_i32_only>);
static_assert(!is_convertible_v<explicitly_convertible_to_i32_only, int32_t>);

struct transformation_validating_converter {
    explicitly_convertible_to_i32_only operator()(int n) const noexcept {
        return {n};
    }
};

struct implicitly_validating_plus {
    implicitly_convertible_to_i32_only operator()(
        implicitly_convertible_to_i32_only l, implicitly_convertible_to_i32_only r) const noexcept {
        return implicitly_convertible_to_i32_only{l.n + r.n};
    }

    implicitly_convertible_to_i32_only operator()(int32_t l, implicitly_convertible_to_i32_only r) const noexcept {
        return implicitly_convertible_to_i32_only{l + r.n};
    }

    implicitly_convertible_to_i32_only operator()(implicitly_convertible_to_i32_only l, int32_t r) const noexcept {
        return implicitly_convertible_to_i32_only{l.n + r};
    }

    implicitly_convertible_to_i32_only operator()(int32_t l, int32_t r) const noexcept {
        return implicitly_convertible_to_i32_only{l + r};
    }
};

struct implicitly_validating_plus_for_transformation {
    implicitly_convertible_to_i32_only operator()(
        explicitly_convertible_to_i32_only l, explicitly_convertible_to_i32_only r) const noexcept {
        return implicitly_convertible_to_i32_only{l.n + r.n};
    }

    implicitly_convertible_to_i32_only operator()(int32_t l, explicitly_convertible_to_i32_only r) const noexcept {
        return implicitly_convertible_to_i32_only{l + r.n};
    }

    implicitly_convertible_to_i32_only operator()(explicitly_convertible_to_i32_only l, int32_t r) const noexcept {
        return implicitly_convertible_to_i32_only{l.n + r};
    }

    implicitly_convertible_to_i32_only operator()(int32_t l, int32_t r) const noexcept {
        return implicitly_convertible_to_i32_only{l + r};
    }
};

struct implicitly_validating_multiplies {
    implicitly_convertible_to_i32_only operator()(
        implicitly_convertible_to_i32_only l, implicitly_convertible_to_i32_only r) const noexcept {
        return implicitly_convertible_to_i32_only{l.n * r.n};
    }

    implicitly_convertible_to_i32_only operator()(int32_t l, implicitly_convertible_to_i32_only r) const noexcept {
        return implicitly_convertible_to_i32_only{l * r.n};
    }

    implicitly_convertible_to_i32_only operator()(implicitly_convertible_to_i32_only l, int32_t r) const noexcept {
        return implicitly_convertible_to_i32_only{l.n * r};
    }

    implicitly_convertible_to_i32_only operator()(int32_t l, int32_t r) const noexcept {
        return implicitly_convertible_to_i32_only{l * r};
    }
};

void test_copy_initialization_for_numeric_algorithms() {
    int arr[1]{};
    implicitly_convertible_to_i32_only brr[1]{};

    assert(reduce(arr, arr, int32_t{}, implicitly_validating_plus{}) == 0);
    assert(reduce(brr, brr, int32_t{}, implicitly_validating_plus{}) == 0);

    assert(transform_reduce(arr, arr, arr, int32_t{}, implicitly_validating_plus{}, implicitly_validating_multiplies{})
           == 0);
    assert(transform_reduce(arr, arr, brr, int32_t{}, implicitly_validating_plus{}, implicitly_validating_multiplies{})
           == 0);

    assert(transform_reduce(arr, arr, int32_t{}, implicitly_validating_plus{}, implicitly_validating_converter{}) == 0);
    assert(transform_reduce(brr, brr, int32_t{}, implicitly_validating_plus{}, implicitly_validating_converter{}) == 0);

    assert(exclusive_scan(arr, arr, arr, int32_t{}, implicitly_validating_plus{}) == arr);

    assert(inclusive_scan(arr, arr, arr, implicitly_validating_plus{}) == arr);
    assert(inclusive_scan(arr, arr, arr, implicitly_validating_plus{}, int32_t{}) == arr);

    assert(transform_exclusive_scan(arr, arr, arr, int32_t{}, implicitly_validating_plus_for_transformation{},
               transformation_validating_converter{})
           == arr);

    assert(transform_inclusive_scan(brr, brr, brr, implicitly_validating_plus{}, implicitly_validating_converter{})
           == brr);
    assert(transform_inclusive_scan(arr, arr, arr, implicitly_validating_plus_for_transformation{},
               transformation_validating_converter{}, int32_t{})
           == arr);
}

template <const auto& ExPo>
void test_copy_initialization_for_parallel_numeric_algorithms() {
    int arr[1]{};
    implicitly_convertible_to_i32_only brr[1]{};

    assert(reduce(ExPo, arr, arr, int32_t{}, implicitly_validating_plus{}) == 0);
    assert(reduce(ExPo, brr, brr, int32_t{}, implicitly_validating_plus{}) == 0);

    assert(transform_reduce(
               ExPo, arr, arr, arr, int32_t{}, implicitly_validating_plus{}, implicitly_validating_multiplies{})
           == 0);
    assert(transform_reduce(
               ExPo, arr, arr, brr, int32_t{}, implicitly_validating_plus{}, implicitly_validating_multiplies{})
           == 0);

    assert(transform_reduce(ExPo, arr, arr, int32_t{}, implicitly_validating_plus{}, implicitly_validating_converter{})
           == 0);
    assert(transform_reduce(ExPo, brr, brr, int32_t{}, implicitly_validating_plus{}, implicitly_validating_converter{})
           == 0);

    assert(exclusive_scan(ExPo, arr, arr, arr, int32_t{}, implicitly_validating_plus{}) == arr);

    assert(inclusive_scan(ExPo, arr, arr, arr, implicitly_validating_plus{}) == arr);
    assert(inclusive_scan(ExPo, arr, arr, arr, implicitly_validating_plus{}, int32_t{}) == arr);

    assert(transform_exclusive_scan(ExPo, arr, arr, arr, int32_t{}, implicitly_validating_plus_for_transformation{},
               transformation_validating_converter{})
           == arr);

    assert(
        transform_inclusive_scan(ExPo, brr, brr, brr, implicitly_validating_plus{}, implicitly_validating_converter{})
        == brr);
    assert(transform_inclusive_scan(ExPo, arr, arr, arr, implicitly_validating_plus_for_transformation{},
               transformation_validating_converter{}, int32_t{})
           == arr);
}

int main() {
    test_copy_initialization_for_numeric_algorithms();
    test_copy_initialization_for_parallel_numeric_algorithms<seq>();
    test_copy_initialization_for_parallel_numeric_algorithms<par>();
    test_copy_initialization_for_parallel_numeric_algorithms<par_unseq>();
#if _HAS_CXX20
    test_copy_initialization_for_parallel_numeric_algorithms<unseq>();
#endif // _HAS_CXX20
}
