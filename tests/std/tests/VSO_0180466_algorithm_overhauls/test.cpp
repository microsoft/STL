// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#pragma warning(disable : 4018) // signed/unsigned mismatch in comparison
#pragma warning(disable : 4365) // signed/unsigned mismatch in implicit conversion
#pragma warning(disable : 4389) // signed/unsigned mismatch in arithmetic

#ifdef __clang__
#pragma clang diagnostic ignored "-Wsign-compare"
#endif // __clang__

#include <algorithm>
#include <array>
#include <cassert>
#include <functional>
#include <iterator>
#include <string>

using namespace std;

struct move_only {
    int val;

    move_only(int val_) : val(val_) {}
    move_only(const move_only&)            = delete;
    move_only(move_only&&)                 = default;
    move_only& operator=(const move_only&) = delete;
    move_only& operator=(move_only&&)      = default;
};

bool operator==(const move_only& lhs, const move_only& rhs) {
    return lhs.val == rhs.val;
}

template <template <class, class> class Test, class Left>
void test_all_numeric_types_impl() {
    Test<Left, char>{}();
    Test<Left, signed char>{}();
    Test<Left, unsigned char>{}();
    Test<Left, wchar_t>{}();
#ifdef __cpp_char8_t
    Test<Left, char8_t>{}();
#endif // __cpp_char8_t
    Test<Left, char16_t>{}();
    Test<Left, char32_t>{}();
    Test<Left, short>{}();
    Test<Left, unsigned short>{}();
    Test<Left, int>{}();
    Test<Left, unsigned int>{}();
    Test<Left, long>{}();
    Test<Left, unsigned long>{}();
    Test<Left, long long>{}();
    Test<Left, unsigned long long>{}();
    Test<Left, float>{}();
    Test<Left, double>{}();
    Test<Left, long double>{}();
}

template <template <class, class> class Test>
void test_all_numeric_types() {
    test_all_numeric_types_impl<Test, char>();
    test_all_numeric_types_impl<Test, signed char>();
    test_all_numeric_types_impl<Test, unsigned char>();
    test_all_numeric_types_impl<Test, wchar_t>();
#ifdef __cpp_char8_t
    test_all_numeric_types_impl<Test, char8_t>();
#endif // __cpp_char8_t
    test_all_numeric_types_impl<Test, char16_t>();
    test_all_numeric_types_impl<Test, char32_t>();
    test_all_numeric_types_impl<Test, short>();
    test_all_numeric_types_impl<Test, unsigned short>();
    test_all_numeric_types_impl<Test, int>();
    test_all_numeric_types_impl<Test, unsigned int>();
    test_all_numeric_types_impl<Test, long>();
    test_all_numeric_types_impl<Test, unsigned long>();
    test_all_numeric_types_impl<Test, long long>();
    test_all_numeric_types_impl<Test, unsigned long long>();
    test_all_numeric_types_impl<Test, float>();
    test_all_numeric_types_impl<Test, double>();
    test_all_numeric_types_impl<Test, long double>();
#ifdef __cpp_lib_byte
    Test<byte, byte>{}();
#endif // __cpp_lib_byte
}

namespace test_std_equal {
    struct test_strings {
        string a = "a";
        string b = "b";
        string c = "c";
    };

    void equal_should_respect_pred() {
        const test_strings str1{};
        const test_strings str2{};
        const char* const equal_chars1[] = {str1.a.c_str(), str1.b.c_str(), str1.c.c_str()};

        const char* const equal_chars2[] = {str2.a.c_str(), str2.b.c_str(), str2.c.c_str()};

        // supplying a custom equal_to<string> compares the string contents, so the chars are equal
        assert(
            equal(begin(equal_chars1), end(equal_chars1), begin(equal_chars2), end(equal_chars2), equal_to<string>{}));
        assert(equal(begin(equal_chars1), end(equal_chars1), equal_chars2, equal_to<string>{}));
        // but other comparisons compare the pointer addresses so they aren't equal
        assert(!equal(begin(equal_chars1), end(equal_chars1), equal_chars2));
        assert(!equal(begin(equal_chars1), end(equal_chars1), begin(equal_chars2), end(equal_chars2)));
        assert(!equal(begin(equal_chars1), end(equal_chars1), begin(equal_chars2), end(equal_chars2), equal_to<>{}));
        assert(!equal(
            begin(equal_chars1), end(equal_chars1), begin(equal_chars2), end(equal_chars2), equal_to<const char*>{}));
        assert(!equal(
            begin(equal_chars1), end(equal_chars1), begin(equal_chars2), end(equal_chars2), equal_to<const void*>{}));
        assert(!equal(begin(equal_chars1), end(equal_chars1), begin(equal_chars2), end(equal_chars2), less<string>{}));
    }

