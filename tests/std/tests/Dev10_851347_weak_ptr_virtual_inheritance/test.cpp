// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <assert.h>
#include <memory>
#include <utility>

using namespace std;

struct A {
    int a;
};

struct B : virtual public A {
    int b;
};

struct C : virtual public A {
    int c;
};

struct D : public B, public C {
    int d;
};

int main() {
    shared_ptr<D> spd(new D);

    weak_ptr<D> wpd(spd);
    weak_ptr<D> wpd2(spd);

    spd.reset();

    weak_ptr<A> wpa1(wpd);

    assert(wpa1.expired());

    weak_ptr<A> wpa2;

    wpa2 = wpd;

    assert(wpa2.expired());


    weak_ptr<A> wpa3(move(wpd));
    assert(wpa3.expired());

    weak_ptr<A> wpa4;
    wpa4 = move(wpd2);
    assert(wpa4.expired());
}
