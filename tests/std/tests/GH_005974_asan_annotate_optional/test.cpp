// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

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
    opt.emplace();
    ASAN_VERIFY_UNPOISONED(reinterpret_cast<Payload*>(&opt));
}

void test_assignment_unpoisoning() {
    std::optional<Payload> opt = std::nullopt;
    opt                        = Payload{};
    ASAN_VERIFY_UNPOISONED(reinterpret_cast<Payload*>(&opt));
}

void test_repoison_after_reset() {
    std::optional<Payload> opt = Payload{};
    ASAN_VERIFY_UNPOISONED(reinterpret_cast<Payload*>(&opt));
    opt.reset();
    ASAN_VERIFY_POISONED(reinterpret_cast<Payload*>(&opt));
}

constexpr bool test_constexpr() {
#if _HAS_CXX20
    bool res                   = true;
    std::optional<Payload> opt = std::nullopt;
    opt                        = Payload{};
    opt.reset();
    opt = Payload{86, 0, 0, 0};
    res = opt->x == 86;
    opt.emplace(42, 0, 0, 0);
    res = res && (opt->x == 42);
    return res;
#else
    std::optional<Payload> opt{Payload{86, 0, 0, 0}};
    return opt->x == 86;
#endif
}

int main() {
    test_poison_on_empty_access();
    test_emplace_unpoisoning();
    test_assignment_unpoisoning();
    test_repoison_after_reset();
    static_assert(test_constexpr(), "constexpr test failed");

    return 0;
}
