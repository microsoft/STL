// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <cassert>
#include <climits>
#include <cstddef>
#include <iosfwd>
#include <random>
#include <sstream>
#include <string>
#include <type_traits>

#define STATIC_ASSERT(...) static_assert(__VA_ARGS__, #__VA_ARGS__)

using namespace std;

constexpr size_t large_block_size = UINT_MAX - 16;
constexpr size_t large_used_block = UINT_MAX - 20;

struct trivial_engine {
    using result_type = size_t;

    static constexpr size_t min() {
        return 0;
    }
    static constexpr size_t max() {
        return SIZE_MAX;
    }

    size_t operator()() noexcept {
        return counter_++;
    }

#if _HAS_CXX20
    friend bool operator==(const trivial_engine&, const trivial_engine&) = default;
#else // ^^^ _HAS_CXX20 / !_HAS_CXX20 vvv
    friend bool operator==(const trivial_engine& lhs, const trivial_engine& rhs) noexcept {
        return lhs.counter_ == rhs.counter_;
    }
    friend bool operator!=(const trivial_engine& lhs, const trivial_engine& rhs) noexcept {
        return lhs.counter_ != rhs.counter_;
    }
#endif // ^^^ !_HAS_CXX20 ^^^

    template <class CharT, class Traits>
    friend basic_istream<CharT, Traits>& operator>>(basic_istream<CharT, Traits>& is, trivial_engine& eng) {
        return is >> eng.counter_;
    }

    template <class CharT, class Traits>
    friend basic_ostream<CharT, Traits>& operator<<(basic_ostream<CharT, Traits>& os, const trivial_engine& eng) {
        return os << eng.counter_;
    }

    size_t counter_ = 0;
};

using trivial_discard_block = discard_block_engine<trivial_engine, large_block_size, large_used_block>;

void test_lwg_3561() {
    trivial_discard_block e{};
    e.discard(168);
    auto rep = (ostringstream{} << e).str();

    assert(e == e);
    assert(!(e != e));
    assert(rep == "168 168"); // relies on the implementation-specific details of operator<<
}

// Also tests the type correctness of discard_block_engine::block_size/used_block

STATIC_ASSERT(is_same_v<const size_t, decltype(trivial_discard_block::block_size)>);
STATIC_ASSERT(is_same_v<const size_t, decltype(trivial_discard_block::used_block)>);

STATIC_ASSERT(is_same_v<const size_t, decltype(ranlux24::block_size)>);
STATIC_ASSERT(is_same_v<const size_t, decltype(ranlux24::used_block)>);

STATIC_ASSERT(is_same_v<const size_t, decltype(ranlux48::block_size)>);
STATIC_ASSERT(is_same_v<const size_t, decltype(ranlux48::used_block)>);

int main() {
    test_lwg_3561();
}
