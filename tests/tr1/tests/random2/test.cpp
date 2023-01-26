// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

// test <random> header, part 2
#define TEST_NAME "<random>, part 2"

#define _SILENCE_TR1_NAMESPACE_DEPRECATION_WARNING

#include "tdefs.h"
#include <math.h>
#include <random>
#include <sstream>

const double double_p = 1.0 + STD numeric_limits<double>::epsilon();
const double double_m = 1.0 - STD numeric_limits<double>::epsilon();

template <class T>
bool eq(T x0, T x1) {
    return STD numeric_limits<T>::is_integer ? x0 == x1 : STD abs(x0 - x1) < 3 * STD numeric_limits<T>::epsilon();
}

template <class Dist, class Engine = STD minstd_rand>
struct test_globals {
    static void test() {
        Engine rng0, rng1, rng2;
        Dist dist0, dist1, dist2;
        (void) dist0(rng0);
        bool mismatched = false;
        for (int i = 0; i < 10; ++i) {
            (void) dist2(rng2);
            if (dist0(rng0) != dist1(rng1)) {
                mismatched = true;
            }
        }
        CHECK(mismatched);
        STD stringstream str;
        str << dist1 << ' ' << rng1;
        CHECK(eq(dist1(rng1), dist2(rng2)));
        str >> dist0 >> rng0;
        (void) dist0(rng0);
        dist0.reset();
        dist1.reset();
        CHECK(eq(dist0(rng0), dist1(rng1)));
    }
};

static void tuniform_int() {
    typedef STD uniform_int<int> dist_t;
    bool st = STD is_same<dist_t::result_type, int>::value;
    CHECK(st);

    dist_t dist0;
    CHECK_INT(dist0.min(), 0);
    CHECK_INT(dist0.max(), 9);
    dist0.reset();
    dist_t dist1(1);
    CHECK_INT(dist1.min(), 1);
    CHECK_INT(dist1.max(), 9);
    dist_t dist2(2, 8);
    CHECK_INT(dist2.min(), 2);
    CHECK_INT(dist2.max(), 8);
    STD stringstream str;
    str << dist0;
    str >> dist2;
    CHECK_INT(dist2.min(), 0);
    CHECK_INT(dist2.max(), 9);

    test_globals<dist_t>::test();
}

static void tbernoulli_distribution() {
    typedef STD bernoulli_distribution dist_t;
    bool st = STD is_same<dist_t::result_type, bool>::value;
    CHECK(st);

    dist_t dist0;
    CHECK_DOUBLE(dist0.p(), 0.5);
    dist0.reset();
    dist_t dist1(double_m);
    CHECK_DOUBLE(dist1.p(), double_m);
    STD stringstream str;
    str << dist1;
    str >> dist0;
    CHECK_DOUBLE(dist0.p(), double_m);

    dist_t::param_type::distribution_type::param_type par0 = dist1.param();
    CHECK(!(par0 != dist1.param()));
    dist0.param(par0);
    CHECK_DOUBLE(dist0.p(), par0.p());
    CHECK(par0 == dist_t::param_type(dist1.p()));

    typedef STD ranlux24 rng_t;
    rng_t rng;
    CHECK(dist0(rng) != dist0(rng, par0) || true);

    CHECK(dist0.min() <= dist0.max());

    CHECK(dist0 == dist0);
    CHECK(!(dist0 != dist0));

    test_globals<dist_t>::test();
}

static void tpoisson_distribution() {
    typedef STD poisson_distribution<> dist_t;
    bool st = STD is_same<dist_t::result_type, int>::value;
    CHECK(st);

    dist_t dist0;
    CHECK_DOUBLE(dist0.mean(), 1.0);
    dist0.reset();
    dist_t dist1(double_m);
    CHECK_DOUBLE(dist1.mean(), double_m);
    STD stringstream str;
    str << dist1;
    str >> dist0;
    CHECK_DOUBLE(dist0.mean(), double_m);

    dist_t::param_type::distribution_type::param_type par0 = dist1.param();
    CHECK(!(par0 != dist1.param()));
    dist0.param(par0);
    CHECK_DOUBLE(dist0.mean(), par0.mean());
    CHECK(par0 == dist_t::param_type(dist1.mean()));

    typedef STD ranlux24 rng_t;
    rng_t rng;
    CHECK(dist0(rng) != dist0(rng, par0) || true);

    CHECK(dist0.min() <= dist0.max());

    CHECK(dist0 == dist0);
    CHECK(!(dist0 != dist0));

    test_globals<dist_t>::test();
}

