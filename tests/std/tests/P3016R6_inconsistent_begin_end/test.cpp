// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <initializer_list>
#include <iterator>
#include <type_traits>
#include <valarray>

#define STATIC_ASSERT(...) static_assert(__VA_ARGS__, #__VA_ARGS__)

namespace my {
    template <class T>
    void begin(std::initializer_list<T>);

    template <class T>
    void end(std::initializer_list<T>);

    template <class T>
    void empty(std::initializer_list<T>);

    template <class T>
    void data(std::initializer_list<T>);
} // namespace my

_CONSTEXPR17 bool test_initializer_list() {
    // Check that free functions in std still can be invoked on std::initializer_list
    std::initializer_list<int> il = {1, 2, 3};

    using namespace std;
    (void) begin(il);
    (void) cbegin(il);
    (void) end(il);
    (void) cend(il);
    (void) size(il);
    (void) empty(il);
    (void) data(il);

    return true;
}

// test_valarray() is not constexpr because valarray is not a literal type
bool test_valarray() {
    // Check that free functions in std can be invoked on std::valarray
    std::valarray<int> v{1};

    using namespace std;
    (void) begin(v);
    (void) cbegin(v); // Did not compile before P3016R6
    (void) end(v);
    (void) cend(v); // Did not compile before P3016R6
    (void) size(v);
    // There are no members 'empty' and 'data' of valarray

    return true;
}

constexpr bool test_braced_init_list_iteration() {
    int sum = 0;
    for (int n : {1, 2, 3, 4}) {
        sum += n;
    }
    return sum == 10;
}

int main() {
    // Check that free functions in std can't be invoked with braced-initializer-list.
    // If they could be invoked, the following expressions would be ambiguous between std:: and my::
    using namespace std;
    using namespace my;
    STATIC_ASSERT(std::is_same_v<decltype(begin({1, 2, 3})), void>);
    STATIC_ASSERT(std::is_same_v<decltype(end({1, 2, 3})), void>);
    STATIC_ASSERT(std::is_same_v<decltype(empty({1, 2, 3})), void>);
    STATIC_ASSERT(std::is_same_v<decltype(data({1, 2, 3})), void>);

    test_initializer_list();
    test_valarray();
    test_braced_init_list_iteration();

#if _HAS_CXX17
    STATIC_ASSERT(test_initializer_list());
#endif // _HAS_CXX17
    STATIC_ASSERT(test_braced_init_list_iteration());
}
