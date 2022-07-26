// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <cassert>
#include <chrono>
#include <cstdio>
#include <cstdlib>
#include <exception>
#include <mutex>
#include <shared_mutex>
#include <string>
#include <system_error>
#include <thread>
#include <utility>

#include <Windows.h>

using namespace std;
using namespace std::chrono;

#define STATIC_ASSERT(...) static_assert(__VA_ARGS__, #__VA_ARGS__)

[[noreturn]] void api_unexpected(const char* const api_name) {
    const DWORD lastError = ::GetLastError();
    const string msg      = system_category().message(static_cast<int>(lastError));
    printf("%s failed; GetLastError: 0x%08lX\n%s", api_name, lastError, msg.c_str());
    abort();
}

HANDLE create_event() {
    const HANDLE result = ::CreateEventW(nullptr, FALSE, FALSE, nullptr);
    if (result == nullptr) {
        api_unexpected("CreateEventW");
    }

    return result;
}

void wait_event(const HANDLE event) {
    const DWORD waitAnswer = ::WaitForSingleObject(event, INFINITE);
    if (waitAnswer != WAIT_OBJECT_0) {
        api_unexpected("WaitForSingleObject");
    }
}

void signal_event(const HANDLE event) {
    if (::SetEvent(event) == 0) {
        api_unexpected("SetEvent");
    }
}

void close_handle(const HANDLE h) {
    if (::CloseHandle(h) == 0) {
        api_unexpected("CloseHandle");
    }
}

template <typename Mutex>
class other_mutex_thread {
    enum class message { idle, shutdown, lock, tryLock, unlock, unlockDelayed };

    Mutex& mtx;
    const HANDLE backgroundEvent;
    const HANDLE foregroundEvent;
    thread thd;
    message currentMessage;
    bool tryLockSuccess;

    void thread_func() {
        for (;;) {
            wait_event(backgroundEvent);
            const message sentMessage = exchange(currentMessage, message::idle);
            switch (sentMessage) {
            case message::lock:
                mtx.lock();
                break;
            case message::tryLock:
                tryLockSuccess = mtx.try_lock();
                break;
            case message::unlockDelayed:
                this_thread::sleep_for(50ms);
                // fallthrough
            case message::unlock:
                mtx.unlock();
                break;
            case message::shutdown:
                // nothing to do
                break;
            case message::idle:
            default:
                assert(!"Bad message received on background thread");
            }

            signal_event(foregroundEvent);
            if (sentMessage == message::shutdown) {
                return;
            }
        }
    }

    void send_message(const message msg) {
        currentMessage = msg;
        signal_event(backgroundEvent);
        wait_event(foregroundEvent);
    }

public:
    explicit other_mutex_thread(Mutex& targetMtx)
        : mtx(targetMtx), backgroundEvent(create_event()), foregroundEvent(create_event()),
          thd(&other_mutex_thread::thread_func, this), currentMessage(message::idle), tryLockSuccess(false) {}

    ~other_mutex_thread() {
        close_handle(backgroundEvent);
        close_handle(foregroundEvent);
    }

    void join() {
        send_message(message::shutdown);
        thd.join();
    }

    void lock() {
        send_message(message::lock);
    }

    bool try_lock() {
        send_message(message::tryLock);
        return tryLockSuccess;
    }

    void unlock() {
        send_message(message::unlock);
    }

    void unlock_delayed() {
        currentMessage = message::unlockDelayed;
        signal_event(backgroundEvent);
    }

    void finish_delayed() {
        wait_event(foregroundEvent);
    }
};

template <typename Func>
nanoseconds time_execution(Func&& f) {
    // system_clock currently powers mutex waits, so we're using system_clock
    // rather than high_resolution_clock here for consistency. This should be
    // fixed with VSO-133414, VSO-166543, VSO-189735.
    const auto startTime = system_clock::now();
    forward<Func>(f)();
    return duration_cast<nanoseconds>(system_clock::now() - startTime);
}

template <typename Mutex>
struct mutex_test_fixture {
    Mutex mtx;
    Mutex mtx2;
    other_mutex_thread<Mutex> ot;

    mutex_test_fixture() : mtx(), mtx2(), ot(mtx) {}

