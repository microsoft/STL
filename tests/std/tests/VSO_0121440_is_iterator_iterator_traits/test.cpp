// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <array>
#include <cstddef>
#include <iterator>
#include <type_traits>
#include <utility>
#include <vector>

using namespace std;

#define STATIC_ASSERT(...) static_assert(__VA_ARGS__, #__VA_ARGS__)

class int_iterator // models InputIterator
{
public:
    explicit int_iterator(int* start) : m_ptr(start) {}

    int_iterator(const int_iterator&)            = default;
    int_iterator& operator=(const int_iterator&) = default;

    int_iterator& operator++() { // prefix
        ++m_ptr;
        return *this;
    }

    void operator++(int) = delete; // avoid postincrement

    int& operator*() const {
        return *m_ptr;
    }

    bool operator==(const int_iterator& rhs) const {
        return m_ptr == rhs.m_ptr;
    }

    bool operator!=(const int_iterator& rhs) const {
        return m_ptr != rhs.m_ptr;
    }

private:
    int* m_ptr;
};

namespace std {
    template <>
    struct iterator_traits<int_iterator> {
        using iterator_category = input_iterator_tag;
        using difference_type   = ptrdiff_t;
        using value_type        = int;
        using pointer           = int*;
        using reference         = int&;
    };
} // namespace std

// Also test that forming iterator_traits<void *> does not cause a hard, not SFINAE-able, error
template <class Ty, enable_if_t<is_same_v<Ty, void*>, int> = 0>
constexpr bool iterator_traits_void_ptr_sfinae_test() {
    return true;
}

template <class Ty, class = typename iterator_traits<Ty>::iterator_category>
constexpr bool iterator_traits_void_ptr_sfinae_test() {
    return false;
}

STATIC_ASSERT(iterator_traits_void_ptr_sfinae_test<void*>());
STATIC_ASSERT(!iterator_traits_void_ptr_sfinae_test<int*>());

int main() {
    array<int, 5> arr{{1, 2, 3, 4, 5}};

    int_iterator begin_arr(arr.data());
    int_iterator end_arr(arr.data() + arr.size());

    // the following will not compile if int_iterator is determined to not be an InputIterator:
    vector<int> vec(begin_arr, end_arr);
}

// DevDiv-557214 "std::forward_iterator_tag derives from std::output_iterator_tag"
STATIC_ASSERT(is_base_of_v<input_iterator_tag, forward_iterator_tag>);
STATIC_ASSERT(is_base_of_v<forward_iterator_tag, bidirectional_iterator_tag>);
STATIC_ASSERT(is_base_of_v<bidirectional_iterator_tag, random_access_iterator_tag>);

STATIC_ASSERT(!is_base_of_v<output_iterator_tag, input_iterator_tag>);
STATIC_ASSERT(!is_base_of_v<output_iterator_tag, forward_iterator_tag>);
STATIC_ASSERT(!is_base_of_v<output_iterator_tag, bidirectional_iterator_tag>);
STATIC_ASSERT(!is_base_of_v<output_iterator_tag, random_access_iterator_tag>);
