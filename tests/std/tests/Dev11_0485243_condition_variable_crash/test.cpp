// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <algorithm>
#include <cassert>
#include <chrono>
#include <condition_variable>
#include <crtdbg.h>
#include <cstdlib>
#include <functional>
#include <list>
#include <mutex>
#include <thread>
#include <utility>
#include <vector>

using namespace std;

void assert_no_leaks() {
#ifdef _DEBUG
    if (_CrtDumpMemoryLeaks()) {
        abort();
    }
#endif
}

void test_484720();

int main() {
    // DevDiv-452211 "<thread>: init_at_thread_exit_mutex() creates a spurious memory leak"
    assert_no_leaks();

    {
        thread t([]() {});
        t.join();
    }

    assert_no_leaks();

    {
        thread t1([]() {});
        thread t2([]() {});
        t1.join();
        t2.join();
    }

    assert_no_leaks();

    {
        once_flag flag;
        call_once(flag, []() {});
    }

    assert_no_leaks();

    {
        // DevDiv-485243 "Crash in runtime library (msvcr110.dll)"
        condition_variable cv;
        mutex m;

        for (int k = 0; k < 100; ++k) {
            vector<thread> v;

            for (int i = 0; i < 16; ++i) {
                v.emplace_back([&cv, &m]() {
                    unique_lock<mutex> l(m);

                    for (int c = 0; c < 100; ++c) {
                        (void) cv.wait_for(l, chrono::milliseconds(1));
                        cv.notify_one();
                    }
                });
            }

            for (auto& t : v) {
                t.join();
            }
        }
    }

    assert_no_leaks();

    test_484720();

    assert_no_leaks();

    {
        // DevDiv-861298 "std::thread not fully initialized due to _Thr_set_null only setting id (not handle)"

        // native_handle()'s behavior is unspecified, but CreateThread() and _beginthreadex()
        // return null for failure, so this seems like a reasonable default.

        thread t;

        assert(t.native_handle() == nullptr);
    }

    assert_no_leaks();
}


// DevDiv-484720 "<condition_variable>: [c++std-lib-32966] Public service announcement concerning
// ~condition_variable_any()"

template <typename T>
class locked_list {
private:
    mutex mut_;
    list<T> list_;

public:
    typedef typename list<T>::iterator iterator;
    typedef typename T::key key;

    template <typename... Args>
    void emplace_back(Args&&... args) {
        list_.emplace_back(forward<Args>(args)...);
    }

    iterator find(const key& k) {
        unique_lock<mutex> lk(mut_);

        for (;;) {
            iterator ep = std::find(list_.begin(), list_.end(), k);

            if (ep == list_.end()) {
                return ep;
            }

            if (!ep->busy()) {
                ep->set_busy();
                return ep;
            }

            ep->wait(lk);
        }
    }

    void erase(iterator i) {
        lock_guard<mutex> _(mut_);
        assert(i->busy());
        i->notify_all();
        list_.erase(i);
    }

    iterator end() {
        return list_.end();
    }
};

template <typename Key>
class elt {
private:
    Key key_;
    condition_variable_any notbusy_;
    bool busy_;

public:
    typedef Key key;

    explicit elt(const Key& k) : key_(k), busy_(false) {}

    bool busy() const {
        return busy_;
    }
    void set_busy() {
        busy_ = true;
    }
    template <typename Lock>
    void wait(Lock& lk) {
        notbusy_.wait(lk);
    }
    void notify_all() {
        notbusy_.notify_all();
    }
    bool operator==(const Key& k) const {
        return key_ == k;
    }
};

void f2(locked_list<elt<int>>& lst) {
    auto i = lst.find(1);
    assert(i == lst.end());
}

void test_484720() {
    locked_list<elt<int>> lst;
    lst.emplace_back(1);

    auto i = lst.find(1);
    assert(i != lst.end());

    thread t2(f2, ref(lst));
    this_thread::sleep_for(chrono::milliseconds(500));

    lst.erase(i);

    t2.join();
}
