// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <cassert>
#include <memory_resource>

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

    for (int i = 0; i < N; ++i) {
        std::destroy_at(arr + i);
    }

    alloc.deallocate_bytes(vp, N, alignof(int));
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

    for (int i = 0; i < N; ++i) {
        std::destroy_at(arr + i);
    }

    alloc.deallocate_object(arr, N);
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
    new_object_test();
}
