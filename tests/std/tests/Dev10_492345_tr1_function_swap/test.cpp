// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <cassert>
#include <cstddef>
#include <functional>
#include <new>
#include <tuple>
#include <vector>

using namespace std;
using namespace std::placeholders;

class mult {
public:
    explicit mult(const int n) {
        m_p = new tuple<mult*, int>(this, n);

        verify();
    }

    mult(const mult& other) {
        other.verify();

        m_p = new tuple<mult*, int>(this, get<1>(*other.m_p));

        verify();
    }

    mult& operator=(const mult& other) {
        verify();
        other.verify();

        get<1>(*m_p) = get<1>(*other.m_p);

        verify();

        return *this;
    }

    ~mult() {
        verify();

        delete m_p;
    }

    int operator()(const int x, const int y) const {
        verify();

        return x * y * get<1>(*m_p);
    }

private:
    void verify() const {
        assert(m_p);
        assert(get<0>(*m_p) == this);
    }

    tuple<mult*, int>* m_p;
};

int add(int a, int b, int c, int d, int e) {
    return a + b + c + d + e;
}

void inspect(const function<int(int, int)>& f, const int val) {
    if (val != 0) {
        assert(f(3, 4) == val);
    } else {
        assert(!f);
    }
}

typedef function<int(int, int)> fxn_t;

void scribble(unsigned char* const p, const bool destroy = true) {
    if (destroy) {
        fxn_t* const f = static_cast<fxn_t*>(static_cast<void*>(p));

        f->~fxn_t();
    }

    for (size_t i = 0; i < sizeof(fxn_t); ++i) {
        p[i] = 0xCC;
    }
}

template <typename F, typename G>
void test(const F& f_orig, const G& g_orig, const int f_val, const int g_val) {
    unsigned char* const a = static_cast<unsigned char*>(malloc(sizeof(fxn_t)));
    unsigned char* const b = static_cast<unsigned char*>(malloc(sizeof(fxn_t)));

    scribble(a, false);
    scribble(b, false);

    {
        fxn_t* fp = new (static_cast<void*>(a)) fxn_t(f_orig);
        fxn_t* gp = new (static_cast<void*>(b)) fxn_t(g_orig);

        _Analysis_assume_(fp);
        _Analysis_assume_(gp);

        fxn_t& f = *fp;
        fxn_t& g = *gp;

        inspect(f, f_val);

        f.swap(g);

        scribble(b);

        inspect(f, g_val);

        scribble(a);
    }

    {
        fxn_t* fp = new (static_cast<void*>(a)) fxn_t(f_orig);
        fxn_t* gp = new (static_cast<void*>(b)) fxn_t(g_orig);

        _Analysis_assume_(fp);
        _Analysis_assume_(gp);

        fxn_t& f = *fp;
        fxn_t& g = *gp;

        inspect(g, g_val);

        f.swap(g);

        scribble(a);

        inspect(g, f_val);

        scribble(b);
    }

    free(a);
    free(b);
}


int global = 0;

void increment() {
    ++global;
}

void ten() {
    global *= 10;
}

int main() {
    test(mult(100), mult(200), 1200, 2400);
    test(mult(300), bind(add, _1, _2, 1000, 200, 30), 3600, 1237);
    test(bind(add, _1, _2, 2000, 300, 40), mult(400), 2347, 4800);
    test(bind(add, _1, _2, 3000, 400, 50), bind(add, _1, _2, 4000, 500, 60), 3457, 4567);
    test(mult(500), nullptr, 6000, 0);
    test(nullptr, mult(600), 0, 7200);
    test(bind(add, _1, _2, 5000, 600, 70), nullptr, 5677, 0);
    test(nullptr, bind(add, _1, _2, 6000, 700, 80), 0, 6787);
    test(nullptr, nullptr, 0, 0);

    assert(global == 0);

    vector<function<void()>> v;

    v.push_back(increment);
    v.push_back(increment);
    v.push_back(increment);
    v.push_back(increment);
    v.push_back(ten);
    v.push_back(increment);
    v.push_back(increment);
    v.push_back(increment);
    v.push_back(increment);
    v.push_back(increment);
    v.push_back(increment);
    v.push_back(increment);

    for (const auto& fn : v) {
        fn();
    }

    assert(global == 47);
}
