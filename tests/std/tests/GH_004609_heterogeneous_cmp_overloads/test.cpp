// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <cassert>
#include <cmath>
#include <limits>
#include <math.h>
#include <type_traits>

template <class F1, class F2>
void test_heterogeneous_floating_overloads() {
    constexpr F1 lhs   = static_cast<F1>(3.14);
    constexpr F1 lhseq = static_cast<F1>(23.5);
    constexpr F1 linf  = std::numeric_limits<F1>::infinity();
    constexpr F1 lnan  = std::numeric_limits<F1>::quiet_NaN();

    constexpr F2 rhs   = static_cast<F2>(17.29);
    constexpr F2 rhseq = static_cast<F2>(23.5);
    constexpr F2 rinf  = std::numeric_limits<F2>::infinity();
    constexpr F2 rnan  = std::numeric_limits<F2>::quiet_NaN();

    // test overloads in std

    assert(!std::isgreater(lhs, rhs));
    assert(!std::isgreater(lhs, rinf));
    assert(std::isgreater(lhs, -rinf));
    assert(std::isgreater(rhs, lhs));
    assert(!std::isgreater(rhs, linf));
    assert(std::isgreater(rhs, -linf));
    assert(!std::isgreater(lhseq, rhseq));
    assert(!std::isgreater(rhseq, lhseq));
    assert(!std::isgreater(linf, rinf));
    assert(!std::isgreater(rinf, linf));
    assert(!std::isgreater(lnan, rhs));
    assert(!std::isgreater(lnan, rinf));
    assert(!std::isgreater(lnan, -rinf));
    assert(!std::isgreater(lnan, rnan));
    assert(!std::isgreater(rnan, lhs));
    assert(!std::isgreater(rnan, linf));
    assert(!std::isgreater(rnan, -linf));
    assert(!std::isgreater(rnan, lnan));

    assert(!std::isgreaterequal(lhs, rhs));
    assert(!std::isgreaterequal(lhs, rinf));
    assert(std::isgreaterequal(lhs, -rinf));
    assert(std::isgreaterequal(rhs, lhs));
    assert(!std::isgreaterequal(rhs, linf));
    assert(std::isgreaterequal(rhs, -linf));
    assert(std::isgreaterequal(lhseq, rhseq));
    assert(std::isgreaterequal(rhseq, lhseq));
    assert(std::isgreaterequal(linf, rinf));
    assert(std::isgreaterequal(rinf, linf));
    assert(!std::isgreaterequal(lnan, rhs));
    assert(!std::isgreaterequal(lnan, rinf));
    assert(!std::isgreaterequal(lnan, -rinf));
    assert(!std::isgreaterequal(lnan, rnan));
    assert(!std::isgreaterequal(rnan, lhs));
    assert(!std::isgreaterequal(rnan, linf));
    assert(!std::isgreaterequal(rnan, -linf));
    assert(!std::isgreaterequal(rnan, lnan));

    assert(std::isless(lhs, rhs));
    assert(std::isless(lhs, rinf));
    assert(!std::isless(lhs, -rinf));
    assert(!std::isless(rhs, lhs));
    assert(std::isless(rhs, linf));
    assert(!std::isless(rhs, -linf));
    assert(!std::isless(lhseq, rhseq));
    assert(!std::isless(rhseq, lhseq));
    assert(!std::isless(linf, rinf));
    assert(!std::isless(rinf, linf));
    assert(!std::isless(lnan, rhs));
    assert(!std::isless(lnan, rinf));
    assert(!std::isless(lnan, -rinf));
    assert(!std::isless(lnan, rnan));
    assert(!std::isless(rnan, lhs));
    assert(!std::isless(rnan, linf));
    assert(!std::isless(rnan, -linf));
    assert(!std::isless(rnan, lnan));

    assert(std::islessequal(lhs, rhs));
    assert(std::islessequal(lhs, rinf));
    assert(!std::islessequal(lhs, -rinf));
    assert(!std::islessequal(rhs, lhs));
    assert(std::islessequal(rhs, linf));
    assert(!std::islessequal(rhs, -linf));
    assert(std::islessequal(lhseq, rhseq));
    assert(std::islessequal(rhseq, lhseq));
    assert(std::islessequal(linf, rinf));
    assert(std::islessequal(rinf, linf));
    assert(!std::islessequal(lnan, rhs));
    assert(!std::islessequal(lnan, rinf));
    assert(!std::islessequal(lnan, -rinf));
    assert(!std::islessequal(lnan, rnan));
    assert(!std::islessequal(rnan, lhs));
    assert(!std::islessequal(rnan, linf));
    assert(!std::islessequal(rnan, -linf));
    assert(!std::islessequal(rnan, lnan));

    assert(std::islessgreater(lhs, rhs));
    assert(std::islessgreater(lhs, rinf));
    assert(std::islessgreater(lhs, -rinf));
    assert(std::islessgreater(rhs, lhs));
    assert(std::islessgreater(rhs, linf));
    assert(std::islessgreater(rhs, -linf));
    assert(!std::islessgreater(lhseq, rhseq));
    assert(!std::islessgreater(rhseq, lhseq));
    assert(!std::islessgreater(linf, rinf));
    assert(!std::islessgreater(rinf, linf));
    assert(!std::islessgreater(lnan, rhs));
    assert(!std::islessgreater(lnan, rinf));
    assert(!std::islessgreater(lnan, -rinf));
    assert(!std::islessgreater(lnan, rnan));
    assert(!std::islessgreater(rnan, lhs));
    assert(!std::islessgreater(rnan, linf));
    assert(!std::islessgreater(rnan, -linf));
    assert(!std::islessgreater(rnan, lnan));

    assert(!std::isunordered(lhs, rhs));
    assert(!std::isunordered(lhs, rinf));
    assert(!std::isunordered(lhs, -rinf));
    assert(!std::isunordered(rhs, lhs));
    assert(!std::isunordered(rhs, linf));
    assert(!std::isunordered(rhs, -linf));
    assert(!std::isunordered(lhseq, rhseq));
    assert(!std::isunordered(rhseq, lhseq));
    assert(!std::isunordered(linf, rinf));
    assert(!std::isunordered(rinf, linf));
    assert(std::isunordered(lnan, rhs));
    assert(std::isunordered(lnan, rinf));
    assert(std::isunordered(lnan, -rinf));
    assert(std::isunordered(lnan, rnan));
    assert(std::isunordered(rnan, lhs));
    assert(std::isunordered(rnan, linf));
    assert(std::isunordered(rnan, -linf));
    assert(std::isunordered(rnan, lnan));

    // test overloads in the global namespace

    assert(!::isgreater(lhs, rhs));
    assert(!::isgreater(lhs, rinf));
    assert(::isgreater(lhs, -rinf));
    assert(::isgreater(rhs, lhs));
    assert(!::isgreater(rhs, linf));
    assert(::isgreater(rhs, -linf));
    assert(!::isgreater(lhseq, rhseq));
    assert(!::isgreater(rhseq, lhseq));
    assert(!::isgreater(linf, rinf));
    assert(!::isgreater(rinf, linf));
    assert(!::isgreater(lnan, rhs));
    assert(!::isgreater(lnan, rinf));
    assert(!::isgreater(lnan, -rinf));
    assert(!::isgreater(lnan, rnan));
    assert(!::isgreater(rnan, lhs));
    assert(!::isgreater(rnan, linf));
    assert(!::isgreater(rnan, -linf));
    assert(!::isgreater(rnan, lnan));

    assert(!::isgreaterequal(lhs, rhs));
    assert(!::isgreaterequal(lhs, rinf));
    assert(::isgreaterequal(lhs, -rinf));
    assert(::isgreaterequal(rhs, lhs));
    assert(!::isgreaterequal(rhs, linf));
    assert(::isgreaterequal(rhs, -linf));
    assert(::isgreaterequal(lhseq, rhseq));
    assert(::isgreaterequal(rhseq, lhseq));
    assert(::isgreaterequal(linf, rinf));
    assert(::isgreaterequal(rinf, linf));
    assert(!::isgreaterequal(lnan, rhs));
    assert(!::isgreaterequal(lnan, rinf));
    assert(!::isgreaterequal(lnan, -rinf));
    assert(!::isgreaterequal(lnan, rnan));
    assert(!::isgreaterequal(rnan, lhs));
    assert(!::isgreaterequal(rnan, linf));
    assert(!::isgreaterequal(rnan, -linf));
    assert(!::isgreaterequal(rnan, lnan));

    assert(::isless(lhs, rhs));
    assert(::isless(lhs, rinf));
    assert(!::isless(lhs, -rinf));
    assert(!::isless(rhs, lhs));
    assert(::isless(rhs, linf));
    assert(!::isless(rhs, -linf));
    assert(!::isless(lhseq, rhseq));
    assert(!::isless(rhseq, lhseq));
    assert(!::isless(linf, rinf));
    assert(!::isless(rinf, linf));
    assert(!::isless(lnan, rhs));
    assert(!::isless(lnan, rinf));
    assert(!::isless(lnan, -rinf));
    assert(!::isless(lnan, rnan));
    assert(!::isless(rnan, lhs));
    assert(!::isless(rnan, linf));
    assert(!::isless(rnan, -linf));
    assert(!::isless(rnan, lnan));

    assert(::islessequal(lhs, rhs));
    assert(::islessequal(lhs, rinf));
    assert(!::islessequal(lhs, -rinf));
    assert(!::islessequal(rhs, lhs));
    assert(::islessequal(rhs, linf));
    assert(!::islessequal(rhs, -linf));
    assert(::islessequal(lhseq, rhseq));
    assert(::islessequal(rhseq, lhseq));
    assert(::islessequal(linf, rinf));
    assert(::islessequal(rinf, linf));
    assert(!::islessequal(lnan, rhs));
    assert(!::islessequal(lnan, rinf));
    assert(!::islessequal(lnan, -rinf));
    assert(!::islessequal(lnan, rnan));
    assert(!::islessequal(rnan, lhs));
    assert(!::islessequal(rnan, linf));
    assert(!::islessequal(rnan, -linf));
    assert(!::islessequal(rnan, lnan));

    assert(::islessgreater(lhs, rhs));
    assert(::islessgreater(lhs, rinf));
    assert(::islessgreater(lhs, -rinf));
    assert(::islessgreater(rhs, lhs));
    assert(::islessgreater(rhs, linf));
    assert(::islessgreater(rhs, -linf));
    assert(!::islessgreater(lhseq, rhseq));
    assert(!::islessgreater(rhseq, lhseq));
    assert(!::islessgreater(linf, rinf));
    assert(!::islessgreater(rinf, linf));
    assert(!::islessgreater(lnan, rhs));
    assert(!::islessgreater(lnan, rinf));
    assert(!::islessgreater(lnan, -rinf));
    assert(!::islessgreater(lnan, rnan));
    assert(!::islessgreater(rnan, lhs));
    assert(!::islessgreater(rnan, linf));
    assert(!::islessgreater(rnan, -linf));
    assert(!::islessgreater(rnan, lnan));

    assert(!::isunordered(lhs, rhs));
    assert(!::isunordered(lhs, rinf));
    assert(!::isunordered(lhs, -rinf));
    assert(!::isunordered(rhs, lhs));
    assert(!::isunordered(rhs, linf));
    assert(!::isunordered(rhs, -linf));
    assert(!::isunordered(lhseq, rhseq));
    assert(!::isunordered(rhseq, lhseq));
    assert(!::isunordered(linf, rinf));
    assert(!::isunordered(rinf, linf));
    assert(::isunordered(lnan, rhs));
    assert(::isunordered(lnan, rinf));
    assert(::isunordered(lnan, -rinf));
    assert(::isunordered(lnan, rnan));
    assert(::isunordered(rnan, lhs));
    assert(::isunordered(rnan, linf));
    assert(::isunordered(rnan, -linf));
    assert(::isunordered(rnan, lnan));
}

