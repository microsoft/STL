// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

// test <memory>
#define TEST_NAME "<memory>"

#define _HAS_AUTO_PTR_ETC                    1
#define _HAS_DEPRECATED_RAW_STORAGE_ITERATOR 1
#define _HAS_DEPRECATED_TEMPORARY_BUFFER     1
#define _SILENCE_CXX17_OLD_ALLOCATOR_MEMBERS_DEPRECATION_WARNING
#define _SILENCE_CXX17_RAW_STORAGE_ITERATOR_DEPRECATION_WARNING
#define _SILENCE_CXX17_TEMPORARY_BUFFER_DEPRECATION_WARNING

#include "tdefs.h"
#include <memory>
#include <new>
#include <stddef.h>

static CSTD size_t cnt;

class Myint { // wrap an integer
public:
    Myint(int x) : val(x) { // initialize with x and count
        ++cnt;
    }
    Myint(const Myint& x) : val(x.val) { // initialize with copy and count
        ++cnt;
    }

    ~Myint() noexcept { // destroy and count
        --cnt;
    }

    int get_val() const { // get stored value
        return val;
    }

private:
    int val;
};

typedef STD allocator<float> Myal;

Myal get_al() { // return an allocator
    static Myal al;

    return al;
}

void test_alloc() { // test allocators
    float fl                     = 0.0F;
    Myal::size_type* p_size      = (CSTD size_t*) nullptr;
    Myal::difference_type* p_val = (CSTD ptrdiff_t*) nullptr;
    Myal::pointer* p_ptr         = (float**) nullptr;
    Myal::const_pointer* p_cptr  = (const float**) nullptr;
    Myal::reference p_ref        = fl;
    Myal::const_reference p_cref = (const float&) fl;
    Myal::value_type* p_dist     = (float*) nullptr;

    Myal::rebind<int>::other::pointer* p_iptr = (int**) nullptr;

    Myal al0 = get_al(), al(al0);

    p_size = p_size; // to quiet diagnostics
    p_val  = p_val;
    p_ptr  = p_ptr;
    p_cptr = p_cptr;
    p_ref  = p_cref;
    p_ref  = p_ref;
    p_dist = p_dist;
    p_iptr = p_iptr;

    STD allocator<void>::pointer* pv_ptr        = (void**) nullptr;
    STD allocator<void>::const_pointer* pv_cptr = (const void**) nullptr;
    STD allocator<void>::value_type* pv_dist    = (void*) nullptr;

    STD allocator<void>::rebind<int>::other::pointer* pv_iptr = (int**) nullptr;

    STD allocator<void> alv0, alv(alv0);
    alv = alv0;

    pv_ptr  = pv_ptr; // to quiet diagnostics
    pv_cptr = pv_cptr;
    pv_dist = pv_dist;
    pv_iptr = pv_iptr;

    float* pfl = al0.address(fl);
    CHECK_PTR(pfl, &fl);
    pfl = al.allocate(3, nullptr);
    al.construct(&pfl[2], 2.0F);
    CHECK(pfl[2] == 2.0F);
    al.destroy(&pfl[2]);

    float three = 3.0F;
    al.construct(&pfl[1], STD move(three));
    CHECK(pfl[1] == 3.0F);
    al.destroy(&pfl[2]);

    {
        STD allocator<Movable_int> al_mi;
        Movable_int* pmi = al_mi.allocate(1);
        al_mi.construct(pmi, 2, 3);
        CHECK_INT(pmi->val, 0x23);
        al_mi.destroy(pmi);
        al_mi.deallocate(pmi, 1);
    }

    al.deallocate(pfl, 3);
    CHECK(0 < al0.max_size());
    CHECK(al0 == al);
    CHECK(!(al0 != al0));
}

