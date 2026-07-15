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
        using RFn = int (MCALLCONV tag::*)(int) &&;
        {
            // N4849 [func.require]/1.1: "... f is a pointer to member function of a class T and
            // is_base_of_v<T, remove_reference_t<decltype(t_1)>> is true"
            static_assert(test<Fn, tag&, int>());
            static_assert(test<Fn, DerivesFrom<tag>&, int>());
            static_assert(test<RFn, tag&&, int>());
            static_assert(!test<RFn, tag&, int>());
            static_assert(!test<Fn, tag&>());
            static_assert(!test<Fn, tag const&, int>());
        }
        {
            // N4849 [func.require]/1.2: "... f is a pointer to a member function of a class T and
            // remove_cvref_t<decltype(t_1)> is a specialization of reference_wrapper"
            using T  = std::reference_wrapper<tag>;
            using DT = std::reference_wrapper<DerivesFrom<tag>>;
            using CT = std::reference_wrapper<tag const>;
            static_assert(test<Fn, T&, int>());
            static_assert(test<Fn, DT&, int>());
            static_assert(test<Fn, T const&, int>());
            static_assert(test<Fn, T&&, int>());
            static_assert(!test<Fn, CT&, int>());
            static_assert(!test<RFn, T, int>());
        }
        {
            // N4849 [func.require]/1.3: "... f is a pointer to a member function of a class T and t_1 does not satisfy
            // the previous two items"
            using T  = tag*;
            using DT = DerivesFrom<tag>*;
            using CT = tag const*;
            using ST = std::unique_ptr<tag>;
            static_assert(test<Fn, T&, int>());
            static_assert(test<Fn, DT&, int>());
            static_assert(test<Fn, T const&, int>());
            static_assert(test<Fn, T&&, int>());
            static_assert(test<Fn, ST, int>());
            static_assert(!test<Fn, CT&, int>());
            static_assert(!test<RFn, T, int>());
        }
    }

    {
        // N4849 [func.require]/1.7: "f(t_1, t_2, ..., t_N) in all other cases"
        {
            // function pointer
            using Fp = void(CALLCONV*)(tag&, int);
            static_assert(test<Fp, tag&, int>());
            static_assert(test<Fp, DerivesFrom<tag>&, int>());
            static_assert(!test<Fp, tag const&, int>());
            static_assert(!test<Fp>());
            static_assert(!test<Fp, tag&>());
        }
        {
            // function reference
            using Fp = void(CALLCONV&)(tag&, int);
            static_assert(test<Fp, tag&, int>());
            static_assert(test<Fp, DerivesFrom<tag>&, int>());
            static_assert(!test<Fp, tag const&, int>());
            static_assert(!test<Fp>());
            static_assert(!test<Fp, tag&>());
        }
        {
            // function object
            struct not_callable_with_int {
                int operator()(int) = delete;
                int MCALLCONV operator()(tag) {
                    return 42;
                }
            };

            static_assert(test<not_callable_with_int, tag>());
            static_assert(!test<not_callable_with_int, int>());
        }
    }

    {
        // function object
        static_assert(test<S, int>());
        static_assert(test<S&, unsigned char, int&>());
        static_assert(test<S const&, unsigned char, int&>());
        static_assert(test<S volatile&, unsigned char, int&>());
        static_assert(test<S const volatile&, unsigned char, int&>());

        static_assert(test<ImplicitTo<void(CALLCONV*)(long)>, int>());
        static_assert(test<ImplicitTo<void(CALLCONV*)(long)>, char>());
        static_assert(test<ImplicitTo<void(CALLCONV*)(long)>, float>());
        static_assert(!test<ImplicitTo<void(CALLCONV*)(long)>, char const*>());
        static_assert(!test<ImplicitTo<void(CALLCONV*)(long)>, S const&>());

        auto lambda_lambda_lambda = [x = 42](int, double) { return x; };
        static_assert(test<decltype((lambda_lambda_lambda)), int, double>());
        static_assert(test<decltype((lambda_lambda_lambda)), double, int>());
        static_assert(test<decltype((lambda_lambda_lambda)), char, char>());
        static_assert(!test<decltype((lambda_lambda_lambda))>());
        static_assert(!test<decltype((lambda_lambda_lambda)), char const*, double>());
        static_assert(!test<decltype((lambda_lambda_lambda)), double, char const*>());
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
        static_assert(test<RF0>());
        static_assert(test<RF1, int>());
        static_assert(test<RF2, int, long>());
        static_assert(test<RF3, int, long, int>());
        static_assert(test<RF4, int, float, void*>());
        static_assert(test<PF0>());
        static_assert(test<PF1, int>());
        static_assert(test<PF2, int, long>());
        static_assert(test<PF3, int, long, int>());
        static_assert(test<PF4, int, float, void*>());
        static_assert(test<RPF0>());
        static_assert(test<RPF1, int>());
        static_assert(test<RPF2, int, long>());
        static_assert(test<RPF3, int, long, int>());
        static_assert(test<RPF4, int, float, void*>());
    }

    { // pointer to member function
        using PMF0  = int (MCALLCONV S::*)();
        using PMF1  = int* (MCALLCONV S::*) (long);
        using PMF2  = int& (MCALLCONV S::*) (long, int);
        using PMF1P = int const& (S::*) (int, ...);
        static_assert(test<PMF0, S>());
        static_assert(test<PMF0, S&>());
        static_assert(test<PMF0, S*>());
        static_assert(test<PMF0, S*&>());
        static_assert(test<PMF0, std::reference_wrapper<S>>());
        static_assert(test<PMF0, std::reference_wrapper<S> const&>());
        static_assert(test<PMF0, std::reference_wrapper<DerivesFrom<S>>>());
        static_assert(test<PMF0, std::reference_wrapper<DerivesFrom<S>> const&>());
        static_assert(test<PMF0, std::unique_ptr<S>>());
        static_assert(test<PMF0, std::unique_ptr<DerivesFrom<S>>>());
        static_assert(!test<PMF0, S const&>());
        static_assert(!test<PMF0, S volatile&>());
        static_assert(!test<PMF0, S const volatile&>());
        static_assert(!test<PMF0, NotDerived&>());
        static_assert(!test<PMF0, NotDerived const&>());
        static_assert(!test<PMF0, std::unique_ptr<S const>>());
        static_assert(!test<PMF0, std::reference_wrapper<S const>>());
        static_assert(!test<PMF0, std::reference_wrapper<NotDerived>>());
        static_assert(!test<PMF0, std::unique_ptr<NotDerived>>());

        static_assert(test<PMF1, S, int>());
        static_assert(test<PMF1, S&, int>());
        static_assert(test<PMF1, S*, int>());
        static_assert(test<PMF1, S*&, int>());
        static_assert(test<PMF1, std::unique_ptr<S>, int>());
        static_assert(test<PMF1, std::unique_ptr<DerivesFrom<S>>, int>());
        static_assert(test<PMF1, std::reference_wrapper<S>, int>());
        static_assert(test<PMF1, std::reference_wrapper<S> const&, int>());
        static_assert(test<PMF1, std::reference_wrapper<DerivesFrom<S>>, int>());
        static_assert(test<PMF1, std::reference_wrapper<DerivesFrom<S>> const&, int>());
        static_assert(!test<PMF1, S const&, int>());
        static_assert(!test<PMF1, S volatile&, int>());
        static_assert(!test<PMF1, S const volatile&, int>());
        static_assert(!test<PMF1, NotDerived&, int>());
        static_assert(!test<PMF1, NotDerived const&, int>());
        static_assert(!test<PMF1, std::unique_ptr<S const>, int>());
        static_assert(!test<PMF1, std::reference_wrapper<S const>, int>());
        static_assert(!test<PMF1, std::reference_wrapper<NotDerived>, int>());
        static_assert(!test<PMF1, std::unique_ptr<NotDerived>, int>());

        static_assert(test<PMF2, S, int, int>());
        static_assert(test<PMF2, S&, int, int>());
        static_assert(test<PMF2, S*, int, int>());
        static_assert(test<PMF2, S*&, int, int>());
        static_assert(test<PMF2, std::unique_ptr<S>, int, int>());
        static_assert(test<PMF2, std::unique_ptr<DerivesFrom<S>>, int, int>());
        static_assert(test<PMF2, std::reference_wrapper<S>, int, int>());
        static_assert(test<PMF2, std::reference_wrapper<S> const&, int, int>());
        static_assert(test<PMF2, std::reference_wrapper<DerivesFrom<S>>, int, int>());
        static_assert(test<PMF2, std::reference_wrapper<DerivesFrom<S>> const&, int, int>());
        static_assert(!test<PMF2, S const&, int, int>());
        static_assert(!test<PMF2, S volatile&, int, int>());
        static_assert(!test<PMF2, S const volatile&, int, int>());
        static_assert(!test<PMF2, std::unique_ptr<S const>, int, int>());
        static_assert(!test<PMF2, std::reference_wrapper<S const>, int, int>());
        static_assert(!test<PMF2, NotDerived const&, int, int>());
        static_assert(!test<PMF2, std::reference_wrapper<NotDerived>, int, int>());
        static_assert(!test<PMF2, std::unique_ptr<NotDerived>, int, int>());

        static_assert(test<PMF1P, S&, int>());
        static_assert(test<PMF1P, S&, int, long>());

        using PMF0C  = int (MCALLCONV S::*)() const;
        using PMF1C  = int* (MCALLCONV S::*) (long) const;
        using PMF2C  = int& (MCALLCONV S::*) (long, int) const;
        using PMF1PC = int const& (S::*) (int, ...) const;
        static_assert(test<PMF0C, S>());
        static_assert(test<PMF0C, S&>());
        static_assert(test<PMF0C, S const&>());
        static_assert(test<PMF0C, S*>());
        static_assert(test<PMF0C, S const*>());
        static_assert(test<PMF0C, S*&>());
        static_assert(test<PMF0C, S const*&>());
        static_assert(test<PMF0C, std::unique_ptr<S>>());
        static_assert(test<PMF0C, std::unique_ptr<DerivesFrom<S>>>());
        static_assert(test<PMF0C, std::reference_wrapper<S>>());
        static_assert(test<PMF0C, std::reference_wrapper<S const>>());
        static_assert(test<PMF0C, std::reference_wrapper<S> const&>());
        static_assert(test<PMF0C, std::reference_wrapper<S const> const&>());
        static_assert(test<PMF0C, std::reference_wrapper<DerivesFrom<S>>>());
        static_assert(test<PMF0C, std::reference_wrapper<DerivesFrom<S> const>>());
        static_assert(test<PMF0C, std::reference_wrapper<DerivesFrom<S>> const&>());
        static_assert(test<PMF0C, std::reference_wrapper<DerivesFrom<S> const> const&>());
        static_assert(!test<PMF0C, S volatile&>());
        static_assert(!test<PMF0C, S const volatile&>());

        static_assert(test<PMF1C, S, int>());
        static_assert(test<PMF1C, S&, int>());
        static_assert(test<PMF1C, S const&, int>());
        static_assert(test<PMF1C, S*, int>());
        static_assert(test<PMF1C, S const*, int>());
        static_assert(test<PMF1C, S*&, int>());
        static_assert(test<PMF1C, S const*&, int>());
        static_assert(test<PMF1C, std::unique_ptr<S>, int>());
        static_assert(!test<PMF1C, S volatile&, int>());
        static_assert(!test<PMF1C, S const volatile&, int>());

        static_assert(test<PMF2C, S, int, int>());
        static_assert(test<PMF2C, S&, int, int>());
        static_assert(test<PMF2C, S const&, int, int>());
        static_assert(test<PMF2C, S*, int, int>());
        static_assert(test<PMF2C, S const*, int, int>());
        static_assert(test<PMF2C, S*&, int, int>());
        static_assert(test<PMF2C, S const*&, int, int>());
        static_assert(test<PMF2C, std::unique_ptr<S>, int, int>());
        static_assert(!test<PMF2C, S volatile&, int, int>());
        static_assert(!test<PMF2C, S const volatile&, int, int>());

        static_assert(test<PMF1PC, S&, int>());
        static_assert(test<PMF1PC, S&, int, long>());

        using PMF0V  = int (MCALLCONV S::*)() volatile;
        using PMF1V  = int* (MCALLCONV S::*) (long) volatile;
        using PMF2V  = int& (MCALLCONV S::*) (long, int) volatile;
        using PMF1PV = int const& (S::*) (int, ...) volatile;
        static_assert(test<PMF0V, S>());
        static_assert(test<PMF0V, S&>());
        static_assert(test<PMF0V, S volatile&>());
        static_assert(test<PMF0V, S*>());
        static_assert(test<PMF0V, S volatile*>());
        static_assert(test<PMF0V, S*&>());
        static_assert(test<PMF0V, S volatile*&>());
        static_assert(test<PMF0V, std::unique_ptr<S>>());
        static_assert(!test<PMF0V, S const&>());
        static_assert(!test<PMF0V, S const volatile&>());

        static_assert(test<PMF1V, S, int>());
        static_assert(test<PMF1V, S&, int>());
        static_assert(test<PMF1V, S volatile&, int>());
        static_assert(test<PMF1V, S*, int>());
        static_assert(test<PMF1V, S volatile*, int>());
        static_assert(test<PMF1V, S*&, int>());
        static_assert(test<PMF1V, S volatile*&, int>());
        static_assert(test<PMF1V, std::unique_ptr<S>, int>());
        static_assert(!test<PMF1V, S const&, int>());
        static_assert(!test<PMF1V, S const volatile&, int>());

        static_assert(test<PMF2V, S, int, int>());
        static_assert(test<PMF2V, S&, int, int>());
        static_assert(test<PMF2V, S volatile&, int, int>());
        static_assert(test<PMF2V, S*, int, int>());
        static_assert(test<PMF2V, S volatile*, int, int>());
        static_assert(test<PMF2V, S*&, int, int>());
        static_assert(test<PMF2V, S volatile*&, int, int>());
        static_assert(test<PMF2V, std::unique_ptr<S>, int, int>());
        static_assert(!test<PMF2V, S const&, int, int>());
        static_assert(!test<PMF2V, S const volatile&, int, int>());

        static_assert(test<PMF1PV, S&, int>());
        static_assert(test<PMF1PV, S&, int, long>());

        using PMF0CV  = int (MCALLCONV S::*)() const volatile;
        using PMF1CV  = int* (MCALLCONV S::*) (long) const volatile;
        using PMF2CV  = int& (MCALLCONV S::*) (long, int) const volatile;
        using PMF1PCV = int const& (S::*) (int, ...) const volatile;
        static_assert(test<PMF0CV, S>());
        static_assert(test<PMF0CV, S&>());
        static_assert(test<PMF0CV, S const&>());
        static_assert(test<PMF0CV, S volatile&>());
        static_assert(test<PMF0CV, S const volatile&>());
        static_assert(test<PMF0CV, S*>());
        static_assert(test<PMF0CV, S const*>());
        static_assert(test<PMF0CV, S volatile*>());
        static_assert(test<PMF0CV, S const volatile*>());
        static_assert(test<PMF0CV, S*&>());
        static_assert(test<PMF0CV, S const*&>());
        static_assert(test<PMF0CV, S volatile*&>());
        static_assert(test<PMF0CV, S const volatile*&>());
        static_assert(test<PMF0CV, std::unique_ptr<S>>());

        static_assert(test<PMF1CV, S, int>());
        static_assert(test<PMF1CV, S&, int>());
        static_assert(test<PMF1CV, S const&, int>());
        static_assert(test<PMF1CV, S volatile&, int>());
        static_assert(test<PMF1CV, S const volatile&, int>());
        static_assert(test<PMF1CV, S*, int>());
        static_assert(test<PMF1CV, S const*, int>());
        static_assert(test<PMF1CV, S volatile*, int>());
        static_assert(test<PMF1CV, S const volatile*, int>());
        static_assert(test<PMF1CV, S*&, int>());
        static_assert(test<PMF1CV, S const*&, int>());
        static_assert(test<PMF1CV, S volatile*&, int>());
        static_assert(test<PMF1CV, S const volatile*&, int>());
        static_assert(test<PMF1CV, std::unique_ptr<S>, int>());

        static_assert(test<PMF2CV, S, int, int>());
        static_assert(test<PMF2CV, S&, int, int>());
        static_assert(test<PMF2CV, S const&, int, int>());
        static_assert(test<PMF2CV, S volatile&, int, int>());
        static_assert(test<PMF2CV, S const volatile&, int, int>());
        static_assert(test<PMF2CV, S*, int, int>());
        static_assert(test<PMF2CV, S const*, int, int>());
        static_assert(test<PMF2CV, S volatile*, int, int>());
        static_assert(test<PMF2CV, S const volatile*, int, int>());
        static_assert(test<PMF2CV, S*&, int, int>());
        static_assert(test<PMF2CV, S const*&, int, int>());
        static_assert(test<PMF2CV, S volatile*&, int, int>());
        static_assert(test<PMF2CV, S const volatile*&, int, int>());
        static_assert(test<PMF2CV, std::unique_ptr<S>, int, int>());

        static_assert(test<PMF1PCV, S&, int>());
        static_assert(test<PMF1PCV, S&, int, long>());

        using PMF0R  = int (MCALLCONV S::*)() &;
        using PMF1R  = int* (MCALLCONV S::*) (long) &;
        using PMF2R  = int& (MCALLCONV S::*) (long, int) &;
        using PMF1PR = int const& (S::*) (int, ...) &;
        static_assert(!test<PMF0R, S>());
        static_assert(test<PMF0R, S&>());
        static_assert(test<PMF0R, S*>());
        static_assert(test<PMF0R, S*&>());
        static_assert(test<PMF0R, std::reference_wrapper<S>>());
        static_assert(test<PMF0R, std::reference_wrapper<S> const&>());
        static_assert(test<PMF0R, std::reference_wrapper<DerivesFrom<S>>>());
        static_assert(test<PMF0R, std::reference_wrapper<DerivesFrom<S>> const&>());
        static_assert(test<PMF0R, std::unique_ptr<S>>());
        static_assert(test<PMF0R, std::unique_ptr<DerivesFrom<S>>>());
        static_assert(!test<PMF0R, S const&>());
        static_assert(!test<PMF0R, S volatile&>());
        static_assert(!test<PMF0R, S const volatile&>());
        static_assert(!test<PMF0R, NotDerived&>());
        static_assert(!test<PMF0R, NotDerived const&>());
        static_assert(!test<PMF0R, std::unique_ptr<S const>>());
        static_assert(!test<PMF0R, std::reference_wrapper<S const>>());
        static_assert(!test<PMF0R, std::reference_wrapper<NotDerived>>());
        static_assert(!test<PMF0R, std::unique_ptr<NotDerived>>());

        static_assert(!test<PMF1R, S, int>());
        static_assert(test<PMF1R, S&, int>());
        static_assert(test<PMF1R, S*, int>());
        static_assert(test<PMF1R, S*&, int>());
        static_assert(test<PMF1R, std::unique_ptr<S>, int>());
        static_assert(test<PMF1R, std::unique_ptr<DerivesFrom<S>>, int>());
        static_assert(test<PMF1R, std::reference_wrapper<S>, int>());
        static_assert(test<PMF1R, std::reference_wrapper<S> const&, int>());
        static_assert(test<PMF1R, std::reference_wrapper<DerivesFrom<S>>, int>());
        static_assert(test<PMF1R, std::reference_wrapper<DerivesFrom<S>> const&, int>());
        static_assert(!test<PMF1R, S const&, int>());
        static_assert(!test<PMF1R, S volatile&, int>());
        static_assert(!test<PMF1R, S const volatile&, int>());
        static_assert(!test<PMF1R, NotDerived&, int>());
        static_assert(!test<PMF1R, NotDerived const&, int>());
        static_assert(!test<PMF1R, std::unique_ptr<S const>, int>());
        static_assert(!test<PMF1R, std::reference_wrapper<S const>, int>());
        static_assert(!test<PMF1R, std::reference_wrapper<NotDerived>, int>());
        static_assert(!test<PMF1R, std::unique_ptr<NotDerived>, int>());

        static_assert(!test<PMF2R, S, int, int>());
        static_assert(test<PMF2R, S&, int, int>());
        static_assert(test<PMF2R, S*, int, int>());
        static_assert(test<PMF2R, S*&, int, int>());
        static_assert(test<PMF2R, std::unique_ptr<S>, int, int>());
        static_assert(test<PMF2R, std::unique_ptr<DerivesFrom<S>>, int, int>());
        static_assert(test<PMF2R, std::reference_wrapper<S>, int, int>());
        static_assert(test<PMF2R, std::reference_wrapper<S> const&, int, int>());
        static_assert(test<PMF2R, std::reference_wrapper<DerivesFrom<S>>, int, int>());
        static_assert(test<PMF2R, std::reference_wrapper<DerivesFrom<S>> const&, int, int>());
        static_assert(!test<PMF2R, S const&, int, int>());
        static_assert(!test<PMF2R, S volatile&, int, int>());
        static_assert(!test<PMF2R, S const volatile&, int, int>());
        static_assert(!test<PMF2R, std::unique_ptr<S const>, int, int>());
        static_assert(!test<PMF2R, std::reference_wrapper<S const>, int, int>());
        static_assert(!test<PMF2R, NotDerived const&, int, int>());
        static_assert(!test<PMF2R, std::reference_wrapper<NotDerived>, int, int>());
        static_assert(!test<PMF2R, std::unique_ptr<NotDerived>, int, int>());

        static_assert(test<PMF1PR, S&, int>());
        static_assert(test<PMF1PR, S&, int, long>());

        using PMF0CR  = int (MCALLCONV S::*)() const&;
        using PMF1CR  = int* (MCALLCONV S::*) (long) const&;
        using PMF2CR  = int& (MCALLCONV S::*) (long, int) const&;
        using PMF1PCR = int const& (S::*) (int, ...) const&;
        static_assert(test<PMF0CR, S>());
        static_assert(test<PMF0CR, S&>());
        static_assert(test<PMF0CR, S*>());
        static_assert(test<PMF0CR, S*&>());
        static_assert(test<PMF0CR, std::reference_wrapper<S>>());
        static_assert(test<PMF0CR, std::reference_wrapper<S> const&>());
        static_assert(test<PMF0CR, std::reference_wrapper<DerivesFrom<S>>>());
        static_assert(test<PMF0CR, std::reference_wrapper<DerivesFrom<S>> const&>());
        static_assert(test<PMF0CR, std::unique_ptr<S>>());
        static_assert(test<PMF0CR, std::unique_ptr<DerivesFrom<S>>>());
        static_assert(test<PMF0CR, S const&>());
        static_assert(!test<PMF0CR, S volatile&>());
        static_assert(!test<PMF0CR, S const volatile&>());
        static_assert(!test<PMF0CR, NotDerived&>());
        static_assert(!test<PMF0CR, NotDerived const&>());
        static_assert(test<PMF0CR, std::unique_ptr<S const>>());
        static_assert(test<PMF0CR, std::reference_wrapper<S const>>());
        static_assert(!test<PMF0CR, std::reference_wrapper<NotDerived>>());
        static_assert(!test<PMF0CR, std::unique_ptr<NotDerived>>());

        static_assert(test<PMF1CR, S, int>());
        static_assert(test<PMF1CR, S&, int>());
        static_assert(test<PMF1CR, S*, int>());
        static_assert(test<PMF1CR, S*&, int>());
        static_assert(test<PMF1CR, std::unique_ptr<S>, int>());
        static_assert(test<PMF1CR, std::unique_ptr<DerivesFrom<S>>, int>());
        static_assert(test<PMF1CR, std::reference_wrapper<S>, int>());
        static_assert(test<PMF1CR, std::reference_wrapper<S> const&, int>());
        static_assert(test<PMF1CR, std::reference_wrapper<DerivesFrom<S>>, int>());
        static_assert(test<PMF1CR, std::reference_wrapper<DerivesFrom<S>> const&, int>());
        static_assert(test<PMF1CR, S const&, int>());
        static_assert(!test<PMF1CR, S volatile&, int>());
        static_assert(!test<PMF1CR, S const volatile&, int>());
        static_assert(!test<PMF1CR, NotDerived&, int>());
        static_assert(!test<PMF1CR, NotDerived const&, int>());
        static_assert(test<PMF1CR, std::unique_ptr<S const>, int>());
        static_assert(test<PMF1CR, std::reference_wrapper<S const>, int>());
        static_assert(!test<PMF1CR, std::reference_wrapper<NotDerived>, int>());
        static_assert(!test<PMF1CR, std::unique_ptr<NotDerived>, int>());

        static_assert(test<PMF2CR, S, int, int>());
        static_assert(test<PMF2CR, S&, int, int>());
        static_assert(test<PMF2CR, S*, int, int>());
        static_assert(test<PMF2CR, S*&, int, int>());
        static_assert(test<PMF2CR, std::unique_ptr<S>, int, int>());
        static_assert(test<PMF2CR, std::unique_ptr<DerivesFrom<S>>, int, int>());
        static_assert(test<PMF2CR, std::reference_wrapper<S>, int, int>());
        static_assert(test<PMF2CR, std::reference_wrapper<S> const&, int, int>());
        static_assert(test<PMF2CR, std::reference_wrapper<DerivesFrom<S>>, int, int>());
        static_assert(test<PMF2CR, std::reference_wrapper<DerivesFrom<S>> const&, int, int>());
        static_assert(test<PMF2CR, S const&, int, int>());
        static_assert(!test<PMF2CR, S volatile&, int, int>());
        static_assert(!test<PMF2CR, S const volatile&, int, int>());
        static_assert(test<PMF2CR, std::unique_ptr<S const>, int, int>());
        static_assert(test<PMF2CR, std::reference_wrapper<S const>, int, int>());
        static_assert(!test<PMF2CR, NotDerived const&, int, int>());
        static_assert(!test<PMF2CR, std::reference_wrapper<NotDerived>, int, int>());
        static_assert(!test<PMF2CR, std::unique_ptr<NotDerived>, int, int>());

        static_assert(test<PMF1PCR, S&, int>());
        static_assert(test<PMF1PCR, S&, int, long>());

        using PMF0CVR  = int (MCALLCONV S::*)() const volatile&;
        using PMF1CVR  = int* (MCALLCONV S::*) (long) const volatile&;
        using PMF2CVR  = int& (MCALLCONV S::*) (long, int) const volatile&;
        using PMF1PCVR = int const& (S::*) (int, ...) const volatile&;
        static_assert(!test<PMF0CVR, S>());
        static_assert(test<PMF0CVR, S&>());
        static_assert(test<PMF0CVR, S*>());
        static_assert(test<PMF0CVR, S*&>());
        static_assert(test<PMF0CVR, std::reference_wrapper<S>>());
        static_assert(test<PMF0CVR, std::reference_wrapper<S> const&>());
        static_assert(test<PMF0CVR, std::reference_wrapper<DerivesFrom<S>>>());
        static_assert(test<PMF0CVR, std::reference_wrapper<DerivesFrom<S>> const&>());
        static_assert(test<PMF0CVR, std::unique_ptr<S>>());
        static_assert(test<PMF0CVR, std::unique_ptr<DerivesFrom<S>>>());
        static_assert(test<PMF0CVR, S const&>());
        static_assert(test<PMF0CVR, S volatile&>());
        static_assert(test<PMF0CVR, S const volatile&>());
        static_assert(!test<PMF0CVR, NotDerived&>());
        static_assert(!test<PMF0CVR, NotDerived const&>());
        static_assert(test<PMF0CVR, std::unique_ptr<S const>>());
        static_assert(test<PMF0CVR, std::reference_wrapper<S const>>());
        static_assert(!test<PMF0CVR, std::reference_wrapper<NotDerived>>());
        static_assert(!test<PMF0CVR, std::unique_ptr<NotDerived>>());

        static_assert(!test<PMF1CVR, S, int>());
        static_assert(test<PMF1CVR, S&, int>());
        static_assert(test<PMF1CVR, S*, int>());
        static_assert(test<PMF1CVR, S*&, int>());
        static_assert(test<PMF1CVR, std::unique_ptr<S>, int>());
        static_assert(test<PMF1CVR, std::unique_ptr<DerivesFrom<S>>, int>());
        static_assert(test<PMF1CVR, std::reference_wrapper<S>, int>());
        static_assert(test<PMF1CVR, std::reference_wrapper<S> const&, int>());
        static_assert(test<PMF1CVR, std::reference_wrapper<DerivesFrom<S>>, int>());
        static_assert(test<PMF1CVR, std::reference_wrapper<DerivesFrom<S>> const&, int>());
        static_assert(test<PMF1CVR, S const&, int>());
        static_assert(test<PMF1CVR, S volatile&, int>());
        static_assert(test<PMF1CVR, S const volatile&, int>());
        static_assert(!test<PMF1CVR, NotDerived&, int>());
        static_assert(!test<PMF1CVR, NotDerived const&, int>());
        static_assert(test<PMF1CVR, std::unique_ptr<S const>, int>());
        static_assert(test<PMF1CVR, std::reference_wrapper<S const>, int>());
        static_assert(!test<PMF1CVR, std::reference_wrapper<NotDerived>, int>());
        static_assert(!test<PMF1CVR, std::unique_ptr<NotDerived>, int>());

        static_assert(!test<PMF2CVR, S, int, int>());
        static_assert(test<PMF2CVR, S&, int, int>());
        static_assert(test<PMF2CVR, S*, int, int>());
        static_assert(test<PMF2CVR, S*&, int, int>());
        static_assert(test<PMF2CVR, std::unique_ptr<S>, int, int>());
        static_assert(test<PMF2CVR, std::unique_ptr<DerivesFrom<S>>, int, int>());
        static_assert(test<PMF2CVR, std::reference_wrapper<S>, int, int>());
        static_assert(test<PMF2CVR, std::reference_wrapper<S> const&, int, int>());
        static_assert(test<PMF2CVR, std::reference_wrapper<DerivesFrom<S>>, int, int>());
        static_assert(test<PMF2CVR, std::reference_wrapper<DerivesFrom<S>> const&, int, int>());
        static_assert(test<PMF2CVR, S const&, int, int>());
        static_assert(test<PMF2CVR, S volatile&, int, int>());
        static_assert(test<PMF2CVR, S const volatile&, int, int>());
        static_assert(test<PMF2CVR, std::unique_ptr<S const>, int, int>());
        static_assert(test<PMF2CVR, std::reference_wrapper<S const>, int, int>());
        static_assert(!test<PMF2CVR, NotDerived const&, int, int>());
        static_assert(!test<PMF2CVR, std::reference_wrapper<NotDerived>, int, int>());
        static_assert(!test<PMF2CVR, std::unique_ptr<NotDerived>, int, int>());

        static_assert(test<PMF1PCVR, S&, int>());
        static_assert(test<PMF1PCVR, S&, int, long>());

        using PMF0RR  = int (MCALLCONV S::*)() &&;
        using PMF1RR  = int* (MCALLCONV S::*) (long) &&;
        using PMF2RR  = int& (MCALLCONV S::*) (long, int) &&;
        using PMF1PRR = int const& (S::*) (int, ...) &&;
        static_assert(test<PMF0RR, S>());
        static_assert(!test<PMF0RR, S&>());
        static_assert(!test<PMF0RR, S*>());
        static_assert(!test<PMF0RR, S*&>());
        static_assert(!test<PMF0RR, std::reference_wrapper<S>>());
        static_assert(!test<PMF0RR, std::reference_wrapper<S> const&>());
        static_assert(!test<PMF0RR, std::reference_wrapper<DerivesFrom<S>>>());
        static_assert(!test<PMF0RR, std::reference_wrapper<DerivesFrom<S>> const&>());
        static_assert(!test<PMF0RR, std::unique_ptr<S>>());
        static_assert(!test<PMF0RR, std::unique_ptr<DerivesFrom<S>>>());
        static_assert(!test<PMF0RR, S const&>());
        static_assert(!test<PMF0RR, S volatile&>());
        static_assert(!test<PMF0RR, S const volatile&>());
        static_assert(!test<PMF0RR, NotDerived&>());
        static_assert(!test<PMF0RR, NotDerived const&>());
        static_assert(!test<PMF0RR, std::unique_ptr<S const>>());
        static_assert(!test<PMF0RR, std::reference_wrapper<S const>>());
        static_assert(!test<PMF0RR, std::reference_wrapper<NotDerived>>());
        static_assert(!test<PMF0RR, std::unique_ptr<NotDerived>>());

        static_assert(test<PMF1RR, S, int>());
        static_assert(!test<PMF1RR, S&, int>());
        static_assert(!test<PMF1RR, S*, int>());
        static_assert(!test<PMF1RR, S*&, int>());
        static_assert(!test<PMF1RR, std::unique_ptr<S>, int>());
        static_assert(!test<PMF1RR, std::unique_ptr<DerivesFrom<S>>, int>());
        static_assert(!test<PMF1RR, std::reference_wrapper<S>, int>());
        static_assert(!test<PMF1RR, std::reference_wrapper<S> const&, int>());
        static_assert(!test<PMF1RR, std::reference_wrapper<DerivesFrom<S>>, int>());
        static_assert(!test<PMF1RR, std::reference_wrapper<DerivesFrom<S>> const&, int>());
        static_assert(!test<PMF1RR, S const&, int>());
        static_assert(!test<PMF1RR, S volatile&, int>());
        static_assert(!test<PMF1RR, S const volatile&, int>());
        static_assert(!test<PMF1RR, NotDerived&, int>());
        static_assert(!test<PMF1RR, NotDerived const&, int>());
        static_assert(!test<PMF1RR, std::unique_ptr<S const>, int>());
        static_assert(!test<PMF1RR, std::reference_wrapper<S const>, int>());
        static_assert(!test<PMF1RR, std::reference_wrapper<NotDerived>, int>());
        static_assert(!test<PMF1RR, std::unique_ptr<NotDerived>, int>());

        static_assert(test<PMF2RR, S, int, int>());
        static_assert(!test<PMF2RR, S&, int, int>());
        static_assert(!test<PMF2RR, S*, int, int>());
        static_assert(!test<PMF2RR, S*&, int, int>());
        static_assert(!test<PMF2RR, std::unique_ptr<S>, int, int>());
        static_assert(!test<PMF2RR, std::unique_ptr<DerivesFrom<S>>, int, int>());
        static_assert(!test<PMF2RR, std::reference_wrapper<S>, int, int>());
        static_assert(!test<PMF2RR, std::reference_wrapper<S> const&, int, int>());
        static_assert(!test<PMF2RR, std::reference_wrapper<DerivesFrom<S>>, int, int>());
        static_assert(!test<PMF2RR, std::reference_wrapper<DerivesFrom<S>> const&, int, int>());
        static_assert(!test<PMF2RR, S const&, int, int>());
        static_assert(!test<PMF2RR, S volatile&, int, int>());
        static_assert(!test<PMF2RR, S const volatile&, int, int>());
        static_assert(!test<PMF2RR, std::unique_ptr<S const>, int, int>());
        static_assert(!test<PMF2RR, std::reference_wrapper<S const>, int, int>());
        static_assert(!test<PMF2RR, NotDerived const&, int, int>());
        static_assert(!test<PMF2RR, std::reference_wrapper<NotDerived>, int, int>());
        static_assert(!test<PMF2RR, std::unique_ptr<NotDerived>, int, int>());

        static_assert(!test<PMF1PRR, S&, int>());
        static_assert(!test<PMF1PRR, S&, int, long>());

        using PMF0CRR  = int (MCALLCONV S::*)() const&&;
        using PMF1CRR  = int* (MCALLCONV S::*) (long) const&&;
        using PMF2CRR  = int& (MCALLCONV S::*) (long, int) const&&;
        using PMF1PCRR = int const& (S::*) (int, ...) const&&;
        static_assert(test<PMF0CRR, S>());
        static_assert(!test<PMF0CRR, S&>());
        static_assert(!test<PMF0CRR, S*>());
        static_assert(!test<PMF0CRR, S*&>());
        static_assert(!test<PMF0CRR, std::reference_wrapper<S>>());
        static_assert(!test<PMF0CRR, std::reference_wrapper<S> const&>());
        static_assert(!test<PMF0CRR, std::reference_wrapper<DerivesFrom<S>>>());
        static_assert(!test<PMF0CRR, std::reference_wrapper<DerivesFrom<S>> const&>());
        static_assert(!test<PMF0CRR, std::unique_ptr<S>>());
        static_assert(!test<PMF0CRR, std::unique_ptr<DerivesFrom<S>>>());
        static_assert(!test<PMF0CRR, S const&>());
        static_assert(!test<PMF0CRR, S volatile&>());
        static_assert(!test<PMF0CRR, S const volatile&>());
        static_assert(!test<PMF0CRR, NotDerived&>());
        static_assert(!test<PMF0CRR, NotDerived const&>());
        static_assert(!test<PMF0CRR, std::unique_ptr<S const>>());
        static_assert(!test<PMF0CRR, std::reference_wrapper<S const>>());
        static_assert(!test<PMF0CRR, std::reference_wrapper<NotDerived>>());
        static_assert(!test<PMF0CRR, std::unique_ptr<NotDerived>>());

        static_assert(test<PMF1CRR, S, int>());
        static_assert(!test<PMF1CRR, S&, int>());
        static_assert(!test<PMF1CRR, S*, int>());
        static_assert(!test<PMF1CRR, S*&, int>());
        static_assert(!test<PMF1CRR, std::unique_ptr<S>, int>());
        static_assert(!test<PMF1CRR, std::unique_ptr<DerivesFrom<S>>, int>());
        static_assert(!test<PMF1CRR, std::reference_wrapper<S>, int>());
        static_assert(!test<PMF1CRR, std::reference_wrapper<S> const&, int>());
        static_assert(!test<PMF1CRR, std::reference_wrapper<DerivesFrom<S>>, int>());
        static_assert(!test<PMF1CRR, std::reference_wrapper<DerivesFrom<S>> const&, int>());
        static_assert(!test<PMF1CRR, S const&, int>());
        static_assert(!test<PMF1CRR, S volatile&, int>());
        static_assert(!test<PMF1CRR, S const volatile&, int>());
        static_assert(!test<PMF1CRR, NotDerived&, int>());
        static_assert(!test<PMF1CRR, NotDerived const&, int>());
        static_assert(!test<PMF1CRR, std::unique_ptr<S const>, int>());
        static_assert(!test<PMF1CRR, std::reference_wrapper<S const>, int>());
        static_assert(!test<PMF1CRR, std::reference_wrapper<NotDerived>, int>());
        static_assert(!test<PMF1CRR, std::unique_ptr<NotDerived>, int>());

        static_assert(test<PMF2CRR, S, int, int>());
        static_assert(!test<PMF2CRR, S&, int, int>());
        static_assert(!test<PMF2CRR, S*, int, int>());
        static_assert(!test<PMF2CRR, S*&, int, int>());
        static_assert(!test<PMF2CRR, std::unique_ptr<S>, int, int>());
        static_assert(!test<PMF2CRR, std::unique_ptr<DerivesFrom<S>>, int, int>());
        static_assert(!test<PMF2CRR, std::reference_wrapper<S>, int, int>());
        static_assert(!test<PMF2CRR, std::reference_wrapper<S> const&, int, int>());
        static_assert(!test<PMF2CRR, std::reference_wrapper<DerivesFrom<S>>, int, int>());
        static_assert(!test<PMF2CRR, std::reference_wrapper<DerivesFrom<S>> const&, int, int>());
        static_assert(!test<PMF2CRR, S const&, int, int>());
        static_assert(!test<PMF2CRR, S volatile&, int, int>());
        static_assert(!test<PMF2CRR, S const volatile&, int, int>());
        static_assert(!test<PMF2CRR, std::unique_ptr<S const>, int, int>());
        static_assert(!test<PMF2CRR, std::reference_wrapper<S const>, int, int>());
        static_assert(!test<PMF2CRR, NotDerived const&, int, int>());
        static_assert(!test<PMF2CRR, std::reference_wrapper<NotDerived>, int, int>());
        static_assert(!test<PMF2CRR, std::unique_ptr<NotDerived>, int, int>());

        static_assert(!test<PMF1PCRR, S&, int>());
        static_assert(!test<PMF1PCRR, S&, int, long>());

        using PMF0VRR  = int (MCALLCONV S::*)() volatile&&;
        using PMF1VRR  = int* (MCALLCONV S::*) (long) volatile&&;
        using PMF2VRR  = int& (MCALLCONV S::*) (long, int) volatile&&;
        using PMF1PVRR = int const& (S::*) (int, ...) volatile&&;
        static_assert(test<PMF0VRR, S>());
        static_assert(!test<PMF0VRR, S&>());
        static_assert(!test<PMF0VRR, S*>());
        static_assert(!test<PMF0VRR, S*&>());
        static_assert(!test<PMF0VRR, std::reference_wrapper<S>>());
        static_assert(!test<PMF0VRR, std::reference_wrapper<S> const&>());
        static_assert(!test<PMF0VRR, std::reference_wrapper<DerivesFrom<S>>>());
        static_assert(!test<PMF0VRR, std::reference_wrapper<DerivesFrom<S>> const&>());
        static_assert(!test<PMF0VRR, std::unique_ptr<S>>());
        static_assert(!test<PMF0VRR, std::unique_ptr<DerivesFrom<S>>>());
        static_assert(!test<PMF0VRR, S const&>());
        static_assert(!test<PMF0VRR, S volatile&>());
        static_assert(!test<PMF0VRR, S const volatile&>());
        static_assert(!test<PMF0VRR, NotDerived&>());
        static_assert(!test<PMF0VRR, NotDerived const&>());
        static_assert(!test<PMF0VRR, std::unique_ptr<S const>>());
        static_assert(!test<PMF0VRR, std::reference_wrapper<S const>>());
        static_assert(!test<PMF0VRR, std::reference_wrapper<NotDerived>>());
        static_assert(!test<PMF0VRR, std::unique_ptr<NotDerived>>());

        static_assert(test<PMF1VRR, S, int>());
        static_assert(!test<PMF1VRR, S&, int>());
        static_assert(!test<PMF1VRR, S*, int>());
        static_assert(!test<PMF1VRR, S*&, int>());
        static_assert(!test<PMF1VRR, std::unique_ptr<S>, int>());
        static_assert(!test<PMF1VRR, std::unique_ptr<DerivesFrom<S>>, int>());
        static_assert(!test<PMF1VRR, std::reference_wrapper<S>, int>());
        static_assert(!test<PMF1VRR, std::reference_wrapper<S> const&, int>());
        static_assert(!test<PMF1VRR, std::reference_wrapper<DerivesFrom<S>>, int>());
        static_assert(!test<PMF1VRR, std::reference_wrapper<DerivesFrom<S>> const&, int>());
        static_assert(!test<PMF1VRR, S const&, int>());
        static_assert(!test<PMF1VRR, S volatile&, int>());
        static_assert(!test<PMF1VRR, S const volatile&, int>());
        static_assert(!test<PMF1VRR, NotDerived&, int>());
        static_assert(!test<PMF1VRR, NotDerived const&, int>());
        static_assert(!test<PMF1VRR, std::unique_ptr<S const>, int>());
        static_assert(!test<PMF1VRR, std::reference_wrapper<S const>, int>());
        static_assert(!test<PMF1VRR, std::reference_wrapper<NotDerived>, int>());
        static_assert(!test<PMF1VRR, std::unique_ptr<NotDerived>, int>());

        static_assert(test<PMF2VRR, S, int, int>());
        static_assert(!test<PMF2VRR, S&, int, int>());
        static_assert(!test<PMF2VRR, S*, int, int>());
        static_assert(!test<PMF2VRR, S*&, int, int>());
        static_assert(!test<PMF2VRR, std::unique_ptr<S>, int, int>());
        static_assert(!test<PMF2VRR, std::unique_ptr<DerivesFrom<S>>, int, int>());
        static_assert(!test<PMF2VRR, std::reference_wrapper<S>, int, int>());
        static_assert(!test<PMF2VRR, std::reference_wrapper<S> const&, int, int>());
        static_assert(!test<PMF2VRR, std::reference_wrapper<DerivesFrom<S>>, int, int>());
        static_assert(!test<PMF2VRR, std::reference_wrapper<DerivesFrom<S>> const&, int, int>());
        static_assert(!test<PMF2VRR, S const&, int, int>());
        static_assert(!test<PMF2VRR, S volatile&, int, int>());
        static_assert(!test<PMF2VRR, S const volatile&, int, int>());
        static_assert(!test<PMF2VRR, std::unique_ptr<S const>, int, int>());
        static_assert(!test<PMF2VRR, std::reference_wrapper<S const>, int, int>());
        static_assert(!test<PMF2VRR, NotDerived const&, int, int>());
        static_assert(!test<PMF2VRR, std::reference_wrapper<NotDerived>, int, int>());
        static_assert(!test<PMF2VRR, std::unique_ptr<NotDerived>, int, int>());

        static_assert(!test<PMF1PVRR, S&, int>());
        static_assert(!test<PMF1PVRR, S&, int, long>());

        using PMF0CVRR  = int (MCALLCONV S::*)() const volatile&&;
        using PMF1CVRR  = int* (MCALLCONV S::*) (long) const volatile&&;
        using PMF2CVRR  = int& (MCALLCONV S::*) (long, int) const volatile&&;
        using PMF1PCVRR = int const& (S::*) (int, ...) const volatile&&;
        static_assert(test<PMF0CVRR, S>());
        static_assert(!test<PMF0CVRR, S&>());
        static_assert(!test<PMF0CVRR, S*>());
        static_assert(!test<PMF0CVRR, S*&>());
        static_assert(!test<PMF0CVRR, std::reference_wrapper<S>>());
        static_assert(!test<PMF0CVRR, std::reference_wrapper<S> const&>());
        static_assert(!test<PMF0CVRR, std::reference_wrapper<DerivesFrom<S>>>());
        static_assert(!test<PMF0CVRR, std::reference_wrapper<DerivesFrom<S>> const&>());
        static_assert(!test<PMF0CVRR, std::unique_ptr<S>>());
        static_assert(!test<PMF0CVRR, std::unique_ptr<DerivesFrom<S>>>());
        static_assert(!test<PMF0CVRR, S const&>());
        static_assert(!test<PMF0CVRR, S volatile&>());
        static_assert(!test<PMF0CVRR, S const volatile&>());
        static_assert(!test<PMF0CVRR, NotDerived&>());
        static_assert(!test<PMF0CVRR, NotDerived const&>());
        static_assert(!test<PMF0CVRR, std::unique_ptr<S const>>());
        static_assert(!test<PMF0CVRR, std::reference_wrapper<S const>>());
        static_assert(!test<PMF0CVRR, std::reference_wrapper<NotDerived>>());
        static_assert(!test<PMF0CVRR, std::unique_ptr<NotDerived>>());

        static_assert(test<PMF1CVRR, S, int>());
        static_assert(!test<PMF1CVRR, S&, int>());
        static_assert(!test<PMF1CVRR, S*, int>());
        static_assert(!test<PMF1CVRR, S*&, int>());
        static_assert(!test<PMF1CVRR, std::unique_ptr<S>, int>());
        static_assert(!test<PMF1CVRR, std::unique_ptr<DerivesFrom<S>>, int>());
        static_assert(!test<PMF1CVRR, std::reference_wrapper<S>, int>());
        static_assert(!test<PMF1CVRR, std::reference_wrapper<S> const&, int>());
        static_assert(!test<PMF1CVRR, std::reference_wrapper<DerivesFrom<S>>, int>());
        static_assert(!test<PMF1CVRR, std::reference_wrapper<DerivesFrom<S>> const&, int>());
        static_assert(!test<PMF1CVRR, S const&, int>());
        static_assert(!test<PMF1CVRR, S volatile&, int>());
        static_assert(!test<PMF1CVRR, S const volatile&, int>());
        static_assert(!test<PMF1CVRR, NotDerived&, int>());
        static_assert(!test<PMF1CVRR, NotDerived const&, int>());
        static_assert(!test<PMF1CVRR, std::unique_ptr<S const>, int>());
        static_assert(!test<PMF1CVRR, std::reference_wrapper<S const>, int>());
        static_assert(!test<PMF1CVRR, std::reference_wrapper<NotDerived>, int>());
        static_assert(!test<PMF1CVRR, std::unique_ptr<NotDerived>, int>());

        static_assert(test<PMF2CVRR, S, int, int>());
        static_assert(!test<PMF2CVRR, S&, int, int>());
        static_assert(!test<PMF2CVRR, S*, int, int>());
        static_assert(!test<PMF2CVRR, S*&, int, int>());
        static_assert(!test<PMF2CVRR, std::unique_ptr<S>, int, int>());
        static_assert(!test<PMF2CVRR, std::unique_ptr<DerivesFrom<S>>, int, int>());
        static_assert(!test<PMF2CVRR, std::reference_wrapper<S>, int, int>());
        static_assert(!test<PMF2CVRR, std::reference_wrapper<S> const&, int, int>());
        static_assert(!test<PMF2CVRR, std::reference_wrapper<DerivesFrom<S>>, int, int>());
        static_assert(!test<PMF2CVRR, std::reference_wrapper<DerivesFrom<S>> const&, int, int>());
        static_assert(!test<PMF2CVRR, S const&, int, int>());
        static_assert(!test<PMF2CVRR, S volatile&, int, int>());
        static_assert(!test<PMF2CVRR, S const volatile&, int, int>());
        static_assert(!test<PMF2CVRR, std::unique_ptr<S const>, int, int>());
        static_assert(!test<PMF2CVRR, std::reference_wrapper<S const>, int, int>());
        static_assert(!test<PMF2CVRR, NotDerived const&, int, int>());
        static_assert(!test<PMF2CVRR, std::reference_wrapper<NotDerived>, int, int>());
        static_assert(!test<PMF2CVRR, std::unique_ptr<NotDerived>, int, int>());

        static_assert(!test<PMF1PCVRR, S&, int>());
        static_assert(!test<PMF1PCVRR, S&, int, long>());
    }
}

#undef CALLCONV
#undef MCALLCONV
#undef NAME
