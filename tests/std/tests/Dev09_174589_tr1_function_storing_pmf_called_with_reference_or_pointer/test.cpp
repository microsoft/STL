// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <cassert>
#include <cstdlib>
#include <functional>
#include <memory>

using namespace std;

struct A {
    int f() {
        return 8;
    }
};

void test_orig() {
    A a;
    A& r = a;

    function<int(A)> f(&A::f);
    function<int(A&)> g(&A::f);
    function<int(A*)> h(&A::f);

    assert(f(a) == 8);
    assert(g(r) == 8);
    assert(h(&a) == 8);
}


// DevDiv-294051 "<functional>: std::function has lost the ability to invoke PMFs/PMDs on various things"

// N4971 [func.require] says that std::function must be able to invoke PMFs/PMDs
// on values, references, raw pointers, smart pointers, and reference_wrappers - all handling base/derived cases.

struct B {
    int func(int i) {
        return i + data + 5;
    }

    int data;
};

struct X : public B {};

void test_DevDiv_294051() {
    shared_ptr<B> b(new B);
    shared_ptr<X> x(new X);

    b->data = 220;
    x->data = 330;

    function<int(B, int)> f1                     = &B::func;
    function<int(X, int)> f1x                    = &B::func;
    function<int(B&, int)> f2                    = &B::func;
    function<int(X&, int)> f3                    = &B::func;
    function<int(B*, int)> f4                    = &B::func;
    function<int(X*, int)> f5                    = &B::func;
    function<int(shared_ptr<B>, int)> f6         = &B::func;
    function<int(shared_ptr<X>, int)> f7         = &B::func;
    function<int(const shared_ptr<B>&, int)> f8  = &B::func;
    function<int(const shared_ptr<X>&, int)> f9  = &B::func;
    function<int(reference_wrapper<B>, int)> f10 = &B::func;
    function<int(reference_wrapper<X>, int)> f11 = &B::func;

    assert(f1(*b, 1000) == 1225);
    assert(f1x(*x, 1000) == 1335);
    assert(f2(*b, 2000) == 2225);
    assert(f3(*x, 3000) == 3335);
    assert(f4(b.get(), 4000) == 4225);
    assert(f5(x.get(), 5000) == 5335);
    assert(f6(b, 6000) == 6225);
    assert(f7(x, 7000) == 7335);
    assert(f8(b, 8000) == 8225);
    assert(f9(x, 9000) == 9335);
    assert(f10(ref(*b), 10000) == 10225);
    assert(f11(ref(*x), 11000) == 11335);

    function<int(B)> g1                     = &B::data;
    function<int(X)> g1x                    = &B::data;
    function<int(B&)> g2                    = &B::data;
    function<int(X&)> g3                    = &B::data;
    function<int(B*)> g4                    = &B::data;
    function<int(X*)> g5                    = &B::data;
    function<int(shared_ptr<B>)> g6         = &B::data;
    function<int(shared_ptr<X>)> g7         = &B::data;
    function<int(const shared_ptr<B>&)> g8  = &B::data;
    function<int(const shared_ptr<X>&)> g9  = &B::data;
    function<int(reference_wrapper<B>)> g10 = &B::data;
    function<int(reference_wrapper<X>)> g11 = &B::data;

    assert(g1(*b) == 220);
    assert(g1x(*x) == 330);
    assert(g2(*b) == 220);
    assert(g3(*x) == 330);
    assert(g4(b.get()) == 220);
    assert(g5(x.get()) == 330);
    assert(g6(b) == 220);
    assert(g7(x) == 330);
    assert(g8(b) == 220);
    assert(g9(x) == 330);
    assert(g10(ref(*b)) == 220);
    assert(g11(ref(*x)) == 330);
}


int main() {
    test_orig();

    test_DevDiv_294051();
}
