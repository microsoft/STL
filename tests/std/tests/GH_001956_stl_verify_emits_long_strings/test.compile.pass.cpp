// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <iostream>
#include <ranges>
#include <vector>

using namespace std;

namespace myVeryLongNamespace {

    struct A {
        double x;
        double getX() const {
            return x;
        }

        template <ranges::range RangeOfA>
        static double getFirstValue(RangeOfA&& r) {
            auto getValue = [](const A& a) -> double { return a.getX(); };
            auto x_range  = r | views::transform(getValue);
            return *begin(x_range);
        }
    };

    struct B {
        A a;
        const A& getA() const {
            return a;
        }

        template <ranges::range RangeOfB>
        static double getFirstValue(RangeOfB&& r) {
            auto getValue = [](const B& b) -> const auto& {
                return b.getA();
            };
            auto b_range = r | views::transform(getValue);
            return A::getFirstValue(b_range);
        }
    };

    struct C {
        B b;
        const B& getB() const {
            return b;
        }

        template <ranges::range RangeOfC>
        static double getFirstValue(RangeOfC&& r) {
            auto getValue = [](const C& c) -> const auto& {
                return c.getB();
            };
            auto c_range = r | views::transform(getValue);
            return B::getFirstValue(c_range);
        }
    };

    struct D {
        C c;
        const C& getC() const {
            return c;
        }

        template <ranges::range RangeOfD>
        static double getFirstValue(RangeOfD&& r) {
            auto getValue = [](const D& d) -> const auto& {
                return d.getC();
            };
            auto d_range = r | views::transform(getValue);
            return C::getFirstValue(d_range);
        }
    };

} // namespace myVeryLongNamespace

void test() {
    using namespace myVeryLongNamespace;
    auto a = A{1};
    auto b = B{a};
    auto c = C{b};
    auto d = D{c};

    auto d_range = vector{d, d, d};

    auto isPositive = [](const auto& d) { return d.c.b.a.x > 0.0; };

    auto filtered_range = d_range | views::filter(isPositive) | views::filter(isPositive) | views::filter(isPositive);

    auto result = D::getFirstValue(filtered_range);
    cout << result << endl;
}

int main() {} // COMPILE-ONLY