    ~mutex_test_fixture() {
        ot.join();
    }

    void test_lockable() {
        mtx.lock();
        mtx.unlock();
        assert(mtx.try_lock());
        mtx.unlock();

        ot.lock();
        assert(!mtx.try_lock());
        ot.unlock();

        test_guard<lock_guard<Mutex>>();
        test_guard<unique_lock<Mutex>>();

        { // unique_lock constructor, move constructor, move assignment
            unique_lock<Mutex> ulOuter;
            STATIC_ASSERT(noexcept(unique_lock<Mutex>()));
            assert(!ulOuter.owns_lock());
            assert(!ulOuter);
            assert(ulOuter.mutex() == nullptr);

            {
                unique_lock<Mutex> ul(mtx);
                assert(!ot.try_lock());
                assert(ul.owns_lock());
                assert(static_cast<bool>(ul));
                assert(ul.mutex() == &mtx);

                unique_lock<Mutex> ulMoveConstructed(move(ul));
                assert(!ul);
                assert(ul.mutex() == nullptr);
                assert(static_cast<bool>(ulMoveConstructed));
                assert(ulMoveConstructed.mutex() == &mtx);

                ulOuter = move(ulMoveConstructed);
                assert(!ulMoveConstructed);
            }

            assert(static_cast<bool>(ulOuter));
        }

        { // unique_lock defer
            STATIC_ASSERT(noexcept(unique_lock<Mutex>(mtx, defer_lock)));
            unique_lock<Mutex> ul(mtx, defer_lock);
            assert(!ul);
            assert(ot.try_lock());
            ot.unlock();
        }

        { // unique_lock try_to_lock success
            unique_lock<Mutex> ul(mtx, try_to_lock);
            assert(static_cast<bool>(ul));
            assert(!ot.try_lock());
        }

        assert(ot.try_lock());

        { // unique_lock try_to_lock failure
            unique_lock<Mutex> ul(mtx, try_to_lock);
            assert(!ul);
        }

        ot.unlock();

        { // swap with empty object
            unique_lock<Mutex> ulLeft(mtx);
            unique_lock<Mutex> ulRight;
            assert(static_cast<bool>(ulLeft));
            assert(!ulRight);
            swap(ulLeft, ulRight);
            assert(!ulLeft);
            assert(static_cast<bool>(ulRight));
        }

        { // swap objects
            unique_lock<Mutex> ulLeft(mtx);
            unique_lock<Mutex> ulRight(mtx2);
            assert(ulLeft.mutex() == &mtx);
            assert(ulRight.mutex() == &mtx2);
            STATIC_ASSERT(noexcept(swap(ulLeft, ulRight)));
            swap(ulLeft, ulRight);
            assert(ulLeft.mutex() == &mtx2);
            assert(ulRight.mutex() == &mtx);
            STATIC_ASSERT(noexcept(ulLeft.swap(ulRight)));
            ulLeft.swap(ulRight);
            assert(ulLeft.mutex() == &mtx);
            assert(ulRight.mutex() == &mtx2);
        }

        { // lock/try_lock/unlock
            unique_lock<Mutex> ul(mtx);
            assert(!ot.try_lock());
            ul.unlock();
            assert(ot.try_lock());
            assert(!ul.try_lock());
            ot.unlock();
            assert(ul.try_lock());
            ul.unlock();
            ul.lock();
        }

        { // release
            unique_lock<Mutex> ul(mtx);
            assert(&mtx == ul.release());
        }

        assert(!ot.try_lock());
        mtx.unlock();
    }

    template <typename Guard>
    void test_guard() {
        {
            Guard lg(mtx);
            assert(!ot.try_lock());
        }

        {
            mtx.lock();
            Guard lg(mtx, adopt_lock);
            assert(!ot.try_lock());
        }

        assert(ot.try_lock());
        ot.unlock();
    }

