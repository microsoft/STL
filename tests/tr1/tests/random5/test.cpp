// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

// test <random> C++11 header, part 5
#define TEST_NAME "<random>, part 5"

#define _SILENCE_TR1_NAMESPACE_DEPRECATION_WARNING

#include "tdefs.h"
#include <cmath>
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
    typedef STD uniform_int_distribution<int> dist_t;
    bool st = STD is_same<dist_t::result_type, int>::value;
    CHECK(st);

    dist_t dist0;
    CHECK_INT(dist0.min(), 0);
    CHECK_INT(dist0.max(), STD numeric_limits<int>::max());
    dist0.reset();
    dist_t dist1(1);
    CHECK_INT(dist1.min(), 1);
    CHECK_INT(dist1.max(), STD numeric_limits<int>::max());
    dist_t dist2(2, 8);
    CHECK_INT(dist2.min(), 2);
    CHECK_INT(dist2.max(), 8);
    STD stringstream str;
    str << dist0;
    str >> dist2;
    CHECK_INT(dist2.min(), 0);
    CHECK_INT(dist2.max(), STD numeric_limits<int>::max());

    CHECK(dist0 == dist0);
    CHECK(!(dist0 != dist0));

    dist_t::param_type::distribution_type::param_type par0 = dist1.param();
    CHECK(!(par0 != dist1.param()));
    dist2.param(par0);
    CHECK_INT(dist2.a(), par0.a());
    CHECK_INT(dist2.b(), par0.b());
    CHECK(par0 == dist_t::param_type(dist1.a(), dist1.b()));

    typedef STD ranlux24 rng_t;
    rng_t rng;
    CHECK(dist0(rng) != dist0(rng, par0));

    test_globals<dist_t>::test();
}

static void tuniform_real() {
    typedef STD uniform_real_distribution<double> dist_t;
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

    CHECK(dist0 == dist0);
    CHECK(!(dist0 != dist0));

    dist_t::param_type::distribution_type::param_type par0 = dist1.param();
    CHECK(!(par0 != dist1.param()));
    dist2.param(par0);
    CHECK_INT(dist2.a(), par0.a());
    CHECK_INT(dist2.b(), par0.b());
    CHECK(par0 == dist_t::param_type(dist1.a(), dist1.b()));

    typedef STD ranlux24 rng_t;
    rng_t rng;
    CHECK(dist0(rng) != dist0(rng, par0));

#if _HAS_TR1_NAMESPACE
    test_globals<dist_t, STD ranlux3_01>::test();
#endif // _HAS_TR1_NAMESPACE
}

static void tnegative_binomial() {
    typedef STD negative_binomial_distribution<> dist_t;
    bool st = STD is_same<dist_t::result_type, int>::value;
    CHECK(st);

    dist_t dist0;
    CHECK_INT(dist0.k(), 1);
    CHECK_DOUBLE(dist0.p(), 0.5);
    dist0.reset();
    dist_t dist1(2, double_m);
    CHECK_INT(dist1.k(), 2);
    CHECK_DOUBLE(dist1.p(), double_m);
    STD stringstream str;
    str << dist1;
    str >> dist0;
    CHECK_INT(dist0.k(), 2);
    CHECK_DOUBLE(dist0.p(), double_m);

    dist_t::param_type::distribution_type::param_type par0 = dist1.param();
    CHECK(!(par0 != dist1.param()));
    dist0.param(par0);
    CHECK_DOUBLE(dist0.k(), par0.k());
    CHECK_DOUBLE(dist0.p(), par0.p());
    CHECK(par0 == dist_t::param_type(dist1.k(), dist1.p()));

    typedef STD ranlux24 rng_t;
    rng_t rng;
    CHECK(dist0(rng) != dist0(rng, par0) || true);

    CHECK_INT(dist0.min(), 0);
    CHECK_INT(dist0.max(), STD numeric_limits<int>::max());

    CHECK(dist0 == dist0);
    CHECK(!(dist0 != dist0));

    test_globals<dist_t>::test();
}

