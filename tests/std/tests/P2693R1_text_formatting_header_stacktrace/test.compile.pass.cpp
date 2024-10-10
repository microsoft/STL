// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

// intentionally avoid including <format> to verify that the formatter specializations are defined in <stacktrace>

#include <concepts>
#include <stacktrace>
#include <utility>

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
    verify_semiregularity_for<formatter<stacktrace_entry, char>>();
    verify_semiregularity_for<formatter<stacktrace, char>>();
    verify_semiregularity_for<formatter<pmr::stacktrace, char>>();
}
