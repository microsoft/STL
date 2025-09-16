// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <cassert>
#include <cstddef>
#include <cstdint>
#include <functional>

#pragma warning(disable : 4324) // structure was padded due to alignment specifier

// SFO (Small Functor Optimization) should not happen
struct alignas(2 * alignof(std::max_align_t)) overaligned_t {
    char non_empty;

    void operator()(const void* const storage, const std::size_t storage_size) const {
        const auto storage_ptr_value = reinterpret_cast<std::uintptr_t>(storage);
        const auto this_ptr_value    = reinterpret_cast<std::uintptr_t>(this);

        // Platform-specific behavior not covered by Standard C++, but fine for this test
        assert(this_ptr_value < storage_ptr_value || this_ptr_value >= storage_ptr_value + storage_size);

        // Before C++17, alignas isn't helpful for aligning allocations via "new"
#ifdef __cpp_aligned_new
        assert(this_ptr_value % alignof(overaligned_t) == 0);
#endif
    }
};

// SFO should happen
struct not_overaligned_t {
    char data[sizeof(overaligned_t)];

    void operator()(const void* const storage, const std::size_t storage_size) const {
        const auto storage_ptr_value = reinterpret_cast<std::uintptr_t>(storage);
        const auto this_ptr_value    = reinterpret_cast<std::uintptr_t>(this);

        // Platform-specific behavior not covered by Standard C++, but fine for this test
        assert(this_ptr_value >= storage_ptr_value && this_ptr_value < storage_ptr_value + storage_size);
    }
};

static_assert(alignof(overaligned_t) > alignof(std::max_align_t), "overaligned_t is not overaligned");

template <class function_t>
void test() {
    struct functions_t {
        function_t first{overaligned_t{}};
        char smallest_pad;
        function_t second{overaligned_t{}};
        function_t third{overaligned_t{}};
    };

    functions_t functions;
    functions.first(&functions.first, sizeof(functions.first));
    functions.second(&functions.second, sizeof(functions.second));
    functions.third(&functions.third, sizeof(functions.third));

    function_t sfo{not_overaligned_t{}};
    sfo(&sfo, sizeof(sfo));
}


int main() {
    test<std::function<void(const void* storage, std::size_t storage_size)>>();
#if _HAS_CXX23
    test<std::move_only_function<void(const void* storage, std::size_t storage_size)>>();
#endif
}
