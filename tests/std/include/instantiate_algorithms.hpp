// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#pragma once
// This tests all STL algorithms, including <numeric> and <memory>.
// Notably, all permutations of all iterator strengths are tested.

#include <algorithm>
#include <cstddef>
#include <forward_list>
#include <functional>
#include <iterator>
#include <list>
#include <memory>
#include <numeric>
#include <type_traits>

#ifdef __cpp_lib_execution
#include <execution>
#endif // __cpp_lib_execution

// Compiling all algorithms takes too long for one test case.
// Therefore, when using this header, be sure to define INSTANTIATE_ALGORITHMS_SPLIT_MODE
// to indicate if this should compile part 1 or part 2.
#if !defined(INSTANTIATE_ALGORITHMS_SPLIT_MODE) || INSTANTIATE_ALGORITHMS_SPLIT_MODE < 1 \
    || INSTANTIATE_ALGORITHMS_SPLIT_MODE > 2
#error INSTANTIATE_ALGORITHMS_SPLIT_MODE must be defined to 1 or 2.
#endif

namespace std_testing {
    namespace dummy_functors {
        // Various functors required by various algorithms.
        // Note that the standard actually requires these to be copyable. As a "nonstandard extension" we want
        // to ensure we don't copy them, because copying some functors (e.g. std::function) is comparatively
        // expensive, and even for relatively cheap to copy function objects we care (somewhat) about debug
        // mode perf.

        struct Immobile {
            Immobile()                           = default;
            Immobile(const Immobile&)            = delete;
            Immobile& operator=(const Immobile&) = delete;
        };

        struct MoveOnly {
            MoveOnly()                           = default;
            MoveOnly(const MoveOnly&)            = delete;
            MoveOnly(MoveOnly&&)                 = default;
            MoveOnly& operator=(const MoveOnly&) = delete;
            MoveOnly& operator=(MoveOnly&&)      = delete;
        };

        template <typename T>
        struct Predicate : MoveOnly {
            bool operator()(const T&) const {
                return true;
            }
        };

        template <typename T>
        struct BiPredicate : MoveOnly {
            bool operator()(const T&, const T&) const {
                return true;
            }
        };

        template <typename T>
        struct UnaryOperation : MoveOnly {
            T operator()(const T&) const {
                return {};
            }
        };

        template <typename T>
        struct BinaryOperation : MoveOnly {
            T operator()(const T&, const T&) const {
                return {};
            }
        };

        template <typename T>
        struct Generator : MoveOnly {
            T operator()() const {
                return {};
            }
        };

        template <typename T>
        struct Function : MoveOnly {
            void operator()(const T&) {}
        };

        struct Rng : Immobile {
            template <typename Integral>
            Integral operator()(Integral) {
                return 0;
            }
        };

        struct Urng : Immobile {
            typedef unsigned int result_type;
            unsigned int operator()() {
                return 4; // chosen by fair dice roll; guaranteed to be random
            }
            static constexpr unsigned int min() {
                return 0;
            }
            static constexpr unsigned int max() {
                return 100;
            }
        };

        // Check for op,()
        template <typename T, typename U>
        void operator,(const T&, const U&) = delete;
    } // namespace dummy_functors


    template <typename ValueType, typename InIt, typename FwdIt, typename BidIt, typename RanIt, typename OutIt,
        typename PRED, typename BIPRED, typename UNARYOP, typename BINARYOP, typename GENERATOR, typename FUNCTION,
        typename RNG, typename URNG>
    struct algorithms_tester {

        // The convention here is that "UPPERCASE" variables are fixed types, while
        // "lowercase" variables are varying types.
        // For example, "FWDIT" is always a forward-only iterator, while "fwd1"
        // is a forward-or-better iterator.

        ValueType VAL;
        ValueType VALARR[10];

        ValueType (&ARRIT)[10] = VALARR;
        typedef decltype(ARRIT) ArrIt;

        typedef BIPRED COMP;

        InIt INIT;
        FwdIt FWDIT;
        BidIt BIDIT;
        RanIt RANIT;
        OutIt OUTIT;

        template <typename Iter>
        typename std::iterator_traits<Iter>::difference_type Get_size(Iter) {
            return {};
        }

#if INSTANTIATE_ALGORITHMS_SPLIT_MODE == 1
        std::list<ValueType> LST;
        std::forward_list<ValueType> FLST;
#endif // SPLIT_MODE

        algorithms_tester(InIt a, FwdIt b, BidIt c, RanIt d, OutIt e)
            : INIT(a), FWDIT(b), BIDIT(c), RANIT(d), OUTIT(e) {}

        // Now, test everything!
        template <typename In1, typename In2, typename Out>
        void test_in1_in2_out(In1 in1, In2 in2, Out out) {
#if INSTANTIATE_ALGORITHMS_SPLIT_MODE == 1
            std::transform(in1, in1, in2, out, BINARYOP{});
            std::merge(in1, in1, in2, in2, out);
            std::merge(in1, in1, in2, in2, out, COMP{});
            std::set_union(in1, in1, in2, in2, out);
            std::set_union(in1, in1, in2, in2, out, COMP{});
#else // ^^^ SPLIT_MODE 1 / SPLIT_MODE 2 vvv
            std::set_intersection(in1, in1, in2, in2, out);
            std::set_intersection(in1, in1, in2, in2, out, COMP{});
            std::set_difference(in1, in1, in2, in2, out);
            std::set_difference(in1, in1, in2, in2, out, COMP{});
            std::set_symmetric_difference(in1, in1, in2, in2, out);
            std::set_symmetric_difference(in1, in1, in2, in2, out, COMP{});
#endif // SPLIT_MODE
        }

