// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <cassert>
#include <complex>
#include <type_traits>
using namespace std;

template <typename F>
constexpr void test_type() {
    static_assert(is_floating_point_v<F>);

    constexpr F f1{1};
    constexpr F f2{2};
    constexpr F f3{3};
    constexpr F f4{4};

    complex<F> c{f1, f2};
    assert(c.real() == f1);
    assert(c.imag() == f2);
    assert((c == complex<F>{f1, f2}));

    c.real(f3);
    assert((c == complex<F>{f3, f2}));

    c.imag(f4);
    assert((c == complex<F>{f3, f4}));

    // Test GH-190 "<complex>: real(T) and imag(T) setters should return void".
    static_assert(is_void_v<decltype(c.real(f3))>);
    static_assert(is_void_v<decltype(c.imag(f4))>);

    c = f1;
    assert(c == complex<F>{f1});

    c.imag(f1);
    assert((c == complex<F>{f1, f1}));

    c += f2;
    assert((c == complex<F>{f3, f1}));

    c -= f1;
    assert((c == complex<F>{f2, f1}));

    c *= f2;
    assert((c == complex<F>{f4, f2}));

    c /= f2;
    assert((c == complex<F>{f2, f1}));

    c = complex<F>{f1, f3};
    assert((c == complex<F>{f1, f3}));

    c += complex<F>{f2, f1};
    assert((c == complex<F>{f3, f4}));

    c -= complex<F>{f1, f3};
    assert((c == complex<F>{f2, f1}));

    c *= complex<F>{f1, f1};
    assert((c == complex<F>{f1, f3}));

    c /= complex<F>{f1, f3};
#ifndef _M_CEE // TRANSITION, VSO-1665481
    assert(c == complex<F>{f1});
#endif // _M_CEE

    {
        using Other = conditional_t<is_same_v<F, float>, double, float>;

        c = complex<Other>{Other{1}, Other{3}};
        assert((c == complex<F>{f1, f3}));

        c += complex<Other>{Other{2}, Other{1}};
        assert((c == complex<F>{f3, f4}));

        c -= complex<Other>{Other{1}, Other{3}};
        assert((c == complex<F>{f2, f1}));

        c *= complex<Other>{Other{1}, Other{1}};
        assert((c == complex<F>{f1, f3}));

        c /= complex<Other>{Other{1}, Other{3}};
#ifndef _M_CEE // TRANSITION, VSO-1665481
        assert(c == complex<F>{f1});
#endif // _M_CEE
    }

    assert((complex<F>{f1, f1} + complex<F>{f1, f2} == complex<F>{f2, f3}));
    assert((complex<F>{f1, f1} + f1 == complex<F>{f2, f1}));
    assert((f1 + complex<F>{f2, f2} == complex<F>{f3, f2}));

    assert((complex<F>{f3, f4} - complex<F>{f1, f1} == complex<F>{f2, f3}));
    assert((complex<F>{f4, f4} - f2 == complex<F>{f2, f4}));
    assert((f2 - complex<F>{f1, f1} == complex<F>{f1, -f1}));

    assert((complex<F>{f1, f1} * complex<F>{f2, f1} == complex<F>{f1, f3}));
    assert((complex<F>{f1, f2} * f2 == complex<F>{f2, f4}));
    assert((f3 * complex<F>{f1, f1} == complex<F>{f3, f3}));

    assert((complex<F>{f4, f4} / complex<F>{f2, f2} == complex<F>{f2}));
    assert((complex<F>{f3, f3} / f3 == complex<F>{f1, f1}));
    assert((f1 / complex<F>{f1, f1} == complex<F>{F{0.5}, F{-0.5}}));

    assert((+complex<F>{f2, f3} == complex<F>{f2, f3}));

    assert((-complex<F>{f2, f3} == complex<F>{-f2, -f3}));

    assert((norm(complex<F>{f3, f4}) == F{25}));

    assert((conj(complex<F>{f3, f4}) == complex<F>{f3, -f4}));

    assert(real(f2) == f2);
    assert(imag(f2) == F{0});
    assert(norm(f2) == f4);
    assert(conj(f2) == f2);
}

constexpr bool test_all() {
    test_type<float>();
    test_type<double>();
    test_type<long double>();

    assert(real(2) == 2.0);
    assert(imag(2) == 0.0);
    assert(norm(2) == 4.0);
    assert(conj(2) == 2.0);

    return true;
}

int main() {
    assert(test_all());
    static_assert(test_all());
}
