// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <assert.h>
#include <memory>
#include <utility>
using namespace std;

// Also test GH-1102 "<memory>: weak_ptr conversions don't preserve control blocks for expired objects"
template <typename T, typename U>
[[nodiscard]] bool owner_equal(const weak_ptr<T>& t, const weak_ptr<U>& u) {
    return !t.owner_before(u) && !u.owner_before(t);
}

void test_owner_equal() {
    shared_ptr<int> sp_alive1(new int(0));
    shared_ptr<int> sp_alive2(new int(0));
    shared_ptr<int> sp_expiring3(new int(0));
    shared_ptr<int> sp_expiring4(new int(0));

    weak_ptr<int> wp_empty;
    weak_ptr<int> wp_also_empty;

    weak_ptr<int> wp_alive(sp_alive1);
    weak_ptr<int> wp_alive_same(sp_alive1);
    weak_ptr<int> wp_alive_different(sp_alive2);

    weak_ptr<int> wp_expired(sp_expiring3);
    weak_ptr<int> wp_expired_same(sp_expiring3);
    weak_ptr<int> wp_expired_different(sp_expiring4);

    sp_expiring3.reset();
    sp_expiring4.reset();

    assert(wp_empty.expired());
    assert(wp_also_empty.expired());

    assert(!wp_alive.expired());
    assert(!wp_alive_same.expired());
    assert(!wp_alive_different.expired());

    assert(wp_expired.expired());
    assert(wp_expired_same.expired());
    assert(wp_expired_different.expired());

    assert(owner_equal(wp_empty, wp_also_empty));

    assert(!owner_equal(wp_empty, wp_alive));
    assert(!owner_equal(wp_empty, wp_expired));

    assert(!owner_equal(wp_alive, wp_empty));
    assert(!owner_equal(wp_expired, wp_empty));

    assert(owner_equal(wp_alive, wp_alive_same));
    assert(owner_equal(wp_expired, wp_expired_same));

    assert(!owner_equal(wp_alive, wp_alive_different));
    assert(!owner_equal(wp_alive, wp_expired));
    assert(!owner_equal(wp_expired, wp_alive));
    assert(!owner_equal(wp_expired, wp_expired_different));
}

struct A {
    int a{10};
};

struct B : virtual A {
    int b{20};
};

struct C : virtual A {
    int c{30};
};

struct D : B, C {
    int d{40};
};

int main() {
    test_owner_equal();

    shared_ptr<D> spd(new D);

    const weak_ptr<D> wpd_zero(spd);
    weak_ptr<D> wpd_one(spd);
    weak_ptr<D> wpd_two(spd);

    weak_ptr<A> wpa0(wpd_zero);
    assert(!wpa0.expired());
    assert(owner_equal(wpa0, wpd_zero));
    assert(wpa0.lock()->a == 10);

    spd.reset();

    weak_ptr<A> wpa1(wpd_one);
    assert(wpa1.expired());
    assert(owner_equal(wpa1, wpd_zero));

    weak_ptr<A> wpa2;
    wpa2 = wpd_one;
    assert(wpa2.expired());
    assert(owner_equal(wpa2, wpd_zero));

    weak_ptr<A> wpa3(move(wpd_one));
    assert(wpa3.expired());
    assert(owner_equal(wpa3, wpd_zero));

    weak_ptr<A> wpa4;
    wpa4 = move(wpd_two);
    assert(wpa4.expired());
    assert(owner_equal(wpa4, wpd_zero));
}