    template <class Left, class Right>
    struct test_equal_should_return_correct_numeric_result {
        void operator()() const {
            Left eq_lhs[]  = {static_cast<Left>(1), static_cast<Left>(2), static_cast<Left>(3)};
            Left lt_lhs[]  = {static_cast<Left>(0), static_cast<Left>(1), static_cast<Left>(2)};
            Right eq_rhs[] = {static_cast<Right>(1), static_cast<Right>(2), static_cast<Right>(3)};

            // plain equal and not equal cases
            assert(equal(eq_lhs, end(eq_lhs), eq_rhs, equal_to<>{}));
            assert(!equal(lt_lhs, end(lt_lhs), eq_rhs, equal_to<>{}));

            // unorthodox builtin comparison function
            assert(equal(lt_lhs, end(lt_lhs), eq_rhs, less<>{}));

            // ensure usual arithmetic conversions are handled correctly
            eq_lhs[0] = static_cast<Left>(-1);
            eq_rhs[0] = static_cast<Right>(-1);
            assert(equal(eq_lhs, end(eq_lhs), eq_rhs) == (eq_lhs[0] == eq_rhs[0]));
        }
    };

    void test() {
        equal_should_respect_pred();
        test_all_numeric_types<test_equal_should_return_correct_numeric_result>();
    }
} // namespace test_std_equal

namespace test_lexicographical_compare {
    // examples in sort order
    struct unsigned_data {
        static const unsigned char fluff[];
        static const unsigned char fluffy[];
        static const unsigned char kitten[];
        static const unsigned char neg[];
    };

    // note use of {'x'} syntax instead of "x" to avoid null terminators
    const unsigned char unsigned_data::fluff[]  = {'f', 'l', 'u', 'f', 'f'}; // prefix of the following
    const unsigned char unsigned_data::fluffy[] = {'f', 'l', 'u', 'f', 'f', 'y'};
    const unsigned char unsigned_data::kitten[] = {'k', 'i', 't', 't', 'e', 'n'};
    const unsigned char unsigned_data::neg[]    = {static_cast<unsigned char>('\xB5')}; // mu character

    struct signed_data {
        static const signed char neg[];
        static const signed char fluff[];
        static const signed char fluffy[];
        static const signed char kitten[];
    };

    const signed char signed_data::neg[]    = {static_cast<signed char>('\xB5')};
    const signed char signed_data::fluff[]  = {'f', 'l', 'u', 'f', 'f'};
    const signed char signed_data::fluffy[] = {'f', 'l', 'u', 'f', 'f', 'y'};
    const signed char signed_data::kitten[] = {'k', 'i', 't', 't', 'e', 'n'};

    struct char_data {
        static const char neg[];
        static const char fluff[];
        static const char fluffy[];
        static const char kitten[];
    };

    const char char_data::neg[]    = {static_cast<char>('\xB5')};
    const char char_data::fluff[]  = {'f', 'l', 'u', 'f', 'f'};
    const char char_data::fluffy[] = {'f', 'l', 'u', 'f', 'f', 'y'};
    const char char_data::kitten[] = {'k', 'i', 't', 't', 'e', 'n'};

#ifdef __cpp_char8_t
    struct char8_data {
        static const char8_t neg[];
        static const char8_t fluff[];
        static const char8_t fluffy[];
        static const char8_t kitten[];
    };

    const char8_t char8_data::neg[]    = {static_cast<char8_t>('\xB5')};
    const char8_t char8_data::fluff[]  = {u8'f', u8'l', u8'u', u8'f', u8'f'};
    const char8_t char8_data::fluffy[] = {u8'f', u8'l', u8'u', u8'f', u8'f', u8'y'};
    const char8_t char8_data::kitten[] = {u8'k', u8'i', u8't', u8't', u8'e', u8'n'};
#endif // __cpp_char8_t

    template <class Data>
    void test_case_non_pred(const bool should_treat_as_signed) {
        // in ascending order
        assert(lexicographical_compare(begin(Data::fluff), end(Data::fluff), begin(Data::fluffy), end(Data::fluffy)));
        assert(lexicographical_compare(begin(Data::fluffy), end(Data::fluffy), begin(Data::kitten), end(Data::kitten)));
        assert(lexicographical_compare(begin(Data::kitten), end(Data::kitten), begin(Data::neg), end(Data::neg))
               != should_treat_as_signed);
        // descending order
        assert(lexicographical_compare(begin(Data::neg), end(Data::neg), begin(Data::kitten), end(Data::kitten))
               == should_treat_as_signed);
        assert(!lexicographical_compare(begin(Data::fluffy), end(Data::fluffy), begin(Data::fluff), end(Data::fluff)));
        assert(
            !lexicographical_compare(begin(Data::kitten), end(Data::kitten), begin(Data::fluffy), end(Data::fluffy)));
        // equal
        assert(
            !lexicographical_compare(begin(Data::kitten), end(Data::kitten), begin(Data::kitten), end(Data::kitten)));
    }