template <class F, class I>
void test_heterogeneous_mixed_overloads_per_integer_type() {
    constexpr F lhs  = static_cast<F>(42.0);
    constexpr F linf = std::numeric_limits<F>::infinity();
    constexpr F lnan = std::numeric_limits<F>::quiet_NaN();

    constexpr I rhslt{41};
    constexpr I rhseq{42};
    constexpr I rhsgt{43};

    // test overloads in std

    assert(std::isgreater(lhs, rhslt));
    assert(!std::isgreater(lhs, rhseq));
    assert(!std::isgreater(lhs, rhsgt));
    assert(std::isgreater(linf, rhseq));
    assert(!std::isgreater(-linf, rhseq));
    assert(!std::isgreater(rhslt, lhs));
    assert(!std::isgreater(rhseq, lhs));
    assert(std::isgreater(rhsgt, lhs));
    assert(!std::isgreater(rhseq, linf));
    assert(std::isgreater(rhseq, -linf));
    assert(!std::isgreater(lnan, rhseq));
    assert(!std::isgreater(rhseq, lnan));

    assert(std::isgreaterequal(lhs, rhslt));
    assert(std::isgreaterequal(lhs, rhseq));
    assert(!std::isgreaterequal(lhs, rhsgt));
    assert(std::isgreaterequal(linf, rhseq));
    assert(!std::isgreaterequal(-linf, rhseq));
    assert(!std::isgreaterequal(rhslt, lhs));
    assert(std::isgreaterequal(rhseq, lhs));
    assert(std::isgreaterequal(rhsgt, lhs));
    assert(!std::isgreaterequal(rhseq, linf));
    assert(std::isgreaterequal(rhseq, -linf));
    assert(!std::isgreaterequal(lnan, rhseq));
    assert(!std::isgreaterequal(rhseq, lnan));

    assert(!std::isless(lhs, rhslt));
    assert(!std::isless(lhs, rhseq));
    assert(std::isless(lhs, rhsgt));
    assert(!std::isless(linf, rhseq));
    assert(std::isless(-linf, rhseq));
    assert(std::isless(rhslt, lhs));
    assert(!std::isless(rhseq, lhs));
    assert(!std::isless(rhsgt, lhs));
    assert(std::isless(rhseq, linf));
    assert(!std::isless(rhseq, -linf));
    assert(!std::isless(lnan, rhseq));
    assert(!std::isless(rhseq, lnan));

    assert(!std::islessequal(lhs, rhslt));
    assert(std::islessequal(lhs, rhseq));
    assert(std::islessequal(lhs, rhsgt));
    assert(!std::islessequal(linf, rhseq));
    assert(std::islessequal(-linf, rhseq));
    assert(std::islessequal(rhslt, lhs));
    assert(std::islessequal(rhseq, lhs));
    assert(!std::islessequal(rhsgt, lhs));
    assert(std::islessequal(rhseq, linf));
    assert(!std::islessequal(rhseq, -linf));
    assert(!std::islessequal(lnan, rhseq));
    assert(!std::islessequal(rhseq, lnan));

    assert(std::islessgreater(lhs, rhslt));
    assert(!std::islessgreater(lhs, rhseq));
    assert(std::islessgreater(lhs, rhsgt));
    assert(std::islessgreater(linf, rhseq));
    assert(std::islessgreater(-linf, rhseq));
    assert(std::islessgreater(rhslt, lhs));
    assert(!std::islessgreater(rhseq, lhs));
    assert(std::islessgreater(rhsgt, lhs));
    assert(std::islessgreater(rhseq, linf));
    assert(std::islessgreater(rhseq, -linf));
    assert(!std::islessgreater(lnan, rhseq));
    assert(!std::islessgreater(rhseq, lnan));

    assert(!std::isunordered(lhs, rhslt));
    assert(!std::isunordered(lhs, rhseq));
    assert(!std::isunordered(lhs, rhsgt));
    assert(!std::isunordered(linf, rhseq));
    assert(!std::isunordered(-linf, rhseq));
    assert(!std::isunordered(rhslt, lhs));
    assert(!std::isunordered(rhseq, lhs));
    assert(!std::isunordered(rhsgt, lhs));
    assert(!std::isunordered(rhseq, linf));
    assert(!std::isunordered(rhseq, -linf));
    assert(std::isunordered(lnan, rhseq));
    assert(std::isunordered(rhseq, lnan));

    // test overloads in the global namespace

    assert(::isgreater(lhs, rhslt));
    assert(!::isgreater(lhs, rhseq));
    assert(!::isgreater(lhs, rhsgt));
    assert(::isgreater(linf, rhseq));
    assert(!::isgreater(-linf, rhseq));
    assert(!::isgreater(rhslt, lhs));
    assert(!::isgreater(rhseq, lhs));
    assert(::isgreater(rhsgt, lhs));
    assert(!::isgreater(rhseq, linf));
    assert(::isgreater(rhseq, -linf));
    assert(!::isgreater(lnan, rhseq));
    assert(!::isgreater(rhseq, lnan));

    assert(::isgreaterequal(lhs, rhslt));
    assert(::isgreaterequal(lhs, rhseq));
    assert(!::isgreaterequal(lhs, rhsgt));
    assert(::isgreaterequal(linf, rhseq));
    assert(!::isgreaterequal(-linf, rhseq));
    assert(!::isgreaterequal(rhslt, lhs));
    assert(::isgreaterequal(rhseq, lhs));
    assert(::isgreaterequal(rhsgt, lhs));
    assert(!::isgreaterequal(rhseq, linf));
    assert(::isgreaterequal(rhseq, -linf));
    assert(!::isgreaterequal(lnan, rhseq));
    assert(!::isgreaterequal(rhseq, lnan));

    assert(!::isless(lhs, rhslt));
    assert(!::isless(lhs, rhseq));
    assert(::isless(lhs, rhsgt));
    assert(!::isless(linf, rhseq));
    assert(::isless(-linf, rhseq));
    assert(::isless(rhslt, lhs));
    assert(!::isless(rhseq, lhs));
    assert(!::isless(rhsgt, lhs));
    assert(::isless(rhseq, linf));
    assert(!::isless(rhseq, -linf));
    assert(!::isless(lnan, rhseq));
    assert(!::isless(rhseq, lnan));

    assert(!::islessequal(lhs, rhslt));
    assert(::islessequal(lhs, rhseq));
    assert(::islessequal(lhs, rhsgt));
    assert(!::islessequal(linf, rhseq));
    assert(::islessequal(-linf, rhseq));
    assert(::islessequal(rhslt, lhs));
    assert(::islessequal(rhseq, lhs));
    assert(!::islessequal(rhsgt, lhs));
    assert(::islessequal(rhseq, linf));
    assert(!::islessequal(rhseq, -linf));
    assert(!::islessequal(lnan, rhseq));
    assert(!::islessequal(rhseq, lnan));

    assert(::islessgreater(lhs, rhslt));
    assert(!::islessgreater(lhs, rhseq));
    assert(::islessgreater(lhs, rhsgt));
    assert(::islessgreater(linf, rhseq));
    assert(::islessgreater(-linf, rhseq));
    assert(::islessgreater(rhslt, lhs));
    assert(!::islessgreater(rhseq, lhs));
    assert(::islessgreater(rhsgt, lhs));
    assert(::islessgreater(rhseq, linf));
    assert(::islessgreater(rhseq, -linf));
    assert(!::islessgreater(lnan, rhseq));
    assert(!::islessgreater(rhseq, lnan));

    assert(!::isunordered(lhs, rhslt));
    assert(!::isunordered(lhs, rhseq));
    assert(!::isunordered(lhs, rhsgt));
    assert(!::isunordered(linf, rhseq));
    assert(!::isunordered(-linf, rhseq));
    assert(!::isunordered(rhslt, lhs));
    assert(!::isunordered(rhseq, lhs));
    assert(!::isunordered(rhsgt, lhs));
    assert(!::isunordered(rhseq, linf));
    assert(!::isunordered(rhseq, -linf));
    assert(::isunordered(lnan, rhseq));
    assert(::isunordered(rhseq, lnan));
}

