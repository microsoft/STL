// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#define _SILENCE_CXX17_CODECVT_HEADER_DEPRECATION_WARNING

#include <cassert>
#include <codecvt>
#include <locale>
#include <new>
#include <string>

using namespace std;

#pragma warning(disable : 28251) // Inconsistent annotation for 'new': this instance has no annotations.

void* weird_malloc(size_t n) {
    void* p = malloc(n + 128);

    assert(p);

    unsigned char* q = static_cast<unsigned char*>(p);

    for (size_t i = 0; i < n + 128; ++i) {
        *q = 0xCC;
    }

    return q + 128;
}

void weird_free(void* p) {
    if (p) {
        free(static_cast<unsigned char*>(p) - 128);
    }
}

void* operator new(size_t n) {
    return weird_malloc(n);
}

void* operator new(size_t n, const nothrow_t&) noexcept {
    return weird_malloc(n);
}

void* operator new[](size_t n) {
    return weird_malloc(n);
}

void* operator new[](size_t n, const nothrow_t&) noexcept {
    return weird_malloc(n);
}

void operator delete(void* p) noexcept {
    weird_free(p);
}

void operator delete(void* p, const nothrow_t&) noexcept {
    weird_free(p);
}

void operator delete[](void* p) noexcept {
    weird_free(p);
}

void operator delete[](void* p, const nothrow_t&) noexcept {
    weird_free(p);
}

int main() {
    wstring_convert<codecvt_utf8_utf16<wchar_t>> conv;

    const wstring utf16(L"\xD835\xDF0D");
    const string utf8("\xF0\x9D\x9C\x8D");

    assert(conv.to_bytes(utf16) == utf8);
    assert(conv.from_bytes(utf8) == utf16);
}
