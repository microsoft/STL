// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <cassert>
#include <cstddef>
#include <memory>
#include <tuple>
#include <type_traits>
#include <utility>
#include <vector>

#if _HAS_CXX20
#define CONSTEXPR20 constexpr
#else // ^^^ _HAS_CXX20 / !_HAS_CXX20 vvv
#define CONSTEXPR20 inline
#endif // ^^^ !_HAS_CXX20 ^^^

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

template <class T>
class payloaded_allocator {
private:
    int payload = 0;

public:
    payloaded_allocator() = default;

    constexpr explicit payloaded_allocator(int n) noexcept : payload{n} {}

    template <class U>
    constexpr explicit payloaded_allocator(payloaded_allocator<U> a) noexcept : payload{a.get_payload()} {}

    template <class U>
    friend constexpr bool operator==(payloaded_allocator x, payloaded_allocator<U> y) noexcept {
        return x.payload == y.payload;
    }

#if !_HAS_CXX20
    template <class U>
    friend constexpr bool operator!=(payloaded_allocator x, payloaded_allocator<U> y) noexcept {
        return !(x == y);
    }
#endif // !_HAS_CXX20

    using value_type = T;

    CONSTEXPR20 T* allocate(size_t n) {
        return allocator<T>{}.allocate(n);
    }

    CONSTEXPR20 void deallocate(T* p, size_t n) {
        return allocator<T>{}.deallocate(p, n);
    }

    constexpr int get_payload() const noexcept {
        return payload;
    }
};

template <class T, class A>
class vector_holder {
public:
    vector_holder() = default;
    template <class... Args>
    CONSTEXPR20 explicit vector_holder(Args&&... args, const A& alloc) : vec_(args..., alloc) {}

    CONSTEXPR20 A get_allocator() const noexcept {
        return vec_.get_allocator();
    }

private:
    vector<T, A> vec_;
};

template <class T, class A>
struct std::uses_allocator<vector_holder<T, A>, A> : true_type {};

class payload_taker {
public:
    payload_taker() = default;

    template <class T>
    constexpr explicit payload_taker(const payloaded_allocator<T>& alloc) noexcept : payload{alloc.get_payload()} {}

    constexpr int get_payload() const noexcept {
        return payload;
    }
    int get_payload() const volatile noexcept {
        return payload;
    }

private:
    int payload = 0;
};

template <class T>
struct std::uses_allocator<payload_taker, payloaded_allocator<T>> : true_type {};

// LWG-3677 "Is a cv-qualified pair specially handled in uses-allocator construction?"
CONSTEXPR20 bool test_lwg3677() {
    using my_allocator = payloaded_allocator<int>;

    tuple<payload_taker> t1{allocator_arg, my_allocator{42}};
    assert(get<0>(t1).get_payload() == 42);
    tuple<const payload_taker> t2{allocator_arg, my_allocator{84}};
    assert(get<0>(t2).get_payload() == 84);

    tuple<vector_holder<int, my_allocator>> t3{allocator_arg, my_allocator{126}};
    assert(get<0>(t3).get_allocator().get_payload() == 126);
    tuple<const vector_holder<int, my_allocator>> t4{allocator_arg, my_allocator{168}};
    assert(get<0>(t4).get_allocator().get_payload() == 168);

    return true;
}

void test_lwg3677_volatile() {
    using my_allocator = payloaded_allocator<int>;

    tuple<volatile payload_taker> t5{allocator_arg, my_allocator{210}};
    assert(get<0>(t5).get_payload() == 210);
    tuple<const volatile payload_taker> t6{allocator_arg, my_allocator{252}};
    assert(get<0>(t6).get_payload() == 252);
}

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

    test_lwg3677();
#if _HAS_CXX20
    static_assert(test_lwg3677());
#endif // _HAS_CXX20
    test_lwg3677_volatile();
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
