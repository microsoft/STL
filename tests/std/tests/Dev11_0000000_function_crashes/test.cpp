// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <functional>
#ifndef _M_CEE_PURE
#include <future>
#endif
using namespace std;

int global = 0;

void add10() {
    global += 10;
}

void add200() {
    global += 200;
}

void add3000() {
    global += 3000;
}

struct Kitten {
    int a;

    explicit Kitten(int x) : a(x) {}

    void operator()() const {
        global += a;
    }
};

int main() {
    {
        function<void()> src  = &add10;
        function<void()> dest = src;
        dest();
    }

    {
        function<void()> src(&add200);
        function<void()> dest(src);
        src = nullptr;
        dest();
    }

    {
        function<void()> dest;

        {
            function<void()> src(&add3000);
            dest = src;
        }

        dest();
    }

    {
        function<void()> g;

        {
            function<void()> f = Kitten(40000);
            f();
            g = f;
        }

        g();
    }

#ifndef _M_CEE_PURE
    {
        future<int> f = async([]() { return 500000; });
        global += f.get();
    }
#endif

#ifdef _M_CEE_PURE
    return global != 83210;
#else
    return global != 583210;
#endif
}
