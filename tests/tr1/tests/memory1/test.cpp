// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

// test <memory>, part 1
#define TEST_NAME "<memory>, part 1"

#define _HAS_AUTO_PTR_ETC                 1
#define _HAS_DEPRECATED_SHARED_PTR_UNIQUE 1
#define _SILENCE_CXX17_SHARED_PTR_UNIQUE_DEPRECATION_WARNING

#include "tdefs.h"
#include <memory>
#include <sstream>
#include <string>

static void t_bad_weak_ptr() { // test bad_weak_ptr
    STD bad_weak_ptr ptr;
    STD exception* eptr = &ptr;

    eptr = eptr; // to quiet diagnostics
    CHECK_STR(ptr.what(), "bad_weak_ptr");
}

struct X0 { // counted object
    X0() { // construct and increment count
        ++objects;
    }
    X0(const X0&) { // construct copy and increment count
        ++objects;
    }
    virtual void f() { // empty
    }
    virtual ~X0() noexcept { // decrement count
        --objects;
    }
    static int objects;
};
int X0::objects;

struct X;

struct X1 { // alternate base object
    virtual STD shared_ptr<X> getX();
    virtual ~X1() noexcept { // do nothing
    }
};

struct X : public X0, virtual public X1, public STD enable_shared_from_this<X> { // slightly complicated base type
    virtual void f() {}
    virtual STD shared_ptr<X> getX() { // return shared_ptr to this
        STD shared_ptr<X> sp = shared_from_this();
        CHECK_PTR(sp.get(), this);
        return sp;
    }
};

STD shared_ptr<X> X1::getX() { // return shared_ptr object
    return STD shared_ptr<X>();
}

struct X2 : X { // test type
};

struct deleter { // deleter test object
    static int called;
    void operator()(X0* ip) { // increment count, delete object
        ++called;
        delete ip;
    }
};
int deleter::called;

