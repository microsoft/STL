// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

// N3797 12.8 [class.copy]/11:
// "A defaulted copy/move constructor for a class X is defined as deleted (8.4.3) if X has:
// [...]
// - for the copy constructor, a non-static data member of rvalue reference type."

// 12.8 [class.copy]/15:
// "The implicitly-defined copy/move constructor for a non-union class X performs
// a memberwise copy/move of its bases and members.
// [...]
// Each base or non-static data member is copied/moved in the manner appropriate to its type:
// [...]
// - if a member m has rvalue reference type T&&, it is direct-initialized with static_cast<T&&>(x.m);"

// 20.3.2 [pairs.pair]:
// pair(const pair&) = default;
// pair(pair&&) = default;

// 20.4.2 [tuple.tuple]:
// tuple(const tuple&) = default;
// tuple(tuple&&) = default;

#include <cassert>
#include <tuple>
#include <type_traits>
#include <utility>

using namespace std;

#define STATIC_ASSERT(...) static_assert(__VA_ARGS__, #__VA_ARGS__)

struct X {
    int&& rv;

    explicit X(int&& i) : rv(move(i)) {}
};

int main() {
    {
        int k = 1701;
        int n = 1729;

        X x1(move(n));
        X x3(move(x1));

        assert(&x1.rv == &n);
        assert(&x3.rv == &n);

        pair<int&&, int&&> p1(move(k), move(n));
        pair<int&&, int&&> p3(move(p1));

        assert(&p1.first == &k);
        assert(&p1.second == &n);
        assert(&p3.first == &k);
        assert(&p3.second == &n);

        tuple<int&&, int&&> t1(move(k), move(n));
        tuple<int&&, int&&> t3(move(t1));

        assert(&get<0>(t1) == &k);
        assert(&get<1>(t1) == &n);
        assert(&get<0>(t3) == &k);
        assert(&get<1>(t3) == &n);

        auto&& f1(forward_as_tuple(move(k), move(n)));
        auto f3(move(f1));

        assert(&get<0>(f1) == &k);
        assert(&get<1>(f1) == &n);
        assert(&get<0>(f3) == &k);
        assert(&get<1>(f3) == &n);
    }


    // Also test get<I> and get<T> on pairs of objects, lvalue references, and rvalue references.

    {
        pair<int, const char*> p(1729, "meow");
        const pair<int, const char*> c(1701, "NCC");
        pair<int, const char*> m(1024, "kilo");

        STATIC_ASSERT(is_same_v<decltype(get<0>(p)), int&>);
        STATIC_ASSERT(is_same_v<decltype(get<1>(p)), const char*&>);
        STATIC_ASSERT(is_same_v<decltype(get<0>(c)), const int&>);
        STATIC_ASSERT(is_same_v<decltype(get<1>(c)), const char* const&>);
        STATIC_ASSERT(is_same_v<decltype(get<0>(move(m))), int&&>);
        STATIC_ASSERT(is_same_v<decltype(get<1>(move(m))), const char*&&>);

        STATIC_ASSERT(is_same_v<decltype(get<int>(p)), int&>);
        STATIC_ASSERT(is_same_v<decltype(get<const char*>(p)), const char*&>);
        STATIC_ASSERT(is_same_v<decltype(get<int>(c)), const int&>);
        STATIC_ASSERT(is_same_v<decltype(get<const char*>(c)), const char* const&>);
        STATIC_ASSERT(is_same_v<decltype(get<int>(move(m))), int&&>);
        STATIC_ASSERT(is_same_v<decltype(get<const char*>(move(m))), const char*&&>);

        assert(&get<0>(p) == &p.first);
        assert(&get<1>(p) == &p.second);
        assert(&get<0>(c) == &c.first);
        assert(&get<1>(c) == &c.second);
        assert(get<0>(move(m)) == m.first);
        assert(get<1>(move(m)) == m.second);

        assert(&get<int>(p) == &p.first);
        assert(&get<const char*>(p) == &p.second);
        assert(&get<int>(c) == &c.first);
        assert(&get<const char*>(c) == &c.second);
        assert(get<int>(move(m)) == m.first);
        assert(get<const char*>(move(m)) == m.second);
    }

    {
        int x  = 11;
        char y = 'a';

        pair<int&, char&> p(x, y);
        const pair<int&, char&> c(x, y);

        STATIC_ASSERT(is_same_v<decltype(get<0>(p)), int&>);
        STATIC_ASSERT(is_same_v<decltype(get<1>(p)), char&>);
        STATIC_ASSERT(is_same_v<decltype(get<0>(c)), int&>);
        STATIC_ASSERT(is_same_v<decltype(get<1>(c)), char&>);
        STATIC_ASSERT(is_same_v<decltype(get<0>(move(p))), int&>);
        STATIC_ASSERT(is_same_v<decltype(get<1>(move(p))), char&>);

        STATIC_ASSERT(is_same_v<decltype(get<int&>(p)), int&>);
        STATIC_ASSERT(is_same_v<decltype(get<char&>(p)), char&>);
        STATIC_ASSERT(is_same_v<decltype(get<int&>(c)), int&>);
        STATIC_ASSERT(is_same_v<decltype(get<char&>(c)), char&>);
        STATIC_ASSERT(is_same_v<decltype(get<int&>(move(p))), int&>);
        STATIC_ASSERT(is_same_v<decltype(get<char&>(move(p))), char&>);

        assert(&get<0>(p) == &x);
        assert(&get<1>(p) == &y);
        assert(&get<0>(c) == &x);
        assert(&get<1>(c) == &y);
        assert(&get<0>(move(p)) == &x);
        assert(&get<1>(move(p)) == &y);

        assert(&get<int&>(p) == &x);
        assert(&get<char&>(p) == &y);
        assert(&get<int&>(c) == &x);
        assert(&get<char&>(c) == &y);
        assert(&get<int&>(move(p)) == &x);
        assert(&get<char&>(move(p)) == &y);
    }

    {
        int x  = 11;
        char y = 'a';

        pair<int&&, char&&> p(move(x), move(y));
        const pair<int&&, char&&> c(move(x), move(y));

        STATIC_ASSERT(is_same_v<decltype(get<0>(p)), int&>);
        STATIC_ASSERT(is_same_v<decltype(get<1>(p)), char&>);
        STATIC_ASSERT(is_same_v<decltype(get<0>(c)), int&>);
        STATIC_ASSERT(is_same_v<decltype(get<1>(c)), char&>);
        STATIC_ASSERT(is_same_v<decltype(get<0>(move(p))), int&&>);
        STATIC_ASSERT(is_same_v<decltype(get<1>(move(p))), char&&>);

        STATIC_ASSERT(is_same_v<decltype(get<int&&>(p)), int&>);
        STATIC_ASSERT(is_same_v<decltype(get<char&&>(p)), char&>);
        STATIC_ASSERT(is_same_v<decltype(get<int&&>(c)), int&>);
        STATIC_ASSERT(is_same_v<decltype(get<char&&>(c)), char&>);
        STATIC_ASSERT(is_same_v<decltype(get<int&&>(move(p))), int&&>);
        STATIC_ASSERT(is_same_v<decltype(get<char&&>(move(p))), char&&>);

        assert(&get<0>(p) == &x);
        assert(&get<1>(p) == &y);
        assert(&get<0>(c) == &x);
        assert(&get<1>(c) == &y);
        assert(get<0>(move(p)) == x);
        assert(get<1>(move(p)) == y);

        assert(&get<int&&>(p) == &x);
        assert(&get<char&&>(p) == &y);
        assert(&get<int&&>(c) == &x);
        assert(&get<char&&>(c) == &y);
        assert(get<int&&>(move(p)) == x);
        assert(get<char&&>(move(p)) == y);
    }
}
