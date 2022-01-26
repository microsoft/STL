// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

// test <scoped_allocator> C++11 additions
#define TEST_NAME "<scoped_allocator>"

#include "tdefs.h"
#include <scoped_allocator>
#include <tuple>

typedef STD allocator<char> Myal_char;
typedef STD allocator<int> Myal_int;
typedef STD allocator<float> Myal_float;

typedef STD scoped_allocator_adaptor<Myal_char> Myscoped1c;
typedef STD scoped_allocator_adaptor<Myal_int> Myscoped1i;
typedef STD scoped_allocator_adaptor<Myal_float> Myscoped1f;

int one = 1;
int two = 2;

void t_one() { // test one allocator
    CHECK_TYPE(Myscoped1i::outer_allocator_type, Myal_int);
    CHECK_TYPE(Myscoped1i::inner_allocator_type, Myscoped1i);

    CHECK_TYPE(Myscoped1i::value_type, int);
    CHECK_TYPE(Myscoped1i::pointer, int*);
    CHECK_TYPE(Myscoped1i::const_pointer, const int*);
    CHECK_TYPE(Myscoped1i::void_pointer, void*);
    CHECK_TYPE(Myscoped1i::const_void_pointer, const void*);

    CHECK_TYPE(Myscoped1i::size_type, STD size_t);
    CHECK_TYPE(Myscoped1i::difference_type, STD ptrdiff_t);

    CHECK_TYPE(Myscoped1i::propagate_on_container_copy_assignment, STD false_type);
    CHECK_TYPE(Myscoped1i::propagate_on_container_move_assignment, STD true_type);
    CHECK_TYPE(Myscoped1i::propagate_on_container_swap, STD false_type);

    CHECK_TYPE(Myscoped1i::is_always_equal, STD true_type);

    CHECK_TYPE(Myscoped1i::rebind<float>::other, Myscoped1f);

    Myscoped1c al1c;
    Myscoped1i al1i(al1c);
    Myscoped1f al1f = al1c;
    CHECK(al1i == al1f);
    CHECK(!(al1i != al1f));
}

typedef STD scoped_allocator_adaptor<Myal_char, Myal_float> Myscoped2cf;
typedef STD scoped_allocator_adaptor<Myal_int, Myal_float> Myscoped2if;
typedef STD scoped_allocator_adaptor<Myal_float, Myal_float> Myscoped2ff;

void t_two() { // test two allocators
    CHECK_TYPE(Myscoped2cf::outer_allocator_type, Myal_char);
    CHECK_TYPE(Myscoped2cf::inner_allocator_type, Myscoped1f);
    CHECK_TYPE(Myscoped2cf::value_type, char);
    CHECK_TYPE(Myscoped2cf::pointer, char*);
    CHECK_TYPE(Myscoped2cf::const_pointer, const char*);
    CHECK_TYPE(Myscoped2cf::void_pointer, void*);
    CHECK_TYPE(Myscoped2cf::const_void_pointer, const void*);

    CHECK_TYPE(Myscoped2cf::size_type, STD size_t);
    CHECK_TYPE(Myscoped2cf::difference_type, STD ptrdiff_t);

    CHECK_TYPE(Myscoped2cf::propagate_on_container_copy_assignment, STD false_type);
    CHECK_TYPE(Myscoped2cf::propagate_on_container_move_assignment, STD true_type);
    CHECK_TYPE(Myscoped2cf::propagate_on_container_swap, STD false_type);

    CHECK_TYPE(Myscoped2cf::rebind<float>::other, Myscoped2ff);
    Myscoped2cf al2cf;
    Myscoped2if al2if(al2cf);
    al2if = al2cf;

    Myscoped2ff al2ff = al2cf;
    CHECK(al2ff == al2cf);
    CHECK(!(al2ff != al2cf));

    CHECK(Myscoped2cf() == Myscoped2ff());
    CHECK(!(Myscoped2cf() != Myscoped2ff()));
}

typedef STD scoped_allocator_adaptor<Myal_char, Myal_int, Myal_float> Myscoped3cif;
typedef STD scoped_allocator_adaptor<Myal_int, Myal_int, Myal_float> Myscoped3iif;
typedef STD scoped_allocator_adaptor<Myal_float, Myal_int, Myal_float> Myscoped3fif;

void t_three() { // test three allocators
    CHECK_TYPE(Myscoped3cif::outer_allocator_type, Myal_char);
    CHECK_TYPE(Myscoped3cif::inner_allocator_type, Myscoped2if);
    CHECK_TYPE(Myscoped3cif::value_type, char);
    CHECK_TYPE(Myscoped3cif::pointer, char*);
    CHECK_TYPE(Myscoped3cif::const_pointer, const char*);
    CHECK_TYPE(Myscoped3cif::void_pointer, void*);
    CHECK_TYPE(Myscoped3cif::const_void_pointer, const void*);

    CHECK_TYPE(Myscoped3cif::size_type, STD size_t);
    CHECK_TYPE(Myscoped3cif::difference_type, STD ptrdiff_t);

    CHECK_TYPE(Myscoped3cif::propagate_on_container_copy_assignment, STD false_type);
    CHECK_TYPE(Myscoped3cif::propagate_on_container_move_assignment, STD true_type);
    CHECK_TYPE(Myscoped3cif::propagate_on_container_swap, STD false_type);

    CHECK_TYPE(Myscoped3cif::rebind<int>::other, Myscoped3iif);
    Myscoped3cif al3cif;
    Myscoped3iif al3iif(al3cif);
    al3iif = al3cif;

    Myscoped3fif al3fif = al3cif;
    CHECK(al3fif == al3cif);
    CHECK(!(al3fif != al3cif));

    CHECK(Myscoped3cif() == Myscoped3fif());
    CHECK(!(Myscoped3cif() != Myscoped3fif()));
}

