// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <atomic>
#include <cassert>
#include <regex>
#include <thread>
#include <vector>

using namespace std;

const int REPEATS = 50; // Repeat the whole test this many times.
const int N       = 4; // Spin up N matching threads and N copying threads.
const int COPIES  = 500000; // Each copying thread performs this many copies.

int main() {
    atomic<bool> atom_success(true);

    for (int repeat = 0; repeat < REPEATS; ++repeat) {
        vector<thread> threads;
        atomic<int> atom_copiers(N);
        const regex r("a+b+c+");

        for (int i = 0; i < N; ++i) {
            threads.emplace_back([&atom_copiers, &r, &atom_success] {
                while (atom_copiers > 0) {
                    const regex dupe(r);

                    if (!regex_match("aaabbbccc", dupe)) {
                        atom_success = false;
                    }
                }
            });
        }

        for (int i = 0; i < N; ++i) {
            threads.emplace_back([&atom_copiers, &r] {
                for (int k = 0; k < COPIES; ++k) {
                    const regex dupe(r);
                }

                --atom_copiers;
            });
        }

        for (auto& t : threads) {
            t.join();
        }
    }

    assert(atom_success);
}
