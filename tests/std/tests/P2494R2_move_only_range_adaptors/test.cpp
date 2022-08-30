// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#include <algorithm>
#include <array>
#include <cassert>
#include <iterator>
#include <ranges>
#include <type_traits>
#include <utility>
#include <vector>

using namespace std;

struct BaseTransform {
    explicit BaseTransform(int v) : val(v) {}
    int operator()(int x) const {
        return val * x;
    }
    int val;
};

enum SmfKind {
    Nothrow,
    Throwing,
    Deleted,
};

template <SmfKind CCtor, SmfKind MCtor, SmfKind CAssign, SmfKind MAssign>
struct Transform : BaseTransform {
    using BaseTransform::BaseTransform;

    Transform(const Transform&) noexcept(CCtor == Nothrow) //
        requires(CCtor != Deleted)                         = default;
    Transform(const Transform&) requires(CCtor == Deleted) = delete;

    Transform(Transform&&) noexcept(MCtor == Nothrow) //
        requires(MCtor != Deleted)                    = default;
    Transform(Transform&&) requires(MCtor == Deleted) = delete;

    Transform& operator=(const Transform&) noexcept(CAssign == Nothrow) //
        requires(CAssign != Deleted)                                    = default;
    Transform& operator=(const Transform&) requires(CAssign == Deleted) = delete;

    Transform& operator=(Transform&&) noexcept(MAssign == Nothrow) //
        requires(MAssign != Deleted)                               = default;
    Transform& operator=(Transform&&) requires(MAssign == Deleted) = delete;
};

template <SmfKind CCtor, SmfKind MCtor, SmfKind CAssign, SmfKind MAssign>
void test_transform() {
    using T = Transform<CCtor, MCtor, CAssign, MAssign>;
    T t{11};

    vector<int> v;
    ranges::copy(array{0, 1, 2, 3, 4, 5} | views::transform(move(t)), back_inserter(v));
    assert(ranges::equal(v, array{0, 11, 22, 33, 44, 55}));

    if constexpr (is_copy_constructible_v<T>
                      ? copyable<T> || (is_nothrow_copy_constructible_v<T> && is_nothrow_move_constructible_v<T>)
                      : movable<T> || is_nothrow_move_constructible_v<T>) {
        static_assert(sizeof(ranges::_Movable_box<T>) == sizeof(T));
    } else {
        static_assert(sizeof(ranges::_Movable_box<T>) > sizeof(T));
    }
}

int main() {
    test_transform<Nothrow, Nothrow, Nothrow, Nothrow>();
    test_transform<Throwing, Throwing, Throwing, Throwing>();

    test_transform<Nothrow, Nothrow, Deleted, Deleted>();
    test_transform<Nothrow, Throwing, Deleted, Deleted>();
    test_transform<Throwing, Nothrow, Deleted, Deleted>();
    test_transform<Throwing, Throwing, Deleted, Deleted>();

    test_transform<Deleted, Nothrow, Deleted, Nothrow>();
    test_transform<Deleted, Nothrow, Deleted, Throwing>();
    test_transform<Deleted, Nothrow, Deleted, Deleted>();

    test_transform<Deleted, Throwing, Deleted, Nothrow>();
    test_transform<Deleted, Throwing, Deleted, Throwing>();
    test_transform<Deleted, Throwing, Deleted, Deleted>();
}
