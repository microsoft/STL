// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

// test <memory> C++11 additions, part 3
#define TEST_NAME "<memory>, part 3"

#include "tdefs.h"
#include <memory>

#define RBOTHER

// base and derived classes
struct base { // base class
    base(int v = 0) : val(v) { // construct from value
    }

    virtual ~base() noexcept { // destroy the object
    }

    int val;
};

struct derived : public base { // derived class
    derived(int v = 0) : base(v) { // construct from value
    }
};

// counting deleter
struct deleter { // deleter test object
    static int called;
    void operator()(int* ip) { // increment count, delete object
        ++called;
        delete ip;
    }
};
int deleter::called;

// counting deleter for arrays
struct deleter_arr { // deleter test object
    static int called;
    void operator()(int* ip) { // increment count, delete object
        ++called;
        delete[] ip;
    }
};
int deleter_arr::called;

void t_del() { // test default_delete for scalars
    typedef STD default_delete<int> deleter_t;
    deleter_t del;
    int* ptr = new int;
    del(ptr);

    STD default_delete<derived> del1;
    STD default_delete<base> del2(del1);
}

void t_del_arr() { // test default_delete for arrays
    typedef STD default_delete<int[]> deleter_t;
    deleter_t del;

    int* ptr = new int[5];
    del(ptr);
}

void t_unique_ptr() { // test unique_ptr interface for scalars
    typedef STD unique_ptr<int> uptr_t;
    typedef STD default_delete<int> deleter_t;
    bool st = STD is_same<uptr_t::element_type, int>::value;
    CHECK(st);
    st = STD is_same<uptr_t::deleter_type, deleter_t>::value;
    CHECK(st);
    st = STD is_same<uptr_t::pointer, int*>::value;
    CHECK(st);

    uptr_t up0;
    deleter_t del(up0.get_deleter());
    del = del; // to quiet diagnostics

    CHECK(up0.get() == nullptr);
    CHECK(!up0);
    CHECK(up0.release() == nullptr);
    int val = 5;
    up0.reset(&val);
    CHECK(up0.get() == &val);
    CHECK((bool) up0);
    CHECK_INT(*up0, 5);
    CHECK(up0.release() == &val);
    CHECK(up0.release() == nullptr);
    up0 = nullptr;
    CHECK(up0.get() == nullptr);

    { // test explicit deleter
        typedef STD unique_ptr<int, deleter> uptrd_t;
        typedef STD unique_ptr<int, deleter&> uptrdr_t;
        deleter del0;

        deleter::called = 0;
        uptrd_t upd0(new int(10), del0);
        uptrdr_t upd1(new int(10), del0);
    }
    CHECK_INT(deleter::called, 2);

    { // test comparisons
        int a[2] = {0};
        uptr_t upa0(&a[0]);
        uptr_t upa1(&a[1]);

        CHECK(upa0 == upa0);
        CHECK(upa0 != upa1);
        CHECK(upa0 < upa1);
        CHECK(upa1 >= upa0);
        CHECK(upa1 > upa0);
        CHECK(upa0 <= upa1);

        upa0.swap(upa1);
        CHECK(upa1 < upa0);

        upa0.release();
        upa1.release();
    }

    // test moves
    typedef STD unique_ptr<base> uptrb_t;
    typedef STD unique_ptr<derived> uptrd_t;
    uptrb_t up1(new base(6));
    CHECK_INT(up1->val, 6);
    uptrb_t up2(new base(7));
    CHECK_INT(up2->val, 7);
    up1 = STD move(up2);
    CHECK_INT(up1->val, 7);
    CHECK(up2.get() == nullptr);

    uptrd_t up3(new derived(7));
    CHECK_INT(up3->val, 7);

    { // test explicit deleter
        typedef STD unique_ptr<int, deleter> uptrdel_t;
        typedef STD unique_ptr<int, deleter&> uptrdelref_t;
        deleter del0;

        deleter::called = 0;
        uptrdel_t upd0(new int(10), deleter());
        uptrdelref_t upd1(new int(10), del0);
    }
    CHECK_INT(deleter::called, 2);

    // test swap
}

