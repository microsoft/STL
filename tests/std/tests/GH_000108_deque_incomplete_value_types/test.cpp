// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <assert.h>
#include <deque>

using namespace std;

void test_gh_000108_push_pop_size();
void test_gh_000108_push_pop_size_with_payload();

int main() {
    test_gh_000108_push_pop_size();
    test_gh_000108_push_pop_size_with_payload();
}

void test_gh_000108_push_pop_size() {
    struct recur {
        deque<recur> d;
    };

    recur x;

    for (int n = 0; n < 1000; ++n) {
        x.d.push_back(recur{});

        if (x.d.size() < 26) {
            continue;
        }

        x.d.pop_front();
    }
}

void test_gh_000108_push_pop_size_with_payload() {
    struct recur_payload {
        int n;
        deque<recur_payload> d;
    };

    recur_payload x{};

    x.d.push_back({10, {}});
    x.d.push_back({20, {}});
    x.d.push_front({30, {}});
    x.d.push_front({40, {}});

    assert(x.d.size() == 4 && x.d[0].n == 40 && x.d[1].n == 30 && x.d[2].n == 10 && x.d[3].n == 20);
}