template <class F>
void test_heterogeneous_mixed_overloads_per_fp_type() {
    test_heterogeneous_mixed_overloads_per_integer_type<F, signed char>();
    test_heterogeneous_mixed_overloads_per_integer_type<F, short>();
    test_heterogeneous_mixed_overloads_per_integer_type<F, int>();
    test_heterogeneous_mixed_overloads_per_integer_type<F, long>();
    test_heterogeneous_mixed_overloads_per_integer_type<F, long long>();
    test_heterogeneous_mixed_overloads_per_integer_type<F, unsigned char>();
    test_heterogeneous_mixed_overloads_per_integer_type<F, unsigned short>();
    test_heterogeneous_mixed_overloads_per_integer_type<F, unsigned int>();
    test_heterogeneous_mixed_overloads_per_integer_type<F, unsigned long>();
    test_heterogeneous_mixed_overloads_per_integer_type<F, unsigned long long>();
    test_heterogeneous_mixed_overloads_per_integer_type<F, char>();
#ifdef __cpp_char8_t
    test_heterogeneous_mixed_overloads_per_integer_type<F, char8_t>();
#endif // defined(__cpp_char8_t)
    test_heterogeneous_mixed_overloads_per_integer_type<F, char16_t>();
    test_heterogeneous_mixed_overloads_per_integer_type<F, char32_t>();
    test_heterogeneous_mixed_overloads_per_integer_type<F, wchar_t>();
}

