// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <cassert>
#include <cmath>
#include <limits>

using namespace std;

template <typename Float>
void assert_close(const Float f, const Float g) {
    auto eps = 0.000003 * abs(f);
    if (eps == Float{0}) {
        eps = numeric_limits<Float>::min();
    }
    assert(abs(f - g) < eps);
}

constexpr float nanf_v       = numeric_limits<float>::quiet_NaN();
constexpr double nand_v      = numeric_limits<double>::quiet_NaN();
constexpr long double nanl_v = numeric_limits<long double>::quiet_NaN();

void test_assoc_laguerre() {
    {
        assert(isnan(assoc_laguerre(1, 10, nanf_v)));
        assert(isnan(assoc_laguerre(1, 10, nand_v)));
        assert(isnan(assoc_laguerre(1, 10, nanl_v)));
        assert(isnan(assoc_laguerref(1, 10, nanf_v)));
        assert(isnan(assoc_laguerrel(1, 10, nanl_v)));
    }
    {
        assert_close(assoc_laguerre(1, 10, 0.5), 10.5);
        assert_close(assoc_laguerre(1, 10, 0.5f), 10.5f);
        assert_close(assoc_laguerre(1, 10, 0.5L), 10.5L);
        assert_close(assoc_laguerref(1, 10, 0.5f), 10.5f);
        assert_close(assoc_laguerrel(1, 10, 0.5L), 10.5L);
    }

    {
        assert_close(assoc_laguerre(2, 10, 0.5), 60.125);
        assert_close(assoc_laguerre(2, 10, 0.5f), 60.125f);
        assert_close(assoc_laguerre(2, 10, 0.5L), 60.125L);
        assert_close(assoc_laguerref(2, 10, 0.5f), 60.125f);
        assert_close(assoc_laguerrel(2, 10, 0.5L), 60.125L);
    }
}

void test_assoc_legendre() {
    {
        assert_close(assoc_legendre(2, 0, 0.5), -0.125);
        assert_close(assoc_legendre(2, 0, 0.5f), -0.125f);
        assert_close(assoc_legendre(2, 0, 0.5L), -0.125L);
        assert_close(assoc_legendref(2, 0, 0.5f), -0.125f);
        assert_close(assoc_legendrel(2, 0, 0.5L), -0.125L);
    }
    {
        assert_close(assoc_legendre(2, 1, 0.5), 1.29903815);
        assert_close(assoc_legendre(2, 1, 0.5f), 1.29903815f);
        assert_close(assoc_legendre(2, 1, 0.5L), 1.29903815L);
        assert_close(assoc_legendref(2, 1, 0.5f), 1.29903815f);
        assert_close(assoc_legendrel(2, 1, 0.5L), 1.29903815L);
    }
    {
        assert_close(assoc_legendre(2, 2, 0.5), 2.25);
        assert_close(assoc_legendre(2, 2, 0.5f), 2.25f);
        assert_close(assoc_legendre(2, 2, 0.5L), 2.25L);
        assert_close(assoc_legendref(2, 2, 0.5f), 2.25f);
        assert_close(assoc_legendrel(2, 2, 0.5L), 2.25L);
    }
}

void test_beta() {
    {
        assert_close(beta(1.0, 1.0), 1.0);
        assert_close(beta(1.0f, 1.0f), 1.0f);
        assert_close(beta(1.0L, 1.0L), 1.0L);
    }
    {
        assert_close(beta(2.0, 3.0), 0.083333333);
        assert_close(beta(2.0f, 3.0f), 0.083333333f);
        assert_close(beta(2.0L, 3.0L), 0.083333333L);
    }
}

void test_comp_ellint_1() {
    {
        assert_close(comp_ellint_1(0.0), 1.5708);
        assert_close(comp_ellint_1(0.0f), 1.5708f);
        assert_close(comp_ellint_1(0.0L), 1.5708L);
        assert_close(comp_ellint_1f(0.0f), 1.5708f);
        assert_close(comp_ellint_1l(0.0L), 1.5708L);
    }
    {
        assert_close(comp_ellint_1(0.5), 1.68575);
        assert_close(comp_ellint_1(0.5f), 1.68575f);
        assert_close(comp_ellint_1(0.5L), 1.68575L);
        assert_close(comp_ellint_1f(0.5f), 1.68575f);
        assert_close(comp_ellint_1l(0.5L), 1.68575L);
    }
}

