// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

//
// Tests the new functions added as part of P0586R2, "Integer Comparison Functions"
//

#include <cassert>
#include <cstdint>
#include <limits>
#include <utility>
using namespace std;

template <typename T, typename U>
constexpr void test_less(const T t, const U u) {
    assert(!cmp_equal(t, u));
    assert(!cmp_equal(u, t));
    assert(cmp_not_equal(t, u));
    assert(cmp_not_equal(u, t));
    assert(cmp_less(t, u));
    assert(!cmp_less(u, t));
    assert(cmp_less_equal(t, u));
    assert(!cmp_less_equal(u, t));
    assert(cmp_greater(u, t));
    assert(!cmp_greater(t, u));
    assert(cmp_greater_equal(u, t));
    assert(!cmp_greater_equal(t, u));
}

template <typename T, typename U>
constexpr void test_equal(const T t, const U u) {
    assert(cmp_equal(t, u));
    assert(cmp_equal(u, t));
    assert(!cmp_not_equal(t, u));
    assert(!cmp_not_equal(u, t));
    assert(!cmp_less(t, u));
    assert(!cmp_less(u, t));
    assert(cmp_less_equal(t, u));
    assert(cmp_less_equal(u, t));
    assert(!cmp_greater(u, t));
    assert(!cmp_greater(t, u));
    assert(cmp_greater_equal(u, t));
    assert(cmp_greater_equal(t, u));
}

template <typename Other, typename T>
constexpr void test_less_with(const T x, const T y) {
    test_less(x, y);
    test_less(x, static_cast<Other>(y));
    test_less(static_cast<Other>(x), y);
    test_less(static_cast<Other>(x), static_cast<Other>(y));
}

template <typename Other, typename T>
constexpr void test_equal_with(const T x, const T y) {
    test_equal(x, y);
    test_equal(x, static_cast<Other>(y));
    test_equal(static_cast<Other>(x), y);
    test_equal(static_cast<Other>(x), static_cast<Other>(y));
}

