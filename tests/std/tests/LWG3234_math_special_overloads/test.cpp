// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <cassert>
#include <cmath>
#include <limits>
#include <type_traits>

using namespace std;

struct Ambiguous : true_type {
    operator float() const {
        return 0.0f;
    }

    operator double() const {
        return 0.0;
    }
};

Ambiguous assoc_laguerre(unsigned int, unsigned int, Ambiguous) {
    return Ambiguous{};
}

Ambiguous assoc_legendre(unsigned int, unsigned int, Ambiguous) {
    return Ambiguous{};
}

Ambiguous beta(Ambiguous, Ambiguous) {
    return Ambiguous{};
}

Ambiguous comp_ellint_1(Ambiguous) {
    return Ambiguous{};
}

Ambiguous comp_ellint_2(Ambiguous) {
    return Ambiguous{};
}

Ambiguous comp_ellint_3(Ambiguous, Ambiguous) {
    return Ambiguous{};
}

Ambiguous cyl_bessel_i(Ambiguous, Ambiguous) {
    return Ambiguous{};
}

Ambiguous cyl_bessel_j(Ambiguous, Ambiguous) {
    return Ambiguous{};
}

Ambiguous cyl_bessel_k(Ambiguous, Ambiguous) {
    return Ambiguous{};
}

Ambiguous cyl_neumann(Ambiguous, Ambiguous) {
    return Ambiguous{};
}

Ambiguous ellint_1(Ambiguous, Ambiguous) {
    return Ambiguous{};
}

Ambiguous ellint_2(Ambiguous, Ambiguous) {
    return Ambiguous{};
}

Ambiguous ellint_3(Ambiguous, Ambiguous, Ambiguous) {
    return Ambiguous{};
}

Ambiguous expint(Ambiguous) {
    return Ambiguous{};
}

Ambiguous hermite(unsigned int, Ambiguous) {
    return Ambiguous{};
}

Ambiguous legendre(unsigned int, Ambiguous) {
    return Ambiguous{};
}

Ambiguous laguerre(unsigned int, Ambiguous) {
    return Ambiguous{};
}

Ambiguous riemann_zeta(Ambiguous) {
    return Ambiguous{};
}

Ambiguous sph_bessel(unsigned int, Ambiguous) {
    return Ambiguous{};
}

Ambiguous sph_legendre(unsigned int, unsigned int, Ambiguous) {
    return Ambiguous{};
}

Ambiguous sph_neumann(unsigned int, Ambiguous) {
    return Ambiguous{};
}

bool expect_epsilons(double expected, double calculated, unsigned int multiple) {
    return abs((calculated - expected) / expected) <= multiple * numeric_limits<double>::epsilon();
}

void test_assoc_laguerre() {
    static_assert(is_same_v<decltype(assoc_laguerre(0u, 0u, 0.0f)), float>);
    static_assert(is_same_v<decltype(assoc_laguerre(0u, 0u, false)), double>);
    static_assert(is_same_v<decltype(assoc_laguerre(0u, 0u, static_cast<unsigned short>(0))), double>);
    static_assert(is_same_v<decltype(assoc_laguerre(0u, 0u, 0)), double>);
    static_assert(is_same_v<decltype(assoc_laguerre(0u, 0u, 0u)), double>);
    static_assert(is_same_v<decltype(assoc_laguerre(0u, 0u, 0l)), double>);
    static_assert(is_same_v<decltype(assoc_laguerre(0u, 0u, 0ul)), double>);
    static_assert(is_same_v<decltype(assoc_laguerre(0u, 0u, 0ll)), double>);
    static_assert(is_same_v<decltype(assoc_laguerre(0u, 0u, 0ull)), double>);
    static_assert(is_same_v<decltype(assoc_laguerre(0u, 0u, 0.0)), double>);
    static_assert(is_same_v<decltype(assoc_laguerre(0u, 0u, 0.0l)), long double>);
    static_assert(is_same_v<decltype(assoc_laguerref(0u, 0u, 0)), float>);
    static_assert(is_same_v<decltype(assoc_laguerrel(0u, 0u, 0)), long double>);
    static_assert(is_same_v<decltype(assoc_laguerre(0u, 0u, Ambiguous{})), Ambiguous>);
    assert(assoc_laguerre(1u, 2u, 4) == -1.0);
}

