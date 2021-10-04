// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <bit>
#include <cassert>

using namespace std;

int main() {
    assert(byteswap('\x13') == '\x13');
    assert(byteswap(static_cast<signed char>('\x13')) == static_cast<signed char>('\x13'));
    assert(byteswap(static_cast<unsigned char>('\x13')) == static_cast<unsigned char>('\x13'));
    assert(byteswap(static_cast<short>(0xAC34)) == static_cast<short>(0x34AC));
    assert(byteswap(static_cast<unsigned short>(0xAC34)) == static_cast<unsigned short>(0x34AC));
    assert(byteswap(static_cast<long>(0x1234ABCD)) == static_cast<long>(0xCDAB3412));
    assert(byteswap(static_cast<unsigned long>(0x1234ABCD)) == static_cast<unsigned long>(0xCDAB3412));
    assert(byteswap(static_cast<long long>(0x1234567890ABCDEF)) == static_cast<long long>(0xEFCDAB9078563412));
    assert(byteswap(static_cast<unsigned long long>(0x1234567890ABCDEF))
           == static_cast<unsigned long long>(0xEFCDAB9078563412));
}