static void tweibull() {
    typedef STD weibull_distribution<> dist_t;
    bool st = STD is_same<dist_t::result_type, double>::value;
    CHECK(st);

    dist_t dist0;
    CHECK_DOUBLE(dist0.a(), 1.0);
    CHECK_DOUBLE(dist0.b(), 1.0);
    dist0.reset();
    dist_t dist1(2.0, double_m);
    CHECK_DOUBLE(dist1.a(), 2.0);
    CHECK_DOUBLE(dist1.b(), double_m);
    STD stringstream str;
    str << dist1;
    str >> dist0;
    CHECK_DOUBLE(dist0.a(), 2.0);
    CHECK_DOUBLE(dist0.b(), double_m);

    dist_t::param_type::distribution_type::param_type par0 = dist1.param();
    CHECK(!(par0 != dist1.param()));
    dist0.param(par0);
    CHECK_DOUBLE(dist0.a(), par0.a());
    CHECK_DOUBLE(dist0.b(), par0.b());
    CHECK(par0 == dist_t::param_type(dist1.a(), dist1.b()));

    typedef STD ranlux24 rng_t;
    rng_t rng;
    CHECK(dist0(rng) != dist0(rng, par0) || true);

    CHECK(dist0.min() <= dist0.max());

    CHECK(dist0 == dist0);
    CHECK(!(dist0 != dist0));

    test_globals<dist_t>::test();
}

static void textreme() {
    typedef STD extreme_value_distribution<> dist_t;
    bool st = STD is_same<dist_t::result_type, double>::value;
    CHECK(st);

    dist_t dist0;
    CHECK_DOUBLE(dist0.a(), 0.0);
    CHECK_DOUBLE(dist0.b(), 1.0);
    dist0.reset();
    dist_t dist1(2.0, double_m);
    CHECK_DOUBLE(dist1.a(), 2.0);
    CHECK_DOUBLE(dist1.b(), double_m);
    STD stringstream str;
    str << dist1;
    str >> dist0;
    CHECK_DOUBLE(dist0.a(), 2.0);
    CHECK_DOUBLE(dist0.b(), double_m);

    dist_t::param_type::distribution_type::param_type par0 = dist1.param();
    CHECK(!(par0 != dist1.param()));
    dist0.param(par0);
    CHECK_DOUBLE(dist0.a(), par0.a());
    CHECK_DOUBLE(dist0.b(), par0.b());
    CHECK(par0 == dist_t::param_type(dist1.a(), dist1.b()));

    typedef STD ranlux24 rng_t;
    rng_t rng;
    CHECK(dist0(rng) != dist0(rng, par0) || true);

    CHECK(dist0.min() <= dist0.max());

    CHECK(dist0 == dist0);
    CHECK(!(dist0 != dist0));

    test_globals<dist_t>::test();
}

static void tlognormal() {
    typedef STD lognormal_distribution<> dist_t;
    bool st = STD is_same<dist_t::result_type, double>::value;
    CHECK(st);

    dist_t dist0;
    CHECK_DOUBLE(dist0.m(), 0.0);
    CHECK_DOUBLE(dist0.s(), 1.0);
    dist0.reset();
    dist_t dist1(2.0, double_m);
    CHECK_DOUBLE(dist1.m(), 2.0);
    CHECK_DOUBLE(dist1.s(), double_m);
    STD stringstream str;
    str << dist1;
    str >> dist0;
    CHECK_DOUBLE(dist0.m(), 2.0);
    CHECK_DOUBLE(dist0.s(), double_m);

    dist_t::param_type::distribution_type::param_type par0 = dist1.param();
    CHECK(!(par0 != dist1.param()));
    dist0.param(par0);
    CHECK_DOUBLE(dist0.m(), par0.m());
    CHECK_DOUBLE(dist0.s(), par0.s());
    CHECK(par0 == dist_t::param_type(dist1.m(), dist1.s()));

    typedef STD ranlux24 rng_t;
    rng_t rng;
    CHECK(dist0(rng) != dist0(rng, par0) || true);

    CHECK(dist0.min() <= dist0.max());

    CHECK(dist0 == dist0);
    CHECK(!(dist0 != dist0));

    test_globals<dist_t>::test();
}