void test_assoc_legendre() {
    static_assert(is_same_v<decltype(assoc_legendre(0u, 0u, 0.0f)), float>);
    static_assert(is_same_v<decltype(assoc_legendre(0u, 0u, false)), double>);
    static_assert(is_same_v<decltype(assoc_legendre(0u, 0u, static_cast<unsigned short>(0))), double>);
    static_assert(is_same_v<decltype(assoc_legendre(0u, 0u, 0)), double>);
    static_assert(is_same_v<decltype(assoc_legendre(0u, 0u, 0u)), double>);
    static_assert(is_same_v<decltype(assoc_legendre(0u, 0u, 0l)), double>);
    static_assert(is_same_v<decltype(assoc_legendre(0u, 0u, 0ul)), double>);
    static_assert(is_same_v<decltype(assoc_legendre(0u, 0u, 0ll)), double>);
    static_assert(is_same_v<decltype(assoc_legendre(0u, 0u, 0ull)), double>);
    static_assert(is_same_v<decltype(assoc_legendre(0u, 0u, 0.0)), double>);
    static_assert(is_same_v<decltype(assoc_legendre(0u, 0u, 0.0l)), long double>);
    static_assert(is_same_v<decltype(assoc_legendref(0u, 0u, 0)), float>);
    static_assert(is_same_v<decltype(assoc_legendrel(0u, 0u, 0)), long double>);
    static_assert(is_same_v<decltype(assoc_legendre(0u, 0u, Ambiguous{})), Ambiguous>);
    assert(assoc_legendre(2u, 0u, 1) == 1.0);
}

void test_beta() {
    static_assert(is_same_v<decltype(beta(0.0f, 0.0f)), float>);
    static_assert(is_same_v<decltype(beta(false, 0.0f)), double>);
    static_assert(is_same_v<decltype(beta(static_cast<unsigned short>(0), 0.0)), double>);
    static_assert(is_same_v<decltype(beta(0, 0.0l)), long double>);
    static_assert(is_same_v<decltype(beta(0.0f, 0u)), double>);
    static_assert(is_same_v<decltype(beta(0.0, 0l)), double>);
    static_assert(is_same_v<decltype(beta(0.0l, 0ul)), long double>);
    static_assert(is_same_v<decltype(beta(0, 0ll)), double>);
    static_assert(is_same_v<decltype(beta(0, 0ull)), double>);
    static_assert(is_same_v<decltype(beta(0.0, 0.0)), double>);
    static_assert(is_same_v<decltype(beta(0.0l, 0.0l)), long double>);
    static_assert(is_same_v<decltype(beta(0.0f, 0.0)), double>);
    static_assert(is_same_v<decltype(beta(0.0f, 0.0l)), long double>);
    static_assert(is_same_v<decltype(beta(0.0, 0.0l)), long double>);
    static_assert(is_same_v<decltype(betaf(0, 0)), float>);
    static_assert(is_same_v<decltype(betal(0, 0)), long double>);
    static_assert(is_same_v<decltype(beta(0, 0)), double>);
    static_assert(is_same_v<decltype(beta(Ambiguous{}, Ambiguous{})), Ambiguous>);
    assert(beta(1, 2) == 0.5);
}

void test_comp_ellint_1() {
    static_assert(is_same_v<decltype(comp_ellint_1(0.0f)), float>);
    static_assert(is_same_v<decltype(comp_ellint_1(false)), double>);
    static_assert(is_same_v<decltype(comp_ellint_1(static_cast<unsigned short>(0))), double>);
    static_assert(is_same_v<decltype(comp_ellint_1(0)), double>);
    static_assert(is_same_v<decltype(comp_ellint_1(0u)), double>);
    static_assert(is_same_v<decltype(comp_ellint_1(0l)), double>);
    static_assert(is_same_v<decltype(comp_ellint_1(0ul)), double>);
    static_assert(is_same_v<decltype(comp_ellint_1(0ll)), double>);
    static_assert(is_same_v<decltype(comp_ellint_1(0ull)), double>);
    static_assert(is_same_v<decltype(comp_ellint_1(0.0)), double>);
    static_assert(is_same_v<decltype(comp_ellint_1(0.0l)), long double>);
    static_assert(is_same_v<decltype(comp_ellint_1f(0)), float>);
    static_assert(is_same_v<decltype(comp_ellint_1l(0)), long double>);
    static_assert(is_same_v<decltype(comp_ellint_1(Ambiguous{})), Ambiguous>);
    assert(comp_ellint_1(0) == acos(-1.0) / 2);
}

void test_comp_ellint_2() {
    static_assert(is_same_v<decltype(comp_ellint_2(0.0f)), float>);
    static_assert(is_same_v<decltype(comp_ellint_2(false)), double>);
    static_assert(is_same_v<decltype(comp_ellint_2(static_cast<unsigned short>(0))), double>);
    static_assert(is_same_v<decltype(comp_ellint_2(0)), double>);
    static_assert(is_same_v<decltype(comp_ellint_2(0u)), double>);
    static_assert(is_same_v<decltype(comp_ellint_2(0l)), double>);
    static_assert(is_same_v<decltype(comp_ellint_2(0ul)), double>);
    static_assert(is_same_v<decltype(comp_ellint_2(0ll)), double>);
    static_assert(is_same_v<decltype(comp_ellint_2(0ull)), double>);
    static_assert(is_same_v<decltype(comp_ellint_2(0.0)), double>);
    static_assert(is_same_v<decltype(comp_ellint_2(0.0l)), long double>);
    static_assert(is_same_v<decltype(comp_ellint_2f(0)), float>);
    static_assert(is_same_v<decltype(comp_ellint_2l(0)), long double>);
    static_assert(is_same_v<decltype(comp_ellint_2(Ambiguous{})), Ambiguous>);
    assert(comp_ellint_2(0) == acos(-1.0) / 2);
}

