// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <algorithm>
#include <iterator>

class contiguous_iterator {
public:
    using value_type        = int;
    using difference_type   = int;
    using element_type      = int;
    using iterator_category = std::contiguous_iterator_tag;
    int* operator->() const;
    int& operator*() const;
    int& operator[](int) const;
    contiguous_iterator& operator++();
    contiguous_iterator operator++(int);
    contiguous_iterator& operator--();
    contiguous_iterator operator--(int);
    contiguous_iterator& operator+=(int);
    contiguous_iterator& operator-=(int);
    friend auto operator<=>(contiguous_iterator, contiguous_iterator) = default;
    friend int operator-(contiguous_iterator, contiguous_iterator);
    friend contiguous_iterator operator+(contiguous_iterator, int);
    friend contiguous_iterator operator-(contiguous_iterator, int);
    friend contiguous_iterator operator+(int, contiguous_iterator);
};

// GH-3663 <algorithm>/<iterator>: contiguous iterators with non-ptrdiff_t difference types
void test() {
    int* p = nullptr;
    std::copy(p, p, contiguous_iterator{});
    std::sort(contiguous_iterator{}, contiguous_iterator{});
}
