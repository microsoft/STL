// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

// test <new>
#define TEST_NAME "<new>"

#include "tdefs.h"
#include <assert.h>
#include <new>
#include <string.h>

#ifdef _M_CEE_PURE
void test_main() { // test nothing
}
#else // _M_CEE_PURE
static int destructor_called = 0;

class abc { // wrap a string
public:
    abc(const char* s = "xxx") { // construct from NTBS
        CSTD strcpy(buf, s);
    }

    ~abc() noexcept { // count destructor calls
        ++destructor_called;
    }

    char* ptr() { // return pointer to string
        return buf;
    }

private:
    char buf[10];
};

static void nop() { // dummy new handler
}

static int subs_called = 0;

_Ret_notnull_ _Post_writable_byte_size_(size) void* operator new[](
    CSTD size_t size) { // replacement for array operator new
    subs_called += 1;
    return operator new(size);
}

void operator delete[](void* p) noexcept { // replacement for array operator delete
    subs_called += 2;
    operator delete(p);
}

void test_main() { // test basic workings of new definitions
    STD bad_alloc x1;

    STD bad_array_new_length x2;

    STD nothrow_t nt          = STD nothrow;
    STD new_handler save_hand = STD set_new_handler(&nop);

    nt = nt; // to quiet diagnostics

    CHECK(STD get_new_handler() == &nop);

    CHECK(STD set_new_handler(save_hand) == &nop);

    // test new, placement new, and delete
    destructor_called = 0;
    abc* p            = new abc("first");
    assert(p != nullptr);
    CHECK_STR(p->ptr(), "first");
    CHECK_PTR(new (p) abc("second"), (void*) p);
    CHECK_STR(p->ptr(), "second");

    operator delete(p, p);
    CHECK_INT(destructor_called, 0);
    delete p;
    CHECK_INT(destructor_called, 1);
    p = new (STD nothrow) abc("third");
    assert(p != nullptr);
    operator delete(p, STD nothrow);
    CHECK_INT(destructor_called, 1);

    // test array new, placement new, and delete
    destructor_called = 0;
    subs_called       = 0;
    abc* p2           = new abc[1];
    assert(p2 != nullptr);
    CHECK_STR(p2->ptr(), "xxx");
    CHECK_PTR(new (p2) abc("second"), static_cast<void*>(p2));
    CHECK_STR(p2->ptr(), "second");

    operator delete[](p2, p2);
    CHECK_INT(destructor_called, 0);
    delete[] p2;
    CHECK_INT(destructor_called, 1);
}
#endif // _M_CEE_PURE
