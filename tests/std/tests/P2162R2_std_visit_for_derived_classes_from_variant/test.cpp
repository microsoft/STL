// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <cassert>
#include <memory>
#include <utility>
#include <variant>

using namespace std;

struct Disconnected {
    int val;
};

struct Connecting {
    char val;
};

struct Connected {
    double val;
};

struct State : variant<Disconnected, Connecting, Connected> {
    using variant::variant;
};


void example1_from_p2162r2() {
    State v1       = Disconnected{45};
    const State v2 = Connecting{'d'};
    visit([](auto x) { assert(x.val == 45); }, v1);
    visit([](auto x) { assert(x.val == 'd'); }, v2);
    visit([](auto x) { assert(x.val == 5.5); }, State{Connected{5.5}});
    visit([](auto x) { assert(x.val == 45); }, move(v1));
    visit([](auto x) { assert(x.val == 'd'); }, move(v2));
}


struct Expr;

struct Neg {
    shared_ptr<Expr> expr;
};

struct Add {
    shared_ptr<Expr> lhs, rhs;
};

struct Mul {
    shared_ptr<Expr> lhs, rhs;
};

struct Expr : variant<int, Neg, Add, Mul> {
    using variant::variant;
};

namespace std {
    template <>
    struct variant_size<Expr> : variant_size<Expr::variant> {};

    template <size_t I>
    struct variant_alternative<I, Expr> : variant_alternative<I, Expr::variant> {};
} // namespace std

int eval(const Expr& expr) {
    struct visitor {
        int operator()(int i) const {
            return i;
        }
        int operator()(const Neg& n) const {
            return -eval(*n.expr);
        }
        int operator()(const Add& a) const {
            return eval(*a.lhs) + eval(*a.rhs);
        }
        int operator()(const Mul& m) const {
            // Optimize multiplication by 0.
            if (int* i = get_if<int>(m.lhs.get()); i && *i == 0) {
                return 0;
            }
            if (int* i = get_if<int>(m.rhs.get()); i && *i == 0) {
                return 0;
            }
            return eval(*m.lhs) * eval(*m.rhs);
        }
    };
    return visit(visitor{}, expr);
}


void example2_from_p2162r2() {
    // (1) + (3*2)
    const Expr e = Add{make_shared<Expr>(1), make_shared<Expr>(Mul{make_shared<Expr>(2), make_shared<Expr>(3)})};
    assert(eval(e) == (1 + 3 * 2));
}


int main() {
    example1_from_p2162r2();
    example2_from_p2162r2();
}
