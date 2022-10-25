// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#define _SILENCE_STDEXT_HASH_DEPRECATION_WARNINGS

#include <array>
#include <cstddef>
#include <cstdlib>
#ifndef _M_CEE_PURE
#include <atomic>
#endif // _M_CEE_PURE
#include <cassert>
#include <deque>
#include <exception>
#include <forward_list>
#include <functional>
#include <hash_map>
#include <hash_set>
#include <istream>
#include <iterator>
#include <list>
#include <map>
#include <memory>
#include <queue>
#include <set>
#include <stack>
#include <tuple>
#include <type_traits>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

using namespace std;
using namespace stdext;

#define STATIC_ASSERT(...) static_assert(__VA_ARGS__, #__VA_ARGS__)

namespace Meow {
    struct Evil {
        int data;

        constexpr Evil() noexcept : data(1701) {}

        template <int = 0>
        const Evil& operator=(const Evil&) const {
            // provide an Evil operator= for std::tuple::swap
            return *this;
        }

        int func() const {
            return 1729;
        }

        Evil* get() {
            return this;
        }

        constexpr const Evil* get() const {
            return this;
        }

        void operator&() const = delete;

        template <typename T>
        operator T&() const = delete;

        bool operator<(const Evil&) const {
            return false;
        }

        bool operator==(const Evil&) const {
            return true;
        }
    };

    istream& operator>>(istream& is, Evil&) {
        return is;
    }

    // If it's dragged in via ADL, this op,() will absorb anything.
    template <typename T, typename U>
    void operator,(const T&, const U&) = delete;

    // VSO-258601 "std::vector should not rely on operator& in Debug mode"
    // If it's dragged in via ADL, this op&() will absorb anything.
    template <typename T>
    void operator&(const T&) = delete;

    size_t hash_value(const Evil&) noexcept {
        // provide an Evil hash value for stdext::hash_compare
        return 0;
    }
} // namespace Meow

using Meow::Evil;

struct Hash {
    size_t operator()(const Evil&) const {
        return 0;
    }
};

template class std::array<Evil, 5>;
template class std::array<Evil, 0>;
template class std::deque<Evil>;
template class std::forward_list<Evil>;
template class std::list<Evil>;
template class std::vector<Evil>;

template class std::priority_queue<Evil>;
template class std::queue<Evil>;
template class std::stack<Evil>;

template struct std::pair<Evil, Evil>;
template class std::tuple<Evil, Evil, Evil>;

template class std::map<Evil, Evil>;
template class std::multimap<Evil, Evil>;
template class std::set<Evil>;
template class std::multiset<Evil>;

template class stdext::hash_map<Evil, Evil>;
template class stdext::hash_multimap<Evil, Evil>;
template class stdext::hash_set<Evil>;
template class stdext::hash_multiset<Evil>;

template class std::unordered_map<Evil, Evil, Hash>;
template class std::unordered_multimap<Evil, Evil, Hash>;
template class std::unordered_set<Evil, Hash>;
template class std::unordered_multiset<Evil, Hash>;


template class std::_Array_iterator<Evil, 5>;
template class std::_Array_const_iterator<Evil, 5>;

template class std::_Deque_iterator<_Deque_val<_Deque_simple_types<Evil>>>;
template class std::_Deque_const_iterator<_Deque_val<_Deque_simple_types<Evil>>>;

template class std::_Flist_iterator<_Flist_val<_Flist_simple_types<Evil>>>;
template class std::_Flist_const_iterator<_Flist_val<_Flist_simple_types<Evil>>>;

template class std::_List_iterator<_List_val<_List_simple_types<Evil>>>;
template class std::_List_const_iterator<_List_val<_List_simple_types<Evil>>>;

template class std::_Vector_iterator<_Vector_val<_Simple_types<Evil>>>;
template class std::_Vector_const_iterator<_Vector_val<_Simple_types<Evil>>>;

template class std::_Tree_iterator<_Tree_val<_Tree_simple_types<pair<const Evil, Evil>>>>;
template class std::_Tree_const_iterator<_Tree_val<_Tree_simple_types<pair<const Evil, Evil>>>>;

template class std::_Tree_const_iterator<_Tree_val<_Tree_simple_types<Evil>>>;

template class std::_List_iterator<_List_val<_List_simple_types<pair<const Evil, Evil>>>>;
template class std::_List_const_iterator<_List_val<_List_simple_types<pair<const Evil, Evil>>>>;


template class std::_Deque_unchecked_iterator<_Deque_val<_Deque_simple_types<Evil>>>;
template class std::_Deque_unchecked_const_iterator<_Deque_val<_Deque_simple_types<Evil>>>;

template class std::_Flist_unchecked_iterator<_Flist_val<_Flist_simple_types<Evil>>>;
template class std::_Flist_unchecked_const_iterator<_Flist_val<_Flist_simple_types<Evil>>>;

template class std::_List_unchecked_iterator<_List_val<_List_simple_types<Evil>>>;
template class std::_List_unchecked_const_iterator<_List_val<_List_simple_types<Evil>>>;

