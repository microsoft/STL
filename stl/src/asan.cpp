// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

namespace std {
    extern "C" {
    extern const bool _Asan_string_should_annotate = true;
    extern const bool _Asan_vector_should_annotate = true;
    }
} // namespace std
