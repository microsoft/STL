// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <cassert>
#include <map>
#include <memory>
#include <scoped_allocator>
#include <tuple>
#include <type_traits>
#include <utility>

using namespace std;

void test_P0475R1() {
    struct DoNotCopy {
        DoNotCopy() = default;
        DoNotCopy(const DoNotCopy&) {
            assert(false);
        }
    };

    struct X {
        using allocator_type = allocator<int>;
        X(DoNotCopy&&, const allocator_type&) {}
    };

    scoped_allocator_adaptor<allocator<pair<X, int>>> alloc;
    auto ptr = alloc.allocate(1);
    alloc.construct(ptr, piecewise_construct, tuple<DoNotCopy>{}, make_tuple(1));
    alloc.destroy(ptr);
    alloc.deallocate(ptr, 1);
}

constexpr bool test_P0591R4() {
    allocator<int> alloc;
    int i = 5;
    pair p(i, i);

    struct AllocatorArgConstructible {
        using allocator_type = allocator<int>;

        constexpr AllocatorArgConstructible(allocator_arg_t, const allocator<int>&, int y) : x(y) {}

        int x;
    };

    struct AllocatorConstructible {
        using allocator_type = allocator<int>;

        constexpr AllocatorConstructible(int y, const allocator<int>&) : x(y) {}

        int x;
    };

    struct OnlyAllocatorArgConstructible {
        using allocator_type = allocator<int>;

        constexpr OnlyAllocatorArgConstructible(allocator_arg_t, const allocator<int>&) {}
    };

    struct OnlyAllocatorConstructible {
        using allocator_type = allocator<int>;

        constexpr OnlyAllocatorConstructible(const allocator<int>&) {}
    };

    struct DefaultConstructible {
        constexpr DefaultConstructible() {}
    };

    using AllocatorArgConstructArgs      = tuple<allocator_arg_t, const allocator<int>&, int&>;
    using AllocatorConstructArgs         = tuple<int&, const allocator<int>&>;
    using ConstAllocatorArgConstructArgs = tuple<allocator_arg_t, const allocator<int>&, const int&>;
    using ConstAllocatorConstructArgs    = tuple<const int&, const allocator<int>&>;
    using MovedAllocatorArgConstructArgs = tuple<allocator_arg_t, const allocator<int>&, int&&>;
    using MovedAllocatorConstructArgs    = tuple<int&&, const allocator<int>&>;
#if _HAS_CXX23
    using MovedConstAllocatorArgConstructArgs = tuple<allocator_arg_t, const allocator<int>&, const int&&>;
    using MovedConstAllocatorConstructArgs    = tuple<const int&&, const allocator<int>&>;
#endif // _HAS_CXX23
    using OnlyAllocatorArgConstructArgs = tuple<allocator_arg_t, const allocator<int>&>;
    using OnlyAllocatorConstructArgs    = tuple<const allocator<int>&>;
    using DefaultConstructArgs          = tuple<>;

    { // non-pair overload
        auto tuple1 = uses_allocator_construction_args<int>(alloc, i);
        static_assert(is_same_v<decltype(tuple1), tuple<int&>>);

        auto tuple2 = uses_allocator_construction_args<AllocatorArgConstructible>(alloc, i);
        static_assert(is_same_v<decltype(tuple2), AllocatorArgConstructArgs>);

        auto tuple3 = uses_allocator_construction_args<AllocatorConstructible>(alloc, i);
        static_assert(is_same_v<decltype(tuple3), AllocatorConstructArgs>);
    }

    { // pair(piecewise_construct_t, tuple, tuple) overload
        auto tuple4 = uses_allocator_construction_args<pair<int, OnlyAllocatorArgConstructible>>(
            alloc, piecewise_construct, forward_as_tuple(i), forward_as_tuple());
        static_assert(
            is_same_v<decltype(tuple4), tuple<piecewise_construct_t, tuple<int&>, OnlyAllocatorArgConstructArgs>>);

        auto tuple5 = uses_allocator_construction_args<pair<AllocatorConstructible, DefaultConstructible>>(
            alloc, piecewise_construct, forward_as_tuple(i), forward_as_tuple());
        static_assert(
            is_same_v<decltype(tuple5), tuple<piecewise_construct_t, AllocatorConstructArgs, DefaultConstructArgs>>);
    }

    { // pair() overload
        auto tuple6 =
            uses_allocator_construction_args<pair<DefaultConstructible, OnlyAllocatorArgConstructible>>(alloc);
        static_assert(is_same_v<decltype(tuple6),
            tuple<piecewise_construct_t, DefaultConstructArgs, OnlyAllocatorArgConstructArgs>>);

        auto tuple7 = uses_allocator_construction_args<pair<OnlyAllocatorConstructible, DefaultConstructible>>(alloc);
        static_assert(is_same_v<decltype(tuple7),
            tuple<piecewise_construct_t, OnlyAllocatorConstructArgs, DefaultConstructArgs>>);
    }

    { // pair(first, second) overload
        auto tuple8 = uses_allocator_construction_args<pair<int, AllocatorArgConstructible>>(alloc, i, i);
        static_assert(
            is_same_v<decltype(tuple8), tuple<piecewise_construct_t, tuple<int&>, AllocatorArgConstructArgs>>);

        auto tuple9 = uses_allocator_construction_args<pair<AllocatorConstructible, int>>(alloc, i, i);
        static_assert(is_same_v<decltype(tuple9), tuple<piecewise_construct_t, AllocatorConstructArgs, tuple<int&>>>);
    }

    { // pair(const pair&) overload before C++23; pair(pair&) overload since C++23
        auto tuple10 = uses_allocator_construction_args<pair<int, AllocatorArgConstructible>>(alloc, p);
#if _HAS_CXX23
        static_assert(
            is_same_v<decltype(tuple10), tuple<piecewise_construct_t, tuple<int&>, AllocatorArgConstructArgs>>);
#else // _HAS_CXX23
        static_assert(is_same_v<decltype(tuple10),
            tuple<piecewise_construct_t, tuple<const int&>, ConstAllocatorArgConstructArgs>>);
#endif // _HAS_CXX23

        auto tuple11 = uses_allocator_construction_args<pair<AllocatorConstructible, int>>(alloc, p);
#if _HAS_CXX23
        static_assert(is_same_v<decltype(tuple11), tuple<piecewise_construct_t, AllocatorConstructArgs, tuple<int&>>>);
#else // _HAS_CXX23
        static_assert(
            is_same_v<decltype(tuple11), tuple<piecewise_construct_t, ConstAllocatorConstructArgs, tuple<const int&>>>);
#endif // _HAS_CXX23
    }

    { // pair(const pair&) overload
        auto tuple10 = uses_allocator_construction_args<pair<int, AllocatorArgConstructible>>(alloc, as_const(p));
        static_assert(is_same_v<decltype(tuple10),
            tuple<piecewise_construct_t, tuple<const int&>, ConstAllocatorArgConstructArgs>>);

        auto tuple11 = uses_allocator_construction_args<pair<AllocatorConstructible, int>>(alloc, as_const(p));
        static_assert(
            is_same_v<decltype(tuple11), tuple<piecewise_construct_t, ConstAllocatorConstructArgs, tuple<const int&>>>);
    }

    { // pair(pair&&) overload
        auto tuple12 = uses_allocator_construction_args<pair<int, AllocatorArgConstructible>>(alloc, move(p));
        static_assert(
            is_same_v<decltype(tuple12), tuple<piecewise_construct_t, tuple<int&&>, MovedAllocatorArgConstructArgs>>);

        auto tuple13 = uses_allocator_construction_args<pair<AllocatorConstructible, int>>(alloc, move(p));
        static_assert(
            is_same_v<decltype(tuple13), tuple<piecewise_construct_t, MovedAllocatorConstructArgs, tuple<int&&>>>);
    }

#if _HAS_CXX23
    { // pair(const pair&&) overload
        auto tuple12 = uses_allocator_construction_args<pair<int, AllocatorArgConstructible>>(alloc, move(as_const(p)));
        static_assert(is_same_v<decltype(tuple12),
            tuple<piecewise_construct_t, tuple<const int&&>, MovedConstAllocatorArgConstructArgs>>);

        auto tuple13 = uses_allocator_construction_args<pair<AllocatorConstructible, int>>(alloc, move(as_const(p)));
        static_assert(is_same_v<decltype(tuple13),
            tuple<piecewise_construct_t, MovedConstAllocatorConstructArgs, tuple<const int&&>>>);
    }
#endif // _HAS_CXX23

    {
        auto obj1 = make_obj_using_allocator<AllocatorArgConstructible>(alloc, i);
        static_assert(is_same_v<decltype(obj1), AllocatorArgConstructible>);
        assert(obj1.x == i);

        auto obj2 = make_obj_using_allocator<AllocatorConstructible>(alloc, i);
        static_assert(is_same_v<decltype(obj2), AllocatorConstructible>);
        assert(obj2.x == i);
    }

    {
        allocator<AllocatorArgConstructible> alloc2;
        auto ptr2 = alloc2.allocate(1);

        uninitialized_construct_using_allocator(ptr2, alloc, i);
        assert(ptr2->x == i);
        destroy_at(ptr2);

        alloc2.deallocate(ptr2, 1);

        allocator<AllocatorConstructible> alloc3;
        auto ptr3 = alloc3.allocate(1);

        uninitialized_construct_using_allocator(ptr3, alloc, i);
        assert(ptr3->x == i);
        destroy_at(ptr3);

        alloc3.deallocate(ptr3, 1);
    }

    return true;
}

void test_gh_2021() { // COMPILE-ONLY
    // GH-2021 <map>: Using operator[] on a std::pmr::map fails to compile when the mapped type is a std::pair
    pmr::map<int, pair<int, int>> tags;
    tags[0];
}

struct MoveOnlyType {
    MoveOnlyType()               = default;
    MoveOnlyType(MoveOnlyType&&) = default;
};

void test_lwg_3527() { // COMPILE-ONLY
    // LWG-3527: "uses_allocator_construction_args handles rvalue pairs of rvalue references incorrectly"
    allocator<MoveOnlyType> alloc;
    MoveOnlyType obj;
    pair<MoveOnlyType&&, MoveOnlyType&&> p{move(obj), move(obj)};
    [[maybe_unused]] auto t = uses_allocator_construction_args<pair<MoveOnlyType&&, MoveOnlyType&&>>(alloc, move(p));
}

int main() {
    test_P0475R1();

    assert(test_P0591R4());
    static_assert(test_P0591R4());
}