        template <typename In1, typename In2>
        void test_in1_in2(In1 in1, In2 in2) {
            test_in1_in2_out(in1, in2, OUTIT);
            test_in1_in2_out(in1, in2, INIT);
            test_in1_in2_out(in1, in2, FWDIT);
            test_in1_in2_out(in1, in2, BIDIT);
            test_in1_in2_out(in1, in2, RANIT);
            test_in1_in2_out(in1, in2, ARRIT);

#if INSTANTIATE_ALGORITHMS_SPLIT_MODE == 2
            (void) std::mismatch(in1, in1, in2);
            (void) std::mismatch(in1, in1, in2, BIPRED{});
            (void) std::mismatch(in1, in1, in2, in2);
            (void) std::mismatch(in1, in1, in2, in2, BIPRED{});
            (void) std::equal(in1, in1, in2);
            (void) std::equal(in1, in1, in2, BIPRED{});
            (void) std::equal(in1, in1, in2, in2);
            (void) std::equal(in1, in1, in2, in2, BIPRED{});
            (void) std::includes(in1, in1, in2, in2);
            (void) std::includes(in1, in1, in2, in2, COMP{});
            (void) std::lexicographical_compare(in1, in1, in2, in2);
            (void) std::lexicographical_compare(in1, in1, in2, in2, COMP{});

            (void) std::inner_product(in1, in1, in2, VAL);
            (void) std::inner_product(in1, in1, in2, VAL, BINARYOP{}, BINARYOP{});
#if _HAS_CXX17
            (void) std::transform_reduce(in1, in1, in2, VAL);
            (void) std::transform_reduce(in1, in1, in2, VAL, BINARYOP{}, BINARYOP{});
#endif // _HAS_CXX17
#endif // SPLIT_MODE
        }

        template <typename In1, typename Fwd1>
        void test_in1_fwd1(In1 in1, Fwd1 fwd1) {
            // SPLIT_MODE 1
            (void) std::find_first_of(in1, in1, fwd1, fwd1);
            (void) std::find_first_of(in1, in1, fwd1, fwd1, BIPRED{});

            std::uninitialized_copy(in1, in1, fwd1);
            std::uninitialized_copy_n(in1, Get_size(in1), fwd1);
#if _HAS_CXX17
            std::uninitialized_move(in1, in1, fwd1);
            std::uninitialized_move_n(in1, Get_size(in1), fwd1);
#endif // _HAS_CXX17
        }

        template <typename In1, typename Out, typename Out2>
        void test_in1_out_out2(In1 in1, Out out, Out2 out2) {
            // SPLIT_MODE 1
            std::partition_copy(in1, in1, out, out2, PRED{});
        }

        template <typename In1, typename Out>
        void test_in1_out(In1 in1, Out out) {
            // SPLIT_MODE 1
            test_in1_out_out2(in1, out, OUTIT);
            test_in1_out_out2(in1, out, INIT);
            test_in1_out_out2(in1, out, FWDIT);
            test_in1_out_out2(in1, out, BIDIT);
            test_in1_out_out2(in1, out, RANIT);
            test_in1_out_out2(in1, out, ARRIT);

            std::copy(in1, in1, out);
            std::copy_n(in1, Get_size(in1), out);
            std::copy_if(in1, in1, out, PRED{});
            std::move(in1, in1, out);
            std::transform(in1, in1, out, UNARYOP{});
            std::replace_copy(in1, in1, out, VAL, VAL);
            std::replace_copy_if(in1, in1, out, PRED{}, VAL);
            std::remove_copy(in1, in1, out, VAL);
            std::remove_copy_if(in1, in1, out, PRED{});
            std::unique_copy(in1, in1, out);
            std::unique_copy(in1, in1, out, BIPRED{});

            std::partial_sum(in1, in1, out);
            std::partial_sum(in1, in1, out, BINARYOP{});
#if _HAS_CXX17
            std::exclusive_scan(in1, in1, out, VAL);
            std::exclusive_scan(in1, in1, out, VAL, BINARYOP{});
            std::inclusive_scan(in1, in1, out);
            std::inclusive_scan(in1, in1, out, BINARYOP{});
            std::inclusive_scan(in1, in1, out, BINARYOP{}, VAL);
            std::transform_exclusive_scan(in1, in1, out, VAL, BINARYOP{}, UNARYOP{});
            std::transform_inclusive_scan(in1, in1, out, BINARYOP{}, UNARYOP{});
            std::transform_inclusive_scan(in1, in1, out, BINARYOP{}, UNARYOP{}, VAL);
#endif // _HAS_CXX17
            std::adjacent_difference(in1, in1, out);
            std::adjacent_difference(in1, in1, out, BINARYOP{});
        }

