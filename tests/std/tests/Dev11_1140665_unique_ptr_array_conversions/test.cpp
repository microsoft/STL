// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <cassert>
#include <memory>
#include <type_traits>
#include <utility>

using namespace std;

#define STATIC_ASSERT(...) static_assert(__VA_ARGS__, #__VA_ARGS__)

int g_objects = 0;

struct X {
    X() {
        ++g_objects;
    }
    ~X() noexcept {
        --g_objects;
    }

    X(const X&)            = delete;
    X& operator=(const X&) = delete;
};

int overloaded_dd(default_delete<X const[][4]>) {
    return 100;
}
int overloaded_dd(default_delete<int const[][7]>) {
    return 200;
}

int overloaded_up(unique_ptr<X const>) {
    return 300;
}
int overloaded_up(unique_ptr<int const>) {
    return 400;
}
int overloaded_up(unique_ptr<X const[][4]>) {
    return 500;
}
int overloaded_up(unique_ptr<int const[][7]>) {
    return 600;
}


// LWG-2801 "Default-constructibility of unique_ptr"

namespace lwg_2801 {
    STATIC_ASSERT(is_default_constructible_v<unique_ptr<int>>);
    STATIC_ASSERT(is_default_constructible_v<unique_ptr<int[]>>);
    STATIC_ASSERT(is_constructible_v<unique_ptr<int>, int*>);
    STATIC_ASSERT(is_constructible_v<unique_ptr<int[]>, int*>);

    // unique_ptrs with pointer deleter
    using vpfvp = void (*)(void*);
    STATIC_ASSERT(!is_default_constructible_v<unique_ptr<int, vpfvp>>);
    STATIC_ASSERT(!is_default_constructible_v<unique_ptr<int[], vpfvp>>);
    STATIC_ASSERT(!is_constructible_v<unique_ptr<int, vpfvp>, nullptr_t>);
    STATIC_ASSERT(!is_constructible_v<unique_ptr<int[], vpfvp>, nullptr_t>);
    STATIC_ASSERT(!is_constructible_v<unique_ptr<int, vpfvp>, int*>);
    STATIC_ASSERT(!is_constructible_v<unique_ptr<int[], vpfvp>, int*>);

    // unique_ptrs with non-default-constructible deleter:
    struct NonDefaultConstructibleDeleter {
        NonDefaultConstructibleDeleter(int);
        void operator()(void*);
    };
    STATIC_ASSERT(!is_default_constructible_v<unique_ptr<int, NonDefaultConstructibleDeleter>>);
    STATIC_ASSERT(!is_default_constructible_v<unique_ptr<int[], NonDefaultConstructibleDeleter>>);
    STATIC_ASSERT(!is_constructible_v<unique_ptr<int, NonDefaultConstructibleDeleter>, nullptr_t>);
    STATIC_ASSERT(!is_constructible_v<unique_ptr<int[], NonDefaultConstructibleDeleter>, nullptr_t>);
    STATIC_ASSERT(!is_constructible_v<unique_ptr<int, NonDefaultConstructibleDeleter>, int*>);
    STATIC_ASSERT(!is_constructible_v<unique_ptr<int[], NonDefaultConstructibleDeleter>, int*>);

    // Other tests for the template pointer constructor of unique_ptr<T[]>
    STATIC_ASSERT(is_constructible_v<unique_ptr<const int[]>, int*>);
    STATIC_ASSERT(!is_constructible_v<unique_ptr<int[]>, void*>);
    STATIC_ASSERT(!is_constructible_v<unique_ptr<int[]>, void (*)()>);
} // namespace lwg_2801


// LWG-2905 "is_constructible_v<unique_ptr<P, D>, P, D const &> should be false when D is not copy constructible"

