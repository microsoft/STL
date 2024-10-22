// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

// intentionally avoid including <format> to verify that the formatter specialization is defined in <stack>

#include <concepts>
#include <deque>
#include <stack>
#include <type_traits>
#include <utility>

using namespace std;

struct cannot_format {};

template <class T>
void verify_semiregularity_for() {
    static_assert(semiregular<T>);

    T x;
    T y = x;
    T z = move(x);
    x   = y;
    x   = move(z);
}

template <class T>
void verify_disabled_for() {
    static_assert(!is_default_constructible_v<T>);
    static_assert(!is_copy_constructible_v<T>);
    static_assert(!is_move_constructible_v<T>);
    static_assert(!is_copy_assignable_v<T>);
    static_assert(!is_move_assignable_v<T>);
}

void verify_formatters() {
    verify_semiregularity_for<formatter<stack<int>, char>>();
    verify_semiregularity_for<formatter<stack<int>, wchar_t>>();
    verify_semiregularity_for<formatter<stack<int, pmr::deque<int>>, char>>();
    verify_semiregularity_for<formatter<stack<int, pmr::deque<int>>, wchar_t>>();

    verify_disabled_for<formatter<stack<cannot_format>, char>>();
    verify_disabled_for<formatter<stack<cannot_format>, wchar_t>>();
    verify_disabled_for<formatter<stack<cannot_format, pmr::deque<cannot_format>>, char>>();
    verify_disabled_for<formatter<stack<cannot_format, pmr::deque<cannot_format>>, wchar_t>>();
}