template <class Ty>
class Min_alloc { // minimal allocator
public:
    typedef Ty value_type;

    Min_alloc() {}
    template <class Other>
    Min_alloc(const Min_alloc<Other>&) {}

    Ty* allocate(size_t count) {
        return (Ty*) ::operator new(count * sizeof(Ty));
    }

    void deallocate(Ty* ptr, size_t) {
        ::operator delete(ptr);
    }
};

template <class Ty, class Other>
bool operator==(const Min_alloc<Ty>&, const Min_alloc<Other>&) {
    return true;
}

template <class Ty, class Other>
bool operator!=(const Min_alloc<Ty>&, const Min_alloc<Other>&) {
    return false;
}

void t_minimal() { // test against minimal allocator
    STD scoped_allocator_adaptor<Min_alloc<char>> myal;

    char* pch = myal.allocate(1);
    CHECK(pch != nullptr);
    myal.construct(pch, 'x');
    CHECK_INT(*pch, 'x');
    myal.destroy(pch);
    myal.deallocate(pch, 1);

    CHECK(myal.max_size() == (STD size_t)(-1));
    CHECK(myal.select_on_container_copy_construction() == myal);
}

void t_pair() { // test pair construction
    typedef STD pair<int, int> Pairii;
    typedef Myscoped1i::rebind<Pairii>::other Myscopedpii;
    Myscopedpii mypal;

    Pairii* pii = mypal.allocate(1);
    mypal.construct(pii, STD piecewise_construct, STD forward_as_tuple(one), STD forward_as_tuple(two));
    CHECK_INT(pii->first, 1);
    CHECK_INT(pii->second, 2);
    mypal.destroy(pii);
    mypal.deallocate(pii, 1);
}

struct Allocable1 { // supports leading allocator only
    Allocable1(STD allocator_arg_t, const STD allocator<int>&) : Myval(0) {}

    Allocable1(STD allocator_arg_t, const STD allocator<int>&, const Allocable1& right) : Myval(right.Myval) {}

    Allocable1(STD allocator_arg_t, const STD allocator<int>&, int val) : Myval(val) {}

    Allocable1(int) : Myval(-2) {}

    Allocable1() : Myval(-3) {}

    int Myval;
};

struct Allocable2 { // supports trailing allocator only
    Allocable2(const STD allocator<int>&) : Myval(0) {}

    Allocable2(const Allocable2& right, const STD allocator<int>&) : Myval(right.Myval) {}

    Allocable2(int val, const STD allocator<int>&) : Myval(val) {}

    Allocable2(int) : Myval(-2) {}

    Allocable2() : Myval(-3) {}

    int Myval;
};

namespace std {
    template <class Alloc>
    struct uses_allocator<Allocable1, Alloc> : true_type {};

    template <class Alloc>
    struct uses_allocator<Allocable2, Alloc> : true_type {};
} // namespace std

void t_pair_alloc() { // test pair construction with allocators
    { // test no allocators
        typedef STD pair<int, int> Pair1a;
        typedef STD scoped_allocator_adaptor<STD allocator<Pair1a>> Myscopedp1a;
        Myscopedp1a mypal;

        Pair1a* p1a = mypal.allocate(1);
        CHECK(p1a != nullptr);
        mypal.construct(p1a, STD piecewise_construct, STD forward_as_tuple(one), STD forward_as_tuple(two));
        CHECK_INT(p1a->first, 1);
        CHECK_INT(p1a->second, 2);
        mypal.destroy(p1a);
        mypal.deallocate(p1a, 1);
    }

    { // test leading allocators
        typedef STD pair<Allocable1, Allocable1> Pair1a;
        typedef STD scoped_allocator_adaptor<STD allocator<Pair1a>> Myscopedp1a;
        Myscopedp1a mypal;

        Pair1a* p1a = mypal.allocate(1);
        CHECK(p1a != nullptr);
        mypal.construct(p1a, STD piecewise_construct, STD forward_as_tuple(one), STD forward_as_tuple(two));
        CHECK_INT(p1a->first.Myval, 1);
        CHECK_INT(p1a->second.Myval, 2);
        mypal.destroy(p1a);
        mypal.deallocate(p1a, 1);
    }

    { // test trailing allocators
        typedef STD pair<Allocable2, Allocable2> Pair1a;
        typedef STD scoped_allocator_adaptor<STD allocator<Pair1a>> Myscopedp1a;
        Myscopedp1a mypal;

        Pair1a* p1a = mypal.allocate(1);
        CHECK(p1a != nullptr);
        mypal.construct(p1a, STD piecewise_construct, STD forward_as_tuple(one), STD forward_as_tuple(two));
        CHECK_INT(p1a->first.Myval, 1);
        CHECK_INT(p1a->second.Myval, 2);
        mypal.destroy(p1a);
        mypal.deallocate(p1a, 1);
    }
}

void test_main() { // run tests
    t_one();
    t_two();
    t_three();
    t_minimal();
    t_pair();
    t_pair_alloc();
}
