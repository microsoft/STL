// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <cassert>
#include <cmath>
#include <limits>
#include <utility>

using namespace std;

template <class Float>
bool isclose(Float f, Float g, const int ulps = 1) {
    if (f == g) {
        return true;
    }

    if (f > g) {
        swap(f, g);
    }
    // f < g
    for (int i = 0; i < ulps; ++i) {
        f = nextafter(f, numeric_limits<Float>::infinity());
        if (f == g) {
            return true;
        }
    }

    return false;
}

template <class Float>
constexpr Float half_pi = static_cast<Float>(1.57'079'632'679'489'662);

constexpr float nanf_v       = numeric_limits<float>::quiet_NaN();
constexpr double nand_v      = numeric_limits<double>::quiet_NaN();
constexpr long double nanl_v = numeric_limits<long double>::quiet_NaN();

// Note: we include 18 significant digits for easier reasoning;
// there are approximately 16 significant digits in binary64 ('double'),
// so adding 2 extra will _certainly_ always give us the exact right number.
// We use 18 significant digits for binary32 ('float') as well, for easier reasoning.
// The correct results were found via Wolfram Alpha.
// The formula for a specific function is given at the top of each test

void test_assoc_laguerre() {
    // assoc_laguerre(x, y, z) = LaguerreP[x, y, z]
    {
        assert(isnan(assoc_laguerre(1, 10, nanf_v)));
        assert(isnan(assoc_laguerre(1, 10, nand_v)));
        assert(isnan(assoc_laguerre(1, 10, nanl_v)));
        assert(isnan(assoc_laguerref(1, 10, nanf_v)));
        assert(isnan(assoc_laguerrel(1, 10, nanl_v)));
    }
    {
        assert(isclose(assoc_laguerre(1, 10, 0.5), 10.5));
        assert(isclose(assoc_laguerre(1, 10, 0.5f), 10.5f));
        assert(isclose(assoc_laguerre(1, 10, 0.5L), 10.5L));
        assert(isclose(assoc_laguerref(1, 10, 0.5f), 10.5f));
        assert(isclose(assoc_laguerrel(1, 10, 0.5L), 10.5L));
    }

    {
        assert(isclose(assoc_laguerre(2, 10, 0.5), 60.125));
        assert(isclose(assoc_laguerre(2, 10, 0.5f), 60.125f));
        assert(isclose(assoc_laguerre(2, 10, 0.5L), 60.125L));
        assert(isclose(assoc_laguerref(2, 10, 0.5f), 60.125f));
        assert(isclose(assoc_laguerrel(2, 10, 0.5L), 60.125L));
    }
}

void test_assoc_legendre() {
    // assoc_legendre(x, y, z) = LegendreP[x, y, z]
    {
        assert(isclose(assoc_legendre(2, 0, 0.5), -0.125));
        assert(isclose(assoc_legendre(2, 0, 0.5f), -0.125f));
        assert(isclose(assoc_legendre(2, 0, 0.5L), -0.125L));
        assert(isclose(assoc_legendref(2, 0, 0.5f), -0.125f));
        assert(isclose(assoc_legendrel(2, 0, 0.5L), -0.125L));
    }
    {
        assert(isclose(assoc_legendre(2, 1, 0.5), 1.29'903'810'567'665'797));
        assert(isclose(assoc_legendre(2, 1, 0.5f), 1.29'903'810'567'665'797f));
        assert(isclose(assoc_legendre(2, 1, 0.5L), 1.29'903'810'567'665'797L));
        assert(isclose(assoc_legendref(2, 1, 0.5f), 1.29'903'810'567'665'797f));
        assert(isclose(assoc_legendrel(2, 1, 0.5L), 1.29'903'810'567'665'797L));
    }
    {
        assert(isclose(assoc_legendre(2, 2, 0.5), 2.25));
        assert(isclose(assoc_legendre(2, 2, 0.5f), 2.25f));
        assert(isclose(assoc_legendre(2, 2, 0.5L), 2.25L));
        assert(isclose(assoc_legendref(2, 2, 0.5f), 2.25f));
        assert(isclose(assoc_legendrel(2, 2, 0.5L), 2.25L));
    }
}

void test_beta() {
    // beta(x, y) = Beta[x, y]
    {
        assert(isclose(beta(1.0, 1.0), 1.0));
        assert(isclose(beta(1.0f, 1.0f), 1.0f));
        assert(isclose(beta(1.0L, 1.0L), 1.0L));
        assert(isclose(betaf(1.0f, 1.0f), 1.0f));
        assert(isclose(betal(1.0L, 1.0L), 1.0L));
    }
    {
        assert(isclose(beta(2.0, 3.0), 0.0833'333'333'333'333'333, 2));
        assert(isclose(beta(2.0f, 3.0f), 0.0833'333'333'333'333'333f));
        assert(isclose(beta(2.0L, 3.0L), 0.0833'333'333'333'333'333L, 2));
        assert(isclose(betaf(2.0f, 3.0f), 0.0833'333'333'333'333'333f));
        assert(isclose(betal(2.0L, 3.0L), 0.0833'333'333'333'333'333L, 2));
    }
}

void test_comp_ellint_1() {
    // comp_ellint_1(x) = EllipticK[x^2]
    {
        assert(isclose(comp_ellint_1(0.0), half_pi<double>));
        assert(isclose(comp_ellint_1(0.0f), half_pi<float>));
        assert(isclose(comp_ellint_1(0.0L), half_pi<long double>));
        assert(isclose(comp_ellint_1f(0.0f), half_pi<float>));
        assert(isclose(comp_ellint_1l(0.0L), half_pi<long double>));
    }
    {
        assert(isclose(comp_ellint_1(0.5), 1.68'575'035'481'259'604));
        assert(isclose(comp_ellint_1(0.5f), 1.68'575'035'481'259'604f));
        assert(isclose(comp_ellint_1(0.5L), 1.68'575'035'481'259'604L));
        assert(isclose(comp_ellint_1f(0.5f), 1.68'575'035'481'259'604f));
        assert(isclose(comp_ellint_1l(0.5L), 1.68'575'035'481'259'604L));
    }
}

void test_comp_ellint_2() {
    // comp_ellint_2(x) = EllipticE[x^2]
    {
        assert(isnan(comp_ellint_2(nanf_v)));
        assert(isnan(comp_ellint_2(nand_v)));
        assert(isnan(comp_ellint_2(nanl_v)));
        assert(isnan(comp_ellint_2f(nanf_v)));
        assert(isnan(comp_ellint_2l(nanl_v)));
    }
    {
        assert(isclose(comp_ellint_2(0.0), half_pi<double>));
        assert(isclose(comp_ellint_2(0.0f), half_pi<float>));
        assert(isclose(comp_ellint_2(0.0L), half_pi<long double>));
        assert(isclose(comp_ellint_2f(0.0f), half_pi<float>));
        assert(isclose(comp_ellint_2l(0.0L), half_pi<long double>));
    }
    {
        assert(isclose(comp_ellint_2(1), 1.0));
        assert(isclose(comp_ellint_2(1.0f), 1.0f));
        assert(isclose(comp_ellint_2(1.0L), 1.0L));
        assert(isclose(comp_ellint_2f(1.0f), 1.0f));
        assert(isclose(comp_ellint_2l(1.0L), 1.0L));
    }
}

void test_comp_ellint_3() {
    // comp_ellint_3(x, y) = Pi[y, x^2]
    {
        assert(isnan(comp_ellint_3(0.0f, nanf_v)));
        assert(isnan(comp_ellint_3(0.0, nand_v)));
        assert(isnan(comp_ellint_3(0.0L, nanl_v)));
        assert(isnan(comp_ellint_3f(0.0f, nanf_v)));
        assert(isnan(comp_ellint_3l(0.0L, nanl_v)));
    }
    {
        assert(isnan(comp_ellint_3(nanf_v, 0.0f)));
        assert(isnan(comp_ellint_3(nand_v, 0.0)));
        assert(isnan(comp_ellint_3(nanl_v, 0.0L)));
        assert(isnan(comp_ellint_3f(nanf_v, 0.0f)));
        assert(isnan(comp_ellint_3l(nanl_v, 0.0L)));
    }
    {
        assert(isclose(comp_ellint_3(0.0, 0.0), half_pi<double>));
        assert(isclose(comp_ellint_3(0.0f, 0.0f), half_pi<float>));
        assert(isclose(comp_ellint_3(0.0L, 0.0L), half_pi<long double>));
        assert(isclose(comp_ellint_3f(0.0f, 0.0f), half_pi<float>));
        assert(isclose(comp_ellint_3l(0.0L, 0.0L), half_pi<long double>));
    }
    {
        assert(isclose(comp_ellint_3(0.5, 0.0), 1.68'575'035'481'259'604));
        assert(isclose(comp_ellint_3(0.5f, 0.0f), 1.68'575'035'481'259'604f));
        assert(isclose(comp_ellint_3(0.5L, 0.0L), 1.68'575'035'481'259'604L));
        assert(isclose(comp_ellint_3f(0.5f, 0.0f), 1.68'575'035'481'259'604f));
        assert(isclose(comp_ellint_3l(0.5L, 0.0L), 1.68'575'035'481'259'604L));
    }
}

void test_cyl_bessel_i() {
    // cyl_bessel_i(x, y) = BesselI[x, y]
    {
        assert(isnan(cyl_bessel_i(0.0, nand_v)));
        assert(isnan(cyl_bessel_if(0.0f, nanf_v)));
        assert(isnan(cyl_bessel_il(0.0L, nanl_v)));
    }
    {
        assert(isnan(cyl_bessel_i(nand_v, 0.0)));
        assert(isnan(cyl_bessel_if(nanf_v, 0.0f)));
        assert(isnan(cyl_bessel_il(nanl_v, 0.0L)));
    }
    {
        assert(isclose(cyl_bessel_i(0.0, 1.2345), 1.41'886'075'127'709'138));
        assert(isclose(cyl_bessel_if(0.0f, 1.2345f), 1.41'886'075'127'709'138f));
        assert(isclose(cyl_bessel_il(0.0L, 1.2345L), 1.41'886'075'127'709'138L));
    }
}

void test_cyl_bessel_j() {
    // cyl_bessel_i(x, y) = BesselJ[x, y]
    {
        assert(isnan(cyl_bessel_j(0.0, nand_v)));
        assert(isnan(cyl_bessel_jf(0.0f, nanf_v)));
        assert(isnan(cyl_bessel_jl(0.0L, nanl_v)));
    }
    {
        assert(isnan(cyl_bessel_j(nand_v, 0.0)));
        assert(isnan(cyl_bessel_jf(nanf_v, 0.0f)));
        assert(isnan(cyl_bessel_jl(nanl_v, 0.0L)));
    }
    {
        assert(isclose(cyl_bessel_j(0.0, 1.2345), 0.653'791'995'647'314'267));
        assert(isclose(cyl_bessel_jf(0.0f, 1.2345f), 0.653'791'995'647'314'267f));
        assert(isclose(cyl_bessel_jl(0.0L, 1.2345L), 0.653'791'995'647'314'267L));
    }
}

void test_cyl_bessel_k() {
    // cyl_bessel_k(x, y) = BesselK[x, y]
    {
        assert(isnan(cyl_bessel_k(0.0, nand_v)));
        assert(isnan(cyl_bessel_kf(0.0f, nanf_v)));
        assert(isnan(cyl_bessel_kl(0.0L, nanl_v)));
    }
    {
        assert(isnan(cyl_bessel_k(nand_v, 0.0)));
        assert(isnan(cyl_bessel_kf(nanf_v, 0.0f)));
        assert(isnan(cyl_bessel_kl(nanl_v, 0.0L)));
    }
    {
        assert(isclose(cyl_bessel_k(0.5, 1.2345), 0.328'229'683'116'574'238, 8));
        assert(isclose(cyl_bessel_kf(0.5f, 1.2345f), 0.328'229'683'116'574'238f));
        assert(isclose(cyl_bessel_kl(0.5L, 1.2345L), 0.328'229'683'116'574'238L, 8));
    }
}

void test_cyl_neumann() {
    // cyl_neumann(x, y) = BesselY[x, y]
    {
        assert(isnan(cyl_neumann(0.0, nand_v)));
        assert(isnan(cyl_neumannf(0.0f, nanf_v)));
        assert(isnan(cyl_neumannl(0.0L, nanl_v)));
    }
    {
        assert(isnan(cyl_neumann(nand_v, 0.0)));
        assert(isnan(cyl_neumannf(nanf_v, 0.0f)));
        assert(isnan(cyl_neumannl(nanl_v, 0.0L)));
    }
    {
        assert(isclose(cyl_neumann(0.5, 0.333), -1.30'671'255'810'072'199, 2));
        assert(isclose(cyl_neumannf(0.5f, 0.333f), -1.30'671'255'810'072'199f));
        assert(isclose(cyl_neumannl(0.5L, 0.333L), -1.30'671'255'810'072'199L, 2));
    }
    {
        assert(isclose(cyl_neumann(0.5, 0.666), -0.768'760'134'281'402'040));
        assert(isclose(cyl_neumannf(0.5f, 0.666f), -0.768'760'134'281'402'040f));
        assert(isclose(cyl_neumannl(0.5L, 0.666L), -0.768'760'134'281'402'040L));
    }
}

void test_ellint_1() {
    // ellint_1(k, phi) = integrate[(1/sqrt[1 - k^2(sin(t))^2]) dt] from 0 to phi
    {
        assert(isnan(ellint_1(0.0, nand_v)));
        assert(isnan(ellint_1f(0.0f, nanf_v)));
        assert(isnan(ellint_1l(0.0L, nanl_v)));
    }
    {
        assert(isnan(ellint_1(nand_v, 0.0)));
        assert(isnan(ellint_1f(nanf_v, 0.0f)));
        assert(isnan(ellint_1l(nanl_v, 0.0L)));
    }
    {
        assert(isclose(ellint_1(0.0, half_pi<double>), half_pi<double>));
        assert(isclose(ellint_1f(0.0f, half_pi<float>), half_pi<float>));
        assert(isclose(ellint_1l(0.0L, half_pi<long double>), half_pi<long double>));
    }
    {
        assert(isclose(ellint_1(0.7, 0.0), 0.0));
        assert(isclose(ellint_1f(0.7f, 0.0f), 0.0f));
        assert(isclose(ellint_1l(0.7L, 0.0L), 0.0L));
    }
}

void test_ellint_2() {
    // ellint_2(k, phi) = integrate[sqrt[1 - k^2(sin(t))^2] dt] from 0 to phi
    {
        assert(isnan(ellint_2(0.0, nand_v)));
        assert(isnan(ellint_2f(0.0f, nanf_v)));
        assert(isnan(ellint_2l(0.0L, nanl_v)));
    }
    {
        assert(isnan(ellint_2(nand_v, 0.0)));
        assert(isnan(ellint_2f(nanf_v, 0.0f)));
        assert(isnan(ellint_2l(nanl_v, 0.0L)));
    }
    {
        assert(isclose(ellint_2(0.0, half_pi<double>), half_pi<double>));
        assert(isclose(ellint_2f(0.0f, half_pi<float>), half_pi<float>));
        assert(isclose(ellint_2l(0.0L, half_pi<long double>), half_pi<long double>));
    }
    {
        assert(isclose(ellint_2(0.7, 0.0), 0.0));
        assert(isclose(ellint_2f(0.7f, 0.0f), 0.0f));
        assert(isclose(ellint_2l(0.7L, 0.0L), 0.0L));
    }
}

void test_ellint_3() {
    // ellint_3(k, nu, phi) = integrate[(1/([1 - nu (sin(t))^2] sqrt[1 - k^2(sin(t))^2])) dt] from 0 to phi
    {
        assert(isnan(ellint_3(nand_v, 0.0, 0.0)));
        assert(isnan(ellint_3f(nanf_v, 0.0f, 0.0f)));
        assert(isnan(ellint_3l(nanl_v, 0.0L, 0.0L)));
    }
    {
        assert(isnan(ellint_3(0.0, nand_v, 0.0)));
        assert(isnan(ellint_3f(0.0f, nanf_v, 0.0f)));
        assert(isnan(ellint_3l(0.0L, nanl_v, 0.0L)));
    }
    {
        assert(isnan(ellint_3(0.0, 0.0, nand_v)));
        assert(isnan(ellint_3f(0.0f, 0.0f, nanf_v)));
        assert(isnan(ellint_3l(0.0L, 0.0L, nanl_v)));
    }
    {
        assert(isclose(ellint_3(0.0, 0.0, half_pi<double>), half_pi<double>));
        assert(isclose(ellint_3f(0.0f, 0.0f, half_pi<float>), half_pi<float>));
        assert(isclose(ellint_3l(0.0L, 0.0L, half_pi<long double>), half_pi<long double>));
    }
}

void test_expint() {
    // expint(x) = Ei[x]
    {
        assert(isnan(expint(nand_v)));
        assert(isnan(expint(nanf_v)));
        assert(isnan(expint(nanl_v)));
        assert(isnan(expintf(nanf_v)));
        assert(isnan(expintl(nanl_v)));
    }
    {
        assert(isclose(expint(1.0), 1.89'511'781'635'593'675));
        assert(isclose(expint(1.0f), 1.89'511'781'635'593'675f));
        assert(isclose(expint(1.0L), 1.89'511'781'635'593'675L));
        assert(isclose(expintf(1.0f), 1.89'511'781'635'593'675f));
        assert(isclose(expintl(1.0L), 1.89'511'781'635'593'675L));
    }
}

void test_hermite() {
    // hermite(x, y) = Hermite[x, y]
    {
        assert(isnan(hermite(3, nand_v)));
        assert(isnan(hermite(3, nanf_v)));
        assert(isnan(hermite(3, nanl_v)));
        assert(isnan(hermitef(3, nanf_v)));
        assert(isnan(hermitel(3, nanl_v)));
    }
    {
        assert(isclose(hermite(3, 10.0), 7880.0));
        assert(isclose(hermite(3, 10.0f), 7880.0f));
        assert(isclose(hermite(3, 10.0L), 7880.0L));
        assert(isclose(hermitef(3, 10.0f), 7880.0f));
        assert(isclose(hermitel(3, 10.0L), 7880.0L));
    }
    {
        assert(isclose(hermite(4, 10.0), 155212.0));
        assert(isclose(hermite(4, 10.0f), 155212.0f));
        assert(isclose(hermite(4, 10.0L), 155212.0L));
        assert(isclose(hermitef(4, 10.0f), 155212.0f));
        assert(isclose(hermitel(4, 10.0L), 155212.0L));
    }
}

void test_legendre() {
    // legendre(x, y) = LegendreP[x, y]
    {
        assert(isnan(legendre(3, nand_v)));
        assert(isnan(legendre(3, nanf_v)));
        assert(isnan(legendre(3, nanl_v)));
        assert(isnan(legendref(3, nanf_v)));
        assert(isnan(legendrel(3, nanl_v)));
    }
    {
        assert(isclose(legendre(3, 0.25), -0.3359375));
        assert(isclose(legendre(3, 0.25f), -0.3359375f));
        assert(isclose(legendre(3, 0.25L), -0.3359375L));
        assert(isclose(legendref(3, 0.25f), -0.3359375f));
        assert(isclose(legendrel(3, 0.25L), -0.3359375L));
    }
    {
        assert(isclose(legendre(4, 0.25), 0.15771484375));
        assert(isclose(legendre(4, 0.25f), 0.15771484375f));
        assert(isclose(legendre(4, 0.25L), 0.15771484375L));
        assert(isclose(legendref(4, 0.25f), 0.15771484375f));
        assert(isclose(legendrel(4, 0.25L), 0.15771484375L));
    }
}

void test_laguerre() {
    // laguerre(x, y) = LaguerreP[x, y]
    {
        assert(isnan(laguerre(3, nand_v)));
        assert(isnan(laguerre(3, nanf_v)));
        assert(isnan(laguerre(3, nanl_v)));
        assert(isnan(laguerref(3, nanf_v)));
        assert(isnan(laguerrel(3, nanl_v)));
    }
    {
        assert(isclose(laguerre(1, 0.5), 0.5));
        assert(isclose(laguerre(1, 0.5f), 0.5f));
        assert(isclose(laguerre(1, 0.5L), 0.5L));
        assert(isclose(laguerref(1, 0.5f), 0.5f));
        assert(isclose(laguerrel(1, 0.5L), 0.5L));
    }
    {
        assert(isclose(laguerre(2, 0.5), 0.125));
        assert(isclose(laguerre(2, 0.5f), 0.125f));
        assert(isclose(laguerre(2, 0.5L), 0.125L));
        assert(isclose(laguerref(2, 0.5f), 0.125f));
        assert(isclose(laguerrel(2, 0.5L), 0.125L));
    }
}

void test_riemann_zeta() {
    // riemann_zeta(x) = Zeta[x]
    {
        assert(isnan(riemann_zeta(nand_v)));
        assert(isnan(riemann_zeta(nanf_v)));
        assert(isnan(riemann_zeta(nanl_v)));
        assert(isnan(riemann_zetaf(nanf_v)));
        assert(isnan(riemann_zetal(nanl_v)));
    }
    {
        assert(isclose(riemann_zeta(-1.0), -0.0833'333'333'333'333'333));
        assert(isclose(riemann_zeta(-1.0f), -0.0833'333'333'333'333'333f));
        assert(isclose(riemann_zeta(-1.0L), -0.0833'333'333'333'333'333L));
        assert(isclose(riemann_zetaf(-1.0f), -0.0833'333'333'333'333'333f));
        assert(isclose(riemann_zetal(-1.0L), -0.0833'333'333'333'333'333L));
    }
    {
        assert(isclose(riemann_zeta(0.0), -0.5));
        assert(isclose(riemann_zeta(0.0f), -0.5f));
        assert(isclose(riemann_zeta(0.0L), -0.5L));
        assert(isclose(riemann_zetaf(0.0f), -0.5f));
        assert(isclose(riemann_zetal(0.0L), -0.5L));
    }
}

void test_sph_bessel() {
    // sph_bessel(x, y) = sqrt[pi/(2 * x)] BesselJ[y + 1/2, x]
    {
        assert(isnan(sph_bessel(1, nand_v)));
        assert(isnan(sph_bessel(1, nanf_v)));
        assert(isnan(sph_bessel(1, nanl_v)));
        assert(isnan(sph_besself(1, nanf_v)));
        assert(isnan(sph_bessell(1, nanl_v)));
    }
    {
        assert(isclose(sph_bessel(1, 1.2345), 0.352'106'432'958'031'885));
        assert(isclose(sph_bessel(1, 1.2345f), 0.352'106'432'958'031'885f));
        assert(isclose(sph_bessel(1, 1.2345L), 0.352'106'432'958'031'885L));
        assert(isclose(sph_besself(1, 1.2345f), 0.352'106'432'958'031'885f));
        assert(isclose(sph_bessell(1, 1.2345L), 0.352'106'432'958'031'885L));
    }
}

void test_sph_legendre() {
    // sph_legendre(x, y, z)
    // = (-1)^y ([2 * x + 1]/4pi * [x - y]!/[x + y]!)^(1 / 2) LegendreP[x, y, cos(z)]
    {
        assert(isnan(sph_legendre(1, 1, nand_v)));
        assert(isnan(sph_legendre(1, 1, nanf_v)));
        assert(isnan(sph_legendre(1, 1, nanl_v)));
        assert(isnan(sph_legendref(1, 1, nanf_v)));
        assert(isnan(sph_legendrel(1, 1, nanl_v)));
    }
    {
        assert(isclose(sph_legendre(3, 0, 1.2345), -0.302'386'890'645'537'993, 2));
        assert(isclose(sph_legendre(3, 0, 1.2345f), -0.302'386'890'645'537'993f));
        assert(isclose(sph_legendre(3, 0, 1.2345L), -0.302'386'890'645'537'993L, 2));
        assert(isclose(sph_legendref(3, 0, 1.2345f), -0.302'386'890'645'537'993f));
        assert(isclose(sph_legendrel(3, 0, 1.2345L), -0.302'386'890'645'537'993L, 2));
    }
}

void test_sph_neumann() {
    // sph_neumann(x, y) = sqrt[pi/2y] BesselY[x + 1/2, y]
    {
        assert(isnan(sph_neumann(1, nand_v)));
        assert(isnan(sph_neumann(1, nanf_v)));
        assert(isnan(sph_neumann(1, nanl_v)));
        assert(isnan(sph_neumannf(1, nanf_v)));
        assert(isnan(sph_neumannl(1, nanl_v)));
    }
    {
        assert(isclose(sph_neumann(1, 1.2345), -0.981'200'877'819'308'099, 2));
        assert(isclose(sph_neumann(1, 1.2345f), -0.981'200'877'819'308'099f));
        assert(isclose(sph_neumann(1, 1.2345L), -0.981'200'877'819'308'099L, 2));
        assert(isclose(sph_neumannf(1, 1.2345f), -0.981'200'877'819'308'099f));
        assert(isclose(sph_neumannl(1, 1.2345L), -0.981'200'877'819'308'099L, 2));
    }
}

// Also test GH-3076 <cmath>: Invalid output for incomplete elliptic integral of the second kind with k = 1
void test_gh_3076() {
#if !defined(_MSVC_INTERNAL_TESTING) || !defined(_DLL) // TRANSITION, 17.6 Preview 2 unlocked redist
    assert(isclose(ellint_2(1, 6.2831853071795862), 4.0));
#endif // !defined(_MSVC_INTERNAL_TESTING) || !defined(_DLL)
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
    test_legendre();
    test_laguerre();
    test_riemann_zeta();
    test_sph_bessel();
    test_sph_legendre();
    test_sph_neumann();

    test_gh_3076();
}
