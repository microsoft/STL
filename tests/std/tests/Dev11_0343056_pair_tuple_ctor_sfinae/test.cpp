// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <memory>
#include <tuple>
#include <utility>

using namespace std;

#ifdef __clang__
#pragma clang diagnostic ignored "-Wzero-as-null-pointer-constant" // This test intentionally uses 0 as null.
#endif // __clang__

struct A {};
struct B : public A {};

struct X {};
struct Y : public X {};

void cat(pair<A*, A*>) {}
void cat(pair<A*, X*>) {}
void cat(pair<X*, A*>) {}
void cat(pair<X*, X*>) {}

void dog(tuple<A*, A*>) {}
void dog(tuple<A*, X*>) {}
void dog(tuple<X*, A*>) {}
void dog(tuple<X*, X*>) {}


struct Quark {};

struct Proton {
    Proton(Quark&) {}
};

struct Neutron {
    Neutron(const Quark&) {}
};

void takes_pair(pair<Proton, Proton>) {}
void takes_pair(pair<Proton, Neutron>) {}
void takes_pair(pair<Neutron, Proton>) {}
void takes_pair(pair<Neutron, Neutron>) {}

void takes_tuple(tuple<Proton, Proton>) {}
void takes_tuple(tuple<Proton, Neutron>) {}
void takes_tuple(tuple<Neutron, Proton>) {}
void takes_tuple(tuple<Neutron, Neutron>) {}

void test_alloc();

int main() {
    B* b = nullptr;
    Y* y = nullptr;

    pair<B*, B*> pbb(b, b);
    pair<B*, Y*> pby(b, y);
    pair<Y*, B*> pyb(y, b);
    pair<Y*, Y*> pyy(y, y);

    tuple<B*, B*> tbb(b, b);
    tuple<B*, Y*> tby(b, y);
    tuple<Y*, B*> tyb(y, b);
    tuple<Y*, Y*> tyy(y, y);

    // template <class U, class V> pair(U&& x, V&& y);
    pair<A*, X*> p1(b, y);
    pair<A*, X*> p2(b, 0);
    pair<A*, X*> p3(0, y);
    pair<A*, X*> p4(0, 0);
    (void) p4;

    // template <class... UTypes> explicit tuple(UTypes&&... u);
    tuple<A*, X*> t1(b, y);
    tuple<A*, X*> t2(b, 0);
    tuple<A*, X*> t3(0, y);
    tuple<A*, X*> t4(0, 0);
    (void) t4;

    // template <class U, class V> pair(const pair<U, V>& p);
    cat(pbb);
    cat(pby);
    cat(pyb);
    cat(pyy);

    // template <class U, class V> pair(pair<U, V>&& p);
    cat(move(pbb));
    cat(move(pby));
    cat(move(pyb));
    cat(move(pyy));

    // template <class U1, class U2> tuple(const pair<U1, U2>& u);
    dog(pbb);
    dog(pby);
    dog(pyb);
    dog(pyy);

    // template <class U1, class U2> tuple(pair<U1, U2>&& u);
    dog(move(pbb));
    dog(move(pby));
    dog(move(pyb));
    dog(move(pyy));

    // template <class... UTypes> tuple(const tuple<UTypes...>& u);
    dog(tbb);
    dog(tby);
    dog(tyb);
    dog(tyy);

    // template <class... UTypes> tuple(tuple<UTypes...>&& u);
    dog(move(tbb));
    dog(move(tby));
    dog(move(tyb));
    dog(move(tyy));


    const pair<Quark, Quark> meow;
    const tuple<Quark, Quark> purr;

    // template <class U, class V> pair(const pair<U, V>& p);
    takes_pair(meow);

    // template <class U1, class U2> tuple(const pair<U1, U2>& u);
    takes_tuple(meow);

    // template <class... UTypes> tuple(const tuple<UTypes...>& u);
    takes_tuple(purr);


    test_alloc();
}

struct Meow {
    Meow(const tuple<int>&) {}
    Meow(const pair<int, int>&) {}
};

void test_alloc() {
    {
        allocator<int> al;

        // template <class Alloc> tuple(allocator_arg_t, const Alloc& a);
        tuple<A*> t1(allocator_arg, al);
        (void) t1;

        // template <class Alloc> tuple(allocator_arg_t, const Alloc& a, const Types&...);
        tuple<A*> t2(allocator_arg, al, 0);

        // template <class Alloc, class... UTypes> tuple(allocator_arg_t, const Alloc& a, const UTypes&&...);
        tuple<A*> t3(allocator_arg, al, nullptr);
        (void) t3;

        // template <class Alloc> tuple(allocator_arg_t, const Alloc& a, const tuple&);
        tuple<A*> t4(allocator_arg, al, t2);

        // template <class Alloc> tuple(allocator_arg_t, const Alloc& a, tuple&&);
        tuple<A*> t5(allocator_arg, al, move(t2));

        tuple<B*> b(nullptr);

        // template <class Alloc, class... UTypes> tuple(allocator_arg_t, const Alloc& a, const tuple<UTypes...>&);
        tuple<A*> t6(allocator_arg, al, b);

        // template <class Alloc, class... UTypes> tuple(allocator_arg_t, const Alloc& a, tuple<UTypes...>&&);
        tuple<A*> t7(allocator_arg, al, move(b));

        pair<B*, Y*> by(nullptr, nullptr);

        // template <class Alloc, class U1, class U2> tuple(allocator_arg_t, const Alloc& a, const pair<U1, U2>&);
        tuple<A*, X*> t8(allocator_arg, al, by);

        // template <class Alloc, class U1, class U2> tuple(allocator_arg_t, const Alloc& a, pair<U1, U2>&&);
        tuple<A*, X*> t9(allocator_arg, al, move(by));

        // const UTypes&&...
        tuple<int> ti(0);
        tuple<Meow> t10(allocator_arg, al, ti);
        tuple<Meow> t11(allocator_arg, al, move(ti));

        pair<int, int> pii(0, 0);
        tuple<Meow> t12(allocator_arg, al, pii);
        tuple<Meow> t13(allocator_arg, al, move(pii));
    }

    {
        tuple<A*> t1;
        (void) t1;

        tuple<A*> t2(0);

        tuple<A*> t3(nullptr);
        (void) t3;

        tuple<A*> t4(t2);
        (void) t4;

        tuple<A*> t5(move(t2));
        (void) t5;

        tuple<B*> b(nullptr);

        tuple<A*> t6(b);

        tuple<A*> t7(move(b));

        pair<B*, Y*> by(nullptr, nullptr);

        tuple<A*, X*> t8(by);

        tuple<A*, X*> t9(move(by));

        tuple<int> ti(0);
        tuple<Meow> t10(ti);
        tuple<Meow> t11(move(ti));

        pair<int, int> pii(0, 0);
        tuple<Meow> t12(pii);
        tuple<Meow> t13(move(pii));
    }
}