    void test_timed_lockable() {
        // Test acquiring locks successfully
        assert(time_execution([this] { assert(mtx.try_lock_for(24h)); }) < 1h);
        mtx.unlock();
        assert(time_execution([this] { assert(mtx.try_lock_until(system_clock::now() + 24h)); }) < 1h);
        mtx.unlock();
        assert(time_execution([this] {
            unique_lock<Mutex> ul(mtx, defer_lock);
            assert(ul.try_lock_for(24h));
        }) < 1h);
        assert(time_execution([this] {
            unique_lock<Mutex> ul(mtx, 24h);
            assert(ul.owns_lock());
        }) < 1h);
        assert(time_execution([this] {
            unique_lock<Mutex> ul(mtx, defer_lock);
            assert(ul.try_lock_until(system_clock::now() + 24h));
        }) < 1h);
        assert(time_execution([this] {
            unique_lock<Mutex> ul(mtx, system_clock::now() + 24h);
            assert(ul.owns_lock());
        }) < 1h);

#if 0 // TRANSITION, GH-1472
      // Test failing to acquire locks on timeout
        ot.lock();
        assert(time_execution([this] { assert(!mtx.try_lock_for(50ms)); }) >= 50ms);
        assert(time_execution([this] { assert(!mtx.try_lock_until(system_clock::now() + 50ms)); }) >= 50ms);
        assert(time_execution([this] {
            unique_lock<Mutex> ul(mtx, defer_lock);
            assert(!ul.try_lock_for(50ms));
        }) >= 50ms);
        assert(time_execution([this] {
            unique_lock<Mutex> ul(mtx, 50ms);
            assert(!ul.owns_lock());
        }) >= 50ms);
        assert(time_execution([this] {
            unique_lock<Mutex> ul(mtx, defer_lock);
            assert(!ul.try_lock_until(system_clock::now() + 50ms));
        }) >= 50ms);
        assert(time_execution([this] {
            unique_lock<Mutex> ul(mtx, system_clock::now() + 50ms);
            assert(!ul.owns_lock());
        }) >= 50ms);
        ot.unlock();
#endif // TRANSITION, GH-1472
    }

    // nonstandard xtime type
    template <class Rep, class Period>
    xtime to_xtime(const chrono::duration<Rep, Period>& rel_time) { // convert duration to xtime
        xtime xt;
        if (rel_time <= chrono::duration<Rep, Period>::zero()) { // negative or zero relative time, return zero
            xt.sec  = 0;
            xt.nsec = 0;
        } else { // positive relative time, convert
            chrono::nanoseconds t0 = chrono::system_clock::now().time_since_epoch();
            t0 += chrono::duration_cast<chrono::nanoseconds>(rel_time);
            xt.sec = chrono::duration_cast<chrono::seconds>(t0).count();
            t0 -= chrono::seconds(xt.sec);
            xt.nsec = static_cast<long>(t0.count());
        }
        return xt;
    }

    void test_timed_lockable_xtime() {
        assert(time_execution([this] {
            const auto xt = to_xtime(24h);
            assert(mtx.try_lock_until(&xt));
        }) < 1h);
        mtx.unlock();
        assert(time_execution([this] {
            const auto xt = to_xtime(24h);
            unique_lock<Mutex> ul(mtx, defer_lock);
            assert(ul.try_lock_until(&xt));
        }) < 1h);

#if 0 // TRANSITION, GH-1472
        ot.lock();
        assert(time_execution([this] {
            const auto xt = to_xtime(50ms);
            assert(!mtx.try_lock_until(&xt));
        }) >= 50ms);
        assert(time_execution([this] {
            const auto xt = to_xtime(50ms);
            unique_lock<Mutex> ul(mtx, defer_lock);
            assert(!ul.try_lock_until(&xt));
        }) >= 50ms);
        ot.unlock();
#endif // TRANSITION, GH-1472
    }

    void test_recursive_lockable() {
        mtx.lock();
        assert(!ot.try_lock());
        mtx.lock();
        assert(!ot.try_lock());
        assert(mtx.try_lock());
        mtx.unlock();
        assert(!ot.try_lock());
        mtx.unlock();
        mtx.unlock();
        assert(ot.try_lock());
        ot.unlock();
    }
};

struct throwing_mutex_threw : exception {
    using exception::exception;
};

struct throwing_mutex {
    void lock() {
        throw throwing_mutex_threw();
    }

    bool try_lock() {
        throw throwing_mutex_threw();
    }

