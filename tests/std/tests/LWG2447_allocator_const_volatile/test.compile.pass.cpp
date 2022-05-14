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

// LWG-2447 doesn't say the default allocator doesn't support cv-qualified types, it says that a type need not support
// cv-qualified types to satisfy the allocator requirements.
void test_lwg_2447() {
#if !_HAS_DEPRECATED_ALLOCATOR_MEMBERS
    vector<const volatile int> v_cv;
    deque<const volatile int> d_cv;
    forward_list<const volatile int> f_cv;
    list<const volatile int> l_cv;
    set<const volatile int> s_cv;
    multiset<const volatile int> ms_cv;
    map<const volatile int, const volatile int> m_cv;
    multimap<const volatile int, const volatile int> mm_cv;
#endif // !_HAS_DEPRECATED_ALLOCATOR_MEMBERS
    vector<volatile int> v_v;
    deque<volatile int> d_v;
    forward_list<volatile int> f_v;
    list<volatile int> l_v;
    set<volatile int> s_v;
    multiset<volatile int> ms_v;
    map<volatile int, volatile int> m_v;
    multimap<volatile int, volatile int> mm_v;
}

int main() {} // COMPILE-ONLY
