// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#pragma once
#include <algorithm>
#include <stddef.h>
#include <thread>

#ifdef EXHAUSTIVE
const size_t max_parallel_test_case_n = 1000;
#else // ^^^ EXHAUSTIVE ^^^ // vvv !EXHAUSTIVE vvv
const size_t max_parallel_test_case_n = std::max(4u, std::thread::hardware_concurrency()) + 1;
#endif // EXHAUSTIVE

#pragma warning(push)
#pragma warning(disable : 4640) // 'cases': construction of local static object is not thread-safe
template <typename Fx, typename... Args>
void parallel_test_case(Fx fn, Args&... vals) {
    // call fn with "interesting" test case sizes and additional parameters
#ifdef EXHAUSTIVE
    for (size_t testSize = 0; testSize < max_parallel_test_case_n; ++testSize) {
#else // ^^^ EXHAUSTIVE ^^^ // vvv !EXHAUSTIVE vvv
    static const size_t cases[] = {
        0, 1, 2, max_parallel_test_case_n - 2, max_parallel_test_case_n - 1, max_parallel_test_case_n};
    for (size_t testSize : cases) {
#endif // EXHAUSTIVE
        fn(testSize, vals...);
    }
}
#pragma warning(pop)
