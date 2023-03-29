// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

// test <memory>, part 2
#define TEST_NAME "<memory>, part 2"

#include "tdefs.h"
#include <memory>
#include <mutex>
#include <stdlib.h>
#include <thread>
#include <vector>

using STD shared_ptr;
using STD weak_ptr;

using STD lock_guard;
using STD mutex;
using STD thread;
using STD vector;

#define NSETS    3000
#define NREPS    300
#define NTHREADS 3

static shared_ptr<int> sp;
static mutex start_mtx;

static void ctors() { // construct a gazillion shared and weak pointers

    { // wait for access
        lock_guard<mutex> lock(start_mtx);
    }

    for (int i = 0; i < NSETS; ++i) {
        for (int j = 0; j < NREPS; ++j) {
            if (rand() % 2 != 0) {
                shared_ptr<int> sp0(sp);
            } else {
                weak_ptr<int> wp0(sp);
            }
        }
    }
}

static void tctors() { // check for race conditions
    sp = shared_ptr<int>(new int);
    vector<thread> grp;

    { // wait for access
        lock_guard<mutex> lock(start_mtx);
        for (int i = 0; i < NTHREADS; ++i) {
            grp.emplace_back(ctors);
        }
    }

    for (int i = 0; i < NTHREADS; ++i) {
        grp[i].join();
    }

    CHECK_INT(sp.use_count(), 1);
}

void test_main() { // run tests
    tctors();
}
