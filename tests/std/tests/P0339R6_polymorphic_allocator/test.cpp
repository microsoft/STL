// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <cassert>
#include <cstddef>
#include <cstdint>
#include <limits>
#include <memory>
#include <memory_resource>
#include <new>

using std::pmr::polymorphic_allocator;

void allocate_bytes_test() {
    constexpr int N = 5;

    polymorphic_allocator<> alloc{};

    void* vp = alloc.allocate_bytes(sizeof(int) * N, alignof(int));

    int* arr = static_cast<int*>(vp);

    for (int i = 0; i < N; ++i) {
        alloc.construct(arr + i, i);
    }

    for (int i = 0; i < N; ++i) {
        assert(arr[i] == i);
    }

    std::destroy(arr, arr + N);

    alloc.deallocate_bytes(vp, sizeof(int) * N, alignof(int));

    void* vp2 = alloc.allocate_bytes(sizeof(int));
    assert(reinterpret_cast<std::uintptr_t>(vp2) % alignof(std::max_align_t) == 0);
    alloc.deallocate_bytes(vp2, sizeof(int));
}

void allocate_object_test() {
    constexpr int N = 10;

    polymorphic_allocator<> alloc{};

    int* arr = alloc.allocate_object<int>(N);

    for (int i = 0; i < N; ++i) {
        alloc.construct(arr + i, i);
    }

    for (int i = 0; i < N; ++i) {
        assert(arr[i] == i);
    }

    std::destroy(arr, arr + N);

    alloc.deallocate_object(arr, N);

    // N = 1
    int* p = alloc.allocate_object<int>();
    alloc.construct(p, 20);
    assert(*p == 20);
    std::destroy_at(p);
    alloc.deallocate_object(p);
}

void allocate_object_overflow_test() {
    constexpr auto threshold = std::numeric_limits<std::size_t>::max() / sizeof(int);

    polymorphic_allocator<> alloc{};

    try {
        int* vp = alloc.allocate_object<int>(threshold);
        alloc.deallocate_object(vp, threshold);
    } catch (const std::bad_alloc&) {
    } catch (...) {
        assert(false);
    }

    try {
        [[maybe_unused]] int* vp = alloc.allocate_object<int>(threshold + 1);
    } catch (const std::bad_array_new_length&) {
        return;
    } catch (...) {
        assert(false);
    }
    assert(false);
}

void new_object_test() {
    polymorphic_allocator<> alloc{};

    int* p = alloc.new_object<int>(20);

    assert(*p == 20);

    alloc.delete_object(p);
}


int main() {
    allocate_bytes_test();
    allocate_object_test();
    allocate_object_overflow_test();
    new_object_test();
}
