// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <cassert>
#include <memory>

using namespace std;

struct X : enable_shared_from_this<X> {
    explicit X(const int n) : m_n(n) {}
    int m_n;
};

int main() {
    // Test DDB-196243 "TR1 VC9 SP1: enable_shared_from_this's copy ctor and copy assignment operator do too much work".
    {
        const shared_ptr<X> sp1(new X(11));
        const shared_ptr<X> sp2(new X(22));

        X* const raw1 = sp1.get();
        X* const raw2 = sp2.get();

        assert(raw1->m_n == 11);
        assert(raw2->m_n == 22);
        assert(raw1->shared_from_this() == sp1);
        assert(raw2->shared_from_this() == sp2);
        assert(raw1->shared_from_this() != sp2);
        assert(raw2->shared_from_this() != sp1);

        *raw1 = *raw2;

        assert(raw1->m_n == 22);
        assert(raw2->m_n == 22);
        assert(raw1->shared_from_this() == sp1);
        assert(raw2->shared_from_this() == sp2);
        assert(raw1->shared_from_this() != sp2);
        assert(raw2->shared_from_this() != sp1);
    }

    // Test DDB-197048 "[VS2008 / TR1] still got problems with shared_ptr<const T>".
    {
        shared_ptr<const int> sp1(static_cast<const int*>(new int(6)));
        shared_ptr<volatile int> sp2(static_cast<volatile int*>(new int(7)));
        shared_ptr<const volatile int> sp3(static_cast<const volatile int*>(new int(8)));

        assert(*sp1 == 6);
        assert(*sp2 == 7);
        assert(*sp3 == 8);
    }

    // Test Dev10-654944 "shared_ptr: assignment is messed up".
    {
        shared_ptr<int> p(new int(1729));
        shared_ptr<int> z;

        assert(!!p);
        assert(*p == 1729);
        assert(!z);

        p = z;

        assert(!p);
        assert(!z);
    }

    // Test DevDiv-1178296 "<memory>: shared_ptr<volatile X> doesn't work with enable_shared_from_this<X>".
    {
        const auto sp1 = make_shared<const X>(100);
        const auto sp2 = make_shared<volatile X>(200);
        const auto sp3 = make_shared<const volatile X>(300);

        const shared_ptr<const X> sp4(static_cast<const X*>(new X(400)));
        const shared_ptr<volatile X> sp5(static_cast<volatile X*>(new X(500)));
        const shared_ptr<const volatile X> sp6(static_cast<const volatile X*>(new X(600)));

        assert(sp1->m_n == 100);
        assert(sp2->m_n == 200);
        assert(sp3->m_n == 300);
        assert(sp4->m_n == 400);
        assert(sp5->m_n == 500);
        assert(sp6->m_n == 600);
    }
}
