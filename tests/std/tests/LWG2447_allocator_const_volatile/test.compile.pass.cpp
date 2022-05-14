// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <deque>
#include <forward_list>
#include <list>
#include <map>
#include <set>
#include <string>
#include <vector>
using namespace std;

// LWG2447: Allocators and volatile-qualified value types
//  LWG-2447 doesn't say the default allocator doesn't support cv-qualified types, it says that a type need not support
//  cv-qualified types to satisfy the allocator requirements.
void test_lwg_2447() {
    vector<const volatile int> v;
    deque<const volatile int> d;
    forward_list<const volatile int> f;
    list<const volatile int> l;
    set<const volatile int> s;
    multiset<const volatile int> ms;
    map<const volatile int, const volatile int> m;
    multimap<const volatile int, const volatile int> mm;
}

int main() {} // COMPILE-ONLY