void test_comp_ellint_2() {
    {
        assert(isnan(comp_ellint_2(nanf_v)));
        assert(isnan(comp_ellint_2(nand_v)));
        assert(isnan(comp_ellint_2(nanl_v)));
        assert(isnan(comp_ellint_2f(nanf_v)));
        assert(isnan(comp_ellint_2l(nanl_v)));
    }
    {
        assert_close(comp_ellint_2(0.0), 1.5708);
        assert_close(comp_ellint_2(0.0f), 1.5708f);
        assert_close(comp_ellint_2(0.0L), 1.5708L);
        assert_close(comp_ellint_2f(0.0f), 1.5708f);
        assert_close(comp_ellint_2l(0.0L), 1.5708L);
    }
    {
        assert_close(comp_ellint_2(1), 1.0);
        assert_close(comp_ellint_2(1.0f), 1.0f);
        assert_close(comp_ellint_2(1.0L), 1.0L);
        assert_close(comp_ellint_2f(1.0f), 1.0f);
        assert_close(comp_ellint_2l(1.0L), 1.0L);
    }
}

void test_comp_ellint_3() {
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
        assert_close(comp_ellint_3(0.0, 0.0), 1.5708);
        assert_close(comp_ellint_3(0.0f, 0.0f), 1.5708f);
        assert_close(comp_ellint_3(0.0L, 0.0L), 1.5708L);
        assert_close(comp_ellint_3f(0.0f, 0.0f), 1.5708f);
        assert_close(comp_ellint_3l(0.0L, 0.0L), 1.5708L);
    }
    {
        assert_close(comp_ellint_3(0.5, 0.0), 1.68575);
        assert_close(comp_ellint_3(0.5f, 0.0f), 1.68575f);
        assert_close(comp_ellint_3(0.5L, 0.0L), 1.68575L);
        assert_close(comp_ellint_3f(0.5f, 0.0f), 1.68575f);
        assert_close(comp_ellint_3l(0.5L, 0.0L), 1.68575L);
    }
}

void test_cyl_bessel_i() {
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
        assert_close(cyl_bessel_i(0.0, 1.2345), 1.41886);
        assert_close(cyl_bessel_if(0.0f, 1.2345f), 1.41886f);
        assert_close(cyl_bessel_il(0.0L, 1.2345L), 1.41886L);
    }
}

void test_cyl_bessel_j() {
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
        assert_close(cyl_bessel_j(0.0, 1.2345), 0.653792);
        assert_close(cyl_bessel_jf(0.0f, 1.2345f), 0.653792f);
        assert_close(cyl_bessel_jl(0.0L, 1.2345L), 0.653792L);
    }
}

void test_cyl_bessel_k() {
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
        assert_close(cyl_bessel_k(0.5, 1.2345), 0.32823);
        assert_close(cyl_bessel_kf(0.5f, 1.2345f), 0.32823f);
        assert_close(cyl_bessel_kl(0.5L, 1.2345L), 0.32823L);
    }
}

void test_cyl_neumann() {
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
        assert_close(cyl_neumann(0.5, 0.333), -1.306713);
        assert_close(cyl_neumannf(0.5f, 0.333f), -1.306713f);
        assert_close(cyl_neumannl(0.5L, 0.333L), -1.306713L);
    }
    {
        assert_close(cyl_neumann(0.5, 0.666), -0.768760);
        assert_close(cyl_neumannf(0.5f, 0.666f), -0.768760f);
        assert_close(cyl_neumannl(0.5L, 0.666L), -0.768760L);
    }
}

void test_ellint_1() {
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
        const long double hpi = acos(-1.0L) / 2.0L;
        assert_close(ellint_1(0.0, static_cast<double>(hpi)), 1.5708);
        assert_close(ellint_1f(0.0f, static_cast<float>(hpi)), 1.5708f);
        assert_close(ellint_1l(0.0L, hpi), 1.5708L);
    }
    {
        assert_close(ellint_1(0.7, 0.0), 0.0);
        assert_close(ellint_1f(0.7f, 0.0f), 0.0f);
        assert_close(ellint_1l(0.7L, 0.0L), 0.0L);
    }
}

