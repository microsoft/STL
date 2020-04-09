// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <cassert>
#include <cstddef>
#include <cstdint>
#include <functional>

#ifndef __cpp_aligned_new
#error This test is only valid when C++17 over-aligned allocation is supported
#endif

#pragma warning(disable : 4324) // "was padded due to alignment specifier", thanks, Captain

struct alignas(2 * alignof(std::max_align_t)) overaligned_t {
    char non_empty;

    void operator()(const void* storage, std::size_t storage_size) const {
        const unsigned char* storage_bytes = reinterpret_cast<const unsigned char*>(storage);
        const unsigned char* this_bytes    = reinterpret_cast<const unsigned char*>(this);

        // platform-specific behavior not covered by Standard C++, but fine for such test
        assert(this_bytes < storage_bytes || this_bytes > storage_bytes + storage_size);
        assert(static_cast<std::size_t>(reinterpret_cast<std::uintptr_t>(this) % alignof(overaligned_t)) == 0);
    }
};

static_assert(alignof(overaligned_t) < sizeof(std::function<void()>), "overaligned_t is not aligned as expected");
static_assert(alignof(overaligned_t) > alignof(std::max_align_t), "overaligned_t is not overaligned");

struct functions_t {
    using function_t = std::function<void(const void* storage, std::size_t storage_size)>;

    function_t first{overaligned_t{}};
    char smallest_pad;
    function_t second{overaligned_t{}};
    function_t third{overaligned_t{}};
};

int main() {
    functions_t functions;
    functions.first(&functions.first, sizeof(functions.first));
    functions.second(&functions.second, sizeof(functions.second));
    functions.third(&functions.third, sizeof(functions.third));
    return 0;
}
