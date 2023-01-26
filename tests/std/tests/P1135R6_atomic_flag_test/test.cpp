// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <algorithm>
#include <atomic>
#include <cassert>
#include <cstddef>
#include <execution>
#include <numeric>
#include <vector>

constexpr auto mo = std::memory_order_seq_cst;

template <typename FlagType, typename IsSet, typename TestAndSet, typename Clear>
void test_flags(const IsSet is_set, const TestAndSet test_and_set, const Clear clear) {
    constexpr std::size_t unique      = 800;
    constexpr std::size_t repetitions = 800;
    constexpr std::size_t total       = unique * repetitions;
    constexpr std::size_t dups        = total - unique;

    FlagType flags[unique];
    std::vector<FlagType*> ptrs;
    ptrs.reserve(total);
    for (std::size_t i = 0; i != repetitions; ++i) {
        for (auto& flag : flags) {
            ptrs.push_back(&flag);
        }
    }

    using std::execution::par;

    assert(std::transform_reduce(par, ptrs.begin(), ptrs.end(), 0, std::plus{}, is_set) == 0);
    assert(std::transform_reduce(par, ptrs.begin(), ptrs.end(), 0, std::plus{}, test_and_set) == dups);
    assert(std::transform_reduce(par, ptrs.begin(), ptrs.end(), 0, std::plus{}, is_set) == total);
    assert(std::transform_reduce(par, ptrs.begin(), ptrs.end(), 0, std::plus{}, test_and_set) == total);
    std::for_each(par, ptrs.begin(), ptrs.end(), clear);
    assert(std::transform_reduce(par, ptrs.begin(), ptrs.end(), 0, std::plus{}, is_set) == 0);
}

template <typename FlagType>
void test_flags_members() {
    const auto is_set       = [](const FlagType* f) { return f->test(); };
    const auto test_and_set = [](FlagType* f) { return f->test_and_set(); };
    const auto clear        = [](FlagType* f) { f->clear(); };

    test_flags<FlagType>(is_set, test_and_set, clear);
}

template <typename FlagType>
void test_flags_members_mo() {
    const auto is_set       = [](const FlagType* f) { return f->test(mo); };
    const auto test_and_set = [](FlagType* f) { return f->test_and_set(mo); };
    const auto clear        = [](FlagType* f) { f->clear(mo); };

    test_flags<FlagType>(is_set, test_and_set, clear);
}

template <typename FlagType>
void test_flags_free() {
    const auto is_set       = [](const FlagType* f) { return std::atomic_flag_test(f); };
    const auto test_and_set = [](FlagType* f) { return std::atomic_flag_test_and_set(f); };
    const auto clear        = [](FlagType* f) { std::atomic_flag_clear(f); };

    test_flags<FlagType>(is_set, test_and_set, clear);
}

template <typename FlagType>
void test_flags_free_mo() {
    const auto is_set       = [](const FlagType* f) { return std::atomic_flag_test_explicit(f, mo); };
    const auto test_and_set = [](FlagType* f) { return std::atomic_flag_test_and_set_explicit(f, mo); };
    const auto clear        = [](FlagType* f) { std::atomic_flag_clear_explicit(f, mo); };

    test_flags<FlagType>(is_set, test_and_set, clear);
}

template <typename FlagType>
void test_flag_type() {
    test_flags_members<FlagType>();
    test_flags_free<FlagType>();
    test_flags_members_mo<FlagType>();
    test_flags_free_mo<FlagType>();
}

int main() {
#ifndef _M_CEE // TRANSITION, VSO-1659408
    test_flag_type<std::atomic_flag>();
    test_flag_type<volatile std::atomic_flag>();
#endif // _M_CEE
}
