// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <cassert>
#include <future>
#include <thread>

int main() {
    {
        std::packaged_task<int()> task1([] { return 1; });
        std::future<int> f1 = task1.get_future();

        std::packaged_task<int()> task2([] { return 2; });
        task1 = std::move(task2);

        try {
            (void) f1.get();
            assert(false);
        } catch (const std::future_error& e) {
            assert(e.code() == std::future_errc::broken_promise);
        }
    }

    return 0;
}
