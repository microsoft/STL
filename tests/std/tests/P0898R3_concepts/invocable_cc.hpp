// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

// expects NAME (function name), CALLCONV (calling convention for non-member functions), and MCALLCONV (calling
// convention for member functions) to be defined before including; undefines all three.

void NAME() {
    struct S {
        using FreeFunc = short(CALLCONV*)(long);
        operator FreeFunc() const;
        double MCALLCONV operator()(char, int&);
        double const& MCALLCONV operator()(char, int&) const;
        double volatile& MCALLCONV operator()(char, int&) volatile;
        double const volatile& MCALLCONV operator()(char, int&) const volatile;
    };

    {
        using Fn  = int (MCALLCONV tag::*)(int);
        using RFn = int (MCALLCONV tag::*)(int)&&;
        {
            // N4849 [func.require]/1.1: "... f is a pointer to member function of a class T and
            // is_base_of_v<T, remove_reference_t<decltype(t_1)>> is true"
            STATIC_ASSERT(test<Fn, tag&, int>());
            STATIC_ASSERT(test<Fn, DerivesFrom<tag>&, int>());
            STATIC_ASSERT(test<RFn, tag&&, int>());
            STATIC_ASSERT(!test<RFn, tag&, int>());
            STATIC_ASSERT(!test<Fn, tag&>());
            STATIC_ASSERT(!test<Fn, tag const&, int>());
        }
        {
            // N4849 [func.require]/1.2: "... f is a pointer to a member function of a class T and
            // remove_cvref_t<decltype(t_1)> is a specialization of reference_wrapper"
            using T  = std::reference_wrapper<tag>;
            using DT = std::reference_wrapper<DerivesFrom<tag>>;
            using CT = std::reference_wrapper<tag const>;
            STATIC_ASSERT(test<Fn, T&, int>());
            STATIC_ASSERT(test<Fn, DT&, int>());
            STATIC_ASSERT(test<Fn, T const&, int>());
            STATIC_ASSERT(test<Fn, T&&, int>());
            STATIC_ASSERT(!test<Fn, CT&, int>());
            STATIC_ASSERT(!test<RFn, T, int>());
        }
        {
            // N4849 [func.require]/1.3: "... f is a pointer to a member function of a class T and t_1 does not satisfy
            // the previous two items"
            using T  = tag*;
            using DT = DerivesFrom<tag>*;
            using CT = tag const*;
            using ST = std::unique_ptr<tag>;
            STATIC_ASSERT(test<Fn, T&, int>());
            STATIC_ASSERT(test<Fn, DT&, int>());
            STATIC_ASSERT(test<Fn, T const&, int>());
            STATIC_ASSERT(test<Fn, T&&, int>());
            STATIC_ASSERT(test<Fn, ST, int>());
            STATIC_ASSERT(!test<Fn, CT&, int>());
            STATIC_ASSERT(!test<RFn, T, int>());
        }
    }

    // clang-format off
    {
        // N4849 [func.require]/1.7: "f(t_1, t_2, ..., t_N) in all other cases"
        {
            // function pointer
            using Fp = void(CALLCONV*)(tag&, int);
            STATIC_ASSERT(test<Fp, tag&, int>());
            STATIC_ASSERT(test<Fp, DerivesFrom<tag>&, int>());
            STATIC_ASSERT(!test<Fp, tag const&, int>());
            STATIC_ASSERT(!test<Fp>());
            STATIC_ASSERT(!test<Fp, tag&>());
        }
        {
            // function reference
            using Fp = void(CALLCONV&)(tag&, int);
            STATIC_ASSERT(test<Fp, tag&, int>());
            STATIC_ASSERT(test<Fp, DerivesFrom<tag>&, int>());
            STATIC_ASSERT(!test<Fp, tag const&, int>());
            STATIC_ASSERT(!test<Fp>());
            STATIC_ASSERT(!test<Fp, tag&>());
        }
        {
            // function object
            struct not_callable_with_int {
                int operator()(int) = delete;
                int MCALLCONV operator()(tag) {
                    return 42;
                }
            };

            STATIC_ASSERT(test<not_callable_with_int, tag>());
            STATIC_ASSERT(!test<not_callable_with_int, int>());
        }
    }

    {
        // function object
        STATIC_ASSERT(test<S, int>());
        STATIC_ASSERT(test<S&, unsigned char, int&>());
        STATIC_ASSERT(test<S const&, unsigned char, int&>());
        STATIC_ASSERT(test<S volatile&, unsigned char, int&>());
        STATIC_ASSERT(test<S const volatile&, unsigned char, int&>());

        STATIC_ASSERT(test<ImplicitTo<void(CALLCONV*)(long)>, int>());
        STATIC_ASSERT(test<ImplicitTo<void(CALLCONV*)(long)>, char>());
        STATIC_ASSERT(test<ImplicitTo<void(CALLCONV*)(long)>, float>());
        STATIC_ASSERT(!test<ImplicitTo<void(CALLCONV*)(long)>, char const*>());
        STATIC_ASSERT(!test<ImplicitTo<void(CALLCONV*)(long)>, S const&>());

        auto lambda_lambda_lambda = [x = 42](int, double) { return x; };
        STATIC_ASSERT(test<decltype((lambda_lambda_lambda)), int, double>());
        STATIC_ASSERT(test<decltype((lambda_lambda_lambda)), double, int>());
        STATIC_ASSERT(test<decltype((lambda_lambda_lambda)), char, char>());
        STATIC_ASSERT(!test<decltype((lambda_lambda_lambda))>());
        STATIC_ASSERT(!test<decltype((lambda_lambda_lambda)), char const*, double>());
        STATIC_ASSERT(!test<decltype((lambda_lambda_lambda)), double, char const*>());
    }

    { // pointer to function
        using RF0  = bool(CALLCONV&)();
        using RF1  = bool*(CALLCONV&) (int);
        using RF2  = bool&(CALLCONV&) (int, int);
        using RF3  = bool const&(CALLCONV&) (int, int, int);
        using RF4  = bool (&)(int, ...);
        using PF0  = bool(CALLCONV*)();
        using PF1  = bool*(CALLCONV*) (int);
        using PF2  = bool&(CALLCONV*) (int, int);
        using PF3  = bool const&(CALLCONV*) (int, int, int);
        using PF4  = bool (*)(int, ...);
        using RPF0 = bool(CALLCONV*&)();
        using RPF1 = bool*(CALLCONV*&) (int);
        using RPF2 = bool&(CALLCONV*&) (int, int);
        using RPF3 = bool const&(CALLCONV*&) (int, int, int);
        using RPF4 = bool (*&)(int, ...);
        STATIC_ASSERT(test<RF0>());
        STATIC_ASSERT(test<RF1, int>());
        STATIC_ASSERT(test<RF2, int, long>());
        STATIC_ASSERT(test<RF3, int, long, int>());
        STATIC_ASSERT(test<RF4, int, float, void*>());
        STATIC_ASSERT(test<PF0>());
        STATIC_ASSERT(test<PF1, int>());
        STATIC_ASSERT(test<PF2, int, long>());
        STATIC_ASSERT(test<PF3, int, long, int>());
        STATIC_ASSERT(test<PF4, int, float, void*>());
        STATIC_ASSERT(test<RPF0>());
        STATIC_ASSERT(test<RPF1, int>());
        STATIC_ASSERT(test<RPF2, int, long>());
        STATIC_ASSERT(test<RPF3, int, long, int>());
        STATIC_ASSERT(test<RPF4, int, float, void*>());
    }

    { // pointer to member function
        using PMF0  = int (MCALLCONV S::*)();
        using PMF1  = int* (MCALLCONV S::*) (long);
        using PMF2  = int& (MCALLCONV S::*) (long, int);
        using PMF1P = int const& (S::*) (int, ...);
        STATIC_ASSERT(test<PMF0, S>());
        STATIC_ASSERT(test<PMF0, S&>());
        STATIC_ASSERT(test<PMF0, S*>());
        STATIC_ASSERT(test<PMF0, S*&>());
        STATIC_ASSERT(test<PMF0, std::reference_wrapper<S>>());
        STATIC_ASSERT(test<PMF0, std::reference_wrapper<S> const&>());
        STATIC_ASSERT(test<PMF0, std::reference_wrapper<DerivesFrom<S>>>());
        STATIC_ASSERT(test<PMF0, std::reference_wrapper<DerivesFrom<S>> const&>());
        STATIC_ASSERT(test<PMF0, std::unique_ptr<S>>());
        STATIC_ASSERT(test<PMF0, std::unique_ptr<DerivesFrom<S>>>());
        STATIC_ASSERT(!test<PMF0, S const&>());
        STATIC_ASSERT(!test<PMF0, S volatile&>());
        STATIC_ASSERT(!test<PMF0, S const volatile&>());
        STATIC_ASSERT(!test<PMF0, NotDerived&>());
        STATIC_ASSERT(!test<PMF0, NotDerived const&>());
        STATIC_ASSERT(!test<PMF0, std::unique_ptr<S const>>());
        STATIC_ASSERT(!test<PMF0, std::reference_wrapper<S const>>());
        STATIC_ASSERT(!test<PMF0, std::reference_wrapper<NotDerived>>());
        STATIC_ASSERT(!test<PMF0, std::unique_ptr<NotDerived>>());

        STATIC_ASSERT(test<PMF1, S, int>());
        STATIC_ASSERT(test<PMF1, S&, int>());
        STATIC_ASSERT(test<PMF1, S*, int>());
        STATIC_ASSERT(test<PMF1, S*&, int>());
        STATIC_ASSERT(test<PMF1, std::unique_ptr<S>, int>());
        STATIC_ASSERT(test<PMF1, std::unique_ptr<DerivesFrom<S>>, int>());
        STATIC_ASSERT(test<PMF1, std::reference_wrapper<S>, int>());
        STATIC_ASSERT(test<PMF1, std::reference_wrapper<S> const&, int>());
        STATIC_ASSERT(test<PMF1, std::reference_wrapper<DerivesFrom<S>>, int>());
        STATIC_ASSERT(test<PMF1, std::reference_wrapper<DerivesFrom<S>> const&, int>());
        STATIC_ASSERT(!test<PMF1, S const&, int>());
        STATIC_ASSERT(!test<PMF1, S volatile&, int>());
        STATIC_ASSERT(!test<PMF1, S const volatile&, int>());
        STATIC_ASSERT(!test<PMF1, NotDerived&, int>());
        STATIC_ASSERT(!test<PMF1, NotDerived const&, int>());
        STATIC_ASSERT(!test<PMF1, std::unique_ptr<S const>, int>());
        STATIC_ASSERT(!test<PMF1, std::reference_wrapper<S const>, int>());
        STATIC_ASSERT(!test<PMF1, std::reference_wrapper<NotDerived>, int>());
        STATIC_ASSERT(!test<PMF1, std::unique_ptr<NotDerived>, int>());

        STATIC_ASSERT(test<PMF2, S, int, int>());
        STATIC_ASSERT(test<PMF2, S&, int, int>());
        STATIC_ASSERT(test<PMF2, S*, int, int>());
        STATIC_ASSERT(test<PMF2, S*&, int, int>());
        STATIC_ASSERT(test<PMF2, std::unique_ptr<S>, int, int>());
        STATIC_ASSERT(test<PMF2, std::unique_ptr<DerivesFrom<S>>, int, int>());
        STATIC_ASSERT(test<PMF2, std::reference_wrapper<S>, int, int>());
        STATIC_ASSERT(test<PMF2, std::reference_wrapper<S> const&, int, int>());
        STATIC_ASSERT(test<PMF2, std::reference_wrapper<DerivesFrom<S>>, int, int>());
        STATIC_ASSERT(test<PMF2, std::reference_wrapper<DerivesFrom<S>> const&, int, int>());
        STATIC_ASSERT(!test<PMF2, S const&, int, int>());
        STATIC_ASSERT(!test<PMF2, S volatile&, int, int>());
        STATIC_ASSERT(!test<PMF2, S const volatile&, int, int>());
        STATIC_ASSERT(!test<PMF2, std::unique_ptr<S const>, int, int>());
        STATIC_ASSERT(!test<PMF2, std::reference_wrapper<S const>, int, int>());
        STATIC_ASSERT(!test<PMF2, NotDerived const&, int, int>());
        STATIC_ASSERT(!test<PMF2, std::reference_wrapper<NotDerived>, int, int>());
        STATIC_ASSERT(!test<PMF2, std::unique_ptr<NotDerived>, int, int>());

        STATIC_ASSERT(test<PMF1P, S&, int>());
        STATIC_ASSERT(test<PMF1P, S&, int, long>());

        using PMF0C  = int (MCALLCONV S::*)() const;
        using PMF1C  = int* (MCALLCONV S::*) (long) const;
        using PMF2C  = int& (MCALLCONV S::*) (long, int) const;
        using PMF1PC = int const& (S::*) (int, ...) const;
        STATIC_ASSERT(test<PMF0C, S>());
        STATIC_ASSERT(test<PMF0C, S&>());
        STATIC_ASSERT(test<PMF0C, S const&>());
        STATIC_ASSERT(test<PMF0C, S*>());
        STATIC_ASSERT(test<PMF0C, S const*>());
        STATIC_ASSERT(test<PMF0C, S*&>());
        STATIC_ASSERT(test<PMF0C, S const*&>());
        STATIC_ASSERT(test<PMF0C, std::unique_ptr<S>>());
        STATIC_ASSERT(test<PMF0C, std::unique_ptr<DerivesFrom<S>>>());
        STATIC_ASSERT(test<PMF0C, std::reference_wrapper<S>>());
        STATIC_ASSERT(test<PMF0C, std::reference_wrapper<S const>>());
        STATIC_ASSERT(test<PMF0C, std::reference_wrapper<S> const&>());
        STATIC_ASSERT(test<PMF0C, std::reference_wrapper<S const> const&>());
        STATIC_ASSERT(test<PMF0C, std::reference_wrapper<DerivesFrom<S>>>());
        STATIC_ASSERT(test<PMF0C, std::reference_wrapper<DerivesFrom<S> const>>());
        STATIC_ASSERT(test<PMF0C, std::reference_wrapper<DerivesFrom<S>> const&>());
        STATIC_ASSERT(test<PMF0C, std::reference_wrapper<DerivesFrom<S> const> const&>());
        STATIC_ASSERT(!test<PMF0C, S volatile&>());
        STATIC_ASSERT(!test<PMF0C, S const volatile&>());

        STATIC_ASSERT(test<PMF1C, S, int>());
        STATIC_ASSERT(test<PMF1C, S&, int>());
        STATIC_ASSERT(test<PMF1C, S const&, int>());
        STATIC_ASSERT(test<PMF1C, S*, int>());
        STATIC_ASSERT(test<PMF1C, S const*, int>());
        STATIC_ASSERT(test<PMF1C, S*&, int>());
        STATIC_ASSERT(test<PMF1C, S const*&, int>());
        STATIC_ASSERT(test<PMF1C, std::unique_ptr<S>, int>());
        STATIC_ASSERT(!test<PMF1C, S volatile&, int>());
        STATIC_ASSERT(!test<PMF1C, S const volatile&, int>());

        STATIC_ASSERT(test<PMF2C, S, int, int>());
        STATIC_ASSERT(test<PMF2C, S&, int, int>());
        STATIC_ASSERT(test<PMF2C, S const&, int, int>());
        STATIC_ASSERT(test<PMF2C, S*, int, int>());
        STATIC_ASSERT(test<PMF2C, S const*, int, int>());
        STATIC_ASSERT(test<PMF2C, S*&, int, int>());
        STATIC_ASSERT(test<PMF2C, S const*&, int, int>());
        STATIC_ASSERT(test<PMF2C, std::unique_ptr<S>, int, int>());
        STATIC_ASSERT(!test<PMF2C, S volatile&, int, int>());
        STATIC_ASSERT(!test<PMF2C, S const volatile&, int, int>());

        STATIC_ASSERT(test<PMF1PC, S&, int>());
        STATIC_ASSERT(test<PMF1PC, S&, int, long>());

        using PMF0V  = int (MCALLCONV S::*)() volatile;
        using PMF1V  = int* (MCALLCONV S::*) (long) volatile;
        using PMF2V  = int& (MCALLCONV S::*) (long, int) volatile;
        using PMF1PV = int const& (S::*) (int, ...) volatile;
        STATIC_ASSERT(test<PMF0V, S>());
        STATIC_ASSERT(test<PMF0V, S&>());
        STATIC_ASSERT(test<PMF0V, S volatile&>());
        STATIC_ASSERT(test<PMF0V, S*>());
        STATIC_ASSERT(test<PMF0V, S volatile*>());
        STATIC_ASSERT(test<PMF0V, S*&>());
        STATIC_ASSERT(test<PMF0V, S volatile*&>());
        STATIC_ASSERT(test<PMF0V, std::unique_ptr<S>>());
        STATIC_ASSERT(!test<PMF0V, S const&>());
        STATIC_ASSERT(!test<PMF0V, S const volatile&>());

        STATIC_ASSERT(test<PMF1V, S, int>());
        STATIC_ASSERT(test<PMF1V, S&, int>());
        STATIC_ASSERT(test<PMF1V, S volatile&, int>());
        STATIC_ASSERT(test<PMF1V, S*, int>());
        STATIC_ASSERT(test<PMF1V, S volatile*, int>());
        STATIC_ASSERT(test<PMF1V, S*&, int>());
        STATIC_ASSERT(test<PMF1V, S volatile*&, int>());
        STATIC_ASSERT(test<PMF1V, std::unique_ptr<S>, int>());
        STATIC_ASSERT(!test<PMF1V, S const&, int>());
        STATIC_ASSERT(!test<PMF1V, S const volatile&, int>());

        STATIC_ASSERT(test<PMF2V, S, int, int>());
        STATIC_ASSERT(test<PMF2V, S&, int, int>());
        STATIC_ASSERT(test<PMF2V, S volatile&, int, int>());
        STATIC_ASSERT(test<PMF2V, S*, int, int>());
        STATIC_ASSERT(test<PMF2V, S volatile*, int, int>());
        STATIC_ASSERT(test<PMF2V, S*&, int, int>());
        STATIC_ASSERT(test<PMF2V, S volatile*&, int, int>());
        STATIC_ASSERT(test<PMF2V, std::unique_ptr<S>, int, int>());
        STATIC_ASSERT(!test<PMF2V, S const&, int, int>());
        STATIC_ASSERT(!test<PMF2V, S const volatile&, int, int>());

        STATIC_ASSERT(test<PMF1PV, S&, int>());
        STATIC_ASSERT(test<PMF1PV, S&, int, long>());

        using PMF0CV  = int (MCALLCONV S::*)() const volatile;
        using PMF1CV  = int* (MCALLCONV S::*) (long) const volatile;
        using PMF2CV  = int& (MCALLCONV S::*) (long, int) const volatile;
        using PMF1PCV = int const& (S::*) (int, ...) const volatile;
        STATIC_ASSERT(test<PMF0CV, S>());
        STATIC_ASSERT(test<PMF0CV, S&>());
        STATIC_ASSERT(test<PMF0CV, S const&>());
        STATIC_ASSERT(test<PMF0CV, S volatile&>());
        STATIC_ASSERT(test<PMF0CV, S const volatile&>());
        STATIC_ASSERT(test<PMF0CV, S*>());
        STATIC_ASSERT(test<PMF0CV, S const*>());
        STATIC_ASSERT(test<PMF0CV, S volatile*>());
        STATIC_ASSERT(test<PMF0CV, S const volatile*>());
        STATIC_ASSERT(test<PMF0CV, S*&>());
        STATIC_ASSERT(test<PMF0CV, S const*&>());
        STATIC_ASSERT(test<PMF0CV, S volatile*&>());
        STATIC_ASSERT(test<PMF0CV, S const volatile*&>());
        STATIC_ASSERT(test<PMF0CV, std::unique_ptr<S>>());

        STATIC_ASSERT(test<PMF1CV, S, int>());
        STATIC_ASSERT(test<PMF1CV, S&, int>());
        STATIC_ASSERT(test<PMF1CV, S const&, int>());
        STATIC_ASSERT(test<PMF1CV, S volatile&, int>());
        STATIC_ASSERT(test<PMF1CV, S const volatile&, int>());
        STATIC_ASSERT(test<PMF1CV, S*, int>());
        STATIC_ASSERT(test<PMF1CV, S const*, int>());
        STATIC_ASSERT(test<PMF1CV, S volatile*, int>());
        STATIC_ASSERT(test<PMF1CV, S const volatile*, int>());
        STATIC_ASSERT(test<PMF1CV, S*&, int>());
        STATIC_ASSERT(test<PMF1CV, S const*&, int>());
        STATIC_ASSERT(test<PMF1CV, S volatile*&, int>());
        STATIC_ASSERT(test<PMF1CV, S const volatile*&, int>());
        STATIC_ASSERT(test<PMF1CV, std::unique_ptr<S>, int>());

        STATIC_ASSERT(test<PMF2CV, S, int, int>());
        STATIC_ASSERT(test<PMF2CV, S&, int, int>());
        STATIC_ASSERT(test<PMF2CV, S const&, int, int>());
        STATIC_ASSERT(test<PMF2CV, S volatile&, int, int>());
        STATIC_ASSERT(test<PMF2CV, S const volatile&, int, int>());
        STATIC_ASSERT(test<PMF2CV, S*, int, int>());
        STATIC_ASSERT(test<PMF2CV, S const*, int, int>());
        STATIC_ASSERT(test<PMF2CV, S volatile*, int, int>());
        STATIC_ASSERT(test<PMF2CV, S const volatile*, int, int>());
        STATIC_ASSERT(test<PMF2CV, S*&, int, int>());
        STATIC_ASSERT(test<PMF2CV, S const*&, int, int>());
        STATIC_ASSERT(test<PMF2CV, S volatile*&, int, int>());
        STATIC_ASSERT(test<PMF2CV, S const volatile*&, int, int>());
        STATIC_ASSERT(test<PMF2CV, std::unique_ptr<S>, int, int>());

        STATIC_ASSERT(test<PMF1PCV, S&, int>());
        STATIC_ASSERT(test<PMF1PCV, S&, int, long>());

        using PMF0R  = int (MCALLCONV S::*)()&;
        using PMF1R  = int* (MCALLCONV S::*) (long) &;
        using PMF2R  = int& (MCALLCONV S::*) (long, int) &;
        using PMF1PR = int const& (S::*) (int, ...)&;
        STATIC_ASSERT(!test<PMF0R, S>());
        STATIC_ASSERT(test<PMF0R, S&>());
        STATIC_ASSERT(test<PMF0R, S*>());
        STATIC_ASSERT(test<PMF0R, S*&>());
        STATIC_ASSERT(test<PMF0R, std::reference_wrapper<S>>());
        STATIC_ASSERT(test<PMF0R, std::reference_wrapper<S> const&>());
        STATIC_ASSERT(test<PMF0R, std::reference_wrapper<DerivesFrom<S>>>());
        STATIC_ASSERT(test<PMF0R, std::reference_wrapper<DerivesFrom<S>> const&>());
        STATIC_ASSERT(test<PMF0R, std::unique_ptr<S>>());
        STATIC_ASSERT(test<PMF0R, std::unique_ptr<DerivesFrom<S>>>());
        STATIC_ASSERT(!test<PMF0R, S const&>());
        STATIC_ASSERT(!test<PMF0R, S volatile&>());
        STATIC_ASSERT(!test<PMF0R, S const volatile&>());
        STATIC_ASSERT(!test<PMF0R, NotDerived&>());
        STATIC_ASSERT(!test<PMF0R, NotDerived const&>());
        STATIC_ASSERT(!test<PMF0R, std::unique_ptr<S const>>());
        STATIC_ASSERT(!test<PMF0R, std::reference_wrapper<S const>>());
        STATIC_ASSERT(!test<PMF0R, std::reference_wrapper<NotDerived>>());
        STATIC_ASSERT(!test<PMF0R, std::unique_ptr<NotDerived>>());

        STATIC_ASSERT(!test<PMF1R, S, int>());
        STATIC_ASSERT(test<PMF1R, S&, int>());
        STATIC_ASSERT(test<PMF1R, S*, int>());
        STATIC_ASSERT(test<PMF1R, S*&, int>());
        STATIC_ASSERT(test<PMF1R, std::unique_ptr<S>, int>());
        STATIC_ASSERT(test<PMF1R, std::unique_ptr<DerivesFrom<S>>, int>());
        STATIC_ASSERT(test<PMF1R, std::reference_wrapper<S>, int>());
        STATIC_ASSERT(test<PMF1R, std::reference_wrapper<S> const&, int>());
        STATIC_ASSERT(test<PMF1R, std::reference_wrapper<DerivesFrom<S>>, int>());
        STATIC_ASSERT(test<PMF1R, std::reference_wrapper<DerivesFrom<S>> const&, int>());
        STATIC_ASSERT(!test<PMF1R, S const&, int>());
        STATIC_ASSERT(!test<PMF1R, S volatile&, int>());
        STATIC_ASSERT(!test<PMF1R, S const volatile&, int>());
        STATIC_ASSERT(!test<PMF1R, NotDerived&, int>());
        STATIC_ASSERT(!test<PMF1R, NotDerived const&, int>());
        STATIC_ASSERT(!test<PMF1R, std::unique_ptr<S const>, int>());
        STATIC_ASSERT(!test<PMF1R, std::reference_wrapper<S const>, int>());
        STATIC_ASSERT(!test<PMF1R, std::reference_wrapper<NotDerived>, int>());
        STATIC_ASSERT(!test<PMF1R, std::unique_ptr<NotDerived>, int>());

        STATIC_ASSERT(!test<PMF2R, S, int, int>());
        STATIC_ASSERT(test<PMF2R, S&, int, int>());
        STATIC_ASSERT(test<PMF2R, S*, int, int>());
        STATIC_ASSERT(test<PMF2R, S*&, int, int>());
        STATIC_ASSERT(test<PMF2R, std::unique_ptr<S>, int, int>());
        STATIC_ASSERT(test<PMF2R, std::unique_ptr<DerivesFrom<S>>, int, int>());
        STATIC_ASSERT(test<PMF2R, std::reference_wrapper<S>, int, int>());
        STATIC_ASSERT(test<PMF2R, std::reference_wrapper<S> const&, int, int>());
        STATIC_ASSERT(test<PMF2R, std::reference_wrapper<DerivesFrom<S>>, int, int>());
        STATIC_ASSERT(test<PMF2R, std::reference_wrapper<DerivesFrom<S>> const&, int, int>());
        STATIC_ASSERT(!test<PMF2R, S const&, int, int>());
        STATIC_ASSERT(!test<PMF2R, S volatile&, int, int>());
        STATIC_ASSERT(!test<PMF2R, S const volatile&, int, int>());
        STATIC_ASSERT(!test<PMF2R, std::unique_ptr<S const>, int, int>());
        STATIC_ASSERT(!test<PMF2R, std::reference_wrapper<S const>, int, int>());
        STATIC_ASSERT(!test<PMF2R, NotDerived const&, int, int>());
        STATIC_ASSERT(!test<PMF2R, std::reference_wrapper<NotDerived>, int, int>());
        STATIC_ASSERT(!test<PMF2R, std::unique_ptr<NotDerived>, int, int>());

        STATIC_ASSERT(test<PMF1PR, S&, int>());
        STATIC_ASSERT(test<PMF1PR, S&, int, long>());

        using PMF0CR  = int (MCALLCONV S::*)() const&;
        using PMF1CR  = int* (MCALLCONV S::*) (long) const&;
        using PMF2CR  = int& (MCALLCONV S::*) (long, int) const&;
        using PMF1PCR = int const& (S::*) (int, ...) const&;
        STATIC_ASSERT(test<PMF0CR, S>());
        STATIC_ASSERT(test<PMF0CR, S&>());
        STATIC_ASSERT(test<PMF0CR, S*>());
        STATIC_ASSERT(test<PMF0CR, S*&>());
        STATIC_ASSERT(test<PMF0CR, std::reference_wrapper<S>>());
        STATIC_ASSERT(test<PMF0CR, std::reference_wrapper<S> const&>());
        STATIC_ASSERT(test<PMF0CR, std::reference_wrapper<DerivesFrom<S>>>());
        STATIC_ASSERT(test<PMF0CR, std::reference_wrapper<DerivesFrom<S>> const&>());
        STATIC_ASSERT(test<PMF0CR, std::unique_ptr<S>>());
        STATIC_ASSERT(test<PMF0CR, std::unique_ptr<DerivesFrom<S>>>());
        STATIC_ASSERT(test<PMF0CR, S const&>());
        STATIC_ASSERT(!test<PMF0CR, S volatile&>());
        STATIC_ASSERT(!test<PMF0CR, S const volatile&>());
        STATIC_ASSERT(!test<PMF0CR, NotDerived&>());
        STATIC_ASSERT(!test<PMF0CR, NotDerived const&>());
        STATIC_ASSERT(test<PMF0CR, std::unique_ptr<S const>>());
        STATIC_ASSERT(test<PMF0CR, std::reference_wrapper<S const>>());
        STATIC_ASSERT(!test<PMF0CR, std::reference_wrapper<NotDerived>>());
        STATIC_ASSERT(!test<PMF0CR, std::unique_ptr<NotDerived>>());

        STATIC_ASSERT(test<PMF1CR, S, int>());
        STATIC_ASSERT(test<PMF1CR, S&, int>());
        STATIC_ASSERT(test<PMF1CR, S*, int>());
        STATIC_ASSERT(test<PMF1CR, S*&, int>());
        STATIC_ASSERT(test<PMF1CR, std::unique_ptr<S>, int>());
        STATIC_ASSERT(test<PMF1CR, std::unique_ptr<DerivesFrom<S>>, int>());
        STATIC_ASSERT(test<PMF1CR, std::reference_wrapper<S>, int>());
        STATIC_ASSERT(test<PMF1CR, std::reference_wrapper<S> const&, int>());
        STATIC_ASSERT(test<PMF1CR, std::reference_wrapper<DerivesFrom<S>>, int>());
        STATIC_ASSERT(test<PMF1CR, std::reference_wrapper<DerivesFrom<S>> const&, int>());
        STATIC_ASSERT(test<PMF1CR, S const&, int>());
        STATIC_ASSERT(!test<PMF1CR, S volatile&, int>());
        STATIC_ASSERT(!test<PMF1CR, S const volatile&, int>());
        STATIC_ASSERT(!test<PMF1CR, NotDerived&, int>());
        STATIC_ASSERT(!test<PMF1CR, NotDerived const&, int>());
        STATIC_ASSERT(test<PMF1CR, std::unique_ptr<S const>, int>());
        STATIC_ASSERT(test<PMF1CR, std::reference_wrapper<S const>, int>());
        STATIC_ASSERT(!test<PMF1CR, std::reference_wrapper<NotDerived>, int>());
        STATIC_ASSERT(!test<PMF1CR, std::unique_ptr<NotDerived>, int>());

        STATIC_ASSERT(test<PMF2CR, S, int, int>());
        STATIC_ASSERT(test<PMF2CR, S&, int, int>());
        STATIC_ASSERT(test<PMF2CR, S*, int, int>());
        STATIC_ASSERT(test<PMF2CR, S*&, int, int>());
        STATIC_ASSERT(test<PMF2CR, std::unique_ptr<S>, int, int>());
        STATIC_ASSERT(test<PMF2CR, std::unique_ptr<DerivesFrom<S>>, int, int>());
        STATIC_ASSERT(test<PMF2CR, std::reference_wrapper<S>, int, int>());
        STATIC_ASSERT(test<PMF2CR, std::reference_wrapper<S> const&, int, int>());
        STATIC_ASSERT(test<PMF2CR, std::reference_wrapper<DerivesFrom<S>>, int, int>());
        STATIC_ASSERT(test<PMF2CR, std::reference_wrapper<DerivesFrom<S>> const&, int, int>());
        STATIC_ASSERT(test<PMF2CR, S const&, int, int>());
        STATIC_ASSERT(!test<PMF2CR, S volatile&, int, int>());
        STATIC_ASSERT(!test<PMF2CR, S const volatile&, int, int>());
        STATIC_ASSERT(test<PMF2CR, std::unique_ptr<S const>, int, int>());
        STATIC_ASSERT(test<PMF2CR, std::reference_wrapper<S const>, int, int>());
        STATIC_ASSERT(!test<PMF2CR, NotDerived const&, int, int>());
        STATIC_ASSERT(!test<PMF2CR, std::reference_wrapper<NotDerived>, int, int>());
        STATIC_ASSERT(!test<PMF2CR, std::unique_ptr<NotDerived>, int, int>());

        STATIC_ASSERT(test<PMF1PCR, S&, int>());
        STATIC_ASSERT(test<PMF1PCR, S&, int, long>());

        using PMF0CVR  = int (MCALLCONV S::*)() const volatile&;
        using PMF1CVR  = int* (MCALLCONV S::*) (long) const volatile&;
        using PMF2CVR  = int& (MCALLCONV S::*) (long, int) const volatile&;
        using PMF1PCVR = int const& (S::*) (int, ...) const volatile&;
        STATIC_ASSERT(!test<PMF0CVR, S>());
        STATIC_ASSERT(test<PMF0CVR, S&>());
        STATIC_ASSERT(test<PMF0CVR, S*>());
        STATIC_ASSERT(test<PMF0CVR, S*&>());
        STATIC_ASSERT(test<PMF0CVR, std::reference_wrapper<S>>());
        STATIC_ASSERT(test<PMF0CVR, std::reference_wrapper<S> const&>());
        STATIC_ASSERT(test<PMF0CVR, std::reference_wrapper<DerivesFrom<S>>>());
        STATIC_ASSERT(test<PMF0CVR, std::reference_wrapper<DerivesFrom<S>> const&>());
        STATIC_ASSERT(test<PMF0CVR, std::unique_ptr<S>>());
        STATIC_ASSERT(test<PMF0CVR, std::unique_ptr<DerivesFrom<S>>>());
        STATIC_ASSERT(test<PMF0CVR, S const&>());
        STATIC_ASSERT(test<PMF0CVR, S volatile&>());
        STATIC_ASSERT(test<PMF0CVR, S const volatile&>());
        STATIC_ASSERT(!test<PMF0CVR, NotDerived&>());
        STATIC_ASSERT(!test<PMF0CVR, NotDerived const&>());
        STATIC_ASSERT(test<PMF0CVR, std::unique_ptr<S const>>());
        STATIC_ASSERT(test<PMF0CVR, std::reference_wrapper<S const>>());
        STATIC_ASSERT(!test<PMF0CVR, std::reference_wrapper<NotDerived>>());
        STATIC_ASSERT(!test<PMF0CVR, std::unique_ptr<NotDerived>>());

        STATIC_ASSERT(!test<PMF1CVR, S, int>());
        STATIC_ASSERT(test<PMF1CVR, S&, int>());
        STATIC_ASSERT(test<PMF1CVR, S*, int>());
        STATIC_ASSERT(test<PMF1CVR, S*&, int>());
        STATIC_ASSERT(test<PMF1CVR, std::unique_ptr<S>, int>());
        STATIC_ASSERT(test<PMF1CVR, std::unique_ptr<DerivesFrom<S>>, int>());
        STATIC_ASSERT(test<PMF1CVR, std::reference_wrapper<S>, int>());
        STATIC_ASSERT(test<PMF1CVR, std::reference_wrapper<S> const&, int>());
        STATIC_ASSERT(test<PMF1CVR, std::reference_wrapper<DerivesFrom<S>>, int>());
        STATIC_ASSERT(test<PMF1CVR, std::reference_wrapper<DerivesFrom<S>> const&, int>());
        STATIC_ASSERT(test<PMF1CVR, S const&, int>());
        STATIC_ASSERT(test<PMF1CVR, S volatile&, int>());
        STATIC_ASSERT(test<PMF1CVR, S const volatile&, int>());
        STATIC_ASSERT(!test<PMF1CVR, NotDerived&, int>());
        STATIC_ASSERT(!test<PMF1CVR, NotDerived const&, int>());
        STATIC_ASSERT(test<PMF1CVR, std::unique_ptr<S const>, int>());
        STATIC_ASSERT(test<PMF1CVR, std::reference_wrapper<S const>, int>());
        STATIC_ASSERT(!test<PMF1CVR, std::reference_wrapper<NotDerived>, int>());
        STATIC_ASSERT(!test<PMF1CVR, std::unique_ptr<NotDerived>, int>());

        STATIC_ASSERT(!test<PMF2CVR, S, int, int>());
        STATIC_ASSERT(test<PMF2CVR, S&, int, int>());
        STATIC_ASSERT(test<PMF2CVR, S*, int, int>());
        STATIC_ASSERT(test<PMF2CVR, S*&, int, int>());
        STATIC_ASSERT(test<PMF2CVR, std::unique_ptr<S>, int, int>());
        STATIC_ASSERT(test<PMF2CVR, std::unique_ptr<DerivesFrom<S>>, int, int>());
        STATIC_ASSERT(test<PMF2CVR, std::reference_wrapper<S>, int, int>());
        STATIC_ASSERT(test<PMF2CVR, std::reference_wrapper<S> const&, int, int>());
        STATIC_ASSERT(test<PMF2CVR, std::reference_wrapper<DerivesFrom<S>>, int, int>());
        STATIC_ASSERT(test<PMF2CVR, std::reference_wrapper<DerivesFrom<S>> const&, int, int>());
        STATIC_ASSERT(test<PMF2CVR, S const&, int, int>());
        STATIC_ASSERT(test<PMF2CVR, S volatile&, int, int>());
        STATIC_ASSERT(test<PMF2CVR, S const volatile&, int, int>());
        STATIC_ASSERT(test<PMF2CVR, std::unique_ptr<S const>, int, int>());
        STATIC_ASSERT(test<PMF2CVR, std::reference_wrapper<S const>, int, int>());
        STATIC_ASSERT(!test<PMF2CVR, NotDerived const&, int, int>());
        STATIC_ASSERT(!test<PMF2CVR, std::reference_wrapper<NotDerived>, int, int>());
        STATIC_ASSERT(!test<PMF2CVR, std::unique_ptr<NotDerived>, int, int>());

        STATIC_ASSERT(test<PMF1PCVR, S&, int>());
        STATIC_ASSERT(test<PMF1PCVR, S&, int, long>());

        using PMF0RR  = int (MCALLCONV S::*)()&&;
        using PMF1RR  = int* (MCALLCONV S::*) (long) &&;
        using PMF2RR  = int& (MCALLCONV S::*) (long, int) &&;
        using PMF1PRR = int const& (S::*) (int, ...)&&;
        STATIC_ASSERT(test<PMF0RR, S>());
        STATIC_ASSERT(!test<PMF0RR, S&>());
        STATIC_ASSERT(!test<PMF0RR, S*>());
        STATIC_ASSERT(!test<PMF0RR, S*&>());
        STATIC_ASSERT(!test<PMF0RR, std::reference_wrapper<S>>());
        STATIC_ASSERT(!test<PMF0RR, std::reference_wrapper<S> const&>());
        STATIC_ASSERT(!test<PMF0RR, std::reference_wrapper<DerivesFrom<S>>>());
        STATIC_ASSERT(!test<PMF0RR, std::reference_wrapper<DerivesFrom<S>> const&>());
        STATIC_ASSERT(!test<PMF0RR, std::unique_ptr<S>>());
        STATIC_ASSERT(!test<PMF0RR, std::unique_ptr<DerivesFrom<S>>>());
        STATIC_ASSERT(!test<PMF0RR, S const&>());
        STATIC_ASSERT(!test<PMF0RR, S volatile&>());
        STATIC_ASSERT(!test<PMF0RR, S const volatile&>());
        STATIC_ASSERT(!test<PMF0RR, NotDerived&>());
        STATIC_ASSERT(!test<PMF0RR, NotDerived const&>());
        STATIC_ASSERT(!test<PMF0RR, std::unique_ptr<S const>>());
        STATIC_ASSERT(!test<PMF0RR, std::reference_wrapper<S const>>());
        STATIC_ASSERT(!test<PMF0RR, std::reference_wrapper<NotDerived>>());
        STATIC_ASSERT(!test<PMF0RR, std::unique_ptr<NotDerived>>());

        STATIC_ASSERT(test<PMF1RR, S, int>());
        STATIC_ASSERT(!test<PMF1RR, S&, int>());
        STATIC_ASSERT(!test<PMF1RR, S*, int>());
        STATIC_ASSERT(!test<PMF1RR, S*&, int>());
        STATIC_ASSERT(!test<PMF1RR, std::unique_ptr<S>, int>());
        STATIC_ASSERT(!test<PMF1RR, std::unique_ptr<DerivesFrom<S>>, int>());
        STATIC_ASSERT(!test<PMF1RR, std::reference_wrapper<S>, int>());
        STATIC_ASSERT(!test<PMF1RR, std::reference_wrapper<S> const&, int>());
        STATIC_ASSERT(!test<PMF1RR, std::reference_wrapper<DerivesFrom<S>>, int>());
        STATIC_ASSERT(!test<PMF1RR, std::reference_wrapper<DerivesFrom<S>> const&, int>());
        STATIC_ASSERT(!test<PMF1RR, S const&, int>());
        STATIC_ASSERT(!test<PMF1RR, S volatile&, int>());
        STATIC_ASSERT(!test<PMF1RR, S const volatile&, int>());
        STATIC_ASSERT(!test<PMF1RR, NotDerived&, int>());
        STATIC_ASSERT(!test<PMF1RR, NotDerived const&, int>());
        STATIC_ASSERT(!test<PMF1RR, std::unique_ptr<S const>, int>());
        STATIC_ASSERT(!test<PMF1RR, std::reference_wrapper<S const>, int>());
        STATIC_ASSERT(!test<PMF1RR, std::reference_wrapper<NotDerived>, int>());
        STATIC_ASSERT(!test<PMF1RR, std::unique_ptr<NotDerived>, int>());

        STATIC_ASSERT(test<PMF2RR, S, int, int>());
        STATIC_ASSERT(!test<PMF2RR, S&, int, int>());
        STATIC_ASSERT(!test<PMF2RR, S*, int, int>());
        STATIC_ASSERT(!test<PMF2RR, S*&, int, int>());
        STATIC_ASSERT(!test<PMF2RR, std::unique_ptr<S>, int, int>());
        STATIC_ASSERT(!test<PMF2RR, std::unique_ptr<DerivesFrom<S>>, int, int>());
        STATIC_ASSERT(!test<PMF2RR, std::reference_wrapper<S>, int, int>());
        STATIC_ASSERT(!test<PMF2RR, std::reference_wrapper<S> const&, int, int>());
        STATIC_ASSERT(!test<PMF2RR, std::reference_wrapper<DerivesFrom<S>>, int, int>());
        STATIC_ASSERT(!test<PMF2RR, std::reference_wrapper<DerivesFrom<S>> const&, int, int>());
        STATIC_ASSERT(!test<PMF2RR, S const&, int, int>());
        STATIC_ASSERT(!test<PMF2RR, S volatile&, int, int>());
        STATIC_ASSERT(!test<PMF2RR, S const volatile&, int, int>());
        STATIC_ASSERT(!test<PMF2RR, std::unique_ptr<S const>, int, int>());
        STATIC_ASSERT(!test<PMF2RR, std::reference_wrapper<S const>, int, int>());
        STATIC_ASSERT(!test<PMF2RR, NotDerived const&, int, int>());
        STATIC_ASSERT(!test<PMF2RR, std::reference_wrapper<NotDerived>, int, int>());
        STATIC_ASSERT(!test<PMF2RR, std::unique_ptr<NotDerived>, int, int>());

        STATIC_ASSERT(!test<PMF1PRR, S&, int>());
        STATIC_ASSERT(!test<PMF1PRR, S&, int, long>());

        using PMF0CRR  = int (MCALLCONV S::*)() const&&;
        using PMF1CRR  = int* (MCALLCONV S::*) (long) const&&;
        using PMF2CRR  = int& (MCALLCONV S::*) (long, int) const&&;
        using PMF1PCRR = int const& (S::*) (int, ...) const&&;
        STATIC_ASSERT(test<PMF0CRR, S>());
        STATIC_ASSERT(!test<PMF0CRR, S&>());
        STATIC_ASSERT(!test<PMF0CRR, S*>());
        STATIC_ASSERT(!test<PMF0CRR, S*&>());
        STATIC_ASSERT(!test<PMF0CRR, std::reference_wrapper<S>>());
        STATIC_ASSERT(!test<PMF0CRR, std::reference_wrapper<S> const&>());
        STATIC_ASSERT(!test<PMF0CRR, std::reference_wrapper<DerivesFrom<S>>>());
        STATIC_ASSERT(!test<PMF0CRR, std::reference_wrapper<DerivesFrom<S>> const&>());
        STATIC_ASSERT(!test<PMF0CRR, std::unique_ptr<S>>());
        STATIC_ASSERT(!test<PMF0CRR, std::unique_ptr<DerivesFrom<S>>>());
        STATIC_ASSERT(!test<PMF0CRR, S const&>());
        STATIC_ASSERT(!test<PMF0CRR, S volatile&>());
        STATIC_ASSERT(!test<PMF0CRR, S const volatile&>());
        STATIC_ASSERT(!test<PMF0CRR, NotDerived&>());
        STATIC_ASSERT(!test<PMF0CRR, NotDerived const&>());
        STATIC_ASSERT(!test<PMF0CRR, std::unique_ptr<S const>>());
        STATIC_ASSERT(!test<PMF0CRR, std::reference_wrapper<S const>>());
        STATIC_ASSERT(!test<PMF0CRR, std::reference_wrapper<NotDerived>>());
        STATIC_ASSERT(!test<PMF0CRR, std::unique_ptr<NotDerived>>());

        STATIC_ASSERT(test<PMF1CRR, S, int>());
        STATIC_ASSERT(!test<PMF1CRR, S&, int>());
        STATIC_ASSERT(!test<PMF1CRR, S*, int>());
        STATIC_ASSERT(!test<PMF1CRR, S*&, int>());
        STATIC_ASSERT(!test<PMF1CRR, std::unique_ptr<S>, int>());
        STATIC_ASSERT(!test<PMF1CRR, std::unique_ptr<DerivesFrom<S>>, int>());
        STATIC_ASSERT(!test<PMF1CRR, std::reference_wrapper<S>, int>());
        STATIC_ASSERT(!test<PMF1CRR, std::reference_wrapper<S> const&, int>());
        STATIC_ASSERT(!test<PMF1CRR, std::reference_wrapper<DerivesFrom<S>>, int>());
        STATIC_ASSERT(!test<PMF1CRR, std::reference_wrapper<DerivesFrom<S>> const&, int>());
        STATIC_ASSERT(!test<PMF1CRR, S const&, int>());
        STATIC_ASSERT(!test<PMF1CRR, S volatile&, int>());
        STATIC_ASSERT(!test<PMF1CRR, S const volatile&, int>());
        STATIC_ASSERT(!test<PMF1CRR, NotDerived&, int>());
        STATIC_ASSERT(!test<PMF1CRR, NotDerived const&, int>());
        STATIC_ASSERT(!test<PMF1CRR, std::unique_ptr<S const>, int>());
        STATIC_ASSERT(!test<PMF1CRR, std::reference_wrapper<S const>, int>());
        STATIC_ASSERT(!test<PMF1CRR, std::reference_wrapper<NotDerived>, int>());
        STATIC_ASSERT(!test<PMF1CRR, std::unique_ptr<NotDerived>, int>());

        STATIC_ASSERT(test<PMF2CRR, S, int, int>());
        STATIC_ASSERT(!test<PMF2CRR, S&, int, int>());
        STATIC_ASSERT(!test<PMF2CRR, S*, int, int>());
        STATIC_ASSERT(!test<PMF2CRR, S*&, int, int>());
        STATIC_ASSERT(!test<PMF2CRR, std::unique_ptr<S>, int, int>());
        STATIC_ASSERT(!test<PMF2CRR, std::unique_ptr<DerivesFrom<S>>, int, int>());
        STATIC_ASSERT(!test<PMF2CRR, std::reference_wrapper<S>, int, int>());
        STATIC_ASSERT(!test<PMF2CRR, std::reference_wrapper<S> const&, int, int>());
        STATIC_ASSERT(!test<PMF2CRR, std::reference_wrapper<DerivesFrom<S>>, int, int>());
        STATIC_ASSERT(!test<PMF2CRR, std::reference_wrapper<DerivesFrom<S>> const&, int, int>());
        STATIC_ASSERT(!test<PMF2CRR, S const&, int, int>());
        STATIC_ASSERT(!test<PMF2CRR, S volatile&, int, int>());
        STATIC_ASSERT(!test<PMF2CRR, S const volatile&, int, int>());
        STATIC_ASSERT(!test<PMF2CRR, std::unique_ptr<S const>, int, int>());
        STATIC_ASSERT(!test<PMF2CRR, std::reference_wrapper<S const>, int, int>());
        STATIC_ASSERT(!test<PMF2CRR, NotDerived const&, int, int>());
        STATIC_ASSERT(!test<PMF2CRR, std::reference_wrapper<NotDerived>, int, int>());
        STATIC_ASSERT(!test<PMF2CRR, std::unique_ptr<NotDerived>, int, int>());

        STATIC_ASSERT(!test<PMF1PCRR, S&, int>());
        STATIC_ASSERT(!test<PMF1PCRR, S&, int, long>());

        using PMF0VRR  = int (MCALLCONV S::*)() volatile&&;
        using PMF1VRR  = int* (MCALLCONV S::*) (long) volatile&&;
        using PMF2VRR  = int& (MCALLCONV S::*) (long, int) volatile&&;
        using PMF1PVRR = int const& (S::*) (int, ...) volatile&&;
        STATIC_ASSERT(test<PMF0VRR, S>());
        STATIC_ASSERT(!test<PMF0VRR, S&>());
        STATIC_ASSERT(!test<PMF0VRR, S*>());
        STATIC_ASSERT(!test<PMF0VRR, S*&>());
        STATIC_ASSERT(!test<PMF0VRR, std::reference_wrapper<S>>());
        STATIC_ASSERT(!test<PMF0VRR, std::reference_wrapper<S> const&>());
        STATIC_ASSERT(!test<PMF0VRR, std::reference_wrapper<DerivesFrom<S>>>());
        STATIC_ASSERT(!test<PMF0VRR, std::reference_wrapper<DerivesFrom<S>> const&>());
        STATIC_ASSERT(!test<PMF0VRR, std::unique_ptr<S>>());
        STATIC_ASSERT(!test<PMF0VRR, std::unique_ptr<DerivesFrom<S>>>());
        STATIC_ASSERT(!test<PMF0VRR, S const&>());
        STATIC_ASSERT(!test<PMF0VRR, S volatile&>());
        STATIC_ASSERT(!test<PMF0VRR, S const volatile&>());
        STATIC_ASSERT(!test<PMF0VRR, NotDerived&>());
        STATIC_ASSERT(!test<PMF0VRR, NotDerived const&>());
        STATIC_ASSERT(!test<PMF0VRR, std::unique_ptr<S const>>());
        STATIC_ASSERT(!test<PMF0VRR, std::reference_wrapper<S const>>());
        STATIC_ASSERT(!test<PMF0VRR, std::reference_wrapper<NotDerived>>());
        STATIC_ASSERT(!test<PMF0VRR, std::unique_ptr<NotDerived>>());

        STATIC_ASSERT(test<PMF1VRR, S, int>());
        STATIC_ASSERT(!test<PMF1VRR, S&, int>());
        STATIC_ASSERT(!test<PMF1VRR, S*, int>());
        STATIC_ASSERT(!test<PMF1VRR, S*&, int>());
        STATIC_ASSERT(!test<PMF1VRR, std::unique_ptr<S>, int>());
        STATIC_ASSERT(!test<PMF1VRR, std::unique_ptr<DerivesFrom<S>>, int>());
        STATIC_ASSERT(!test<PMF1VRR, std::reference_wrapper<S>, int>());
        STATIC_ASSERT(!test<PMF1VRR, std::reference_wrapper<S> const&, int>());
        STATIC_ASSERT(!test<PMF1VRR, std::reference_wrapper<DerivesFrom<S>>, int>());
        STATIC_ASSERT(!test<PMF1VRR, std::reference_wrapper<DerivesFrom<S>> const&, int>());
        STATIC_ASSERT(!test<PMF1VRR, S const&, int>());
        STATIC_ASSERT(!test<PMF1VRR, S volatile&, int>());
        STATIC_ASSERT(!test<PMF1VRR, S const volatile&, int>());
        STATIC_ASSERT(!test<PMF1VRR, NotDerived&, int>());
        STATIC_ASSERT(!test<PMF1VRR, NotDerived const&, int>());
        STATIC_ASSERT(!test<PMF1VRR, std::unique_ptr<S const>, int>());
        STATIC_ASSERT(!test<PMF1VRR, std::reference_wrapper<S const>, int>());
        STATIC_ASSERT(!test<PMF1VRR, std::reference_wrapper<NotDerived>, int>());
        STATIC_ASSERT(!test<PMF1VRR, std::unique_ptr<NotDerived>, int>());

        STATIC_ASSERT(test<PMF2VRR, S, int, int>());
        STATIC_ASSERT(!test<PMF2VRR, S&, int, int>());
        STATIC_ASSERT(!test<PMF2VRR, S*, int, int>());
        STATIC_ASSERT(!test<PMF2VRR, S*&, int, int>());
        STATIC_ASSERT(!test<PMF2VRR, std::unique_ptr<S>, int, int>());
        STATIC_ASSERT(!test<PMF2VRR, std::unique_ptr<DerivesFrom<S>>, int, int>());
        STATIC_ASSERT(!test<PMF2VRR, std::reference_wrapper<S>, int, int>());
        STATIC_ASSERT(!test<PMF2VRR, std::reference_wrapper<S> const&, int, int>());
        STATIC_ASSERT(!test<PMF2VRR, std::reference_wrapper<DerivesFrom<S>>, int, int>());
        STATIC_ASSERT(!test<PMF2VRR, std::reference_wrapper<DerivesFrom<S>> const&, int, int>());
        STATIC_ASSERT(!test<PMF2VRR, S const&, int, int>());
        STATIC_ASSERT(!test<PMF2VRR, S volatile&, int, int>());
        STATIC_ASSERT(!test<PMF2VRR, S const volatile&, int, int>());
        STATIC_ASSERT(!test<PMF2VRR, std::unique_ptr<S const>, int, int>());
        STATIC_ASSERT(!test<PMF2VRR, std::reference_wrapper<S const>, int, int>());
        STATIC_ASSERT(!test<PMF2VRR, NotDerived const&, int, int>());
        STATIC_ASSERT(!test<PMF2VRR, std::reference_wrapper<NotDerived>, int, int>());
        STATIC_ASSERT(!test<PMF2VRR, std::unique_ptr<NotDerived>, int, int>());

        STATIC_ASSERT(!test<PMF1PVRR, S&, int>());
        STATIC_ASSERT(!test<PMF1PVRR, S&, int, long>());

        using PMF0CVRR  = int (MCALLCONV S::*)() const volatile&&;
        using PMF1CVRR  = int* (MCALLCONV S::*) (long) const volatile&&;
        using PMF2CVRR  = int& (MCALLCONV S::*) (long, int) const volatile&&;
        using PMF1PCVRR = int const& (S::*) (int, ...) const volatile&&;
        STATIC_ASSERT(test<PMF0CVRR, S>());
        STATIC_ASSERT(!test<PMF0CVRR, S&>());
        STATIC_ASSERT(!test<PMF0CVRR, S*>());
        STATIC_ASSERT(!test<PMF0CVRR, S*&>());
        STATIC_ASSERT(!test<PMF0CVRR, std::reference_wrapper<S>>());
        STATIC_ASSERT(!test<PMF0CVRR, std::reference_wrapper<S> const&>());
        STATIC_ASSERT(!test<PMF0CVRR, std::reference_wrapper<DerivesFrom<S>>>());
        STATIC_ASSERT(!test<PMF0CVRR, std::reference_wrapper<DerivesFrom<S>> const&>());
        STATIC_ASSERT(!test<PMF0CVRR, std::unique_ptr<S>>());
        STATIC_ASSERT(!test<PMF0CVRR, std::unique_ptr<DerivesFrom<S>>>());
        STATIC_ASSERT(!test<PMF0CVRR, S const&>());
        STATIC_ASSERT(!test<PMF0CVRR, S volatile&>());
        STATIC_ASSERT(!test<PMF0CVRR, S const volatile&>());
        STATIC_ASSERT(!test<PMF0CVRR, NotDerived&>());
        STATIC_ASSERT(!test<PMF0CVRR, NotDerived const&>());
        STATIC_ASSERT(!test<PMF0CVRR, std::unique_ptr<S const>>());
        STATIC_ASSERT(!test<PMF0CVRR, std::reference_wrapper<S const>>());
        STATIC_ASSERT(!test<PMF0CVRR, std::reference_wrapper<NotDerived>>());
        STATIC_ASSERT(!test<PMF0CVRR, std::unique_ptr<NotDerived>>());

        STATIC_ASSERT(test<PMF1CVRR, S, int>());
        STATIC_ASSERT(!test<PMF1CVRR, S&, int>());
        STATIC_ASSERT(!test<PMF1CVRR, S*, int>());
        STATIC_ASSERT(!test<PMF1CVRR, S*&, int>());
        STATIC_ASSERT(!test<PMF1CVRR, std::unique_ptr<S>, int>());
        STATIC_ASSERT(!test<PMF1CVRR, std::unique_ptr<DerivesFrom<S>>, int>());
        STATIC_ASSERT(!test<PMF1CVRR, std::reference_wrapper<S>, int>());
        STATIC_ASSERT(!test<PMF1CVRR, std::reference_wrapper<S> const&, int>());
        STATIC_ASSERT(!test<PMF1CVRR, std::reference_wrapper<DerivesFrom<S>>, int>());
        STATIC_ASSERT(!test<PMF1CVRR, std::reference_wrapper<DerivesFrom<S>> const&, int>());
        STATIC_ASSERT(!test<PMF1CVRR, S const&, int>());
        STATIC_ASSERT(!test<PMF1CVRR, S volatile&, int>());
        STATIC_ASSERT(!test<PMF1CVRR, S const volatile&, int>());
        STATIC_ASSERT(!test<PMF1CVRR, NotDerived&, int>());
        STATIC_ASSERT(!test<PMF1CVRR, NotDerived const&, int>());
        STATIC_ASSERT(!test<PMF1CVRR, std::unique_ptr<S const>, int>());
        STATIC_ASSERT(!test<PMF1CVRR, std::reference_wrapper<S const>, int>());
        STATIC_ASSERT(!test<PMF1CVRR, std::reference_wrapper<NotDerived>, int>());
        STATIC_ASSERT(!test<PMF1CVRR, std::unique_ptr<NotDerived>, int>());

        STATIC_ASSERT(test<PMF2CVRR, S, int, int>());
        STATIC_ASSERT(!test<PMF2CVRR, S&, int, int>());
        STATIC_ASSERT(!test<PMF2CVRR, S*, int, int>());
        STATIC_ASSERT(!test<PMF2CVRR, S*&, int, int>());
        STATIC_ASSERT(!test<PMF2CVRR, std::unique_ptr<S>, int, int>());
        STATIC_ASSERT(!test<PMF2CVRR, std::unique_ptr<DerivesFrom<S>>, int, int>());
        STATIC_ASSERT(!test<PMF2CVRR, std::reference_wrapper<S>, int, int>());
        STATIC_ASSERT(!test<PMF2CVRR, std::reference_wrapper<S> const&, int, int>());
        STATIC_ASSERT(!test<PMF2CVRR, std::reference_wrapper<DerivesFrom<S>>, int, int>());
        STATIC_ASSERT(!test<PMF2CVRR, std::reference_wrapper<DerivesFrom<S>> const&, int, int>());
        STATIC_ASSERT(!test<PMF2CVRR, S const&, int, int>());
        STATIC_ASSERT(!test<PMF2CVRR, S volatile&, int, int>());
        STATIC_ASSERT(!test<PMF2CVRR, S const volatile&, int, int>());
        STATIC_ASSERT(!test<PMF2CVRR, std::unique_ptr<S const>, int, int>());
        STATIC_ASSERT(!test<PMF2CVRR, std::reference_wrapper<S const>, int, int>());
        STATIC_ASSERT(!test<PMF2CVRR, NotDerived const&, int, int>());
        STATIC_ASSERT(!test<PMF2CVRR, std::reference_wrapper<NotDerived>, int, int>());
        STATIC_ASSERT(!test<PMF2CVRR, std::unique_ptr<NotDerived>, int, int>());

        STATIC_ASSERT(!test<PMF1PCVRR, S&, int>());
        STATIC_ASSERT(!test<PMF1PCVRR, S&, int, long>());
    }
// clang-format on
}

#undef CALLCONV
#undef MCALLCONV
#undef NAME
