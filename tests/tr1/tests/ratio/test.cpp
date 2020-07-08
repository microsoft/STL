// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

// test <ratio> header, C++
#define TEST_NAME "<ratio>"

#include "tdefs.h"
#include <iostream>
#include <ratio>

namespace {
    typedef STD ratio<0, 2> r_0_2;
    typedef STD ratio<1, 2> r_1_2;
    typedef STD ratio<-1, 2> r_m1_2;
    typedef STD ratio<1, -2> r_1_m2;
    typedef STD ratio<-1, -2> r_m1_m2;
    typedef STD ratio<4, 6> r_4_6;
    typedef STD ratio<-4, 6> r_m4_6;
    typedef STD ratio<4, -6> r_4_m6;
    typedef STD ratio<-4, -6> r_m4_m6;

    void t_ratio() { // test ratio template
        CHECK_INT(r_0_2::num, 0);
        CHECK_INT(r_0_2::den, 1);

        CHECK_INT(r_1_2::num, 1);
        CHECK_INT(r_1_2::den, 2);

        CHECK_INT(r_m1_2::num, -1);
        CHECK_INT(r_m1_2::den, 2);

        CHECK_INT(r_1_m2::num, -1);
        CHECK_INT(r_1_m2::den, 2);

        CHECK_INT(r_m1_m2::num, 1);
        CHECK_INT(r_m1_m2::den, 2);

        CHECK_INT(r_4_6::num, 2);
        CHECK_INT(r_4_6::den, 3);

        CHECK_INT(r_m4_6::num, -2);
        CHECK_INT(r_m4_6::den, 3);

        CHECK_INT(r_4_m6::num, -2);
        CHECK_INT(r_4_m6::den, 3);

        CHECK_INT(r_m4_m6::num, 2);
        CHECK_INT(r_m4_m6::den, 3);

        CHECK_TYPE(r_m4_6::type, r_4_m6::type);
    }

    void t_ratio_add() { // test ratio_add template
        typedef STD ratio_add<r_0_2, r_1_2>::type a_0_2_1_2;
        CHECK_INT(a_0_2_1_2::num, 1);
        CHECK_INT(a_0_2_1_2::den, 2);

        typedef STD ratio_add<r_1_2, r_1_2>::type a_1_2_1_2;
        CHECK_INT(a_1_2_1_2::num, 1);
        CHECK_INT(a_1_2_1_2::den, 1);

        typedef STD ratio_add<r_1_2, r_m4_6>::type a_1_2_m4_6;
        CHECK_INT(a_1_2_m4_6::num, -1);
        CHECK_INT(a_1_2_m4_6::den, 6);

        typedef STD ratio_add<r_m1_2, r_4_6>::type a_m1_2_4_6;
        CHECK_INT(a_m1_2_4_6::num, 1);
        CHECK_INT(a_m1_2_4_6::den, 6);

        typedef STD ratio_add<r_m1_2, r_4_6> a_m1_2_4_6_alias;
        typedef STD ratio<1, 6> ans;
        CHECK_TYPE(ans, a_m1_2_4_6_alias);
    }

    void t_ratio_subtract() { // test ratio_subtract template
        typedef STD ratio_subtract<r_0_2, r_1_2>::type s_0_2_1_2;
        CHECK_INT(s_0_2_1_2::num, -1);
        CHECK_INT(s_0_2_1_2::den, 2);

        typedef STD ratio_subtract<r_1_2, r_1_2>::type s_1_2_1_2;
        CHECK_INT(s_1_2_1_2::num, 0);
        CHECK_INT(s_1_2_1_2::den, 1);

        typedef STD ratio_subtract<r_1_2, r_m4_6>::type s_1_2_m4_6;
        CHECK_INT(s_1_2_m4_6::num, 7);
        CHECK_INT(s_1_2_m4_6::den, 6);

        typedef STD ratio_subtract<r_m1_2, r_4_6>::type s_m1_2_4_6;
        CHECK_INT(s_m1_2_4_6::num, -7);
        CHECK_INT(s_m1_2_4_6::den, 6);

        typedef STD ratio_subtract<r_m1_2, r_4_6> s_m1_2_4_6_alias;
        typedef STD ratio<-7, 6> ans;
        CHECK_TYPE(ans, s_m1_2_4_6_alias);
    }

