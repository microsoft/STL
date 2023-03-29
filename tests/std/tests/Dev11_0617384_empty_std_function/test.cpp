// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <cassert>
#include <functional>
#include <memory>
#include <utility>

using namespace std;

struct X {};

int main() {
    function<int(int)> same;
    short (*fp)(long long) = nullptr;
    function<short(long long)> other;

    assert(!function<int(int)>());
    assert(!function<int(int)>(nullptr));
    assert(!function<int(int)>(same));
    assert(!function<int(int)>(move(same)));
    same = nullptr;
    assert(!function<int(int)>(fp));
    assert(!function<int(int)>(other));

#if _HAS_FUNCTION_ALLOCATOR_SUPPORT
    allocator<int> al;
    assert(!function<int(int)>(allocator_arg, al));
    assert(!function<int(int)>(allocator_arg, al, nullptr));
    assert(!function<int(int)>(allocator_arg, al, same));
    assert(!function<int(int)>(allocator_arg, al, move(same)));
    same = nullptr;
    assert(!function<int(int)>(allocator_arg, al, fp));
    assert(!function<int(int)>(allocator_arg, al, other));
#endif // _HAS_FUNCTION_ALLOCATOR_SUPPORT

    int (X::*pmf)(int) = nullptr;
    int X::*pmd        = nullptr;

    assert(!function<int(X, int)>(pmf));
    assert(!function<int(X)>(pmd));

#if _HAS_FUNCTION_ALLOCATOR_SUPPORT
    assert(!function<int(X, int)>(allocator_arg, al, pmf));
    assert(!function<int(X)>(allocator_arg, al, pmd));
#endif // _HAS_FUNCTION_ALLOCATOR_SUPPORT
}
