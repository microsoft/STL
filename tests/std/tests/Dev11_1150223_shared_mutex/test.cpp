// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <atomic>
#include <cassert>
#include <chrono>
#include <mutex>
#include <shared_mutex>
#include <thread>
#include <type_traits>
#include <vector>

#define STATIC_ASSERT(...) static_assert(__VA_ARGS__, #__VA_ARGS__)

#ifndef _M_CEE
#include <condition_variable>
#endif // _M_CEE
using namespace std;

#ifndef _M_CEE
STATIC_ASSERT(is_standard_layout_v<mutex>); // N4296 30.4.1.2.1 [thread.mutex.class]/3
STATIC_ASSERT(is_standard_layout_v<recursive_mutex>); // N4296 30.4.1.2.2 [thread.mutex.recursive]/2
STATIC_ASSERT(is_standard_layout_v<timed_mutex>); // N4296 30.4.1.3.1 [thread.timedmutex.class]/2
STATIC_ASSERT(is_standard_layout_v<recursive_timed_mutex>); // N4296 30.4.1.3.2 [thread.timedmutex.recursive]/2
#endif // _M_CEE
STATIC_ASSERT(is_standard_layout_v<shared_mutex>); // N4527 30.4.1.4.1 [thread.sharedmutex.class]/2
#ifndef _M_CEE
STATIC_ASSERT(is_standard_layout_v<shared_timed_mutex>); // N4296 30.4.1.4.1 [thread.sharedtimedmutex.class]/2
STATIC_ASSERT(is_standard_layout_v<condition_variable>); // N4296 30.5.1 [thread.condition.condvar]/1
#endif // _M_CEE

void join_and_clear(vector<thread>& threads) {
    for (auto& t : threads) {
        t.join();
    }

    threads.clear();
}

template <typename Mutex>
void test_one_writer() {
    // One simultaneous writer.
    atomic<int> atom(-1);
    Mutex mut;
    vector<thread> threads;

    for (int i = 0; i < 4; ++i) {
        threads.emplace_back([&atom, &mut] {
            while (atom == -1) {
            }
            lock_guard<Mutex> ExclusiveLock(mut);
            const int val = ++atom;
            this_thread::sleep_for(25ms); // Not a timing assumption.
            assert(atom == val);
        });
    }

    assert(atom.exchange(0) == -1);
    join_and_clear(threads);
    assert(atom == 4);
}

template <typename Mutex>
void test_multiple_readers() {
    // Many simultaneous readers.
    atomic<int> atom(-1);
    Mutex mut;
    vector<thread> threads;

    for (int i = 0; i < 4; ++i) {
        threads.emplace_back([&atom, &mut] {
            while (atom == -1) {
            }
            shared_lock<Mutex> SharedLock(mut);
            ++atom;
            while (atom < 4) {
            }
        });
    }

    assert(atom.exchange(0) == -1);
    join_and_clear(threads);
    assert(atom == 4);
}

template <typename Mutex>
void test_writer_blocking_readers() {
    // One writer blocking many readers.
    atomic<int> atom(-4);
    Mutex mut;
    vector<thread> threads;

    threads.emplace_back([&atom, &mut] {
        while (atom < 0) {
        }
        lock_guard<Mutex> ExclusiveLock(mut);
        assert(atom.exchange(1000) == 0);
        this_thread::sleep_for(50ms); // Not a timing assumption.
        assert(atom.exchange(1729) == 1000);
    });

    for (int i = 0; i < 4; ++i) {
        threads.emplace_back([&atom, &mut] {
            ++atom;
            while (atom < 1000) {
            }
            shared_lock<Mutex> SharedLock(mut);
            assert(atom == 1729);
        });
    }

    join_and_clear(threads);
    assert(atom == 1729);
}

template <typename Mutex>
void test_readers_blocking_writer() {
    // Many readers blocking one writer.
    atomic<int> atom(-5);
    Mutex mut;
    vector<thread> threads;

    for (int i = 0; i < 4; ++i) {
        threads.emplace_back([&atom, &mut] {
            shared_lock<Mutex> SharedLock(mut);
            ++atom;
            while (atom < 0) {
            }
            this_thread::sleep_for(50ms); // Not a timing assumption.
            atom += 10;
        });
    }

    threads.emplace_back([&atom, &mut] {
        ++atom;
        while (atom < 0) {
        }
        lock_guard<Mutex> ExclusiveLock(mut);
        assert(atom == 40);
    });

    join_and_clear(threads);
    assert(atom == 40);
}

template <typename Mutex>
void test_try_lock_and_try_lock_shared() {
    // Test try_lock() and try_lock_shared().
    Mutex mut;

    {
        unique_lock<Mutex> MainExclusive(mut, try_to_lock);
        assert(MainExclusive.owns_lock());

        thread t([&mut] {
            {
                unique_lock<Mutex> ExclusiveLock(mut, try_to_lock);
                assert(!ExclusiveLock.owns_lock());
            }

            {
                shared_lock<Mutex> SharedLock(mut, try_to_lock);
                assert(!SharedLock.owns_lock());
            }
        });

        t.join();
    }

    {
        shared_lock<Mutex> MainShared(mut, try_to_lock);
        assert(MainShared.owns_lock());

        thread t([&mut] {
            {
                unique_lock<Mutex> ExclusiveLock(mut, try_to_lock);
                assert(!ExclusiveLock.owns_lock());
            }

            {
                shared_lock<Mutex> SharedLock(mut, try_to_lock);
                assert(SharedLock.owns_lock());
            }
        });

        t.join();
    }
}

