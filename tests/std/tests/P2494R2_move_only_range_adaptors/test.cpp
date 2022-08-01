// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#include <algorithm>
#include <array>
#include <cassert>
#include <iterator>
#include <ranges>
#include <type_traits>
#include <vector>

using namespace std;

struct BaseTransform {
    BaseTransform(int v) : val(v) {}
    int operator()(int x) {
        return x * val;
    }
    int val;
};

struct CopyableTransform : BaseTransform {
    using BaseTransform::BaseTransform;

    CopyableTransform(const CopyableTransform&)            = default;
    CopyableTransform& operator=(const CopyableTransform&) = default;
};
struct CopyConstructibleTransform : BaseTransform {
    using BaseTransform::BaseTransform;

    CopyConstructibleTransform(const CopyConstructibleTransform&)            = default;
    CopyConstructibleTransform& operator=(const CopyConstructibleTransform&) = delete;
};
struct CopyConstructibleExceptTransform : BaseTransform {
    using BaseTransform::BaseTransform;

    CopyConstructibleExceptTransform(const CopyConstructibleExceptTransform& other) : BaseTransform(other.val) {}
    CopyConstructibleExceptTransform& operator=(const CopyConstructibleExceptTransform&) = delete;
};
struct MovableTransform : BaseTransform {
    using BaseTransform::BaseTransform;

    MovableTransform(const MovableTransform&)            = delete;
    MovableTransform(MovableTransform&&)                 = default;
    MovableTransform& operator=(const MovableTransform&) = delete;
    MovableTransform& operator=(MovableTransform&&)      = default;
};
struct MoveConstructibleTransform : BaseTransform {
    using BaseTransform::BaseTransform;

    MoveConstructibleTransform(const MoveConstructibleTransform&)            = delete;
    MoveConstructibleTransform(MoveConstructibleTransform&&)                 = default;
    MoveConstructibleTransform& operator=(const MoveConstructibleTransform&) = delete;
    MoveConstructibleTransform& operator=(MoveConstructibleTransform&&)      = delete;
};
struct MoveConstructibleExceptTransform : BaseTransform {
    using BaseTransform::BaseTransform;

    MoveConstructibleExceptTransform(const MoveConstructibleExceptTransform&) = delete;
    MoveConstructibleExceptTransform(MoveConstructibleExceptTransform&& other) : BaseTransform(other.val) {}
    MoveConstructibleExceptTransform& operator=(const MoveConstructibleExceptTransform&) = delete;
    MoveConstructibleExceptTransform& operator=(MoveConstructibleExceptTransform&&)      = delete;
};

template <class T>
void test_transform() {
    T t{2};

    vector<int> v;
    ranges::copy(array{0, 1, 2, 3, 4, 5} | views::transform(std::move(t)), back_inserter(v));
    assert(ranges::equal(v, initializer_list<int>{0, 2, 4, 6, 8, 10}));

    if constexpr (copyable<T> || is_nothrow_copy_constructible_v<T>) {
        static_assert(sizeof(ranges::_Movable_box<T>) == sizeof(T));
    } else if constexpr (movable<T> || is_nothrow_move_constructible_v<T>) {
        static_assert(sizeof(ranges::_Movable_box<T>) == sizeof(T));
    }
}

int main() {
    test_transform<CopyableTransform>();
    test_transform<CopyConstructibleTransform>();
    test_transform<CopyConstructibleExceptTransform>();
    test_transform<MovableTransform>();
    test_transform<MoveConstructibleTransform>();
    test_transform<MoveConstructibleExceptTransform>();
}