template <class F>
void test_heterogeneous_mixed_overloads_with_bool() {
    constexpr F f0{};
    constexpr F f1   = static_cast<F>(1.0);
    constexpr F fmid = static_cast<F>(0.5);
    constexpr F finf = std::numeric_limits<F>::infinity();
    constexpr F fnan = std::numeric_limits<F>::quiet_NaN();

    // test overloads in std

    assert(!std::isgreater(f0, false));
    assert(!std::isgreater(f1, true));
    assert(std::isgreater(fmid, false));
    assert(!std::isgreater(fmid, true));
    assert(std::isgreater(finf, false));
    assert(std::isgreater(finf, true));
    assert(!std::isgreater(-finf, false));
    assert(!std::isgreater(-finf, true));
    assert(!std::isgreater(false, f0));
    assert(!std::isgreater(true, f1));
    assert(!std::isgreater(false, fmid));
    assert(std::isgreater(true, fmid));
    assert(!std::isgreater(false, finf));
    assert(!std::isgreater(true, finf));
    assert(std::isgreater(false, -finf));
    assert(std::isgreater(true, -finf));
    assert(!std::isgreater(fnan, false));
    assert(!std::isgreater(fnan, true));
    assert(!std::isgreater(false, fnan));
    assert(!std::isgreater(true, fnan));

    assert(std::isgreaterequal(f0, false));
    assert(std::isgreaterequal(f1, true));
    assert(std::isgreaterequal(fmid, false));
    assert(!std::isgreaterequal(fmid, true));
    assert(std::isgreaterequal(finf, false));
    assert(std::isgreaterequal(finf, true));
    assert(!std::isgreaterequal(-finf, false));
    assert(!std::isgreaterequal(-finf, true));
    assert(std::isgreaterequal(false, f0));
    assert(std::isgreaterequal(true, f1));
    assert(!std::isgreaterequal(false, fmid));
    assert(std::isgreaterequal(true, fmid));
    assert(!std::isgreaterequal(false, finf));
    assert(!std::isgreaterequal(true, finf));
    assert(std::isgreaterequal(false, -finf));
    assert(std::isgreaterequal(true, -finf));
    assert(!std::isgreaterequal(fnan, false));
    assert(!std::isgreaterequal(fnan, true));
    assert(!std::isgreaterequal(false, fnan));
    assert(!std::isgreaterequal(true, fnan));

    assert(!std::isless(f0, false));
    assert(!std::isless(f1, true));
    assert(!std::isless(fmid, false));
    assert(std::isless(fmid, true));
    assert(!std::isless(finf, false));
    assert(!std::isless(finf, true));
    assert(std::isless(-finf, false));
    assert(std::isless(-finf, true));
    assert(!std::isless(false, f0));
    assert(!std::isless(true, f1));
    assert(std::isless(false, fmid));
    assert(!std::isless(true, fmid));
    assert(std::isless(false, finf));
    assert(std::isless(true, finf));
    assert(!std::isless(false, -finf));
    assert(!std::isless(true, -finf));
    assert(!std::isless(fnan, false));
    assert(!std::isless(fnan, true));
    assert(!std::isless(false, fnan));
    assert(!std::isless(true, fnan));

    assert(std::islessequal(f0, false));
    assert(std::islessequal(f1, true));
    assert(!std::islessequal(fmid, false));
    assert(std::islessequal(fmid, true));
    assert(!std::islessequal(finf, false));
    assert(!std::islessequal(finf, true));
    assert(std::islessequal(-finf, false));
    assert(std::islessequal(-finf, true));
    assert(std::islessequal(false, f0));
    assert(std::islessequal(true, f1));
    assert(std::islessequal(false, fmid));
    assert(!std::islessequal(true, fmid));
    assert(std::islessequal(false, finf));
    assert(std::islessequal(true, finf));
    assert(!std::islessequal(false, -finf));
    assert(!std::islessequal(true, -finf));
    assert(!std::islessequal(fnan, false));
    assert(!std::islessequal(fnan, true));
    assert(!std::islessequal(false, fnan));
    assert(!std::islessequal(true, fnan));

    assert(!std::islessgreater(f0, false));
    assert(!std::islessgreater(f1, true));
    assert(std::islessgreater(fmid, false));
    assert(std::islessgreater(fmid, true));
    assert(std::islessgreater(finf, false));
    assert(std::islessgreater(finf, true));
    assert(std::islessgreater(-finf, false));
    assert(std::islessgreater(-finf, true));
    assert(!std::islessgreater(false, f0));
    assert(!std::islessgreater(true, f1));
    assert(std::islessgreater(false, fmid));
    assert(std::islessgreater(true, fmid));
    assert(std::islessgreater(false, finf));
    assert(std::islessgreater(true, finf));
    assert(std::islessgreater(false, -finf));
    assert(std::islessgreater(true, -finf));
    assert(!std::islessgreater(fnan, false));
    assert(!std::islessgreater(fnan, true));
    assert(!std::islessgreater(false, fnan));
    assert(!std::islessgreater(true, fnan));

    assert(!std::isunordered(f0, false));
    assert(!std::isunordered(f1, true));
    assert(!std::isunordered(fmid, false));
    assert(!std::isunordered(fmid, true));
    assert(!std::isunordered(finf, false));
    assert(!std::isunordered(finf, true));
    assert(!std::isunordered(-finf, false));
    assert(!std::isunordered(-finf, true));
    assert(!std::isunordered(false, f0));
    assert(!std::isunordered(true, f1));
    assert(!std::isunordered(false, fmid));
    assert(!std::isunordered(true, fmid));
    assert(!std::isunordered(false, finf));
    assert(!std::isunordered(true, finf));
    assert(!std::isunordered(false, -finf));
    assert(!std::isunordered(true, -finf));
    assert(std::isunordered(fnan, false));
    assert(std::isunordered(fnan, true));
    assert(std::isunordered(false, fnan));
    assert(std::isunordered(true, fnan));

    // test overloads in the global namespace

    assert(!::isgreater(f0, false));
    assert(!::isgreater(f1, true));
    assert(::isgreater(fmid, false));
    assert(!::isgreater(fmid, true));
    assert(::isgreater(finf, false));
    assert(::isgreater(finf, true));
    assert(!::isgreater(-finf, false));
    assert(!::isgreater(-finf, true));
    assert(!::isgreater(false, f0));
    assert(!::isgreater(true, f1));
    assert(!::isgreater(false, fmid));
    assert(::isgreater(true, fmid));
    assert(!::isgreater(false, finf));
    assert(!::isgreater(true, finf));
    assert(::isgreater(false, -finf));
    assert(::isgreater(true, -finf));
    assert(!::isgreater(fnan, false));
    assert(!::isgreater(fnan, true));
    assert(!::isgreater(false, fnan));
    assert(!::isgreater(true, fnan));

    assert(::isgreaterequal(f0, false));
    assert(::isgreaterequal(f1, true));
    assert(::isgreaterequal(fmid, false));
    assert(!::isgreaterequal(fmid, true));
    assert(::isgreaterequal(finf, false));
    assert(::isgreaterequal(finf, true));
    assert(!::isgreaterequal(-finf, false));
    assert(!::isgreaterequal(-finf, true));
    assert(::isgreaterequal(false, f0));
    assert(::isgreaterequal(true, f1));
    assert(!::isgreaterequal(false, fmid));
    assert(::isgreaterequal(true, fmid));
    assert(!::isgreaterequal(false, finf));
    assert(!::isgreaterequal(true, finf));
    assert(::isgreaterequal(false, -finf));
    assert(::isgreaterequal(true, -finf));
    assert(!::isgreaterequal(fnan, false));
    assert(!::isgreaterequal(fnan, true));
    assert(!::isgreaterequal(false, fnan));
    assert(!::isgreaterequal(true, fnan));

    assert(!::isless(f0, false));
    assert(!::isless(f1, true));
    assert(!::isless(fmid, false));
    assert(::isless(fmid, true));
    assert(!::isless(finf, false));
    assert(!::isless(finf, true));
    assert(::isless(-finf, false));
    assert(::isless(-finf, true));
    assert(!::isless(false, f0));
    assert(!::isless(true, f1));
    assert(::isless(false, fmid));
    assert(!::isless(true, fmid));
    assert(::isless(false, finf));
    assert(::isless(true, finf));
    assert(!::isless(false, -finf));
    assert(!::isless(true, -finf));
    assert(!::isless(fnan, false));
    assert(!::isless(fnan, true));
    assert(!::isless(false, fnan));
    assert(!::isless(true, fnan));

    assert(::islessequal(f0, false));
    assert(::islessequal(f1, true));
    assert(!::islessequal(fmid, false));
    assert(::islessequal(fmid, true));
    assert(!::islessequal(finf, false));
    assert(!::islessequal(finf, true));
    assert(::islessequal(-finf, false));
    assert(::islessequal(-finf, true));
    assert(::islessequal(false, f0));
    assert(::islessequal(true, f1));
    assert(::islessequal(false, fmid));
    assert(!::islessequal(true, fmid));
    assert(::islessequal(false, finf));
    assert(::islessequal(true, finf));
    assert(!::islessequal(false, -finf));
    assert(!::islessequal(true, -finf));
    assert(!::islessequal(fnan, false));
    assert(!::islessequal(fnan, true));
    assert(!::islessequal(false, fnan));
    assert(!::islessequal(true, fnan));

    assert(!::islessgreater(f0, false));
    assert(!::islessgreater(f1, true));
    assert(::islessgreater(fmid, false));
    assert(::islessgreater(fmid, true));
    assert(::islessgreater(finf, false));
    assert(::islessgreater(finf, true));
    assert(::islessgreater(-finf, false));
    assert(::islessgreater(-finf, true));
    assert(!::islessgreater(false, f0));
    assert(!::islessgreater(true, f1));
    assert(::islessgreater(false, fmid));
    assert(::islessgreater(true, fmid));
    assert(::islessgreater(false, finf));
    assert(::islessgreater(true, finf));
    assert(::islessgreater(false, -finf));
    assert(::islessgreater(true, -finf));
    assert(!::islessgreater(fnan, false));
    assert(!::islessgreater(fnan, true));
    assert(!::islessgreater(false, fnan));
    assert(!::islessgreater(true, fnan));

    assert(!::isunordered(f0, false));
    assert(!::isunordered(f1, true));
    assert(!::isunordered(fmid, false));
    assert(!::isunordered(fmid, true));
    assert(!::isunordered(finf, false));
    assert(!::isunordered(finf, true));
    assert(!::isunordered(-finf, false));
    assert(!::isunordered(-finf, true));
    assert(!::isunordered(false, f0));
    assert(!::isunordered(true, f1));
    assert(!::isunordered(false, fmid));
    assert(!::isunordered(true, fmid));
    assert(!::isunordered(false, finf));
    assert(!::isunordered(true, finf));
    assert(!::isunordered(false, -finf));
    assert(!::isunordered(true, -finf));
    assert(::isunordered(fnan, false));
    assert(::isunordered(fnan, true));
    assert(::isunordered(false, fnan));
    assert(::isunordered(true, fnan));
}

