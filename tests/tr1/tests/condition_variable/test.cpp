// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

// test <condition_variable> header, C++
#define TEST_NAME "<condition_variable>"

#include "tdefs.h"
#include <chrono>
#include <condition_variable>
#include <iostream>
#include <limits>
#include <mutex>
#include <system_error>
#include <thread>

namespace {
    int count;
    int value;
    STD mutex sync_mutex;
    typedef STD unique_lock<STD mutex> sync_lock_type;
    sync_lock_type sync_lock(sync_mutex, STD defer_lock);
    STD condition_variable sync_cond;

    const long interval = (long) STD chrono::nanoseconds(STD chrono::milliseconds(20)).count();
    const long one_sec  = (long) STD chrono::nanoseconds(STD chrono::seconds(1)).count();

    enum { // test types
        timed_xtime = 0x01,
        timed_until = 0x02,
        timed_for   = 0x04,
        predicate   = 0x08,
        notify_one  = 0x10,
        notify_all  = 0x20
    };

    bool my_pred() { // predicate
        return value == 1;
    }

    bool (*pred)() = my_pred;

    bool should_time_out(int flags) {
        return (flags & (timed_until | timed_xtime | timed_for)) != 0 && (flags & (notify_one | notify_all)) == 0;
    }

    template <class Cond, class Mutex>
    class waiter { // wait in separate thread
    public:
        waiter(Cond& c, Mutex& m, int& r) : cnd(c), mtx(m), result(r) {}

        void operator()(int flags) const { // launch thread and synchronize with main thread
            try { // make sure exceptions don't escape
                bool res = false;
                wait_for_sync();

                if (flags & timed_xtime)
                    res = do_timed_xtime(flags);
                else if (flags & timed_until)
                    res = do_timed_until(flags);
                else if (flags & timed_for)
                    res = do_timed_for(flags);
                else
                    res = do_unbounded(flags);
                STD lock_guard<STD mutex> guard(sync_mutex);
                result = res ? value : -1;
            } catch (...) { // report exception and continue
                CHECK_MSG("exception thrown in thread function", false);
                STD lock_guard<STD mutex> guard(sync_mutex);
                result = -1;
            }
        }

        typedef void result_type;

    private:
        void wait_for_sync() const {
            STD lock_guard<STD mutex> guard(sync_mutex);
            ++count;
            if (count == 3)
                sync_cond.notify_one();
        }

        bool do_timed_xtime(int flags) const { // wait with timeout
            xtime target;
            xtime_get(&target, TIME_UTC);

            if (should_time_out(flags)) {
                if (one_sec - interval <= target.nsec) { // normalize (target + interval)
                    ++target.sec;
                    target.nsec -= (one_sec - interval);
                } else
                    target.nsec += interval;
            } else {
                target.sec += 60 * 60; // ~ 1 hour
                target.nsec = 0;
            }

            bool res;
            if (flags & predicate) {
                STD unique_lock<Mutex> guard(mtx);
                res = cnd.wait_until(guard, &target, pred);
            } else { // wait without predicate
                res = true;
                STD unique_lock<Mutex> guard(mtx);
                while (res && value != 1)
                    res = cnd.wait_until(guard, &target) != STD cv_status::timeout;
            }
            return res;
        }

        bool do_timed_until(int flags) const { // wait with timeout
            STD chrono::time_point<STD chrono::system_clock, STD chrono::nanoseconds> tgt;
            if (should_time_out(flags))
                tgt = STD chrono::system_clock::now() + STD chrono::nanoseconds(interval);
            else
                tgt = STD chrono::system_clock::now() + STD chrono::hours(1);

            bool res;
            if (flags & predicate) {
                STD unique_lock<Mutex> guard(mtx);
                res = cnd.wait_until(guard, tgt, pred);
            } else { // wait without predicate
                res = true;
                STD unique_lock<Mutex> guard(mtx);
                while (res && value != 1)
                    res = cnd.wait_until(guard, tgt) != STD cv_status::timeout;
            }
            return res;
        }

        bool do_timed_for(int flags) const { // wait with timeout
            STD chrono::nanoseconds dur;
            if (should_time_out(flags))
                dur = STD chrono::nanoseconds(interval);
            else
                dur = STD chrono::hours(1);

            bool res;
            if (flags & predicate) {
                STD unique_lock<Mutex> guard(mtx);
                res = cnd.wait_for(guard, dur, pred);
            } else { // wait without predicate
                res = true;
                STD unique_lock<Mutex> guard(mtx);
                while (res && value != 1)
                    res = cnd.wait_for(guard, dur) != STD cv_status::timeout;
            }
            return res;
        }

        bool do_unbounded(int flags) const { // wait without timeout
            if (flags & predicate) {
                STD unique_lock<Mutex> guard(mtx);
                cnd.wait(guard, pred);
            } else {
                STD unique_lock<Mutex> guard(mtx);
                while (value != 1)
                    cnd.wait(guard);
            }
            return true;
        }
        Cond& cnd;
        Mutex& mtx;
        int& result;
    };

