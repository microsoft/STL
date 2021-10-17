// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <atomic>
#include <memory>
#include <thread>

using namespace std;

struct base1 {
    int i = 0;
};

struct base2 {
    int j = 0;
};

struct base3 {
    int k = 0;
};

struct derived : virtual base1, virtual base2, base3 {};

int main() {
    for (int i = 0; i < 10; ++i) {
        // not make_shared -- with make_shared the test would not catch errors
        shared_ptr<derived> d{new derived{}};
        weak_ptr<derived> wd{d};
        atomic<bool> work{true};
        thread thd([&] {
            d.reset();
            this_thread::yield(); // make crash on incorrect optimization even more likely
            work = false;
        });

        if ((i % 2) == 0) {
            while (work) {
                // likely to crash if optimized for a case we shouldn't
                weak_ptr<base1> wb1{wd};
                weak_ptr<base2> wb2{wd};
                weak_ptr<base3> wb3{wd};
            }
        } else {
            while (work) {
                // likely to crash if optimized for a case we shouldn't
                weak_ptr<base1> wb1{weak_ptr<derived>{wd}};
                weak_ptr<base2> wb2{weak_ptr<derived>{wd}};
                weak_ptr<base3> wb3{weak_ptr<derived>{wd}};
            }
        }

        thd.join();
    }
}