void test_ellint_2() {
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
        const long double hpi = acos(-1.0L) / 2.0L;
        assert_close(ellint_2(0.0, static_cast<double>(hpi)), 1.5708);
        assert_close(ellint_2f(0.0f, static_cast<float>(hpi)), 1.5708f);
        assert_close(ellint_2l(0.0L, hpi), 1.5708L);
    }
    {
        assert_close(ellint_2(0.7, 0.0), 0.0);
        assert_close(ellint_2f(0.7f, 0.0f), 0.0f);
        assert_close(ellint_2l(0.7L, 0.0L), 0.0L);
    }
}

void test_ellint_3() {
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
        const long double hpi = acos(-1.0L) / 2.0L;
        assert_close(ellint_3(0.0, 0.0, static_cast<double>(hpi)), 1.5708);
        assert_close(ellint_3f(0.0f, 0.0f, static_cast<float>(hpi)), 1.5708f);
        assert_close(ellint_3l(0.0L, 0.0L, hpi), 1.5708L);
    }
}

void test_expint() {
    {
        assert(isnan(expint(nand_v)));
        assert(isnan(expint(nanf_v)));
        assert(isnan(expint(nanl_v)));
        assert(isnan(expintf(nanf_v)));
        assert(isnan(expintl(nanl_v)));
    }
    {
        assert_close(expint(1.0), 1.89512);
        assert_close(expint(1.0f), 1.89512f);
        assert_close(expint(1.0L), 1.89512L);
        assert_close(expintf(1.0f), 1.89512f);
        assert_close(expintl(1.0L), 1.89512L);
    }
}

void test_hermite() {
    {
        assert(isnan(hermite(3, nand_v)));
        assert(isnan(hermite(3, nanf_v)));
        assert(isnan(hermite(3, nanl_v)));
        assert(isnan(hermitef(3, nanf_v)));
        assert(isnan(hermitel(3, nanl_v)));
    }
    {
        assert_close(hermite(3, 10.0), 7880.0);
        assert_close(hermite(3, 10.0f), 7880.0f);
        assert_close(hermite(3, 10.0L), 7880.0L);
        assert_close(hermitef(3, 10.0f), 7880.0f);
        assert_close(hermitel(3, 10.0L), 7880.0L);
    }
    {
        assert_close(hermite(4, 10.0), 155212.0);
        assert_close(hermite(4, 10.0f), 155212.0f);
        assert_close(hermite(4, 10.0L), 155212.0L);
        assert_close(hermitef(4, 10.0f), 155212.0f);
        assert_close(hermitel(4, 10.0L), 155212.0L);
    }
}

void test_legendre() {
    {
        assert(isnan(legendre(3, nand_v)));
        assert(isnan(legendre(3, nanf_v)));
        assert(isnan(legendre(3, nanl_v)));
        assert(isnan(legendref(3, nanf_v)));
        assert(isnan(legendrel(3, nanl_v)));
    }
    {
        assert_close(legendre(3, 0.25), -0.335938);
        assert_close(legendre(3, 0.25f), -0.335938f);
        assert_close(legendre(3, 0.25L), -0.335938L);
        assert_close(legendref(3, 0.25f), -0.335938f);
        assert_close(legendrel(3, 0.25L), -0.335938L);
    }
    {
        assert_close(legendre(4, 0.25), 0.157715);
        assert_close(legendre(4, 0.25f), 0.157715f);
        assert_close(legendre(4, 0.25L), 0.157715L);
        assert_close(legendref(4, 0.25f), 0.157715f);
        assert_close(legendrel(4, 0.25L), 0.157715L);
    }
}

