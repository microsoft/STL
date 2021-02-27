// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

// The instantiate_algorithm* tests take too long individually, so must be split into two parts.
// instantiate_algorithms_op_deref.hpp contains the common test code.

int main() {} // COMPILE-ONLY

#define _USE_NAMED_IDL_NAMESPACE          1
#define INSTANTIATE_ALGORITHMS_SPLIT_MODE 1
using test_difference_type = short;
#include <instantiate_algorithms_op_deref.hpp>