void test_all_heterogeneous_floating_overloads() {
    test_heterogeneous_floating_overloads<float, double>();
    test_heterogeneous_floating_overloads<float, long double>();
    test_heterogeneous_floating_overloads<double, float>();
    test_heterogeneous_floating_overloads<double, long double>();
    test_heterogeneous_floating_overloads<long double, float>();
    test_heterogeneous_floating_overloads<long double, double>();

    test_heterogeneous_mixed_overloads_per_fp_type<float>();
    test_heterogeneous_mixed_overloads_per_fp_type<double>();
    test_heterogeneous_mixed_overloads_per_fp_type<long double>();

    test_heterogeneous_mixed_overloads_with_bool<float>();
    test_heterogeneous_mixed_overloads_with_bool<double>();
    test_heterogeneous_mixed_overloads_with_bool<long double>();
}

template <class I1, class I2, std::enable_if_t<std::is_same_v<I1, I2>, int> = 0>
void test_heterogeneous_integer_overloads_per_rhs_type() {}

template <class I1, class I2, std::enable_if_t<!std::is_same_v<I1, I2>, int> = 0>
void test_heterogeneous_integer_overloads_per_rhs_type() {
    constexpr I1 l0{};
    constexpr I1 l1{1};

    constexpr I2 r0{};
    constexpr I2 r1{1};

    // test overloads in std

    assert(!std::isgreater(l0, r0));
    assert(std::isgreater(l1, r0));
    assert(!std::isgreater(l0, r1));
    assert(!std::isgreater(l1, r1));

    assert(std::isgreaterequal(l0, r0));
    assert(std::isgreaterequal(l1, r0));
    assert(!std::isgreater(l0, r1));
    assert(std::isgreaterequal(l1, r1));

    assert(!std::isless(l0, r0));
    assert(!std::isless(l1, r0));
    assert(std::isless(l0, r1));
    assert(!std::isless(l1, r1));

    assert(std::islessequal(l0, r0));
    assert(!std::islessequal(l1, r0));
    assert(std::islessequal(l0, r1));
    assert(std::islessequal(l1, r1));

    assert(!std::islessgreater(l0, r0));
    assert(std::islessgreater(l1, r0));
    assert(std::islessgreater(l0, r1));
    assert(!std::islessgreater(l1, r1));

    assert(!std::isunordered(l0, r0));
    assert(!std::isunordered(l1, r0));
    assert(!std::isunordered(l0, r1));
    assert(!std::isunordered(l1, r1));

    // test overloads in the global namespace

    assert(!::isgreater(l0, r0));
    assert(::isgreater(l1, r0));
    assert(!::isgreater(l0, r1));
    assert(!::isgreater(l1, r1));

    assert(::isgreaterequal(l0, r0));
    assert(::isgreaterequal(l1, r0));
    assert(!::isgreater(l0, r1));
    assert(::isgreaterequal(l1, r1));

    assert(!::isless(l0, r0));
    assert(!::isless(l1, r0));
    assert(::isless(l0, r1));
    assert(!::isless(l1, r1));

    assert(::islessequal(l0, r0));
    assert(!::islessequal(l1, r0));
    assert(::islessequal(l0, r1));
    assert(::islessequal(l1, r1));

    assert(!::islessgreater(l0, r0));
    assert(::islessgreater(l1, r0));
    assert(::islessgreater(l0, r1));
    assert(!::islessgreater(l1, r1));

    assert(!::isunordered(l0, r0));
    assert(!::isunordered(l1, r0));
    assert(!::isunordered(l0, r1));
    assert(!::isunordered(l1, r1));
}