    void t_ratio_multiply() { // test ratio_multiply template
        typedef STD ratio_multiply<r_0_2, r_1_2>::type m_0_2_1_2;
        CHECK_INT(m_0_2_1_2::num, 0);
        CHECK_INT(m_0_2_1_2::den, 1);

        typedef STD ratio_multiply<r_1_2, r_1_2>::type m_1_2_1_2;
        CHECK_INT(m_1_2_1_2::num, 1);
        CHECK_INT(m_1_2_1_2::den, 4);

        typedef STD ratio_multiply<r_1_2, r_m4_6>::type m_1_2_m4_6;
        CHECK_INT(m_1_2_m4_6::num, -1);
        CHECK_INT(m_1_2_m4_6::den, 3);

        typedef STD ratio_multiply<r_m1_2, r_4_6>::type m_m1_2_4_6;
        CHECK_INT(m_m1_2_4_6::num, -1);
        CHECK_INT(m_m1_2_4_6::den, 3);

        typedef STD ratio_multiply<r_m1_2, r_4_6> m_m1_2_4_6_alias;
        typedef STD ratio<-1, 3> ans;
        CHECK_TYPE(ans, m_m1_2_4_6_alias);
    }

    void t_ratio_divide() { // test ratio_divide template
        typedef STD ratio_divide<r_0_2, r_1_2>::type d_0_2_1_2;
        CHECK_INT(d_0_2_1_2::num, 0);
        CHECK_INT(d_0_2_1_2::den, 1);

        typedef STD ratio_divide<r_1_2, r_1_2>::type d_1_2_1_2;
        CHECK_INT(d_1_2_1_2::num, 1);
        CHECK_INT(d_1_2_1_2::den, 1);

        typedef STD ratio_divide<r_1_2, r_m4_6>::type d_1_2_m4_6;
        CHECK_INT(d_1_2_m4_6::num, -3);
        CHECK_INT(d_1_2_m4_6::den, 4);

        typedef STD ratio_divide<r_m1_2, r_4_6>::type d_m1_2_4_6;
        CHECK_INT(d_m1_2_4_6::num, -3);
        CHECK_INT(d_m1_2_4_6::den, 4);

        typedef STD ratio_divide<r_m1_2, r_4_6> d_m1_2_4_6_alias;
        typedef STD ratio<-3, 4> ans;
        CHECK_TYPE(ans, d_m1_2_4_6_alias);
    }

