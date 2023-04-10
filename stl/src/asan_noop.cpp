// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

extern "C" {
extern const bool _Asan_string_should_annotate_default = false;
extern const bool _Asan_vector_should_annotate_default = false;

void __cdecl __sanitizer_annotate_contiguous_container_default(
    const void*, const void*, const void*, const void*) noexcept {}
}