void test_comp_ellint_3() {
    static_assert(is_same_v<decltype(comp_ellint_3(0.0f, 0.0f)), float>);
    static_assert(is_same_v<decltype(comp_ellint_3(false, 0.0f)), double>);
    static_assert(is_same_v<decltype(comp_ellint_3(static_cast<unsigned short>(0), 0.0)), double>);
    static_assert(is_same_v<decltype(comp_ellint_3(0, 0.0l)), long double>);
    static_assert(is_same_v<decltype(comp_ellint_3(0.0f, 0u)), double>);
    static_assert(is_same_v<decltype(comp_ellint_3(0.0, 0l)), double>);
    static_assert(is_same_v<decltype(comp_ellint_3(0.0l, 0ul)), long double>);
    static_assert(is_same_v<decltype(comp_ellint_3(0, 0ll)), double>);
    static_assert(is_same_v<decltype(comp_ellint_3(0, 0ull)), double>);
    static_assert(is_same_v<decltype(comp_ellint_3(0.0, 0.0)), double>);
    static_assert(is_same_v<decltype(comp_ellint_3(0.0l, 0.0l)), long double>);
    static_assert(is_same_v<decltype(comp_ellint_3(0.0f, 0.0)), double>);
    static_assert(is_same_v<decltype(comp_ellint_3(0.0f, 0.0l)), long double>);
    static_assert(is_same_v<decltype(comp_ellint_3(0.0, 0.0l)), long double>);
    static_assert(is_same_v<decltype(comp_ellint_3f(0, 0)), float>);
    static_assert(is_same_v<decltype(comp_ellint_3l(0, 0)), long double>);
    static_assert(is_same_v<decltype(comp_ellint_3(0, 0)), double>);
    static_assert(is_same_v<decltype(comp_ellint_3(Ambiguous{}, Ambiguous{})), Ambiguous>);
    assert(comp_ellint_3(0, 0) == acos(-1.0) / 2);
}

void test_cyl_bessel_i() {
    static_assert(is_same_v<decltype(cyl_bessel_i(0.0f, 0.0f)), float>);
    static_assert(is_same_v<decltype(cyl_bessel_i(false, 0.0f)), double>);
    static_assert(is_same_v<decltype(cyl_bessel_i(static_cast<unsigned short>(0), 0.0)), double>);
    static_assert(is_same_v<decltype(cyl_bessel_i(0, 0.0l)), long double>);
    static_assert(is_same_v<decltype(cyl_bessel_i(0.0f, 0u)), double>);
    static_assert(is_same_v<decltype(cyl_bessel_i(0.0, 0l)), double>);
    static_assert(is_same_v<decltype(cyl_bessel_i(0.0l, 0ul)), long double>);
    static_assert(is_same_v<decltype(cyl_bessel_i(0, 0ll)), double>);
    static_assert(is_same_v<decltype(cyl_bessel_i(0, 0ull)), double>);
    static_assert(is_same_v<decltype(cyl_bessel_i(0.0, 0.0)), double>);
    static_assert(is_same_v<decltype(cyl_bessel_i(0.0l, 0.0l)), long double>);
    static_assert(is_same_v<decltype(cyl_bessel_i(0.0f, 0.0)), double>);
    static_assert(is_same_v<decltype(cyl_bessel_i(0.0f, 0.0l)), long double>);
    static_assert(is_same_v<decltype(cyl_bessel_i(0.0, 0.0l)), long double>);
    static_assert(is_same_v<decltype(cyl_bessel_if(0, 0)), float>);
    static_assert(is_same_v<decltype(cyl_bessel_il(0, 0)), long double>);
    static_assert(is_same_v<decltype(cyl_bessel_i(0, 0)), double>);
    static_assert(is_same_v<decltype(cyl_bessel_i(Ambiguous{}, Ambiguous{})), Ambiguous>);
    assert(cyl_bessel_i(0, 0) == 1.0);
}

