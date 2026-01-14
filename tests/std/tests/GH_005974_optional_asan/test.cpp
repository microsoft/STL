// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <__msvc_sanitizer_annotate_container.hpp>
#include <cassert>
#include <optional>

#ifdef __SANITIZE_ADDRESS__
extern "C" int __cdecl __asan_address_is_poisoned(void const volatile* addr);
#define ASAN_VERIFY_POISONED(addr)   assert(__asan_address_is_poisoned((addr)) != 0)
#define ASAN_VERIFY_UNPOISONED(addr) assert(__asan_address_is_poisoned((addr)) == 0)
#else
#define ASAN_VERIFY_POISONED(addr)   ((void) (addr))
#define ASAN_VERIFY_UNPOISONED(addr) ((void) (addr))
#endif

struct Payload {
    long long x;
    long long y;
    long long z;
    long long w;
};

void test_poison_on_empty_access() {
    [[maybe_unused]] std::optional<Payload> opt;
    ASAN_VERIFY_POISONED(reinterpret_cast<Payload*>(&opt));
}

void test_emplace_unpoisoning() {
    std::optional<Payload> opt;
    opt.emplace(Payload());
    ASAN_VERIFY_UNPOISONED(reinterpret_cast<Payload*>(&opt));
}

void test_assignment_unpoisoning() {
    std::optional<Payload> opt = std::nullopt;
    opt                        = Payload();
    ASAN_VERIFY_UNPOISONED(reinterpret_cast<Payload*>(&opt));
}

void test_repoison_after_reset() {
    std::optional<Payload> opt = Payload();
    ASAN_VERIFY_UNPOISONED(reinterpret_cast<Payload*>(&opt));
    opt.reset();
    ASAN_VERIFY_POISONED(reinterpret_cast<Payload*>(&opt));
}

int main() {
    test_poison_on_empty_access();
    test_emplace_unpoisoning();
    test_assignment_unpoisoning();
    test_repoison_after_reset();

    return 0;
}