static void tchi_squared() {
    typedef STD chi_squared_distribution<> dist_t;
    bool st = STD is_same<dist_t::result_type, double>::value;
    CHECK(st);

    dist_t dist0;
    CHECK_INT(dist0.n(), 1);
    dist0.reset();
    dist_t dist1(2);
    CHECK_INT(dist1.n(), 2);
    STD stringstream str;
    str << dist1;
    str >> dist0;
    CHECK_INT(dist0.n(), 2);

    dist_t::param_type::distribution_type::param_type par0 = dist1.param();
    CHECK(!(par0 != dist1.param()));
    dist0.param(par0);
    CHECK_DOUBLE(dist0.n(), par0.n());
    CHECK(par0 == dist_t::param_type(dist1.n()));

    typedef STD ranlux24 rng_t;
    rng_t rng;
    CHECK(dist0(rng) != dist0(rng, par0) || true);

    CHECK(dist0.min() <= dist0.max());

    CHECK(dist0 == dist0);
    CHECK(!(dist0 != dist0));

    test_globals<dist_t>::test();
}

static void tcauchy() {
    typedef STD cauchy_distribution<> dist_t;
    bool st = STD is_same<dist_t::result_type, double>::value;
    CHECK(st);

    dist_t dist0;
    CHECK_DOUBLE(dist0.a(), 0.0);
    CHECK_DOUBLE(dist0.b(), 1.0);
    dist0.reset();
    dist_t dist1(2.0, double_m);
    CHECK_DOUBLE(dist1.a(), 2.0);
    CHECK_DOUBLE(dist1.b(), double_m);
    STD stringstream str;
    str << dist1;
    str >> dist0;
    CHECK_DOUBLE(dist0.a(), 2.0);
    CHECK_DOUBLE(dist0.b(), double_m);

    dist_t::param_type::distribution_type::param_type par0 = dist1.param();
    CHECK(!(par0 != dist1.param()));
    dist0.param(par0);
    CHECK_DOUBLE(dist0.a(), par0.a());
    CHECK_DOUBLE(dist0.b(), par0.b());
    CHECK(par0 == dist_t::param_type(dist1.a(), dist1.b()));

    typedef STD ranlux24 rng_t;
    rng_t rng;
    CHECK(dist0(rng) != dist0(rng, par0) || true);

    CHECK(dist0.min() <= dist0.max());

    CHECK(dist0 == dist0);
    CHECK(!(dist0 != dist0));

    test_globals<dist_t>::test();
}

static void tfisher_f() {
    typedef STD fisher_f_distribution<> dist_t;
    bool st = STD is_same<dist_t::result_type, double>::value;
    CHECK(st);

    dist_t dist0;
    CHECK_DOUBLE(dist0.m(), 1.0);
    CHECK_DOUBLE(dist0.n(), 1.0);
    dist0.reset();
    dist_t dist1(2, 3);
    CHECK_DOUBLE(dist1.m(), 2.0);
    CHECK_DOUBLE(dist1.n(), 3.0);
    STD stringstream str;
    str << dist1;
    str >> dist0;
    CHECK_DOUBLE(dist0.m(), 2.0);
    CHECK_DOUBLE(dist0.n(), 3.0);

    dist_t::param_type::distribution_type::param_type par0 = dist1.param();
    CHECK(!(par0 != dist1.param()));
    dist0.param(par0);
    CHECK_DOUBLE(dist0.m(), par0.m());
    CHECK_DOUBLE(dist0.n(), par0.n());
    CHECK(par0 == dist_t::param_type(dist1.m(), dist1.n()));

    typedef STD ranlux24 rng_t;
    rng_t rng;
    CHECK(dist0(rng) != dist0(rng, par0) || true);

    CHECK(dist0.min() <= dist0.max());

    CHECK(dist0 == dist0);
    CHECK(!(dist0 != dist0));

    test_globals<dist_t>::test();
}

static void tstudent_t() {
    typedef STD student_t_distribution<> dist_t;
    bool st = STD is_same<dist_t::result_type, double>::value;
    CHECK(st);

    dist_t dist0;
    CHECK_DOUBLE(dist0.n(), 1.0);
    dist0.reset();
    dist_t dist1(2);
    CHECK_DOUBLE(dist1.n(), 2.0);
    STD stringstream str;
    str << dist1;
    str >> dist0;
    CHECK_DOUBLE(dist0.n(), 2.0);

    dist_t::param_type::distribution_type::param_type par0 = dist1.param();
    CHECK(!(par0 != dist1.param()));
    dist0.param(par0);
    CHECK_DOUBLE(dist0.n(), par0.n());
    CHECK(par0 == dist_t::param_type(dist1.n()));

    typedef STD ranlux24 rng_t;
    rng_t rng;
    CHECK(dist0(rng) != dist0(rng, par0) || true);

    CHECK(dist0.min() <= dist0.max());

    CHECK(dist0 == dist0);
    CHECK(!(dist0 != dist0));

    test_globals<dist_t>::test();
}