        template <typename In1>
        void test_in1(In1 in1) {
            test_in1_in2(in1, INIT);
            test_in1_in2(in1, FWDIT);
            test_in1_in2(in1, BIDIT);
            test_in1_in2(in1, RANIT);
            test_in1_in2(in1, ARRIT);

#if INSTANTIATE_ALGORITHMS_SPLIT_MODE == 1
            test_in1_fwd1(in1, FWDIT);
            test_in1_fwd1(in1, BIDIT);
            test_in1_fwd1(in1, RANIT);
            test_in1_fwd1(in1, ARRIT);

            test_in1_out(in1, OUTIT);
            test_in1_out(in1, INIT);
            test_in1_out(in1, FWDIT);
            test_in1_out(in1, BIDIT);
            test_in1_out(in1, RANIT);
            test_in1_out(in1, ARRIT);

            LST.assign(in1, in1);
            LST.insert(LST.end(), in1, in1);

            FLST.assign(in1, in1);
            FLST.insert_after(FLST.begin(), in1, in1);

#else // ^^^ SPLIT_MODE 1 / SPLIT_MODE 2 vvv
            (void) std::all_of(in1, in1, PRED{});
            (void) std::any_of(in1, in1, PRED{});
            (void) std::none_of(in1, in1, PRED{});
            std::for_each(in1, in1, FUNCTION{});
#if _HAS_CXX17
            std::for_each_n(in1, Get_size(in1), FUNCTION{});
#endif // _HAS_CXX17
            (void) std::find(in1, in1, VAL);
            (void) std::find_if(in1, in1, PRED{});
            (void) std::find_if_not(in1, in1, PRED{});
            (void) std::count(in1, in1, VAL);
            (void) std::count_if(in1, in1, PRED{});
            (void) std::is_partitioned(in1, in1, PRED{});
            std::partial_sort_copy(in1, in1, RANIT, RANIT);
            std::partial_sort_copy(in1, in1, RANIT, RANIT, COMP{});
#if _HAS_CXX17
            std::sample(in1, in1, RANIT, Get_size(in1), URNG{});
            std::sample(in1, in1, ARRIT, Get_size(in1), URNG{});
#endif // _HAS_CXX17

            (void) std::accumulate(in1, in1, VAL);
            (void) std::accumulate(in1, in1, VAL, BINARYOP{});
#if _HAS_CXX17
            (void) std::reduce(in1, in1);
            (void) std::reduce(in1, in1, VAL);
            (void) std::reduce(in1, in1, VAL, BINARYOP{});
            (void) std::transform_reduce(in1, in1, VAL, BINARYOP{}, UNARYOP{});
#endif // _HAS_CXX17


#endif // SPLIT_MODE
        }

#ifdef __cpp_lib_execution
        template <typename Fwd1, typename Fwd2, typename Fwd3, typename ExecutionPolicy>
        void test_exec_fwd1_fwd2_fwd3(ExecutionPolicy&& exec, Fwd1 fwd1, Fwd2 fwd2, Fwd3 fwd3) {
            std::transform(std::forward<ExecutionPolicy>(exec), fwd1, fwd1, fwd2, fwd3, BINARYOP{});

            std::partition_copy(std::forward<ExecutionPolicy>(exec), fwd1, fwd1, fwd2, fwd3, PRED{});

            std::merge(std::forward<ExecutionPolicy>(exec), fwd1, fwd1, fwd2, fwd2, fwd3);
            std::merge(std::forward<ExecutionPolicy>(exec), fwd1, fwd1, fwd2, fwd2, fwd3, COMP{});

            std::set_union(std::forward<ExecutionPolicy>(exec), fwd1, fwd1, fwd2, fwd2, fwd3);
            std::set_union(std::forward<ExecutionPolicy>(exec), fwd1, fwd1, fwd2, fwd2, fwd3, COMP{});
            std::set_intersection(std::forward<ExecutionPolicy>(exec), fwd1, fwd1, fwd2, fwd2, fwd3);
            std::set_intersection(std::forward<ExecutionPolicy>(exec), fwd1, fwd1, fwd2, fwd2, fwd3, COMP{});
            std::set_difference(std::forward<ExecutionPolicy>(exec), fwd1, fwd1, fwd2, fwd2, fwd3);
            std::set_difference(std::forward<ExecutionPolicy>(exec), fwd1, fwd1, fwd2, fwd2, fwd3, COMP{});
            std::set_symmetric_difference(std::forward<ExecutionPolicy>(exec), fwd1, fwd1, fwd2, fwd2, fwd3);
            std::set_symmetric_difference(std::forward<ExecutionPolicy>(exec), fwd1, fwd1, fwd2, fwd2, fwd3, COMP{});
        }

