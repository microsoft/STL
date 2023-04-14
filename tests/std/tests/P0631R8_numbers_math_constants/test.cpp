// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <cassert>
#include <numbers>
#include <type_traits>

#pragma warning(disable : 4197) // '%s': top-level volatile in cast is ignored

#define STATIC_ASSERT(...) static_assert(__VA_ARGS__, #__VA_ARGS__)

template <class T, class U>
[[nodiscard]] constexpr bool test_case(T& actual, const U expected) {
    STATIC_ASSERT(std::is_same_v<const std::remove_cv_t<T>, const U>);
    return actual == expected;
}

enum class modify_cv {
    type_identity,
    add_const,
    add_volatile,
    add_cv,
};

template <modify_cv Modification, class T>
struct apply_modify_cv {
    using type = T;
};

template <class T>
struct apply_modify_cv<modify_cv::add_const, T> {
    using type = const T;
};

template <class T>
struct apply_modify_cv<modify_cv::add_volatile, T> {
    using type = volatile T;
};

template <class T>
struct apply_modify_cv<modify_cv::add_cv, T> {
    using type = const volatile T;
};

template <modify_cv Modification, class T>
using apply_modify_cv_t = typename apply_modify_cv<Modification, T>::type;

template <modify_cv Modification>
constexpr void test_cv_floating_point() {
    using F = apply_modify_cv_t<Modification, float>;
    using D = apply_modify_cv_t<Modification, double>;
    using L = apply_modify_cv_t<Modification, long double>;

    assert(test_case(std::numbers::e_v<F>, 0x1.5bf0a8p+1f));
    assert(test_case(std::numbers::e_v<D>, 0x1.5bf0a8b145769p+1));
    assert(test_case(std::numbers::e_v<L>, 0x1.5bf0a8b145769p+1L));

    assert(test_case(std::numbers::log2e_v<F>, 0x1.715476p+0f));
    assert(test_case(std::numbers::log2e_v<D>, 0x1.71547652b82fep+0));
    assert(test_case(std::numbers::log2e_v<L>, 0x1.71547652b82fep+0L));

    assert(test_case(std::numbers::log10e_v<F>, 0x1.bcb7b2p-2f));
    assert(test_case(std::numbers::log10e_v<D>, 0x1.bcb7b1526e50ep-2));
    assert(test_case(std::numbers::log10e_v<L>, 0x1.bcb7b1526e50ep-2L));

    assert(test_case(std::numbers::pi_v<F>, 0x1.921fb6p+1f));
    assert(test_case(std::numbers::pi_v<D>, 0x1.921fb54442d18p+1));
    assert(test_case(std::numbers::pi_v<L>, 0x1.921fb54442d18p+1L));

    assert(test_case(std::numbers::inv_pi_v<F>, 0x1.45f306p-2f));
    assert(test_case(std::numbers::inv_pi_v<D>, 0x1.45f306dc9c883p-2));
    assert(test_case(std::numbers::inv_pi_v<L>, 0x1.45f306dc9c883p-2L));

    assert(test_case(std::numbers::inv_sqrtpi_v<F>, 0x1.20dd76p-1f));
    assert(test_case(std::numbers::inv_sqrtpi_v<D>, 0x1.20dd750429b6dp-1));
    assert(test_case(std::numbers::inv_sqrtpi_v<L>, 0x1.20dd750429b6dp-1L));

    assert(test_case(std::numbers::ln2_v<F>, 0x1.62e430p-1f));
    assert(test_case(std::numbers::ln2_v<D>, 0x1.62e42fefa39efp-1));
    assert(test_case(std::numbers::ln2_v<L>, 0x1.62e42fefa39efp-1L));

    assert(test_case(std::numbers::ln10_v<F>, 0x1.26bb1cp+1f));
    assert(test_case(std::numbers::ln10_v<D>, 0x1.26bb1bbb55516p+1));
    assert(test_case(std::numbers::ln10_v<L>, 0x1.26bb1bbb55516p+1L));

    assert(test_case(std::numbers::sqrt2_v<F>, 0x1.6a09e6p+0f));
    assert(test_case(std::numbers::sqrt2_v<D>, 0x1.6a09e667f3bcdp+0));
    assert(test_case(std::numbers::sqrt2_v<L>, 0x1.6a09e667f3bcdp+0L));

    assert(test_case(std::numbers::sqrt3_v<F>, 0x1.bb67aep+0f));
    assert(test_case(std::numbers::sqrt3_v<D>, 0x1.bb67ae8584caap+0));
    assert(test_case(std::numbers::sqrt3_v<L>, 0x1.bb67ae8584caap+0L));

    assert(test_case(std::numbers::inv_sqrt3_v<F>, 0x1.279a74p-1f));
    assert(test_case(std::numbers::inv_sqrt3_v<D>, 0x1.279a74590331cp-1));
    assert(test_case(std::numbers::inv_sqrt3_v<L>, 0x1.279a74590331cp-1L));

    assert(test_case(std::numbers::egamma_v<F>, 0x1.2788d0p-1f));
    assert(test_case(std::numbers::egamma_v<D>, 0x1.2788cfc6fb619p-1));
    assert(test_case(std::numbers::egamma_v<L>, 0x1.2788cfc6fb619p-1L));

    assert(test_case(std::numbers::phi_v<F>, 0x1.9e377ap+0f));
    assert(test_case(std::numbers::phi_v<D>, 0x1.9e3779b97f4a8p+0));
    assert(test_case(std::numbers::phi_v<L>, 0x1.9e3779b97f4a8p+0L));
}

