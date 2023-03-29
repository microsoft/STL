// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#pragma once
// This header contains all code for the instantiate_algorithms_op_deref* test cases.
// The instantiate_algorithm* tests take too long individually, so must be split into two parts.

#include <instantiate_algorithms.hpp>

// DevDiv-758138 "Several algorithms break with iterators that overload the comma operator."
// DevDiv-758134 "uninitialized_copy and uninitialized_copy_n break with classes that overload operator &."

// ADL will search this namespace for op,().
namespace Meow {

    // This is an evil element type that overloads op&().
    // It supports various operations required by various algorithms.
    struct Evil {
        void operator&() const = delete;

        bool operator<(const Evil&) const {
            return false;
        }
        bool operator==(const Evil&) const {
            return true;
        }

        Evil operator+(const Evil& e) const {
            return e;
        }
        Evil operator-(const Evil& e) const {
            return e;
        }
        Evil operator*(const Evil& e) const {
            return e;
        }

        Evil& operator++() {
            return *this;
        }
    };

    // These iterators point to Evil elements and will activate the op,() below.
    template <bool is_checked>
    struct BasicInIt {
        using iterator_category = std::input_iterator_tag;
        using value_type        = Evil;
        using difference_type   = test_difference_type;
        using pointer           = Evil*;
        using reference         = Evil&;

        BasicInIt(int, int) {}
        BasicInIt(const BasicInIt&)            = default;
        BasicInIt(BasicInIt&&)                 = default;
        BasicInIt& operator=(const BasicInIt&) = default;
        BasicInIt& operator=(BasicInIt&&)      = default;
        ~BasicInIt()                           = default;

        Evil& operator*() const {
            static Evil val{};
            return val;
        }
        Evil* operator->() const {
            return nullptr;
        }
        BasicInIt& operator++() {
            return *this;
        }
        void operator++(int) = delete; // avoid postincrement
        bool operator==(const BasicInIt&) const {
            return true;
        }
        bool operator!=(const BasicInIt&) const {
            return true;
        }

        void _Seek_to(BasicInIt<false>) {}

        BasicInIt<false> _Unwrapped() const {
            return BasicInIt<false>{11, 22};
        }

        static constexpr bool _Unwrap_when_unverified = _ITERATOR_DEBUG_LEVEL == 0;
    };

    template <bool is_checked>
    struct BasicFwdIt {
        using iterator_category = std::forward_iterator_tag;
        using value_type        = Evil;
        using difference_type   = test_difference_type;
        using pointer           = Evil*;
        using reference         = Evil&;

        BasicFwdIt()                             = default;
        BasicFwdIt(const BasicFwdIt&)            = default;
        BasicFwdIt(BasicFwdIt&&)                 = default;
        BasicFwdIt& operator=(const BasicFwdIt&) = default;
        BasicFwdIt& operator=(BasicFwdIt&&)      = default;
        ~BasicFwdIt()                            = default;

        Evil& operator*() const {
            static Evil val{};
            return val;
        }
        Evil* operator->() const {
            return nullptr;
        }
        BasicFwdIt& operator++() {
            return *this;
        }
        void operator++(int) = delete; // avoid postincrement
        bool operator==(const BasicFwdIt&) const {
            return true;
        }
        bool operator!=(const BasicFwdIt&) const {
            return true;
        }

        void _Seek_to(BasicFwdIt<false>) {}

        BasicFwdIt<false> _Unwrapped() const {
            return BasicFwdIt<false>{};
        }

        static constexpr bool _Unwrap_when_unverified = _ITERATOR_DEBUG_LEVEL == 0;
    };

    template <bool is_checked>
    struct BasicBidIt {
        using iterator_category = std::bidirectional_iterator_tag;
        using value_type        = Evil;
        using difference_type   = test_difference_type;
        using pointer           = Evil*;
        using reference         = Evil&;

        BasicBidIt()                             = default;
        BasicBidIt(const BasicBidIt&)            = default;
        BasicBidIt(BasicBidIt&&)                 = default;
        BasicBidIt& operator=(const BasicBidIt&) = default;
        BasicBidIt& operator=(BasicBidIt&&)      = default;
        ~BasicBidIt()                            = default;

        Evil& operator*() const {
            static Evil val{};
            return val;
        }
        Evil* operator->() const {
            return nullptr;
        }
        BasicBidIt& operator++() {
            return *this;
        }
        void operator++(int) = delete; // avoid postincrement
        bool operator==(const BasicBidIt&) const {
            return true;
        }
        bool operator!=(const BasicBidIt&) const {
            return true;
        }

        BasicBidIt& operator--() {
            return *this;
        }
        void operator--(int) = delete; // avoid postdecrement


        void _Seek_to(BasicBidIt<false>) {}

        BasicBidIt<false> _Unwrapped() const {
            return BasicBidIt<false>{};
        }

        static constexpr bool _Unwrap_when_unverified = _ITERATOR_DEBUG_LEVEL == 0;
    };

    template <bool is_checked>
    struct BasicRanIt {
        using iterator_category = std::random_access_iterator_tag;
        using value_type        = Evil;
        using difference_type   = test_difference_type;
        using pointer           = Evil*;
        using reference         = Evil&;