    template <class Data, class Pred>
    void test_case_pred(const bool should_treat_as_signed, const Pred pred) {
        // differ only in length
        assert(lexicographical_compare(
            begin(Data::fluff), end(Data::fluff), begin(Data::fluffy), end(Data::fluffy), pred));
        assert(!lexicographical_compare(
            begin(Data::fluffy), end(Data::fluffy), begin(Data::fluff), end(Data::fluff), pred));

        // differ in content
        const bool is_less = pred('a', 'b');
        assert(lexicographical_compare(
                   begin(Data::fluffy), end(Data::fluffy), begin(Data::kitten), end(Data::kitten), pred)
               == is_less);
        assert(lexicographical_compare(begin(Data::kitten), end(Data::kitten), begin(Data::neg), end(Data::neg), pred)
               == (is_less != should_treat_as_signed));

        assert(lexicographical_compare(begin(Data::neg), end(Data::neg), begin(Data::kitten), end(Data::kitten), pred)
               == (is_less == should_treat_as_signed));
        assert(lexicographical_compare(
                   begin(Data::kitten), end(Data::kitten), begin(Data::fluffy), end(Data::fluffy), pred)
               == !is_less);

        // equal
        assert(!lexicographical_compare(
            begin(Data::kitten), end(Data::kitten), begin(Data::kitten), end(Data::kitten), pred));
    }

    void test_changing_pred_does_not_change_length_behavior() {
        const unsigned char prefix[] = {'a', 'b', 'c'};
        const unsigned char full[]   = {'a', 'b', 'c', 'd'};
        // basic tests
        assert(lexicographical_compare(begin(prefix), end(prefix), begin(full), end(full), less<>{}));
        assert(!lexicographical_compare(begin(full), end(full), begin(prefix), end(prefix), less<>{}));
        // greater<>{} changes element-wise comparisons, but doesn't change length-wise comparisons
        assert(lexicographical_compare(begin(prefix), end(prefix), begin(full), end(full), greater<>{}));
        assert(!lexicographical_compare(begin(full), end(full), begin(prefix), end(prefix), greater<>{}));
    }

    template <class Left, class Right>
    struct test_lex_compare_should_return_correct_numeric_result {
        void operator()() const {
            const Left eq_lhs[]  = {static_cast<Left>(1), static_cast<Left>(2), static_cast<Left>(3)};
            const Left lt_lhs[]  = {static_cast<Left>(0), static_cast<Left>(1), static_cast<Left>(2)};
            const Left gt_lhs[]  = {static_cast<Left>(2), static_cast<Left>(3), static_cast<Left>(4)};
            const Right eq_rhs[] = {static_cast<Right>(1), static_cast<Right>(2), static_cast<Right>(3)};

            // plain equal and not equal cases
            assert(!lexicographical_compare(eq_lhs, end(eq_lhs), eq_rhs, end(eq_rhs)));
            assert(lexicographical_compare(lt_lhs, end(lt_lhs), eq_rhs, end(eq_rhs)));

            assert(!lexicographical_compare(eq_lhs, end(eq_lhs), eq_rhs, end(eq_rhs), less<>{}));
            assert(lexicographical_compare(lt_lhs, end(lt_lhs), eq_rhs, end(eq_rhs), less<>{}));

            assert(!lexicographical_compare(eq_lhs, end(eq_lhs), eq_rhs, end(eq_rhs), greater<>{}));
            assert(!lexicographical_compare(lt_lhs, end(lt_lhs), eq_rhs, end(eq_rhs), greater<>{}));
            assert(lexicographical_compare(gt_lhs, end(gt_lhs), eq_rhs, end(eq_rhs), greater<>{}));
        }
    };