template <class I>
void test_heterogeneous_integer_overloads_with_bool() {
    constexpr I lm1{static_cast<I>(-1)};
    constexpr I l0{};
    constexpr I l1{1};
    constexpr I l2{2};

    // test overloads in std

    assert(std::isgreater(lm1, false) == std::is_unsigned_v<I>);
    assert(std::isgreater(lm1, true) == std::is_unsigned_v<I>);
    assert(!std::isgreater(l0, false));
    assert(!std::isgreater(l0, true));
    assert(std::isgreater(l1, false));
    assert(!std::isgreater(l1, true));
    assert(std::isgreater(l2, false));
    assert(std::isgreater(l2, true));
    assert(std::isgreater(false, lm1) == std::is_signed_v<I>);
    assert(std::isgreater(true, lm1) == std::is_signed_v<I>);
    assert(!std::isgreater(false, l0));
    assert(std::isgreater(true, l0));
    assert(!std::isgreater(false, l1));
    assert(!std::isgreater(true, l1));
    assert(!std::isgreater(false, l2));
    assert(!std::isgreater(true, l2));

    assert(std::isgreaterequal(lm1, false) == std::is_unsigned_v<I>);
    assert(std::isgreaterequal(lm1, true) == std::is_unsigned_v<I>);
    assert(std::isgreaterequal(l0, false));
    assert(!std::isgreaterequal(l0, true));
    assert(std::isgreaterequal(l1, false));
    assert(std::isgreaterequal(l1, true));
    assert(std::isgreaterequal(l2, false));
    assert(std::isgreaterequal(l2, true));
    assert(std::isgreaterequal(false, lm1) == std::is_signed_v<I>);
    assert(std::isgreaterequal(true, lm1) == std::is_signed_v<I>);
    assert(std::isgreaterequal(false, l0));
    assert(std::isgreaterequal(true, l0));
    assert(!std::isgreaterequal(false, l1));
    assert(std::isgreaterequal(true, l1));
    assert(!std::isgreaterequal(false, l2));
    assert(!std::isgreaterequal(true, l2));

    assert(std::isless(lm1, false) == std::is_signed_v<I>);
    assert(std::isless(lm1, true) == std::is_signed_v<I>);
    assert(!std::isless(l0, false));
    assert(std::isless(l0, true));
    assert(!std::isless(l1, false));
    assert(!std::isless(l1, true));
    assert(!std::isless(l2, false));
    assert(!std::isless(l2, true));
    assert(std::isless(false, lm1) == std::is_unsigned_v<I>);
    assert(std::isless(true, lm1) == std::is_unsigned_v<I>);
    assert(!std::isless(false, l0));
    assert(!std::isless(true, l0));
    assert(std::isless(false, l1));
    assert(!std::isless(true, l1));
    assert(std::isless(false, l2));
    assert(std::isless(true, l2));

    assert(std::islessequal(lm1, false) == std::is_signed_v<I>);
    assert(std::islessequal(lm1, true) == std::is_signed_v<I>);
    assert(std::islessequal(l0, false));
    assert(std::islessequal(l0, true));
    assert(!std::islessequal(l1, false));
    assert(std::islessequal(l1, true));
    assert(!std::islessequal(l2, false));
    assert(!std::islessequal(l2, true));
    assert(std::islessequal(false, lm1) == std::is_unsigned_v<I>);
    assert(std::islessequal(true, lm1) == std::is_unsigned_v<I>);
    assert(std::islessequal(false, l0));
    assert(!std::islessequal(true, l0));
    assert(std::islessequal(false, l1));
    assert(std::islessequal(true, l1));
    assert(std::islessequal(false, l2));
    assert(std::islessequal(true, l2));

    assert(std::islessgreater(lm1, false));
    assert(std::islessgreater(lm1, true));
    assert(!std::islessgreater(l0, false));
    assert(std::islessgreater(l0, true));
    assert(std::islessgreater(l1, false));
    assert(!std::islessgreater(l1, true));
    assert(std::islessgreater(l2, false));
    assert(std::islessgreater(l2, true));
    assert(std::islessgreater(false, lm1));
    assert(std::islessgreater(true, lm1));
    assert(!std::islessgreater(false, l0));
    assert(std::islessgreater(true, l0));
    assert(std::islessgreater(false, l1));
    assert(!std::islessgreater(true, l1));
    assert(std::islessgreater(false, l2));
    assert(std::islessgreater(true, l2));

    assert(!std::isunordered(lm1, false));
    assert(!std::isunordered(lm1, true));
    assert(!std::isunordered(l0, false));
    assert(!std::isunordered(l0, true));
    assert(!std::isunordered(l1, false));
    assert(!std::isunordered(l1, true));
    assert(!std::isunordered(l2, false));
    assert(!std::isunordered(l2, true));
    assert(!std::isunordered(false, lm1));
    assert(!std::isunordered(true, lm1));
    assert(!std::isunordered(false, l0));
    assert(!std::isunordered(true, l0));
    assert(!std::isunordered(false, l1));
    assert(!std::isunordered(true, l1));
    assert(!std::isunordered(false, l2));
    assert(!std::isunordered(true, l2));

    // test overloads in the global namespace

    assert(::isgreater(lm1, false) == std::is_unsigned_v<I>);
    assert(::isgreater(lm1, true) == std::is_unsigned_v<I>);
    assert(!::isgreater(l0, false));
    assert(!::isgreater(l0, true));
    assert(::isgreater(l1, false));
    assert(!::isgreater(l1, true));
    assert(::isgreater(l2, false));
    assert(::isgreater(l2, true));
    assert(::isgreater(false, lm1) == std::is_signed_v<I>);
    assert(::isgreater(true, lm1) == std::is_signed_v<I>);
    assert(!::isgreater(false, l0));
    assert(::isgreater(true, l0));
    assert(!::isgreater(false, l1));
    assert(!::isgreater(true, l1));
    assert(!::isgreater(false, l2));
    assert(!::isgreater(true, l2));

    assert(::isgreaterequal(lm1, false) == std::is_unsigned_v<I>);
    assert(::isgreaterequal(lm1, true) == std::is_unsigned_v<I>);
    assert(::isgreaterequal(l0, false));
    assert(!::isgreaterequal(l0, true));
    assert(::isgreaterequal(l1, false));
    assert(::isgreaterequal(l1, true));
    assert(::isgreaterequal(l2, false));
    assert(::isgreaterequal(l2, true));
    assert(::isgreaterequal(false, lm1) == std::is_signed_v<I>);
    assert(::isgreaterequal(true, lm1) == std::is_signed_v<I>);
    assert(::isgreaterequal(false, l0));
    assert(::isgreaterequal(true, l0));
    assert(!::isgreaterequal(false, l1));
    assert(::isgreaterequal(true, l1));
    assert(!::isgreaterequal(false, l2));
    assert(!::isgreaterequal(true, l2));

    assert(::isless(lm1, false) == std::is_signed_v<I>);
    assert(::isless(lm1, true) == std::is_signed_v<I>);
    assert(!::isless(l0, false));
    assert(::isless(l0, true));
    assert(!::isless(l1, false));
    assert(!::isless(l1, true));
    assert(!::isless(l2, false));
    assert(!::isless(l2, true));
    assert(::isless(false, lm1) == std::is_unsigned_v<I>);
    assert(::isless(true, lm1) == std::is_unsigned_v<I>);
    assert(!::isless(false, l0));
    assert(!::isless(true, l0));
    assert(::isless(false, l1));
    assert(!::isless(true, l1));
    assert(::isless(false, l2));
    assert(::isless(true, l2));

    assert(::islessequal(lm1, false) == std::is_signed_v<I>);
    assert(::islessequal(lm1, true) == std::is_signed_v<I>);
    assert(::islessequal(l0, false));
    assert(::islessequal(l0, true));
    assert(!::islessequal(l1, false));
    assert(::islessequal(l1, true));
    assert(!::islessequal(l2, false));
    assert(!::islessequal(l2, true));
    assert(::islessequal(false, lm1) == std::is_unsigned_v<I>);
    assert(::islessequal(true, lm1) == std::is_unsigned_v<I>);
    assert(::islessequal(false, l0));
    assert(!::islessequal(true, l0));
    assert(::islessequal(false, l1));
    assert(::islessequal(true, l1));
    assert(::islessequal(false, l2));
    assert(::islessequal(true, l2));

    assert(::islessgreater(lm1, false));
    assert(::islessgreater(lm1, true));
    assert(!::islessgreater(l0, false));
    assert(::islessgreater(l0, true));
    assert(::islessgreater(l1, false));
    assert(!::islessgreater(l1, true));
    assert(::islessgreater(l2, false));
    assert(::islessgreater(l2, true));
    assert(::islessgreater(false, lm1));
    assert(::islessgreater(true, lm1));
    assert(!::islessgreater(false, l0));
    assert(::islessgreater(true, l0));
    assert(::islessgreater(false, l1));
    assert(!::islessgreater(true, l1));
    assert(::islessgreater(false, l2));
    assert(::islessgreater(true, l2));

    assert(!::isunordered(lm1, false));
    assert(!::isunordered(lm1, true));
    assert(!::isunordered(l0, false));
    assert(!::isunordered(l0, true));
    assert(!::isunordered(l1, false));
    assert(!::isunordered(l1, true));
    assert(!::isunordered(l2, false));
    assert(!::isunordered(l2, true));
    assert(!::isunordered(false, lm1));
    assert(!::isunordered(true, lm1));
    assert(!::isunordered(false, l0));
    assert(!::isunordered(true, l0));
    assert(!::isunordered(false, l1));
    assert(!::isunordered(true, l1));
    assert(!::isunordered(false, l2));
    assert(!::isunordered(true, l2));
}

