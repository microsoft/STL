// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <cassert>
#include <cstddef>
#include <cstdint>
#include <functional>

#pragma warning(disable : 4324) // "was padded due to alignment specifier", thanks, Captain

struct alignas(16) overaligned_t {
    char non_empty;

    void operator()() const {
        assert(static_cast<std::size_t>(reinterpret_cast<std::uintptr_t>(this) % alignof(overaligned_t)) == 0);
    }
};

static_assert(alignof(overaligned_t) == 16, "overaligned_t is not aligned as expected");
static_assert(alignof(overaligned_t) > alignof(std::max_align_t), "overaligned_t is not overaligned");
static_assert(alignof(std::max_align_t) == 8, "max_align_t has changed, the whole stuff should be revised");

struct functions_t {
    using function_t = std::function<void()>;

    function_t first{overaligned_t{}};
    char smallest_pad;
    function_t second{overaligned_t{}};
    function_t third{overaligned_t{}};
};

int main() {
    functions_t functions;
    functions.first();
    functions.second();
    functions.third();
    return 0;
}
