// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <algorithm>
#include <cassert>
#include <cstddef>
#include <iostream>
#include <type_traits>
#include <valarray>

#define STATIC_ASSERT(...) static_assert(__VA_ARGS__, #__VA_ARGS__)

template <class T>
bool eq(const std::valarray<T>& v, std::initializer_list<T> il) {
    return std::equal(begin(v), end(v), il.begin(), il.end());
}

void test_slice() {
    std::valarray<int> v{0, 1, 2, 3, 4};

    std::slice_array<int> slice_array      = v[std::slice(2, 2, 2)];
    std::slice_array<int> slice_array_copy = slice_array;
    (void) slice_array_copy;

    assert(eq(v, {0, 1, 2, 3, 4}));

    std::slice_array<int> other_slice_array = v[std::slice(0, 2, 1)];
    other_slice_array                       = slice_array;

    assert(eq(v, {2, 4, 2, 3, 4}));
}

void test_gslice() {
    std::valarray<int> v{0, 1, 2, 3, 4};

    std::gslice gslice(2, std::valarray<std::size_t>{2}, std::valarray<std::size_t>{2});
    std::gslice_array<int> gslice_array      = v[gslice];
    std::gslice_array<int> gslice_array_copy = gslice_array;
    (void) gslice_array_copy;

    assert(eq(v, {0, 1, 2, 3, 4}));

    std::gslice other_gslice(0, std::valarray<std::size_t>{2}, std::valarray<std::size_t>{1});
    std::gslice_array<int> other_gslice_array = v[other_gslice];
    other_gslice_array                        = gslice_array;

    assert(eq(v, {2, 4, 2, 3, 4}));
}

void test_mask() {
    std::valarray<int> v{0, 1, 2, 3, 4};

    std::valarray<bool> mask{true, false, false, false, true};
    std::mask_array<int> mask_array      = v[mask];
    std::mask_array<int> mask_array_copy = mask_array;
    (void) mask_array_copy;

    assert(eq(v, {0, 1, 2, 3, 4}));

    std::valarray<bool> other_mask{false, true, true, false, false};
    std::mask_array<int> other_mask_array = v[other_mask];
    other_mask_array                      = mask_array;

    assert(eq(v, {0, 0, 4, 3, 4}));
}

void test_indirect() {
    std::valarray<int> v{0, 1, 2, 3, 4};

    std::valarray<std::size_t> indices{2, 3};
    std::indirect_array<int> indirect_array      = v[indices];
    std::indirect_array<int> indirect_array_copy = indirect_array;
    (void) indirect_array_copy;

    assert(eq(v, {0, 1, 2, 3, 4}));

    std::valarray<std::size_t> other_indices{4, 0};
    std::indirect_array<int> other_indirect_array = v[other_indices];
    other_indirect_array                          = indirect_array;

    assert(eq(v, {3, 1, 2, 3, 2}));
}

template <class T>
void test_strengthened_exception_specification() {
    STATIC_ASSERT(std::is_nothrow_default_constructible_v<std::valarray<T>>);

    std::valarray<T> va{};
    const auto& cva = va;
    STATIC_ASSERT(noexcept(std::begin(va)));
    STATIC_ASSERT(noexcept(std::end(va)));
    STATIC_ASSERT(noexcept(std::begin(cva)));
    STATIC_ASSERT(noexcept(std::end(cva)));

    STATIC_ASSERT(std::is_nothrow_assignable_v<const std::slice_array<T>, const std::slice_array<T>&>);
    STATIC_ASSERT(std::is_nothrow_assignable_v<const std::slice_array<T>, std::slice_array<T>>);

    STATIC_ASSERT(std::is_nothrow_assignable_v<const std::mask_array<T>, const std::mask_array<T>&>);
    STATIC_ASSERT(std::is_nothrow_assignable_v<const std::mask_array<T>, std::mask_array<T>>);

    STATIC_ASSERT(std::is_nothrow_assignable_v<const std::indirect_array<T>, const std::indirect_array<T>&>);
    STATIC_ASSERT(std::is_nothrow_assignable_v<const std::indirect_array<T>, std::indirect_array<T>>);
}

int main() {
    test_slice();
    test_gslice();
    test_mask();
    test_indirect();

    // Also test strengthened exception specifications
    test_strengthened_exception_specification<bool>();
    test_strengthened_exception_specification<std::size_t>();
    test_strengthened_exception_specification<double>();

#if defined(__clang__) || defined(__EDG__) // TRANSITION, DevCom-10416247
    STATIC_ASSERT(std::is_nothrow_default_constructible_v<std::gslice>); // strengthened
#endif // ^^^ no workaround ^^^
    std::gslice gs{};
    STATIC_ASSERT(noexcept(gs.start())); // strengthened
}
