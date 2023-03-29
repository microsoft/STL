// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

// test <chrono> C++11 header
#define TEST_NAME "<chrono>"

#include "tdefs.h"
#include <chrono>
#include <iostream>
#include <time.h>
#include <typeinfo>

namespace {

    void t_common_type() {
        typedef STD common_type<signed char, unsigned char> c_sc_uc;
        CHECK_TYPE(c_sc_uc::type, int);
        typedef STD common_type<int, float> c_i_f;
        CHECK_TYPE(c_i_f::type, float);
        typedef STD common_type<long long, unsigned long long> c_ll_ull;
        CHECK_TYPE(c_ll_ull::type, unsigned long long);

        typedef STD chrono::duration<int, STD ratio<2, 3>> di;
        typedef STD chrono::duration<long, STD ratio<4, 9>> dl;
        typedef STD ratio<2, 9> r;
        typedef STD common_type<di, dl> c_di_dl;
        CHECK_TYPE(c_di_dl::type::rep, long);
        CHECK_TYPE(c_di_dl::type::period, r);

        typedef STD chrono::time_point<STD chrono::system_clock, di> ti;
        typedef STD chrono::time_point<STD chrono::system_clock, dl> tl;
        typedef STD chrono::time_point<STD chrono::system_clock, c_di_dl::type> res;
        typedef STD common_type<ti, tl> c_ti_tl;
        CHECK_TYPE(c_ti_tl::type, res);
    }

    void t_treat_as_floating_point() {
        CHECK_INT(STD chrono::treat_as_floating_point<char>::value, false);
        CHECK_INT(STD chrono::treat_as_floating_point<short>::value, false);
        CHECK_INT(STD chrono::treat_as_floating_point<unsigned int>::value, false);
        CHECK_INT(STD chrono::treat_as_floating_point<int>::value, false);
        CHECK_INT(STD chrono::treat_as_floating_point<long>::value, false);
        CHECK_INT(STD chrono::treat_as_floating_point<unsigned long>::value, false);
        CHECK_INT(STD chrono::treat_as_floating_point<float>::value, true);
        CHECK_INT(STD chrono::treat_as_floating_point<double>::value, true);
        CHECK_INT(STD chrono::treat_as_floating_point<long double>::value, true);
    }

    void t_duration_values() {
        typedef STD chrono::duration_values<int> d_i;
        CHECK_INT(d_i::zero(), 0);
        CHECK_INT(d_i::min(), STD numeric_limits<int>::lowest());
        CHECK_INT(d_i::max(), STD numeric_limits<int>::max());

        typedef STD chrono::duration_values<double> d_d;
        CHECK_DOUBLE(d_d::zero(), 0.0);
        CHECK_DOUBLE(d_d::min(), STD numeric_limits<double>::lowest());
        CHECK_DOUBLE(d_d::max(), STD numeric_limits<double>::max());
    }

    template <class Rep, class Period>
    void test_arithmetic_op(const STD chrono::duration<Rep, Period>& d1, const STD chrono::duration<Rep, Period>& d2) {
        CHECK_INT(d1.count(), d2.count());
    }

