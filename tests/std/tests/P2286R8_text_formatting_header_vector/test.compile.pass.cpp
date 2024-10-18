// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

// intentionally avoid including <format> to verify that the formatter specialization is defined in <vector>

#include <concepts>
#include <utility>
#include <vector>

using namespace std;

template <class T>
void verify_semiregularity_for() {
    static_assert(semiregular<T>);

    T x;
    T y = x;
    T z = move(x);
    x   = y;
    x   = move(z);
}

void verify_formatters() {
    verify_semiregularity_for<formatter<vector<bool>::reference, char>>();
    verify_semiregularity_for<formatter<vector<bool>::reference, wchar_t>>();
    verify_semiregularity_for<formatter<pmr::vector<bool>::reference, char>>();
    verify_semiregularity_for<formatter<pmr::vector<bool>::reference, wchar_t>>();
}