        template <typename Fwd1, typename Fwd2, typename ExecutionPolicy>
        void test_exec_fwd1_fwd2(ExecutionPolicy&& exec, Fwd1 fwd1, Fwd2 fwd2) {
            test_exec_fwd1_fwd2_fwd3(std::forward<ExecutionPolicy>(exec), fwd1, fwd2, FWDIT);
            test_exec_fwd1_fwd2_fwd3(std::forward<ExecutionPolicy>(exec), fwd1, fwd2, BIDIT);
            test_exec_fwd1_fwd2_fwd3(std::forward<ExecutionPolicy>(exec), fwd1, fwd2, RANIT);
            test_exec_fwd1_fwd2_fwd3(std::forward<ExecutionPolicy>(exec), fwd1, fwd2, ARRIT);

            (void) std::find_end(std::forward<ExecutionPolicy>(exec), fwd1, fwd1, fwd2, fwd2);
            (void) std::find_end(std::forward<ExecutionPolicy>(exec), fwd1, fwd1, fwd2, fwd2, BIPRED{});
            (void) std::find_first_of(std::forward<ExecutionPolicy>(exec), fwd1, fwd1, fwd2, fwd2);
            (void) std::find_first_of(std::forward<ExecutionPolicy>(exec), fwd1, fwd1, fwd2, fwd2, BIPRED{});
            (void) std::mismatch(std::forward<ExecutionPolicy>(exec), fwd1, fwd1, fwd2);
            (void) std::mismatch(std::forward<ExecutionPolicy>(exec), fwd1, fwd1, fwd2, BIPRED{});
            (void) std::mismatch(std::forward<ExecutionPolicy>(exec), fwd1, fwd1, fwd2, fwd2);
            (void) std::mismatch(std::forward<ExecutionPolicy>(exec), fwd1, fwd1, fwd2, fwd2, BIPRED{});
            (void) std::equal(std::forward<ExecutionPolicy>(exec), fwd1, fwd1, fwd2);
            (void) std::equal(std::forward<ExecutionPolicy>(exec), fwd1, fwd1, fwd2, BIPRED{});
            (void) std::equal(std::forward<ExecutionPolicy>(exec), fwd1, fwd1, fwd2, fwd2);
            (void) std::equal(std::forward<ExecutionPolicy>(exec), fwd1, fwd1, fwd2, fwd2, BIPRED{});
            (void) std::search(std::forward<ExecutionPolicy>(exec), fwd1, fwd1, fwd2, fwd2);
            (void) std::search(std::forward<ExecutionPolicy>(exec), fwd1, fwd1, fwd2, fwd2, BIPRED{});

            std::copy(std::forward<ExecutionPolicy>(exec), fwd1, fwd1, fwd2);
            std::copy_if(std::forward<ExecutionPolicy>(exec), fwd1, fwd1, fwd2, PRED{});
            std::copy_n(std::forward<ExecutionPolicy>(exec), fwd1, Get_size(fwd1), fwd2);
            std::move(std::forward<ExecutionPolicy>(exec), fwd1, fwd1, fwd2);
            std::swap_ranges(std::forward<ExecutionPolicy>(exec), fwd1, fwd1, fwd2);
            std::transform(std::forward<ExecutionPolicy>(exec), fwd1, fwd1, fwd2, UNARYOP{});
            std::rotate_copy(std::forward<ExecutionPolicy>(exec), fwd1, fwd1, fwd1, fwd2);
            std::replace_copy(std::forward<ExecutionPolicy>(exec), fwd1, fwd1, fwd2, VAL, VAL);
            std::replace_copy_if(std::forward<ExecutionPolicy>(exec), fwd1, fwd1, fwd2, PRED{}, VAL);
            std::remove_copy(std::forward<ExecutionPolicy>(exec), fwd1, fwd1, fwd2, VAL);
            std::remove_copy_if(std::forward<ExecutionPolicy>(exec), fwd1, fwd1, fwd2, PRED{});
            std::unique_copy(std::forward<ExecutionPolicy>(exec), fwd1, fwd1, fwd2);
            std::unique_copy(std::forward<ExecutionPolicy>(exec), fwd1, fwd1, fwd2, BIPRED{});

            (void) std::includes(std::forward<ExecutionPolicy>(exec), fwd1, fwd1, fwd2, fwd2);
            (void) std::includes(std::forward<ExecutionPolicy>(exec), fwd1, fwd1, fwd2, fwd2, COMP{});

            (void) std::lexicographical_compare(std::forward<ExecutionPolicy>(exec), fwd1, fwd1, fwd2, fwd2);
            (void) std::lexicographical_compare(std::forward<ExecutionPolicy>(exec), fwd1, fwd1, fwd2, fwd2, COMP{});

            (void) std::transform_reduce(std::forward<ExecutionPolicy>(exec), fwd1, fwd1, fwd2, VAL);
            (void) std::transform_reduce(
                std::forward<ExecutionPolicy>(exec), fwd1, fwd1, fwd2, VAL, BINARYOP{}, BINARYOP{});
            std::exclusive_scan(std::forward<ExecutionPolicy>(exec), fwd1, fwd1, fwd2, VAL);
            std::exclusive_scan(std::forward<ExecutionPolicy>(exec), fwd1, fwd1, fwd2, VAL, BINARYOP{});
            std::inclusive_scan(std::forward<ExecutionPolicy>(exec), fwd1, fwd1, fwd2);
            std::inclusive_scan(std::forward<ExecutionPolicy>(exec), fwd1, fwd1, fwd2, BINARYOP{});
            std::inclusive_scan(std::forward<ExecutionPolicy>(exec), fwd1, fwd1, fwd2, BINARYOP{}, VAL);
            std::transform_exclusive_scan(
                std::forward<ExecutionPolicy>(exec), fwd1, fwd1, fwd2, VAL, BINARYOP{}, UNARYOP{});
            std::transform_inclusive_scan(std::forward<ExecutionPolicy>(exec), fwd1, fwd1, fwd2, BINARYOP{}, UNARYOP{});
            std::transform_inclusive_scan(
                std::forward<ExecutionPolicy>(exec), fwd1, fwd1, fwd2, BINARYOP{}, UNARYOP{}, VAL);
            std::adjacent_difference(std::forward<ExecutionPolicy>(exec), fwd1, fwd1, fwd2);
            std::adjacent_difference(std::forward<ExecutionPolicy>(exec), fwd1, fwd1, fwd2, BINARYOP{});
        }
#endif // __cpp_lib_execution

        template <typename Fwd1, typename Fwd2>
        void test_fwd1_fwd2(Fwd1 fwd1, Fwd2 fwd2) {
            // SPLIT_MODE 1
#ifdef __cpp_lib_execution
            test_exec_fwd1_fwd2(std::execution::seq, fwd1, fwd2);
            test_exec_fwd1_fwd2(std::execution::par, fwd1, fwd2);
            test_exec_fwd1_fwd2(std::execution::par_unseq, fwd1, fwd2);
#if _HAS_CXX20
            test_exec_fwd1_fwd2(std::execution::unseq, fwd1, fwd2);
#endif // _HAS_CXX20
#endif // __cpp_lib_execution

            (void) std::find_end(fwd1, fwd1, fwd2, fwd2);
            (void) std::find_end(fwd1, fwd1, fwd2, fwd2, BIPRED{});
            (void) std::is_permutation(fwd1, fwd1, fwd2);
            (void) std::is_permutation(fwd1, fwd1, fwd2, BIPRED{});
            (void) std::is_permutation(fwd1, fwd1, fwd2, fwd2);
            (void) std::is_permutation(fwd1, fwd1, fwd2, fwd2, BIPRED{});
            (void) std::search(fwd1, fwd1, fwd2, fwd2);
            (void) std::search(fwd1, fwd1, fwd2, fwd2, BIPRED{});
            std::swap_ranges(fwd1, fwd1, fwd2);
            std::iter_swap(fwd1, fwd2);
        }