#ifndef _M_CEE
void test_timed_behavior() {
    { // Test try_lock_for() and try_lock_shared_for(). No timing assumptions.
        shared_timed_mutex stm;

        {
            unique_lock<shared_timed_mutex> MainExclusive(stm, 25ms);
            assert(MainExclusive.owns_lock());

            thread t([&stm] {
                {
                    unique_lock<shared_timed_mutex> ExclusiveLock(stm, 25ms);
                    assert(!ExclusiveLock.owns_lock());
                }

                {
                    shared_lock<shared_timed_mutex> SharedLock(stm, 25ms);
                    assert(!SharedLock.owns_lock());
                }
            });

            t.join();
        }

        {
            shared_lock<shared_timed_mutex> MainShared(stm, 25ms);
            assert(MainShared.owns_lock());

            thread t([&stm] {
                {
                    unique_lock<shared_timed_mutex> ExclusiveLock(stm, 25ms);
                    assert(!ExclusiveLock.owns_lock());
                }

                {
                    shared_lock<shared_timed_mutex> SharedLock(stm, 25ms);
                    assert(SharedLock.owns_lock());
                }
            });

            t.join();
        }
    }

    { // Test delayed try_lock_for() success. GENEROUS timing assumptions.
        atomic<int> atom(-5);
        shared_timed_mutex stm;
        vector<thread> threads;

        shared_lock<shared_timed_mutex> MainShared(stm);

        for (int i = 0; i < 4; ++i) {
            threads.emplace_back([&atom, &stm] {
                ++atom;
                while (atom < 0) {
                }
                unique_lock<shared_timed_mutex> ExclusiveLock(stm, 1min);
                assert(ExclusiveLock.owns_lock());
                const int val = (atom += 100);
                this_thread::sleep_for(25ms);
                assert(atom == val);
            });
        }

        ++atom;
        while (atom < 0) {
        }
        this_thread::sleep_for(50ms);
        MainShared.unlock();
        join_and_clear(threads);
        assert(atom == 400);
    }

    { // Test delayed try_lock_shared_for() success. GENEROUS timing assumptions.
        atomic<int> atom(-5);
        shared_timed_mutex stm;
        vector<thread> threads;

        unique_lock<shared_timed_mutex> MainExclusive(stm);

        for (int i = 0; i < 4; ++i) {
            threads.emplace_back([&atom, &stm] {
                ++atom;
                while (atom < 0) {
                }
                shared_lock<shared_timed_mutex> SharedLock(stm, 1min);
                assert(SharedLock.owns_lock());
                atom += 11;
                while (atom < 44) {
                }
            });
        }

        ++atom;
        while (atom < 0) {
        }
        this_thread::sleep_for(50ms);
        MainExclusive.unlock();
        join_and_clear(threads);
        assert(atom == 44);
    }

    { // THE GRAND FINALE: If try_lock_for() gives up due to stubborn readers,
      // it needs to deliver notifications. No timing assumptions.
        atomic<bool> launch_readers(false);
        shared_timed_mutex stm;
        vector<thread> threads;

        shared_lock<shared_timed_mutex> MainShared(stm);

        threads.emplace_back([&launch_readers, &stm] {
            unique_lock<shared_timed_mutex> ExclusiveLock(stm, 100ms);
            assert(!ExclusiveLock.owns_lock());
            launch_readers = true;
        });

        threads.emplace_back([&launch_readers, &stm] {
            while (!launch_readers) {
                shared_lock<shared_timed_mutex> SharedLock(stm, try_to_lock);

                if (!SharedLock.owns_lock()) {
                    launch_readers = true;
                }
            }
        });

        while (!launch_readers) {
        }

        atomic<int> readers(0);

        for (int i = 0; i < 4; ++i) {
            threads.emplace_back([&stm, &readers] {
                shared_lock<shared_timed_mutex> SharedLock(stm);
                ++readers;
                while (readers < 4) {
                }
            });
        }

        join_and_clear(threads);
        assert(readers == 4);
    }
}
#endif // _M_CEE

int main() {
    test_one_writer<shared_mutex>();
    test_multiple_readers<shared_mutex>();
    test_writer_blocking_readers<shared_mutex>();
    test_readers_blocking_writer<shared_mutex>();
    test_try_lock_and_try_lock_shared<shared_mutex>();

#ifndef _M_CEE
    test_one_writer<shared_timed_mutex>();
    test_multiple_readers<shared_timed_mutex>();
    test_writer_blocking_readers<shared_timed_mutex>();
    test_readers_blocking_writer<shared_timed_mutex>();
    test_try_lock_and_try_lock_shared<shared_timed_mutex>();

    test_timed_behavior();
#endif // _M_CEE
}
