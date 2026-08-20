// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <atomic>
#include <cassert>

using namespace std;

#pragma warning(push)
#pragma warning(disable : 4996)

void test_volatile_inc_dec() {
    volatile atomic<int> ai{10};
    (void) ai++;
    (void) ++ai;
    (void) ai--;
    (void) --ai;

    int arr[5] = {0};
    volatile atomic<int*> ap{arr};
    (void) ap++;
    (void) ++ap;
    (void) ap--;
    (void) --ap;
}

#if _HAS_CXX20
void test_volatile_wait_notify() {
    volatile atomic<int> ai{0};
    ai.notify_one();
    ai.notify_all();
    ai.wait(1);

    int dummy = 0;
    volatile atomic<int*> ap{&dummy};
    ap.notify_one();
    ap.notify_all();
    int other = 0;
    ap.wait(&other);
}
#endif // _HAS_CXX20

#pragma warning(pop)

int main() {
    test_volatile_inc_dec();
#if _HAS_CXX20
    test_volatile_wait_notify();
#endif
}
