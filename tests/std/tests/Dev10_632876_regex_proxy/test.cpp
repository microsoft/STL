// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <cassert>
#include <cstddef>
#include <cstdlib>
#include <cstring>
#include <new>
#include <regex>

using namespace std;

#pragma warning(disable : 28251) // Inconsistent annotation for 'new': this instance has no annotations.

void* weird_malloc(size_t n) {
    void* p = malloc(n + 128);

    assert(p != nullptr);

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
    regex r("\\w+");

    const char* const s = "cute fluffy kittens";

    cregex_token_iterator i(s, s + strlen(s), r);
    cregex_token_iterator end;

    assert(i != end);
    assert(i->first == s);
    assert(i->second == s + 4);

    ++i;

    assert(i != end);
    assert(i->first == s + 5);
    assert(i->second == s + 11);

    ++i;

    assert(i != end);
    assert(i->first == s + 12);
    assert(i->second == s + 19);

    ++i;

    assert(i == end);
}