        BasicRanIt()                             = default;
        BasicRanIt(const BasicRanIt&)            = default;
        BasicRanIt(BasicRanIt&&)                 = default;
        BasicRanIt& operator=(const BasicRanIt&) = default;
        BasicRanIt& operator=(BasicRanIt&&)      = default;
        ~BasicRanIt()                            = default;

        Evil& operator*() const {
            static Evil val{};
            return val;
        }
        Evil* operator->() const {
            return nullptr;
        }
        BasicRanIt& operator++() {
            return *this;
        }
        void operator++(int) = delete; // avoid postincrement
        bool operator==(const BasicRanIt&) const {
            return true;
        }
        bool operator!=(const BasicRanIt&) const {
            return true;
        }
        BasicRanIt& operator--() {
            return *this;
        }
        void operator--(int) = delete; // avoid postdecrement

        BasicRanIt& operator+=(difference_type) {
            return *this;
        }
        BasicRanIt& operator-=(difference_type) {
            return *this;
        }
        BasicRanIt operator+(difference_type) const {
            return *this;
        }
        BasicRanIt operator-(difference_type) const {
            return *this;
        }
        difference_type operator-(const BasicRanIt&) const {
            return 0;
        }

        Evil& operator[](difference_type) const = delete; // avoid subscript, N4849 [algorithms.requirements]/8

        bool operator<(const BasicRanIt&) const {
            return true;
        }
        bool operator<=(const BasicRanIt&) const {
            return true;
        }
        bool operator>(const BasicRanIt&) const {
            return true;
        }
        bool operator>=(const BasicRanIt&) const {
            return true;
        }

        void _Seek_to(BasicRanIt<false>) {}

        BasicRanIt<false> _Unwrapped() const {
            return BasicRanIt<false>{};
        }

        static constexpr bool _Unwrap_when_unverified = _ITERATOR_DEBUG_LEVEL == 0;

#if _ITERATOR_DEBUG_LEVEL != 0
        void _Verify_offset(difference_type) const {}
#endif // _ITERATOR_DEBUG_LEVEL != 0
    };

    template <bool is_checked>
    BasicRanIt<is_checked> operator+(test_difference_type, const BasicRanIt<is_checked>& i) {
        return i;
    }

    template <bool is_checked>
    struct BasicOutIt {
        using iterator_category = std::output_iterator_tag;
        using value_type        = void;
        using difference_type   = void;
        using pointer           = void;
        using reference         = void;

        BasicOutIt(int, int) {}
        BasicOutIt(const BasicOutIt&)            = default;
        BasicOutIt(BasicOutIt&&)                 = default;
        BasicOutIt& operator=(const BasicOutIt&) = default;
        BasicOutIt& operator=(BasicOutIt&&)      = default;
        ~BasicOutIt()                            = default;

        Evil& operator*() const {
            static Evil val{};
            return val;
        }
        BasicOutIt& operator++() {
            return *this;
        }
        void operator++(int) = delete; // avoid postincrement


        void _Seek_to(BasicOutIt<false>) {}

        BasicOutIt<false> _Unwrapped() const {
            return BasicOutIt<false>{11, 22};
        }

        static constexpr bool _Unwrap_when_unverified = _ITERATOR_DEBUG_LEVEL == 0;
    };

    // If it's dragged in via ADL, this op,() will absorb anything.
    template <typename T, typename U>
    void operator,(const T&, const U&) = delete;

    // VSO-258601 "std::vector should not rely on operator& in Debug mode"
    // If it's dragged in via ADL, this op&() will absorb anything.
    template <typename T>
    void operator&(const T&) = delete;

} // namespace Meow

void test() {
    // Test with iterators which have overloaded comma operator.
    std_testing::instantiate_std_algorithms_with<Meow::Evil>(Meow::BasicInIt<true>{11, 22}, Meow::BasicFwdIt<true>{},
        Meow::BasicBidIt<true>{}, Meow::BasicRanIt<true>{}, Meow::BasicOutIt<true>{11, 22});
}

// Also test DevDiv-938759 "<type_traits>: is_assignable should tolerate overloaded comma operators [libcxx]".
#define STATIC_ASSERT(...) static_assert(__VA_ARGS__, #__VA_ARGS__)

STATIC_ASSERT(std::is_assignable_v<Meow::Evil&, Meow::Evil>);
STATIC_ASSERT(!std::is_assignable_v<Meow::Evil&, int>);

#undef STATIC_ASSERT

#if _HAS_CXX17
#include <optional>

// Defend against regression of the "," operator in optional::value
void test_optional() {
    using Meow::Evil;
    using std::optional;
    {
        optional<Evil> oe{Evil()};
        (void) oe.value();
    }
    {
        const optional<Evil> oe{Evil()};
        (void) oe.value();
    }
    { (void) optional<Evil>{Evil()}.value(); }
    {
        using T = const optional<Evil>;
        (void) T{Evil()}.value();
    }
}
#endif // _HAS_CXX17
