// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <__msvc_all_public_headers.hpp>
#include <assert.h>

namespace fs = std::experimental::filesystem;

int meow();

inline bool test_wchar_t_minus() {
    // Test for DevDiv Bug#1004799: <filesystem>: /Zc:wchar_t- explodes. Calling file_size
    // should cause the blow-up to occur if we are wchar_t incorrect. Test is disabled
    // (i.e. always passes) if compiled with /clr:pure and /Zc:wchar_t-, as it triggers
    // LNK2031: calling convention missing in metadata errors, which are irrelevant here.
#if defined(_M_CEE_PURE) && !defined(_NATIVE_WCHAR_T_DEFINED)
    return true;
#else
    return fs::file_size(fs::current_path() / "two.cpp") != 0u;
#endif
}

int main() {
    assert(meow() == 1729 && test_wchar_t_minus());
}
