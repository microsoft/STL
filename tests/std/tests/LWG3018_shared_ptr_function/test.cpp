// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <cassert>
#include <memory>

using namespace std;

int val = 0;

struct decrement_val
{
    void operator()(void (*)(int)) {
        --val;
    }
};

void add(int i) {
    val += i;
}

int add_return(int i) {
    return val += i;
}

void subtract(int i) {
    val -= i;
}

int main() {
    {
        shared_ptr<void(int)> s1(add, decrement_val{});
        assert(s1.use_count() == 1);
        assert(s1.get() == add);

        s1.get()(2);
        assert(val == 2);

        shared_ptr<void(int)> s2 = s1;
        assert(s1.use_count() == 2);
        assert(s2.use_count() == 2);
        assert(s2.get() == add);

        s2.reset(subtract, decrement_val{});
        assert(s1.use_count() == 1);
        assert(s2.use_count() == 1);
        assert(s2.get() == subtract);

        (*s2)(1);
        assert(val == 1);

        s2.reset();
        assert(s2 == nullptr);
        assert(val == 0);

        s2.swap(s1);
        assert(s1 == nullptr);
        assert(s2.get() == add);

        shared_ptr<int(int)> s3(s2, add_return);
        assert((*s3)(1) == 1);

        weak_ptr<void(int)> w1 = s2;
        (*w1.lock())(2);
        assert(val == 3);
    }
    assert(val == 2);
}
