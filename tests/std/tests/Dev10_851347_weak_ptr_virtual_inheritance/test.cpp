// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <cassert>
#include <memory>
#include <utility>
#ifndef _M_CEE_PURE // in /clr:pure we miss runtime coverage of weak_ptr converting constructor
#include <atomic>
#include <thread>
#endif // _M_CEE_PURE

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

void test_gh_258() {
    // GH-258 <memory>: weak_ptr's converting constructors could sometimes avoid locking
#ifndef _M_CEE_PURE
    struct base1 {
        int i = 0;
    };

    struct base2 {
        int j = 0;
    };

    struct base3 {
        int k = 0;
    };

    struct derived : virtual base1, virtual base2, base3 {};

    static_assert(weak_ptr<base1>::_Must_avoid_expired_conversions_from<derived>, "Should avoid expired");
    static_assert(weak_ptr<base2>::_Must_avoid_expired_conversions_from<derived>, "Should avoid expired");
    static_assert(weak_ptr<const base1>::_Must_avoid_expired_conversions_from<derived>, "Should avoid expired");
    static_assert(weak_ptr<const base2>::_Must_avoid_expired_conversions_from<derived>, "Should avoid expired");
    static_assert(weak_ptr<const base1>::_Must_avoid_expired_conversions_from<const derived>, "Should avoid expired");
    static_assert(weak_ptr<const base2>::_Must_avoid_expired_conversions_from<const derived>, "Should avoid expired");

    static_assert(!weak_ptr<base3>::_Must_avoid_expired_conversions_from<derived>, "Should optimize");
    static_assert(!weak_ptr<const base3>::_Must_avoid_expired_conversions_from<derived>, "Should optimize");
    static_assert(!weak_ptr<const base3>::_Must_avoid_expired_conversions_from<const derived>, "Should optimize");
    static_assert(!weak_ptr<derived>::_Must_avoid_expired_conversions_from<derived>, "Should optimize");
    static_assert(!weak_ptr<const derived>::_Must_avoid_expired_conversions_from<derived>, "Should optimize");
    static_assert(!weak_ptr<const derived>::_Must_avoid_expired_conversions_from<const derived>, "Should optimize");

    static_assert(!weak_ptr<int>::_Must_avoid_expired_conversions_from<int>, "Should optimize");
    static_assert(!weak_ptr<const int>::_Must_avoid_expired_conversions_from<int>, "Should optimize");
    static_assert(!weak_ptr<const int>::_Must_avoid_expired_conversions_from<const int>, "Should optimize");

    for (int i = 0; i < 10; ++i) {
        // not make_shared -- with make_shared the test would not catch errors
        shared_ptr<derived> d{new derived{}};
        weak_ptr<derived> wd{d};
        atomic<bool> work{true};
        thread thd{[&] {
            d.reset();
            this_thread::yield(); // make crash on incorrect optimization even more likely
            work = false;
        }};

        if ((i % 2) == 0) {
            while (work) {
                // likely to crash if optimized for a case we shouldn't
                weak_ptr<base1> wb1{wd};
                weak_ptr<base2> wb2{wd};
                weak_ptr<const base3> wb3{wd};
            }
        } else {
            while (work) {
                // likely to crash if optimized for a case we shouldn't
                weak_ptr<base1> wb1{weak_ptr<derived>{wd}};
                weak_ptr<base2> wb2{weak_ptr<derived>{wd}};
                weak_ptr<base3> wb3{weak_ptr<derived>{wd}};
            }
        }

        thd.join();
    }
#endif // _M_CEE_PURE
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
    test_gh_258();

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