static void t_shared_ptr() { // test shared_ptr interface

    { // shared_ptr(), operator (user-defined, equivalent to bool)
        deleter::called = 0;
        STD shared_ptr<X0> sp0;
        CHECK_INT(sp0.use_count(), 0);
        CHECK_PTR(sp0.get(), nullptr);
        CHECK(!sp0);
        CHECK(STD get_deleter<deleter>(sp0) == nullptr);
    }
    CHECK_INT(X0::objects, 0);
    CHECK_INT(deleter::called, 0);

    { // shared_ptr(_U*(0))
        deleter::called = 0;
        STD shared_ptr<X0> sp0((X0*) nullptr);
        CHECK_INT(sp0.use_count(), 1);
        CHECK_PTR(sp0.get(), nullptr);
        CHECK(STD get_deleter<deleter>(sp0) == nullptr);
    }
    CHECK_INT(X0::objects, 0);
    CHECK_INT(deleter::called, 0);

    { // shared_ptr(_U*), operator (user-defined, equivalent to bool)
        deleter::called = 0;
        X0* it          = new X0;
        STD shared_ptr<X0> sp0(it);
        CHECK_INT(sp0.use_count(), 1);
        CHECK_PTR(sp0.get(), it);

        CHECK(sp0 != nullptr);

        CHECK(STD get_deleter<deleter>(sp0) == nullptr);
    }
    CHECK_INT(X0::objects, 0);
    CHECK_INT(deleter::called, 0);

    { // shared_ptr(_U*(0), _D)
        deleter::called = 0;
        STD shared_ptr<X0> sp0((X0*) nullptr, deleter());
        CHECK_INT(sp0.use_count(), 1);
        CHECK_PTR(sp0.get(), nullptr);
        CHECK(STD get_deleter<deleter>(sp0) != nullptr);
    }
    CHECK_INT(X0::objects, 0);
    CHECK_INT(deleter::called, 1);

    { // shared_ptr(_U*, _D)
        deleter::called = 0;
        X0* it          = new X0;
        STD shared_ptr<X0> sp0(it, deleter());
        CHECK_INT(sp0.use_count(), 1);
        CHECK_PTR(sp0.get(), it);
        CHECK(STD get_deleter<deleter>(sp0) != nullptr);
    }
    CHECK_INT(X0::objects, 0);
    CHECK_INT(deleter::called, 1);

    { // shared_ptr(_V*, _D)
        deleter::called = 0;
        X2* it          = new X2;
        STD shared_ptr<X0> sp0(it, deleter());
        CHECK_INT(sp0.use_count(), 1);
        CHECK_PTR(sp0.get(), it);
        CHECK(STD get_deleter<deleter>(sp0) != nullptr);
    }
    CHECK_INT(X0::objects, 0);
    CHECK_INT(deleter::called, 1);

    { // shared_ptr(shared_ptr(0))
        deleter::called = 0;
        STD shared_ptr<X0> sp0;
        STD shared_ptr<X0> sp1(sp0);
        CHECK_INT(sp0.use_count(), 0);
        CHECK_PTR(sp0.get(), nullptr);
        CHECK_INT(sp1.use_count(), 0);
        CHECK_PTR(sp1.get(), nullptr);
        CHECK(STD get_deleter<deleter>(sp0) == nullptr);
        CHECK(STD get_deleter<deleter>(sp1) == nullptr);
    }
    CHECK_INT(X0::objects, 0);
    CHECK_INT(deleter::called, 0);

    { // shared_ptr(shared_ptr(_U*))
        deleter::called = 0;
        X0* it          = new X0;
        STD shared_ptr<X0> sp0(it);
        STD shared_ptr<X0> sp1(sp0);
        CHECK_INT(sp0.use_count(), 2);
        CHECK_PTR(sp0.get(), it);
        CHECK_INT(sp1.use_count(), 2);
        CHECK_PTR(sp1.get(), it);
        CHECK(STD get_deleter<deleter>(sp0) == nullptr);
        CHECK(STD get_deleter<deleter>(sp1) == nullptr);
    }
    CHECK_INT(X0::objects, 0);
    CHECK_INT(deleter::called, 0);

    { // shared_ptr(shared_ptr(_U*, _D))
        deleter::called = 0;
        X0* it          = new X0;
        STD shared_ptr<X0> sp0(it, deleter());
        STD shared_ptr<X0> sp1(sp0);
        CHECK_INT(sp0.use_count(), 2);
        CHECK_PTR(sp0.get(), it);
        CHECK_INT(sp1.use_count(), 2);
        CHECK_PTR(sp1.get(), it);
        CHECK(STD get_deleter<deleter>(sp0) != nullptr);
        CHECK(STD get_deleter<deleter>(sp1) != nullptr);
    }
    CHECK_INT(X0::objects, 0);
    CHECK_INT(deleter::called, 1);

    { // shared_ptr(shared_ptr(_V*, _D))
        deleter::called = 0;
        X2* it          = new X2;
        STD shared_ptr<X2> sp0(it, deleter());
        STD shared_ptr<X0> sp1(sp0);
        CHECK_INT(sp0.use_count(), 2);
        CHECK_PTR(sp0.get(), it);
        CHECK_INT(sp1.use_count(), 2);
        CHECK_PTR(sp1.get(), it);
        CHECK(STD get_deleter<deleter>(sp0) != nullptr);
        CHECK(STD get_deleter<deleter>(sp1) != nullptr);
    }
    CHECK_INT(X0::objects, 0);
    CHECK_INT(deleter::called, 1);

    { // shared_ptr(weak_ptr(_V*))
        deleter::called = 0;
        X2* it          = new X2;
        STD shared_ptr<X2> sp0(it);
        STD weak_ptr<X2> wp0(sp0);
        STD shared_ptr<X0> sp1(wp0);
        CHECK_INT(sp0.use_count(), 2);
        CHECK_PTR(sp0.get(), it);
        CHECK_INT(sp1.use_count(), 2);
        CHECK_PTR(sp1.get(), it);
        CHECK(STD get_deleter<deleter>(sp0) == nullptr);
        CHECK(STD get_deleter<deleter>(sp1) == nullptr);
    }
    CHECK_INT(X0::objects, 0);
    CHECK_INT(deleter::called, 0);

    { // shared_ptr(STD auto_ptr<_U>(0))
        deleter::called = 0;
        STD auto_ptr<X0> ap(nullptr);
        STD shared_ptr<X0> sp0(STD move(ap));
        CHECK_INT(sp0.use_count(), 1);
        CHECK_PTR(sp0.get(), nullptr);
        CHECK_PTR(ap.get(), nullptr);
        CHECK(STD get_deleter<deleter>(sp0) == nullptr);
    }
    CHECK_INT(X0::objects, 0);
    CHECK_INT(deleter::called, 0);

    { // shared_ptr(STD auto_ptr<_U>)
        deleter::called = 0;
        X0* it          = new X0;
        STD auto_ptr<X0> ap(it);
        STD shared_ptr<X0> sp0(STD move(ap));
        CHECK_INT(sp0.use_count(), 1);
        CHECK_PTR(sp0.get(), it);
        CHECK_PTR(ap.get(), nullptr);
        CHECK(STD get_deleter<deleter>(sp0) == nullptr);
    }
    CHECK_INT(X0::objects, 0);
    CHECK_INT(deleter::called, 0);

    { // shared_ptr = shared_ptr
        deleter::called = 0;
        X0* it          = new X0;
        STD shared_ptr<X0> sp0(it);
        STD shared_ptr<X0> sp1;
        sp1 = sp0;
        CHECK_INT(sp0.use_count(), 2);
        CHECK_PTR(sp0.get(), it);
        CHECK_INT(sp1.use_count(), 2);
        CHECK_PTR(sp1.get(), it);
    }
    CHECK_INT(X0::objects, 0);
    CHECK_INT(deleter::called, 0);

    { // shared_ptr = shared_ptr(_V*)
        deleter::called = 0;
        X2* it          = new X2;
        STD shared_ptr<X2> sp0(it);
        STD shared_ptr<X0> sp1;
        sp1 = sp0;
        CHECK_INT(sp0.use_count(), 2);
        CHECK_PTR(sp0.get(), it);
        CHECK_INT(sp1.use_count(), 2);
        CHECK_PTR(sp1.get(), it);
        CHECK(STD get_deleter<deleter>(sp0) == nullptr);
        CHECK(STD get_deleter<deleter>(sp1) == nullptr);
    }
    CHECK_INT(X0::objects, 0);
    CHECK_INT(deleter::called, 0);

    { // shared_ptr = shared_ptr(_V*, _D)
        deleter::called = 0;
        X2* it          = new X2;
        STD shared_ptr<X2> sp0(it, deleter());
        STD shared_ptr<X0> sp1;
        sp1 = sp0;
        CHECK_INT(sp0.use_count(), 2);
        CHECK_PTR(sp0.get(), it);
        CHECK_INT(sp1.use_count(), 2);
        CHECK_PTR(sp1.get(), it);
        CHECK(STD get_deleter<deleter>(sp0) != nullptr);
        CHECK(STD get_deleter<deleter>(sp1) != nullptr);
    }
    CHECK_INT(X0::objects, 0);
    CHECK_INT(deleter::called, 1);

    { // shared_ptr = shared_ptr(_V*, _D, _A)
        deleter::called = 0;
        X2* it          = new X2;
        STD shared_ptr<X2> sp0(it, deleter(), STD allocator<int>());
        STD shared_ptr<X0> sp1;
        sp1 = sp0;
        CHECK_INT(sp0.use_count(), 2);
        CHECK_PTR(sp0.get(), it);
        CHECK_INT(sp1.use_count(), 2);
        CHECK_PTR(sp1.get(), it);
        CHECK(STD get_deleter<deleter>(sp0) != nullptr);
        CHECK(STD get_deleter<deleter>(sp1) != nullptr);
    }
    CHECK_INT(X0::objects, 0);
    CHECK_INT(deleter::called, 1);

    { // shared_ptr = shared_ptr(const _SP&, _T*)
        deleter::called = 0;
        X2* it          = new X2;
        STD shared_ptr<X2> sp0(it, deleter(), STD allocator<int>());
        int x = 3;
        STD shared_ptr<int> sp1(sp0, &x);
        CHECK_INT(sp0.use_count(), 2);
        CHECK_PTR(sp0.get(), it);
        CHECK_INT(sp1.use_count(), 2);
        CHECK_PTR(sp1.get(), &x);
        CHECK(STD get_deleter<deleter>(sp0) != nullptr);
        CHECK(STD get_deleter<deleter>(sp1) != nullptr);
    }
    CHECK_INT(deleter::called, 1);

    { // shared_ptr = allocate_shared/make_shared
        STD shared_ptr<Movable_int> sp0;
        sp0 = STD make_shared<Movable_int>();
        CHECK_INT(sp0.use_count(), 1);
        CHECK_INT(*sp0, 0);
        sp0 = STD make_shared<Movable_int>(2);
        CHECK_INT(sp0.use_count(), 1);
        CHECK_INT(*sp0, 2);
        sp0 = STD make_shared<Movable_int>(3, 2);
        CHECK_INT(sp0.use_count(), 1);
        CHECK_INT(*sp0, 0x32);
        sp0 = STD make_shared<Movable_int>(4, 3, 2);
        CHECK_INT(sp0.use_count(), 1);
        CHECK_INT(*sp0, 0x432);
        sp0 = STD make_shared<Movable_int>(5, 4, 3, 2);
        CHECK_INT(sp0.use_count(), 1);
        CHECK_INT(*sp0, 0x5432);
        sp0 = STD make_shared<Movable_int>(6, 5, 4, 3, 2);
        CHECK_INT(sp0.use_count(), 1);
        CHECK_INT(*sp0, 0x65432);

        STD allocator<Movable_int> myal;
        sp0 = STD allocate_shared<Movable_int>(myal);
        CHECK_INT(sp0.use_count(), 1);
        CHECK_INT(*sp0, 0);
        sp0 = STD allocate_shared<Movable_int>(myal, 2);
        CHECK_INT(sp0.use_count(), 1);
        CHECK_INT(*sp0, 2);
        sp0 = STD allocate_shared<Movable_int>(myal, 3, 2);
        CHECK_INT(sp0.use_count(), 1);
        CHECK_INT(*sp0, 0x32);
        sp0 = STD allocate_shared<Movable_int>(myal, 4, 3, 2);
        CHECK_INT(sp0.use_count(), 1);
        CHECK_INT(*sp0, 0x432);
        sp0 = STD allocate_shared<Movable_int>(myal, 5, 4, 3, 2);
        CHECK_INT(sp0.use_count(), 1);
        CHECK_INT(*sp0, 0x5432);
        sp0 = STD allocate_shared<Movable_int>(myal, 6, 5, 4, 3, 2);
        CHECK_INT(sp0.use_count(), 1);
        CHECK_INT(*sp0, 0x65432);
    }

    { // shared_ptr(forward(shared_ptr(_V*)))
        X2* it = new X2;
        STD shared_ptr<X2> sp0(it);
        STD shared_ptr<X2> sp1(STD move(sp0));
        CHECK_INT(sp0.use_count(), 0);
        CHECK_PTR(sp0.get(), nullptr);
        CHECK_INT(sp1.use_count(), 1);
        CHECK_PTR(sp1.get(), it);
    }

    { // shared_ptr = forward(shared_ptr(_V*))
        X2* it = new X2;
        STD shared_ptr<X2> sp0(it);
        STD shared_ptr<X2> sp1;
        sp1 = STD move(sp0);
        CHECK_INT(sp0.use_count(), 0);
        CHECK_PTR(sp0.get(), nullptr);
        CHECK_INT(sp1.use_count(), 1);
        CHECK_PTR(sp1.get(), it);
    }

    { // shared_ptr(STD unique_ptr<_U>)
        deleter::called = 0;
        X0* it          = new X0;
        STD unique_ptr<X0> up(it);
        STD shared_ptr<X0> sp0(STD move(up));
        CHECK_INT(sp0.use_count(), 1);
        CHECK_PTR(sp0.get(), it);
        CHECK_PTR(up.get(), nullptr);
        CHECK(STD get_deleter<deleter>(sp0) == nullptr);
    }
    CHECK_INT(X0::objects, 0);
    CHECK_INT(deleter::called, 0);

    { // shared_ptr = STD unique_ptr<_U>
        deleter::called = 0;
        X0* it          = new X0;
        STD unique_ptr<X0> up(it);
        STD shared_ptr<X0> sp0;
        sp0 = STD move(up);
        CHECK_INT(sp0.use_count(), 1);
        CHECK_PTR(sp0.get(), it);
        CHECK_PTR(up.get(), nullptr);
        CHECK(STD get_deleter<deleter>(sp0) == nullptr);
    }
    CHECK_INT(X0::objects, 0);
    CHECK_INT(deleter::called, 0);

    { // shared_ptr = auto_ptr(V)
        deleter::called = 0;
        X2* it          = new X2;
        STD auto_ptr<X2> ap(it);
        STD shared_ptr<X0> sp0;
        sp0 = STD move(ap);
        CHECK_INT(sp0.use_count(), 1);
        CHECK_PTR(sp0.get(), it);
        CHECK(ap.get() == nullptr);
        CHECK(STD get_deleter<deleter>(sp0) == nullptr);
    }
    CHECK_INT(X0::objects, 0);
    CHECK_INT(deleter::called, 0);

    { // swap, member and global
        deleter::called = 0;
        X0* it0         = new X0;
        X0* it1         = new X0;
        STD shared_ptr<X0> sp0(it0);
        STD shared_ptr<X0> sp1(it1);
        STD shared_ptr<X0> sp2(sp1);
        sp0.swap(sp1);
        CHECK_INT(sp0.use_count(), 2);
        CHECK_PTR(sp0.get(), it1);
        CHECK_INT(sp1.use_count(), 1);
        CHECK_PTR(sp1.get(), it0);
        CHECK_INT(sp2.use_count(), 2);
        CHECK_PTR(sp2.get(), it1);
        STD swap(sp0, sp1);
        CHECK_INT(sp0.use_count(), 1);
        CHECK_PTR(sp0.get(), it0);
        CHECK_INT(sp1.use_count(), 2);
        CHECK_PTR(sp1.get(), it1);
        CHECK_INT(sp2.use_count(), 2);
        CHECK_PTR(sp2.get(), it1);
    }
    CHECK_INT(X0::objects, 0);
    CHECK_INT(deleter::called, 0);

    { // reset
        deleter::called = 0;
        X0* it0         = new X0;
        STD shared_ptr<X0> sp0(it0);
        sp0.reset();
        CHECK_INT(sp0.use_count(), 0);
        CHECK_PTR(sp0.get(), nullptr);
        CHECK(STD get_deleter<deleter>(sp0) == nullptr);
    }
    CHECK_INT(X0::objects, 0);
    CHECK_INT(deleter::called, 0);

    { // reset(_V*)
        deleter::called = 0;
        X0* it0         = new X0;
        X2* it1         = new X2;
        STD shared_ptr<X0> sp0(it0);
        sp0.reset(it1);
        CHECK_INT(sp0.use_count(), 1);
        CHECK_PTR(sp0.get(), it1);
        CHECK(STD get_deleter<deleter>(sp0) == nullptr);
    }
    CHECK_INT(X0::objects, 0);
    CHECK_INT(deleter::called, 0);

    { // reset(_V*, _D)
        deleter::called = 0;
        X0* it0         = new X0;
        X2* it1         = new X2;
        STD shared_ptr<X0> sp0(it0);
        sp0.reset(it1, deleter());
        CHECK_INT(sp0.use_count(), 1);
        CHECK_PTR(sp0.get(), it1);
        CHECK(STD get_deleter<deleter>(sp0) != nullptr);
    }
    CHECK_INT(X0::objects, 0);
    CHECK_INT(deleter::called, 1);

    { // reset(_V*, _D, _A)
        deleter::called = 0;
        X0* it0         = new X0;
        X2* it1         = new X2;
        STD shared_ptr<X0> sp0(it0);
        sp0.reset(it1, deleter(), STD allocator<int>());
        CHECK_INT(sp0.use_count(), 1);
        CHECK_PTR(sp0.get(), it1);
        CHECK(STD get_deleter<deleter>(sp0) != nullptr);
    }
    CHECK_INT(X0::objects, 0);
    CHECK_INT(deleter::called, 1);

    { // operator*, ->, unique
        X0* it0 = new X0;
        STD shared_ptr<X0> sp0(it0);
        CHECK_PTR(&*sp0, it0);
        CHECK_PTR(sp0.operator->(), it0);
        CHECK(sp0.unique());
        STD shared_ptr<X0> sp1(sp0);
        CHECK(!sp0.unique());
    }

    { // operator==, operator!=, operator<
        X0* it0 = new X0;
        X0* it1 = new X0;
        STD shared_ptr<X0> sp0(it0);
        STD shared_ptr<X0> sp1(it1);
        STD shared_ptr<X0> sp2(sp1);
        CHECK(!(sp0 == sp1) && sp0 != sp1 && sp1 == sp2 && !(sp1 != sp2));
        CHECK(!(sp1 < sp2) && !(sp2 < sp1));

        CHECK(sp1 <= sp2);
        CHECK(!(sp2 > sp1));
        CHECK(sp1 >= sp2);
    }

    { // operator<<
        X0* it0 = new X0;
        STD shared_ptr<X0> sp0(it0);
        STD ostringstream str0, str1;
        str0 << sp0;
        str1 << it0;
        CHECK_STRING(str0.str(), str1.str());
    }

    { // static_pointer_cast
        deleter::called = 0;
        X0* it0         = new X2;
        STD shared_ptr<X0> sp0(it0);
        STD shared_ptr<X2> sp1 = STD static_pointer_cast<X2>(sp0);
        CHECK_INT(sp0.use_count(), 2);
        CHECK_PTR(sp0.get(), it0);
        CHECK_INT(sp1.use_count(), 2);
        CHECK_PTR(sp1.get(), it0);
        CHECK(STD get_deleter<deleter>(sp0) == nullptr);
        CHECK(STD get_deleter<deleter>(sp1) == nullptr);
    }
    CHECK_INT(X0::objects, 0);
    CHECK_INT(deleter::called, 0);

    { // const_pointer_cast
        deleter::called = 0;
        X0* it0         = new X2;
        STD shared_ptr<X0> sp0(it0);
        STD shared_ptr<const X0> sp1 = STD const_pointer_cast<const X0>(sp0);
        CHECK_INT(sp0.use_count(), 2);
        CHECK_PTR(sp0.get(), it0);
        CHECK_INT(sp1.use_count(), 2);
        CHECK_PTR(sp1.get(), it0);
        CHECK(STD get_deleter<deleter>(sp0) == nullptr);
        CHECK(STD get_deleter<deleter>(sp1) == nullptr);
    }
    CHECK_INT(X0::objects, 0);
    CHECK_INT(deleter::called, 0);

#if NO_EXCEPTIONS
#else // NO_EXCEPTIONS
    { // dynamic_pointer_cast
        deleter::called = 0;
        X0* it0         = new X2;
        STD shared_ptr<X0> sp0(it0);
        STD shared_ptr<X1> sp1 = STD dynamic_pointer_cast<X1>(sp0);
        CHECK_INT(sp0.use_count(), 2);
        CHECK_PTR(sp0.get(), it0);
        CHECK_INT(sp1.use_count(), 2);
        CHECK_PTR(sp1.get(), dynamic_cast<X1*>(it0));
        CHECK(STD get_deleter<deleter>(sp0) == nullptr);
        CHECK(STD get_deleter<deleter>(sp1) == nullptr);
    }
    CHECK_INT(X0::objects, 0);
    CHECK_INT(deleter::called, 0);
#endif // NO_EXCEPTIONS
}

