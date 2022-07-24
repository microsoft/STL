// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <cassert>
#include <memory>
#include <string>
#include <type_traits>
#include <vector>

#include <test_death.hpp>

template <class, template <class...> class>
inline constexpr bool is_specialization = false;
template <class... Args, template <class...> class T>
inline constexpr bool is_specialization<T<Args...>, T> = true;

template <bool IsConst, class C>
using maybe_const_iter = std::conditional_t<IsConst, typename C::const_iterator, typename C::iterator>;

template <class C, bool IsConst>
void test_positive_cases() {
    // to_address should accept value-initialized iterators:
    assert(std::to_address(maybe_const_iter<IsConst, C>{}) == nullptr); // nullptr not guaranteed, but likely true

    C c;

    {
        // begin and end iterator for empty container:
        maybe_const_iter<IsConst, C> const first = c.begin();
        maybe_const_iter<IsConst, C> const last  = c.end();
        // Ditto "nullptr not guaranteed":
        assert(((std::to_address(first) == nullptr) == !is_specialization<C, std::basic_string>) );
        assert(std::to_address(last) == std::to_address(first));
    }

    c = C{typename C::value_type{}};

    {
        // begin and end iterator for non-empty container:
        maybe_const_iter<IsConst, C> const first = c.begin();
        maybe_const_iter<IsConst, C> const last  = c.end();
        assert(std::to_address(first) != nullptr);
        assert(std::to_address(last) == 1 + std::to_address(first));
    }

    c.pop_back();

    {
        // empty again for completeness:
        maybe_const_iter<IsConst, C> const first = c.begin();
        maybe_const_iter<IsConst, C> const last  = c.end();
        assert(std::to_address(last) == std::to_address(first));
    }
}

// Negative tests:
template <class C, bool IsConst>
void test_invalidated_by_pop_back() {
    using T = typename C::value_type;
    C c     = {T{}, {}, {}};

    maybe_const_iter<IsConst, C> const pos = c.end() - 1;

    c.pop_back();

    (void) std::to_address(pos); // Cannot convert invalidated iterator to pointer
}

template <class C, bool IsConst>
void test_invalidated_by_push_back() {
    using T = typename C::value_type;
    C c;
    c.resize(1024, T{});
    c.resize(512);

    maybe_const_iter<IsConst, C> const pos = c.end();

    c.push_back(T{});

    (void) std::to_address(pos); // Cannot convert invalidated iterator to pointer
}

template <class C, bool IsConst>
void test_invalidated_by_reallocation() {
    using T = typename C::value_type;
    C c     = {T{}, {}, {}};

    maybe_const_iter<IsConst, C> const pos = c.begin();

    c.resize(8 * 1024);

    (void) std::to_address(pos); // Cannot convert invalidated iterator to pointer
}

template <class C, bool IsConst>
void test_out_of_range() {
    using T = typename C::value_type;
    C c     = {T{}, {}, {}};

    maybe_const_iter<IsConst, C> const last = c.end();

    c.pop_back();

    (void) std::to_address(last); // Cannot convert out-of-range iterator to pointer
}

int main(int argc, char* argv[]) {
    std_testing::death_test_executive exec([] {
        test_positive_cases<std::string, true>();
        test_positive_cases<std::string, false>();
        test_positive_cases<std::vector<int>, true>();
        test_positive_cases<std::vector<int>, false>();
    });

    // clang-format off
#if _ITERATOR_DEBUG_LEVEL != 0
    exec.add_death_tests({
        // Tests for only vector: basic_string doesn't invalidate on element creation/destruction because POD.
#if _ITERATOR_DEBUG_LEVEL == 2
        test_invalidated_by_pop_back<std::vector<int>, false>,
        test_invalidated_by_pop_back<std::vector<int>, true>,
        test_invalidated_by_push_back<std::vector<int>, false>,
        test_invalidated_by_push_back<std::vector<int>, true>,
#endif // _ITERATOR_DEBUG_LEVEL == 2

        // Tests for both string and vector
        test_invalidated_by_reallocation<std::string, false>,
        test_invalidated_by_reallocation<std::string, true>,
        test_invalidated_by_reallocation<std::vector<int>, false>,
        test_invalidated_by_reallocation<std::vector<int>, true>,
        test_out_of_range<std::string, false>,
        test_out_of_range<std::string, true>,
        test_out_of_range<std::vector<int>, false>,
        test_out_of_range<std::vector<int>, true>,
    });
#endif // _ITERATOR_DEBUG_LEVEL != 0
    // clang-format on

    return exec.run(argc, argv);
}
