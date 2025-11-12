// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <iterator>
#include <initializer_list>
#include <type_traits>
#include <valarray>

namespace my {
    
    template<typename T>
    void begin(std::initializer_list<T>);

    template<typename T>
    void end(std::initializer_list<T>);

    template<typename T>
    void empty(std::initializer_list<T>);

    template<typename T>
    void data(std::initializer_list<T>);
}


int main() {
    {
        // Check that free functions in std can't be invoked with braced-initializer-list.
        // If they could be invoked, the following expressions would be ambiguous between std:: and my::
        using namespace std;
        using namespace my;
        static_assert(std::is_same_v<decltype(begin({1, 2, 3})), void>);
        static_assert(std::is_same_v<decltype(end({1, 2, 3})), void>);
        static_assert(std::is_same_v<decltype(empty({1, 2, 3})), void>);
        static_assert(std::is_same_v<decltype(data({1, 2, 3})), void>);
    }
    
    { 
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
    }

    {
        // Check that free functions in std can be invoked on std::valarray
        std::valarray<int> v{1};

        using namespace std;
        (void) begin(v);
        (void) cbegin(v); // Did not compile before P3016R6
        (void) end(v);
        (void) cend(v); // Did not compile before P3016R6
        (void) size(v);
        // There are no members 'empty' and 'data' of valarray
        // (void) empty(il);
        // (void) data(il);
    }
}