void test_uninit() { // test uninitialized copy and fill
    cnt      = 0;
    Myint* p = (Myint*) operator new(8 * sizeof(Myint));

    STD uninitialized_fill(p, p + 2, 3);
    CHECK_INT(p[1].get_val(), 3);
    CHECK_INT(cnt, 2);
    STD uninitialized_fill_n(p + 2, 2, 5);
    CHECK_INT(p[3].get_val(), 5);
    CHECK_INT(cnt, 4);
    CHECK_PTR(STD uninitialized_copy(p + 1, p + 3, p + 4), p + 6);
    CHECK_INT(p[4].get_val(), 3);
    CHECK_INT(cnt, 6);
    CHECK_INT(p[5].get_val(), 5);

    CHECK_PTR(STD uninitialized_copy_n(p + 1, 2, p + 6), p + 8);
    CHECK_INT(p[6].get_val(), 3);
    CHECK_INT(cnt, 8);
    CHECK_INT(p[7].get_val(), 5);
    CHECK_PTR(STD uninitialized_fill_n(p + 2, 2, 5), p + 4);

    CHECK_PTR(STD addressof(cnt), &cnt);
    operator delete(p);
}

void test_tempbuf() { // test temporary buffers
    STD pair<short*, CSTD ptrdiff_t> tbuf = STD get_temporary_buffer<short>(5);

    CHECK(tbuf.first != nullptr && tbuf.second == 5);

    typedef STD raw_storage_iterator<short*, short> Rit;
    Rit it(tbuf.first);

    for (short i = 0; i < 5; ++i) { // fill the buffer
        *it++ = i;
        CHECK_INT(tbuf.first[i], i);
    }
    STD return_temporary_buffer(tbuf.first);
}

void test_autoptr() { // test auto_ptr
    typedef STD auto_ptr<Myint> Myptr;
    Myptr::element_type* p_elem = (Myint*) nullptr;
    Myptr p0;

    p_elem = p_elem; // to quiet diagnostics

    CHECK_PTR(p0.get(), nullptr);
    { // test explicit allocation and freeing
        cnt = 0;
        Myptr p1(new Myint(3));
        Myint* p = p1.get();
        CHECK_INT(cnt, 1);
        CHECK_INT(p->get_val(), 3);
        CHECK_INT((*p1).get_val(), 3);
        CHECK_INT(p1.release()->get_val(), 3);
        CHECK_PTR(p1.get(), nullptr);
        delete (p);
        CHECK_INT(cnt, 0);
    }
    CHECK_INT(cnt, 0);

    { // test auto freeing
        cnt = 0;
        Myptr p2(new Myint(5));
        CHECK_INT(cnt, 1);
    }
    CHECK_INT(cnt, 0);

    { // test transfers
        cnt = 0;
        Myptr p3(new Myint(7)), p4(p3);
        CHECK_INT(cnt, 1);
        CHECK_PTR(p3.get(), nullptr);
        CHECK_INT(p4.get()->get_val(), 7);
        p3 = p4;
        CHECK_PTR(p4.get(), nullptr);
        CHECK_INT(p3.get()->get_val(), 7);
        p3.reset();
        CHECK_PTR(p3.get(), nullptr);
        CHECK_INT(cnt, 0);
        p4.reset();
        CHECK_PTR(p4.get(), nullptr);
    }
    CHECK_INT(cnt, 0);
}

void test_gc() { // test garbage collection control
    char x;
    STD declare_reachable(&x);
    CHECK_PTR(STD undeclare_reachable(&x), &x);
    STD declare_no_pointers(&x, sizeof(x));
    STD undeclare_no_pointers(&x, sizeof(x));
    CHECK((int) STD get_pointer_safety() == (int) STD pointer_safety::relaxed
          || (int) STD get_pointer_safety() == (int) STD pointer_safety::preferred
          || (int) STD get_pointer_safety() == (int) STD pointer_safety::strict);
}

void test_main() { // test basic properties of memory definitions
    test_alloc();
    test_uninit();
    test_tempbuf();
    test_autoptr();
    test_gc();
}
