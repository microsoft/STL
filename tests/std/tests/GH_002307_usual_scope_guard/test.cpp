// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <algorithm>
#include <cassert>
#include <cstring>
#include <deque>
#include <stdexcept>

using namespace std;

struct countdown {
    int val;

    static int count;

    void tick() {
        if (count == 0) {
            throw runtime_error{"GO"};
        } else {
            --count;
        }
    }

    explicit countdown(const int init) : val(init) {
        tick();
    }

    countdown(const countdown& other) : val(other.val) {
        tick();
    }

    countdown& operator=(const countdown& other) {
        tick();
        val = other.val;
        return *this;
    }

    bool operator==(int other) const {
        return val == other;
    }
};

int countdown::count = 0;

constexpr int init_data[] = {1, 2, 3, 4, 5, 6, 7};
constexpr int more_data[] = {10, 11, 12, 13, 14};

template <class Container>
void check(const Container& c) {
    assert(equal(c.begin(), c.end(), begin(init_data), end(init_data)));
}

void check_exception(const runtime_error& ex) {
    assert(strcmp(ex.what(), "GO") == 0);
}

void test_deque() {
    countdown::count = 8;

    deque<countdown> dq(begin(init_data), end(init_data));

    try {
        countdown::count = 3;
        dq.insert(dq.end() - 2, begin(more_data), end(more_data));
        assert(false); // Should have thrown an exception
    } catch (const runtime_error& ex) {
        check_exception(ex);
        check(dq);
    }

    try {
        countdown::count = 3;
        dq.insert(dq.begin() + 2, begin(more_data), end(more_data));
        assert(false); // Should have thrown an exception
    } catch (const runtime_error& ex) {
        check_exception(ex);
        check(dq);
    }

    try {
        countdown::count = 3;
        dq.insert(dq.end() - 2, 6, countdown{10});
        assert(false); // Should have thrown an exception
    } catch (const runtime_error& ex) {
        check_exception(ex);
        check(dq);
    }

    try {
        countdown::count = 3;
        dq.insert(dq.begin() + 2, 6, countdown{11});
        assert(false); // Should have thrown an exception
    } catch (const runtime_error& ex) {
        check_exception(ex);
        check(dq);
    }
}

int main() {
    test_deque();
}