constexpr void test_double() {
    assert(test_case(std::numbers::e, 0x1.5bf0a8b145769p+1));
    assert(test_case(std::numbers::log2e, 0x1.71547652b82fep+0));
    assert(test_case(std::numbers::log10e, 0x1.bcb7b1526e50ep-2));
    assert(test_case(std::numbers::pi, 0x1.921fb54442d18p+1));
    assert(test_case(std::numbers::inv_pi, 0x1.45f306dc9c883p-2));
    assert(test_case(std::numbers::inv_sqrtpi, 0x1.20dd750429b6dp-1));
    assert(test_case(std::numbers::ln2, 0x1.62e42fefa39efp-1));
    assert(test_case(std::numbers::ln10, 0x1.26bb1bbb55516p+1));
    assert(test_case(std::numbers::sqrt2, 0x1.6a09e667f3bcdp+0));
    assert(test_case(std::numbers::sqrt3, 0x1.bb67ae8584caap+0));
    assert(test_case(std::numbers::inv_sqrt3, 0x1.279a74590331cp-1));
    assert(test_case(std::numbers::egamma, 0x1.2788cfc6fb619p-1));
    assert(test_case(std::numbers::phi, 0x1.9e3779b97f4a8p+0));
}

// N4944 [math.constants]/2: "a program may partially or explicitly specialize a mathematical constant
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

constexpr void test_program_defined_specialization() {
    assert(test_case(std::numbers::e_v<Meow>.val, -10));
    assert(test_case(std::numbers::log2e_v<Meow>.val, -20));
    assert(test_case(std::numbers::log10e_v<Meow>.val, -30));
    assert(test_case(std::numbers::pi_v<Meow>.val, -40));
    assert(test_case(std::numbers::inv_pi_v<Meow>.val, -50));
    assert(test_case(std::numbers::inv_sqrtpi_v<Meow>.val, -60));
    assert(test_case(std::numbers::ln2_v<Meow>.val, -70));
    assert(test_case(std::numbers::ln10_v<Meow>.val, -80));
    assert(test_case(std::numbers::sqrt2_v<Meow>.val, -90));
    assert(test_case(std::numbers::sqrt3_v<Meow>.val, -100));
    assert(test_case(std::numbers::inv_sqrt3_v<Meow>.val, -110));
    assert(test_case(std::numbers::egamma_v<Meow>.val, -120));
    assert(test_case(std::numbers::phi_v<Meow>.val, -130));
}

constexpr bool test_all() {
    test_cv_floating_point<modify_cv::type_identity>();
    test_cv_floating_point<modify_cv::add_const>();

    if (!std::is_constant_evaluated()) {
        test_cv_floating_point<modify_cv::add_volatile>(); // constexpr-incompatible
        test_cv_floating_point<modify_cv::add_cv>(); // constexpr-incompatible
    }

    test_double();
    test_program_defined_specialization();

    return true;
}

int main() {
    assert(test_all());
    STATIC_ASSERT(test_all());
}
