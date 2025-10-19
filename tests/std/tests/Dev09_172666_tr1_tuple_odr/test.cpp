// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#define _SILENCE_EXPERIMENTAL_FILESYSTEM_DEPRECATION_WARNING

#include <__msvc_all_public_headers.hpp>
#include <cassert>

#include "experimental_filesystem.hpp"

int meow();

int main() {
    assert(meow() == 1729);

    // Test for DevDiv-1004799: <filesystem>: /Zc:wchar_t- fails.
    // Calling file_size should cause the failure to occur if we are wchar_t incorrect.
#if defined(_M_CEE_PURE) && !defined(_NATIVE_WCHAR_T_DEFINED)
    // Test is disabled if compiled with /clr:pure and /Zc:wchar_t-, as it triggers
    // LNK2031: calling convention missing in metadata errors, which are irrelevant here.
#else // ^^^ workaround / no workaround vvv
    {
        namespace fs = std::experimental::filesystem;
        assert(fs::file_size(fs::current_path() / "Dev09_172666_tr1_tuple_odr.exe") != 0u);
    }

#if _HAS_CXX17
    assert(std::filesystem::file_size(std::filesystem::current_path() / "Dev09_172666_tr1_tuple_odr.exe") != 0u);
#endif // _HAS_CXX17
#endif // ^^^ no workaround ^^^
}