void test_cyl_bessel_j() {
    static_assert(is_same_v<decltype(cyl_bessel_j(0.0f, 0.0f)), float>);
    static_assert(is_same_v<decltype(cyl_bessel_j(false, 0.0f)), double>);
    static_assert(is_same_v<decltype(cyl_bessel_j(static_cast<unsigned short>(0), 0.0)), double>);
    static_assert(is_same_v<decltype(cyl_bessel_j(0, 0.0l)), long double>);
    static_assert(is_same_v<decltype(cyl_bessel_j(0.0f, 0u)), double>);
    static_assert(is_same_v<decltype(cyl_bessel_j(0.0, 0l)), double>);
    static_assert(is_same_v<decltype(cyl_bessel_j(0.0l, 0ul)), long double>);
    static_assert(is_same_v<decltype(cyl_bessel_j(0, 0ll)), double>);
    static_assert(is_same_v<decltype(cyl_bessel_j(0, 0ull)), double>);
    static_assert(is_same_v<decltype(cyl_bessel_j(0.0, 0.0)), double>);
    static_assert(is_same_v<decltype(cyl_bessel_j(0.0l, 0.0l)), long double>);
    static_assert(is_same_v<decltype(cyl_bessel_j(0.0f, 0.0)), double>);
    static_assert(is_same_v<decltype(cyl_bessel_j(0.0f, 0.0l)), long double>);
    static_assert(is_same_v<decltype(cyl_bessel_j(0.0, 0.0l)), long double>);
    static_assert(is_same_v<decltype(cyl_bessel_jf(0, 0)), float>);
    static_assert(is_same_v<decltype(cyl_bessel_jl(0, 0)), long double>);
    static_assert(is_same_v<decltype(cyl_bessel_j(0, 0)), double>);
    static_assert(is_same_v<decltype(cyl_bessel_j(Ambiguous{}, Ambiguous{})), Ambiguous>);
    assert(cyl_bessel_j(0, 0) == 1.0);
}

void test_cyl_bessel_k() {
    static_assert(is_same_v<decltype(cyl_bessel_k(0.0f, 0.0f)), float>);
    static_assert(is_same_v<decltype(cyl_bessel_k(false, 0.0f)), double>);
    static_assert(is_same_v<decltype(cyl_bessel_k(static_cast<unsigned short>(0), 0.0)), double>);
    static_assert(is_same_v<decltype(cyl_bessel_k(0, 0.0l)), long double>);
    static_assert(is_same_v<decltype(cyl_bessel_k(0.0f, 0u)), double>);
    static_assert(is_same_v<decltype(cyl_bessel_k(0.0, 0l)), double>);
    static_assert(is_same_v<decltype(cyl_bessel_k(0.0l, 0ul)), long double>);
    static_assert(is_same_v<decltype(cyl_bessel_k(0, 0ll)), double>);
    static_assert(is_same_v<decltype(cyl_bessel_k(0, 0ull)), double>);
    static_assert(is_same_v<decltype(cyl_bessel_k(0.0, 0.0)), double>);
    static_assert(is_same_v<decltype(cyl_bessel_k(0.0l, 0.0l)), long double>);
    static_assert(is_same_v<decltype(cyl_bessel_k(0.0f, 0.0)), double>);
    static_assert(is_same_v<decltype(cyl_bessel_k(0.0f, 0.0l)), long double>);
    static_assert(is_same_v<decltype(cyl_bessel_k(0.0, 0.0l)), long double>);
    static_assert(is_same_v<decltype(cyl_bessel_kf(0, 0)), float>);
    static_assert(is_same_v<decltype(cyl_bessel_kl(0, 0)), long double>);
    static_assert(is_same_v<decltype(cyl_bessel_k(0, 0)), double>);
    static_assert(is_same_v<decltype(cyl_bessel_k(Ambiguous{}, Ambiguous{})), Ambiguous>);
    assert(expect_epsilons(cyl_bessel_k(0.5, 1), acos(-1.0) / 2 * (cyl_bessel_i(-0.5, 1) - cyl_bessel_i(0.5, 1)), 10));
}

void test_cyl_neumann() {
    static_assert(is_same_v<decltype(cyl_neumann(0.0f, 0.0f)), float>);
    static_assert(is_same_v<decltype(cyl_neumann(false, 0.0f)), double>);
    static_assert(is_same_v<decltype(cyl_neumann(static_cast<unsigned short>(0), 0.0)), double>);
    static_assert(is_same_v<decltype(cyl_neumann(0, 0.0l)), long double>);
    static_assert(is_same_v<decltype(cyl_neumann(0.0f, 0u)), double>);
    static_assert(is_same_v<decltype(cyl_neumann(0.0, 0l)), double>);
    static_assert(is_same_v<decltype(cyl_neumann(0.0l, 0ul)), long double>);
    static_assert(is_same_v<decltype(cyl_neumann(0, 0ll)), double>);
    static_assert(is_same_v<decltype(cyl_neumann(0, 0ull)), double>);
    static_assert(is_same_v<decltype(cyl_neumann(0.0, 0.0)), double>);
    static_assert(is_same_v<decltype(cyl_neumann(0.0l, 0.0l)), long double>);
    static_assert(is_same_v<decltype(cyl_neumann(0.0f, 0.0)), double>);
    static_assert(is_same_v<decltype(cyl_neumann(0.0f, 0.0l)), long double>);
    static_assert(is_same_v<decltype(cyl_neumann(0.0, 0.0l)), long double>);
    static_assert(is_same_v<decltype(cyl_neumannf(0, 0)), float>);
    static_assert(is_same_v<decltype(cyl_neumannl(0, 0)), long double>);
    static_assert(is_same_v<decltype(cyl_neumann(0, 0)), double>);
    static_assert(is_same_v<decltype(cyl_neumann(Ambiguous{}, Ambiguous{})), Ambiguous>);
    assert(cyl_neumann(0.5, 1) == -cyl_bessel_j(-0.5, 1));
}