    void t_compare() { // test ratio comparison templates
        typedef STD ratio_equal<r_0_2, r_0_2> eq_0_2_0_2;
        CHECK_INT(eq_0_2_0_2::value, true);
        typedef STD ratio_not_equal<r_0_2, r_0_2> ne_0_2_0_2;
        CHECK_INT(ne_0_2_0_2::value, false);
        typedef STD ratio_less<r_0_2, r_0_2> lt_0_2_0_2;
        CHECK_INT(lt_0_2_0_2::value, false);
        typedef STD ratio_less_equal<r_0_2, r_0_2> le_0_2_0_2;
        CHECK_INT(le_0_2_0_2::value, true);
        typedef STD ratio_greater<r_0_2, r_0_2> gt_0_2_0_2;
        CHECK_INT(gt_0_2_0_2::value, false);
        typedef STD ratio_greater_equal<r_0_2, r_0_2> ge_0_2_0_2;
        CHECK_INT(ge_0_2_0_2::value, true);

        typedef STD ratio_equal<r_1_2, r_1_2> eq_1_2_1_2;
        CHECK_INT(eq_1_2_1_2::value, true);
        typedef STD ratio_not_equal<r_1_2, r_1_2> ne_1_2_1_2;
        CHECK_INT(ne_1_2_1_2::value, false);
        typedef STD ratio_less<r_1_2, r_1_2> lt_1_2_1_2;
        CHECK_INT(lt_1_2_1_2::value, false);
        typedef STD ratio_less_equal<r_1_2, r_1_2> le_1_2_1_2;
        CHECK_INT(le_1_2_1_2::value, true);
        typedef STD ratio_greater<r_1_2, r_1_2> gt_1_2_1_2;
        CHECK_INT(gt_1_2_1_2::value, false);
        typedef STD ratio_greater_equal<r_1_2, r_1_2> ge_1_2_1_2;
        CHECK_INT(ge_1_2_1_2::value, true);

        typedef STD ratio_equal<r_m1_2, r_m1_2> eq_m1_2_m1_2;
        CHECK_INT(eq_m1_2_m1_2::value, true);
        typedef STD ratio_not_equal<r_m1_2, r_m1_2> ne_m1_2_m1_2;
        CHECK_INT(ne_m1_2_m1_2::value, false);
        typedef STD ratio_less<r_m1_2, r_m1_2> lt_m1_2_m1_2;
        CHECK_INT(lt_m1_2_m1_2::value, false);
        typedef STD ratio_less_equal<r_m1_2, r_m1_2> le_m1_2_m1_2;
        CHECK_INT(le_m1_2_m1_2::value, true);
        typedef STD ratio_greater<r_m1_2, r_m1_2> gt_m1_2_m1_2;
        CHECK_INT(gt_m1_2_m1_2::value, false);
        typedef STD ratio_greater_equal<r_m1_2, r_m1_2> ge_m1_2_m1_2;
        CHECK_INT(ge_m1_2_m1_2::value, true);

        typedef STD ratio_equal<r_0_2, r_1_2> eq_0_2_1_2;
        CHECK_INT(eq_0_2_1_2::value, false);
        typedef STD ratio_not_equal<r_0_2, r_1_2> ne_0_2_1_2;
        CHECK_INT(ne_0_2_1_2::value, true);
        typedef STD ratio_less<r_0_2, r_1_2> lt_0_2_1_2;
        CHECK_INT(lt_0_2_1_2::value, true);
        typedef STD ratio_less_equal<r_0_2, r_1_2> le_0_2_1_2;
        CHECK_INT(le_0_2_1_2::value, true);
        typedef STD ratio_greater<r_0_2, r_1_2> gt_0_2_1_2;
        CHECK_INT(gt_0_2_1_2::value, false);
        typedef STD ratio_greater_equal<r_0_2, r_1_2> ge_0_2_1_2;
        CHECK_INT(ge_0_2_1_2::value, false);

        typedef STD ratio_equal<r_0_2, r_m1_2> eq_0_2_m1_2;
        CHECK_INT(eq_0_2_m1_2::value, false);
        typedef STD ratio_not_equal<r_0_2, r_m1_2> ne_0_2_m1_2;
        CHECK_INT(ne_0_2_m1_2::value, true);
        typedef STD ratio_less<r_0_2, r_m1_2> lt_0_2_m1_2;
        CHECK_INT(lt_0_2_m1_2::value, false);
        typedef STD ratio_less_equal<r_0_2, r_m1_2> le_0_2_m1_2;
        CHECK_INT(le_0_2_m1_2::value, false);
        typedef STD ratio_greater<r_0_2, r_m1_2> gt_0_2_m1_2;
        CHECK_INT(gt_0_2_m1_2::value, true);
        typedef STD ratio_greater_equal<r_0_2, r_m1_2> ge_0_2_m1_2;
        CHECK_INT(ge_0_2_m1_2::value, true);

        typedef STD ratio_equal<r_4_6, r_1_2> eq_4_6_1_2;
        CHECK_INT(eq_4_6_1_2::value, false);
        typedef STD ratio_not_equal<r_4_6, r_1_2> ne_4_6_1_2;
        CHECK_INT(ne_4_6_1_2::value, true);
        typedef STD ratio_less<r_4_6, r_1_2> lt_4_6_1_2;
        CHECK_INT(lt_4_6_1_2::value, false);
        typedef STD ratio_less_equal<r_4_6, r_1_2> le_4_6_1_2;
        CHECK_INT(le_4_6_1_2::value, false);
        typedef STD ratio_greater<r_4_6, r_1_2> gt_4_6_1_2;
        CHECK_INT(gt_4_6_1_2::value, true);
        typedef STD ratio_greater_equal<r_4_6, r_1_2> ge_4_6_1_2;
        CHECK_INT(ge_4_6_1_2::value, true);

        typedef STD ratio_equal<r_4_6, r_m1_2> eq_4_6_m1_2;
        CHECK_INT(eq_4_6_m1_2::value, false);
        typedef STD ratio_not_equal<r_4_6, r_m1_2> ne_4_6_m1_2;
        CHECK_INT(ne_4_6_m1_2::value, true);
        typedef STD ratio_less<r_4_6, r_m1_2> lt_4_6_m1_2;
        CHECK_INT(lt_4_6_m1_2::value, false);
        typedef STD ratio_less_equal<r_4_6, r_m1_2> le_4_6_m1_2;
        CHECK_INT(le_4_6_m1_2::value, false);
        typedef STD ratio_greater<r_4_6, r_m1_2> gt_4_6_m1_2;
        CHECK_INT(gt_4_6_m1_2::value, true);
        typedef STD ratio_greater_equal<r_4_6, r_m1_2> ge_4_6_m1_2;
        CHECK_INT(ge_4_6_m1_2::value, true);

        typedef STD ratio_equal<r_m4_6, r_1_2> eq_m4_6_1_2;
        CHECK_INT(eq_m4_6_1_2::value, false);
        typedef STD ratio_not_equal<r_m4_6, r_1_2> ne_m4_6_1_2;
        CHECK_INT(ne_m4_6_1_2::value, true);
        typedef STD ratio_less<r_m4_6, r_1_2> lt_m4_6_1_2;
        CHECK_INT(lt_m4_6_1_2::value, true);
        typedef STD ratio_less_equal<r_m4_6, r_1_2> le_m4_6_1_2;
        CHECK_INT(le_m4_6_1_2::value, true);
        typedef STD ratio_greater<r_m4_6, r_1_2> gt_m4_6_1_2;
        CHECK_INT(gt_m4_6_1_2::value, false);
        typedef STD ratio_greater_equal<r_m4_6, r_1_2> ge_m4_6_1_2;
        CHECK_INT(ge_m4_6_1_2::value, false);

        typedef STD ratio_equal<r_m4_6, r_m1_2> eq_m4_6_m1_2;
        CHECK_INT(eq_m4_6_m1_2::value, false);
        typedef STD ratio_not_equal<r_m4_6, r_m1_2> ne_m4_6_m1_2;
        CHECK_INT(ne_m4_6_m1_2::value, true);
        typedef STD ratio_less<r_m4_6, r_m1_2> lt_m4_6_m1_2;
        CHECK_INT(lt_m4_6_m1_2::value, true);
        typedef STD ratio_less_equal<r_m4_6, r_m1_2> le_m4_6_m1_2;
        CHECK_INT(le_m4_6_m1_2::value, true);
        typedef STD ratio_greater<r_m4_6, r_m1_2> gt_m4_6_m1_2;
        CHECK_INT(gt_m4_6_m1_2::value, false);
        typedef STD ratio_greater_equal<r_m4_6, r_m1_2> ge_m4_6_m1_2;
        CHECK_INT(ge_m4_6_m1_2::value, false);
    }

