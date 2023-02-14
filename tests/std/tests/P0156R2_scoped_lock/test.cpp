// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <atomic>
#include <cstdlib>
#include <iostream>
#include <mutex>
#include <thread>
#include <type_traits>
#include <utility>

#define STATIC_ASSERT(...) static_assert(__VA_ARGS__, #__VA_ARGS__)

using namespace std;

// nothrow-destructibility required by N4928 [res.on.exception.handling]/3
STATIC_ASSERT(is_nothrow_destructible_v<lock_guard<mutex>>);
STATIC_ASSERT(is_nothrow_destructible_v<unique_lock<mutex>>);
#if _HAS_CXX17
STATIC_ASSERT(is_nothrow_destructible_v<scoped_lock<>>);
STATIC_ASSERT(is_nothrow_destructible_v<scoped_lock<mutex>>);
STATIC_ASSERT(is_nothrow_destructible_v<scoped_lock<mutex, recursive_mutex>>);
#endif // _HAS_CXX17

// Test mandatory and strengthened exception specification for default construction
STATIC_ASSERT(is_nothrow_default_constructible_v<unique_lock<mutex>>);
#if _HAS_CXX17
STATIC_ASSERT(is_nothrow_default_constructible_v<scoped_lock<>>); // strengthened
#endif // _HAS_CXX17

// Test strengthened exception specification for adopt_lock construction
STATIC_ASSERT(is_nothrow_constructible_v<lock_guard<mutex>, mutex&, adopt_lock_t>);
STATIC_ASSERT(is_nothrow_constructible_v<lock_guard<mutex>, mutex&, const adopt_lock_t&>);
STATIC_ASSERT(is_nothrow_constructible_v<unique_lock<mutex>, mutex&, adopt_lock_t>);
STATIC_ASSERT(is_nothrow_constructible_v<unique_lock<mutex>, mutex&, const adopt_lock_t&>);
#if _HAS_CXX17
STATIC_ASSERT(is_nothrow_constructible_v<scoped_lock<>, adopt_lock_t>);
STATIC_ASSERT(is_nothrow_constructible_v<scoped_lock<>, const adopt_lock_t&>);
STATIC_ASSERT(is_nothrow_constructible_v<scoped_lock<mutex>, adopt_lock_t, mutex&>);
STATIC_ASSERT(is_nothrow_constructible_v<scoped_lock<mutex>, const adopt_lock_t&, mutex&>);
STATIC_ASSERT(is_nothrow_constructible_v<scoped_lock<mutex, recursive_mutex>, adopt_lock_t, mutex&, recursive_mutex&>);
STATIC_ASSERT(
    is_nothrow_constructible_v<scoped_lock<mutex, recursive_mutex>, const adopt_lock_t&, mutex&, recursive_mutex&>);
#endif // _HAS_CXX17

// LOCK ORDERING: g_coutMutex is locked after all other locks and synchronizes
// access to cout.
using TestMutex = mutex;

TestMutex g_coutMutex;

atomic<int> g_testResult(0);

void verify(bool b, const char* msg) {
    if (!b) {
        lock_guard<TestMutex> lck(g_coutMutex);
        cout << "FAIL: " << msg << endl;
        abort();
    }
}

class ownership_tracking_mutex {
    TestMutex mtx;
    atomic<thread::id> owningId;
    const char* name;

public:
    explicit ownership_tracking_mutex(const char* name) : mtx(), owningId(thread::id()), name(name) {
        // Purposely empty
    }

    void lock() {
        this->mtx.lock();
        this->owningId = this_thread::get_id();
    }

    void unlock() {
        this->owningId = thread::id();
        this->mtx.unlock();
    }

    bool try_lock() {
        bool lockTaken = this->mtx.try_lock();
        if (lockTaken) {
            this->owningId = this_thread::get_id();
        }

        return lockTaken;
    }