    void test() {
        const bool char_is_signed = is_signed_v<char>;
        test_case_non_pred<unsigned_data>(false);
        test_case_non_pred<signed_data>(true);
        test_case_non_pred<char_data>(char_is_signed);
#ifdef __cpp_char8_t
        test_case_non_pred<char8_data>(false);
#endif // __cpp_char8_t

        test_case_pred<unsigned_data>(false, less<>{});
        test_case_pred<unsigned_data>(false, less<unsigned char>{});
        test_case_pred<unsigned_data>(true, less<signed char>{});
        test_case_pred<unsigned_data>(char_is_signed, less<char>{});
#ifdef __cpp_char8_t
        test_case_pred<unsigned_data>(false, less<char8_t>{});
#endif // __cpp_char8_t
        test_case_pred<unsigned_data>(false, greater<>{});
        test_case_pred<unsigned_data>(false, greater<unsigned char>{});
        test_case_pred<unsigned_data>(true, greater<signed char>{});
        test_case_pred<unsigned_data>(char_is_signed, greater<char>{});
#ifdef __cpp_char8_t
        test_case_pred<unsigned_data>(false, greater<char8_t>{});
#endif // __cpp_char8_t

        test_case_pred<signed_data>(true, less<>{});
        test_case_pred<signed_data>(false, less<unsigned char>{});
        test_case_pred<signed_data>(true, less<signed char>{});
        test_case_pred<signed_data>(char_is_signed, less<char>{});
#ifdef __cpp_char8_t
        test_case_pred<signed_data>(false, less<char8_t>{});
#endif // __cpp_char8_t
        test_case_pred<signed_data>(true, greater<>{});
        test_case_pred<signed_data>(false, greater<unsigned char>{});
        test_case_pred<signed_data>(true, greater<signed char>{});
        test_case_pred<signed_data>(char_is_signed, greater<char>{});
#ifdef __cpp_char8_t
        test_case_pred<signed_data>(false, greater<char8_t>{});
#endif // __cpp_char8_t

        test_case_pred<char_data>(char_is_signed, less<>{});
        test_case_pred<char_data>(false, less<unsigned char>{});
        test_case_pred<char_data>(true, less<signed char>{});
        test_case_pred<char_data>(char_is_signed, less<char>{});
#ifdef __cpp_char8_t
        test_case_pred<char_data>(false, less<char8_t>{});
#endif // __cpp_char8_t
        test_case_pred<char_data>(char_is_signed, greater<>{});
        test_case_pred<char_data>(false, greater<unsigned char>{});
        test_case_pred<char_data>(true, greater<signed char>{});
        test_case_pred<char_data>(char_is_signed, greater<char>{});
#ifdef __cpp_char8_t
        test_case_pred<char_data>(false, greater<char8_t>{});
#endif // __cpp_char8_t

#ifdef __cpp_char8_t
        test_case_pred<char8_data>(false, less<>{});
        test_case_pred<char8_data>(false, less<unsigned char>{});
        test_case_pred<char8_data>(true, less<signed char>{});
        test_case_pred<char8_data>(char_is_signed, less<char>{});
        test_case_pred<char8_data>(false, less<char8_t>{});
        test_case_pred<char8_data>(false, greater<>{});
        test_case_pred<char8_data>(false, greater<unsigned char>{});
        test_case_pred<char8_data>(true, greater<signed char>{});
        test_case_pred<char8_data>(char_is_signed, greater<char>{});
        test_case_pred<char8_data>(false, greater<char8_t>{});
#endif // __cpp_char8_t

        test_changing_pred_does_not_change_length_behavior();
        test_all_numeric_types<test_lex_compare_should_return_correct_numeric_result>();
    }
} // namespace test_lexicographical_compare

namespace test_std_copy {
    void test() {
        {
            array<int, 4> target{{42, 43, 44, 45}};
            array<int, 2> input{{1729, 1730}};
            copy(input.begin(), input.end(), target.begin() + 1);
            assert((target == array<int, 4>{{42, 1729, 1730, 45}}));
        }
        {
            // GH-177: copy different-element-types ranges.
            const array<short, 2> input{10, 20};
            array<int, 2> target;
            copy(input.begin(), input.end(), target.begin());
            assert((target == array<int, 2>{{10, 20}}));
        }
        {
            // GH-177: copy partial-overlapping ranges.
            array<int, 4> input{10, 20, 30, 40};
            copy(input.begin() + 1, input.end(), input.begin());
            assert((input == array<int, 4>{{20, 30, 40, 40}}));
        }
    }
} // namespace test_std_copy

namespace test_std_move {
    void test() {
        array<move_only, 4> target{{{42}, {43}, {44}, {45}}};
        array<move_only, 2> input{{{1729}, {1730}}};
        move(input.begin(), input.end(), target.begin() + 1);
        assert((target == array<move_only, 4>{{{42}, {1729}, {1730}, {45}}}));
    }
} // namespace test_std_move

namespace test_std_copy_backward {
    void test() {
        array<int, 4> target{{42, 43, 44, 45}};
        array<int, 2> input{{1729, 1730}};
        copy_backward(input.begin(), input.end(), target.end());
        assert((target == array<int, 4>{{42, 43, 1729, 1730}}));
    }
} // namespace test_std_copy_backward

namespace test_std_move_backward {
    void test() {
        array<move_only, 4> target{{{42}, {43}, {44}, {45}}};
        array<move_only, 2> input{{{1729}, {1730}}};
        move_backward(input.begin(), input.end(), target.end());
        assert((target == array<move_only, 4>{{{42}, {43}, {1729}, {1730}}}));
    }
} // namespace test_std_move_backward

int main() {
    test_std_equal::test();
    test_lexicographical_compare::test();
    test_std_copy::test();
    test_std_move::test();
    test_std_copy_backward::test();
    test_std_move_backward::test();
}