    void t_duration() {
        // nested typedefs
        typedef STD ratio<1, 1> r_1_1;
        typedef STD chrono::duration<int, r_1_1> d_i_r_1_1;
        CHECK_TYPE(d_i_r_1_1::rep, int);
        CHECK_TYPE(d_i_r_1_1::period, r_1_1);

        typedef STD ratio<1, 60> r_1_60;
        typedef STD chrono::duration<long, r_1_60> d_l_r_1_60;
        CHECK_TYPE(d_l_r_1_60::rep, long);
        CHECK_TYPE(d_l_r_1_60::period, r_1_60);

        // constructors
        d_i_r_1_1 d0(0L);
        CHECK_INT(d0.count(), 0);
        d_i_r_1_1 d1(1L);
        CHECK_INT(d1.count(), 1);

        d_l_r_1_60 d2(d1);
        CHECK_INT(d2.count(), 60);

        // copy and assign
        d_i_r_1_1 d3(d1);
        CHECK_INT(d3.count(), d1.count());
        d3 = d0;
        CHECK_INT(d3.count(), d0.count());

        // arithmetic member functions
        CHECK_INT((+d1).count(), d1.count());
        CHECK_INT(d1.count(), 1);
        CHECK_INT((-d1).count(), -d1.count());
        CHECK_INT(d1.count(), 1);

        CHECK_INT((++d1).count(), 2);
        CHECK_INT(d1.count(), 2);
        CHECK_INT((d1++).count(), 2);
        CHECK_INT(d1.count(), 3);

        CHECK_INT((--d1).count(), 2);
        CHECK_INT(d1.count(), 2);
        CHECK_INT((d1--).count(), 2);
        CHECK_INT(d1.count(), 1);

        d3 += d1;
        CHECK_INT(d3.count(), 1);
        d3 -= d1;
        CHECK_INT(d3.count(), 0);

        d1 *= 4;
        CHECK_INT(d1.count(), 4);
        d1 /= 2;
        CHECK_INT(d1.count(), 2);
        d1 %= 2;
        CHECK_INT(d1.count(), 0);

        // arithmetic non-member functions
        typedef STD chrono::duration<int, STD ratio<2, 3>> di;
        typedef STD chrono::duration<long, STD ratio<4, 9>> dl;
        di d4(3);
        dl d5(4);
        STD chrono::duration<long, STD ratio<2, 9>> res_plus(17);
        test_arithmetic_op(d4 + d5, res_plus);
        STD chrono::duration<long, STD ratio<2, 9>> res_minus(1);
        test_arithmetic_op(d4 - d5, res_minus);

        STD chrono::duration<long, STD ratio<2, 3>> res_times(6);
        test_arithmetic_op(d4 * 2L, res_times);
        test_arithmetic_op(2L * d4, res_times);

        STD chrono::duration<long, STD ratio<2, 3>> res_divide0(1);
        test_arithmetic_op(d4 / 3L, res_divide0);
        CHECK_TYPE(d4 / d5, long);
        CHECK_INT(d4 / d5, 1);

        // comparisons
        di d6(6);
        dl d7(9);
        CHECK_INT(d6 == d7, true);
        CHECK_INT(d4 == d5, false);
        CHECK_INT(d6 != d7, false);
        CHECK_INT(d4 != d5, true);

        CHECK_INT(d6 < d7, false);
        CHECK_INT(d4 < d5, false);
        CHECK_INT(d5 < d4, true);
        CHECK_INT(d6 <= d7, true);
        CHECK_INT(d4 <= d5, false);
        CHECK_INT(d5 <= d4, true);

        CHECK_INT(d6 > d7, false);
        CHECK_INT(d4 > d5, true);
        CHECK_INT(d5 > d4, false);
        CHECK_INT(d6 >= d7, true);
        CHECK_INT(d4 >= d5, true);
        CHECK_INT(d5 >= d4, false);

        // special values
        typedef STD chrono::duration_values<int> dvi;
        CHECK_INT(di::zero().count(), dvi::zero());
        CHECK_INT(di::min().count(), dvi::min());
        CHECK_INT(di::max().count(), dvi::max());

        // duration_cast
        d5 = STD chrono::duration_cast<dl>(d4);
        CHECK_INT(d5.count(), 4);

        // convenience typedefs
        STD chrono::minutes mins = STD chrono::duration_cast<STD chrono::minutes>(STD chrono::hours(1));
        CHECK_INT(mins.count(), 60);
        STD chrono::seconds secs = STD chrono::duration_cast<STD chrono::seconds>(STD chrono::minutes(1));
        CHECK_INT(secs.count(), 60);
        STD chrono::milliseconds msecs = STD chrono::duration_cast<STD chrono::milliseconds>(STD chrono::seconds(1));
        CHECK_INT(msecs.count(), 1000);
        STD chrono::microseconds mmsecs =
            STD chrono::duration_cast<STD chrono::microseconds>(STD chrono::milliseconds(1));
        CHECK_INT(mmsecs.count(), 1000);
    }

    template <class Clock>
    void test_clock(bool tp_known, bool steady_known, bool steady) {
        typedef STD chrono::duration<typename Clock::rep, typename Clock::period> dur;
        CHECK_TYPE(typename Clock::duration, dur);
        typedef STD chrono::time_point<Clock, typename Clock::duration> tp;
        if (tp_known) {
            CHECK_TYPE(typename Clock::time_point, tp);
        }
        static_assert(STD is_same_v<decltype(Clock::is_steady), const bool>, "incorrect is_steady type");
        if (steady_known) {
            CHECK_INT(Clock::is_steady, steady);
        }
        CHECK_INT(Clock::now() < Clock::time_point::max(), true);
    }