void test_ellint_1() {
    static_assert(is_same_v<decltype(ellint_1(0.0f, 0.0f)), float>);
    static_assert(is_same_v<decltype(ellint_1(false, 0.0f)), double>);
    static_assert(is_same_v<decltype(ellint_1(static_cast<unsigned short>(0), 0.0)), double>);
    static_assert(is_same_v<decltype(ellint_1(0, 0.0l)), long double>);
    static_assert(is_same_v<decltype(ellint_1(0.0f, 0u)), double>);
    static_assert(is_same_v<decltype(ellint_1(0.0, 0l)), double>);
    static_assert(is_same_v<decltype(ellint_1(0.0l, 0ul)), long double>);
    static_assert(is_same_v<decltype(ellint_1(0, 0ll)), double>);
    static_assert(is_same_v<decltype(ellint_1(0, 0ull)), double>);
    static_assert(is_same_v<decltype(ellint_1(0.0, 0.0)), double>);
    static_assert(is_same_v<decltype(ellint_1(0.0l, 0.0l)), long double>);
    static_assert(is_same_v<decltype(ellint_1(0.0f, 0.0)), double>);
    static_assert(is_same_v<decltype(ellint_1(0.0f, 0.0l)), long double>);
    static_assert(is_same_v<decltype(ellint_1(0.0, 0.0l)), long double>);
    static_assert(is_same_v<decltype(ellint_1f(0, 0)), float>);
    static_assert(is_same_v<decltype(ellint_1l(0, 0)), long double>);
    static_assert(is_same_v<decltype(ellint_1(0, 0)), double>);
    static_assert(is_same_v<decltype(ellint_1(Ambiguous{}, Ambiguous{})), Ambiguous>);

    const double half_pi = acos(-1.0) / 2.0;
    assert(ellint_1(0, half_pi) == half_pi);
    assert(ellint_1(0, -half_pi) == -half_pi);
    assert(ellint_1(0, 0) == 0);
}

void test_ellint_2() {
    static_assert(is_same_v<decltype(ellint_2(0.0f, 0.0f)), float>);
    static_assert(is_same_v<decltype(ellint_2(false, 0.0f)), double>);
    static_assert(is_same_v<decltype(ellint_2(static_cast<unsigned short>(0), 0.0)), double>);
    static_assert(is_same_v<decltype(ellint_2(0, 0.0l)), long double>);
    static_assert(is_same_v<decltype(ellint_2(0.0f, 0u)), double>);
    static_assert(is_same_v<decltype(ellint_2(0.0, 0l)), double>);
    static_assert(is_same_v<decltype(ellint_2(0.0l, 0ul)), long double>);
    static_assert(is_same_v<decltype(ellint_2(0, 0ll)), double>);
    static_assert(is_same_v<decltype(ellint_2(0, 0ull)), double>);
    static_assert(is_same_v<decltype(ellint_2(0.0, 0.0)), double>);
    static_assert(is_same_v<decltype(ellint_2(0.0l, 0.0l)), long double>);
    static_assert(is_same_v<decltype(ellint_2(0.0f, 0.0)), double>);
    static_assert(is_same_v<decltype(ellint_2(0.0f, 0.0l)), long double>);
    static_assert(is_same_v<decltype(ellint_2(0.0, 0.0l)), long double>);
    static_assert(is_same_v<decltype(ellint_2f(0, 0)), float>);
    static_assert(is_same_v<decltype(ellint_2l(0, 0)), long double>);
    static_assert(is_same_v<decltype(ellint_2(0, 0)), double>);
    static_assert(is_same_v<decltype(ellint_2(Ambiguous{}, Ambiguous{})), Ambiguous>);

    const double half_pi = acos(-1.0) / 2.0;
    assert(ellint_2(0, half_pi) == half_pi);
    assert(ellint_2(0, -half_pi) == -half_pi);
    assert(ellint_2(0, 0) == 0.0);
    assert(ellint_2(1, half_pi) == 1.0);
}

