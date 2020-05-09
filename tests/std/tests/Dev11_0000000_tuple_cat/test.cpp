// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <array>
#include <assert.h>
#include <functional>
#include <memory>
#include <string>
#include <tuple>
#include <type_traits>
#include <utility>

using namespace std;

#define STATIC_ASSERT(...) static_assert(__VA_ARGS__, #__VA_ARGS__)

int main() {
    {
        tuple<> x;

        auto t0 = tuple_cat();
        auto t1 = tuple_cat(x);
        auto t2 = tuple_cat(x, x);
        auto t3 = tuple_cat(x, x, x);

        STATIC_ASSERT(is_same_v<decltype(t0), tuple<>>);
        STATIC_ASSERT(is_same_v<decltype(t1), tuple<>>);
        STATIC_ASSERT(is_same_v<decltype(t2), tuple<>>);
        STATIC_ASSERT(is_same_v<decltype(t3), tuple<>>);

        (void) t0;
        (void) t1;
        (void) t2;
        (void) t3;
    }

    {
        tuple<int> a(11);
        const tuple<int> b(22);
        tuple<int> c(33);
        const tuple<int> d(44);

        auto t0 = tuple_cat(a);
        auto t1 = tuple_cat(b);
        auto t2 = tuple_cat(move(c));
        auto t3 = tuple_cat(move(d));
        auto t4 = tuple_cat(a, b, move(c), move(d));

        STATIC_ASSERT(is_same_v<decltype(t0), tuple<int>>);
        STATIC_ASSERT(is_same_v<decltype(t1), tuple<int>>);
        STATIC_ASSERT(is_same_v<decltype(t2), tuple<int>>);
        STATIC_ASSERT(is_same_v<decltype(t3), tuple<int>>);
        STATIC_ASSERT(is_same_v<decltype(t4), tuple<int, int, int, int>>);

        assert(get<0>(t0) == 11);
        assert(get<0>(t1) == 22);
        assert(get<0>(t2) == 33);
        assert(get<0>(t3) == 44);
        assert(t4 == make_tuple(11, 22, 33, 44));
    }

    {
        tuple<> x;

        tuple<short, int, long> a(static_cast<short>(11), 22, 33L);
        tuple<unsigned short, unsigned int> b(static_cast<unsigned short>(44), 55U);
        tuple<unsigned long> c(66UL);
        tuple<long> d(100L);
        tuple<int, short> e(200, static_cast<short>(300));
        tuple<unsigned long, unsigned int, unsigned short> f(400UL, 500U, static_cast<unsigned short>(600));

        auto t0 = tuple_cat(a, b, c, d, e, f);
        auto t1 = tuple_cat(x, x, a, x, b, x, c, x, x, x, d, x, e, x, f, x, x);
        auto t2 = tuple_cat(a);
        auto t3 = tuple_cat(a, f);

        STATIC_ASSERT(is_same_v<decltype(t0), decltype(t1)>);
        STATIC_ASSERT(is_same_v<decltype(t0), tuple<short, int, long, unsigned short, unsigned int, unsigned long, long,
                                                  int, short, unsigned long, unsigned int, unsigned short>>);
        STATIC_ASSERT(is_same_v<decltype(t2), tuple<short, int, long>>);
        STATIC_ASSERT(is_same_v<decltype(t3), tuple<short, int, long, unsigned long, unsigned int, unsigned short>>);

        assert(t0 == t1);
        assert(t0
               == make_tuple(static_cast<short>(11), 22, 33L, static_cast<unsigned short>(44), 55U, 66UL, 100L, 200,
                   static_cast<short>(300), 400UL, 500U, static_cast<unsigned short>(600)));
        assert(t2 == a);
        assert(t3 == make_tuple(static_cast<short>(11), 22, 33L, 400UL, 500U, static_cast<unsigned short>(600)));
    }

    {
        unique_ptr<int> up;

        up.reset(new int(10));
        tuple<string, unique_ptr<int>, int> a("ten", move(up), 11);

        up.reset(new int(20));
        tuple<string, int, unique_ptr<int>> b("twenty", 22, move(up));

        up.reset(new int(30));
        tuple<int, string, unique_ptr<int>> c(33, "thirty", move(up));

        tuple<int, int> d(44, 55);

        auto t0 = tuple_cat(move(a), move(b), move(c), d);

        STATIC_ASSERT(is_same_v<decltype(t0),
            tuple<string, unique_ptr<int>, int, string, int, unique_ptr<int>, int, string, unique_ptr<int>, int, int>>);

        assert(!get<1>(a));
        assert(!get<2>(b));
        assert(!get<2>(c));

        assert(get<0>(t0) == "ten");
        assert(*get<1>(t0) == 10);
        assert(get<2>(t0) == 11);
        assert(get<3>(t0) == "twenty");
        assert(get<4>(t0) == 22);
        assert(*get<5>(t0) == 20);
        assert(get<6>(t0) == 33);
        assert(get<7>(t0) == "thirty");
        assert(*get<8>(t0) == 30);
        assert(get<9>(t0) == 44);
        assert(get<10>(t0) == 55);
    }

    {
        int w = 11, x = 22, y = 33, z = 44;

        tuple<int&, const int&, int&&, const int&&> t0(w, x, move(y), move(z));

        assert(&get<0>(t0) == &w);
        assert(&get<1>(t0) == &x);
        assert(&get<2>(t0) == &y);
        assert(&get<3>(t0) == &z);

        auto t1 = tuple_cat(make_tuple(50, 60), move(t0), make_tuple(70, 80));

        STATIC_ASSERT(is_same_v<decltype(t1), tuple<int, int, int&, const int&, int&&, const int&&, int, int>>);

        assert(t1 == make_tuple(50, 60, 11, 22, 33, 44, 70, 80));
        assert(&get<2>(t1) == &w);
        assert(&get<3>(t1) == &x);
        assert(&get<4>(t1) == &y);
        assert(&get<5>(t1) == &z);
    }

    {
        pair<int, int> p(11, 22);

        const char* const s = "meow";

        array<short, 0> zero;

        array<int, 1> one = {{40}};

        array<long, 2> two = {{50L, 60L}};

        auto make_array = [] {
            array<char, 3> ret = {{'x', 'y', 'z'}};
            return ret;
        };

        const auto t = tuple_cat(p, make_pair(33U, s), zero, one, two, make_array(), make_tuple(1234LL));

        STATIC_ASSERT(is_same_v<decltype(t),
            const tuple<int, int, unsigned int, const char*, int, long, long, char, char, char, long long>>);

        assert(t == make_tuple(11, 22, 33U, s, 40, 50L, 60L, 'x', 'y', 'z', 1234LL));
    }

// Also test C++17 apply() and make_from_tuple().
#if _HAS_CXX17
    {
        struct Point {
            int x;
            int y;

            int add(int z) const {
                return x + y + z;
            }
        };

        const Point p{100, 20};

        assert(apply(&Point::add, make_tuple(&p, 3)) == 123);

        assert(make_from_tuple<string>(make_tuple(static_cast<size_t>(5), 's')) == "sssss");
    }
#endif // _HAS_CXX17

    // Also test VSO-181496 "Variadic template emits spurious warning C4100: unreferenced formal parameter".
    {
        auto b1 = bind([] { return 256; });
        assert(b1() == 256);

        auto b2 = bind([](int i) { return i * 3; }, b1);
        assert(b2() == 768);

        const pair<int, int> p(piecewise_construct, tuple<>{}, tuple<>{});
        assert(p.first == 0);
        assert(p.second == 0);

        assert(tuple_cat() == tuple<>{});

#if _HAS_CXX17
        assert(apply([] { return 1729; }, tuple<>{}) == 1729);

        assert(make_from_tuple<int>(tuple<>{}) == 0);
#endif // _HAS_CXX17
    }
}

// Also test DevDiv#1205400 "C++ compiler: static_assert in std::tuple_element prevents SFINAE".
template <typename T, typename = void>
struct HasTupleElement : false_type {};

template <typename T>
struct HasTupleElement<T, void_t<tuple_element_t<0, T>>> : true_type {};

STATIC_ASSERT(!HasTupleElement<int>::value);
STATIC_ASSERT(HasTupleElement<tuple<short, long>>::value);

// Also test DevDiv#1192603 "<tuple>: tuple_size's static_assert is problematic".
template <typename T, typename = void>
struct HasTupleSize : false_type {};

template <typename T>
struct HasTupleSize<T, void_t<typename tuple_size<T>::type>> : true_type {};

STATIC_ASSERT(!HasTupleSize<int>::value);
STATIC_ASSERT(HasTupleSize<tuple<short, long>>::value);
