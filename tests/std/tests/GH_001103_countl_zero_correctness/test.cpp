// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <bit>
#include <cassert>
#include <limits>

// Indirectly test countl_zero/countr_zero on old x86/x64 processors by testing a private helper,
// which is different from the usual branch.

using namespace std;

int main() {
    // This test is applicable only to x86 and x64 platforms
#if (defined(_M_IX86) && !defined(_M_HYBRID_X86_ARM64)) || (defined(_M_X64) && !defined(_M_ARM64EC))
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

    assert(_Countr_zero_bsf(static_cast<unsigned char>(0x00)) == 8);
    assert(_Countr_zero_bsf(static_cast<unsigned char>(0x13)) == 0);
    assert(_Countr_zero_bsf(static_cast<unsigned char>(0x82)) == 1);
    assert(_Countr_zero_bsf(static_cast<unsigned char>(0x80)) == 7);
    assert(_Countr_zero_bsf(static_cast<unsigned char>(0xF8)) == 3);

    assert(_Countr_zero_bsf(static_cast<unsigned short>(0x0000)) == 16);
    assert(_Countr_zero_bsf(static_cast<unsigned short>(0x0013)) == 0);
    assert(_Countr_zero_bsf(static_cast<unsigned short>(0x8002)) == 1);
    assert(_Countr_zero_bsf(static_cast<unsigned short>(0x8000)) == 15);
    assert(_Countr_zero_bsf(static_cast<unsigned short>(0xF008)) == 3);

    assert(_Countr_zero_bsf(static_cast<unsigned int>(0x0000'0000)) == 32);
    assert(_Countr_zero_bsf(static_cast<unsigned int>(0x0000'0013)) == 0);
    assert(_Countr_zero_bsf(static_cast<unsigned int>(0x8000'0002)) == 1);
    assert(_Countr_zero_bsf(static_cast<unsigned int>(0x8000'0000)) == 31);
    assert(_Countr_zero_bsf(static_cast<unsigned int>(0xF000'0008)) == 3);

    assert(_Countr_zero_bsf(static_cast<unsigned long>(0x0000'0000)) == 32);
    assert(_Countr_zero_bsf(static_cast<unsigned long>(0x0000'0013)) == 0);
    assert(_Countr_zero_bsf(static_cast<unsigned long>(0x8000'0002)) == 1);
    assert(_Countr_zero_bsf(static_cast<unsigned long>(0x8000'0000)) == 31);
    assert(_Countr_zero_bsf(static_cast<unsigned long>(0xF000'0008)) == 3);

    assert(_Countr_zero_bsf(static_cast<unsigned long long>(0x0000'0000'0000'0000)) == 64);
    assert(_Countr_zero_bsf(static_cast<unsigned long long>(0x0000'0000'0000'0013)) == 0);
    assert(_Countr_zero_bsf(static_cast<unsigned long long>(0x8000'0000'0000'0002)) == 1);
    assert(_Countr_zero_bsf(static_cast<unsigned long long>(0x8000'0000'0000'0000)) == 63);
    assert(_Countr_zero_bsf(static_cast<unsigned long long>(0xF000'0000'0000'0008)) == 3);
#endif // ^^^ (defined(_M_IX86) && !defined(_M_HYBRID_X86_ARM64)) || (defined(_M_X64) && !defined(_M_ARM64EC)) ^^^
}