    void verify_unowned(int line) const {
        thread::id owner = this->owningId;
        if (owner != thread::id()) {
            lock_guard<TestMutex> lck(g_coutMutex);
            cout << "FAIL (" << line << "): Expected mutex " << this->name << " to be unowned but " << owner
                 << " owns it." << endl;
            abort();
        }
    }

    void verify_owns(int line) const {
        thread::id owner   = this->owningId;
        thread::id current = this_thread::get_id();
        if (owner == thread::id()) {
            lock_guard<TestMutex> lck(g_coutMutex);
            cout << "FAIL (" << line << "): Expected mutex " << this->name << " to be owned by " << current
                 << " but it was unowned." << endl;
            abort();
        }

        if (owner != current) {
            lock_guard<TestMutex> lck(g_coutMutex);
            cout << "FAIL (" << line << "): Expected mutex " << this->name << " to be owned by " << current << " but "
                 << owner << " owns it." << endl;
            abort();
        }
    }
};

#define VERIFY_UNOWNED(m) ((m).verify_unowned(__LINE__))
#define VERIFY_OWNS(m)    ((m).verify_owns(__LINE__))

ownership_tracking_mutex g_mutexA("A");
ownership_tracking_mutex g_mutexB("B");
ownership_tracking_mutex g_mutexC("C");
ownership_tracking_mutex g_mutexD("D");

class logging_thread {
    thread t;

public:
    template <class FuncT, class... Args>
    explicit logging_thread(FuncT&& f, Args&&... args) : t(forward<FuncT>(f), forward<Args>(args)...) {
        lock_guard<TestMutex> lck(g_coutMutex);
        cout << "Started thread " << this->t.get_id() << endl;
    }

    logging_thread(logging_thread const&)            = delete;
    logging_thread& operator=(logging_thread const&) = delete;

    ~logging_thread() {
        {
            lock_guard<TestMutex> lck(g_coutMutex);
            cout << "Terminating thread " << this->t.get_id() << endl;
        }

        this->t.join();
    }
};

static_assert(is_same_v<TestMutex, lock_guard<TestMutex>::mutex_type>, "lock_guard should expose mutex_type");

template <typename LockTested>
void exec_test_lock_operates_with_one_mutex() {
    VERIFY_UNOWNED(g_mutexA);
    {
        LockTested lck(g_mutexA);
        VERIFY_OWNS(g_mutexA);
        VERIFY_UNOWNED(g_mutexB);
        VERIFY_UNOWNED(g_mutexC);
        VERIFY_UNOWNED(g_mutexD);
    }

    VERIFY_UNOWNED(g_mutexA);
}

template <typename LockTested>
void exec_test_lock_guard_adopts_one_mutex() {
    VERIFY_UNOWNED(g_mutexA);
    {
        g_mutexA.lock();
        VERIFY_OWNS(g_mutexA);
        LockTested lckAdopt(g_mutexA, adopt_lock);
        VERIFY_OWNS(g_mutexA);
    }

    VERIFY_UNOWNED(g_mutexA);
}

template <typename LockTested>
void exec_test_scoped_lock_adopts_one_mutex() {
    VERIFY_UNOWNED(g_mutexA);
    {
        g_mutexA.lock();
        VERIFY_OWNS(g_mutexA);
        LockTested lckAdopt(adopt_lock, g_mutexA);
        VERIFY_OWNS(g_mutexA);
    }

    VERIFY_UNOWNED(g_mutexA);
}

#if _HAS_CXX17
// Special case for 0 mutex types.
void exec_test_scoped_lock_compiles_with_no_mutexes() {
    [[maybe_unused]] scoped_lock<> takeNoLocks;
    VERIFY_UNOWNED(g_mutexA);
    VERIFY_UNOWNED(g_mutexB);
    VERIFY_UNOWNED(g_mutexC);
    VERIFY_UNOWNED(g_mutexD);

    scoped_lock<> takeNoAdoptedLocks(adopt_lock);
    VERIFY_UNOWNED(g_mutexA);
    VERIFY_UNOWNED(g_mutexB);
    VERIFY_UNOWNED(g_mutexC);
    VERIFY_UNOWNED(g_mutexD);
}

