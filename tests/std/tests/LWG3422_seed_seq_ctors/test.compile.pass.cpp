// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <random>
#include <type_traits>
#include <vector>

using namespace std;

static_assert(is_nothrow_default_constructible_v<seed_seq>, "");
static_assert(!is_constructible_v<seed_seq, initializer_list<vector<int>::iterator>>, "");
static_assert(is_constructible_v<seed_seq, initializer_list<int>>, "");