template class std::_Tree_unchecked_iterator<_Tree_val<_Tree_simple_types<pair<const Evil, Evil>>>>;
template class std::_Tree_unchecked_const_iterator<_Tree_val<_Tree_simple_types<pair<const Evil, Evil>>>>;

template class std::_Tree_unchecked_const_iterator<_Tree_val<_Tree_simple_types<Evil>>>;

template class std::_List_unchecked_iterator<_List_val<_List_simple_types<pair<const Evil, Evil>>>>;
template class std::_List_unchecked_const_iterator<_List_val<_List_simple_types<pair<const Evil, Evil>>>>;


template class std::reverse_iterator<Evil*>;
template class std::move_iterator<Evil*>;
template class std::istream_iterator<Evil>;
template class stdext::checked_array_iterator<Evil*>;
template class stdext::unchecked_array_iterator<Evil*>;


template class std::shared_ptr<Evil>;
template class std::weak_ptr<Evil>;
template class std::unique_ptr<Evil>;
template class std::unique_ptr<Evil[]>;


template class std::reference_wrapper<Evil>;
template class std::reference_wrapper<const Evil>;


#ifndef _M_CEE_PURE
// VSO-198738 "<atomic>: atomic<T> should support overloaded op&"
template struct std::atomic<Evil>;
#endif // _M_CEE_PURE


void test(Evil* p, Evil* correct, int i) {
    assert(p == correct && i == 1);
}

void test(const Evil* p, Evil* correct, int i) {
    assert(p == correct && i == 2);
}

void test(volatile Evil* p, Evil* correct, int i) {
    assert(p == correct && i == 3);
}

void test(const volatile Evil* p, Evil* correct, int i) {
    assert(p == correct && i == 4);
}

void test_LWG_2296();

int main() {
    Evil e;

    assert(addressof(e) == e.get());

    Evil& m                 = e;
    const Evil& c           = e;
    volatile Evil& v        = e;
    const volatile Evil& cv = e;

    test(addressof(m), e.get(), 1);
    test(addressof(c), e.get(), 2);
    test(addressof(v), e.get(), 3);
    test(addressof(cv), e.get(), 4);

    (void) ref(e);
    (void) ref(c);
    (void) cref(e);
    (void) cref(c);

    assert(mem_fn(&Evil::data)(e) == 1701);
    assert(mem_fn(&Evil::func)(e) == 1729);

    Evil arr[5];
    vector<Evil> vec(begin(arr), end(arr));

    (void) make_exception_ptr(e);

    Evil arr2[5];
    swap(arr, arr2);
    swap(arr[0], arr2[0]);

    test_LWG_2296();
}


// LWG-2296 "std::addressof should be constexpr"

constexpr int g_int = 7;
constexpr Evil g_evil{};
int func(int x) {
    return x * x;
}
constexpr int g_array[3] = {11, 22, 33};

void test_LWG_2296() {
    STATIC_ASSERT(addressof(g_int) == &g_int);
    STATIC_ASSERT(addressof(g_evil) == g_evil.get());
    STATIC_ASSERT(addressof(func) == &func);
    STATIC_ASSERT(addressof(g_array) == &g_array);

    assert(addressof(g_int) == &g_int);
    assert(addressof(g_evil) == g_evil.get());
    assert(addressof(func) == &func);
    assert(addressof(g_array) == &g_array);

    int i                  = 11;
    const int ci           = 22;
    volatile int vi        = 33;
    const volatile int cvi = 44;

    assert(addressof(i) == &i);
    assert(addressof(ci) == &ci);
    assert(addressof(vi) == &vi);
    assert(addressof(cvi) == &cvi);
}

// LWG-2598 "addressof works on temporaries [and should not]"

template <typename, typename = void>
constexpr bool can_addressof = false;
template <typename T>
constexpr bool can_addressof<T, void_t<decltype(addressof(declval<T>()))>> = true;

void test_LWG_2598() { // COMPILE-ONLY
    STATIC_ASSERT(can_addressof<int&>);
    STATIC_ASSERT(can_addressof<const int&>);
    STATIC_ASSERT(can_addressof<volatile int&>);
    STATIC_ASSERT(can_addressof<const volatile int&>);
    STATIC_ASSERT(!can_addressof<int>);
    STATIC_ASSERT(!can_addressof<const int>);
    STATIC_ASSERT(!can_addressof<volatile int>);
    STATIC_ASSERT(!can_addressof<const volatile int>);
}

// Also test DevCom-1134328, in which `deque<S*>::_Unchecked_iterator{} - 1` finds
// operator-(const S&, int) by argument-dependent lookup causing overload resolution
// to fail due to ambiguity when compiling 64-bit.
struct S {
    S() = default;

    template <typename T>
    S(T&&);
};

S operator-(const S&, int);

void test_DevCom_1134328() { // COMPILE-ONLY
    deque<S*> d{nullptr};
    (void) d.back();
}