// Special case for 1 mutex type.
static_assert(is_same_v<TestMutex, scoped_lock<TestMutex>::mutex_type>,
    "scoped_lock should expose mutex_type when there is one mutex.");

// General case
void exec_test_scoped_lock_operates_with_multiple_mutexes() {
    using tested_type = scoped_lock<ownership_tracking_mutex, ownership_tracking_mutex, ownership_tracking_mutex,
        ownership_tracking_mutex>;
    atomic<bool> start(false);
    bool xDone = false;
    bool yDone = false;
    bool zDone = false;

    {
        logging_thread xThread([&]() {
            while (!start) {
                // Wait for other threads to be constructed for greater chance
                // of real lock contention.
            }

            tested_type lck(g_mutexA, g_mutexB, g_mutexC, g_mutexD);
            VERIFY_OWNS(g_mutexA);
            VERIFY_OWNS(g_mutexB);
            VERIFY_OWNS(g_mutexC);
            VERIFY_OWNS(g_mutexD);
            xDone = true;
        });

        logging_thread yThread([&]() {
            while (!start) {
                // Ditto
            }

            // Note different mutex acquire order
            tested_type lck(g_mutexA, g_mutexC, g_mutexD, g_mutexB);
            VERIFY_OWNS(g_mutexA);
            VERIFY_OWNS(g_mutexB);
            VERIFY_OWNS(g_mutexC);
            VERIFY_OWNS(g_mutexD);
            yDone = true;
        });

        logging_thread zThread([&]() {
            while (!start) {
                // Ditto
            }

            // Note still different mutex acquire order
            tested_type lck(g_mutexD, g_mutexC, g_mutexB, g_mutexA);
            VERIFY_OWNS(g_mutexA);
            VERIFY_OWNS(g_mutexB);
            VERIFY_OWNS(g_mutexC);
            VERIFY_OWNS(g_mutexD);
            zDone = true;
        });

        start = true;
    } // synchronize with xThread, yThread, and zThread

    verify(xDone, "Thread X didn't complete work");
    verify(yDone, "Thread Y didn't complete work");
    verify(zDone, "Thread Z didn't complete work");
    VERIFY_UNOWNED(g_mutexA);
    VERIFY_UNOWNED(g_mutexB);
    VERIFY_UNOWNED(g_mutexC);
    VERIFY_UNOWNED(g_mutexD);
}

void exec_test_scoped_lock_adopts_multiple_mutexes() {
    VERIFY_UNOWNED(g_mutexA);
    VERIFY_UNOWNED(g_mutexB);
    {
        lock(g_mutexA, g_mutexB);
        VERIFY_OWNS(g_mutexA);
        VERIFY_OWNS(g_mutexB);
        scoped_lock<ownership_tracking_mutex, ownership_tracking_mutex> lckAdopt(adopt_lock, g_mutexA, g_mutexB);
        VERIFY_OWNS(g_mutexA);
        VERIFY_OWNS(g_mutexB);
    }

    VERIFY_UNOWNED(g_mutexA);
    VERIFY_UNOWNED(g_mutexB);
}
#endif // _HAS_CXX17

int main() {
    exec_test_lock_operates_with_one_mutex<lock_guard<ownership_tracking_mutex>>();
    exec_test_lock_guard_adopts_one_mutex<lock_guard<ownership_tracking_mutex>>();

#if _HAS_CXX17
    exec_test_scoped_lock_compiles_with_no_mutexes();
    exec_test_lock_operates_with_one_mutex<scoped_lock<ownership_tracking_mutex>>();
    exec_test_scoped_lock_adopts_one_mutex<scoped_lock<ownership_tracking_mutex>>();
    exec_test_scoped_lock_operates_with_multiple_mutexes();
    exec_test_scoped_lock_adopts_multiple_mutexes();
#endif // _HAS_CXX17

    return g_testResult;
}
