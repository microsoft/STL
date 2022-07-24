// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <cassert>
#include <cstddef>
#include <iterator>
#include <numeric>
#include <utility>

using namespace std;

struct not_std_plus {
    template <class T, class U>
    constexpr decltype(auto) operator()(T&& t, U&& u) const {
        return forward<T>(t) + forward<U>(u);
    }
};

struct not_std_minus {
    template <class T, class U>
    constexpr decltype(auto) operator()(T&& t, U&& u) const {
        return forward<T>(t) - forward<U>(u);
    }
};

struct not_std_multiplies {
    template <class T, class U>
    constexpr decltype(auto) operator()(T&& t, U&& u) const {
        return forward<T>(t) * forward<U>(u);
    }
};

struct plus_one {
    constexpr int operator()(int a) const {
        return a + 1;
    }
};

template <size_t N>
constexpr bool is_partial_sum(const int (&output)[N], int temp = 0) {
    for (size_t idx = 0; idx < N; ++idx) {
        temp += static_cast<int>(idx) + 1;
        if (output[idx] != temp) {
            return false;
        }
    }

    return true;
}

template <size_t N>
constexpr bool is_exclusive_scan(const int (&output)[N]) {
    int temp = 0;
    for (size_t idx = 0; idx < N; ++idx) {
        if (output[idx] != temp) {
            return false;
        }

        temp += static_cast<int>(idx) + 1;
    }

    return true;
}

template <size_t N>
constexpr bool is_transform_inclusive_scan(const int (&output)[N], int temp = 0) {
    for (size_t idx = 0; idx < N; ++idx) {
        temp += static_cast<int>(idx) + 2;
        if (output[idx] != temp) {
            return false;
        }
    }

    return true;
}

template <size_t N>
constexpr bool is_transform_exclusive_scan(const int (&output)[N]) {
    int temp = 0;
    for (size_t idx = 0; idx < N; ++idx) {
        if (output[idx] != temp) {
            return false;
        }

        temp += static_cast<int>(idx) + 2;
    }

    return true;
}

