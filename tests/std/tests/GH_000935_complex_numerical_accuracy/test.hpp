// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#pragma once

#include <complex>
#include <limits>

#include "floating_point_utils.hpp"

constexpr double double_inf = std::numeric_limits<double>::infinity();
constexpr double double_nan = std::numeric_limits<double>::quiet_NaN();

constexpr float float_inf = std::numeric_limits<float>::infinity();
constexpr float float_nan = std::numeric_limits<float>::quiet_NaN();

struct complex_result_exactness {
    bool real = false;
    bool imag = false;
};

template <typename T>
struct complex_unary_test_case {
    std::complex<T> input;
    std::complex<T> expected_result;
    complex_result_exactness result_exactness{};
};
