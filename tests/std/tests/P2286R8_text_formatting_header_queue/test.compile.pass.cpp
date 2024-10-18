// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

// intentionally avoid including <format> to verify that the formatter specializations are defined in <queue>

#include <concepts>
#include <deque>
#include <functional>
#include <queue>
#include <type_traits>
#include <utility>
#include <vector>

using namespace std;

struct cannot_format {
    friend auto operator<=>(cannot_format, cannot_format) = default;
};

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
    verify_semiregularity_for<formatter<queue<int>, char>>();
    verify_semiregularity_for<formatter<queue<int>, wchar_t>>();
    verify_semiregularity_for<formatter<queue<int, pmr::deque<int>>, char>>();
    verify_semiregularity_for<formatter<queue<int, pmr::deque<int>>, wchar_t>>();

    verify_semiregularity_for<formatter<priority_queue<int>, char>>();
    verify_semiregularity_for<formatter<priority_queue<int>, wchar_t>>();
    verify_semiregularity_for<formatter<priority_queue<int, vector<int>, greater<>>, char>>();
    verify_semiregularity_for<formatter<priority_queue<int, vector<int>, greater<>>, wchar_t>>();
    verify_semiregularity_for<formatter<priority_queue<int, pmr::vector<int>>, char>>();
    verify_semiregularity_for<formatter<priority_queue<int, pmr::vector<int>>, wchar_t>>();

    verify_disabled_for<formatter<queue<cannot_format>, char>>();
    verify_disabled_for<formatter<queue<cannot_format>, wchar_t>>();
    verify_disabled_for<formatter<queue<cannot_format, pmr::deque<cannot_format>>, char>>();
    verify_disabled_for<formatter<queue<cannot_format, pmr::deque<cannot_format>>, wchar_t>>();

    verify_disabled_for<formatter<priority_queue<cannot_format>, char>>();
    verify_disabled_for<formatter<priority_queue<cannot_format>, wchar_t>>();
    verify_disabled_for<formatter<priority_queue<cannot_format, vector<cannot_format>, greater<>>, char>>();
    verify_disabled_for<formatter<priority_queue<cannot_format, vector<cannot_format>, greater<>>, wchar_t>>();
    verify_disabled_for<formatter<priority_queue<cannot_format, pmr::vector<cannot_format>>, char>>();
    verify_disabled_for<formatter<priority_queue<cannot_format, pmr::vector<cannot_format>>, wchar_t>>();
}