    void t_clocks() {
        test_clock<STD chrono::high_resolution_clock>(false, false, false);
        test_clock<STD chrono::system_clock>(true, false, false);
        test_clock<STD chrono::steady_clock>(false, true, true);
        CHECK_INT(STD chrono::system_clock::duration::min() <= STD chrono::system_clock::duration::zero(), true);

        time_t t = time(nullptr);

        STD chrono::system_clock::time_point tp = STD chrono::system_clock::from_time_t(t);
        CHECK_INT(t, STD chrono::system_clock::to_time_t(tp));
    }

    struct my_clock {
        typedef int rep;
        typedef STD ratio<2, 1> period;
        typedef STD chrono::duration<rep, period> duration;
        typedef STD chrono::time_point<my_clock> time_point;
        static const bool is_steady = false;

        static time_point now() {
            return time_point(duration(3));
        }
    };

    void t_time_point() {
        typedef long rep;
        typedef STD ratio<1, 1> r_1_1;
        typedef STD chrono::duration<rep, r_1_1> dur;
        typedef STD chrono::duration<int, STD ratio<2, 1>> dur1;
        typedef STD chrono::time_point<my_clock, dur> tp;
        typedef STD chrono::time_point<my_clock, dur1> tp1;

        // nested typedefs
        CHECK_TYPE(tp::clock, my_clock);
        CHECK_TYPE(tp::duration, dur);
        CHECK_TYPE(tp::rep, rep);
        CHECK_TYPE(tp::period, r_1_1);

        bool is_steady = my_clock::is_steady; // to quiet diagnostics
        if (is_steady) {
            my_clock::now();
        }

        // constructors, time_since_epoch
        tp t0;
        CHECK_INT(t0.time_since_epoch() == tp::duration::zero(), true);
        dur d1(1);
        tp t1(d1);
        CHECK_INT(t1.time_since_epoch() == d1, true);
        STD chrono::minutes d2(1);
        tp t2(d2);
        CHECK_INT(t2.time_since_epoch() == d2, true);

        // arithmetic member functions
        t1 += dur1(1);
        CHECK_INT(t1.time_since_epoch() == dur(3), true);
        t1 -= dur1(1);
        CHECK_INT(t1.time_since_epoch() == dur(1), true);

        // arithmetic member functions
        CHECK_INT((t1 + dur1(1)).time_since_epoch() == dur(3), true);
        CHECK_INT((dur1(1) + t1).time_since_epoch() == dur(3), true);
        CHECK_INT((t1 - dur1(1)).time_since_epoch() == dur(-1), true);
        tp1 t3(dur1(2));
        CHECK_INT(t1 - t3 == dur(-3), true);

        // comparisons
        tp t4(dur(1));
        tp t5(dur(2));
        tp1 t6(dur1(1));
        CHECK_INT(t5 == t6, true);
        CHECK_INT(t4 == t6, false);
        CHECK_INT(t5 != t6, false);
        CHECK_INT(t4 != t6, true);

        CHECK_INT(t5 < t6, false);
        CHECK_INT(t4 < t6, true);
        CHECK_INT(t5 < t4, false);
        CHECK_INT(t5 <= t6, true);
        CHECK_INT(t4 <= t6, true);
        CHECK_INT(t5 <= t4, false);

        CHECK_INT(t5 > t6, false);
        CHECK_INT(t4 > t6, false);
        CHECK_INT(t5 > t4, true);
        CHECK_INT(t5 >= t6, true);
        CHECK_INT(t4 >= t6, false);
        CHECK_INT(t5 >= t4, true);

        // special values
        CHECK_INT(tp::min() == tp(dur::min()), true);
        CHECK_INT(tp::max() == tp(dur::max()), true);

        // time_point_cast
        CHECK_INT(STD chrono::time_point_cast<dur1>(t5) == t6, true);

        // time_t conversions
        time_t t = time(nullptr);
        if (t != -1) {
            STD chrono::system_clock::time_point now = STD chrono::system_clock::from_time_t(t);
            CHECK_INT(t == STD chrono::system_clock::to_time_t(now), true);
        }
    }

} // unnamed namespace

void test_main() {
    t_common_type();
    t_treat_as_floating_point();
    t_duration_values();
    t_duration();
    t_clocks();
    t_time_point();
}