    template <class Cond, class Mutex>
    void do_t_wait(Cond& cnd, Mutex& mtx, int flags) { // test wait
        count = 0;
        value = 0;

        if ((flags & (timed_for | timed_until | timed_xtime | notify_all | notify_one)) == 0)
            flags = notify_all; // ensure that we return when we're not timed

        int result0 = -1;
        waiter<Cond, Mutex> w0(cnd, mtx, result0);
        STD thread thr0(w0, flags);
        int result1 = -1;
        waiter<Cond, Mutex> w1(cnd, mtx, result1);
        STD thread thr1(w1, flags);
        int result2 = -1;
        waiter<Cond, Mutex> w2(cnd, mtx, result2);
        STD thread thr2(w2, flags);

        // block until all threads have started
        {
            STD unique_lock<STD mutex> guard(sync_mutex);
            sync_cond.wait(guard, []() { return count == 3; });
        }

        if (flags & predicate) { // simulate spurious wakeup
            cnd.notify_all();
        }

        if (flags & notify_one) { // notify one at a time
            STD lock_guard<Mutex> guard(mtx);
            value = 1;

            cnd.notify_one();
            cnd.notify_one();
            cnd.notify_one();
        } else if (flags & notify_all) {
            STD lock_guard<Mutex> guard(mtx);
            value = 1;

            cnd.notify_all();
        }

        thr0.join();
        thr1.join();
        thr2.join();

        if (should_time_out(flags)) { // should time out without wakeup
            CHECK_INT(result0, -1);
            CHECK_INT(result1, -1);
            CHECK_INT(result2, -1);
        } else { // check for proper wakeup
            CHECK_INT(result0, 1);
            CHECK_INT(result1, 1);
            CHECK_INT(result2, 1);
        }
    }

    template <class Cond, class Mutex>
    void t_wait(Cond& cnd, Mutex& mtx, int flags, const char* name) { // test multiple waits
        name = name; // to quiet diagnostics

        for (int i = 0; i < 5; ++i)
            do_t_wait(cnd, mtx, flags);
    }

    template <class Cond, class Mutex>
    void t_condition_variables(Cond& cnd, Mutex& mtx, const char* name) { // test wait functions of condition variables
        t_wait(cnd, mtx, notify_one, name);
        t_wait(cnd, mtx, notify_all, name);

        t_wait(cnd, mtx, timed_xtime, name);
        t_wait(cnd, mtx, timed_xtime | notify_one, name);
        t_wait(cnd, mtx, timed_xtime | notify_all, name);

        t_wait(cnd, mtx, timed_until, name);
        t_wait(cnd, mtx, timed_until | notify_one, name);
        t_wait(cnd, mtx, timed_until | notify_all, name);
        t_wait(cnd, mtx, timed_for, name);
        t_wait(cnd, mtx, timed_for | notify_one, name);
        t_wait(cnd, mtx, timed_for | notify_all, name);

        t_wait(cnd, mtx, notify_one | predicate, name);
        t_wait(cnd, mtx, notify_all | predicate, name);

        t_wait(cnd, mtx, timed_xtime | predicate, name);
        t_wait(cnd, mtx, timed_xtime | notify_one | predicate, name);
        t_wait(cnd, mtx, timed_xtime | notify_all | predicate, name);

        t_wait(cnd, mtx, timed_until | predicate, name);
        t_wait(cnd, mtx, timed_until | notify_one | predicate, name);
        t_wait(cnd, mtx, timed_until | notify_all | predicate, name);
        t_wait(cnd, mtx, timed_for | predicate, name);
        t_wait(cnd, mtx, timed_for | notify_one | predicate, name);
        t_wait(cnd, mtx, timed_for | notify_all | predicate, name);
    }

    void t_condition_variable() { // test condition_variable
        STD condition_variable cnd;
        STD mutex mtx;
        STD unique_lock<STD mutex> lock(mtx, STD defer_lock);

        t_condition_variables(cnd, mtx, "unique_lock");

        bool thrown = false;
        try { // check for exception on attempted wait with unowned mutex
            STD chrono::time_point<STD chrono::system_clock, STD chrono::nanoseconds> target =
                STD chrono::system_clock::now() + STD chrono::nanoseconds(interval);
            CHECK(cnd.wait_until(lock, target) == STD cv_status::timeout);
        } catch (const STD system_error& err) { // catch the exception
            thrown = true;
            CHECK(err.code().value() == (int) STD errc::operation_not_permitted);
        }
        CHECK(thrown);
    }

    class my_mutex { // user-defined mutex type
    public:
        void lock() {
            mtx.lock();
        }
        void unlock() {
            mtx.unlock();
        }

    private:
        STD mutex mtx;
    };

    void t_condition_variable_any() { // test condition_variable_any with various mutex types
        STD condition_variable_any cnd;
        STD mutex mtx0;
        t_condition_variables(cnd, mtx0, "mutex");

        STD timed_mutex mtx1;
        t_condition_variables(cnd, mtx1, "timed_mutex");

        STD recursive_mutex mtx2;
        t_condition_variables(cnd, mtx2, "recursive_mutex");

        STD recursive_timed_mutex mtx3;
        t_condition_variables(cnd, mtx3, "recursive_timed_mutex");

        my_mutex mtx4;
        t_condition_variables(cnd, mtx4, "my_mutex");
    }
} // unnamed namespace

void test_main() { // test header <condition_variable>
    t_condition_variable();
    t_condition_variable_any();
}
