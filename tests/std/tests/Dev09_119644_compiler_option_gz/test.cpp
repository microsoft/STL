// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <__msvc_all_public_headers.hpp>

#ifdef _M_CEE_PURE
int main() {
#else
int __cdecl main() {
#endif
    // Test Dev10-465793 "iostreams: <locale> is incompatible with /Gr and /Gz".
    std::locale loc("english_US");
}
