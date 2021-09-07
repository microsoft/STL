// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

namespace std {
    extern "C" {
    extern const bool _Asan_vector_should_annotate_default = false;

    void __sanitizer_annotate_contiguous_container_default(
        const void*, const void*, const void*, const void*) noexcept {}
    }
} // namespace std