void test_ellint_3() {
    static_assert(is_same_v<decltype(ellint_3(false, 0.0f, 0.0f)), double>);
    static_assert(is_same_v<decltype(ellint_3('\0', 0.0f, 0.0f)), double>);
    static_assert(is_same_v<decltype(ellint_3(0u, 0.0f, 0.0f)), double>);
    static_assert(is_same_v<decltype(ellint_3(0.0f, 0, 0.0f)), double>);
    static_assert(is_same_v<decltype(ellint_3(0.0f, 0l, 0.0f)), double>);
    static_assert(is_same_v<decltype(ellint_3(0.0f, 0.0f, 0ull)), double>);
    static_assert(is_same_v<decltype(ellint_3(0.0f, 0.0f, 0.0)), double>);
    static_assert(is_same_v<decltype(ellint_3(0.0f, 0.0f, 0.0l)), long double>);
    static_assert(is_same_v<decltype(ellint_3(0.0f, 0.0f, 0.0f)), float>);

    static_assert(is_same_v<decltype(ellint_3(false, 0.0, 0.0)), double>);
    static_assert(is_same_v<decltype(ellint_3('\0', 0.0, 0.0)), double>);
    static_assert(is_same_v<decltype(ellint_3(0u, 0.0, 0.0)), double>);
    static_assert(is_same_v<decltype(ellint_3(0.0, 0, 0.0)), double>);
    static_assert(is_same_v<decltype(ellint_3(0.0, 0l, 0.0)), double>);
    static_assert(is_same_v<decltype(ellint_3(0.0, 0.0, 0ull)), double>);
    static_assert(is_same_v<decltype(ellint_3(0.0, 0.0, 0.0f)), double>);
    static_assert(is_same_v<decltype(ellint_3(0.0, 0.0, 0.0l)), long double>);
    static_assert(is_same_v<decltype(ellint_3(0.0, 0.0, 0.0)), double>);

    static_assert(is_same_v<decltype(ellint_3(false, 0.0l, 0.0l)), long double>);
    static_assert(is_same_v<decltype(ellint_3('\0', 0.0l, 0.0l)), long double>);
    static_assert(is_same_v<decltype(ellint_3(0u, 0.0l, 0.0l)), long double>);
    static_assert(is_same_v<decltype(ellint_3(0.0l, 0, 0.0l)), long double>);
    static_assert(is_same_v<decltype(ellint_3(0.0l, 0l, 0.0l)), long double>);
    static_assert(is_same_v<decltype(ellint_3(0.0l, 0.0l, 0ull)), long double>);
    static_assert(is_same_v<decltype(ellint_3(0.0l, 0.0l, 0.0f)), long double>);
    static_assert(is_same_v<decltype(ellint_3(0.0, 0.0l, 0.0l)), long double>);
    static_assert(is_same_v<decltype(ellint_3(0.0l, 0.0l, 0.0l)), long double>);

    static_assert(is_same_v<decltype(ellint_3f(0, 0, 0)), float>);
    static_assert(is_same_v<decltype(ellint_3l(0, 0, 0)), long double>);
    static_assert(is_same_v<decltype(ellint_3(Ambiguous{}, Ambiguous{}, Ambiguous{})), Ambiguous>);

    const double half_pi = acos(-1.0) / 2.0;
    assert(ellint_3(0, 0, half_pi) == half_pi);
}

void test_expint() {
    static_assert(is_same_v<decltype(expint(0.0f)), float>);
    static_assert(is_same_v<decltype(expint(false)), double>);
    static_assert(is_same_v<decltype(expint(static_cast<unsigned short>(0))), double>);
    static_assert(is_same_v<decltype(expint(0)), double>);
    static_assert(is_same_v<decltype(expint(0u)), double>);
    static_assert(is_same_v<decltype(expint(0l)), double>);
    static_assert(is_same_v<decltype(expint(0ul)), double>);
    static_assert(is_same_v<decltype(expint(0ll)), double>);
    static_assert(is_same_v<decltype(expint(0ull)), double>);
    static_assert(is_same_v<decltype(expint(0.0)), double>);
    static_assert(is_same_v<decltype(expint(0.0l)), long double>);
    static_assert(is_same_v<decltype(expintf(0)), float>);
    static_assert(is_same_v<decltype(expintl(0)), long double>);
    static_assert(is_same_v<decltype(expint(Ambiguous{})), Ambiguous>);
    assert(expint(0) == -numeric_limits<double>::infinity());
}