namespace lwg_2905 {
    void test_nothrow() {
        // per LWG-2905
        using T       = int;
        using pointer = int*;
        {
            using A = default_delete<int>;
            using D = A;
            STATIC_ASSERT(is_nothrow_constructible_v<unique_ptr<T, D>, pointer, const A&>);
            STATIC_ASSERT(is_nothrow_constructible_v<unique_ptr<T[], D>, pointer, const A&>);
            STATIC_ASSERT(is_nothrow_constructible_v<unique_ptr<T, D>, pointer, A>);
            STATIC_ASSERT(is_nothrow_constructible_v<unique_ptr<T[], D>, pointer, A>);
        }
        {
            using A = default_delete<int>;
            using D = A&;
            STATIC_ASSERT(is_nothrow_constructible_v<unique_ptr<T, D>, pointer, A&>);
            STATIC_ASSERT(is_nothrow_constructible_v<unique_ptr<T[], D>, pointer, A&>);
            STATIC_ASSERT(!is_constructible_v<unique_ptr<T, D>, pointer, A>);
            STATIC_ASSERT(!is_constructible_v<unique_ptr<T[], D>, pointer, A>);
        }
        {
            using A = default_delete<int>;
            using D = const A&;
            STATIC_ASSERT(is_nothrow_constructible_v<unique_ptr<T, D>, pointer, const A&>);
            STATIC_ASSERT(is_nothrow_constructible_v<unique_ptr<T[], D>, pointer, const A&>);
            STATIC_ASSERT(!is_constructible_v<unique_ptr<T, D>, pointer, const A>);
            STATIC_ASSERT(!is_constructible_v<unique_ptr<T[], D>, pointer, const A>);
        }
    }

    void test_sfinae() {
        // per LWG-2905
        // also test LWG-2899 "is_(nothrow_)move_constructible and tuple, optional and unique_ptr"
        using T       = int;
        using pointer = int*;
        struct Immobile {
            Immobile()                           = default;
            Immobile(Immobile const&)            = delete;
            Immobile& operator=(Immobile const&) = delete;

            void operator()(int*) const {}
        };
        {
            using A = Immobile;
            using D = A;
            STATIC_ASSERT(!is_constructible_v<unique_ptr<T, D>, pointer, const A&>);
            STATIC_ASSERT(!is_constructible_v<unique_ptr<T[], D>, pointer, const A&>);
            STATIC_ASSERT(!is_constructible_v<unique_ptr<T, D>, pointer, A>);
            STATIC_ASSERT(!is_constructible_v<unique_ptr<T[], D>, pointer, A>);

            // per LWG-2899
            STATIC_ASSERT(!is_move_constructible_v<unique_ptr<T, D>>);
            STATIC_ASSERT(!is_move_constructible_v<unique_ptr<T[], D>>);
            STATIC_ASSERT(!is_move_assignable_v<unique_ptr<T, D>>);
            STATIC_ASSERT(!is_move_assignable_v<unique_ptr<T[], D>>);
        }
        {
            using A = Immobile;
            using D = A&;
            STATIC_ASSERT(is_nothrow_constructible_v<unique_ptr<T, D>, pointer, A&>);
            STATIC_ASSERT(is_nothrow_constructible_v<unique_ptr<T[], D>, pointer, A&>);
            STATIC_ASSERT(!is_constructible_v<unique_ptr<T, D>, pointer, A>);
            STATIC_ASSERT(!is_constructible_v<unique_ptr<T[], D>, pointer, A>);

            // per LWG-2899
            STATIC_ASSERT(is_nothrow_move_constructible_v<unique_ptr<T, D>>);
            STATIC_ASSERT(is_nothrow_move_constructible_v<unique_ptr<T[], D>>);
            STATIC_ASSERT(!is_move_assignable_v<unique_ptr<T, D>>);
            STATIC_ASSERT(!is_move_assignable_v<unique_ptr<T[], D>>);
        }
        {
            using A = Immobile;
            using D = const A&;
            STATIC_ASSERT(is_nothrow_constructible_v<unique_ptr<T, D>, pointer, const A&>);
            STATIC_ASSERT(is_nothrow_constructible_v<unique_ptr<T[], D>, pointer, const A&>);
            STATIC_ASSERT(!is_constructible_v<unique_ptr<T, D>, pointer, const A>);
            STATIC_ASSERT(!is_constructible_v<unique_ptr<T[], D>, pointer, const A>);

            // per LWG-2899
            STATIC_ASSERT(is_nothrow_move_constructible_v<unique_ptr<T, D>>);
            STATIC_ASSERT(is_nothrow_move_constructible_v<unique_ptr<T[], D>>);
            STATIC_ASSERT(!is_move_assignable_v<unique_ptr<T, D>>);
            STATIC_ASSERT(!is_move_assignable_v<unique_ptr<T[], D>>);
        }
    }

