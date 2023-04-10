// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <__msvc_all_public_headers.hpp>
#include <cassert>

namespace fs = std::experimental::filesystem;

int meow();

inline bool test_wchar_t_minus() {
    // Test for DevDiv-1004799: <filesystem>: /Zc:wchar_t- fails. Calling file_size
    // should cause the failure to occur if we are wchar_t incorrect. Test is disabled
    // (i.e. always passes) if compiled with /clr:pure and /Zc:wchar_t-, as it triggers
    // LNK2031: calling convention missing in metadata errors, which are irrelevant here.
#if defined(_M_CEE_PURE) && !defined(_NATIVE_WCHAR_T_DEFINED)
    return true;
#else // ^^^ /clr:pure /Zc:wchar_t- / Other vvv
    return fs::file_size(fs::current_path() / "Dev09_172666_tr1_tuple_odr.exe") != 0u;
#endif // defined(_M_CEE_PURE) && !defined(_NATIVE_WCHAR_T_DEFINED)
}

int main() {
    assert(meow() == 1729);
    assert(test_wchar_t_minus());
}
