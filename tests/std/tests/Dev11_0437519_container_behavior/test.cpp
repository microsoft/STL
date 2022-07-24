// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <array>
#include <cassert>
#include <cstddef>
#include <deque>
#include <forward_list>
#include <iterator>
#include <list>
#include <vector>

using namespace std;

void assert_forward_list_resize_empty() {
    forward_list<int> e;
    e.resize(0);
    assert(e.empty());
}

// This verifies that we can use list<T>::remove to remove elements from a list, even if the
// argument is a reference to an element in the list that will be removed.

struct A {
    A(unsigned int value) : _value(value) {}
    ~A() {
        _value = 0;
    }

    friend bool operator==(A const& lhs, A const& rhs) {
        assert(lhs._value != 0 && rhs._value != 0);
        return lhs._value == rhs._value;
    }

private:
    unsigned int _value;
};

void assert_list_remove_element_in_sequence() {
    list<A> x;
    x.push_back(1);
    x.push_back(2);
    x.push_back(1);
    x.push_back(3);
    x.push_back(1);

    x.remove(*next(x.begin(), 2));

    assert(x.size() == 2);
    assert(*x.begin() == A(2));
    assert(*next(x.begin()) == A(3));

// P0646R1 list/forward_list remove()/remove_if()/unique() Return size_type
#if _HAS_CXX20
    list<A> y{1, 2, 2};

    assert(y.remove(*y.begin()) == 1);
    assert(y.remove(*y.begin()) == 2);

    y = {1, 2, 2};

    assert(y.remove(42) == 0);
    assert(y.remove(1) == 1);
    assert(y.remove(2) == 2);

    y = {2, 3, 3};

    auto pr1 = [](A) { return false; };
    auto pr2 = [](A value) { return value == 2; };
    auto pr3 = [](A value) { return value == 3; };

    assert(y.remove_if(pr1) == 0);
    assert(y.remove_if(pr2) == 1);
    assert(y.remove_if(pr3) == 2);

    y = {4, 4, 5, 5, 5};

    auto pr4 = [](A, A) { return false; };
    auto pr5 = [](A value1, A value2) { return value1 == 4 && value2 == 4; };
    auto pr6 = [](A value1, A value2) { return value1 == 5 && value2 == 5; };

    assert(y.unique(pr4) == 0);
    assert(y.unique(pr5) == 1);
    assert(y.unique(pr6) == 2);

    y = {1, 2, 3};

    assert(y.unique() == 0);

    y.push_back(3);

    assert(y.unique() == 1);

    y.push_back(3);
    y.push_back(3);

    assert(y.unique() == 2);
#endif // _HAS_CXX20
}

void assert_forward_list_remove_element_in_sequence() {
    forward_list<A> x;
    x.push_front(1);
    x.push_front(2);
    x.push_front(1);
    x.push_front(3);
    x.push_front(1);

    x.remove(*next(x.begin(), 2));

    assert(distance(x.begin(), x.end()) == 2);
    assert(*x.begin() == A(3));
    assert(*next(x.begin()) == A(2));

// P0646R1 list/forward_list remove()/remove_if()/unique() Return size_type
#if _HAS_CXX20
    forward_list<A> y{1, 2, 2};

    assert(y.remove(*y.begin()) == 1);
    assert(y.remove(*y.begin()) == 2);

    y = {1, 2, 2};

    assert(y.remove(42) == 0);
    assert(y.remove(1) == 1);
    assert(y.remove(2) == 2);

    y = {2, 3, 3};

    auto pr1 = [](A) { return false; };
    auto pr2 = [](A value) { return value == 2; };
    auto pr3 = [](A value) { return value == 3; };

    assert(y.remove_if(pr1) == 0);
    assert(y.remove_if(pr2) == 1);
    assert(y.remove_if(pr3) == 2);

    y = {4, 4, 5, 5, 5};

    auto pr4 = [](A, A) { return false; };
    auto pr5 = [](A value1, A value2) { return value1 == 4 && value2 == 4; };
    auto pr6 = [](A value1, A value2) { return value1 == 5 && value2 == 5; };

    assert(y.unique(pr4) == 0);
    assert(y.unique(pr5) == 1);
    assert(y.unique(pr6) == 2);

    y = {1, 2, 3};

    assert(y.unique() == 0);

    y.push_front(1);

    assert(y.unique() == 1);

    y.push_front(1);
    y.push_front(1);

    assert(y.unique() == 2);
#endif // _HAS_CXX20
}

