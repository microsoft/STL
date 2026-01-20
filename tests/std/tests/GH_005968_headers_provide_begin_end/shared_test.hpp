// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

// Intentionally avoid including anything, relevant headers are included in .cpp files.
// Test requirements of N5032 [iterator.range]/1.

namespace detail {

    // file test_iterator.cpp uses a non-std container and thus we can't rely on ADL to find begin/end etc.
    using namespace std;

    // Define minimal metaprogramming tools, avoid including anything

#define DEFINE_CONDITIONAL_CALLER_OF(member_name)                                                              \
    template <typename T, typename = void>                                                                     \
    struct conditional_caller_of_##member_name {                                                               \
        constexpr void operator()(T& t) {                                                                      \
            (void) t;                                                                                          \
        }                                                                                                      \
    };                                                                                                         \
                                                                                                               \
    template <typename T>                                                                                      \
    struct conditional_caller_of_##member_name<T&, decltype((void) static_cast<T*>(nullptr)->member_name())> { \
        constexpr auto operator()(T& t) {                                                                      \
            return member_name(t);                                                                             \
        }                                                                                                      \
    };

#define CONDITIONALLY_CALL(c, member_name) conditional_caller_of_##member_name<decltype(c)>{}(c)

    DEFINE_CONDITIONAL_CALLER_OF(rbegin);
    DEFINE_CONDITIONAL_CALLER_OF(rend);
    DEFINE_CONDITIONAL_CALLER_OF(crbegin);
    DEFINE_CONDITIONAL_CALLER_OF(crend);
    DEFINE_CONDITIONAL_CALLER_OF(size);
    DEFINE_CONDITIONAL_CALLER_OF(ssize);
    DEFINE_CONDITIONAL_CALLER_OF(empty);
    DEFINE_CONDITIONAL_CALLER_OF(data);


    template <typename C>
    void test_free_container_functions(C& c) {
        (void) begin(c);
        (void) end(c);
        (void) cbegin(c);
        (void) cend(c);
        CONDITIONALLY_CALL(c, rbegin); // missing e.g. for forward_list
        CONDITIONALLY_CALL(c, rend); // missing e.g. for forward_list
        CONDITIONALLY_CALL(c, crbegin); // missing e.g. for forward_list
        CONDITIONALLY_CALL(c, crend); // missing e.g. for forward_list
        CONDITIONALLY_CALL(c, size); // missing e.g. for optional
        CONDITIONALLY_CALL(c, ssize); // missing e.g. for optional
        CONDITIONALLY_CALL(c, empty); // missing e.g. for valarray
        CONDITIONALLY_CALL(c, data); // missing e.g. for valarray
    }

    inline void test_free_array_functions() {
        int a[]{1, 2, 3};

        (void) begin(a);
        (void) end(a);
        (void) cbegin(a);
        (void) cend(a);
        (void) rbegin(a);
        (void) rend(a);
        (void) crbegin(a);
        (void) crend(a);
        (void) size(a);
#if _HAS_CXX20
        (void) ssize(a);
#endif
        (void) empty(a);
        (void) data(a);
    }
} // namespace detail

template <typename C>
void shared_test(C& c) {
    detail::test_free_container_functions(c);
    // as_const from <utility> not required to be available
    detail::test_free_container_functions(const_cast<const C&>(c));

    detail::test_free_array_functions();
}
