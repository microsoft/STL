// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <numbers>
#include <type_traits>

#define STATIC_ASSERT(...) static_assert(__VA_ARGS__, #__VA_ARGS__)

template <typename T, typename U>
[[nodiscard]] constexpr bool test_case(T& actual, const U expected) {
    STATIC_ASSERT(std::is_same_v<T, const U>);
    return actual == expected;
}

// N4835 [math.constants]/2: "a program may partially or explicitly specialize a mathematical constant
// variable template provided that the specialization depends on a program-defined type."
struct Meow {
    int val;
};

template <>
inline constexpr Meow std::numbers::e_v<Meow>{-10};
template <>
inline constexpr Meow std::numbers::log2e_v<Meow>{-20};
template <>
inline constexpr Meow std::numbers::log10e_v<Meow>{-30};
template <>
inline constexpr Meow std::numbers::pi_v<Meow>{-40};
template <>
inline constexpr Meow std::numbers::inv_pi_v<Meow>{-50};
template <>
inline constexpr Meow std::numbers::inv_sqrtpi_v<Meow>{-60};
template <>
inline constexpr Meow std::numbers::ln2_v<Meow>{-70};
template <>
inline constexpr Meow std::numbers::ln10_v<Meow>{-80};
template <>
inline constexpr Meow std::numbers::sqrt2_v<Meow>{-90};
template <>
inline constexpr Meow std::numbers::sqrt3_v<Meow>{-100};
template <>
inline constexpr Meow std::numbers::inv_sqrt3_v<Meow>{-110};
template <>
inline constexpr Meow std::numbers::egamma_v<Meow>{-120};
template <>
inline constexpr Meow std::numbers::phi_v<Meow>{-130};

