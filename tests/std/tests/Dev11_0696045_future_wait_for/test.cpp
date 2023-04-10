// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <cassert>
#include <chrono>
#include <future>

using namespace std;

int func() {
    return 1729;
}

int main() {
    { // DevDiv-482796 "C++11 unexpected behavior for std::future::wait_for and std::packaged_task"
        packaged_task<int()> pt(func);

        future<int> f = pt.get_future();

        assert(f.wait_for(chrono::seconds(0)) == future_status::timeout);

        pt();

        assert(f.wait_for(chrono::seconds(0)) == future_status::ready);

        assert(f.get() == 1729);
    }

    { // DevDiv-696045 "<future>: The function wait_for() wait until timeout."
        future<int> f = async(launch::deferred, func);

        const auto dur = chrono::minutes(5);

        const auto start = chrono::steady_clock::now();

        assert(f.wait_for(dur) == future_status::deferred);

        const auto finish = chrono::steady_clock::now();

        // Technically, this is an improper timing assumption.
        // In practice, there's no way wait_for()'s immediate return will take 5 minutes.
        assert(finish < start + dur);


        assert(f.wait_for(chrono::seconds(0)) == future_status::deferred);

        f.wait();

        assert(f.wait_for(chrono::seconds(0)) == future_status::ready);

        assert(f.get() == 1729);
    }
}
