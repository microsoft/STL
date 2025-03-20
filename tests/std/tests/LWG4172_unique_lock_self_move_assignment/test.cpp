// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <cassert>
#include <mutex>
#include <shared_mutex>
#include <utility>

using namespace std;

struct lockable_with_counters {
    void lock() {
        ++lock_count;
    }

    void unlock() {
        ++unlock_count;
    }

    void lock_shared() {
        ++shared_lock_count;
    }

    void unlock_shared() {
        ++shared_unlock_count;
    }

    int lock_count          = 0;
    int unlock_count        = 0;
    int shared_lock_count   = 0;
    int shared_unlock_count = 0;
};

#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wself-move"
#endif // __clang__
#pragma warning(push)
#pragma warning(disable : 26800) // use a moved-from object
int main() {
    lockable_with_counters lockable1;
    lockable_with_counters lockable2;

    {
        unique_lock<lockable_with_counters> lock_a(lockable1);
        assert(lockable1.lock_count == 1);
        assert(lockable1.unlock_count == 0);

        lock_a = move(lock_a);
        assert(lockable1.lock_count == 1);
        assert(lockable1.unlock_count == 0);
        {
            unique_lock<lockable_with_counters> lock_b(lockable2);
            assert(lockable2.lock_count == 1);
            assert(lockable2.unlock_count == 0);

            lock_a = move(lock_b);

            assert(lockable1.lock_count == 1);
            assert(lockable1.unlock_count == 1);
            assert(lockable2.lock_count == 1);
            assert(lockable2.unlock_count == 0);
        }

        assert(lockable1.lock_count == 1);
        assert(lockable1.unlock_count == 1);
        assert(lockable2.lock_count == 1);
        assert(lockable2.unlock_count == 0);
    }

    assert(lockable1.lock_count == 1);
    assert(lockable1.unlock_count == 1);
    assert(lockable2.lock_count == 1);
    assert(lockable2.unlock_count == 1);

    {
        shared_lock<lockable_with_counters> lock_a(lockable1);
        assert(lockable1.shared_lock_count == 1);
        assert(lockable1.shared_unlock_count == 0);

        lock_a = move(lock_a);
        assert(lockable1.shared_lock_count == 1);
        assert(lockable1.shared_unlock_count == 0);
        {
            shared_lock<lockable_with_counters> lock_b(lockable2);
            assert(lockable2.shared_lock_count == 1);
            assert(lockable2.shared_unlock_count == 0);

            lock_a = move(lock_b);

            assert(lockable1.shared_lock_count == 1);
            assert(lockable1.shared_unlock_count == 1);
            assert(lockable2.shared_lock_count == 1);
            assert(lockable2.shared_unlock_count == 0);
        }

        assert(lockable1.shared_lock_count == 1);
        assert(lockable1.shared_unlock_count == 1);
        assert(lockable2.shared_lock_count == 1);
        assert(lockable2.shared_unlock_count == 0);
    }

    assert(lockable1.shared_lock_count == 1);
    assert(lockable1.shared_unlock_count == 1);
    assert(lockable2.shared_lock_count == 1);
    assert(lockable2.shared_unlock_count == 1);
}
#pragma warning(pop)
#ifdef __clang__
#pragma clang diagnostic pop
#endif // __clang__
