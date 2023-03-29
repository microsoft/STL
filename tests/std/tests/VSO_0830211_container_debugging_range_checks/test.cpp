// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#define _CONTAINER_DEBUG_LEVEL 1

#include <array>
#include <cstddef>
#include <deque>
#include <string>
#include <vector>

#include <test_death.hpp>

using namespace std;

template <typename Traits>
struct TestCases {
    using ContainerType     = typename Traits::ContainerType;
    using IteratorType      = typename Traits::IteratorType; // ContainerType::(const_)?iterator
    using ZeroContainerType = typename Traits::ZeroContainerType;

    using MutableIteratorType = typename ContainerType::iterator; // never const_iterator

    static void test_case_operator_dereference_value_initialized_iterator() {
        IteratorType it; // note: for IDL to work correctly, default-init and value-init are equivalent
        (void) *it;
    }

    static void test_case_operator_arrow_value_initialized_iterator2(true_type) {
        IteratorType it;
        (void) it.operator->();
    }

    static void test_case_operator_arrow_value_initialized_iterator2(false_type) {}

    static void test_case_operator_arrow_value_initialized_iterator() {
        return test_case_operator_arrow_value_initialized_iterator2(bool_constant<Traits::has_arrow>{});
    }

    static void test_case_operator_preincrement_value_initialized_iterator() {
        IteratorType it;
        ++it;
    }

    static void test_case_operator_predecrement_value_initialized_iterator() {
        IteratorType it;
        --it;
    }

    static void test_case_operator_advance_value_initialized_iterator() {
        IteratorType it;
        it += 1;
    }

    static void test_case_operator_advance_value_initialized_iterator_zero() {
        IteratorType it;
        it += 0; // OK
    }

    static void test_case_operator_retreat_value_initialized_iterator() {
        IteratorType it;
        it -= 1;
    }

    static void test_case_operator_retreat_value_initialized_iterator_zero() {
        IteratorType it;
        it -= 0; // OK
    }

    static void test_case_operator_dereference_end_iterator() {
        ContainerType a{false, true, false, true};
        auto it = a.end();
        (void) *it;
    }

    static void test_case_operator_arrow_end_iterator2(true_type) {
        ContainerType a{false, true, false, true};
        auto it = a.end();
        (void) it.operator->();
    }

    static void test_case_operator_arrow_end_iterator2(false_type) {}

    static void test_case_operator_arrow_end_iterator() {
        return test_case_operator_arrow_end_iterator2(bool_constant<Traits::has_arrow>{});
    }

    static void test_case_operator_preincrement_off_end() {
        ContainerType a{false, true, false, true};
        auto it = a.begin();
        for (size_t idx = 0; idx < 5; ++idx) {
            ++it;
        }
    }

    static void test_case_operator_predecrement_before_begin() {
        ContainerType a{false, true, false, true};
        auto it = a.begin();
        --it;
    }

    static void test_case_operator_advance_before_begin() {
        ContainerType a{false, true, false, true};
        auto it = a.begin();
        it += -1;
    }

    static void test_case_operator_advance_after_end() {
        ContainerType a{false, true, false, true};
        auto it = a.begin();
        it += 5;
    }

    static void test_case_operator_retreat_before_begin() {
        ContainerType a{false, true, false, true};
        auto it = a.begin();
        it -= 1;
    }

    static void test_case_operator_retreat_after_end() {
        ContainerType a{false, true, false, true};
        auto it = a.begin();
        it -= -5;
    }

    static void test_case_operator_subtract_incompatible_different_views() {
        ContainerType a{false, true, false, true};
        ContainerType b{false, true, false, true};
        (void) (a.begin() - b.begin());
    }

    static void test_case_operator_subtract_incompatible_value_initialized() {
        ContainerType a{false, true, false, true};
        (void) (a.begin() - MutableIteratorType{});
    }

    static void test_case_operator_equal_incompatible_different_views() {
        ContainerType a{false, true, false, true};
        ContainerType b{false, true, false, true};
        (void) (a.begin() == b.begin());
    }

    static void test_case_operator_equal_incompatible_value_initialized() {
        ContainerType a{false, true, false, true};
        (void) (a.begin() == MutableIteratorType{});
    }

    static void test_case_operator_less_incompatible_different_views() {
        ContainerType a{false, true, false, true};
        ContainerType b{false, true, false, true};
        (void) (a.begin() < b.begin());
    }

    static void test_case_operator_less_incompatible_value_initialized() {
        ContainerType a{false, true, false, true};
        (void) (a.begin() < MutableIteratorType{});
    }

    static void test_case_operator_subscript_out_of_range_empty() {
        ZeroContainerType a{};
        (void) a[Traits::zero_offset];
    }

    static void test_case_operator_subscript_out_of_range() {
        ContainerType a{false, true, false, true};
#pragma warning(suppress : 28020) // Yay sometimes PREfast catches this one at compile time!
        (void) a[Traits::zero_offset + 4];
    }

    static void test_case_front_empty() {
        ZeroContainerType a{};
        (void) a.front();
    }

    static void test_case_back_empty() {
        ZeroContainerType a{};
        (void) a.back();
    }

    static void negative_cases() {
        test_case_operator_advance_value_initialized_iterator_zero();
        test_case_operator_retreat_value_initialized_iterator_zero();
    }