        template <typename Fwd1, typename Out>
        void test_fwd1_out(Fwd1 fwd1, Out out) {
            // SPLIT_MODE 2
            std::rotate_copy(fwd1, fwd1, fwd1, out);
#if _HAS_CXX17
            std::sample(fwd1, fwd1, out, Get_size(fwd1), URNG{});
#endif // _HAS_CXX17
        }

#ifdef __cpp_lib_execution
        template <typename Fwd1, typename ExecutionPolicy>
        void test_exec_fwd1(ExecutionPolicy&& exec, Fwd1 fwd1) {
            (void) std::all_of(std::forward<ExecutionPolicy>(exec), fwd1, fwd1, PRED{});
            (void) std::any_of(std::forward<ExecutionPolicy>(exec), fwd1, fwd1, PRED{});
            (void) std::none_of(std::forward<ExecutionPolicy>(exec), fwd1, fwd1, PRED{});
            std::for_each(std::forward<ExecutionPolicy>(exec), fwd1, fwd1, FUNCTION{});
            std::for_each_n(std::forward<ExecutionPolicy>(exec), fwd1, Get_size(fwd1), FUNCTION{});
            (void) std::find(std::forward<ExecutionPolicy>(exec), fwd1, fwd1, VAL);
            (void) std::find_if(std::forward<ExecutionPolicy>(exec), fwd1, fwd1, PRED{});
            (void) std::find_if_not(std::forward<ExecutionPolicy>(exec), fwd1, fwd1, PRED{});
            (void) std::adjacent_find(std::forward<ExecutionPolicy>(exec), fwd1, fwd1);
            (void) std::adjacent_find(std::forward<ExecutionPolicy>(exec), fwd1, fwd1, BIPRED{});
            (void) std::count(std::forward<ExecutionPolicy>(exec), fwd1, fwd1, VAL);
            (void) std::count_if(std::forward<ExecutionPolicy>(exec), fwd1, fwd1, PRED{});
            (void) std::search_n(std::forward<ExecutionPolicy>(exec), fwd1, fwd1, Get_size(fwd1), VAL);
            (void) std::search_n(std::forward<ExecutionPolicy>(exec), fwd1, fwd1, Get_size(fwd1), VAL, BIPRED{});
            std::replace(std::forward<ExecutionPolicy>(exec), fwd1, fwd1, VAL, VAL);
            std::replace_if(std::forward<ExecutionPolicy>(exec), fwd1, fwd1, PRED{}, VAL);
            std::fill(std::forward<ExecutionPolicy>(exec), fwd1, fwd1, VAL);
            std::fill_n(std::forward<ExecutionPolicy>(exec), fwd1, Get_size(fwd1), VAL);
            std::generate(std::forward<ExecutionPolicy>(exec), fwd1, fwd1, GENERATOR{});
            std::generate_n(std::forward<ExecutionPolicy>(exec), fwd1, Get_size(fwd1), GENERATOR{});
            (void) std::remove(std::forward<ExecutionPolicy>(exec), fwd1, fwd1, VAL);
            (void) std::remove_if(std::forward<ExecutionPolicy>(exec), fwd1, fwd1, PRED{});
            (void) std::unique(std::forward<ExecutionPolicy>(exec), fwd1, fwd1);
            (void) std::unique(std::forward<ExecutionPolicy>(exec), fwd1, fwd1, COMP{});
            std::rotate(std::forward<ExecutionPolicy>(exec), fwd1, fwd1, fwd1);

            std::partial_sort_copy(std::forward<ExecutionPolicy>(exec), fwd1, fwd1, RANIT, RANIT);
            std::partial_sort_copy(std::forward<ExecutionPolicy>(exec), fwd1, fwd1, RANIT, RANIT, COMP{});
            (void) std::is_sorted(std::forward<ExecutionPolicy>(exec), fwd1, fwd1);
            (void) std::is_sorted(std::forward<ExecutionPolicy>(exec), fwd1, fwd1, COMP{});
            (void) std::is_sorted_until(std::forward<ExecutionPolicy>(exec), fwd1, fwd1);
            (void) std::is_sorted_until(std::forward<ExecutionPolicy>(exec), fwd1, fwd1, COMP{});

#if _HAS_CXX20
            std::shift_left(std::forward<ExecutionPolicy>(exec), fwd1, fwd1, 42);
            std::shift_right(std::forward<ExecutionPolicy>(exec), fwd1, fwd1, 42);
#endif // _HAS_CXX20

            std::partition(std::forward<ExecutionPolicy>(exec), fwd1, fwd1, PRED{});
            (void) std::is_partitioned(std::forward<ExecutionPolicy>(exec), fwd1, fwd1, PRED{});

            (void) std::min_element(std::forward<ExecutionPolicy>(exec), fwd1, fwd1);
            (void) std::min_element(std::forward<ExecutionPolicy>(exec), fwd1, fwd1, COMP{});
            (void) std::max_element(std::forward<ExecutionPolicy>(exec), fwd1, fwd1);
            (void) std::max_element(std::forward<ExecutionPolicy>(exec), fwd1, fwd1, COMP{});
            (void) std::minmax_element(std::forward<ExecutionPolicy>(exec), fwd1, fwd1);
            (void) std::minmax_element(std::forward<ExecutionPolicy>(exec), fwd1, fwd1, COMP{});

            (void) std::reduce(std::forward<ExecutionPolicy>(exec), fwd1, fwd1);
            (void) std::reduce(std::forward<ExecutionPolicy>(exec), fwd1, fwd1, VAL);
            (void) std::reduce(std::forward<ExecutionPolicy>(exec), fwd1, fwd1, VAL, BINARYOP{});
            (void) std::transform_reduce(std::forward<ExecutionPolicy>(exec), fwd1, fwd1, VAL, BINARYOP{}, UNARYOP{});
        }
#endif // __cpp_lib_execution