constexpr bool run_tests() {
    int input[] = {0, 0, 0, 0, 0};
    // test iota
    iota(input, input + 0, 1);
    for (size_t idx = 0; idx < size(input); ++idx) {
        assert(input[idx] == 0);
    }
    iota(begin(input), end(input), 1);
    for (size_t idx = 0; idx < size(input); ++idx) {
        assert(input[idx] == static_cast<int>(idx) + 1);
    }

    // test accumulate
    assert(accumulate(begin(input), end(input), 0) == 15);
    assert(accumulate(begin(input), end(input), 1) == 16);
    assert(accumulate(input, input + 0, 1) == 1);
    assert(accumulate(begin(input), end(input), 0, not_std_plus{}) == 15);
    assert(accumulate(begin(input), end(input), 1, not_std_plus{}) == 16);
    assert(accumulate(input, input + 0, 1, not_std_plus{}) == 1);

    // test reduce
    assert(reduce(begin(input), end(input)) == 15);
    assert(reduce(begin(input), end(input), 0) == 15);
    assert(reduce(begin(input), end(input), 1) == 16);
    assert(reduce(input, input + 0) == 0);
    assert(reduce(input, input + 0, 1) == 1);
    assert(reduce(begin(input), end(input), 0, not_std_plus{}) == 15);
    assert(reduce(begin(input), end(input), 1, not_std_plus{}) == 16);
    assert(reduce(input, input + 0, 1, not_std_plus{}) == 1);

    int input2[] = {2, 3, 4, 5, 6};
    static_assert(size(input) == size(input2));

    // test inner_product
    assert(inner_product(begin(input), end(input), begin(input2), 0) == 70);
    assert(inner_product(begin(input), end(input), begin(input2), 1) == 71);
    assert(inner_product(begin(input), input + 0, begin(input2), 0) == 0);
    assert(inner_product(begin(input), input + 0, begin(input2), 1) == 1);
    assert(inner_product(begin(input), end(input), begin(input2), 0, not_std_plus{}, not_std_multiplies{}) == 70);
    assert(inner_product(begin(input), end(input), begin(input2), 1, not_std_plus{}, not_std_multiplies{}) == 71);
    assert(inner_product(begin(input), input + 0, begin(input2), 0, not_std_plus{}, not_std_multiplies{}) == 0);
    assert(inner_product(begin(input), input + 0, begin(input2), 1, not_std_plus{}, not_std_multiplies{}) == 1);

    // test transform_reduce (1 range)
    assert(transform_reduce(begin(input), end(input), 0, not_std_plus{}, plus_one{}) == 20);

    // test transform_reduce (1.5 range)
    assert(transform_reduce(begin(input), end(input), begin(input2), 0) == 70);
    assert(transform_reduce(begin(input), end(input), begin(input2), 1) == 71);
    assert(transform_reduce(begin(input), input + 0, begin(input2), 0) == 0);
    assert(transform_reduce(begin(input), input + 0, begin(input2), 1) == 1);
    assert(transform_reduce(begin(input), end(input), begin(input2), 0, not_std_plus{}, not_std_multiplies{}) == 70);
    assert(transform_reduce(begin(input), end(input), begin(input2), 1, not_std_plus{}, not_std_multiplies{}) == 71);
    assert(transform_reduce(begin(input), input + 0, begin(input2), 0, not_std_plus{}, not_std_multiplies{}) == 0);
    assert(transform_reduce(begin(input), input + 0, begin(input2), 1, not_std_plus{}, not_std_multiplies{}) == 1);

    // test partial_sum
    {
        int output[size(input)] = {};
        assert(end(output) == partial_sum(begin(input), end(input), begin(output)));
        assert(is_partial_sum(output));
    }

    {
        int output[size(input)] = {};
        assert(end(output) == partial_sum(begin(input), end(input), begin(output), not_std_plus{}));
        assert(is_partial_sum(output));
    }

    // test inclusive_scan
    {
        int output[size(input)] = {};
        assert(end(output) == inclusive_scan(begin(input), end(input), begin(output)));
        assert(is_partial_sum(output));
    }

    {
        int output[size(input)] = {};
        assert(end(output) == inclusive_scan(begin(input), end(input), begin(output), not_std_plus{}));
        assert(is_partial_sum(output));
    }

    {
        int output[size(input)] = {};
        assert(end(output) == inclusive_scan(begin(input), end(input), begin(output), not_std_plus{}, 1));
        assert(is_partial_sum(output, 1));
    }

    // test exclusive_scan
    {
        int output[size(input)] = {};
        assert(end(output) == exclusive_scan(begin(input), end(input), begin(output), 0));
        assert(is_exclusive_scan(output));
    }

    {
        int output[size(input)] = {};
        assert(end(output) == exclusive_scan(begin(input), end(input), begin(output), 0, not_std_plus{}));
        assert(is_exclusive_scan(output));
    }

    // test transform_inclusive_scan
    {
        int output[size(input)] = {};
        assert(end(output) == transform_inclusive_scan(begin(input), end(input), begin(output), plus{}, plus_one{}));
        assert(is_transform_inclusive_scan(output));
    }

    {
        int output[size(input)] = {};
        assert(end(output) == transform_inclusive_scan(begin(input), end(input), begin(output), plus{}, plus_one{}, 1));
        assert(is_transform_inclusive_scan(output, 1));
    }

    // test transform_exclusive_scan
    {
        int output[size(input)] = {};
        assert(end(output) == transform_exclusive_scan(begin(input), end(input), begin(output), 0, plus{}, plus_one{}));
        assert(is_transform_exclusive_scan(output));
    }

    // test adjacent_difference
    {
        int output[size(input)] = {};
        assert(end(output) == adjacent_difference(begin(input), end(input), begin(output)));
        for (const int& o : output) {
            assert(o == 1);
        }
    }

    {
        int output[size(input)] = {};
        assert(end(output) == adjacent_difference(begin(input), end(input), begin(output), not_std_minus{}));
        for (const int& o : output) {
            assert(o == 1);
        }
    }

    return true;
}

int main() {
    static_assert(run_tests());
    assert(run_tests());
}