double myfn(double x) { // sample PDF
    return 2.0 * x;
}

static void tdiscrete() {
    typedef STD discrete_distribution<> dist_t;
    bool st = STD is_same<dist_t::result_type, int>::value;
    CHECK(st);

    dist_t dist0;
    CHECK_INT(dist0.probabilities().size(), 1);
    CHECK_DOUBLE(dist0.probabilities()[0], 1.0);
    dist0.reset();

    STD vector<double> vec(4, 1.0);

    dist_t dist1{1.0, 1.0, 1.0, 1.0};

    CHECK_INT(dist1.probabilities().size(), 4);
    STD stringstream str;
    str << dist1;
    str >> dist0;
    CHECK_INT(dist0.probabilities().size(), 4);

    dist_t dist2(10, 1.0, 2.0, myfn);
    CHECK_INT(dist2.probabilities().size(), 10);

    dist_t::param_type::distribution_type::param_type par0 = dist1.param();
    CHECK(!(par0 != dist1.param()));
    dist0.param(par0);
    CHECK(dist0.probabilities() == par0.probabilities());
    vec = par0.probabilities();

    CHECK(par0 == dist_t::param_type(vec.begin(), vec.end()));

    typedef STD ranlux24 rng_t;
    rng_t rng;
    CHECK(dist0(rng) != dist0(rng, par0) || true);

    CHECK(dist0.min() <= dist0.max());

    CHECK(dist0 == dist0);
    CHECK(!(dist0 != dist0));
}

static void tpiecewise_constant() {
    typedef STD piecewise_constant_distribution<> dist_t;
    bool st = STD is_same<dist_t::result_type, double>::value;
    CHECK(st);

    dist_t dist0;
    CHECK_INT(dist0.densities().size(), 1);
    CHECK_DOUBLE(dist0.densities()[0], 1.0);
    CHECK_INT(dist0.intervals().size(), 2);
    CHECK_DOUBLE(dist0.intervals()[0], 0.0);
    CHECK_DOUBLE(dist0.intervals()[1], 1.0);
    dist0.reset();

    STD vector<double> ends_not_enough = {0.5};
    STD vector<double> vec_not_enough  = {0.5};
    dist_t dist0_2(ends_not_enough.begin(), ends_not_enough.end(), vec_not_enough.begin());
    CHECK_INT(dist0_2.densities().size(), 1);
    CHECK_DOUBLE(dist0_2.densities()[0], 1.0);
    CHECK_INT(dist0_2.intervals().size(), 2);
    CHECK_DOUBLE(dist0_2.intervals()[0], 0.0);
    CHECK_DOUBLE(dist0_2.intervals()[1], 1.0);


    STD vector<double> ends;
    ends.push_back(0.0);
    ends.push_back(1.0);
    ends.push_back(2.0);
    ends.push_back(3.0);
    ends.push_back(4.0);
    STD vector<double> vec(4, 1.0);
    dist_t dist1(ends.begin(), ends.end(), vec.begin());
    CHECK_INT(dist1.densities().size(), 4);
    CHECK_DOUBLE(dist1.densities()[0], 0.25);
    CHECK_INT(dist1.intervals().size(), 5);
    CHECK_DOUBLE(dist1.intervals()[0], 0.0);
    STD stringstream str;
    str << dist1;
    str >> dist0;
    CHECK_INT(dist0.densities().size(), 4);
    CHECK_DOUBLE(dist0.densities()[0], 0.25);

    dist_t dist2(10, 1.0, 2.0, myfn);
    CHECK_INT(dist2.densities().size(), 10);

    STD initializer_list<double> ilist{1.0, 1.5, 2.0, 3.0, 4.0};
    dist_t dist3(ilist, myfn);
    CHECK_INT(dist3.densities().size(), 4);
    CHECK_DOUBLE(dist3.densities()[0], 0.2777777777777778);

    dist_t::param_type::distribution_type::param_type par0 = dist1.param();
    CHECK(!(par0 != dist1.param()));
    dist0.param(par0);
    CHECK(dist0.densities() == par0.densities());
    CHECK(dist0.intervals() == par0.intervals());

    ends = par0.intervals();
    vec  = par0.densities();
    CHECK(par0 == dist_t::param_type(ends.begin(), ends.end(), vec.begin()));

    CHECK(dist2.param() == dist_t::param_type(10, 1.0, 2.0, myfn));

    CHECK(dist3.param() == dist_t::param_type(ilist, myfn));

    typedef STD ranlux24 rng_t;
    rng_t rng;
    CHECK(dist0(rng) != dist0(rng, par0) || true);

    CHECK(dist0.min() <= dist0.max());

    CHECK(dist0 == dist0);
    CHECK(!(dist0 != dist0));
}

