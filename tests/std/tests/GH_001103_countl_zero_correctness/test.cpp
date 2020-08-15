// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <bit>
#include <cassert>

// Indirectly test countl_zero on old x86/x64 processors by testing a private helper,
// which is different from the usual branch.

// Since the fallback intrinsic is available on ARM too, don't need to exclude ARM

// Currently need this test only in C++20 mode;
// may update to older C++ if the helper is used internally too, for example in <bitset>.

using namespace std;

int main() {
#ifdef _CHAR_UNSIGNED
    assert(_Countl_zero_bsr(static_cast<char>(0x00)) == 8);
    assert(_Countl_zero_bsr(static_cast<char>(0x13)) == 3);
    assert(_Countl_zero_bsr(static_cast<char>(0x83)) == 0);
    assert(_Countl_zero_bsr(static_cast<char>(0xF8)) == 0);
#endif

    assert(_Countl_zero_bsr(static_cast<unsigned char>(0x00)) == 8);
    assert(_Countl_zero_bsr(static_cast<unsigned char>(0x13)) == 3);
    assert(_Countl_zero_bsr(static_cast<unsigned char>(0x83)) == 0);
    assert(_Countl_zero_bsr(static_cast<unsigned char>(0xF8)) == 0);

    assert(_Countl_zero_bsr(static_cast<unsigned short>(0x0000)) == 16);
    assert(_Countl_zero_bsr(static_cast<unsigned short>(0x0013)) == 11);
    assert(_Countl_zero_bsr(static_cast<unsigned short>(0x8003)) == 0);
    assert(_Countl_zero_bsr(static_cast<unsigned short>(0xF008)) == 0);

    assert(_Countl_zero_bsr(static_cast<unsigned int>(0x0000'0000)) == 32);
    assert(_Countl_zero_bsr(static_cast<unsigned int>(0x0000'0013)) == 27);
    assert(_Countl_zero_bsr(static_cast<unsigned int>(0x8000'0003)) == 0);
    assert(_Countl_zero_bsr(static_cast<unsigned int>(0xF000'0008)) == 0);

    assert(_Countl_zero_bsr(static_cast<unsigned long>(0x0000'0000)) == 32);
    assert(_Countl_zero_bsr(static_cast<unsigned long>(0x0000'0013)) == 27);
    assert(_Countl_zero_bsr(static_cast<unsigned long>(0x8000'0003)) == 0);
    assert(_Countl_zero_bsr(static_cast<unsigned long>(0xF000'0008)) == 0);

    assert(_Countl_zero_bsr(static_cast<unsigned long long>(0x0000'0000'0000'0000)) == 64);
    assert(_Countl_zero_bsr(static_cast<unsigned long long>(0x0000'0000'0000'0013)) == 59);
    assert(_Countl_zero_bsr(static_cast<unsigned long long>(0x8000'0000'0000'0003)) == 0);
    assert(_Countl_zero_bsr(static_cast<unsigned long long>(0xF000'0000'0000'0008)) == 0);

    return 0;
}