template <class I1>
void test_heterogeneous_integer_overloads_per_integer_type() {
    test_heterogeneous_integer_overloads_per_rhs_type<I1, signed char>();
    test_heterogeneous_integer_overloads_per_rhs_type<I1, short>();
    test_heterogeneous_integer_overloads_per_rhs_type<I1, int>();
    test_heterogeneous_integer_overloads_per_rhs_type<I1, long>();
    test_heterogeneous_integer_overloads_per_rhs_type<I1, long long>();
    test_heterogeneous_integer_overloads_per_rhs_type<I1, unsigned char>();
    test_heterogeneous_integer_overloads_per_rhs_type<I1, unsigned short>();
    test_heterogeneous_integer_overloads_per_rhs_type<I1, unsigned int>();
    test_heterogeneous_integer_overloads_per_rhs_type<I1, unsigned long>();
    test_heterogeneous_integer_overloads_per_rhs_type<I1, unsigned long long>();
    test_heterogeneous_integer_overloads_per_rhs_type<I1, char>();
#ifdef __cpp_char8_t
    test_heterogeneous_integer_overloads_per_rhs_type<I1, char8_t>();
#endif // defined(__cpp_char8_t)
    test_heterogeneous_integer_overloads_per_rhs_type<I1, char16_t>();
    test_heterogeneous_integer_overloads_per_rhs_type<I1, char32_t>();
    test_heterogeneous_integer_overloads_per_rhs_type<I1, wchar_t>();

    test_heterogeneous_integer_overloads_with_bool<I1>();
}

