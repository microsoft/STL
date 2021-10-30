// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#define _CORE_HEADERS_ONLY

#include <cassert>
#include <deque>
#include <stdexcept>

using namespace std;

struct bomb {
    int val;

    static int countdown;

    void tick() {
        if (countdown == 0) {
            throw runtime_error("BOOM");
        } else {
            --countdown;
        }
    }

    bomb(int init) : val(init) {
        tick();
    }

    bomb(const bomb& other) : val(other.val) {
        tick();
    }

    bomb& operator=(const bomb& other) {
        val = other.val;
        return *this;
    }

    bool operator==(int other) const {
        return val == other;
    }
};

int bomb::countdown = 0;

constexpr int init_data[] = {1, 2, 3, 4, 5, 6, 7};
constexpr int more_data[] = {10, 11, 12, 13, 14};


template <class Container>
void check(Container& c) {
    assert(equal(c.begin(), c.end(), init_data));
}

void check_exception(runtime_error& ex) {
    assert(strcmp(ex.what(), "BOOM") == 0);
}

void test_deque() {

    bomb::countdown = 8;

    deque<bomb> dq(begin(init_data), end(init_data));

    try {
        bomb::countdown = 3;
        dq.insert(dq.end() - 2, begin(more_data), end(more_data));
        abort();
    } catch (runtime_error& ex) {
        check_exception(ex);
        check(dq);
    }

    try {
        bomb::countdown    = 3;
        const int data_x[] = {10, 11, 12, 13, 14};
        dq.insert(dq.begin() + 2, begin(more_data), end(more_data));
        abort();
    } catch (runtime_error& ex) {
        check_exception(ex);
        check(dq);
    }

    try {
        bomb::countdown = 3;
        dq.insert(dq.end() - 2, 6, 10);
        abort();
    } catch (runtime_error& ex) {
        check_exception(ex);
        check(dq);
    }

    try {
        bomb::countdown = 3;
        dq.insert(dq.begin() + 2, 6, 11);
        abort();
    } catch (runtime_error& ex) {
        check_exception(ex);
        check(dq);
    }
}

int main() {
    test_deque();
}
