// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <memory>
#include <type_traits>
#include <utility>

using namespace std;

template <class T, class ConstructAssert>
struct ConstructConstrainingAllocator {
    using value_type = T;

    ConstructConstrainingAllocator() = default;
    template <class Other>
    ConstructConstrainingAllocator(const ConstructConstrainingAllocator<Other, ConstructAssert>&) {}
    ConstructConstrainingAllocator(const ConstructConstrainingAllocator&) = default;
    ConstructConstrainingAllocator& operator=(const ConstructConstrainingAllocator&) = delete;

    T* allocate(size_t n) {
        return allocator<T>{}.allocate(n);
    }

    void deallocate(T* p, size_t n) noexcept {
        return allocator<T>{}.deallocate(p, n);
    }

    template <class Other, class... Args>
    void construct(Other* p, Args&&... vals) {
        allocator<Other> a;
        static_assert(is_same_v<Other, value_type> && is_same_v<ConstructAssert, Other>, "incorrect construct call");
        allocator_traits<allocator<Other>>::construct(a, p, forward<Args>(vals)...);
    }

    template <class Other>
    void destroy(Other* p) noexcept {
        allocator<Other> a;
        static_assert(is_same_v<Other, value_type> && is_same_v<ConstructAssert, Other>, "incorrect destroy call");
        allocator_traits<allocator<Other>>::destroy(a, p);
    }
};

int main() {
    ConstructConstrainingAllocator<void, int> a{};
    (void) allocate_shared<int>(a, 42);
    (void) allocate_shared<const int>(a, 42);
}
