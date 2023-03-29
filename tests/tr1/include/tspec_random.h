// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

// common header for random distribution tests
#include <iostream>
#include <random>
using namespace std;

// COMMON TYPES
#define FLIT(x)                x
#define BINSIZE                100
#define SAMPLESIZE             100000
#define CHI_SQUARED_FAIL_LIMIT 125.0

#define TESTR(fun) do_random_test<fun##_type>::gen_data(fun##_dist, fun##_vals, fun##_smaller_ok, fun##_larger_ok)

typedef struct One_arg { // argument plus return value
    Float_type arg1;
    Float_type ans;
    Float_type sensitivity;
} One_arg;

#include "tspec_random_defs.h"

template <class Ty>
class do_random_test { // template for running a random distribution test
public:
    template <class _Distribution>
    static void gen_data(
        _Distribution& _Dist, One_arg* tab, int smaller_ok, int larger_ok) { // generate random data with given _Dist
                                                                             // and compare against expected results
        Ty rand_value;
        double prob_want[BINSIZE];
        double want_count[BINSIZE];
        int got_count[BINSIZE];
        int over_bin, under_bin, junk_bin;

        // load wanted probabilities from data
        int i;
        for (i = 0; i < 10; ++i) {
            prob_want[(int) tab[i].arg1] = tab[i].ans;
        }

        // collect random sample data from given distribution
        mt19937 gen;
        i = 0;
        for (; i < BINSIZE; ++i) {
            got_count[i] = 0;
        }

        over_bin  = 0;
        under_bin = 0;
        junk_bin  = 0;

        _Dist.reset();
        for (i = 0; i < SAMPLESIZE; i++) { // generate a random number using the given distribution
                                           // and engine and build the histogram with results
            rand_value = _Dist(gen);

            Ty zero = (Ty) 0; // to quiet diagnostics
            if (zero <= rand_value && rand_value < (Ty) BINSIZE) { // increase the count of the proper bin
                got_count[(int) rand_value]++;
            } else if (rand_value < zero) {
                ++under_bin;
            } else if ((Ty) (BINSIZE - 1) < rand_value) {
                ++over_bin;
            } else {
                ++junk_bin;
            }
        }

        int tc0 = 0;
        if (verbose) {
            printf("bin\twant(bin)\tgot(bin)\n");
        }

        // Compute results
        for (i = 0; i < BINSIZE; ++i) { // compute results for each bin
            want_count[i] = (double) (prob_want[i] * SAMPLESIZE);

            if ((int) i == (int) tab[tc0].arg1) {
                ++tc0;
                if (verbose) {
                    printf("%.2d\t%.9g\t%d\n", i, want_count[i], got_count[i]);
                }
            }
        }

        // Do chi squared test
        double chi_squared = 0;
        int tc             = 0;

        for (i = 0; i < BINSIZE; i++) {
            if ((int) i == (int) tab[tc].arg1) {
                ++tc;
                if (0 < want_count[i]) { // avoid divide by zero and count empty bins
                    double want_diff = got_count[i] - (double) want_count[i];
                    chi_squared += (want_diff * want_diff) / (double) want_count[i];
                } else if (0 < got_count[i]) {
                    chi_squared += got_count[i];
                }
            }
        }

        // Check results
        int under_bin_ok = 1;
        int over_bin_ok  = 1;

        if (0 < under_bin && !smaller_ok) {
            under_bin_ok = 0;
        }

        if (0 < over_bin && !larger_ok) {
            over_bin_ok = 0;
        }

        CHECK_MSG("smaller values not ok\n", under_bin_ok);
        CHECK_MSG("larger values not ok\n", over_bin_ok);
        CHECK_MSG("invalid values generated\n", junk_bin == 0);
        CHECK_MSG("chi squared too high\n", chi_squared < CHI_SQUARED_FAIL_LIMIT);
    }
};

void test_random() { // test all of the distributions
    TESTR(bernoulli_distribution);
    TESTR(binomial_distribution);
    TESTR(exponential_distribution);
    TESTR(gamma_distribution);
    TESTR(geometric_distribution);
    TESTR(normal_distribution);
    TESTR(poisson_distribution);
    TESTR(uniform_int);
    TESTR(uniform_real);

    TESTR(cauchy_distribution);
    TESTR(chi_squared_distribution);
    TESTR(extreme_value_distribution);
    TESTR(fisher_f_distribution);
    TESTR(lognormal_distribution);
    TESTR(negative_binomial_distribution);
    TESTR(weibull_distribution);
    TESTR(student_t_distribution);
    TESTR(discrete_distribution);
    TESTR(piecewise_constant_distribution);
    TESTR(piecewise_linear_distribution);
}
