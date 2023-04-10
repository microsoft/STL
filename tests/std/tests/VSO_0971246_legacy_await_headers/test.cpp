// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <cstdio>
#include <experimental/generator>
#include <future>
#include <optional>
#include <vector>

using namespace std::experimental;
using namespace std;

generator<int> g0() {
    co_return;
}

generator<int> g1() {
    co_yield 9;
}

generator<int> g2() {
    co_yield 9;
    co_yield 8;
}

generator<int> g3() {
    throw 42;
    co_return;
}

generator<int> g4() {
    co_yield 1;
    throw 42;
}

void dump(char const* name, vector<int> const& v, optional<int> e) {
    printf("%s: (", name);
    for (auto const& elem : v) {
        printf(" %d", elem);
    }
    if (e.has_value()) {
        printf(") except(%d)", *e);
    } else {
        printf(") noexcept");
    }
}

bool test_gen(char const* name, generator<int> g, vector<int> const& v, optional<int> e) {
    vector<int> r;
    optional<int> re;
    try {
        try {
            r.insert(r.end(), g.begin(), g.end());
        } catch (int evalue) {
            re = evalue;
        }
    } catch (...) {
        printf("%s: unexpected exception ...\n", name);
        return false;
    }
    if (re == e && r == v) {
        return true;
    }

    printf("unexpected result: ");
    dump(name, r, re);
    dump(" , expected", v, e);
    putchar('\n');
    return false;
}

#ifdef _RESUMABLE_FUNCTIONS_SUPPORTED
struct Immovable {
    int val;

    explicit Immovable(int v) : val(v) {}

    Immovable(Immovable const&)            = delete;
    Immovable& operator=(Immovable const&) = delete;
};

Immovable imm1(1);

future<Immovable&> fimm() {
    co_return imm1;
}

future<void> f0() {
    co_return;
}

future<int> f1() {
    co_await f0();
#if defined(__EDG__) // TRANSITION, VSO-974870
    // error: initial value of reference to non-const must be an lvalue
    // Immovable &r1 = co_await fimm();
    //                 ^
    auto& imm = imm1;
#else // ^^^ workaround / no workaround vvv
    auto& imm = co_await fimm();
#endif // TRANSITION, VSO-974870
    if (&imm != &imm1) {
        puts("&imm != &imm1");
        co_return 1984;
    }
    co_return imm.val;
}

future<int> f2() {
    auto x = f1();
    auto y = co_await f1();
    co_return co_await x + y + co_await f1() - 1;
}

future<int> f3() {
    throw 3;
    co_return 42;
}

future<int> f4() {
    co_await f0();
    throw 4;
    co_return 42;
}

void dump(char const* name, optional<int> v) {
    printf("%s: (", name);
    if (v) {
        printf("%d) ", *v);
    } else {
        printf(") ");
    }
}

void dump(char const* name, optional<int> v, optional<int> e) {
    dump(name, v);
    dump("except", e);
}

bool test_fut(char const* name, future<int> f, optional<int> v, optional<int> e) {
    optional<int> result;
    optional<int> except;
    try {
        result = f.get();
    } catch (int c) {
        except = c;
    } catch (...) {
        printf("%s: unexpected exception\n", name);
        return false;
    }

    if (result == v && except == e) {
        return true;
    }

    printf("unexpected result: ");
    dump(name, result, except);
    dump(" , expected", v, e);
    putchar('\n');
    return false;
}
#endif // _RESUMABLE_FUNCTIONS_SUPPORTED

bool test_all() {
    bool success = true;

    success = success && test_gen("g0", g0(), {}, nullopt);
    success = success && test_gen("g1", g1(), {9}, nullopt);
    success = success && test_gen("g2", g2(), {9, 8}, nullopt);
    success = success && test_gen("g3", g3(), {}, 42);
    success = success && test_gen("g4", g4(), {1}, 42);
#ifdef _RESUMABLE_FUNCTIONS_SUPPORTED
    puts("testing future adapters");
    success = success && test_fut("f1", f1(), 1, nullopt);
    success = success && test_fut("f2", f2(), 2, nullopt);
    success = success && test_fut("f3", f3(), nullopt, 3);
    success = success && test_fut("f4", f4(), nullopt, 4);
#endif // _RESUMABLE_FUNCTIONS_SUPPORTED

    return success;
}

int main() {
    bool success = test_all();
    puts(success ? "pass" : "fail");
    return !success;
}