void t_unique_ptr_arr() { // test unique_ptr interface for arrays
    typedef STD unique_ptr<int[]> uptr_t;
    typedef STD default_delete<int[]> deleter_t;
    bool st = STD is_same<uptr_t::element_type, int>::value;
    CHECK(st);
    st = STD is_same<uptr_t::deleter_type, deleter_t>::value;
    CHECK(st);
    st = STD is_same<uptr_t::pointer, int*>::value;
    CHECK(st);

    uptr_t up0;
    deleter_t del(up0.get_deleter());
    del = del; // to quiet diagnostics

    CHECK(up0.get() == nullptr);
    CHECK(!up0);
    CHECK(up0.release() == nullptr);
    int val = 5;
    up0.reset(&val);
    CHECK(up0.get() == &val);
    CHECK((bool) up0);
    CHECK_INT(up0[0], 5);
    CHECK(up0.release() == &val);
    CHECK(up0.release() == nullptr);
    up0 = nullptr;
    CHECK(up0.get() == nullptr);

    { // test explicit deleter
        typedef STD unique_ptr<int[], deleter_arr> uptrd_t;
        typedef STD unique_ptr<int[], deleter_arr&> uptrdr_t;
        deleter_arr del0;

        deleter_arr::called = 0;
        uptrd_t upd0(new int[10], del0);
        uptrdr_t upd1(new int[10], del0);
    }
    CHECK_INT(deleter_arr::called, 2);

    { // test comparisons
        int a[2] = {0};
        uptr_t upa0(&a[0]);
        uptr_t upa1(&a[1]);

        CHECK(upa0 == upa0);
        CHECK(upa0 != upa1);
        CHECK(upa0 < upa1);
        CHECK(upa1 >= upa0);
        CHECK(upa1 > upa0);
        CHECK(upa0 <= upa1);

        upa0.swap(upa1);
        CHECK(upa1 < upa0);

        upa0.release();
        upa1.release();
    }

    // test moves
    typedef STD unique_ptr<base[]> uptrb_t;
    uptrb_t up1(new base[2]);
    up1[1].val = 6;
    CHECK_INT(up1[1].val, 6);
    uptrb_t up2(new base[2]);
    up2[1].val = 7;
    CHECK_INT(up2[1].val, 7);
    up1 = STD move(up2);
    CHECK_INT(up1[1].val, 7);
    CHECK(up2.get() == nullptr);

    { // test explicit deleter
        typedef STD unique_ptr<int[], deleter_arr> uptrd_t;
        typedef STD unique_ptr<int[], deleter_arr&> uptrdr_t;
        deleter_arr del0;

        deleter_arr::called = 0;
        uptrd_t upd0(new int(10), deleter_arr());
        uptrdr_t upd1(new int(10), del0);
    }
    CHECK_INT(deleter_arr::called, 2);
}

void t_hash() { // test hash functions
    typedef STD shared_ptr<int> sptr_t;
    sptr_t sp0;
    CSTD size_t hash_val = STD hash<sptr_t>()(sp0);
    (void) hash_val;

    typedef STD unique_ptr<int> uptr_t;
    uptr_t up0;
    hash_val = STD hash<uptr_t>()(up0);
}

template <class Ty>
struct Ptr { // wraps a pointer
    typedef Ty* pointer;

    Ptr(Ty* addr) : myaddr(addr) { // construct from raw pointer
    }

    Ty operator*() { // dereference pointer
        return *myaddr;
    }

    static Ptr<Ty> pointer_to(Ty& addr) { // get address
        return Ptr(&addr);
    }

    Ty* myaddr;
};