void test_laguerre() {
    {
        assert(isnan(laguerre(3, nand_v)));
        assert(isnan(laguerre(3, nanf_v)));
        assert(isnan(laguerre(3, nanl_v)));
        assert(isnan(laguerref(3, nanf_v)));
        assert(isnan(laguerrel(3, nanl_v)));
    }
    {
        assert_close(laguerre(1, 0.5), 0.5);
        assert_close(laguerre(1, 0.5f), 0.5f);
        assert_close(laguerre(1, 0.5L), 0.5L);
        assert_close(laguerref(1, 0.5f), 0.5f);
        assert_close(laguerrel(1, 0.5L), 0.5L);
    }
    {
        assert_close(laguerre(2, 0.5), 0.125);
        assert_close(laguerre(2, 0.5f), 0.125f);
        assert_close(laguerre(2, 0.5L), 0.125L);
        assert_close(laguerref(2, 0.5f), 0.125f);
        assert_close(laguerrel(2, 0.5L), 0.125L);
    }
}

void test_riemann_zeta() {
    {
        assert(isnan(riemann_zeta(nand_v)));
        assert(isnan(riemann_zeta(nanf_v)));
        assert(isnan(riemann_zeta(nanl_v)));
        assert(isnan(riemann_zetaf(nanf_v)));
        assert(isnan(riemann_zetal(nanl_v)));
    }
    {
        assert_close(riemann_zeta(-1.0), -0.0833333);
        assert_close(riemann_zeta(-1.0f), -0.0833333f);
        assert_close(riemann_zeta(-1.0L), -0.0833333L);
        assert_close(riemann_zetaf(-1.0f), -0.0833333f);
        assert_close(riemann_zetal(-1.0L), -0.0833333L);
    }
    {
        assert_close(riemann_zeta(0.0), -0.5);
        assert_close(riemann_zeta(0.0f), -0.5f);
        assert_close(riemann_zeta(0.0L), -0.5L);
        assert_close(riemann_zetaf(0.0f), -0.5f);
        assert_close(riemann_zetal(0.0L), -0.5L);
    }
}

void test_sph_bessel() {
    {
        assert(isnan(sph_bessel(1, nand_v)));
        assert(isnan(sph_bessel(1, nanf_v)));
        assert(isnan(sph_bessel(1, nanl_v)));
        assert(isnan(sph_besself(1, nanf_v)));
        assert(isnan(sph_bessell(1, nanl_v)));
    }
    {
        assert_close(sph_bessel(1, 1.2345), 0.352106);
        assert_close(sph_bessel(1, 1.2345f), 0.352106f);
        assert_close(sph_bessel(1, 1.2345L), 0.352106L);
        assert_close(sph_bessel(1, 1.2345f), 0.352106f);
        assert_close(sph_bessel(1, 1.2345L), 0.352106L);
    }
}

void test_sph_legendre() {
    {
        assert(isnan(sph_legendre(1, 1, nand_v)));
        assert(isnan(sph_legendre(1, 1, nanf_v)));
        assert(isnan(sph_legendre(1, 1, nanl_v)));
        assert(isnan(sph_legendref(1, 1, nanf_v)));
        assert(isnan(sph_legendrel(1, 1, nanl_v)));
    }
    {
        assert_close(sph_legendre(3, 0, 1.2345), -0.302387);
        assert_close(sph_legendre(3, 0, 1.2345f), -0.302387f);
        assert_close(sph_legendre(3, 0, 1.2345L), -0.302387L);
        assert_close(sph_legendre(3, 0, 1.2345f), -0.302387f);
        assert_close(sph_legendre(3, 0, 1.2345L), -0.302387L);
    }
}

void test_sph_neumann() {
    {
        assert(isnan(sph_neumann(1, nand_v)));
        assert(isnan(sph_neumann(1, nanf_v)));
        assert(isnan(sph_neumann(1, nanl_v)));
        assert(isnan(sph_neumannf(1, nanf_v)));
        assert(isnan(sph_neumannl(1, nanl_v)));
    }
    {
        assert_close(sph_neumann(1, 1.2345), -0.981201);
        assert_close(sph_neumann(1, 1.2345f), -0.981201f);
        assert_close(sph_neumann(1, 1.2345L), -0.981201L);
        assert_close(sph_neumann(1, 1.2345f), -0.981201f);
        assert_close(sph_neumann(1, 1.2345L), -0.981201L);
    }
}

// Also test GH-3076 <cmath>: Invalid output for incomplete elliptic integral of the second kind with k = 1
void test_gh_3076() {
    assert_close(ellint_2(1, 6.2831853071795862), 4.0);
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