constexpr bool test() {
    test_equal_with<long>(-1, -1);
    test_equal_with<long>(0, 0);
    test_equal_with<long>(1, 1);
    test_less_with<long>(-42, -1);
    test_less_with<long>(-1, 0);
    test_less_with<long>(-1, 1);
    test_less_with<long>(0, 1);
    test_less_with<long>(1, 42);

    test_equal_with<short>(-1, -1);
    test_equal_with<short>(0, 0);
    test_equal_with<short>(1, 1);
    test_less_with<short>(-42, -1);
    test_less_with<short>(-1, 0);
    test_less_with<short>(-1, 1);
    test_less_with<short>(0, 1);
    test_less_with<short>(1, 42);

    test_equal_with<unsigned int>(0, 0);
    test_equal_with<unsigned int>(7, 7);
    test_less_with<unsigned int>(0, 42);
    test_less_with<unsigned int>(7, 42);

    test_equal_with<unsigned short>(0, 0);
    test_equal_with<unsigned short>(7, 7);
    test_less_with<unsigned short>(0, 42);
    test_less_with<unsigned short>(7, 42);

    test_less(-1, 0u);
    test_less(-1, 1u);

    test_less(int8_t{-1}, static_cast<uint8_t>(-1));
    test_less(int16_t{-1}, static_cast<uint16_t>(-1));
    test_less(int32_t{-1}, static_cast<uint32_t>(-1));
    test_less(int64_t{-1}, static_cast<uint64_t>(-1));

    test_less(int64_t{-1000}, int8_t{5});
    test_less(int64_t{-129}, int8_t{5});
    test_less(int64_t{-128}, int8_t{5});
    test_less(int64_t{-127}, int8_t{5});
    test_less(int64_t{-5}, int8_t{5});
    test_less(int64_t{-1}, int8_t{5});
    test_less(int64_t{0}, int8_t{5});
    test_less(int64_t{1}, int8_t{5});
    test_less(int64_t{4}, int8_t{5});
    test_equal(int8_t{5}, int64_t{5});
    test_less(int8_t{5}, int64_t{6});
    test_less(int8_t{5}, int64_t{126});
    test_less(int8_t{5}, int64_t{127});
    test_less(int8_t{5}, int64_t{128});
    test_less(int8_t{5}, int64_t{1000});

    test_less(int8_t{-128}, uint8_t{5});
    test_less(int8_t{-128}, uint16_t{5});
    test_less(int8_t{-128}, uint32_t{5});
    test_less(int8_t{-128}, uint64_t{5});

    test_less(int8_t{-128}, uint8_t{255});
    test_less(int8_t{-128}, uint16_t{5000});
    test_less(int8_t{-128}, uint32_t{5000});
    test_less(int8_t{-128}, uint64_t{5000});

    test_less(uint8_t{126}, int8_t{127});
    test_less(uint16_t{126}, int8_t{127});
    test_less(uint32_t{126}, int8_t{127});
    test_less(uint64_t{126}, int8_t{127});

    test_equal(int8_t{127}, uint8_t{127});
    test_equal(int8_t{127}, uint16_t{127});
    test_equal(int8_t{127}, uint32_t{127});
    test_equal(int8_t{127}, uint64_t{127});

    test_less(int8_t{127}, uint8_t{128});
    test_less(int8_t{127}, uint16_t{128});
    test_less(int8_t{127}, uint32_t{128});
    test_less(int8_t{127}, uint64_t{128});

    test_less(uint8_t{100}, uint64_t{1000});
    test_less(uint64_t{10}, uint8_t{100});

    assert(in_range<int>(1));
    assert(in_range<int>(1L));
    assert(!in_range<unsigned int>(-1));
    assert(in_range<unsigned int>(0));
    assert(in_range<unsigned int>(42));
    assert(!in_range<int>(numeric_limits<long long>::min()));
    assert(!in_range<int>(numeric_limits<long long>::max()));
    assert(in_range<int>(numeric_limits<int>::min()));
    assert(in_range<int>(numeric_limits<int>::max()));
    assert(in_range<long>(numeric_limits<int>::min()));
    assert(in_range<long>(numeric_limits<int>::max()));

    assert(in_range<int16_t>(int8_t{-128}));
    assert(in_range<int16_t>(int8_t{127}));

    assert(in_range<int16_t>(int16_t{-32768}));
    assert(in_range<int16_t>(int16_t{32767}));

    assert(!in_range<int16_t>(int32_t{-32769}));
    assert(in_range<int16_t>(int32_t{-32768}));
    assert(in_range<int16_t>(int32_t{32767}));
    assert(!in_range<int16_t>(int32_t{32768}));

    assert(in_range<int16_t>(uint8_t{0}));
    assert(in_range<int16_t>(uint8_t{255}));

    assert(in_range<int16_t>(uint16_t{0}));
    assert(in_range<int16_t>(uint16_t{32767}));
    assert(!in_range<int16_t>(uint16_t{32768}));

    assert(in_range<int16_t>(uint32_t{0}));
    assert(in_range<int16_t>(uint32_t{32767}));
    assert(!in_range<int16_t>(uint32_t{32768}));

    assert(!in_range<uint16_t>(int8_t{-1}));
    assert(in_range<uint16_t>(int8_t{0}));
    assert(in_range<uint16_t>(int8_t{127}));

    assert(!in_range<uint16_t>(int16_t{-1}));
    assert(in_range<uint16_t>(int16_t{0}));
    assert(in_range<uint16_t>(int16_t{32767}));

    assert(!in_range<uint16_t>(int32_t{-1}));
    assert(in_range<uint16_t>(int32_t{0}));
    assert(in_range<uint16_t>(int32_t{65535}));
    assert(!in_range<uint16_t>(int32_t{65536}));

    assert(in_range<uint16_t>(uint8_t{0}));
    assert(in_range<uint16_t>(uint8_t{255}));

    assert(in_range<uint16_t>(uint16_t{0}));
    assert(in_range<uint16_t>(uint16_t{65535}));

    assert(in_range<uint16_t>(uint32_t{0}));
    assert(in_range<uint16_t>(uint32_t{65535}));
    assert(!in_range<uint16_t>(uint32_t{65536}));

    return true;
}

int main() {
    assert(test());
    static_assert(test());
}