void t_pointer_traits() { // test pointer_traits
    { // test raw pointers
        typedef STD pointer_traits<char*> Mytraits;
        CHECK_TYPE(Mytraits::element_type, char);
        CHECK_TYPE(Mytraits::pointer, char*);
        CHECK_TYPE(Mytraits::difference_type, STD ptrdiff_t);
        CHECK_TYPE(Mytraits::rebind<int> RBOTHER, int*);

        char val;
        CHECK_PTR(Mytraits::pointer_to(val), &val);
    }

    { // test arbitrary class
        typedef STD pointer_traits<Ptr<char>> Mytraits;
        CHECK_TYPE(Mytraits::element_type, char);
        CHECK_TYPE(Mytraits::pointer, Ptr<char>);
        CHECK_TYPE(Mytraits::difference_type, STD ptrdiff_t);
        CHECK_TYPE(Mytraits::rebind<int> RBOTHER, Ptr<int>);

        char val = 3;
        CHECK_INT(*Mytraits::pointer_to(val), 3);
    }

    { // test void pointer
        typedef STD pointer_traits<void*> Mytraits;
        CHECK_TYPE(Mytraits::element_type, void);
        CHECK_TYPE(Mytraits::pointer, void*);
        CHECK_TYPE(Mytraits::difference_type, STD ptrdiff_t);
        CHECK_TYPE(Mytraits::rebind<int> RBOTHER, int*);

        char val;
        CHECK_PTR((char*) Mytraits::pointer_to(val), &val);
    }
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

void t_allocator_traits() { // test allocator_traits
    { // test against standard allocator
        typedef STD allocator_traits<STD allocator<char>> Mytraits;
        CHECK_TYPE(Mytraits::allocator_type, STD allocator<char>);
        CHECK_TYPE(Mytraits::value_type, char);

        CHECK_TYPE(Mytraits::pointer, char*);
        CHECK_TYPE(Mytraits::const_pointer, const char*);
        CHECK_TYPE(Mytraits::void_pointer, void*);
        CHECK_TYPE(Mytraits::const_void_pointer, const void*);

        CHECK_TYPE(Mytraits::size_type, STD size_t);
        CHECK_TYPE(Mytraits::difference_type, STD ptrdiff_t);

        CHECK_TYPE(Mytraits::rebind_alloc<int> RBOTHER, STD allocator<int>);
        CHECK_TYPE(Mytraits::rebind_traits<int> RBOTHER, STD allocator_traits<STD allocator<int>>);

        CHECK_TYPE(Mytraits::propagate_on_container_copy_assignment, STD false_type);
        CHECK_TYPE(Mytraits::propagate_on_container_move_assignment, STD true_type);
        CHECK_TYPE(Mytraits::propagate_on_container_swap, STD false_type);

        CHECK_TYPE(Mytraits::is_always_equal, STD true_type);

        STD allocator<char> myal;
        char* pch = Mytraits::allocate(myal, 1);
        CHECK(pch != nullptr);
        if (pch != nullptr) { // avoid tests if allocate fails
            Mytraits::construct(myal, pch, 'x');
            CHECK_INT(*pch, 'x');
            Mytraits::destroy(myal, pch);
            Mytraits::deallocate(myal, pch, 1);
        }

        CHECK(Mytraits::max_size(myal) == (STD size_t) (-1));
        CHECK(Mytraits::select_on_container_copy_construction(myal) == myal);
    }

    { // test against minimal allocator
        Min_alloc<char> myal;
        typedef STD allocator_traits<Min_alloc<char>> Mytraits;
        CHECK_TYPE(Mytraits::allocator_type, Min_alloc<char>);
        CHECK_TYPE(Mytraits::value_type, char);

        CHECK_TYPE(Mytraits::pointer, char*);
        CHECK_TYPE(Mytraits::const_pointer, const char*);
        CHECK_TYPE(Mytraits::void_pointer, void*);
        CHECK_TYPE(Mytraits::const_void_pointer, const void*);

        CHECK_TYPE(Mytraits::size_type, STD size_t);
        CHECK_TYPE(Mytraits::difference_type, STD ptrdiff_t);

        CHECK_TYPE(Mytraits::rebind_alloc<int> RBOTHER, Min_alloc<int>);
        CHECK_TYPE(Mytraits::rebind_traits<int> RBOTHER, STD allocator_traits<Min_alloc<int>>);

        CHECK_TYPE(Mytraits::propagate_on_container_copy_assignment, STD false_type);
        CHECK_TYPE(Mytraits::propagate_on_container_move_assignment, STD false_type);
        CHECK_TYPE(Mytraits::propagate_on_container_swap, STD false_type);

        char* pch = Mytraits::allocate(myal, 1);
        CHECK(pch != nullptr);
        Mytraits::construct(myal, pch, 'x');
        CHECK_INT(*pch, 'x');
        Mytraits::destroy(myal, pch);
        Mytraits::deallocate(myal, pch, 1);
        CHECK(Mytraits::max_size(myal) == (STD size_t) (-1));
        CHECK(Mytraits::select_on_container_copy_construction(myal) == myal);
    }
}

STD allocator_arg_t arg(STD allocator_arg);

#define NSPACE 128
struct Space { // space for testing align
    char ch;
    char buf[NSPACE];
} space;

void t_align() { // test align
    void* ptr      = space.buf;
    STD size_t len = NSPACE;
    CHECK_PTR(STD align(1, NSPACE + 1, ptr, len), (void*) nullptr);
    CHECK_PTR(ptr, space.buf);
    CHECK_INT(len, NSPACE);
    CHECK_PTR(STD align(1, 1, ptr, len), space.buf);
    CHECK_PTR(ptr, space.buf);
    CHECK_INT(len, NSPACE);
    void* adjusted = STD align(4, 4, ptr, len);
    CHECK(adjusted == ptr);
    CHECK((char*) ptr >= space.buf);
    CHECK((char*) ptr < space.buf + 4);
    CHECK(len > NSPACE - 4);
    CHECK(len <= NSPACE);
}

void test_main() { // run tests
    t_del();
    t_del_arr();
    t_unique_ptr();
    t_hash();
    t_pointer_traits();
    t_allocator_traits();
    t_align();
}