void test_hermite() {
    static_assert(is_same_v<decltype(hermite(0u, 0.0f)), float>);
    static_assert(is_same_v<decltype(hermite(0u, false)), double>);
    static_assert(is_same_v<decltype(hermite(0u, static_cast<unsigned short>(0))), double>);
    static_assert(is_same_v<decltype(hermite(0u, 0)), double>);
    static_assert(is_same_v<decltype(hermite(0u, 0u)), double>);
    static_assert(is_same_v<decltype(hermite(0u, 0l)), double>);
    static_assert(is_same_v<decltype(hermite(0u, 0ul)), double>);
    static_assert(is_same_v<decltype(hermite(0u, 0ll)), double>);
    static_assert(is_same_v<decltype(hermite(0u, 0ull)), double>);
    static_assert(is_same_v<decltype(hermite(0u, 0.0)), double>);
    static_assert(is_same_v<decltype(hermite(0u, 0.0l)), long double>);
    static_assert(is_same_v<decltype(hermitef(0u, 0)), float>);
    static_assert(is_same_v<decltype(hermitel(0u, 0)), long double>);
    static_assert(is_same_v<decltype(hermite(0u, Ambiguous{})), Ambiguous>);
    assert(hermite(2, 3) == 34.0);
}

void test_laguerre() {
    static_assert(is_same_v<decltype(laguerre(0u, 0.0f)), float>);
    static_assert(is_same_v<decltype(laguerre(0u, false)), double>);
    static_assert(is_same_v<decltype(laguerre(0u, static_cast<unsigned short>(0))), double>);
    static_assert(is_same_v<decltype(laguerre(0u, 0)), double>);
    static_assert(is_same_v<decltype(laguerre(0u, 0u)), double>);
    static_assert(is_same_v<decltype(laguerre(0u, 0l)), double>);
    static_assert(is_same_v<decltype(laguerre(0u, 0ul)), double>);
    static_assert(is_same_v<decltype(laguerre(0u, 0ll)), double>);
    static_assert(is_same_v<decltype(laguerre(0u, 0ull)), double>);
    static_assert(is_same_v<decltype(laguerre(0u, 0.0)), double>);
    static_assert(is_same_v<decltype(laguerre(0u, 0.0l)), long double>);
    static_assert(is_same_v<decltype(laguerref(0u, 0)), float>);
    static_assert(is_same_v<decltype(laguerrel(0u, 0)), long double>);
    static_assert(is_same_v<decltype(laguerre(0u, Ambiguous{})), Ambiguous>);
    assert(laguerre(2, 3) == -0.5);
}

void test_legendre() {
    static_assert(is_same_v<decltype(legendre(0u, 0.0f)), float>);
    static_assert(is_same_v<decltype(legendre(0u, false)), double>);
    static_assert(is_same_v<decltype(legendre(0u, static_cast<unsigned short>(0))), double>);
    static_assert(is_same_v<decltype(legendre(0u, 0)), double>);
    static_assert(is_same_v<decltype(legendre(0u, 0u)), double>);
    static_assert(is_same_v<decltype(legendre(0u, 0l)), double>);
    static_assert(is_same_v<decltype(legendre(0u, 0ul)), double>);
    static_assert(is_same_v<decltype(legendre(0u, 0ll)), double>);
    static_assert(is_same_v<decltype(legendre(0u, 0ull)), double>);
    static_assert(is_same_v<decltype(legendre(0u, 0.0)), double>);
    static_assert(is_same_v<decltype(legendre(0u, 0.0l)), long double>);
    static_assert(is_same_v<decltype(legendref(0u, 0)), float>);
    static_assert(is_same_v<decltype(legendrel(0u, 0)), long double>);
    static_assert(is_same_v<decltype(legendre(0u, Ambiguous{})), Ambiguous>);
    assert(legendre(2, -1) == 1.0);
}

void test_riemann_zeta() {
    static_assert(is_same_v<decltype(riemann_zeta(0.0f)), float>);
    static_assert(is_same_v<decltype(riemann_zeta(false)), double>);
    static_assert(is_same_v<decltype(riemann_zeta(static_cast<unsigned short>(0))), double>);
    static_assert(is_same_v<decltype(riemann_zeta(0)), double>);
    static_assert(is_same_v<decltype(riemann_zeta(0u)), double>);
    static_assert(is_same_v<decltype(riemann_zeta(0l)), double>);
    static_assert(is_same_v<decltype(riemann_zeta(0ul)), double>);
    static_assert(is_same_v<decltype(riemann_zeta(0ll)), double>);
    static_assert(is_same_v<decltype(riemann_zeta(0ull)), double>);
    static_assert(is_same_v<decltype(riemann_zeta(0.0)), double>);
    static_assert(is_same_v<decltype(riemann_zeta(0.0l)), long double>);
    static_assert(is_same_v<decltype(riemann_zetaf(0)), float>);
    static_assert(is_same_v<decltype(riemann_zetal(0)), long double>);
    static_assert(is_same_v<decltype(riemann_zeta(Ambiguous{})), Ambiguous>);
    assert(riemann_zeta(0) == -0.5);
}