    void test() {
        test_nothrow();
        test_sfinae();
    }
} // namespace lwg_2905

// also test DevCom-754487 "std::swap of unique_ptr to incomplete type fails to compile"
struct incomplete;
void my_swap(unique_ptr<incomplete>& lhs, unique_ptr<incomplete>& rhs) {
    STATIC_ASSERT(noexcept(swap(lhs, rhs)));
    swap(lhs, rhs);
}

// also test LWG-3865 Sorting a range of pairs
constexpr bool test_lwg3865() {
    const pair<int, int> a{1, 2};
    const pair<long, long> b{1, 2};
    const pair<long, long> c{2, 2};
    assert(a == b);
    assert(a != c);
    assert(c >= a);
    assert(b >= a);
    assert(c > a);
    assert(!(b > a));
    assert(a < c);
    assert(!(a < b));
    assert(a <= c);
    assert(a <= b);

    return true;
}

int main() {
    {
        assert(g_objects == 0);
        X(*p)[4] = new X[3][4];
        assert(g_objects == 12);
        X const(*p2)[4] = p;
        delete[] p2;
        assert(g_objects == 0);
    }

    {
        assert(g_objects == 0);
        X(*p)[4] = new X[3][4];
        assert(g_objects == 12);
        default_delete<X const[][4]> del;
        del(p);
        assert(g_objects == 0);
    }

    assert(overloaded_dd(default_delete<X[][4]>{}) == 100);
    assert(overloaded_dd(default_delete<int[][7]>{}) == 200);

    STATIC_ASSERT(is_same_v<unique_ptr<float>::pointer, float*>);
    STATIC_ASSERT(is_same_v<unique_ptr<double[]>::pointer, double*>);

    {
        assert(g_objects == 0);
        unique_ptr<X const[][4]> up(new X[3][4]);
        assert(g_objects == 12);
        up.reset();
        assert(g_objects == 0);
        up.reset(new X[11][4]);
        assert(g_objects == 44);
        up.reset(nullptr);
        assert(g_objects == 0);
    }

    {
        assert(g_objects == 0);
        default_delete<X const[][4]> del;
        unique_ptr<X const[][4]> up(new X[3][4], del);
        assert(g_objects == 12);
        up.reset();
        assert(g_objects == 0);
    }

    {
        assert(g_objects == 0);
        default_delete<X const[][4]> del;
        unique_ptr<X const[][4]> up(new X[3][4], move(del));
        assert(g_objects == 12);
        up.reset();
        assert(g_objects == 0);
    }

    assert(overloaded_up(unique_ptr<X>{}) == 300);
    assert(overloaded_up(unique_ptr<int>{}) == 400);
    assert(overloaded_up(unique_ptr<X[][4]>{}) == 500);
    assert(overloaded_up(unique_ptr<int[][7]>{}) == 600);

    {
        assert(g_objects == 0);
        unique_ptr<X[][4]> src(new X[3][4]);
        assert(g_objects == 12);
        unique_ptr<X const[][4]> dest = move(src);
        assert(!src);
        assert(g_objects == 12);
        dest.reset();
        assert(g_objects == 0);
        src.reset(new X[5][4]);
        assert(g_objects == 20);
        dest = move(src);
        assert(!src);
        assert(g_objects == 20);
        dest.reset();
        assert(g_objects == 0);
    }

    lwg_2905::test();

    {
        // Guard against regression of VSO-542636
        struct S {
            virtual ~S()     = default;
            virtual void f() = 0;
        };
        (void) make_unique<unique_ptr<S>[]>(42);
    }

    test_lwg3865();
    STATIC_ASSERT(test_lwg3865());
}
