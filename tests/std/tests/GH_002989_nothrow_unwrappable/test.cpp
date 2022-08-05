// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <filesystem>
#include <list>
#include <type_traits>
#include <vector>
#include <xutility>

using namespace std;

#define STATIC_ASSERT(...) static_assert(__VA_ARGS__, #__VA_ARGS__)

template <class T, bool CopyUnwrapNothrow = true>
void do_test() {
    STATIC_ASSERT(_Is_nothrow_unwrappable_v<T>);
    STATIC_ASSERT(_Is_nothrow_unwrappable_v<T&&>);
    STATIC_ASSERT(noexcept(_Get_unwrapped(declval<T>())));

    STATIC_ASSERT(_Is_nothrow_unwrappable_v<const T&> == CopyUnwrapNothrow);
    STATIC_ASSERT(noexcept(_Get_unwrapped(declval<const T&>())) == CopyUnwrapNothrow);
    STATIC_ASSERT(_Is_nothrow_unwrappable_v<const T&&> == CopyUnwrapNothrow);
    STATIC_ASSERT(noexcept(_Get_unwrapped(declval<const T&&>())) == CopyUnwrapNothrow);
}

int main() {
    do_test<int>();
    do_test<int*>();
    do_test<int[]>();

    do_test<vector<int>::iterator>();
    do_test<vector<int>::const_iterator>();
    do_test<list<int>::iterator>();
    do_test<list<int>::const_iterator>();

    do_test<filesystem::path::iterator, false>();
}