void test_sph_bessel() {
    static_assert(is_same_v<decltype(sph_bessel(0u, 0.0f)), float>);
    static_assert(is_same_v<decltype(sph_bessel(0u, false)), double>);
    static_assert(is_same_v<decltype(sph_bessel(0u, static_cast<unsigned short>(0))), double>);
    static_assert(is_same_v<decltype(sph_bessel(0u, 0)), double>);
    static_assert(is_same_v<decltype(sph_bessel(0u, 0u)), double>);
    static_assert(is_same_v<decltype(sph_bessel(0u, 0l)), double>);
    static_assert(is_same_v<decltype(sph_bessel(0u, 0ul)), double>);
    static_assert(is_same_v<decltype(sph_bessel(0u, 0ll)), double>);
    static_assert(is_same_v<decltype(sph_bessel(0u, 0ull)), double>);
    static_assert(is_same_v<decltype(sph_bessel(0u, 0.0)), double>);
    static_assert(is_same_v<decltype(sph_bessel(0u, 0.0l)), long double>);
    static_assert(is_same_v<decltype(sph_besself(0u, 0)), float>);
    static_assert(is_same_v<decltype(sph_bessell(0u, 0)), long double>);
    static_assert(is_same_v<decltype(sph_bessel(0u, Ambiguous{})), Ambiguous>);
    assert(expect_epsilons(sph_bessel(1, 2), sin(2) / (static_cast<double>(2) * 2) - cos(2) / 2, 2));
}

void test_sph_legendre() {
    static_assert(is_same_v<decltype(sph_legendre(0u, 0u, 0.0f)), float>);
    static_assert(is_same_v<decltype(sph_legendre(0u, 0u, false)), double>);
    static_assert(is_same_v<decltype(sph_legendre(0u, 0u, static_cast<unsigned short>(0))), double>);
    static_assert(is_same_v<decltype(sph_legendre(0u, 0u, 0)), double>);
    static_assert(is_same_v<decltype(sph_legendre(0u, 0u, 0u)), double>);
    static_assert(is_same_v<decltype(sph_legendre(0u, 0u, 0l)), double>);
    static_assert(is_same_v<decltype(sph_legendre(0u, 0u, 0ul)), double>);
    static_assert(is_same_v<decltype(sph_legendre(0u, 0u, 0ll)), double>);
    static_assert(is_same_v<decltype(sph_legendre(0u, 0u, 0ull)), double>);
    static_assert(is_same_v<decltype(sph_legendre(0u, 0u, 0.0)), double>);
    static_assert(is_same_v<decltype(sph_legendre(0u, 0u, 0.0l)), long double>);
    static_assert(is_same_v<decltype(sph_legendref(0u, 0u, 0)), float>);
    static_assert(is_same_v<decltype(sph_legendrel(0u, 0u, 0)), long double>);
    static_assert(is_same_v<decltype(sph_legendre(0u, 0u, Ambiguous{})), Ambiguous>);

    const double pi = acos(-1.0);
    assert(expect_epsilons(sph_legendre(3, 0, 1), 0.25 * sqrt(7 / pi) * (5 * pow(cos(1), 3) - 3 * cos(1)), 2));
}

void test_sph_neumann() {
    static_assert(is_same_v<decltype(sph_neumann(0u, 0.0f)), float>);
    static_assert(is_same_v<decltype(sph_neumann(0u, false)), double>);
    static_assert(is_same_v<decltype(sph_neumann(0u, static_cast<unsigned short>(0))), double>);
    static_assert(is_same_v<decltype(sph_neumann(0u, 0)), double>);
    static_assert(is_same_v<decltype(sph_neumann(0u, 0u)), double>);
    static_assert(is_same_v<decltype(sph_neumann(0u, 0l)), double>);
    static_assert(is_same_v<decltype(sph_neumann(0u, 0ul)), double>);
    static_assert(is_same_v<decltype(sph_neumann(0u, 0ll)), double>);
    static_assert(is_same_v<decltype(sph_neumann(0u, 0ull)), double>);
    static_assert(is_same_v<decltype(sph_neumann(0u, 0.0)), double>);
    static_assert(is_same_v<decltype(sph_neumann(0u, 0.0l)), long double>);
    static_assert(is_same_v<decltype(sph_neumannf(0u, 0)), float>);
    static_assert(is_same_v<decltype(sph_neumannl(0u, 0)), long double>);
    static_assert(is_same_v<decltype(sph_neumann(0u, Ambiguous{})), Ambiguous>);
    assert(expect_epsilons(sph_neumann(1, 1), -cos(1) - sin(1), 2));
}

int main() {
    test_assoc_laguerre();
    test_assoc_legendre();
    test_beta();
    test_comp_ellint_1();
    test_comp_ellint_2();
    test_comp_ellint_3();
    test_cyl_bessel_i();
    test_cyl_bessel_j();
    test_cyl_bessel_k();
    test_cyl_neumann();
    test_ellint_1();
    test_ellint_2();
    test_ellint_3();
    test_expint();
    test_hermite();
    test_laguerre();
    test_legendre();
    test_riemann_zeta();
    test_sph_bessel();
    test_sph_legendre();
    test_sph_neumann();
}
