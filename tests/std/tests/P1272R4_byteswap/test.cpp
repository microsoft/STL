// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <bit>
#include <cassert>

using namespace std;

template <typename T>
constexpr bool test_byteswap(const auto src, const auto result) {
    static_assert(noexcept(byteswap(T{})));
    return byteswap(static_cast<T>(src)) == static_cast<T>(result);
}

constexpr bool test_byteswap_all_types() {
    assert(test_byteswap<bool>(true, true));

    assert(test_byteswap<char>(0x13, 0x13));
    assert(test_byteswap<signed char>(0x13, 0x13));
    assert(test_byteswap<unsigned char>(0x13, 0x13));
#ifdef __cpp_char8_t
    assert(test_byteswap<char8_t>(0x13, 0x13));
#endif

    assert(test_byteswap<short>(0xAC34, 0x34AC));
    assert(test_byteswap<unsigned short>(0xAC34, 0x34AC));
    assert(test_byteswap<char16_t>(0xAC34, 0x34AC));
    assert(test_byteswap<wchar_t>(0xAC34, 0x34AC));

    assert(test_byteswap<int>(0x1234ABCD, 0xCDAB3412));
    assert(test_byteswap<unsigned int>(0x1234ABCD, 0xCDAB3412));
    assert(test_byteswap<long>(0x1234ABCD, 0xCDAB3412));
    assert(test_byteswap<unsigned long>(0x1234ABCD, 0xCDAB3412));
    assert(test_byteswap<char32_t>(0x1234ABCD, 0xCDAB3412));

    assert(test_byteswap<long long>(0x1234567890ABCDEF, 0xEFCDAB9078563412));
    assert(test_byteswap<unsigned long long>(0x1234567890ABCDEF, 0xEFCDAB9078563412));

    return true;
}

static_assert(test_byteswap_all_types());

int main() {
    test_byteswap_all_types();
}