static void tgeometric_distribution() {
    typedef STD geometric_distribution<> dist_t;
    bool st = STD is_same<dist_t::result_type, int>::value;
    CHECK(st);

    dist_t dist0;
    CHECK_DOUBLE(dist0.p(), 0.5);
    dist0.reset();
    dist_t dist1(double_m);
    CHECK_DOUBLE(dist1.p(), double_m);
    STD stringstream str;
    str << dist1;
    str >> dist0;
    CHECK_DOUBLE(dist0.p(), double_m);

    dist_t::param_type::distribution_type::param_type par0 = dist1.param();
    CHECK(!(par0 != dist1.param()));
    dist0.param(par0);
    CHECK_DOUBLE(dist0.p(), par0.p());
    CHECK(par0 == dist_t::param_type(dist1.p()));

    typedef STD ranlux24 rng_t;
    rng_t rng;
    CHECK(dist0(rng) != dist0(rng, par0) || true);

    CHECK_INT(dist0.min(), 0);
    CHECK_INT(dist0.max(), STD numeric_limits<int>::max());

    CHECK(dist0 == dist0);
    CHECK(!(dist0 != dist0));

    test_globals<dist_t>::test();
}

static void tbinomial_distribution() {
    typedef STD binomial_distribution<> dist_t;
    bool st = STD is_same<dist_t::result_type, int>::value;
    CHECK(st);

    dist_t dist0;
    CHECK_INT(dist0.t(), 1);
    CHECK_DOUBLE(dist0.p(), 0.5);
    dist0.reset();
    dist_t dist1(2, double_m);
    CHECK_INT(dist1.t(), 2);
    CHECK_DOUBLE(dist1.p(), double_m);
    STD stringstream str;
    str << dist1;
    str >> dist0;
    CHECK_INT(dist0.t(), 2);
    CHECK_DOUBLE(dist0.p(), double_m);

    dist_t::param_type::distribution_type::param_type par0 = dist1.param();
    CHECK(!(par0 != dist1.param()));
    dist0.param(par0);
    CHECK_DOUBLE(dist0.t(), par0.t());
    CHECK_DOUBLE(dist0.p(), par0.p());
    CHECK(par0 == dist_t::param_type(dist1.t(), dist1.p()));

    typedef STD ranlux24 rng_t;
    rng_t rng;
    CHECK(dist0(rng) != dist0(rng, par0) || true);

    CHECK(dist0.min() <= dist0.max());

    CHECK(dist0 == dist0);
    CHECK(!(dist0 != dist0));

    test_globals<dist_t>::test();
}

static void tuniform_real() {
    typedef STD uniform_real<double> dist_t;
    bool st = STD is_same<dist_t::result_type, double>::value;
    CHECK(st);

    dist_t dist0;
    CHECK_DOUBLE(dist0.min(), 0.0);
    CHECK_DOUBLE(dist0.max(), 1.0);
    dist0.reset();
    dist_t dist1(0.25);
    CHECK_DOUBLE(dist1.min(), 0.25);
    CHECK_DOUBLE(dist1.max(), 1.0);
    dist_t dist2(double_m, double_p);
    CHECK_DOUBLE(dist2.min(), double_m);
    CHECK_DOUBLE(dist2.max(), double_p);
    STD stringstream str;
    str << dist2;
    str >> dist0;
    CHECK_DOUBLE(dist0.min(), double_m);
    CHECK_DOUBLE(dist0.max(), double_p);

#if _HAS_TR1_NAMESPACE
    test_globals<dist_t, STD ranlux3_01>::test();
#endif // _HAS_TR1_NAMESPACE
}

