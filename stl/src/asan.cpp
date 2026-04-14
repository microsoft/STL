// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

extern "C" {
// TRANSITION, ABI: preserved for compatibility with old headers, which
// added stl_asan.lib to the link line. We use __declspec(selectany) to be
// compatible with the new headers that define these variables as
// __declspec(selectany) in __msvc_sanitizer_annotate_container.hpp.
// The new method is preferred because previously enabling just string
// would also enable vector and optional. See GH-6186 for details.
__declspec(selectany) extern const bool _Asan_string_should_annotate   = true;
__declspec(selectany) extern const bool _Asan_vector_should_annotate   = true;
__declspec(selectany) extern const bool _Asan_optional_should_annotate = true;
} // extern "C"
