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

// FDIS 20.8.11.2 [func.wrap.func] specifies:
//     template<class R, class... ArgTypes> class function<R(ArgTypes...)>
// /7 says:
//     template<class F> function(F f);
//     Requires: F shall be CopyConstructible. f shall be Callable (20.8.11.2) for
//     argument types ArgTypes and return type R. The copy constructor and
//     destructor of A shall not throw exceptions.
// /2 says:
//     A callable object f of type F is Callable for argument types ArgTypes and
//     return type R if the expression INVOKE(f, declval<ArgTypes>()..., R),
//     considered as an unevaluated operand (Clause 5), is well formed (20.8.2).
// 20.8.2 [func.require]/1-2 says:
//     Define INVOKE(f, t1, t2, ..., tN) as follows:
//     - (t1.*f)(t2, ..., tN) when f is a pointer to a member function of a class T
//     and t1 is an object of type T or a reference to an object of type T or
//     a reference to an object of a type derived from T;
//     - ((*t1).*f)(t2, ..., tN) when f is a pointer to a member function of a class T
//     and t1 is not one of the types described in the previous item;
//     - t1.*f when N == 1 and f is a pointer to member data of a class T
//     and t1 is an object of type T or a reference to an object of type T
//     or a reference to an object of a type derived from T;
//     - (*t1).*f when N == 1 and f is a pointer to member data of a class T
//     and t1 is not one of the types described in the previous item;
//     - f(t1, t2, ..., tN) in all other cases.
//     Define INVOKE(f, t1, t2, ..., tN, R) as INVOKE(f, t1, t2, ..., tN) implicitly converted to R.

// Therefore, std::function must be able to invoke PMFs/PMDs
// on values, references, derived references, raw pointers, and smart pointers.

struct B {
    int func(int i) {
        return i + data + 5;
    }

    int data;
};

struct X : public B {};

void test_294051() {
    shared_ptr<B> b(new B);
    shared_ptr<X> x(new X);

    b->data = 220;
    x->data = 330;

    function<int(B, int)> f1                    = &B::func;
    function<int(B&, int)> f2                   = &B::func;
    function<int(X&, int)> f3                   = &B::func;
    function<int(B*, int)> f4                   = &B::func;
    function<int(X*, int)> f5                   = &B::func;
    function<int(shared_ptr<B>, int)> f6        = &B::func;
    function<int(shared_ptr<X>, int)> f7        = &B::func;
    function<int(const shared_ptr<B>&, int)> f8 = &B::func;
    function<int(const shared_ptr<X>&, int)> f9 = &B::func;

    assert(f1(*b, 1000) == 1225);
    assert(f2(*b, 2000) == 2225);
    assert(f3(*x, 3000) == 3335);
    assert(f4(b.get(), 4000) == 4225);
    assert(f5(x.get(), 5000) == 5335);
    assert(f6(b, 6000) == 6225);
    assert(f7(x, 7000) == 7335);
    assert(f8(b, 8000) == 8225);
    assert(f9(x, 9000) == 9335);

    function<int(B)> g1                    = &B::data;
    function<int(B&)> g2                   = &B::data;
    function<int(X&)> g3                   = &B::data;
    function<int(B*)> g4                   = &B::data;
    function<int(X*)> g5                   = &B::data;
    function<int(shared_ptr<B>)> g6        = &B::data;
    function<int(shared_ptr<X>)> g7        = &B::data;
    function<int(const shared_ptr<B>&)> g8 = &B::data;
    function<int(const shared_ptr<X>&)> g9 = &B::data;

    assert(g1(*b) == 220);
    assert(g2(*b) == 220);
    assert(g3(*x) == 330);
    assert(g4(b.get()) == 220);
    assert(g5(x.get()) == 330);
    assert(g6(b) == 220);
    assert(g7(x) == 330);
    assert(g8(b) == 220);
    assert(g9(x) == 330);
}


int main() {
    test_orig();

    test_294051();
}