int main() {
    STATIC_ASSERT(test_case(std::numbers::e_v<Meow>.val, -10));
    STATIC_ASSERT(test_case(std::numbers::log2e_v<Meow>.val, -20));
    STATIC_ASSERT(test_case(std::numbers::log10e_v<Meow>.val, -30));
    STATIC_ASSERT(test_case(std::numbers::pi_v<Meow>.val, -40));
    STATIC_ASSERT(test_case(std::numbers::inv_pi_v<Meow>.val, -50));
    STATIC_ASSERT(test_case(std::numbers::inv_sqrtpi_v<Meow>.val, -60));
    STATIC_ASSERT(test_case(std::numbers::ln2_v<Meow>.val, -70));
    STATIC_ASSERT(test_case(std::numbers::ln10_v<Meow>.val, -80));
    STATIC_ASSERT(test_case(std::numbers::sqrt2_v<Meow>.val, -90));
    STATIC_ASSERT(test_case(std::numbers::sqrt3_v<Meow>.val, -100));
    STATIC_ASSERT(test_case(std::numbers::inv_sqrt3_v<Meow>.val, -110));
    STATIC_ASSERT(test_case(std::numbers::egamma_v<Meow>.val, -120));
    STATIC_ASSERT(test_case(std::numbers::phi_v<Meow>.val, -130));

    using F = float;
    using D = double;
    using L = long double;

    STATIC_ASSERT(test_case(std::numbers::e_v<F>, 0x1.5bf0a8p+1f));
    STATIC_ASSERT(test_case(std::numbers::e_v<D>, 0x1.5bf0a8b145769p+1));
    STATIC_ASSERT(test_case(std::numbers::e_v<L>, 0x1.5bf0a8b145769p+1L));
    STATIC_ASSERT(test_case(std::numbers::e /**/, 0x1.5bf0a8b145769p+1));

    STATIC_ASSERT(test_case(std::numbers::log2e_v<F>, 0x1.715476p+0f));
    STATIC_ASSERT(test_case(std::numbers::log2e_v<D>, 0x1.71547652b82fep+0));
    STATIC_ASSERT(test_case(std::numbers::log2e_v<L>, 0x1.71547652b82fep+0L));
    STATIC_ASSERT(test_case(std::numbers::log2e /**/, 0x1.71547652b82fep+0));

    STATIC_ASSERT(test_case(std::numbers::log10e_v<F>, 0x1.bcb7b2p-2f));
    STATIC_ASSERT(test_case(std::numbers::log10e_v<D>, 0x1.bcb7b1526e50ep-2));
    STATIC_ASSERT(test_case(std::numbers::log10e_v<L>, 0x1.bcb7b1526e50ep-2L));
    STATIC_ASSERT(test_case(std::numbers::log10e /**/, 0x1.bcb7b1526e50ep-2));

    STATIC_ASSERT(test_case(std::numbers::pi_v<F>, 0x1.921fb6p+1f));
    STATIC_ASSERT(test_case(std::numbers::pi_v<D>, 0x1.921fb54442d18p+1));
    STATIC_ASSERT(test_case(std::numbers::pi_v<L>, 0x1.921fb54442d18p+1L));
    STATIC_ASSERT(test_case(std::numbers::pi /**/, 0x1.921fb54442d18p+1));

    STATIC_ASSERT(test_case(std::numbers::inv_pi_v<F>, 0x1.45f306p-2f));
    STATIC_ASSERT(test_case(std::numbers::inv_pi_v<D>, 0x1.45f306dc9c883p-2));
    STATIC_ASSERT(test_case(std::numbers::inv_pi_v<L>, 0x1.45f306dc9c883p-2L));
    STATIC_ASSERT(test_case(std::numbers::inv_pi /**/, 0x1.45f306dc9c883p-2));

    STATIC_ASSERT(test_case(std::numbers::inv_sqrtpi_v<F>, 0x1.20dd76p-1f));
    STATIC_ASSERT(test_case(std::numbers::inv_sqrtpi_v<D>, 0x1.20dd750429b6dp-1));
    STATIC_ASSERT(test_case(std::numbers::inv_sqrtpi_v<L>, 0x1.20dd750429b6dp-1L));
    STATIC_ASSERT(test_case(std::numbers::inv_sqrtpi /**/, 0x1.20dd750429b6dp-1));

    STATIC_ASSERT(test_case(std::numbers::ln2_v<F>, 0x1.62e430p-1f));
    STATIC_ASSERT(test_case(std::numbers::ln2_v<D>, 0x1.62e42fefa39efp-1));
    STATIC_ASSERT(test_case(std::numbers::ln2_v<L>, 0x1.62e42fefa39efp-1L));
    STATIC_ASSERT(test_case(std::numbers::ln2 /**/, 0x1.62e42fefa39efp-1));

    STATIC_ASSERT(test_case(std::numbers::ln10_v<F>, 0x1.26bb1cp+1f));
    STATIC_ASSERT(test_case(std::numbers::ln10_v<D>, 0x1.26bb1bbb55516p+1));
    STATIC_ASSERT(test_case(std::numbers::ln10_v<L>, 0x1.26bb1bbb55516p+1L));
    STATIC_ASSERT(test_case(std::numbers::ln10 /**/, 0x1.26bb1bbb55516p+1));

    STATIC_ASSERT(test_case(std::numbers::sqrt2_v<F>, 0x1.6a09e6p+0f));
    STATIC_ASSERT(test_case(std::numbers::sqrt2_v<D>, 0x1.6a09e667f3bcdp+0));
    STATIC_ASSERT(test_case(std::numbers::sqrt2_v<L>, 0x1.6a09e667f3bcdp+0L));
    STATIC_ASSERT(test_case(std::numbers::sqrt2 /**/, 0x1.6a09e667f3bcdp+0));

    STATIC_ASSERT(test_case(std::numbers::sqrt3_v<F>, 0x1.bb67aep+0f));
    STATIC_ASSERT(test_case(std::numbers::sqrt3_v<D>, 0x1.bb67ae8584caap+0));
    STATIC_ASSERT(test_case(std::numbers::sqrt3_v<L>, 0x1.bb67ae8584caap+0L));
    STATIC_ASSERT(test_case(std::numbers::sqrt3 /**/, 0x1.bb67ae8584caap+0));

    STATIC_ASSERT(test_case(std::numbers::inv_sqrt3_v<F>, 0x1.279a74p-1f));
    STATIC_ASSERT(test_case(std::numbers::inv_sqrt3_v<D>, 0x1.279a74590331cp-1));
    STATIC_ASSERT(test_case(std::numbers::inv_sqrt3_v<L>, 0x1.279a74590331cp-1L));
    STATIC_ASSERT(test_case(std::numbers::inv_sqrt3 /**/, 0x1.279a74590331cp-1));

    STATIC_ASSERT(test_case(std::numbers::egamma_v<F>, 0x1.2788d0p-1f));
    STATIC_ASSERT(test_case(std::numbers::egamma_v<D>, 0x1.2788cfc6fb619p-1));
    STATIC_ASSERT(test_case(std::numbers::egamma_v<L>, 0x1.2788cfc6fb619p-1L));
    STATIC_ASSERT(test_case(std::numbers::egamma /**/, 0x1.2788cfc6fb619p-1));

    STATIC_ASSERT(test_case(std::numbers::phi_v<F>, 0x1.9e377ap+0f));
    STATIC_ASSERT(test_case(std::numbers::phi_v<D>, 0x1.9e3779b97f4a8p+0));
    STATIC_ASSERT(test_case(std::numbers::phi_v<L>, 0x1.9e3779b97f4a8p+0L));
    STATIC_ASSERT(test_case(std::numbers::phi /**/, 0x1.9e3779b97f4a8p+0));
}
