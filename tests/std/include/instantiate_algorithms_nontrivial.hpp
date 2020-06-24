// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#pragma once
// This header contains all code for the instantiate_algorithms_nontrivial* test cases.
// The instantiate_algorithm* tests take too long individually, so must be split into two parts.

#include <random>
#include <type_traits>

#include <input_iterator.hpp>
#include <instantiate_algorithms.hpp>

#define STATIC_ASSERT(...) static_assert(__VA_ARGS__, #__VA_ARGS__)

// This is a type whose constructors and destructor are all non-trivial,
// since we support some optimizations for trivial types.
struct NonTrivialType {
    NonTrivialType() : m_data(get_random()) {}
    ~NonTrivialType() {
        m_data = get_random();
    }
    NonTrivialType(const NonTrivialType&) : m_data(get_random()) {}

    NonTrivialType& operator=(const NonTrivialType&) {
        m_data = get_random();
        return *this;
    }

    bool operator<(const NonTrivialType&) const {
        return false;
    }
    bool operator==(const NonTrivialType&) const {
        return true;
    }

    NonTrivialType operator+(const NonTrivialType& v) const {
        return v;
    }
    NonTrivialType operator-(const NonTrivialType& v) const {
        return v;
    }
    NonTrivialType operator*(const NonTrivialType& v) const {
        return v;
    }

    NonTrivialType& operator++() {
        return *this;
    }

    unsigned int get_random() {
        std::random_device get_rand{};
        return get_rand();
    }

    unsigned int m_data;
};

STATIC_ASSERT(!std::is_trivially_default_constructible_v<NonTrivialType>);
STATIC_ASSERT(!std::is_trivially_copy_constructible_v<NonTrivialType>);
STATIC_ASSERT(!std::is_trivially_move_constructible_v<NonTrivialType>);
STATIC_ASSERT(!std::is_trivially_destructible_v<NonTrivialType>);
STATIC_ASSERT(!std::is_trivially_copy_assignable_v<NonTrivialType>);
STATIC_ASSERT(!std::is_trivially_move_assignable_v<NonTrivialType>);

void test() {
    std_testing::input_iterator_container<NonTrivialType> input_it{};
    std::forward_list<NonTrivialType> fwd_it{};
    std::list<NonTrivialType> bidi_it{};
    std::vector<NonTrivialType> rand_it{};

    std_testing::instantiate_std_algorithms_with<NonTrivialType>(
        input_it.begin(), fwd_it.begin(), bidi_it.begin(), rand_it.begin(), std::front_inserter(fwd_it));
}

#undef STATIC_ASSERT