static void texponential_distribution() {
    typedef STD exponential_distribution<> dist_t;
    bool st = STD is_same<dist_t::result_type, double>::value;
    CHECK(st);

    dist_t dist0;
    CHECK_DOUBLE(dist0.lambda(), 1.0);
    dist0.reset();
    dist_t dist1(double_p);
    CHECK_DOUBLE(dist1.lambda(), double_p);
    STD stringstream str;
    str << dist1;
    str >> dist0;
    CHECK_DOUBLE(dist0.lambda(), double_p);

    dist_t::param_type::distribution_type::param_type par0 = dist1.param();
    CHECK(!(par0 != dist1.param()));
    dist0.param(par0);
    CHECK_DOUBLE(dist0.lambda(), par0.lambda());
    CHECK(par0 == dist_t::param_type(dist1.lambda()));

    typedef STD ranlux24 rng_t;
    rng_t rng;
    CHECK(dist0(rng) != dist0(rng, par0) || true);

    CHECK(dist0.min() <= dist0.max());

    CHECK(dist0 == dist0);
    CHECK(!(dist0 != dist0));

    test_globals<dist_t>::test();
}

static void tnormal_distribution() {
    typedef STD normal_distribution<> dist_t;
    bool st = STD is_same<dist_t::result_type, double>::value;
    CHECK(st);

    dist_t dist0;
    CHECK_DOUBLE(dist0.mean(), 0.0);
    CHECK_DOUBLE(dist0.sigma(), 1.0);
    dist0.reset();
    dist_t dist1(double_m, double_p);
    CHECK_DOUBLE(dist1.mean(), double_m);
    CHECK_DOUBLE(dist1.sigma(), double_p);
    STD stringstream str;
    str << dist1;
    str >> dist0;
    CHECK_DOUBLE(dist0.mean(), double_m);
    CHECK_DOUBLE(dist0.sigma(), double_p);

    dist_t::param_type::distribution_type::param_type par0 = dist1.param();
    CHECK(!(par0 != dist1.param()));
    dist0.param(par0);
    CHECK_DOUBLE(dist0.mean(), par0.mean());
    CHECK_DOUBLE(dist0.sigma(), par0.sigma());
    CHECK_DOUBLE(dist0.stddev(), par0.stddev());
    CHECK(par0 == dist_t::param_type(dist1.mean(), dist1.sigma()));

    typedef STD ranlux24 rng_t;
    rng_t rng;
    CHECK(dist0(rng) != dist0(rng, par0) || true);

    CHECK(dist0.min() <= dist0.max());

    CHECK(dist0 == dist0);
    CHECK(!(dist0 != dist0));

    test_globals<dist_t>::test();
}

static void tgamma_distribution() {
    typedef STD gamma_distribution<> dist_t;
    bool st = STD is_same<dist_t::result_type, double>::value;
    CHECK(st);

    dist_t dist0;
    CHECK_DOUBLE(dist0.alpha(), 1.0);
    dist0.reset();
    dist_t dist1(double_p);
    CHECK_DOUBLE(dist1.alpha(), double_p);
    STD stringstream str;
    str << dist1;
    str >> dist0;
    CHECK_DOUBLE(dist0.alpha(), double_p);

    dist_t::param_type::distribution_type::param_type par0 = dist1.param();
    CHECK(!(par0 != dist1.param()));
    dist0.param(par0);
    CHECK_DOUBLE(dist0.alpha(), par0.alpha());
    CHECK_DOUBLE(dist0.beta(), par0.beta());
    CHECK(par0 == dist_t::param_type(dist1.alpha(), dist1.beta()));

    typedef STD ranlux24 rng_t;
    rng_t rng;
    CHECK(dist0(rng) != dist0(rng, par0) || true);

    CHECK(dist0.min() <= dist0.max());

    CHECK(dist0 == dist0);
    CHECK(!(dist0 != dist0));

    test_globals<dist_t>::test();
}

static void trandom_device() {
    STD random_device dev;

    CHECK_INT(STD random_device::min(), 0);
    CHECK(0 < STD random_device::max());

    CHECK_INT(dev.min(), 0);
    CHECK(0 < dev.max());
    CHECK(0.0 <= dev.entropy());
    CHECK(dev() != dev() || dev() != dev());
}

void test_main() { // test distributions
    tuniform_int();
    tbernoulli_distribution();
    tgeometric_distribution();
    tpoisson_distribution();
    tbinomial_distribution();
    tuniform_real();
    texponential_distribution();
    tnormal_distribution();
    tgamma_distribution();
    trandom_device();
}