// This verifies that the new list assignment logic correctly destroys all nodes
// when an exception is thrown during construction.

struct ListAllocatorException {};

struct B {};

struct C {
    C() {}
    C(B const&) {
        throw ListAllocatorException();
    }
};

struct ListAllocatorContext {
    int allocations;
};

template <typename T>
struct ListAllocator {
    typedef T value_type;

    explicit ListAllocator(ListAllocatorContext* context) : _context(context) {}

    template <typename U>
    ListAllocator(ListAllocator<U> const& other) : _context(other._context) {}

    template <typename U>
    struct rebind {
        typedef ListAllocator<U> other;
    };

    value_type* allocate(size_t n) {
        value_type* p = (value_type*) operator new(n * sizeof(T));
        ++_context->allocations;
        return p;
    }

    void deallocate(value_type* p, size_t) {
        --_context->allocations;
        operator delete(p);
    }

    friend bool operator==(ListAllocator const&, ListAllocator const&) {
        return true;
    }
    friend bool operator!=(ListAllocator const&, ListAllocator const&) {
        return false;
    }

    ListAllocatorContext* _context;
};

void assert_list_node_reuse_exception_handling() {
    ListAllocatorContext context = {0};

    {
        list<C, ListAllocator<C>> x((ListAllocator<C>(&context)));
        int const start_allocations(context.allocations);
        x.push_back(C());
        x.push_back(C());
        x.push_back(C());
        int const after_push_allocations(context.allocations);

        try {
            B b;
            x.assign(&b, &b + 1);
        } catch (ListAllocatorException const&) {
        }

        assert(start_allocations == context.allocations || after_push_allocations == context.allocations);
    }

    assert(context.allocations == 0);
}


// This verifies that resize(n) and resize(n, t) work, especially when t is an element in the sequence.

template <typename C>
void test_resize() {
    {
        C c(4);
        assert(c.size() == 4);
        assert(c[0] == 0);
        assert(c[1] == 0);
        assert(c[2] == 0);
        assert(c[3] == 0);
    }

    const array<int, 5> arr = {{11, 22, 33, 44, 55}};

    const C orig(arr.begin(), arr.end());
    assert(orig.size() == 5);
    assert(orig[0] == 11);
    assert(orig[1] == 22);
    assert(orig[2] == 33);
    assert(orig[3] == 44);
    assert(orig[4] == 55);

    {
        C cEmpty;
        cEmpty.resize(0);
        assert(cEmpty.empty());
    }

    {
        auto c1(orig);
        c1.resize(3);
        assert(c1.size() == 3);
        assert(c1[0] == 11);
        assert(c1[1] == 22);
        assert(c1[2] == 33);
    }

    {
        auto c2(orig);
        c2.resize(7);
        assert(c2.size() == 7);
        assert(c2[0] == 11);
        assert(c2[1] == 22);
        assert(c2[2] == 33);
        assert(c2[3] == 44);
        assert(c2[4] == 55);
        assert(c2[5] == 0);
        assert(c2[6] == 0);
    }

    {
        auto c3(orig);
        c3.resize(2, c3[2]);
        assert(c3.size() == 2);
        assert(c3[0] == 11);
        assert(c3[1] == 22);
    }

    {
        auto c4(orig);
        c4.resize(8, c4[2]);
        assert(c4.size() == 8);
        assert(c4[0] == 11);
        assert(c4[1] == 22);
        assert(c4[2] == 33);
        assert(c4[3] == 44);
        assert(c4[4] == 55);
        assert(c4[5] == 33);
        assert(c4[6] == 33);
        assert(c4[7] == 33);
    }
}

int main() {
    assert_forward_list_resize_empty();
    assert_list_remove_element_in_sequence();
    assert_forward_list_remove_element_in_sequence();
    assert_list_node_reuse_exception_handling();

    test_resize<deque<int>>();
    test_resize<vector<int>>();
}
