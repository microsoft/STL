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
    shared_ptr<derived> d(new derived);
    weak_ptr<derived> wd(d);

    for (int i = 0; i < 10; ++i) {
        std::atomic<bool> work{true};
        std::thread thd([&] {
            d.reset();
            this_thread::yield();
            work = false;
        });

        while (work) {
            weak_ptr<base1> wb1(wd);
            weak_ptr<base2> wb2(wd);
            weak_ptr<base3> wb3(wd);
        }

        thd.join();
    }
}