    void unlock() {}
};

void test_nonmember_lock() {
    mutex mtx;
    other_mutex_thread<mutex> ot(mtx);

    recursive_mutex rMtx;
    timed_mutex tMtx;

    recursive_timed_mutex rtMtx;
    other_mutex_thread<recursive_timed_mutex> rtOt(rtMtx);

    lock(mtx, rMtx, tMtx, rtMtx);
    mtx.unlock();
    rMtx.unlock();
    tMtx.unlock();
    rtMtx.unlock();

    ot.lock();
    rtOt.lock();
    ot.unlock_delayed(); // no timing assumptions: lock() retries until it
    rtOt.unlock_delayed(); // can lock all input mutexes
    lock(rtMtx, tMtx, mtx, rMtx);
    mtx.unlock();
    rMtx.unlock();
    tMtx.unlock();
    rtMtx.unlock();
    ot.finish_delayed();
    rtOt.finish_delayed();

    throwing_mutex throwing;
    try {
        lock(rtMtx, mtx, throwing); // throws
        abort();
    } catch (const throwing_mutex_threw&) {
        assert(ot.try_lock()); // check that other mutexes unlocked after throw
        ot.unlock();
        assert(rtOt.try_lock());
        rtOt.unlock();
    }

    ot.join();
    rtOt.join();
}

void test_nonmember_try_lock() {
    mutex mtx;
    other_mutex_thread<mutex> ot(mtx);

    recursive_mutex rMtx;
    timed_mutex tMtx;

    recursive_timed_mutex rtMtx;
    other_mutex_thread<recursive_timed_mutex> rtOt(rtMtx);

    // try_lock with all mutexes unlocked
    assert(try_lock(mtx, rMtx, tMtx, rtMtx) == -1);
    mtx.unlock();
    rMtx.unlock();
    tMtx.unlock();
    rtMtx.unlock();

    // try_lock with some mutexes locked
    rtOt.lock();
    assert(try_lock(mtx, rMtx, tMtx, rtMtx) == 3);
    assert(ot.try_lock());
    ot.unlock();
    rtOt.unlock();

    // try_lock with throw
    throwing_mutex throwing;
    try {
        (void) try_lock(mtx, rtMtx, throwing);
        abort();
    } catch (const throwing_mutex_threw&) {
        assert(ot.try_lock()); // check that other mutexes unlocked after throw
        ot.unlock();
        assert(rtOt.try_lock());
        rtOt.unlock();
    }

    ot.join();
    rtOt.join();
}

// Also test VSO-1253916, in which RWC like the following broke when we annotated unique_lock with [[nodiscard]].
unique_lock<shared_mutex> do_locked_things(unique_lock<shared_mutex> lck) {
    return lck;
}

shared_lock<shared_mutex> do_shared_locked_things(shared_lock<shared_mutex> lck) {
    return lck;
}

void test_vso_1253916() {
    shared_mutex mtx;
    do_locked_things(unique_lock<shared_mutex>{mtx});
    do_shared_locked_things(shared_lock<shared_mutex>{mtx});
}

int main() {
    {
        mutex_test_fixture<mutex> fixture;
        fixture.test_lockable();
    }

    {
        mutex_test_fixture<timed_mutex> fixture;
        fixture.test_lockable();
        fixture.test_timed_lockable();
        fixture.test_timed_lockable_xtime();
    }

    {
        mutex_test_fixture<recursive_mutex> fixture;
        fixture.test_lockable();
        fixture.test_recursive_lockable();
    }

    {
        mutex_test_fixture<recursive_timed_mutex> fixture;
        fixture.test_lockable();
        fixture.test_timed_lockable();
        fixture.test_timed_lockable_xtime();
        fixture.test_recursive_lockable();
    }

    {
        mutex_test_fixture<shared_mutex> fixture;
        fixture.test_lockable();
        // shared-ownership locking behavior tested in Dev11_1150223_shared_mutex
    }

    {
        mutex_test_fixture<shared_timed_mutex> fixture;
        fixture.test_lockable();
        fixture.test_timed_lockable();
    }

    test_nonmember_lock();
    test_nonmember_try_lock();

    test_vso_1253916();
}