    void t_constants() {
#if 1000000 <= INTMAX_MAX / 1000000000 / 1000000000
        typedef STD ratio<1, (long long) 1000000 * 1000000000 * 1000000000> yocto_val;
#endif // 1000000 <= INTMAX_MAX / 1000000000 / 1000000000

#if 1000 <= INTMAX_MAX / 1000000000 / 1000000000
        typedef STD ratio<1, (long long) 1000 * 1000000000 * 1000000000> zepto_val;
#endif // 1000 <= INTMAX_MAX / 1000000000000000000

        typedef STD ratio<1, (long long) 1000000000 * 1000000000> atto_val;
        typedef STD ratio<1, (long long) 1000000 * 1000000000> femto_val;
        typedef STD ratio<1, (long long) 1000 * 1000000000> pico_val;
        typedef STD ratio<1, 1000000000> nano_val;
        typedef STD ratio<1, 1000000> micro_val;
        typedef STD ratio<1, 1000> milli_val;
        typedef STD ratio<1, 100> centi_val;
        typedef STD ratio<1, 10> deci_val;
        typedef STD ratio<10, 1> deca_val;
        typedef STD ratio<100, 1> hecto_val;
        typedef STD ratio<1000, 1> kilo_val;
        typedef STD ratio<1000000, 1> mega_val;
        typedef STD ratio<1000000000, 1> giga_val;
        typedef STD ratio<(long long) 1000 * 1000000000, 1> tera_val;
        typedef STD ratio<(long long) 1000000 * 1000000000, 1> peta_val;
        typedef STD ratio<(long long) 1000000000 * 1000000000, 1> exa_val;

#if 1000 <= INTMAX_MAX / 1000000000 / 1000000000
        typedef STD ratio<(long long) 1000 * 1000000000 * 1000000000, 1> zetta_val;
#endif // 1000 <= INTMAX_MAX / 1000000000 * 1000000000

#if 1000000 <= INTMAX_MAX / 1000000000 / 1000000000
        typedef STD ratio<(long long) 1000000 * 1000000000 * 1000000000, 1> yotta_val;
#endif // 1000000 <= INTMAX_MAX / 1000000000000000000

#if 1000000 <= INTMAX_MAX / 1000000000 / 1000000000
        typedef STD ratio_equal<yocto_val, STD yocto> yocto_test;
        CHECK_INT(yocto_test::value, true);
#endif // 1000000 <= INTMAX_MAX / 1000000000 / 1000000000

#if 1000 <= INTMAX_MAX / 1000000000 / 1000000000
        typedef STD ratio_equal<zepto_val, STD zepto> zepto_test;
        CHECK_INT(zepto_test::value, true);
#endif // 1000 <= INTMAX_MAX / 1000000000 / 1000000000

        typedef STD ratio_equal<atto_val, STD atto> atto_test;
        CHECK_INT(atto_test::value, true);

        typedef STD ratio_equal<femto_val, STD femto> femto_test;
        CHECK_INT(femto_test::value, true);

        typedef STD ratio_equal<pico_val, STD pico> pico_test;
        CHECK_INT(pico_test::value, true);

        typedef STD ratio_equal<nano_val, STD nano> nano_test;
        CHECK_INT(nano_test::value, true);

        typedef STD ratio_equal<micro_val, STD micro> micro_test;
        CHECK_INT(micro_test::value, true);

        typedef STD ratio_equal<milli_val, STD milli> milli_test;
        CHECK_INT(milli_test::value, true);

        typedef STD ratio_equal<centi_val, STD centi> centi_test;
        CHECK_INT(centi_test::value, true);

        typedef STD ratio_equal<deci_val, STD deci> deci_test;
        CHECK_INT(deci_test::value, true);

        typedef STD ratio_equal<deca_val, STD deca> deca_test;
        CHECK_INT(deca_test::value, true);

        typedef STD ratio_equal<hecto_val, STD hecto> hecto_test;
        CHECK_INT(hecto_test::value, true);

        typedef STD ratio_equal<kilo_val, STD kilo> kilo_test;
        CHECK_INT(kilo_test::value, true);

        typedef STD ratio_equal<mega_val, STD mega> mega_test;
        CHECK_INT(mega_test::value, true);

        typedef STD ratio_equal<giga_val, STD giga> giga_test;
        CHECK_INT(giga_test::value, true);

        typedef STD ratio_equal<tera_val, STD tera> tera_test;
        CHECK_INT(tera_test::value, true);

        typedef STD ratio_equal<peta_val, STD peta> peta_test;
        CHECK_INT(peta_test::value, true);

        typedef STD ratio_equal<exa_val, STD exa> exa_test;
        CHECK_INT(exa_test::value, true);

#if 1000 <= INTMAX_MAX / 1000000000000000000
        typedef STD ratio_equal<zetta_val, STD zetta> zetta_test;
        CHECK_INT(zetta_test::value, true);
#endif // 1000 <= INTMAX_MAX / 1000000000000000000

#if 1000000 <= INTMAX_MAX / 1000000000000000000
        typedef STD ratio_equal<yotta_val, STD yotta> yotta_test;
        CHECK_INT(yotta_test::value, true);
#endif // 1000000 <= INTMAX_MAX / 1000000000000000000
    }
} // unnamed namespace

void test_main() { // test header <ratio>
    t_ratio();
    t_ratio_add();
    t_ratio_subtract();
    t_ratio_multiply();
    t_ratio_divide();
    t_compare();
    t_constants();
}