    static void add_cases(std_testing::death_test_executive& exec) {
#if _ITERATOR_DEBUG_LEVEL != 0
        static constexpr std_testing::death_function_t a[] = {
            // TRANSITION, VSO-847348
            test_case_operator_dereference_value_initialized_iterator,
            test_case_operator_preincrement_value_initialized_iterator,
            test_case_operator_predecrement_value_initialized_iterator,
            test_case_operator_advance_value_initialized_iterator,
            test_case_operator_retreat_value_initialized_iterator,
            test_case_operator_dereference_end_iterator,
            test_case_operator_preincrement_off_end,
            test_case_operator_predecrement_before_begin,
            test_case_operator_advance_before_begin,
            test_case_operator_advance_after_end,
            test_case_operator_retreat_before_begin,
            test_case_operator_retreat_after_end,
            test_case_operator_subtract_incompatible_different_views,
            test_case_operator_subtract_incompatible_value_initialized,
            test_case_operator_equal_incompatible_different_views,
            test_case_operator_equal_incompatible_value_initialized,
            test_case_operator_less_incompatible_different_views,
            test_case_operator_less_incompatible_value_initialized,
        };
        exec.add_death_tests(a);

        if (Traits::has_arrow) {
            static constexpr std_testing::death_function_t b[] = {
                // TRANSITION, VSO-847348
                test_case_operator_arrow_value_initialized_iterator,
                test_case_operator_arrow_end_iterator,
            };
            exec.add_death_tests(b);
        }
#endif // _ITERATOR_DEBUG_LEVEL != 0

        static constexpr std_testing::death_function_t c[] = {
            // TRANSITION, VSO-847348
            test_case_operator_subscript_out_of_range_empty,
            test_case_operator_subscript_out_of_range,
            test_case_front_empty,
            test_case_back_empty,
        };
        exec.add_death_tests(c);
    }
};


struct ArrayTestCaseTraits {
    using ContainerType     = array<int, 4>;
    using IteratorType      = array<int, 4>::iterator;
    using ZeroContainerType = array<int, 0>;

    static constexpr size_t zero_offset = 0;
    static constexpr bool has_arrow     = true;
};

struct ConstArrayTestCaseTraits {
    using ContainerType     = const array<int, 4>;
    using IteratorType      = array<int, 4>::const_iterator;
    using ZeroContainerType = const array<int, 0>;

    static constexpr size_t zero_offset = 0;
    static constexpr bool has_arrow     = true;
};

struct VectorTestCaseTraits {
    using ContainerType     = vector<int>;
    using IteratorType      = vector<int>::iterator;
    using ZeroContainerType = ContainerType;

    static constexpr size_t zero_offset = 0;
    static constexpr bool has_arrow     = true;
};

struct ConstVectorTestCaseTraits {
    using ContainerType     = const vector<int>;
    using IteratorType      = vector<int>::const_iterator;
    using ZeroContainerType = ContainerType;

    static constexpr size_t zero_offset = 0;
    static constexpr bool has_arrow     = true;
};

struct VectorBoolTestCaseTraits {
    using ContainerType     = vector<bool>;
    using IteratorType      = vector<bool>::iterator;
    using ZeroContainerType = ContainerType;

    static constexpr size_t zero_offset = 0;
    static constexpr bool has_arrow     = false;
};

struct ConstVectorBoolTestCaseTraits {
    using ContainerType     = const vector<bool>;
    using IteratorType      = vector<bool>::const_iterator;
    using ZeroContainerType = ContainerType;

    static constexpr size_t zero_offset = 0;
    static constexpr bool has_arrow     = false;
};

struct DequeTestCaseTraits {
    using ContainerType     = deque<int>;
    using IteratorType      = deque<int>::iterator;
    using ZeroContainerType = ContainerType;

    static constexpr size_t zero_offset = 0;
    static constexpr bool has_arrow     = true;
};

struct ConstDequeTestCaseTraits {
    using ContainerType     = const deque<int>;
    using IteratorType      = deque<int>::const_iterator;
    using ZeroContainerType = ContainerType;

    static constexpr size_t zero_offset = 0;
    static constexpr bool has_arrow     = true;
};

struct StringTestCaseTraits {
    using ContainerType     = string;
    using IteratorType      = string::iterator;
    using ZeroContainerType = ContainerType;

    static constexpr size_t zero_offset = 1;
    static constexpr bool has_arrow     = true;
};

struct ConstStringTestCaseTraits {
    using ContainerType     = const string;
    using IteratorType      = string::const_iterator;
    using ZeroContainerType = ContainerType;

    static constexpr size_t zero_offset = 1;
    static constexpr bool has_arrow     = true;
};


int main(int argc, char* argv[]) {
    std_testing::death_test_executive exec([] {
        TestCases<ArrayTestCaseTraits>::negative_cases();
        TestCases<ConstArrayTestCaseTraits>::negative_cases();
        TestCases<VectorTestCaseTraits>::negative_cases();
        TestCases<ConstVectorTestCaseTraits>::negative_cases();
        TestCases<VectorBoolTestCaseTraits>::negative_cases();
        TestCases<ConstVectorBoolTestCaseTraits>::negative_cases();
        TestCases<DequeTestCaseTraits>::negative_cases();
        TestCases<ConstDequeTestCaseTraits>::negative_cases();
        TestCases<StringTestCaseTraits>::negative_cases();
        TestCases<ConstStringTestCaseTraits>::negative_cases();
    });

    TestCases<ArrayTestCaseTraits>::add_cases(exec);
    TestCases<ConstArrayTestCaseTraits>::add_cases(exec);
    TestCases<VectorTestCaseTraits>::add_cases(exec);
    TestCases<ConstVectorTestCaseTraits>::add_cases(exec);
    TestCases<VectorBoolTestCaseTraits>::add_cases(exec);
    TestCases<ConstVectorBoolTestCaseTraits>::add_cases(exec);
    TestCases<DequeTestCaseTraits>::add_cases(exec);
    TestCases<ConstDequeTestCaseTraits>::add_cases(exec);
    TestCases<StringTestCaseTraits>::add_cases(exec);
    TestCases<ConstStringTestCaseTraits>::add_cases(exec);
    return exec.run(argc, argv);
}
