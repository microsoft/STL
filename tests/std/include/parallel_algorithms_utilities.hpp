// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#pragma once
#include <algorithm>
#include <cstddef>
#include <limits>
#include <thread>

#ifdef EXHAUSTIVE
const size_t max_parallel_test_case_n = 4096;
// the number of linear steps a test case that is quadratic should attempt:
const size_t quadratic_complexity_case_limit = std::numeric_limits<std::size_t>::max();
#else // ^^^ EXHAUSTIVE / !EXHAUSTIVE vvv
// The constant 32 comes from std::_Oversubscription_multiplier
const size_t max_parallel_test_case_n        = std::max(4u, std::thread::hardware_concurrency() * 32) + 1;
const size_t quadratic_complexity_case_limit = 128;
#endif // EXHAUSTIVE

#pragma warning(push)
#pragma warning(disable : 4640) // 'cases': construction of local static object is not thread-safe
template <typename Fx, typename... Args>
void parallel_test_case(Fx fn, Args&... vals) {
    // call fn with "interesting" test case sizes and additional parameters
#ifdef EXHAUSTIVE
    for (size_t testSize = 0; testSize < max_parallel_test_case_n; ++testSize) {
#else // ^^^ EXHAUSTIVE / !EXHAUSTIVE vvv
    static const size_t cases[] = {
        0, 1, 2, max_parallel_test_case_n - 2, max_parallel_test_case_n - 1, max_parallel_test_case_n};
    for (size_t testSize : cases) {
#endif // EXHAUSTIVE
        fn(testSize, vals...);
    }
}
#pragma warning(pop)
