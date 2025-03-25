// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#define _MSVC_TESTING_NVCC

#include <__msvc_all_public_headers.hpp>

using namespace std;

#if _HAS_CXX20
// Test VSO-2411436 "[Feedback] CUDA (12.8) host code compilation fails with std::format and VS2022"
void test_VSO_2411436() {
    (void) format("{}", 1729);
}
#endif // _HAS_CXX20