        template <typename Fwd1>
        void test_fwd1(Fwd1 fwd1) {
            // SPLIT_MODE 2
#ifdef __cpp_lib_execution
            test_exec_fwd1(std::execution::seq, fwd1);
            test_exec_fwd1(std::execution::par, fwd1);
            test_exec_fwd1(std::execution::par_unseq, fwd1);
#if _HAS_CXX20
            test_exec_fwd1(std::execution::unseq, fwd1);
#endif // _HAS_CXX20
#endif // __cpp_lib_execution

            test_fwd1_fwd2(fwd1, FWDIT);
            test_fwd1_fwd2(fwd1, BIDIT);
            test_fwd1_fwd2(fwd1, RANIT);
            test_fwd1_fwd2(fwd1, ARRIT);

            test_fwd1_out(fwd1, OUTIT);
            test_fwd1_out(fwd1, INIT);
            test_fwd1_out(fwd1, FWDIT);
            test_fwd1_out(fwd1, BIDIT);
            test_fwd1_out(fwd1, RANIT);
            test_fwd1_out(fwd1, ARRIT);

            (void) std::adjacent_find(fwd1, fwd1);
            (void) std::adjacent_find(fwd1, fwd1, BIPRED{});
            (void) std::search_n(fwd1, fwd1, Get_size(fwd1), VAL);
            (void) std::search_n(fwd1, fwd1, Get_size(fwd1), VAL, BIPRED{});
            std::replace(fwd1, fwd1, VAL, VAL);
            std::replace_if(fwd1, fwd1, PRED{}, VAL);
            std::fill(fwd1, fwd1, VAL);
            std::generate(fwd1, fwd1, GENERATOR{});
            (void) std::remove(fwd1, fwd1, VAL);
            (void) std::remove_if(fwd1, fwd1, PRED{});
            (void) std::unique(fwd1, fwd1);
            (void) std::unique(fwd1, fwd1, BIPRED{});
            std::rotate(fwd1, fwd1, fwd1);

#if _HAS_CXX20
            std::shift_left(fwd1, fwd1, 42);
            std::shift_right(fwd1, fwd1, 42);
#endif // _HAS_CXX20

            std::partition(fwd1, fwd1, PRED{});
            (void) std::partition_point(fwd1, fwd1, PRED{});
            (void) std::is_sorted(fwd1, fwd1);
            (void) std::is_sorted(fwd1, fwd1, COMP{});
            (void) std::is_sorted_until(fwd1, fwd1);
            (void) std::is_sorted_until(fwd1, fwd1, COMP{});
            (void) std::lower_bound(fwd1, fwd1, VAL);
            (void) std::lower_bound(fwd1, fwd1, VAL, COMP{});
            (void) std::upper_bound(fwd1, fwd1, VAL);
            (void) std::upper_bound(fwd1, fwd1, VAL, COMP{});
            (void) std::equal_range(fwd1, fwd1, VAL);
            (void) std::equal_range(fwd1, fwd1, VAL, COMP{});
            (void) std::binary_search(fwd1, fwd1, VAL);
            (void) std::binary_search(fwd1, fwd1, VAL, COMP{});
            (void) std::min_element(fwd1, fwd1);
            (void) std::min_element(fwd1, fwd1, COMP{});
            (void) std::max_element(fwd1, fwd1);
            (void) std::max_element(fwd1, fwd1, COMP{});
            (void) std::minmax_element(fwd1, fwd1);
            (void) std::minmax_element(fwd1, fwd1, COMP{});

            std::iota(fwd1, fwd1, VAL);

#if _HAS_CXX17
            std::uninitialized_default_construct(fwd1, fwd1);
            std::uninitialized_default_construct_n(fwd1, Get_size(fwd1));

            std::uninitialized_value_construct(fwd1, fwd1);
            std::uninitialized_value_construct_n(fwd1, Get_size(fwd1));
#endif // _HAS_CXX17

            std::uninitialized_fill(fwd1, fwd1, VAL);
            std::uninitialized_fill_n(fwd1, Get_size(fwd1), VAL);

#if _HAS_CXX17
            std::destroy(fwd1, fwd1);
            std::destroy_n(fwd1, Get_size(fwd1));
#endif // _HAS_CXX17
        }

        template <typename Bid1, typename Bid2>
        void test_bid1_bid2_xxx_backward(Bid1 bid1, Bid2 bid2) {
            // SPLIT_MODE 2
            std::copy_backward(bid1, bid1, bid2);
            std::move_backward(bid1, bid1, bid2);
        }