void test_all_heterogeneous_integer_overloads() {
    test_heterogeneous_integer_overloads_per_integer_type<signed char>();
    test_heterogeneous_integer_overloads_per_integer_type<short>();
    test_heterogeneous_integer_overloads_per_integer_type<int>();
    test_heterogeneous_integer_overloads_per_integer_type<long>();
    test_heterogeneous_integer_overloads_per_integer_type<long long>();
    test_heterogeneous_integer_overloads_per_integer_type<unsigned char>();
    test_heterogeneous_integer_overloads_per_integer_type<unsigned short>();
    test_heterogeneous_integer_overloads_per_integer_type<unsigned int>();
    test_heterogeneous_integer_overloads_per_integer_type<unsigned long>();
    test_heterogeneous_integer_overloads_per_integer_type<unsigned long long>();
    test_heterogeneous_integer_overloads_per_integer_type<char>();
#ifdef __cpp_char8_t
    test_heterogeneous_integer_overloads_per_integer_type<char8_t>();
#endif // defined(__cpp_char8_t)
    test_heterogeneous_integer_overloads_per_integer_type<char16_t>();
    test_heterogeneous_integer_overloads_per_integer_type<char32_t>();
    test_heterogeneous_integer_overloads_per_integer_type<wchar_t>();
}

int main() {
    test_all_heterogeneous_floating_overloads();
    test_all_heterogeneous_integer_overloads();
}
