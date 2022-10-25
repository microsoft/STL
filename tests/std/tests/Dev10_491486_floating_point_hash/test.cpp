// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <algorithm>
#include <array>
#include <cassert>
#include <cstring>
#include <functional>
#include <type_traits>

using namespace std;

#define STATIC_ASSERT(...) static_assert(__VA_ARGS__, #__VA_ARGS__)

template <class Target, class Source>
[[nodiscard]] Target fake_bit_cast(const Source& source) noexcept {
    STATIC_ASSERT(sizeof(Source) == sizeof(Target));
    Target target;
    memcpy(&target, &source, sizeof(Target));
    return target;
}

template <class Floating>
void test_case() {
    using MatchingUInt   = conditional_t<is_same_v<Floating, float>, unsigned int, unsigned long long>;
    const auto topBitSet = ~(~MatchingUInt{} >> 1);

    const auto pos0d = static_cast<Floating>(0.0);
    assert(fake_bit_cast<MatchingUInt>(pos0d) == 0);
    const auto neg0d = static_cast<Floating>(0.0 * -1.0);
    assert(fake_bit_cast<MatchingUInt>(neg0d) == topBitSet);
#pragma warning(suppress : 6326) // potential comparison of a constant with another constant
    assert(pos0d == neg0d);
    assert(hash<Floating>()(pos0d) == hash<Floating>()(neg0d));

    array<size_t, 15> a{{
        hash<Floating>()(static_cast<Floating>(0.0)),
        hash<Floating>()(static_cast<Floating>(0.1)),
        hash<Floating>()(static_cast<Floating>(0.2)),
        hash<Floating>()(static_cast<Floating>(0.3)),
        hash<Floating>()(static_cast<Floating>(0.4)),
        hash<Floating>()(static_cast<Floating>(0.5)),
        hash<Floating>()(static_cast<Floating>(0.6)),
        hash<Floating>()(static_cast<Floating>(0.7)),
        hash<Floating>()(static_cast<Floating>(0.8)),
        hash<Floating>()(static_cast<Floating>(0.9)),
        hash<Floating>()(static_cast<Floating>(1.0)),
        hash<Floating>()(static_cast<Floating>(1.1)),
        hash<Floating>()(static_cast<Floating>(1.2)),
        hash<Floating>()(static_cast<Floating>(1.3)),
        hash<Floating>()(static_cast<Floating>(1.4)),
    }};

    sort(a.begin(), a.end());
    assert(unique(a.begin(), a.end()) == a.end());
}

int main() {
    test_case<float>();
    test_case<double>();
    test_case<long double>();
}