        template <typename Bid1, typename Out>
        void test_bid1_out(Bid1 bid1, Out out) {
            // SPLIT_MODE 2
            std::reverse_copy(bid1, bid1, out);
        }

#ifdef __cpp_lib_execution
        template <typename Bid1, typename Fwd1, typename ExecutionPolicy>
        void test_exec_bid1_fwd1(ExecutionPolicy&& exec, Bid1 bid1, Fwd1 fwd1) {
            std::reverse_copy(std::forward<ExecutionPolicy>(exec), bid1, bid1, fwd1);
        }

        template <typename Bid1, typename Fwd1>
        void test_bid1_fwd1(Bid1 bid1, Fwd1 fwd1) {
            test_exec_bid1_fwd1(std::execution::seq, bid1, fwd1);
            test_exec_bid1_fwd1(std::execution::par, bid1, fwd1);
            test_exec_bid1_fwd1(std::execution::par_unseq, bid1, fwd1);
#if _HAS_CXX20
            test_exec_bid1_fwd1(std::execution::unseq, bid1, fwd1);
#endif // _HAS_CXX20
        }

        template <typename Bid1, typename ExecutionPolicy>
        void test_exec_bid1(ExecutionPolicy&& exec, Bid1 bid1) {
            std::reverse(std::forward<ExecutionPolicy>(exec), bid1, bid1);
            // Currently the standard requires random-access iterators for stable_sort, but our implementation
            // works with bidirectional iterators and we don't want to regress this.
            std::stable_sort(std::forward<ExecutionPolicy>(exec), bid1, bid1);
            std::stable_sort(std::forward<ExecutionPolicy>(exec), bid1, bid1, COMP{});
            std::stable_partition(std::forward<ExecutionPolicy>(exec), bid1, bid1, PRED{});
            std::inplace_merge(std::forward<ExecutionPolicy>(exec), bid1, bid1, bid1);
            std::inplace_merge(std::forward<ExecutionPolicy>(exec), bid1, bid1, bid1, COMP{});
        }
#endif // __cpp_lib_execution

        template <typename Bid1>
        void test_bid1(Bid1 bid1) {
            // SPLIT_MODE 2
            test_bid1_bid2_xxx_backward(bid1, BIDIT);
            test_bid1_bid2_xxx_backward(bid1, RANIT);
            test_bid1_bid2_xxx_backward(bid1, ARRIT);

            test_bid1_out(bid1, OUTIT);
            test_bid1_out(bid1, INIT);
            test_bid1_out(bid1, FWDIT);
            test_bid1_out(bid1, BIDIT);
            test_bid1_out(bid1, RANIT);
            test_bid1_out(bid1, ARRIT);

#ifdef __cpp_lib_execution
            test_bid1_fwd1(bid1, FWDIT);
            test_bid1_fwd1(bid1, BIDIT);
            test_bid1_fwd1(bid1, RANIT);
            test_bid1_fwd1(bid1, ARRIT);

            test_exec_bid1(std::execution::seq, bid1);
            test_exec_bid1(std::execution::par, bid1);
            test_exec_bid1(std::execution::par_unseq, bid1);
#if _HAS_CXX20
            test_exec_bid1(std::execution::unseq, bid1);
#endif // _HAS_CXX20
#endif // __cpp_lib_execution

            std::reverse(bid1, bid1);
            // Currently the standard requires random-access iterators for stable_sort, but our implementation
            // works with bidirectional iterators and we don't want to regress this.
            std::stable_sort(bid1, bid1);
            std::stable_sort(bid1, bid1, COMP{});
            std::stable_partition(bid1, bid1, PRED{});
            std::inplace_merge(bid1, bid1, bid1);
            std::inplace_merge(bid1, bid1, bid1, COMP{});
            std::next_permutation(bid1, bid1);
            std::next_permutation(bid1, bid1, COMP{});
            std::prev_permutation(bid1, bid1);
            std::prev_permutation(bid1, bid1, COMP{});
        }


        template <typename Out>
        void test_out(Out out) {
            // SPLIT_MODE 1
            std::fill_n(out, 0, VAL);
            std::generate_n(out, 0, GENERATOR{});
        }

#ifdef __cpp_lib_execution
        template <typename Ran, typename ExecutionPolicy>
        void test_exec_ran(ExecutionPolicy&& exec, Ran ran) {
            std::sort(std::forward<ExecutionPolicy>(exec), ran, ran);
            std::sort(std::forward<ExecutionPolicy>(exec), ran, ran, COMP{});
            std::partial_sort(std::forward<ExecutionPolicy>(exec), ran, ran, ran);
            std::partial_sort(std::forward<ExecutionPolicy>(exec), ran, ran, ran, COMP{});
            std::nth_element(std::forward<ExecutionPolicy>(exec), ran, ran, ran);
            std::nth_element(std::forward<ExecutionPolicy>(exec), ran, ran, ran, COMP{});
            (void) std::is_heap(std::forward<ExecutionPolicy>(exec), ran, ran);
            (void) std::is_heap(std::forward<ExecutionPolicy>(exec), ran, ran, COMP{});
            (void) std::is_heap_until(std::forward<ExecutionPolicy>(exec), ran, ran);
            (void) std::is_heap_until(std::forward<ExecutionPolicy>(exec), ran, ran, COMP{});
        }
#endif // __cpp_lib_execution