static void tpiecewise_linear() {
    typedef STD piecewise_linear_distribution<> dist_t;
    bool st = STD is_same<dist_t::result_type, double>::value;
    CHECK(st);

    dist_t dist0;
    CHECK_INT(dist0.densities().size(), 2);
    CHECK_DOUBLE(dist0.densities()[0], 1.0);
    CHECK_DOUBLE(dist0.densities()[1], 1.0);
    CHECK_INT(dist0.intervals().size(), 2);
    CHECK_DOUBLE(dist0.intervals()[0], 0.0);
    CHECK_DOUBLE(dist0.intervals()[1], 1.0);
    dist0.reset();

    STD vector<double> ends_not_enough = {0.5};
    STD vector<double> vec_not_enough  = {0.5};
    dist_t dist0_2(ends_not_enough.begin(), ends_not_enough.end(), vec_not_enough.begin());
    CHECK_INT(dist0_2.densities().size(), 2);
    CHECK_DOUBLE(dist0_2.densities()[0], 1.0);
    CHECK_DOUBLE(dist0_2.densities()[1], 1.0);
    CHECK_INT(dist0_2.intervals().size(), 2);
    CHECK_DOUBLE(dist0_2.intervals()[0], 0.0);
    CHECK_DOUBLE(dist0_2.intervals()[1], 1.0);

    STD vector<double> ends;
    ends.push_back(0.0);
    ends.push_back(1.0);
    ends.push_back(2.0);
    ends.push_back(3.0);
    ends.push_back(4.0);
    STD vector<double> vec(5, 1.0);
    dist_t dist1(ends.begin(), ends.end(), vec.begin());
    CHECK_INT(dist1.densities().size(), 5);
    CHECK_DOUBLE(dist1.densities()[0], 0.25);
    CHECK_INT(dist1.intervals().size(), 5);
    CHECK_DOUBLE(dist1.intervals()[0], 0.0);
    STD stringstream str;
    str << dist1;
    str >> dist0;
    CHECK_INT(dist0.densities().size(), 5);
    CHECK_DOUBLE(dist0.densities()[0], 0.25);

    dist_t dist2(10, 1.0, 2.0, myfn);
    CHECK_INT(dist2.densities().size(), 11);

    STD initializer_list<double> ilist{1.0, 1.5, 2.0, 3.0, 4.0};
    dist_t dist3(ilist, myfn);
    CHECK_INT(dist3.densities().size(), 5);
    CHECK_DOUBLE(dist3.densities()[0], 0.13333333333333333);

    dist_t::param_type::distribution_type::param_type par0 = dist1.param();
    CHECK(!(par0 != dist1.param()));
    dist0.param(par0);
    CHECK(dist0.densities() == par0.densities());
    CHECK(dist0.intervals() == par0.intervals());

    ends = par0.intervals();
    vec  = par0.densities();
    CHECK(par0 == dist_t::param_type(ends.begin(), ends.end(), vec.begin()));

    CHECK(dist2.param() == dist_t::param_type(10, 1.0, 2.0, myfn));

    CHECK(dist3.param() == dist_t::param_type(ilist, myfn));

    typedef STD ranlux24 rng_t;
    rng_t rng;
    CHECK(dist0(rng) != dist0(rng, par0) || true);

    CHECK(dist0.min() <= dist0.max());

    CHECK(dist0 == dist0);
    CHECK(!(dist0 != dist0));
}

static void tdefault() {
    STD default_random_engine rng;
    STD default_random_engine::result_type val = rng();
    CHECK(val != rng());
}

void test_main() { // test generators
    tuniform_int();
    tuniform_real();
    tnegative_binomial();
    tweibull();
    textreme();
    tlognormal();
    tchi_squared();
    tcauchy();
    tfisher_f();
    tstudent_t();
    tdiscrete();
    tpiecewise_constant();
    tpiecewise_linear();
    tdefault();
}
