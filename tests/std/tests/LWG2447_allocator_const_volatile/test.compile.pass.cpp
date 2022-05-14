// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <deque>
#include <forward_list>
#include <list>
#include <map>
#include <set>
#include <string>
#include <vector>

#if _HAS_CXX17
#include <memory_resource>
#endif // _HAS_CXX17

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

#if _HAS_CXX17
    pmr::vector<volatile int> pv_v;
    pmr::deque<volatile int> pd_v;
    pmr::forward_list<volatile int> pf_v;
    pmr::list<volatile int> pl_v;
    pmr::set<volatile int> ps_v;
    pmr::multiset<volatile int> pms_v;
    pmr::map<volatile int, volatile int> pm_v;
    pmr::multimap<volatile int, volatile int> pmm_v;

    pmr::vector<const volatile int> pv_cv;
    pmr::deque<const volatile int> pd_cv;
    pmr::forward_list<const volatile int> pf_cv;
    pmr::list<const volatile int> pl_cv;
    pmr::set<const volatile int> ps_cv;
    pmr::multiset<const volatile int> pms_cv;
    pmr::map<const volatile int, const volatile int> pm_cv;
    pmr::multimap<const volatile int, const volatile int> pmm_cv;
#endif // _HAS_CXX17
}

int main() {} // COMPILE-ONLY