static void t_make_unique() { // test make_unique interface
    STD unique_ptr<int> up0;
    up0 = STD make_unique<int>();
    CHECK_INT(*up0, 0);
    up0 = STD make_unique<int>(2);
    CHECK_INT(*up0, 2);

    STD unique_ptr<int[]> up1;
    up1 = STD make_unique<int[]>(3);
    CHECK_INT(up1[0], 0);
    CHECK_INT(up1[2], 0);
}

static void t_weak_ptr() { // test weak_ptr interface

    { // weak_ptr()
        deleter::called = 0;
        STD weak_ptr<X0> wp0;
        CHECK_INT(wp0.use_count(), 0);
    }
    CHECK_INT(X0::objects, 0);
    CHECK_INT(deleter::called, 0);

    { // weak_ptr(shared_ptr())
        deleter::called = 0;
        STD shared_ptr<X2> sp0;
        STD weak_ptr<X0> wp0(sp0);
        CHECK_INT(wp0.use_count(), 0);
        CHECK_INT(sp0.use_count(), 0);
    }
    CHECK_INT(X0::objects, 0);
    CHECK_INT(deleter::called, 0);

    { // weak_ptr(shared_ptr(_V*))
        deleter::called = 0;
        X2* it0         = new X2;
        STD shared_ptr<X2> sp0(it0);
        STD weak_ptr<X0> wp0(sp0);
        CHECK_INT(wp0.use_count(), 1);
        CHECK_INT(sp0.use_count(), 1);
    }
    CHECK_INT(X0::objects, 0);
    CHECK_INT(deleter::called, 0);

    { // weak_ptr(shared_ptr(_V*, _D))
        deleter::called = 0;
        X2* it0         = new X2;
        STD shared_ptr<X2> sp0(it0, deleter());
        STD weak_ptr<X0> wp0(sp0);
        CHECK_INT(wp0.use_count(), 1);
        CHECK_INT(sp0.use_count(), 1);
    }
    CHECK_INT(X0::objects, 0);
    CHECK_INT(deleter::called, 1);

    { // weak_ptr(weak_ptr(_U*))
        deleter::called = 0;
        X0* it0         = new X2;
        STD shared_ptr<X0> sp0(it0);
        STD weak_ptr<X0> wp0(sp0);
        STD weak_ptr<X0> wp1(wp0);
        CHECK_INT(wp0.use_count(), 1);
        CHECK_INT(wp1.use_count(), 1);
        CHECK_INT(sp0.use_count(), 1);
    }
    CHECK_INT(X0::objects, 0);
    CHECK_INT(deleter::called, 0);

    { // weak_ptr(weak_ptr(_V*))
        deleter::called = 0;
        X2* it0         = new X2;
        STD shared_ptr<X2> sp0(it0);
        STD weak_ptr<X0> wp0(sp0);
        STD weak_ptr<X0> wp1(wp0);
        CHECK_INT(wp0.use_count(), 1);
        CHECK_INT(wp1.use_count(), 1);
        CHECK_INT(sp0.use_count(), 1);
    }
    CHECK_INT(X0::objects, 0);
    CHECK_INT(deleter::called, 0);

    { // weak_ptr = weak_ptr
        deleter::called = 0;
        STD weak_ptr<X0> wp0;
        STD weak_ptr<X0> wp1;
        wp0 = wp1;
        CHECK_INT(wp0.use_count(), 0);
        CHECK_INT(wp1.use_count(), 0);
    }
    CHECK_INT(X0::objects, 0);
    CHECK_INT(deleter::called, 0);

    { // weak_ptr = weak_ptr(shared_ptr)
        deleter::called = 0;
        X0* it0         = new X2;
        STD weak_ptr<X0> wp0;
        STD shared_ptr<X0> sp0(it0);
        STD weak_ptr<X0> wp1(sp0);
        wp0 = wp1;
        CHECK_INT(wp0.use_count(), 1);
        CHECK_INT(wp1.use_count(), 1);
        CHECK_INT(sp0.use_count(), 1);
    }
    CHECK_INT(X0::objects, 0);
    CHECK_INT(deleter::called, 0);

    { // weak_ptr = shared_ptr
        deleter::called = 0;
        STD weak_ptr<X0> wp0;
        STD shared_ptr<X0> sp0;
        wp0 = sp0;
        CHECK_INT(sp0.use_count(), 0);
        CHECK_INT(sp0.use_count(), 0);
    }
    CHECK_INT(X0::objects, 0);
    CHECK_INT(deleter::called, 0);

    { // weak_ptr.swap(weak_ptr)
        deleter::called = 0;
        STD weak_ptr<X0> wp0;
        STD weak_ptr<X0> wp1;
        wp0.swap(wp1);
        CHECK_INT(wp0.use_count(), 0);
        CHECK_INT(wp1.use_count(), 0);
        STD swap(wp0, wp1);
        CHECK_INT(wp0.use_count(), 0);
        CHECK_INT(wp1.use_count(), 0);
    }
    CHECK_INT(X0::objects, 0);
    CHECK_INT(deleter::called, 0);

    { // weak_ptr.swap(weak_ptr(_U*))
        deleter::called = 0;
        X0* it0         = new X2;
        STD shared_ptr<X0> sp0(it0);
        STD weak_ptr<X0> wp0;
        STD weak_ptr<X0> wp1(sp0);
        wp0.swap(wp1);
        CHECK_INT(wp0.use_count(), 1);
        CHECK_INT(wp1.use_count(), 0);
        CHECK_INT(sp0.use_count(), 1);
        STD swap(wp0, wp1);
        CHECK_INT(wp0.use_count(), 0);
        CHECK_INT(wp1.use_count(), 1);
        CHECK_INT(sp0.use_count(), 1);
    }
    CHECK_INT(X0::objects, 0);
    CHECK_INT(deleter::called, 0);

    { // weak_ptr(_U*).swap(weak_ptr)
        deleter::called = 0;
        X0* it0         = new X2;
        STD shared_ptr<X0> sp0(it0);
        STD weak_ptr<X0> wp0(sp0);
        STD weak_ptr<X0> wp1;
        wp0.swap(wp1);
        CHECK_INT(wp0.use_count(), 0);
        CHECK_INT(wp1.use_count(), 1);
        CHECK_INT(sp0.use_count(), 1);
        STD swap(wp0, wp1);
        CHECK_INT(wp0.use_count(), 1);
        CHECK_INT(wp1.use_count(), 0);
        CHECK_INT(sp0.use_count(), 1);
    }
    CHECK_INT(X0::objects, 0);
    CHECK_INT(deleter::called, 0);

    { // weak_ptr(_U*).swap(weak_ptr(_U*))
        deleter::called = 0;
        X0* it0         = new X2;
        X0* it1         = new X2;
        STD shared_ptr<X0> sp0(it0);
        STD shared_ptr<X0> sp1(it1);
        STD weak_ptr<X0> wp0(sp0);
        STD weak_ptr<X0> wp1(sp1);
        wp0.swap(wp1);
        CHECK_INT(wp0.use_count(), 1);
        CHECK_INT(wp1.use_count(), 1);
        CHECK_INT(sp0.use_count(), 1);
        CHECK_INT(sp1.use_count(), 1);
        STD swap(wp0, wp1);
        CHECK_INT(wp0.use_count(), 1);
        CHECK_INT(wp1.use_count(), 1);
        CHECK_INT(sp0.use_count(), 1);
        CHECK_INT(sp1.use_count(), 1);
    }
    CHECK_INT(X0::objects, 0);
    CHECK_INT(deleter::called, 0);

    { // reset
        deleter::called = 0;
        X0* it0         = new X2;
        STD shared_ptr<X0> sp0(it0);
        STD weak_ptr<X0> wp0(sp0);
        STD weak_ptr<X0> wp1;
        wp0.reset();
        wp1.reset();
        CHECK_INT(wp0.use_count(), 0);
        CHECK_INT(wp1.use_count(), 0);
        CHECK_INT(sp0.use_count(), 1);
    }
    CHECK_INT(X0::objects, 0);
    CHECK_INT(deleter::called, 0);

    { // expired
        deleter::called = 0;
        X0* it0         = new X2;
        STD weak_ptr<X0> wp0;
        { // create local shared_ptr which releases resource at end of block
            STD shared_ptr<X0> sp0(it0);
            wp0 = sp0;
            CHECK_INT(sp0.use_count(), 1);
            CHECK_INT(wp0.use_count(), 1);
            CHECK(!wp0.expired());
        }
        CHECK_INT(X0::objects, 0);
        CHECK_INT(deleter::called, 0);
        CHECK_INT(wp0.use_count(), 0);
        CHECK(wp0.expired());

#if NO_EXCEPTIONS
#else // NO_EXCEPTIONS
        bool caught = false;
        try { // look for exception from expired weak_ptr object
            STD shared_ptr<X0> sp1(wp0);
        } catch (const STD bad_weak_ptr&) { // catch exception
            caught = true;
        } catch (...) {
        }
        CHECK(caught);
#endif // NO_EXCEPTIONS
    }

    { // lock
        deleter::called = 0;
        X0* it0         = new X2;
        STD weak_ptr<X0> wp0;
        { // create local shared_ptr which releases resource at end of block
            STD shared_ptr<X0> sp0(it0);
            wp0                    = sp0;
            STD shared_ptr<X0> sp1 = wp0.lock();
            CHECK_INT(wp0.use_count(), 2);
            CHECK_INT(sp0.use_count(), 2);
            CHECK_INT(sp1.use_count(), 2);
        }

        CHECK_INT(X0::objects, 0);
        CHECK_INT(deleter::called, 0);
        CHECK_INT(wp0.use_count(), 0);
        CHECK(wp0.expired());
        STD shared_ptr<X0> sp2 = wp0.lock();
        CHECK_INT(wp0.use_count(), 0);
        CHECK(wp0.expired());
        CHECK_INT(sp2.use_count(), 0);
        CHECK_PTR(sp2.get(), nullptr);
    }

    { // operator<
        X0* it1 = new X0;

        STD shared_ptr<X0> sp1(it1);
        STD shared_ptr<X0> sp2(sp1);

        STD weak_ptr<X0> wp1(sp1);
        STD weak_ptr<X0> wp2(sp2);

        CHECK(!(sp1.owner_before(sp2)) && !(sp2.owner_before(sp1)));
        CHECK(!(wp1.owner_before(wp2)) && !(wp2.owner_before(wp1)));

        CHECK(!STD owner_less<STD shared_ptr<X0>>()(sp1, sp2));
        CHECK(!STD owner_less<STD shared_ptr<X0>>()(sp1, wp2));
        CHECK(!STD owner_less<STD shared_ptr<X0>>()(wp1, sp2));

        CHECK(!STD owner_less<STD weak_ptr<X0>>()(wp1, wp2));
        CHECK(!STD owner_less<STD weak_ptr<X0>>()(wp1, sp2));
        CHECK(!STD owner_less<STD weak_ptr<X0>>()(sp1, wp2));
    }
}

static void t_enable_shared_from_this() { // test enable_shared_from_this interface
    STD shared_ptr<X> sp0(new X2);

#if NO_EXCEPTIONS
    STD shared_ptr<X> sp1 = sp0->shared_from_this();
    CHECK(sp0 == sp1);
    CHECK_INT(sp0.use_count(), 2);
    CHECK_INT(sp1.use_count(), 2);

#else // NO_EXCEPTIONS
    try { // make sure no exceptions escape
        STD shared_ptr<X> sp1 = sp0->shared_from_this();
        CHECK(sp0 == sp1);
        CHECK_INT(sp0.use_count(), 2);
        CHECK_INT(sp1.use_count(), 2);
    } catch (const STD bad_weak_ptr&) { // catch failed conversion
        CHECK_MSG("enable_shared failed", 0);
    } catch (...) {
    }
#endif // NO_EXCEPTIONS
}

void test_main() { // run tests
    t_bad_weak_ptr();
    t_shared_ptr();
    t_make_unique();
    t_weak_ptr();
    t_enable_shared_from_this();
}