        template <typename Ran>
        void test_ran(Ran ran) {
            // SPLIT_MODE 1
#ifdef __cpp_lib_execution
            test_exec_ran(std::execution::seq, ran);
            test_exec_ran(std::execution::par, ran);
            test_exec_ran(std::execution::par_unseq, ran);
#if _HAS_CXX20
            test_exec_ran(std::execution::unseq, ran);
#endif // _HAS_CXX20
#endif // __cpp_lib_execution

#if _HAS_AUTO_PTR_ETC
            std::random_shuffle(ran, ran);
            std::random_shuffle(ran, ran, RNG{});
#endif // _HAS_AUTO_PTR_ETC
            std::shuffle(ran, ran, URNG{});
            std::sort(ran, ran);
            std::sort(ran, ran, COMP{});
            std::partial_sort(ran, ran, ran);
            std::partial_sort(ran, ran, ran, COMP{});
            std::nth_element(ran, ran, ran);
            std::nth_element(ran, ran, ran, COMP{});
            std::push_heap(ran, ran);
            std::push_heap(ran, ran, COMP{});
            std::pop_heap(ran, ran);
            std::pop_heap(ran, ran, COMP{});
            std::make_heap(ran, ran);
            std::make_heap(ran, ran, COMP{});
            std::sort_heap(ran, ran);
            std::sort_heap(ran, ran, COMP{});
            (void) std::is_heap(ran, ran);
            (void) std::is_heap(ran, ran, COMP{});
            (void) std::is_heap_until(ran, ran);
            (void) std::is_heap_until(ran, ran, COMP{});
        }


        void test() {
            test_in1(INIT);
            test_in1(FWDIT);
            test_in1(BIDIT);
            test_in1(RANIT);
            test_in1(ARRIT);

#if INSTANTIATE_ALGORITHMS_SPLIT_MODE == 2
            test_fwd1(FWDIT);
            test_fwd1(BIDIT);
            test_fwd1(RANIT);
            test_fwd1(ARRIT);

            test_bid1(BIDIT);
            test_bid1(RANIT);
            test_bid1(ARRIT);
#endif // SPLIT_MODE

#if INSTANTIATE_ALGORITHMS_SPLIT_MODE == 1
            test_out(OUTIT);
            test_out(INIT);
            test_out(FWDIT);
            test_out(BIDIT);
            test_out(RANIT);
            test_out(ARRIT);

            test_ran(RANIT);
            test_ran(ARRIT);

            LST.merge(LST);
            LST.merge(std::move(LST));
            LST.merge(LST, COMP{});
            LST.merge(std::move(LST), COMP{});
            LST.remove(VAL);
            LST.remove_if(PRED{});
            LST.reverse();
            LST.sort();
            LST.sort(COMP{});
            LST.splice(LST.end(), LST);
            LST.splice(LST.end(), std::move(LST));
            LST.splice(LST.end(), LST, LST.begin());
            LST.splice(LST.end(), std::move(LST), LST.begin());
            LST.splice(LST.end(), LST, LST.begin(), LST.end());
            LST.splice(LST.end(), std::move(LST), LST.begin(), LST.end());
            LST.unique();
            LST.unique(COMP{});
            (void) (LST == LST);

            FLST.merge(FLST);
            FLST.merge(std::move(FLST));
            FLST.merge(FLST, COMP{});
            FLST.merge(std::move(FLST), COMP{});
            FLST.remove(VAL);
            FLST.remove_if(PRED{});
            FLST.reverse();
            FLST.sort();
            FLST.sort(COMP{});
            FLST.splice_after(FLST.end(), FLST);
            FLST.splice_after(FLST.end(), std::move(FLST));
            FLST.splice_after(FLST.end(), FLST, FLST.begin());
            FLST.splice_after(FLST.end(), std::move(FLST), FLST.begin());
            FLST.splice_after(FLST.end(), FLST, FLST.begin(), FLST.end());
            FLST.splice_after(FLST.end(), std::move(FLST), FLST.begin(), FLST.end());
            FLST.unique();
            FLST.unique(COMP{});
            (void) (FLST == FLST);

            (void) std::min(VAL, VAL);
            (void) std::min(VAL, VAL, COMP{});
            (void) std::min({VAL, VAL, VAL});
            (void) std::min({VAL, VAL, VAL}, COMP{});
            (void) std::max(VAL, VAL);
            (void) std::max(VAL, VAL, COMP{});
            (void) std::max({VAL, VAL, VAL});
            (void) std::max({VAL, VAL, VAL}, COMP{});
#if _HAS_CXX17
            (void) std::clamp(VAL, VAL, VAL);
            (void) std::clamp(VAL, VAL, VAL, COMP{});
#endif // _HAS_CXX17
            (void) std::minmax(VAL, VAL);
            (void) std::minmax(VAL, VAL, COMP{});
            (void) std::minmax({VAL, VAL, VAL});
            (void) std::minmax({VAL, VAL, VAL}, COMP{});
#endif // SPLIT_MODE
        }
    };

    template <typename ValueType, typename InIt, typename FwdIt, typename BidIt, typename RanIt, typename OutIt,
        typename PRED = dummy_functors::Predicate<ValueType>, typename BIPRED = dummy_functors::BiPredicate<ValueType>,
        typename UNARYOP   = dummy_functors::UnaryOperation<ValueType>,
        typename BINARYOP  = dummy_functors::BinaryOperation<ValueType>,
        typename GENERATOR = dummy_functors::Generator<ValueType>,
        typename FUNCTION = dummy_functors::Function<ValueType>, typename RNG = dummy_functors::Rng,
        typename URNG = dummy_functors::Urng>
    void instantiate_std_algorithms_with(InIt a, FwdIt b, BidIt c, RanIt d, OutIt e) {
        algorithms_tester<ValueType, InIt, FwdIt, BidIt, RanIt, OutIt, PRED, BIPRED, UNARYOP, BINARYOP, GENERATOR,
            FUNCTION, RNG, URNG>
            test_algorithms{a, b, c, d, e};
        test_algorithms.test();
    }
} // namespace std_testing
